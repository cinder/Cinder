package org.libcinder.app;

import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.app.NativeActivity;
import android.content.Context;
import android.graphics.Point;
import android.os.Build;
import android.os.Bundle;
import android.view.Display;
import android.view.WindowManager;

import org.libcinder.hardware.Camera;
import org.libcinder.hardware.CameraV1;
import org.libcinder.hardware.CameraV2;

public class CinderNativeActivity extends NativeActivity {

    private static CinderNativeActivity sInstance = null;

    private Camera mCamera = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        sInstance = this;

        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            mCamera = new CameraV2();
        }
        else {
            mCamera = new CameraV1();
        }

        if(mCamera.isBackCameraAvailable() || mCamera.isFrontCameraAvailable()) {
            FragmentManager fm = getFragmentManager();
            FragmentTransaction ft = fm.beginTransaction();
            ft.add(mCamera, "cinder-camera-fragment").commit();
        }
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

    public Display getDefaultDisplay() {
        WindowManager wm = (WindowManager)sInstance.getSystemService(Context.WINDOW_SERVICE);
        Display result = wm.getDefaultDisplay();
        return result;
    }

    public Point getDefaultDisplaySize() {
        Display defaultDisplay = this.getDefaultDisplay();
        Point result = new Point();
        defaultDisplay.getRealSize(result);
        return result;
    }

    public boolean isBackCameraAvailable() {
        return ((null != mCamera) && mCamera.isFrontCameraAvailable());
    }

    public boolean isFrontCameraAvailable() {
        return ((null != mCamera) && mCamera.isFrontCameraAvailable());
    }

    public boolean isInternetAvailable() {
        return false;
    }
}