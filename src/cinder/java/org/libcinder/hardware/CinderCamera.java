package org.libcinder.hardware;

import org.libcinder.app.Platform;

import android.hardware.Camera;
import android.hardware.Camera.CameraInfo;
import android.util.Log;
import android.graphics.SurfaceTexture;

import java.util.concurrent.locks.ReentrantLock;

/** \class CinderCamera
 *
 */
public class CinderCamera implements Camera.PreviewCallback {
    private SurfaceTexture mDummyTexture = null;

    private int mActiveCameraId = -1;
    private Camera mCamera = null;

    private int mFrontCameraId = -1;
    private int mBackCameraId = -1;

    private int mWidth = 0;
    private int mHeight = 0;
    private byte[] mPixels = null;
    private ReentrantLock mMutex;

    public CinderCamera() {
        int numberOfCameras = Camera.getNumberOfCameras();
        for( int i = 0; i < numberOfCameras; ++i ) {
            CameraInfo info = new CameraInfo();
            Camera.getCameraInfo(i, info);
            if(CameraInfo.CAMERA_FACING_FRONT == info.facing) {
                mFrontCameraId = i;
            }
            else if(CameraInfo.CAMERA_FACING_BACK == info.facing) {
                mBackCameraId = i;
            }
        }

        mActiveCameraId = (-1 != mBackCameraId) ? mBackCameraId : ((-1 != mFrontCameraId) ? mFrontCameraId : -1);

        Log.i(Platform.TAG, "Front Camera: " + mFrontCameraId);
        Log.i(Platform.TAG, "Back Camera: " + mBackCameraId);

        mDummyTexture = new SurfaceTexture(0);

        mMutex = new ReentrantLock();
    }

    public void startCamera() {
        // Bail if we don't have a valid camera id or mCamera isn't null
        if ((-1 == mActiveCameraId) || (null != mCamera)) {
            return;
        }

        try {
            mCamera = Camera.open(mActiveCameraId);

            Camera.Parameters params = mCamera.getParameters();
            mWidth = params.getPreviewSize().width;
            mHeight = params.getPreviewSize().height;

            mCamera.setPreviewTexture(mDummyTexture);
            mCamera.setPreviewCallback(this);
            mCamera.startPreview();
        }
        catch(Exception e ) {
            Log.e(Platform.TAG, "CinderCamera.startCamera failed: " + e);
        }
    }

    public void stopCamera() {
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
            Log.e(Platform.TAG, "CinderCamera.stopCamera failed: " + e);
        }
    }

    public void startFrontCamera() {
        if(mFrontCameraId == mActiveCameraId) {
            return;
        }

        stopCamera();

        mActiveCameraId = mFrontCameraId;
        startCamera();
    }

    public void startBackCamera() {
        if(mBackCameraId == mActiveCameraId) {
            return;
        }

        stopCamera();

        mActiveCameraId = mBackCameraId;
        startCamera();
    }

    private void privateLockPixels() {
        //Log.i(Platform.TAG, "privateLockPixels");
        mMutex.lock();
    }

    private void privateUnlockPixels() {
        mMutex.unlock();;
        //Log.i(Platform.TAG, "privateUnlockPixels");
    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        privateLockPixels();
        try {
            mPixels = data;
        }
        finally {
            privateUnlockPixels();
        }
    }

    // =============================================================================================
    // Static Methods for C++
    // =============================================================================================

    private static CinderCamera sCamera = null;

    public static boolean initialize() {
        if(null == sCamera) {
            sCamera = new CinderCamera();
        }
        return (-1 != sCamera.mFrontCameraId || -1 != sCamera.mBackCameraId);
    }

    public static boolean hasFrontCamera() {
        return (null != sCamera) && (-1 != sCamera.mFrontCameraId);
    }

    public static boolean hasBackCamera() {
        return (null != sCamera) && (-1 != sCamera.mBackCameraId);
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
