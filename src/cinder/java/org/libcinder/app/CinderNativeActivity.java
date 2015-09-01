package org.libcinder.app;

import android.Manifest;
import android.app.NativeActivity;
import android.app.WallpaperManager;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Point;
import android.graphics.SurfaceTexture;
import android.net.Uri;
import android.os.Bundle;
import android.os.ConditionVariable;
import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.Display;
import android.view.View;
import android.view.WindowManager;

import org.libcinder.Cinder;
import org.libcinder.hardware.Camera;

import java.io.File;
import java.io.InputStream;
import java.net.URLDecoder;
import java.security.spec.ECField;

public class CinderNativeActivity extends NativeActivity {

    private static final String TAG = "CinderNativeActivity";

    private static CinderNativeActivity sInstance = null;

    private Handler mHandler = null;

    private boolean mKeepScreenOn = false;
    private boolean mFullScreen = false;

    public static CinderNativeActivity getInstance() {
        return sInstance;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        sInstance = this;

        mHandler = new Handler(Looper.getMainLooper());

        Log.i(TAG, "onCreate | -------------- ");
    }

    @Override
    protected void onRestart() {
        super.onRestart();

        Log.i(TAG, "onRestart | -------------- ");
    }

    @Override
    protected void onStart() {
        super.onStart();

        Log.i(TAG, "onStart | -------------- ");

//        if( mKeepScreenOn ) {
//            getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
//            Log.i(TAG, "KEEPING SCREEN ON | -------------- ");
//        }
    }

    @Override
    protected void onResume() {
        super.onResume();

        Log.i(TAG, "onResume | -------------- ");
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);

        Log.i(TAG, "onWindowFocusChanged | -------------- ");

        if( mFullScreen && hasFocus ) {
            View decorView = getWindow().getDecorView();
            decorView.setSystemUiVisibility(
                    View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                            | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                            | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                            | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                            | View.SYSTEM_UI_FLAG_FULLSCREEN
                            | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
            );

            Log.i(TAG, "GOING IMMERSIVE | -------------- ");
        }
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
    // Misc
    // =============================================================================================

    public String getCacheDirectory() {
        String result = this.getExternalCacheDir().toString();
        return result;
    }

    public String getPicturesDirectory() {
        String result = Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_PICTURES).toString();
        return result;
    }

    public void setWallpaper(String path) {
        File file = new File(path);
        if(file.exists()) {
            try {
                Bitmap bitmap = BitmapFactory.decodeFile(path);
                WallpaperManager wm = WallpaperManager.getInstance(getApplicationContext());
                wm.setBitmap(bitmap);
            }
            catch(Exception e) {
                Log.e(TAG, "setWallpaper failed: " + e.getMessage());
            }
        }
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

    public void setKeepScreenOn( boolean keepScreenOn ) {
        mKeepScreenOn = keepScreenOn;

        Log.i(TAG, "setKeepScreenOn : keepScreenOn=" + keepScreenOn + " | -------------- ");
    }

    public void setFullScreen( boolean fullScreen ) {
        mFullScreen = fullScreen;

        Log.i(TAG, "setFullscreen : fullScreen=" + fullScreen + " | -------------- ");
    }

    // =============================================================================================
    // Actions
    // =============================================================================================

    public void launchWebBrowser(String url) {
        try {
            String decodedUrl = URLDecoder.decode(url, "UTF-8");
            Intent intent = new Intent(Intent.ACTION_VIEW, Uri.parse(decodedUrl));
            startActivity(intent);
        }
        catch(Exception e) {
            Log.e(TAG, "launchWebBrowser failed: " + e.getMessage());
        }
    }

    // NOTE: tempImagePath should be a png
    public void launchTwitter(String text, String tempImagePath) {
        Log.i(TAG, "launchTwitter: text=" + text + ", tempImagePath=" + tempImagePath);

        try {
            File tempImageFile = null;
            try {
                if(!tempImagePath.isEmpty()) {
                    tempImageFile = new File(tempImagePath);
                }
            }
            catch(Exception e) {
                tempImageFile = null;
                Log.w(TAG, "launchTwitter: couldn't load requested image");
            }

            Intent intent = new Intent(Intent.ACTION_SEND);
            intent.setType("/*");
            intent.setClassName("com.twitter.android", "com.twitter.android.composer.ComposerActivity");
            intent.putExtra(Intent.EXTRA_TEXT, text);
            if(null != tempImageFile) {
                intent.putExtra(Intent.EXTRA_STREAM, Uri.fromFile(tempImageFile));
                intent.setType("image/png");
            }
            startActivity(intent);
        }
        catch(final ActivityNotFoundException e) {
            Log.e(TAG, "Couldn't launch Twitter app (Twitter may have changed ComposerActivity...): " + e.getMessage());
        }
        catch(Exception e) {
            Log.e(TAG, "shareOnTwitterMsg failed: " + e.getMessage());
        }
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
            Log.i(TAG, "hardware_camera_enumerateDevices: Found " + result.length + " devices");
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
        Log.i(TAG, "hardware_camera_stopCapture: mHandler=" + mHandler);

        if(null == mCamera) {
            return;
        }

        mCamera.stopCapture();
        mCamera = null;

        /*
        if(1 != Thread.currentThread().getId()) {
            try {
                Log.i(TAG, "hardware_camera_stopCapture: Mark 0.0");
                final ConditionVariable condition = new ConditionVariable(false);
                Log.i(TAG, "hardware_camera_stopCapture: Mark 0.1");
                mHandler.post(new Runnable() {
                    @Override
                    public void run() {
                        Log.i(TAG, "hardware_camera_stopCapture: Mark 0.5");
                        mCamera.stopCapture();
                        condition.open();
                    }
                });
                Log.i(TAG, "hardware_camera_stopCapture: Mark 0.6");
                condition.block();
                Log.i(TAG, "hardware_camera_stopCapture: Mark 0.7");
            }
            catch(Exception e) {
                Log.e(TAG, "hardware_camera_stopCapture failed: " + e.getMessage());
            }
        }
        else {
            Log.i(TAG, "hardware_camera_stopCapture: Mark 1.0");
            mCamera.stopCapture();
        }

        Log.i(TAG, "hardware_camera_stopCapture: Mark 2.0");
        mCamera = null;
        */
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