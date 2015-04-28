package org.libcinder.hardware;

import org.libcinder.Cinder;

import android.hardware.Camera;
import android.hardware.Camera.CameraInfo;
import android.util.Log;
import android.graphics.SurfaceTexture;
import android.view.Surface;

/** \class CameraV1
 *
 */
public class CameraV1 extends org.libcinder.hardware.Camera implements android.hardware.Camera.PreviewCallback {

    private static final String TAG = "CameraV1";

    private android.hardware.Camera mCamera = null;

    private SurfaceTexture mDummyTexture = null;

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

    private void cameraSetPreviewTexture(SurfaceTexture previewTexture) {
        try {
            mPreviewTexture = previewTexture;
            if (null != mPreviewTexture) {
                mPreviewTexture.setDefaultBufferSize(getWidth(), getHeight());
                mCamera.setPreviewTexture(mPreviewTexture);
            } else {
                // If previewTexture is null - use the dummy texture. The camera expects
                // a texture target to be present at all times to send frames to.
                mDummyTexture.setDefaultBufferSize(getWidth(), getHeight());
                mCamera.setPreviewTexture(mDummyTexture);
            }
        }
        catch(Exception e) {
            throw new RuntimeException(e.getMessage() + " (CameraV1.cameraSetPreviewTexture)");
        }
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
            mActiveDeviceId = deviceId;
            mCamera = android.hardware.Camera.open(Integer.parseInt(mActiveDeviceId));

            Camera.Parameters params = mCamera.getParameters();
            setPreferredPreviewSize(params.getPreviewSize().width, params.getPreviewSize().height);

            cameraSetPreviewTexture(mPreviewTexture);
            mCamera.setPreviewCallback(this);

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
        try {
            mActiveDeviceId = null;

            if (null != mCamera) {
                mCamera.setPreviewTexture(null);
                mCamera.setPreviewCallback(null);
                mCamera.stopPreview();
                mCamera.release();
                mCamera = null;
            }
        }
        catch(Exception e ) {
            Log.e(Cinder.TAG, "CinderCamera.stopDevice error: " + e.getMessage());
        }
    }

    /** startPreview
     *
     */
    private void startPreview() {
        if(null == mCamera) {
            return;
        }

        mCamera.startPreview();
    }

    /** stopPreview
     *
     */
    private void stopPreview() {
        if(null == mCamera) {
            return;
        }

        mCamera.stopPreview();
    }

    /** onPreviewFrame
     *
     */
    @Override
    public void onPreviewFrame(byte[] data, android.hardware.Camera camera) {
        lockPixels();
        try {
            if ((null == mPixels) || (mPixels.length != data.length)) {
                mPixels = new byte[data.length];
            }
            System.arraycopy(data, 0, mPixels, 0, data.length);
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
    }

    /** setPreviewTexture
     *
     */
    @Override
    protected void setPreviewTextureImpl(SurfaceTexture previewTexture) {
        if(null == mCamera) {
            return;
        }

        try {
            stopPreview();
            cameraSetPreviewTexture(previewTexture);
            startPreview();
        }
        catch(Exception e) {
            Log.w(TAG, "failed setting preview texture: " + e.getMessage());
        }
    }

    /** startSessionImpl
     *
     */
    @Override
    protected void startSessionImpl(String deviceId) {
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

    /** stopSessionImpl
     *
     */
    @Override
    protected void stopSessionImpl() {
        stopPreview();
        stopDevice();
    }

    /** startCaptureImpl
     *
     */
    @Override
    protected void startCaptureImpl() {
        if(null == mDummyTexture) {
            mDummyTexture = new SurfaceTexture(0);
        }

        startSession();
    }

    /** stopCaptureImpl
     *
     */
    @Override
    protected void stopCaptureImpl() {
        stopSession();
        mDummyTexture = null;
    }

    /** switchToBackCamera
     *
     */
    @Override
    protected void switchToBackCameraImpl() {
        if((null != mActiveDeviceId) && (mActiveDeviceId.equals(mBackDeviceId))) {
            return;
        }

        stopPreview();
        startDevice(mBackDeviceId);
        startPreview();
    }

    /** switchToFrontCamera
     *
     */
    @Override
    protected void switchToFrontCameraImpl() {
        if((null != mActiveDeviceId) && (mActiveDeviceId.equals(mFrontDeviceId))) {
            return;
        }

        stopPreview();
        startDevice(mFrontDeviceId);
        startPreview();
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
