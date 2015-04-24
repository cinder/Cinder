package org.libcinder.hardware;

import org.libcinder.Cinder;

import android.hardware.Camera;
import android.hardware.Camera.CameraInfo;
import android.util.Log;
import android.graphics.SurfaceTexture;
import android.view.Surface;

import java.util.concurrent.locks.ReentrantLock;

/** \class CameraV1
 *
 */
public class CameraV1 extends org.libcinder.hardware.Camera implements android.hardware.Camera.PreviewCallback {

    private static final String TAG = "CameraV1";

    private android.hardware.Camera mCamera = null;

    private ReentrantLock mPixelsMutex  = null;

    /** CameraV1
     *
     */
    public CameraV1() {
        // @TODO
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

    /** startDevice
     *
     */
    private void startDevice() {
        // Bail if we don't have a valid camera id or mCamera isn't null
        if ((null == mActiveDeviceId) || (null != mCamera)) {
            return;
        }

        try {
            mCamera = android.hardware.Camera.open(Integer.parseInt(mActiveDeviceId));

            Camera.Parameters params = mCamera.getParameters();
            setPreferredPreviewSize(params.getPreviewSize().width, params.getPreviewSize().height);

            if(null == mDummyTexture) {
                mDummyTexture = new SurfaceTexture(0);
            }
            mDummyTexture.setDefaultBufferSize(getWidth(), getHeight());

            mCamera.setPreviewTexture(mDummyTexture);
            mCamera.setPreviewCallback(this);
            mCamera.startPreview();

        }
        catch(Exception e ) {
            Log.e(Cinder.TAG, "CinderCamera.startDevice failed: " + e);
        }
    }

    /** stopDevice
     *
     */
    private void stopDevice() {
        try {
            if (null != mCamera) {
                mCamera.setPreviewTexture(null);
                mCamera.setPreviewCallback(null);
                mCamera.stopPreview();
                mCamera.release();
                mCamera = null;
            }
        }
        catch(Exception e ) {
            Log.e(Cinder.TAG, "CinderCamera.stopDevice failed: " + e);
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

    /** onPreviewFrame
     *
     */
    @Override
    public void onPreviewFrame(byte[] data, android.hardware.Camera camera) {
        lockPixels();
        try {
            mPixels = data;
        }
        finally {
            unlockPixels();
        }
    }

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

        mPixelsMutex = new ReentrantLock();
    }

    /** setDummyTexture
     *
     */
    @Override
    protected void setDummyTextureImpl(SurfaceTexture dummyTexture) {
        mDummyTexture = dummyTexture;
        if(null != mCamera) {
            try {
                mCamera.setPreviewTexture(mDummyTexture);
            }
            catch(Exception e) {
                Log.w(TAG, "(setDummyTexture) Camera.setPreviewTexture failed");
            }
        }
    }

    /** startCaptureImpl
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

    /** stopCaptureImpl
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
        if((null != mActiveDeviceId) && (mActiveDeviceId.equals(mBackDeviceId))) {
            return;
        }

        startDevice(mBackDeviceId);
    }

    /** switchToFrontCamera
     *
     */
    @Override
    protected void switchToFrontCameraImpl() {
        if((null != mActiveDeviceId) && (mActiveDeviceId.equals(mFrontDeviceId))) {
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

    /** setDisplayOrientation
     *
     */
    protected void setDisplayOrientation(int displayRotation) {
        if((null != mActiveDeviceId) && (null == mCamera)) {
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
