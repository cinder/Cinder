package org.libcinder.hardware;

import org.libcinder.Cinder;
import org.libcinder.app.CinderNativeActivity;

import android.annotation.TargetApi;
import android.content.Context;
import android.graphics.Point;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
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

/** \class CameraV2
 *
 */
@TargetApi(21)
public class CameraV2 extends org.libcinder.hardware.Camera {

    private SurfaceTexture mDummyTexture = null;
    private Surface mDummySurface = null;

    private CameraManager mCameraManager = null;

    private Semaphore mCameraOpenCloseLock = new Semaphore(1);

    private CameraDevice mCamera = null;

    private String mBackDeviceId = null;
    private String mFrontDeviceId = null;
    private String mActiveDeviceId = null;

    private ReentrantLock mPixelsMutex = null;

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
            lockPixels();
            try {
                Image image = reader.acquireLatestImage();
                if((null != image) && (image.getPlanes().length > 0)) {
                    Log.i(Cinder.TAG, "Number of planes:" + image.getPlanes().length);

                    ByteBuffer buf0 = image.getPlanes()[0].getBuffer();
                    ByteBuffer buf1 = image.getPlanes()[1].getBuffer();
                    ByteBuffer buf2 = image.getPlanes()[2].getBuffer();
                    Log.i(Cinder.TAG, "...plane[0] size:" + buf0.remaining());
                    Log.i(Cinder.TAG, "...plane[1] size:" + buf1.remaining());
                    Log.i(Cinder.TAG, "...plane[2] size:" + buf2.remaining());

                    ByteBuffer buffer = image.getPlanes()[0].getBuffer();

                    if(null == mPixels) {
                        mPixels = new byte[buffer.remaining()];
                    }
                    buffer.get(mPixels);

                    image.close();
                }
            }
            finally {
                unlockPixels();
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
    public CameraV2() {
    }

    @Override
    protected void initialize() {
        mCameraManager = (CameraManager)getActivity().getSystemService(Context.CAMERA_SERVICE);

        try {
            for (String cameraId : mCameraManager.getCameraIdList()) {
                CameraCharacteristics info = mCameraManager.getCameraCharacteristics(cameraId);

                int facing = info.get(CameraCharacteristics.LENS_FACING);
                if(CameraCharacteristics.LENS_FACING_FRONT == facing) {
                    mFrontDeviceId = cameraId;
                }
                else if(CameraCharacteristics.LENS_FACING_BACK == facing) {
                    mBackDeviceId = cameraId;
                }
            }
        }
        catch(Exception e ) {
            throw new UnsupportedOperationException("Failed getting camera: " + e);
        }

        mActiveDeviceId = (null != mBackDeviceId) ? mBackDeviceId : ((null != mFrontDeviceId) ? mFrontDeviceId : null);

        // Get preview size
        if(null != mActiveDeviceId) {
            Size previewSize = getOptimalPreviewSize(mActiveDeviceId);
            if (null == previewSize) {
                throw new UnsupportedOperationException("couldn't get preview size for Camera " + mActiveDeviceId);
            }

            mWidth = previewSize.getWidth();
            mHeight = previewSize.getHeight();
        }

        mPixelsMutex = new ReentrantLock();
    }

    /** \func getOptimalPreviewSize
     *
     */
    private Size getOptimalPreviewSize(String cameraId) {
        Size result = null;
        try {
            // Get the raw display size
            Point displaySize = CinderNativeActivity.getActivity().getDefaultDisplaySize();
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
                if(aspectRatio < 1.0f) {
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
            Log.e(Cinder.TAG, "getOptimalPreviewSize failed: " + e);
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
                        try {
                            mPreviewRequest = mPreviewRequestBuilder.build();
                            mPreviewCaptureSession.setRepeatingRequest(mPreviewRequest, null, mCameraHandler);
                        } catch (Exception e) {
                            Log.e(Cinder.TAG, "CameraCaptureSession.setRepeatingRequest failed: " + e);
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
            Log.e(Cinder.TAG, "CameraDevice.StateCallback.onOpened failed: " + e);
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
    }

//    private void privateLockPixels() {
//        //Log.i(Platform.TAG, "privateLockPixels");
//        mPixelsMutex.lock();
//    }
//
//    private void privateUnlockPixels() {
//        mPixelsMutex.unlock();;
//        //Log.i(Platform.TAG, "privateUnlockPixels");
//    }

    /** \func startDevice
     *
     */
    private void startDevice() {
        // Bail if we don't have a valid camera id or mCamera isn't null
        if ((null == mActiveDeviceId) || (null != mCamera)) {
            return;
        }

        try {
            mCameraHandlerThread = new HandlerThread("camera-handler-thread");
            mCameraHandlerThread.start();
            mCameraHandler = new Handler(mCameraHandlerThread.getLooper());
            mCameraManager.openCamera(mActiveDeviceId, mStateCallback, mCameraHandler);
        }
        catch(Exception e ) {
            Log.e(Cinder.TAG, "CinderCamera2.startDevice failed: " + e);
        }
    }

    /** \func stopDevice
     *
     */
    private void stopDevice() {
        stopPreview();

        if(null != mCameraHandlerThread) {
            try {
                mCameraHandlerThread.quitSafely();
                mCameraHandlerThread.join();
                mCameraHandlerThread = null;
                mCameraHandler = null;
            }
            catch(Exception e ) {
                Log.e(Cinder.TAG, "Camera handler thread stop failed: " + e);
            }
        }
    }

    /** startBackDevice
     *
     */
    private void startBackDevice() {
        if(mBackDeviceId.equals(mActiveDeviceId)) {
            return;
        }

        stopDevice();

        mActiveDeviceId = mBackDeviceId;
        startDevice();
    }

    /** startFrontDevice
     *
     */
    private void startFrontDevice() {
        if(mFrontDeviceId.equals(mActiveDeviceId)) {
            return;
        }

        stopDevice();

        mActiveDeviceId = mFrontDeviceId;
        startDevice();
    }

    /** isBackCameraAvailable
     *
     */
    @Override
    public boolean isBackCameraAvailable() {
        return null != mBackDeviceId;
    }

    /** isFrontCameraAvailable
     *
     */
    @Override
    public boolean isFrontCameraAvailable() {
        return null != mFrontDeviceId;
    }

    /** startCapture
     *
     */
    @Override
    public void startCapture() {
        if(isBackCameraAvailable()) {
            startBackDevice();
        }
        else if(isFrontCameraAvailable()) {
            startFrontDevice();
        }
    }

    /** stopCapture
     *
     */
    @Override
    public void stopCapture() {
        stopDevice();
    }

    /** switchToBackCamera
     *
     */
    @Override
    public void switchToBackCamera() {
        if(mBackDeviceId == mActiveDeviceId) {
            return;
        }

        startBackDevice();
    }

    /** switchToFrontCamera
     *
     */
    @Override
    public void switchToFrontCamera() {
        if(mFrontDeviceId == mActiveDeviceId) {
            return;
        }

        startFrontDevice();
    }

    /** lockPixels
     *
     */
    @Override
    public byte[] lockPixels() {
        mPixelsMutex.lock();
        return mPixels;
    }

    /** unlockPixels
     *
     */
    @Override
    public void unlockPixels() {
        mPixelsMutex.unlock();
    }


/*
    // =============================================================================================
    // Static Methods for C++
    // =============================================================================================

    private static CameraV2 sCamera = null;

    public static boolean initialize() {
        if(null == sCamera) {
            sCamera = new CameraV2();
        }

        return (null != sCamera.mFrontDeviceId || null != sCamera.mBackDeviceId);
    }

    public static boolean hasFrontCamera() {
        return (null != sCamera) && (null != sCamera.mFrontDeviceId);
    }

    public static boolean hasBackCamera() {
        return (null != sCamera) && (null != sCamera.mBackDeviceId);
    }

    public static void startCapture() {
        if(null == sCamera) {
            return;
        }

        sCamera.startDevice();
    }

    public static void stopCapture() {
        if(null == sCamera) {
            return;
        }

        sCamera.stopDevice();
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

    public static void takePicture() {
        if(null == sCamera) {
            return;
        }

    }
*/
}
