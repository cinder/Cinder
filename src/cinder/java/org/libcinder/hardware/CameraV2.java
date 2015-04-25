package org.libcinder.hardware;

import org.libcinder.app.ModulesFragment;

import android.annotation.TargetApi;
import android.content.Context;
import android.graphics.Matrix;
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

    private static final String TAG = "CameraV2";

    private CameraManager mCameraManager = null;

    private Semaphore mCameraOpenCloseLock = new Semaphore(1);

    private CameraDevice mCamera = null;

    private ReentrantLock mPixelsMutex = null;

    private HandlerThread mCameraHandlerThread;
    private Handler mCameraHandler;

    private int mPreviewImageFormat = ImageFormat.YUV_420_888;
    private ImageReader mPreviewImageReader = null;
    private Surface mImageSurface = null;
    private Surface mDummySurface = null;

    private CaptureRequest mPreviewRequest = null;
    private CaptureRequest.Builder mPreviewRequestBuilder = null;
    private CameraCaptureSession mPreviewCaptureSession = null;

    private final ImageReader.OnImageAvailableListener mOnImageAvailableListener = new ImageReader.OnImageAvailableListener() {
        @Override
        public void onImageAvailable(ImageReader reader) {
            lockPixels();
            try {
                Image image = reader.acquireLatestImage();
                if ((null != image) && (image.getPlanes().length > 0)) {
                    //Log.i(TAG, "Number of planes:" + image.getPlanes().length);

                    ByteBuffer buf0 = image.getPlanes()[0].getBuffer();
                    ByteBuffer buf1 = image.getPlanes()[1].getBuffer();
                    ByteBuffer buf2 = image.getPlanes()[2].getBuffer();
                    //Log.i(TAG, "...plane[0] size:" + buf0.remaining());
                    //Log.i(TAG, "...plane[1] size:" + buf1.remaining());
                    //Log.i(TAG, "...plane[2] size:" + buf2.remaining());

                    ByteBuffer buffer = image.getPlanes()[0].getBuffer();

                    if ((null == mPixels) || (mPixels.length != buffer.remaining())) {
                        mPixels = new byte[buffer.remaining()];
                    }
                    buffer.get(mPixels);

                    image.close();
                }
            } finally {
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

    /** CameraV2
     *
     */
    public CameraV2() {
        // @TODO
    }

    /** checkCameraPresence
     *
     */
    public static void checkCameraPresence(boolean[] back, boolean[] front) {
        back[0] = false;
        front[0] = false;

        CameraManager cm = (CameraManager) ModulesFragment.activity().getSystemService(Context.CAMERA_SERVICE);

        try {
            for (String cameraId : cm.getCameraIdList()) {
                CameraCharacteristics info = cm.getCameraCharacteristics(cameraId);

                int facing = info.get(CameraCharacteristics.LENS_FACING);
                if (CameraCharacteristics.LENS_FACING_BACK == facing) {
                    back[0] = true;
                } else if (CameraCharacteristics.LENS_FACING_FRONT == facing) {
                    front[0] = true;
                }
            }
        } catch (Exception e) {
            throw new RuntimeException("failed getting camera: " + e);
        }
    }

    /** startPreview
     *
     */
    private void startPreview() {
        // Start the preview
        try {
            // Get preview size
            Size previewSize = getOptimalPreviewSize(mCamera.getId());
            if (null == previewSize) {
                throw new RuntimeException("couldn't get preview size for Camera " + mCamera.getId());
            }

            setPreferredPreviewSize(previewSize.getWidth(), previewSize.getHeight());

            // Create ImageReader
            final int maxImages = 2;
            mPreviewImageReader = ImageReader.newInstance(getWidth(), getHeight(), mPreviewImageFormat, maxImages);
            mPreviewImageReader.setOnImageAvailableListener(mOnImageAvailableListener, mCameraHandler);
            mImageSurface = mPreviewImageReader.getSurface();

            // Create SurfaceTexture
            if (null == mDummyTexture) {
                mDummyTexture = new SurfaceTexture(0);
            }
            mDummyTexture.setDefaultBufferSize(getWidth(), getHeight());

            // Create Surface
            if (null == mDummySurface) {
                mDummySurface = new Surface(mDummyTexture);
            }

            // Create CaptureRequest.Builder
            mPreviewRequestBuilder = mCamera.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
            mPreviewRequestBuilder.addTarget(mDummySurface);
            mPreviewRequestBuilder.addTarget(mImageSurface);

            // Create CameraCaptureSession
            mCamera.createCaptureSession(
                Arrays.asList(mDummySurface, mImageSurface),
                new CameraCaptureSession.StateCallback() {
                    @Override
                    public void onConfigured(CameraCaptureSession session) {
                        if (null == mCamera) {
                            return;
                        }

                        mPreviewCaptureSession = session;
                        try {
                            mPreviewRequest = mPreviewRequestBuilder.build();
                            mPreviewCaptureSession.setRepeatingRequest(mPreviewRequest, null, mCameraHandler);
                        } catch (Exception e) {
                            Log.e(TAG, "CameraCaptureSession.setRepeatingRequest failed: " + e.getMessage());
                        }
                    }

                    @Override
                    public void onConfigureFailed(CameraCaptureSession session) {
                        Log.e(TAG, "Unable to create capture session using current configuration");
                    }
                },
                null
            );
        } catch (Exception e) {
            Log.e(TAG, "CameraDevice.StateCallback.onOpened failed: " + e);
        }
    }

    /** stopPreview
     *
     */
    private void stopPreview() {
        if (null != mPreviewCaptureSession) {
            try {
                mPreviewCaptureSession.stopRepeating();
            }
            catch(Exception e) {
                Log.e(TAG, "CameraCaptureSession.stopRepeating failed: " + e.getMessage());
            }

            mPreviewCaptureSession.close();
            mPreviewCaptureSession = null;
        }

        if (null != mCamera) {
            mCamera.close();
            mCamera = null;
        }

        if (null != mPreviewImageReader) {
            mPreviewImageReader.close();
            mPreviewImageReader = null;
        }
    }

    /** startDevice
     *
     */
    private void startDevice() {
        // Bail if we don't have a valid camera id or mCamera isn't null
        if ((null == mActiveDeviceId) || (null != mCamera)) {
            return;
        }

        //
        try {
            mCameraHandlerThread = new HandlerThread("camera-handler-thread");
            mCameraHandlerThread.start();

            mCameraHandler = new Handler(mCameraHandlerThread.getLooper());

            mCameraManager.openCamera(mActiveDeviceId, mStateCallback, mCameraHandler);
        } catch (Exception e) {
            Log.e(TAG, "startDevice failed: " + e.getMessage());
        }
    }

    /** stopDevice
     *
     */
    private void stopDevice() {
        stopPreview();

        if (null != mCameraHandlerThread) {
            try {
                mCameraHandlerThread.quitSafely();
                mCameraHandlerThread.join();
                mCameraHandlerThread = null;
                mCameraHandler = null;
            } catch (Exception e) {
                Log.e(TAG, "Camera handler thread stop failed: " + e);
            }
        }
    }

    private void startDevice(String deviceId) {
        if((null != mActiveDeviceId) && (mActiveDeviceId.equals(deviceId))) {
            return;
        }

        stopDevice();

        mActiveDeviceId = deviceId;
        startDevice();
    }

    /** updatePreviewTransform
     *
     */
    public void updatePreviewTransform(int viewWidth, int viewHeight, int orientation, int displayRotation) {
        float centerX = viewWidth/2.0f;
        float centerY = viewHeight/2.0f;
        float scaleX = 1;
        float scaleY = 1;

        float rotationDegrees = 0;
        switch(displayRotation) {
            case Surface.ROTATION_0: {
                rotationDegrees = 0;
            }
            break;

            case Surface.ROTATION_90: {
                rotationDegrees = 90;
                scaleX = (float)viewWidth/(float)viewHeight;
                scaleY = (float)viewHeight/(float)viewWidth;
            }
            break;

            case Surface.ROTATION_180: {
                rotationDegrees = 180;
            }
            break;

            case Surface.ROTATION_270: {
                rotationDegrees = 270;
                scaleX = (float)viewWidth/(float)viewHeight;
                scaleY = (float)viewHeight/(float)viewWidth;
            }
            break;
        }

        mPreviewTransform.reset();
        mPreviewTransform.postRotate(360.0f - rotationDegrees, centerX, centerY);
        mPreviewTransform.postScale(scaleX, scaleY, centerX, centerY);
    }

    // =============================================================================================
    // Camera functions
    // =============================================================================================

    /** initializeImpl
     *
     */
    @Override
    protected final void initializeImpl() {
        mCameraManager = (CameraManager) ModulesFragment.activity().getSystemService(Context.CAMERA_SERVICE);

        try {
            for (String cameraId : mCameraManager.getCameraIdList()) {
                CameraCharacteristics info = mCameraManager.getCameraCharacteristics(cameraId);

                int facing = info.get(CameraCharacteristics.LENS_FACING);
                if (CameraCharacteristics.LENS_FACING_BACK == facing) {
                    mBackDeviceId = cameraId;
                } else if (CameraCharacteristics.LENS_FACING_FRONT == facing) {
                    mFrontDeviceId = cameraId;
                }
            }
        } catch (Exception e) {
            throw new RuntimeException("Failed getting camera: " + e);
        }

        mPixelsMutex = new ReentrantLock();
    }

    /** setDummyTexture
     *
     */
    @Override
    protected void setDummyTextureImpl(SurfaceTexture dummyTexture) {
        if (null != mPreviewRequestBuilder) {
            mPreviewRequestBuilder.removeTarget(mDummySurface);
        }

        mDummyTexture = dummyTexture;
        mDummySurface = new Surface(mDummyTexture);

        if (null != mPreviewRequestBuilder) {
            mPreviewRequestBuilder.addTarget(mDummySurface);
        }
    }

    /** startCapture
     *
     */
    @Override
    protected void startCaptureImpl(String deviceId) {
        if(null != deviceId) {
            startDevice(deviceId);
        }
        else {
            if (isBackCameraAvailable()) {
                startDevice(mBackDeviceId);
            } else if (isFrontCameraAvailable()) {
                startDevice(mFrontDeviceId);
            }
        }
    }

    /** stopCapture
     *
     */
    @Override
    protected void stopCaptureImpl() {
        stopDevice();
    }

    /** switchToBackCamera
     *
     */
    @Override
    protected void switchToBackCameraImpl() {
        if (mActiveDeviceId.equals(mBackDeviceId)) {
            return;
        }


        startDevice(mBackDeviceId);
    }

    /** switchToFrontCamera
     *
     */
    @Override
    protected void switchToFrontCameraImpl() {
        if (mActiveDeviceId.equals(mFrontDeviceId)) {
            return;
        }

        startDevice(mFrontDeviceId);
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


    // =============================================================================================
    // Support methods
    // =============================================================================================

    /** getOptimalPreviewSize
     *
     */
    private Size getOptimalPreviewSize(String cameraId) {
        Size result = null;
        try {
            // Get the raw display size
            Point displaySize = ModulesFragment.get().getDefaultDisplaySize();
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
            Log.e(TAG, "getOptimalPreviewSize failed: " + e);
        }
        return result;
    }

}
