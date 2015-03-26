package org.libcinder.hardware;

import org.libcinder.app.Platform;

import android.content.Context;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;

public class CinderCamera2 {
    private CameraManager mCameraManager = null;
    private String mFrontCameraId = null;
    private String mBackCameraId = null;

    private String mActiveCameraId = null;
    private CameraDevice mActiveCamera = null;

    private final CameraDevice.StateCallback mStateCallback = new CameraDevice.StateCallback() {
        @Override
        public void onOpened(CameraDevice camera) {
            mActiveCamera = camera;
        }

        @Override
        public void onDisconnected(CameraDevice camera) {
            mActiveCamera = null;
        }

        @Override
        public void onError(CameraDevice camera, int error) {

        }
    };

    private HandlerThread mBackgroundThread;
    private Handler mBackgroundHandler;

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

        Log.i(Platform.TAG, "Front Camera: " + mFrontCameraId);
        Log.i(Platform.TAG, "Back Camera: " + mBackCameraId);
    }


    public void startCamera() {
        // Bail if we don't have a valid camera id or mActiveCamera isn't null
        if ((null == mActiveCameraId) || (null != mActiveCamera)) {
            return;
        }

        try {
            mBackgroundThread = new HandlerThread("CameraBackground");
            mBackgroundThread.start();

            mBackgroundHandler = new Handler(mBackgroundThread.getLooper());

            mCameraManager.openCamera(mActiveCameraId, mStateCallback, mBackgroundHandler);
        }
        catch(Exception e ) {
            Log.e(Platform.TAG, "CinderCamera2.startCamera failed: " + e);
        }
    }

    public void stopCamera() {
        if(null != mBackgroundThread) {
            mBackgroundThread.quitSafely();
            try {
                mBackgroundThread.join();
                mBackgroundThread = null;
                mBackgroundHandler = null;
            }
            catch(Exception e ) {
                Log.e(Platform.TAG, "CinderCamera2.stopCamera failed: " + e);
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
}
