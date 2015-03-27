package org.libcinder.hardware;

import org.libcinder.app.Platform;

import android.annotation.TargetApi;
import android.content.Context;
import android.graphics.Point;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CameraMetadata;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.Image;
import android.media.ImageReader;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.util.Size;
import android.view.Surface;

import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.concurrent.Semaphore;
import java.util.concurrent.locks.ReentrantLock;

/** \class CinderCamera2
 *
 */
@TargetApi(21)
public class CinderCamera2 {
    private SurfaceTexture mDummyTexture = null;
    private Surface mDummySurface = null;

    private CameraManager mCameraManager = null;

    private Semaphore mCameraOpenCloseLock = new Semaphore(1);

    private String mFrontCameraId = null;
    private String mBackCameraId = null;

    private String mActiveCameraId = null;
    private CameraDevice mCamera = null;

    private int mWidth = 0;
    private int mHeight = 0;
    private byte[] mPixels = null;
    private ReentrantLock mMutex;

    private HandlerThread mCameraHandlerThread;
    private Handler mCameraHandler;

    private static final int sPreviewImageFormat = ImageFormat.YUV_420_888;
    private ImageReader mPreviewImageReader = null;

    private CaptureRequest mPreviewRequest = null;
    private CaptureRequest.Builder mPreviewRequestBuilder = null;
    private CameraCaptureSession mPreviewCaptureSession = null;

    private final ImageReader.OnImageAvailableListener mOnImageAvailableListener = new ImageReader.OnImageAvailableListener() {
        @Override
        public void onImageAvailable(ImageReader reader) {
            privateLockPixels();
            try {
                Image image = reader.acquireLatestImage();
                if((null != image) && (image.getPlanes().length > 0)) {
                    ByteBuffer buffer = image.getPlanes()[0].getBuffer();

                    if(null == mPixels) {
                        mPixels = new byte[buffer.remaining()];
                    }
                    buffer.get(mPixels);

                    image.close();
                }
            }
            finally {
                privateUnlockPixels();
            }
        }
    };

    private final CameraDevice.StateCallback mStateCallback = new CameraDevice.StateCallback() {
        @Override
        public void onOpened(CameraDevice camera) {
            mCameraOpenCloseLock.release();
            mCamera = camera;
            startPreview();
        }

        @Override
        public void onDisconnected(CameraDevice camera) {
            mCameraOpenCloseLock.release();
            camera.close();
            mCamera = null;
        }

        @Override
        public void onError(CameraDevice camera, int error) {
            mCameraOpenCloseLock.release();
            camera.close();
            mCamera = null;
        }
    };

    /** \func CinderCamera2
     *
     */
    public CinderCamera2() {
        mCameraManager = (CameraManager)Platform.getActivity().getSystemService(Context.CAMERA_SERVICE);

        try {
            for (String cameraId : mCameraManager.getCameraIdList()) {
                CameraCharacteristics info = mCameraManager.getCameraCharacteristics(cameraId);

                int facing = info.get(CameraCharacteristics.LENS_FACING);
                if(CameraCharacteristics.LENS_FACING_FRONT == facing) {
                    mFrontCameraId = cameraId;
                }
                else if(CameraCharacteristics.LENS_FACING_BACK == facing) {
                    mBackCameraId = cameraId;
                }
            }
        }
        catch(Exception e ) {
            throw new UnsupportedOperationException("Failed getting camera: " + e);
        }

        mActiveCameraId = (null != mBackCameraId) ? mBackCameraId : ((null != mFrontCameraId) ? mFrontCameraId : null);

        // Get preview size
        if(null != mActiveCameraId) {
            Size previewSize = getOptimalPreviewSize(mActiveCameraId);
            if (null == previewSize) {
                throw new UnsupportedOperationException("couldn't get preview size for Camera " + mActiveCameraId);
            }

            mWidth = previewSize.getWidth();
            mHeight = previewSize.getHeight();
        }

        mMutex = new ReentrantLock();

//        Log.i(Platform.TAG, "Front camera: " + mFrontCameraId);
//        Log.i(Platform.TAG, "Back camera: " + mBackCameraId);
//
//        try {
//            if(null != mFrontCameraId) {
//                Log.i(Platform.TAG, "Front camera preview size: " + getOptimalPreviewSize(mFrontCameraId));
//            }
//
//            if(null != mBackCameraId) {
//                Log.i(Platform.TAG, "Back camera preview size: " + getOptimalPreviewSize(mBackCameraId));
//            }
//        }
//        catch(Exception e) {
//        }
    }

    /** \func getOptimalPreviewSize
     *
     */
    private Size getOptimalPreviewSize(String cameraId) {
        Size result = null;
        try {
            // Get the raw display size
            Point displaySize = Platform.getDisplaySize();
            int rawDisplayWidth = displaySize.x;
            int rawDisplayHeight = displaySize.y;
            int rawDisplayArea = rawDisplayWidth*rawDisplayHeight;
            float rawAspectRatio = (float)rawDisplayWidth/(float)rawDisplayHeight;
            if(rawAspectRatio < 1.0f) {
                int t = rawDisplayWidth;
                rawDisplayWidth = rawDisplayHeight;
                rawDisplayHeight = t;
                rawAspectRatio = (float)rawDisplayWidth/(float)rawDisplayHeight;
            }

            // Get a list of output sizes and sort it
            CameraCharacteristics info = mCameraManager.getCameraCharacteristics(cameraId);
            StreamConfigurationMap scm = info.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
            List<Size> sizes = Arrays.asList(scm.getOutputSizes(SurfaceTexture.class));
            Collections.sort(sizes, new Comparator<Size>() {
                @Override
                public int compare(Size a, Size b) {
                    int val = (a.getWidth()*a.getHeight()) - (b.getWidth()*b.getHeight());
                    return (val < 0) ? -1 : ((val > 0) ? 1 : 0);
                }
            });

            // Try to match up greatest area and aspect ratio
            for(Size size : sizes) {
                int width = size.getWidth();
                int height = size.getHeight();
                int area = width*height;
                float aspectRatio = (float)width/(float)height;
                if(aspectRatio < 1.0) {
                    int t = width;
                    width = height;
                    height = t;
                    aspectRatio = (float)width/(float)height;
                }

                float aspectRatioDiff = Math.abs(rawAspectRatio - aspectRatio);
                if((rawDisplayArea >= area) && (aspectRatioDiff < 0.001f)) {
                    result = size;
                }
            }

            // If we didn't find a size - use just greatest area
            if(null == result) {
                for(Size size : sizes) {
                    int width = size.getWidth();
                    int height = size.getHeight();
                    int area = width*height;

                    if(rawDisplayArea >= area) {
                        result = size;
                    }
                }
            }
        }
        catch(Exception e) {
            Log.e(Platform.TAG, "getOptimalPreviewSize failed: " + e);
        }
        return result;
    }

    /** \func startPreview
     *
     */
    private void startPreview() {
        // Start the preview
        try {
            // Get preview size
            Size previewSize = getOptimalPreviewSize(mCamera.getId());
            if(null == previewSize) {
                throw new UnsupportedOperationException("couldn't get preview size for Camera " + mCamera.getId());
            }

            // Create ImageReader
            mPreviewImageReader = ImageReader.newInstance(previewSize.getWidth(), previewSize.getHeight(), sPreviewImageFormat, 2);
            mPreviewImageReader.setOnImageAvailableListener(mOnImageAvailableListener, mCameraHandler);

            // Create SurfaceTexture
            mDummyTexture = new SurfaceTexture(0);
            mDummyTexture.setDefaultBufferSize(previewSize.getWidth(), previewSize.getHeight());

            // Create Surface
            mDummySurface = new Surface(mDummyTexture);

            // Create CaptureRequest.Builder
            mPreviewRequestBuilder = mCamera.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
            mPreviewRequestBuilder.addTarget(mDummySurface);
            mPreviewRequestBuilder.addTarget(mPreviewImageReader.getSurface());

            // Create CameraCaptureSession
            mCamera.createCaptureSession(
                    Arrays.asList(mDummySurface, mPreviewImageReader.getSurface()),
                    new CameraCaptureSession.StateCallback() {
                        @Override
                        public void onConfigured(CameraCaptureSession session) {
                            if(null == mCamera) {
                                return;
                            }

                            mPreviewCaptureSession = session;
                            //mPreviewRequestBuilder.set(CaptureRequest.CONTROL_MODE, CameraMetadata.CONTROL_MODE_AUTO);
                            //mPreviewHandlerThread = new HandlerThread("preview-handler-thread");
                            //mPreviewHandlerThread.start();
                            //mPreviewHandler = new Handler(mPreviewHandlerThread.getLooper());

                            try {
                                mPreviewRequest = mPreviewRequestBuilder.build();
                                mPreviewCaptureSession.setRepeatingRequest(mPreviewRequest, null, mCameraHandler);
                            } catch (Exception e) {
                                Log.e(Platform.TAG, "CameraCaptureSession.setRepeatingRequest failed: " + e);
                            }
                        }

                        @Override
                        public void onConfigureFailed(CameraCaptureSession session) {
                        }
                    },
                    null
            );
        }
        catch(Exception e ) {
            Log.e(Platform.TAG, "CameraDevice.StateCallback.onOpened failed: " + e);
        }
    }

    /** \func stopPreview
     *
     */
    private void stopPreview() {
        if(null != mCamera) {
            mCamera.close();
            mCamera = null;
        }

        if(null != mPreviewImageReader) {
            mPreviewImageReader.close();
            mPreviewImageReader = null;
        }

//        if(null != mPreviewHandlerThread) {
//            try {
//                if(null != mPreviewCaptureSession) {
//                    mPreviewCaptureSession.stopRepeating();
//                }
//                mPreviewHandlerThread.quitSafely();
//                mPreviewHandlerThread.join();;
//                mPreviewHandlerThread = null;
//                mPreviewHandler = null;
//            }
//            catch(Exception e) {
//                Log.e(Platform.TAG, "Preview handler stop failed: " + e);
//            }
//        }
    }

    private void privateLockPixels() {
        //Log.i(Platform.TAG, "privateLockPixels");
        mMutex.lock();
    }

    private void privateUnlockPixels() {
        mMutex.unlock();;
        //Log.i(Platform.TAG, "privateUnlockPixels");
    }

    /** \func startCamera
     *
     */
    public void startCamera() {
        // Bail if we don't have a valid camera id or mCamera isn't null
        if ((null == mActiveCameraId) || (null != mCamera)) {
            return;
        }

        try {
            mCameraHandlerThread = new HandlerThread("camera-handler-thread");
            mCameraHandlerThread.start();
            mCameraHandler = new Handler(mCameraHandlerThread.getLooper());
            mCameraManager.openCamera(mActiveCameraId, mStateCallback, mCameraHandler);
        }
        catch(Exception e ) {
            Log.e(Platform.TAG, "CinderCamera2.startCamera failed: " + e);
        }
    }

    /** \func stopCamera
     *
     */
    public void stopCamera() {
        stopPreview();

        if(null != mCameraHandlerThread) {
            try {
                mCameraHandlerThread.quitSafely();
                mCameraHandlerThread.join();
                mCameraHandlerThread = null;
                mCameraHandler = null;
            }
            catch(Exception e ) {
                Log.e(Platform.TAG, "Camera handler thread stop failed: " + e);
            }
        }
    }

    // -------------------------------------------------------------------------

    private static CinderCamera2 sCamera = null;

    public static boolean initialize() {
        if(null == sCamera) {
            sCamera = new CinderCamera2();
        }

        return (null != sCamera.mFrontCameraId || null != sCamera.mBackCameraId);
    }

    public static boolean hasFrontCamera() {
        return (null != sCamera) && (null != sCamera.mFrontCameraId);
    }

    public static boolean hasBackCamera() {
        return (null != sCamera) && (null != sCamera.mBackCameraId);
    }

    public static void startCapture() {
        if(null == sCamera) {
            return;
        }

        sCamera.startCamera();
    }

    public static void stopCapture() {
        if(null == sCamera) {
            return;
        }

        sCamera.stopCamera();
    }

    public static byte[] lockPixels() {
        if(null == sCamera) {
            return null;
        }

        sCamera.privateLockPixels();
        return sCamera.mPixels;
    }

    public static void unlockPixels() {
        if(null == sCamera) {
            return;
        }

        sCamera.privateUnlockPixels();
    }

    public static int getWidth() {
        return (null != sCamera) ? sCamera.mWidth : 0;
    }

    public static int getHeight() {
        return (null != sCamera) ? sCamera.mHeight : 0;
    }
}
