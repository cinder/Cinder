package org.libcinder.hardware;

import org.libcinder.Cinder;
import org.libcinder.app.CinderNativeActivity;

import android.app.Activity;
import android.hardware.Camera;
import android.hardware.Camera.CameraInfo;
import android.opengl.GLES20;
import android.os.ConditionVariable;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.util.Log;
import android.graphics.SurfaceTexture;
import android.view.Surface;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.atomic.AtomicBoolean;


/** \class CameraV1
 *
 */
public class CameraV1 extends org.libcinder.hardware.Camera {

    private static final String TAG = "cinder|CameraV1";

    private android.hardware.Camera mCamera = null;

    private final int [] mDummyTextureHandle = new int[1];
    private SurfaceTexture mDummyTexture = null;

    private HandlerThread mCameraHandlerThread;
    private Handler mCameraHandler;

    private AtomicBoolean mPingBack = new AtomicBoolean(false);

    private Camera.PreviewCallback mPreviewCallback = new Camera.PreviewCallback() {
        @Override
        public void onPreviewFrame(byte[] data, android.hardware.Camera camera) {
            //Log.i(TAG, "onPreviewFrame: ThreadId=" + Thread.currentThread().getId());

            lockPixels();
            try {
                if ((null == mPixels) || (mPixels.length != data.length)) {
                    mPixels = new byte[data.length];
                }
                System.arraycopy(data, 0, mPixels, 0, data.length);

                camera.addCallbackBuffer(data);

                mNewFrameAvailable.set(true);
            }
            finally {
                unlockPixels();
            }

            //camera.addCallbackBuffer(data);
        }
    };

    /** CameraV1
     *
     */
    public CameraV1(CinderNativeActivity activity) {
        super(activity);
        Log.i(TAG, "CameraV1 constructed: ThreadId=" + Thread.currentThread().getId());
    }

    /** checkCameraPresence
     *
     */
    public static void checkCameraPresence(boolean[] back, boolean[] front) {
        back[0] = false;
        front[0] = false;
        int numberOfCameras = Camera.getNumberOfCameras();
        for( int i = 0; i < numberOfCameras; ++i ) {
            CameraInfo info = new CameraInfo();
            Camera.getCameraInfo(i, info);
            if(CameraInfo.CAMERA_FACING_BACK == info.facing) {
                back[0] = true;
            }
            else if(CameraInfo.CAMERA_FACING_FRONT == info.facing) {
                front[0] = true;
            }
        }
    }

    public DeviceInfo[] enumerateDevices() {
        Log.i(TAG, "enumerateDevices: ThreadId=" + Thread.currentThread().getId());

        ArrayList<DeviceInfo> devices = new ArrayList<>();

        for (int i = 0; i < Camera.getNumberOfCameras(); ++i) {
            CameraInfo info = new CameraInfo();
            Camera.getCameraInfo(i, info);
            String id = Integer.toString(i);

            boolean frontFacing = (CameraInfo.CAMERA_FACING_FRONT == info.facing);

            Camera cam = Camera.open(i);
            Camera.Parameters params = cam.getParameters();
            List<Camera.Size> previewSizes = params.getSupportedPreviewSizes();
            int[] resolutions = new int[2 * previewSizes.size()];
            for (int j = 0; j < previewSizes.size(); ++j) {
                Camera.Size size = previewSizes.get(j);
                resolutions[2 * j + 0] = size.width;
                resolutions[2 * j + 1] = size.height;
            }
            //
            Log.i(TAG, "Camera " + i);
            List<Integer> previewFormats = params.getSupportedPreviewFormats();
            for( Integer value : previewFormats ) {
                Log.i(TAG, "   format: " + org.libcinder.hardware.Camera.imageFormatString(value.intValue()) + ", value: " + value.intValue());
            }
            //
            cam.release();

            devices.add(new DeviceInfo(id, frontFacing, resolutions));
        }

        if (!devices.isEmpty()) {
            mCachedDeviceInfos = new DeviceInfo[devices.size()];
            for (int i = 0; i < devices.size(); ++i) {
                mCachedDeviceInfos[i] = devices.get(i);
            }
        }

        return mCachedDeviceInfos;
    }

    private void cameraSetPreviewTexture(SurfaceTexture previewTexture) {
        try {
            mPreviewTexture = previewTexture;
            if (null != mPreviewTexture) {
                mPreviewTexture.setDefaultBufferSize(getWidth(), getHeight());
                mCamera.setPreviewTexture(mPreviewTexture);
                Log.i(TAG, "Using allocated preview texture");
            } else {
                // If previewTexture is null - use the dummy texture. The camera expects
                // a texture target to be present at all times to send frames to.
                mDummyTexture.setDefaultBufferSize(getWidth(), getHeight());
                mCamera.setPreviewTexture(mDummyTexture);
                Log.i(TAG, "Using dummy texture for preview");
            }
        }
        catch(Exception e) {
            throw new RuntimeException(e.getMessage() + " (CameraV1.cameraSetPreviewTexture)");
        }
    }

    /** startCameraThread
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
                //mCameraHandler = new Handler(Looper.getMainLooper());
            }
        }
        catch(Exception e) {
            throw new RuntimeException("camera handler create error: " + e.getMessage());
        }

        Log.i(TAG, "startCameraThread EXIT");
    }

    /** stopCameraThread
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

        Log.i(TAG, "stopCameraThread EXIT");
    }

    /** startDevice
     *
     */
    private void startDevice(String deviceId) {
        Log.i(TAG, "startDevice ENTER: ThreadID=" + Thread.currentThread().getId());

        if((null != mActiveDeviceId) && mActiveDeviceId.equals(deviceId)) {
            return;
        }

        if(null != mActiveDeviceId) {
            stopDevice();
        }

        try {
            mActiveDeviceId = deviceId;
            mCamera = android.hardware.Camera.open(Integer.parseInt(mActiveDeviceId));

            Camera.Parameters params = mCamera.getParameters();

            if((mPreferredPreviewWidth > 0) && (mPreferredPreviewHeight > 0)) {
                params.setPreviewSize(mPreferredPreviewWidth, mPreferredPreviewHeight);
                mCamera.setParameters(params);
            }

            setPreferredPreviewSize(params.getPreviewSize().width, params.getPreviewSize().height);

            if(null == mDummyTexture) {
                GLES20.glGenTextures(1, mDummyTextureHandle, 0);
                Log.i(TAG, "Created mDummyTextureHandle: " + mDummyTextureHandle[0]);

                mDummyTexture = new SurfaceTexture(mDummyTextureHandle[0]);
                //mDummyTexture = new SurfaceTexture(0);
                mDummyTexture.setDefaultBufferSize( params.getPreviewSize().width, params.getPreviewSize().height );
            }

            cameraSetPreviewTexture(mPreviewTexture);

            Log.i(TAG, "Started Camera " + deviceId + ": res=" + getWidth() + "x" + getHeight() + ", fmt=NV21");
        }
        catch(Exception e ) {
            Log.e(Cinder.TAG, "startDevice error: " + e.getMessage());
        }
    }

    /** stopDevice
     *
     */
    private void stopDevice() {
        Log.i(TAG, "CameraV1.stopDevice");
        try {
            mActiveDeviceId = null;

            if (null != mCamera) {
                //mCamera.setPreviewCallback(null);
                //mCamera.stopPreview();
                mCamera.release();
                mCamera = null;
            }

            GLES20.glDeleteTextures(1, mDummyTextureHandle, 0);
            Log.i(TAG, "Deleted mDummyTextureHandle: " + mDummyTextureHandle[0]);
        }
        catch(Exception e ) {
            Log.e(Cinder.TAG, "CinderCamera.stopDevice error: " + e.getMessage());
        }
    }

    /** startPreview
     *
     */
    private void startPreview() {
        Log.i(TAG, "startPreview ENTER: ThreadID=" + Thread.currentThread().getId());
        if(null == mCamera) {
            return;
        }

        mNewFrameAvailable.set(false);
        mCamera.setPreviewCallback(mPreviewCallback);
        mCamera.startPreview();

        Log.i(TAG, "startPreview EXIT");
    }

    /** stopPreview
     *
     */
    private void stopPreview() {
        Log.i(TAG, "CameraV1.stopPreview");

        if(null == mCamera) {
            return;
        }

        try {
            mNewFrameAvailable.set(false);
            mCamera.setPreviewCallback(null);
            mCamera.stopPreview();
        }
        catch(Exception e) {
            Log.e(TAG, "CameraV1.stopPreview failed: " + e.getMessage());
        }
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

//    /** onPreviewFrame
//     *
//     */
//    @Override
//    public void onPreviewFrame(byte[] data, android.hardware.Camera camera) {
//        Log.i(TAG, "onPreviewFrame: ThreadId=" + Thread.currentThread().getId());
//
//        lockPixels();
//        try {
//            if ((null == mPixels) || (mPixels.length != data.length)) {
//                mPixels = new byte[data.length];
//            }
//            System.arraycopy(data, 0, mPixels, 0, data.length);
//
//            camera.addCallbackBuffer(data);
//        }
//        finally {
//            unlockPixels();
//        }
//    }

    // =============================================================================================
    // Camera functions
    // =============================================================================================

    @Override
    protected final void initializeImpl() {
        int numberOfCameras = Camera.getNumberOfCameras();
        for( int i = 0; i < numberOfCameras; ++i ) {
            android.hardware.Camera.CameraInfo info = new android.hardware.Camera.CameraInfo();
            Camera.getCameraInfo(i, info);
            if(CameraInfo.CAMERA_FACING_BACK == info.facing) {
                mBackDeviceId = Integer.toString(i);
            }
            else if(CameraInfo.CAMERA_FACING_FRONT == info.facing) {
                mFrontDeviceId = Integer.toString(i);
            }
        }
    }

    /** setPreviewTexture
     *
     */
    @Override
    protected void setPreviewTextureImpl(final SurfaceTexture previewTexture) {
        if(null == mCamera) {
            return;
        }

        mCameraHandler.post(new Runnable() {
            @Override
            public void run() {
                try {
                    stopPreview();
                    cameraSetPreviewTexture(previewTexture);
                    startPreview();
                }
                catch(Exception e) {
                    Log.w(TAG, "failed setting preview texture: " + e.getMessage());
                }
            }
        });

        /*
        try {
            stopPreview();
            cameraSetPreviewTexture(previewTexture);
            startPreview();
        }
        catch(Exception e) {
            Log.w(TAG, "failed setting preview texture: " + e.getMessage());
        }
        */
    }

    /** startSessionImpl
     *
     */
    @Override
    protected void startSessionImpl(final String deviceId) {
        Log.i(TAG, "startSessionImpl: ThreadId=" + Thread.currentThread().getId());

        if(null == mCameraHandler) {
            return;
        }

        mCameraHandler.post(new Runnable() {
            @Override
            public void run() {
                if (null != deviceId) {
                    startDevice(deviceId);
                }
                else {
                    if (isBackCameraAvailable()) {
                        startDevice(mBackDeviceId);
                    } else if (isFrontCameraAvailable()) {
                        startDevice(mFrontDeviceId);
                    }
                }

                startPreview();
            }
        });


        /*
        if (null != deviceId) {
            startDevice(deviceId);
        }
        else {
            if (isBackCameraAvailable()) {
                startDevice(mBackDeviceId);
            } else if (isFrontCameraAvailable()) {
                startDevice(mFrontDeviceId);
            }
        }

        startPreview();
        */
    }

    /**
     * stopSessionImpl
     *
     */
    @Override
    protected void stopSessionImpl() {
        Log.i(TAG, "CameraV1.stopSessionImpl");
        stopPreview();
        stopDevice();
    }

    /**
     * startCaptureImpl
     *
     */
    @Override
    protected void startCaptureImpl(final String deviceId) {
        startCameraThread();
    }

    /**
     * stopCaptureImpl
     *
     */
    @Override
    protected void stopCaptureImpl() {
        Log.i(TAG, "CameraV1.stopCaptureImpl");
        stopCameraThread();
        mDummyTexture = null;
    }

    /**
     * lockPixels
     *
     */
    @Override
    public byte[] lockPixels() {
        mPixelsMutex.lock();
        return mPixels;
    }

    /**
     * unlockPixels
     *
     */
    @Override
    public void unlockPixels() {
        mPixelsMutex.unlock();
    }

    /** setDisplayOrientation
     *
     */
    protected void setDisplayOrientation(int displayRotation) {
        if((null == mActiveDeviceId) || (null == mCamera)) {
            return;
        }

        int degrees = 0;
        switch(displayRotation) {
            case Surface.ROTATION_0   : degrees = 0;   break;
            case Surface.ROTATION_90  : degrees = 90;  break;
            case Surface.ROTATION_180 : degrees = 180; break;
            case Surface.ROTATION_270 : degrees = 270; break;
        }

        int cameraId = Integer.parseInt(mActiveDeviceId);
        CameraInfo info = new CameraInfo();
        Camera.getCameraInfo(cameraId, info);

        int adjusted = 0;
        if(CameraInfo.CAMERA_FACING_FRONT == info.facing) {
            adjusted = (info.orientation + degrees) % 360;
            adjusted = (360 - adjusted) % 360;
        }
        else {
            adjusted = (info.orientation - degrees + 360) % 360;
        }

        Log.i(TAG, "info.orientation: " + info.orientation);

        mCamera.setDisplayOrientation(adjusted);
    }
}
