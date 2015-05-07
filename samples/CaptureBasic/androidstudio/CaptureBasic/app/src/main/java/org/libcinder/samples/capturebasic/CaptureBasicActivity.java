package org.libcinder.samples.capturebasic;

import android.app.Fragment;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.app.NativeActivity;
import android.os.Build;
import android.os.Bundle;

import org.libcinder.app.CinderNativeActivity;
//import org.libcinder.app.ComponentManager;
import org.libcinder.hardware.Camera;

//public class CaptureBasicActivity extends NativeActivity {
public class CaptureBasicActivity extends CinderNativeActivity {
    static final String TAG = "CaptureBasicActivity";

    //private ComponentManager mComponentManager;
    private Camera mCamera;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        /*
        FragmentManager fragmentManager = getFragmentManager();
        Fragment fragment = fragmentManager.findFragmentByTag(ComponentManager.FRAGMENT_TAG);
        if(null == fragment) {
            mComponentManager = new ComponentManager();
            FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
            fragmentTransaction.add(mComponentManager, ComponentManager.FRAGMENT_TAG);
            fragmentTransaction.commit();
        }
        else {
            mComponentManager = (ComponentManager)fragment;
        }
        */
    }

    @Override
    protected void onStart() {
        super.onStart();
        //mCamera = mComponentManager.getCamera(Build.VERSION_CODES.KITKAT);
        //mCamera.startCapture("0");
    }
}
