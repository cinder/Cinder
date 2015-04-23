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

    private void initializeCamera() {
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
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        sInstance = this;
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

    public static CinderNativeActivity getActivity() {
        return sInstance;
    }

    public static void setActivity(CinderNativeActivity activity) {
        if(null == sInstance) {
            sInstance = activity;
        }
    }
}