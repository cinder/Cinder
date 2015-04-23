package org.libcinder.hardware;

import android.app.Activity;
import android.app.Fragment;
import android.os.Build;
import android.os.Bundle;

public abstract class Camera extends Fragment {

    protected int mWidth = 0;
    protected int mHeight = 0;
    protected byte[] mPixels = null;

    public Camera() {

    }

    public static Camera create() {
        Camera result = null;
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            result = new CameraV2();
        }
        else {
            result = new CameraV1();
        }
        return result;
    }

    public static void checkCameraPresence(boolean[] back, boolean[] front) {
        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            CameraV2.checkCameraPresence(back, front);
        }
        else {
            CameraV1.checkCameraPresence(back, front);
        }
    }

    public int getWidth() {
        return mWidth;
    }

    public int getHeight() {
        return mHeight;
    }

    protected abstract void initialize();

    public abstract boolean isBackCameraAvailable();

    public abstract boolean isFrontCameraAvailable();

    public abstract void startCapture();

    public abstract void stopCapture();

    public abstract void switchToBackCamera();

    public abstract void switchToFrontCamera();

    public abstract byte[] lockPixels();

    public abstract void unlockPixels();

    /** onAttach
     *
     */
    @Override
    public void onAttach(Activity activity) {
        super.onAttach(activity);
        initialize();
    }

    /** onCreate
     *
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    /** onActivityCreated
     *
     */
    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
    }

    /** onStart
     *
     */
    @Override
    public void onStart() {
        super.onStart();
    }

    /** onResume
     *
     */
    @Override
    public void onResume() {
        super.onResume();
    }

    /** onPause
     *
     */
    @Override
    public void onPause() {
        super.onPause();
    }

    /** onStop
     *
     */
    @Override
    public void onStop() {
        super.onStop();

        stopCapture();
    }

    /** onDestroy
     *
     */
    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    /** onDetach
     *
     */
    @Override
    public void onDetach() {
        super.onDestroy();
    }
}
