package org.libcinder.hardware;

import org.libcinder.app.Platform;

import android.hardware.Camera;
import android.hardware.Camera.CameraInfo;
import android.util.Log;
import android.graphics.SurfaceTexture;

import java.util.concurrent.locks.ReentrantLock;

public class CinderCamera implements Camera.PreviewCallback {
    private SurfaceTexture mDummyTexture = null;

    private int mActiveCameraId = -1;
    private Camera mActiveCamera = null;

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
        // Bail if we don't have a valid camera id or mActiveCamera isn't null
        if ((-1 == mActiveCameraId) || (null != mActiveCamera)) {
            return;
        }

        Log.i(Platform.TAG, "startCamera (0)");

        try {
            Log.i(Platform.TAG, "startCamera (1)");
            mActiveCamera = Camera.open(mActiveCameraId);

            Camera.Parameters params = mActiveCamera.getParameters();
            mWidth = params.getPreviewSize().width;
            mHeight = params.getPreviewSize().height;

            Log.i(Platform.TAG, "startCamera (2)");
            mActiveCamera.setPreviewTexture(mDummyTexture);
            Log.i(Platform.TAG, "startCamera (3)");
            mActiveCamera.setPreviewCallback(this);
            Log.i(Platform.TAG, "startCamera (4)");
            mActiveCamera.startPreview();
            Log.i(Platform.TAG, "startCamera (5)");
        }
        catch(Exception e ) {
            Log.e(Platform.TAG, "CinderCamera.startCamera failed: " + e);
        }

        Log.i(Platform.TAG, "startCamera (6)");
    }

    public void stopCamera() {
        try {
            if (null != mActiveCamera) {
                mActiveCamera.setPreviewTexture(null);
                mActiveCamera.setPreviewCallback(null);
                mActiveCamera.stopPreview();
                mActiveCamera.release();
                mActiveCamera = null;
            }
        }
        catch(Exception e ) {
            Log.e(Platform.TAG, "CinderCamera.stopCamera failed: " + e);
        }
    }

    public void startFrontCamera() {
        stopCamera();

        mActiveCameraId = mFrontCameraId;
        startCamera();
    }

    public void startBackCamera() {
        stopCamera();

        mActiveCameraId = mBackCameraId;
        startCamera();
    }

    private void privateLockPixels() {
        mMutex.lock();
    }

    private void privateUnlockPixels() {
        mMutex.unlock();;
    }

    @Override
    public void onPreviewFrame(byte[] data, Camera camera) {
        //Camera.Parameters params = camera.getParameters();
        //int w = params.getPreviewSize().width;
        //int h = params.getPreviewSize().height;
        //Log.i(Platform.TAG, "onPreviewFrame: " + w + "x" + h + ", numBytes: " + data.length);

        privateLockPixels();

        try {
            Camera.Parameters params = camera.getParameters();
            mPixels = data;
        }
        finally {
            privateUnlockPixels();
        }
    }

    // -------------------------------------------------------------------------

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
        sCamera.startCamera();
    }

    public static void stopCapture() {
        sCamera.stopCamera();
    }

    public static byte[] lockPixels() {
        sCamera.privateLockPixels();
        return sCamera.mPixels;
    }

    public static void unlockPixels() {
        sCamera.privateLockPixels();
    }

    public static int getWidth() {
        return sCamera.mWidth;
    }

    public static int getHeight() {
        return sCamera.mHeight;
    }
}
