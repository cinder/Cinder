package org.libcinder.hardware;

import org.libcinder.app.CinderNativeActivity;

import android.annotation.TargetApi;
import android.app.Activity;
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
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;

/** \class CameraV2
 *
 */
@TargetApi(21)
public class CameraV2 extends org.libcinder.hardware.Camera {

    private static final String TAG = "cinder|CameraV2";

    private CameraManager mCameraManager = null;

    private Semaphore mCameraOpenCloseLock = new Semaphore(1);

    private CameraDevice mCamera = null;

    private static int MAX_WAIT_TIME_MS = 10000;

    private HandlerThread mCameraHandlerThread;
    private Handler mCameraHandler;

    private static final int MAX_PREVIEW_IMAGES = 2;
    private int mPreviewImageFormat = ImageFormat.YUV_420_888;
    private ImageReader mImageReader = null;
    private Surface mImageReaderSurface = null;

    private SurfaceTexture mDummyTexture = null;
    private Surface mPreviewSurface = null;

    private CaptureRequest mPreviewRequest = null;
    private CaptureRequest.Builder mPreviewRequestBuilder = null;
    private CameraCaptureSession mPreviewCaptureSession = null;
    private AtomicBoolean mPreviewCaptureSessionCreated = new AtomicBoolean(false);

    private AtomicBoolean mPingBack = new AtomicBoolean(false);

    // =============================================================================================
    // Callback/listener member vars
    // =============================================================================================

    private final ImageReader.OnImageAvailableListener mOnImageAvailableListener = new ImageReader.OnImageAvailableListener() {
        @Override
        public void onImageAvailable(ImageReader reader) {
            lockPixels();
            try {
                Image image = reader.acquireLatestImage();
                if ((null != image) && (image.getPlanes().length > 0)) {
                    /*
                    //Log.i(TAG, "Number of planes:" + image.getPlanes().length);

                    //ByteBuffer buf0 = image.getPlanes()[0].getBuffer();
                    //ByteBuffer buf1 = image.getPlanes()[1].getBuffer();
                    //ByteBuffer buf2 = image.getPlanes()[2].getBuffer();
                    //Log.i(TAG, "...plane[0] size:" + buf0.remaining());
                    //Log.i(TAG, "...plane[1] size:" + buf1.remaining());
                    //Log.i(TAG, "...plane[2] size:" + buf2.remaining());

                    ByteBuffer buffer = image.getPlanes()[0].getBuffer();

                    if ((null == mPixels) || (mPixels.length != buffer.remaining())) {
                        mPixels = new byte[buffer.remaining()];
                    }
                    buffer.get(mPixels);

                    image.close();
                    image = null;

                    mNewFrameAvailable.set(true);
                    */
                }
            }
            finally {
                unlockPixels();

                reader.close();
                reader = null;
            }
        }
    };

    private final CameraDevice.StateCallback mStateCallback = new CameraDevice.StateCallback() {
        @Override
        public void onOpened(CameraDevice camera) {
            mCameraOpenCloseLock.release();
            mCamera = camera;

            Log.i(TAG, "Thread ID: " + Thread.currentThread().getId() + " | (CameraDevice.StateCallback.onOpened)");
        }

        /*
        @Override
        public void onClosed(CameraDevice camera) {
            Log.i(TAG, "Thread ID: " + Thread.currentThread().getId() + " | (CameraDevice.StateCallback.onClosed)");
        }
        */

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

    // =============================================================================================
    // CameraV2
    // =============================================================================================

    /** CameraV2
     *
     */
    public CameraV2(CinderNativeActivity activity) {
        super(activity);
        Log.i(TAG, "CameraV2 constructed");
    }

    /** checkCameraPresence
     *
     */
    public static void checkCameraPresence(boolean[] back, boolean[] front) {
        back[0] = false;
        front[0] = false;

        CameraManager cm = (CameraManager)CinderNativeActivity.getInstance().getSystemService(Context.CAMERA_SERVICE);

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

    public DeviceInfo[] enumerateDevices() {
        ArrayList<DeviceInfo> devices = new ArrayList<>();

        try {
            CameraManager cm = (CameraManager)getActivity().getSystemService(Context.CAMERA_SERVICE);
            for (String id : cm.getCameraIdList()) {
                CameraCharacteristics info = cm.getCameraCharacteristics(id);

                int facing = info.get(CameraCharacteristics.LENS_FACING);
                boolean frontFacing = (CameraCharacteristics.LENS_FACING_FRONT == facing);

                StreamConfigurationMap scm = info.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
                List<Size> sizes = Arrays.asList(scm.getOutputSizes(SurfaceTexture.class));
                int[] resolutions = new int[2*sizes.size()];
                for(int j = 0; j < sizes.size(); ++j) {
                    Size size = sizes.get(j);
                    resolutions[2*j + 0] = size.getWidth();
                    resolutions[2*j + 1] = size.getHeight();
                }

                devices.add(new DeviceInfo(id, frontFacing, resolutions));

                if (!devices.isEmpty()) {
                    mCachedDeviceInfos = new DeviceInfo[devices.size()];
                    for (int i = 0; i < devices.size(); ++i) {
                        mCachedDeviceInfos[i] = devices.get(i);
                    }
                }
            }
        }
        catch(Exception e ) {
            Log.w(TAG, "enumerateDevices error: unable to get camera device list");
        }

        DeviceInfo[] result = null;
        if(! devices.isEmpty()) {
            result = new DeviceInfo[devices.size()];
            for(int i = 0; i < devices.size(); ++i) {
                result[i] = devices.get(i);
            }
        }

        return mCachedDeviceInfos;
    }

    /** cameraThreadFn_setPreviewTexture
     *
     * [Only call from mCameraHandlerThread]
     *
     */
    private void cameraThreadFn_setPreviewTexture(final SurfaceTexture previewTexture) {
        try {
            mCameraOpenCloseLock.acquire();

            if(null != previewTexture) {
                mPreviewTexture = previewTexture;
                mPreviewTexture.setDefaultBufferSize(getWidth(), getHeight());
                mPreviewSurface = new Surface(mPreviewTexture);
            }
            else {
                mDummyTexture.setDefaultBufferSize(getWidth(), getHeight());
                mPreviewSurface = new Surface(mDummyTexture);
            }

        }
        catch(Exception e) {
            Log.e(TAG, "cameraThreadFn_setPreviewTexture error: " + e.getMessage());
        }
        finally {
            mCameraOpenCloseLock.release();
        }
    }

    /** cameraThreadFn_createCaptureSession
     *
     * [Only call from mCameraHandlerThread]
     *
     */
    private void cameraThreadFn_createCaptureSession() {
        Log.i(TAG, "cameraThreadFn_createCaptureSession ENTER");

        try {
            mCameraOpenCloseLock.acquire();

            Log.i(TAG, "----- cameraThreadFn_createCaptureSession: Thread ID=" + Thread.currentThread().getId());

            if (mPreviewCaptureSessionCreated.get()) {
                Log.i(TAG, "----- cameraThreadFn_createCaptureSession: exiting - session already created");
                return;
            }

            if (null == mImageReaderSurface) {
                throw new RuntimeException("mImageReaderSurface is null");
            }

            if (null == mPreviewSurface) {
                throw new RuntimeException("mPreviewSurface is null");
            }

            // Allocate CaptureRequest.Builder
            mPreviewRequestBuilder = mCamera.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
            mPreviewRequestBuilder.addTarget(mImageReaderSurface);
            mPreviewRequestBuilder.addTarget(mPreviewSurface);

            mCamera.createCaptureSession(
                Arrays.asList(mImageReaderSurface, mPreviewSurface),
                new CameraCaptureSession.StateCallback() {
                    @Override
                    public void onConfigured(CameraCaptureSession session) {
                        //Log.i(TAG, "CameraCaptureSession.StateCallback.onConfigured");

                        if (null == mCamera) {
                            return;
                        }

                        mPreviewCaptureSession = session;
                        try {
                            mPreviewRequest = mPreviewRequestBuilder.build();
                            mPreviewCaptureSession.setRepeatingRequest(mPreviewRequest, null, mCameraHandler);
                        } catch (Exception e) {
                            //throw new RuntimeException("CameraCaptureSession.setRepeatingRequest error: " + e.getMessage());
                            e.printStackTrace();
                        }
                    }

                    @Override
                    public void onConfigureFailed(CameraCaptureSession session) {
                        String config = "(res:" + getWidth() + "x" + getHeight() + ", fmt:" + Camera.imageFormatString(mPreviewImageFormat) + ")";
                        throw new RuntimeException("Unable to create capture session using current configuration " + config);
                    }
                },
                null
            );

            mPreviewCaptureSessionCreated.set(true);
        }
        catch(Exception e) {
            Log.e(TAG, "cameraThreadFn_createCaptureSession error: " + e.getMessage());
        }
        finally {
            mCameraOpenCloseLock.release();
        }

        Log.i(TAG, "cameraThreadFn_createCaptureSession EXIT");
    }

    /** cameraThreadFn_destroyCaptureSession
     *
     * [Only call from mCameraHandlerThread]
     *
     */
    private void cameraThreadFn_destroyCaptureSession() {
        Log.i(TAG, "cameraThreadFn_destroyCaptureSession ENTER");

        if(! mPreviewCaptureSessionCreated.get()) {
            Log.i(TAG, "cameraThreadFn_destroyCaptureSession: exiting - no active session");
            return;
        }

        try {
            mCameraOpenCloseLock.acquire();

            if(null != mPreviewCaptureSession) {
                mPreviewCaptureSession.abortCaptures();
                mPreviewCaptureSession.close();
                mPreviewCaptureSession = null;
            }

            mPreviewRequestBuilder = null;

            mPreviewCaptureSessionCreated.set(false);
        }
        catch(Exception e) {
            Log.e(TAG, "cameraThreadFn_destroyCaptureSession error: " + e.getMessage());
        }
        finally {
            mCameraOpenCloseLock.release();
        }

        Log.i(TAG, "cameraThreadFn_destroyCaptureSession EXIT");
    }

    /** startCameraHandlerThread
     *
     */
    private void startCameraThread() {
        Log.i(TAG, "startCameraThread ENTER");

        // Create camera handler thread
        try {
            if (null == mCameraHandlerThread) {
                mCameraHandlerThread = new HandlerThread("camera-handler-thread");
                mCameraHandlerThread.start();
                Log.i(TAG, "Thread ID: " + mCameraHandlerThread.getId() + "| (mCameraHandlerThread.getId())");
            }
        }
        catch(Exception e) {
            throw new RuntimeException("camera handler thread error: " + e.getMessage());
        }

        // Create camera handler
        try {
            if (null == mCameraHandler) {
                mCameraHandler = new Handler(mCameraHandlerThread.getLooper());
            }
        }
        catch(Exception e) {
            throw new RuntimeException("camera handler create error: " + e.getMessage());
        }

        Log.i(TAG, "startCameraThread EXIT");
    }

    /** stopCameraHandlerThread
     *
     */
    private void stopCameraThread() {
        Log.i(TAG, "stopCameraThread ENTER");

        try {
            if (null != mCameraHandlerThread) {
                flushCameraHandler();

                mCameraHandlerThread.quitSafely();
                mCameraHandlerThread.join();
                mCameraHandlerThread = null;
                mCameraHandler = null;
            }

        } catch (Exception e) {
            throw new RuntimeException("camera handler thread stop error: " + e.getMessage());
        }

        mCameraManager = null;

        Log.i(TAG, "stopCameraThread EXIT");
    }

    /** startPreview
     *
     */
    private void startPreview() {
        Log.i(TAG, "startPreview ENTER");

        try {
            if(null != mCameraHandler) {
                mCameraHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        cameraThreadFn_createCaptureSession();
                    }
                });
            }
        }
        catch(Exception e) {
            Log.e(TAG, "startPreview error: " + e.getMessage());
        }

        Log.i(TAG, "startPreview EXIT");
    }

    /** stopPreview
     *
     */
    private void stopPreview() {
        Log.i(TAG, "stopPreview ENTER");

        if(! mPreviewCaptureSessionCreated.get()) {
            Log.i(TAG, "stopPreview: exiting - no active session");
            return;
        }

        try {
            mCameraOpenCloseLock.acquire();

            if(null != mPreviewCaptureSession) {
                mPreviewCaptureSession.abortCaptures();
                mPreviewCaptureSession.close();
                mPreviewCaptureSession = null;
            }

            mPreviewRequestBuilder = null;

            mPreviewCaptureSessionCreated.set(false);
        }
        catch(Exception e) {
            Log.e(TAG, "stopPreview error: " + e.getMessage());
        }
        finally {
            mCameraOpenCloseLock.release();
        }

        /*
        try {
            if(null != mCameraHandler) {
                mCameraHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        cameraThreadFn_destroyCaptureSession();
                    }
                });
            }
        }
        catch(Exception e) {
            Log.e(TAG, "stopPreview error: " + e.getMessage());
        }
        */

        Log.i(TAG, "stopPreview EXIT");
    }

    /** startDevice
     *
     */
    private void startDevice(String deviceId) {
        if((null != mActiveDeviceId) && mActiveDeviceId.equals(deviceId)) {
            return;
        }

        if(null != mActiveDeviceId) {
            stopDevice();
        }

        try {
            if(null == deviceId) {
                throw new RuntimeException("parameter deviceId is null");
            }

            mActiveDeviceId = deviceId;

            Size previewSize = new Size(mPreferredPreviewWidth, mPreferredPreviewHeight);
            if((0 == mPreferredPreviewWidth) && (0 == mPreferredPreviewHeight)) {
                previewSize = getOptimalPreviewSize(mActiveDeviceId);
                if (null == previewSize) {
                    throw new RuntimeException("couldn't getInstance preview size for Camera " + mCamera.getId());
                }
            }
            setPreferredPreviewSize(previewSize.getWidth(), previewSize.getHeight());

            // We create this here since the resolution is device dependent.
            mImageReader = ImageReader.newInstance(getWidth(), getHeight(), mPreviewImageFormat, MAX_PREVIEW_IMAGES);
            mImageReader.setOnImageAvailableListener(mOnImageAvailableListener, mCameraHandler);
            mImageReaderSurface = mImageReader.getSurface();

            // Readjust size to match device
            mDummyTexture.setDefaultBufferSize(getWidth(), getHeight());
            if(null != mPreviewTexture) {
                mPreviewTexture.setDefaultBufferSize(getWidth(), getHeight());
            }

            // Acquire lock to start device
            if (! mCameraOpenCloseLock.tryAcquire(MAX_WAIT_TIME_MS, TimeUnit.MILLISECONDS)) {
                throw new RuntimeException("timed out acquiring camera lock");
            }
            Log.i(TAG, "Thread ID: " + Thread.currentThread().getId() + " | (CameraManager.openCamera)");
            mCameraManager.openCamera(mActiveDeviceId, mStateCallback, mCameraHandler);


            Log.i(TAG, "Started Camera " + deviceId + ": res=" + getWidth() + "x" + getHeight() + ", fmt=" + Camera.imageFormatString(mPreviewImageFormat));
        }
        catch(Exception e) {
            Log.e(TAG, "startDevice error: " + e.getMessage());
        }
    }

    /** stopDevice
     *
     */
    private void stopDevice() {
        Log.i(TAG, "stopDevice ENTER");

        try {
            mCameraOpenCloseLock.acquire();

            mActiveDeviceId = null;

            if(null != mCamera) {
                mCamera.close();
                mCamera = null;
            }
        }
        catch(Exception e) {
            Log.e(TAG, "stopDevice error: " + e.getMessage());
        }
        finally {
            mCameraOpenCloseLock.release();
        }

        /*
        try {
            if((null != mCameraHandler) && (null != mCameraHandlerThread) && (mCameraHandlerThread.isAlive())) {
                mCameraHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        cameraThreadFn_stopCamera();
                    }
                });
            }
        }
        catch(Exception e) {
            Log.e(TAG, "stopDevice error: " + e.getMessage());
        }
        */


        Log.i(TAG, "stopDevice EXIT");
    }

    private void flushCameraHandler() {
        try {
            mPingBack.set(false);
            if (null != mCameraHandler) {
                mCameraHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        mPingBack.set(true);
                    }
                });

                while (!mPingBack.get()) {
                    Thread.sleep(1);
                }
                mCameraHandler.removeCallbacksAndMessages(null);
            }
        }
        catch(Exception e) {
            throw new RuntimeException("flushCamerHandler error: " + e.getMessage());
        }
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
        mCameraManager = (CameraManager)getActivity().getSystemService(Context.CAMERA_SERVICE);

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
            throw new RuntimeException("failed getting camera: " + e.getMessage());
        }
    }

    /** setPreviewTextureImpl
     *
     */
    @Override
    protected void setPreviewTextureImpl(final SurfaceTexture previewTexture) {
        Log.i(TAG, "setPreviewTextureImpl ENTER");

        try {
            flushCameraHandler();

            stopPreview();

            try {
                mCameraOpenCloseLock.acquire();

                if(null != previewTexture) {
                    mPreviewTexture = previewTexture;
                    mPreviewTexture.setDefaultBufferSize(getWidth(), getHeight());
                    mPreviewSurface = new Surface(mPreviewTexture);
                }
                else {
                    mDummyTexture.setDefaultBufferSize(getWidth(), getHeight());
                    mPreviewSurface = new Surface(mDummyTexture);
                }
            }
            catch(Exception e) {
                Log.e(TAG, "stopPreview error: " + e.getMessage());
            }
            finally {
                mCameraOpenCloseLock.release();
            }

            startPreview();

        }
        catch(Exception e) {
            Log.w(TAG, "failed setting preview texture: " + e.getMessage());
        }

        /*
        try {
            if((null != mCameraHandler) && (null != mCameraHandlerThread) && (mCameraHandlerThread.isAlive())) {
                mCameraHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        cameraThreadFn_destroyCaptureSession();
                        cameraThreadFn_setPreviewTexture(previewTexture);
                        cameraThreadFn_createCaptureSession();
                    }
                });
            }
        }
        catch(Exception e) {
            Log.w(TAG, "failed setting preview texture: " + e.getMessage());
        }
        */

        /*
        try {
            mPreviewTexture = previewTexture;
            if(null != mPreviewTexture) {
                mPreviewTexture.setDefaultBufferSize(getWidth(), getHeight());
            }

            cameraThreadFn_setPreviewTexture(mPreviewTexture);
        }
        catch(Exception e) {
            Log.w(TAG, "failed setting preview texture: " + e.getMessage());
        }
        */

        Log.i(TAG, "setPreviewTextureImpl EXIT");
    }

    /** startSessionImpl
     *
     */
    @Override
    protected void startSessionImpl(String deviceId) {
        if(null != deviceId) {
            startDevice(deviceId);
        }
        else {
            if(isBackCameraAvailable()) {
                startDevice(mBackDeviceId);
            } else if(isFrontCameraAvailable()) {
                startDevice(mFrontDeviceId);
            }
        }

        startPreview();
    }

    /** stopSessionImpl
     *
     */
    @Override
    protected void stopSessionImpl() {
        Log.i(TAG, "stopSessionImpl ENTER");

        stopPreview();
        stopDevice();

        Log.i(TAG, "stopSessionImpl EXIT");
    }

    /** startCaptureImpl
     *
     */
    @Override
    protected void startCaptureImpl(String deviceId) {
        // Create dummy texture
        if(null == mDummyTexture) {
            mDummyTexture = new SurfaceTexture(0);
        }

        // Create preview surface
        if(null == mPreviewSurface) {
            mPreviewSurface = new Surface(mDummyTexture);
        }

        startCameraThread();
        startSession(deviceId);
    }

    /** stopCaptureImpl
     *
     */
    @Override
    protected void stopCaptureImpl() {
        Log.i(TAG, "stopCaptureImpl ENTER");

        stopSession();
        stopCameraThread();
        mPreviewSurface = null;
        mDummyTexture = null;

        Log.i(TAG, "stopCaptureImpl EXIT");
    }

//    /** switchToBackCameraImpl
//     *
//     */
//    @Override
//    protected void switchToBackCameraImpl() {
//        if (mActiveDeviceId.equals(mBackDeviceId)) {
//            return;
//        }
//
//        stopPreview();
//        startDevice(mBackDeviceId);
//        startPreview();
//    }
//
//    /** switchToFrontCameraImpl
//     *
//     */
//    @Override
//    protected void switchToFrontCameraImpl() {
//        if (mActiveDeviceId.equals(mFrontDeviceId)) {
//            return;
//        }
//
//        stopPreview();
//        startDevice(mFrontDeviceId);
//        startPreview();
//    }

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
            Point displaySize = getActivity().getDefaultDisplaySize();
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
