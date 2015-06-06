package org.libcinder.app;

import android.Manifest;
import android.app.Activity;
import android.app.Fragment;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.app.NativeActivity;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.Point;
import android.graphics.SurfaceTexture;
import android.os.Build;
import android.os.Bundle;
import android.os.ConditionVariable;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.Display;
import android.view.Surface;
import android.view.WindowManager;

import org.libcinder.hardware.Camera;

public class CinderNativeActivity extends NativeActivity {

    private static final String TAG = "CinderNativeActivity";

    private static CinderNativeActivity sInstance = null;

    private Handler mHandler = null;

    public static CinderNativeActivity getInstance() {
        return sInstance;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        sInstance = this;

        mHandler = new Handler(Looper.getMainLooper());
    }

    @Override
    protected void onRestart() {
        super.onRestart();
    }

    @Override
    protected void onStart() {
        super.onStart();
    }

    @Override
    protected void onResume() {
        super.onResume();
    }

    @Override
    protected void onStop() {
        super.onStop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    // =============================================================================================
    // Permission
    // =============================================================================================

    /** \class Permissions
     *
     */
    public class Permissions {

        /** \class Missing
         *
         */
        public class Missing {

            private String msg(String ident) {
                return "Permission denied (maybe missing " + ident + " permission)";
            }

            public String CAMERA()                  { return msg(Manifest.permission.CAMERA); }
            public String INTERNET()                { return msg(Manifest.permission.INTERNET); }
            public String WRITE_EXTERNAL_STORAGE()  { return msg(Manifest.permission.WRITE_EXTERNAL_STORAGE); }
        }

        private Missing mMissing = new Missing();

        public Missing missing() {
            return mMissing;
        }

        private boolean check(String ident) {
            return (checkCallingOrSelfPermission(ident) == PackageManager.PERMISSION_GRANTED);
        }

        public boolean CAMERA()                 { return check(Manifest.permission.CAMERA); }
        public boolean INTERNET()               { return check(Manifest.permission.INTERNET); }
        public boolean WRITE_EXTERNAL_STORAGE() { return check(Manifest.permission.WRITE_EXTERNAL_STORAGE); }

    }

    private Permissions mPermssions = new Permissions();

    public static Permissions permissions() {
        return sInstance.mPermssions;
    }

    // =============================================================================================
    // Display
    // =============================================================================================

    public Display getDefaultDisplay() {
        Display result = null;
        WindowManager wm = (WindowManager)getSystemService(Context.WINDOW_SERVICE);
        result = wm.getDefaultDisplay();
        return result;
    }

    public Point getDefaultDisplaySize() {
        Display defaultDisplay = this.getDefaultDisplay();
        Point result = new Point();
        defaultDisplay.getRealSize(result);
        return result;
    }

    public int getDisplayRotation() {
        int result = getDefaultDisplay().getRotation();
        return result;
    }

    // =============================================================================================
    // Camera
    // =============================================================================================

    private Camera mCamera;

    /**
     * hardware_camera_initialize
     *
     */
    public void hardware_camera_initialize(final int apiLevel) {
        Log.i(TAG, "hardware_camera_initialize");

        if(null != mCamera) {
            return;
        }

        if(1 != Thread.currentThread().getId()) {
            final ConditionVariable condition = new ConditionVariable(false);
            final CinderNativeActivity activity = this;
            mHandler.post(new Runnable() {
                @Override
                public void run() {
                    //mCamera = Camera.create(Build.VERSION_CODES.KITKAT, activity);
                    mCamera = Camera.create(apiLevel, activity);
                    mCamera.initialize();
                    condition.open();
                }
            });
            condition.block();
        }
        else {
            mCamera = Camera.create(apiLevel, this);
        }
    }

    /**
     * hardware_camera_enumerateDevices
     *
     */
    public Camera.DeviceInfo[] hardware_camera_enumerateDevices() {
        Log.i(TAG, "hardware_camera_enumerateDevices");

        Camera.DeviceInfo[] result = null;

        if(null != mCamera) {
            result = mCamera.enumerateDevices();
        }

        return result;
    }

    /**
     * hardware_camera_startCapture
     *
     */
    public void hardware_camera_startCapture(final String deviceId, final int width, final int height) {
    //public void hardware_camera_startCapture() {
        Log.i(TAG, "hardware_camera_startCapture");

        //if(null == mCamera) {
        //    return;
        //}

        if(1 != Thread.currentThread().getId()) {
            final ConditionVariable condition = new ConditionVariable(false);
            final CinderNativeActivity activity = this;
            mHandler.post(new Runnable() {
                @Override
                public void run() {
                    //mCamera = Camera.create(Build.VERSION_CODES.KITKAT, activity);
                    mCamera.startCapture(deviceId, width, height);
                    condition.open();
                }
            });
            condition.block();
        }
        else {
            mCamera.startCapture(deviceId, width, height);
        }
    }

    /**
     * hardware_camera_stopCapture
     *
     */
    public void hardware_camera_stopCapture() {
        Log.i(TAG, "hardware_camera_stopCapture");

        if(null == mCamera) {
            return;
        }

        if(1 != Thread.currentThread().getId()) {
            final ConditionVariable condition = new ConditionVariable(false);
            mHandler.post(new Runnable() {
                @Override
                public void run() {
                    mCamera.stopCapture();
                    condition.open();
                }
            });
            condition.block();
        }
        else {
            mCamera.stopCapture();
        }

        mCamera = null;
    }

    /**
     * hardware_camera_lockPixels
     *
     */
    public byte[] hardware_camera_lockPixels() {
        //Log.i(TAG, "hardware_camera_lockPixels");

        byte[] result = null;

        if(null != mCamera) {
            result = mCamera.lockPixels();
        }

        return result;
    }

    /**
     * hardware_camera_unlockPixels
     *
     */
    public void hardware_camera_unlockPixels() {
        //Log.i(TAG, "hardware_camera_unlockPixels");

        if(null != mCamera) {
            mCamera.unlockPixels();
        }
    }

    /**
     * hardware_camera_isNewFrameAvailable
     *
     */
    public boolean hardware_camera_isNewFrameAvailable() {
        //Log.i(TAG, "hardware_camera_isNewFrameAvailable");

        return (null != mCamera) && (mCamera.isNewFrameAvailable());
    }

    /**
     * hardware_camera_clearNewFrameAvailable
     *
     */
    public void hardware_camera_clearNewFrameAvailable() {
        //Log.i(TAG, "hardware_camera_clearNewFrameAvailable");

        if(null != mCamera) {
            mCamera.clearNewFrameAvailable();
        }
    }

    /**
     * hardware_camera_initPreviewTexture
     *
     */
    public void hardware_camera_initPreviewTexture(int texName) {
        Log.i(TAG, "hardware_camera_initPreviewTexture: " + texName);

        if(null != mCamera) {
            try {
                SurfaceTexture previewTexture = new SurfaceTexture(texName);
                mCamera.setPreviewTexture(previewTexture);
            }
            catch(Exception e) {
                Log.e(TAG, "hardware_camera_initPreviewTexture error: " + e.getMessage());
            }
        }
    }

    /**
     * hardware_camera_updateTexImage
     *
     */
    public void hardware_camera_updateTexImage() {
        //Log.i(TAG, "hardware_camera_updateTexImage");

        if ((null != mCamera) && (null != mCamera.getPreviewTexture())) {
            mCamera.getPreviewTexture().updateTexImage();
        }

        /*
        try {
            if ((null != mCamera) && (null != mCamera.getPreviewTexture())) {
                mCamera.getPreviewTexture().updateTexImage();
            }
        }
        catch(Exception e) {
            Log.e(TAG, "hardware_camera_updateTexImage error:" + e.getMessage());
        }
        */
    }
}