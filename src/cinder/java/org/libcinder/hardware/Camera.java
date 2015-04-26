package org.libcinder.hardware;

import android.app.Activity;
import android.app.Fragment;
import android.content.res.Configuration;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.graphics.SurfaceTexture;
import android.hardware.SensorManager;
import android.os.Build;
import android.os.Bundle;
import android.util.Log;
import android.view.OrientationEventListener;

import org.libcinder.app.ModulesFragment;

import java.util.concurrent.locks.ReentrantLock;

public abstract class Camera extends Fragment {

    private static final String TAG = "Camera";
    public static final String FRAGMENT_TAG = "fragment:org.libcinder.hardware.Camera";

    public interface DisplayLayoutListener {
        void onDisplayLayoutChanged(int width, int height, int orientation, int displayRotation);
    }

    private boolean mInitialized = false;

    protected String mBackDeviceId   = null;
    protected String mFrontDeviceId  = null;
    protected String mActiveDeviceId = null;

    private static String sLastDeviceId = null;

    private int mPreferredPreviewWidth = 0;
    private int mPreferredPreviewHeight = 0;
    private int mWidth = 0;
    private int mHeight = 0;
    protected byte[] mPixels = null;
    protected ReentrantLock mPixelsMutex  = null;

    protected OrientationEventListener mOrientationListener;
    protected int mOrientation = -1;
    protected int mDisplayRotation = -1;
    private DisplayLayoutListener mDisplayLayoutListener;

    protected SurfaceTexture mPreviewTexture = null;


    /**
     * If we're in Java, we might use a TextureView to draw the
     * preview. This matrix is used by CameraV2 to rotate and
     * scale the preview so it looks correct. CameraV1 ignores it.
     *
     */
    protected Matrix mPreviewTransform = new Matrix();

    /** Camera
     *
     */
    public Camera() {
        // @TODO
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

    public static Camera create(int version) {
        Camera result = null;
        if((2 == version) && (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP)) {
            result = new CameraV2();
        }
        else if(1 == version) {
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

    public static String imageFormatString(int imageFormat) {
        String result = null;
        switch(imageFormat) {
            case ImageFormat.RGB_565     : result = "RGB_565";     break;
            case ImageFormat.NV16        : result = "NV16";        break;
            case ImageFormat.YUY2        : result = "YUY2";        break;
            case ImageFormat.YV12        : result = "YV12";        break;
            case ImageFormat.JPEG        : result = "JPEG";        break;
            case ImageFormat.NV21        : result = "NV21";        break;
            case ImageFormat.YUV_420_888 : result = "YUV_420_888"; break;
            case ImageFormat.RAW_SENSOR  : result = "RAW_SENSOR";  break;
            case ImageFormat.RAW10       : result = "RAW10";       break;
        }
        return result;
    }

    public void initialize() {
        if( ! mInitialized) {
            initializeImpl();
            mInitialized = true;
        }

        // Set these in case we're coming back from an activity restart or sleep.
        mOrientation = ModulesFragment.activity().getResources().getConfiguration().orientation;
        mDisplayRotation = ModulesFragment.get().getDefaultDisplay().getRotation();
    }

    protected void setPreferredPreviewSize(int width, int height) {
        mPreferredPreviewWidth = width;
        mPreferredPreviewHeight = height;
        mWidth = mPreferredPreviewWidth;
        mHeight = mPreferredPreviewHeight;

        if(null == mOrientationListener) {
            startOrientationListener();
        }

        emitDisplayLayoutChanged();
        //Log.i(TAG, "Camaera.setPreferredPreviewSize: " + mWidth + "x" + mHeight);
    }

    public void setPreviewTexture(SurfaceTexture previewTexture) {
        setPreviewTextureImpl(previewTexture);
        emitDisplayLayoutChanged();
    }

    public SurfaceTexture getPreviewTexture() {
        return mPreviewTexture;
    }

    protected void startOrientationListener() {
        // Initialize orientation listener
        if(null == mOrientationListener) {
            mOrientationListener = new OrientationEventListener(ModulesFragment.activity(), SensorManager.SENSOR_DELAY_NORMAL) {
                @Override
                public void onOrientationChanged(int so) {
                    int orientation = ModulesFragment.activity().getResources().getConfiguration().orientation;
                    int rotation = ModulesFragment.get().getDefaultDisplay().getRotation();
                    if ((orientation == mOrientation) && (rotation == mDisplayRotation)) {
                        return;
                    }

                    Log.i(TAG, "onOrientationChanged: " + orientation + ", " + rotation);

                    mOrientation = orientation;
                    mDisplayRotation = rotation;
                    emitDisplayLayoutChanged();
                }
            };
        }

        if(mOrientationListener.canDetectOrientation()) {
            mOrientationListener.enable();
        }
    }

    protected void stopOrientationListener() {
        if(null != mOrientationListener) {
            mOrientationListener.disable();
            mOrientationListener = null;
        }
    }

    /** startSession
     *
     */
    public void startSession() {
        startSessionImpl(Camera.sLastDeviceId);
        Camera.sLastDeviceId = mActiveDeviceId;
    }

    /** stopSession
     *
     */
    public void stopSession() {
        stopSessionImpl();
        stopOrientationListener();
    }

    /** startCapture
     *
     */
    public void startCapture() {
        if(! mInitialized) {
            initialize();
        }

        if(null == mPixelsMutex) {
            mPixelsMutex = new ReentrantLock();
        }

        startCaptureImpl();
    }

    /** stopCapture
     *
     * Stops all threads that are used for capturing. Closes cameras and releases the devices
     * associated with the. Sets all classes member variables to null or their default value.
     *
     */
    public void stopCapture() {
        stopCaptureImpl();
        mInitialized = false;
        mActiveDeviceId = null;
        mBackDeviceId = null;
        mFrontDeviceId = null;
        mPreferredPreviewWidth = 0;
        mPreferredPreviewHeight = 0;
        mWidth = 0;
        mHeight = 0;
        mPixels = null;
        mPixelsMutex = null;
        mOrientation = -1;
        mDisplayRotation = -1;
        mDisplayLayoutListener = null;
        mPreviewTexture = null;
    }

    public void switchToBackCamera() {
        switchToBackCameraImpl();
        Camera.sLastDeviceId = mActiveDeviceId;
    }

    public void switchToFrontCamera() {
        switchToFrontCameraImpl();
        Camera.sLastDeviceId = mActiveDeviceId;
    }

    protected abstract void initializeImpl();

    protected abstract void setPreviewTextureImpl(SurfaceTexture previewTexture);

    protected abstract void startSessionImpl(String deviceId);

    protected abstract void stopSessionImpl();

    protected abstract void startCaptureImpl();

    protected abstract void stopCaptureImpl();

    protected abstract void switchToBackCameraImpl();

    protected abstract void switchToFrontCameraImpl();

    public abstract byte[] lockPixels();

    public abstract void unlockPixels();

    /** setDisplayOrientation
     *
     */
    protected void setDisplayOrientation(int displayRotation) {
        // CameraV1 overrides this
    }

    /** setDisplayLayoutListener
     *
     */
    public void setDisplayLayoutListener(DisplayLayoutListener listener) {
        mDisplayLayoutListener = listener;
    }

    /** emitDisplayLayoutChanged
     *
     */
    private void emitDisplayLayoutChanged() {
        Log.i(TAG, "Camera.emitDisplayLayoutChanged: orientation=" + mOrientation + ", displayRotation=" + mDisplayRotation);

        /*
        if(Configuration.ORIENTATION_PORTRAIT == mOrientation) {
            mWidth = mPreferredPreviewWidth;
            mHeight = mPreferredPreviewHeight;
        }
        else if(Configuration.ORIENTATION_LANDSCAPE == mOrientation) {
            mWidth = mPreferredPreviewHeight;
            mHeight = mPreferredPreviewWidth;
        }
        */

        setDisplayOrientation(mDisplayRotation);

        if(null != mDisplayLayoutListener) {
            mDisplayLayoutListener.onDisplayLayoutChanged(mWidth, mHeight, mOrientation, mDisplayRotation);
        }

    }

    /** getWidth
     *
     */
    public int getWidth() {
        return mWidth;
    }

    /** getHeight
     *
     */
    public int getHeight() {
        return mHeight;
    }

    /** updatePreviewTransform
     *
     */
    public void updatePreviewTransform(int viewWidth, int viewHeight, int orientation, int displayRotation) {
        // CameraV2 overrides this.
    }

    /** getPreviewTransform
     *
     */
    public Matrix getPreviewTransform() {
        return mPreviewTransform;
    }

    /** isBackCameraAvailable
     *
     */
    public boolean isBackCameraAvailable() {
        return (null != mBackDeviceId);
    }

    /** isFrontCameraAvailable
     *
     */
    public boolean isFrontCameraAvailable() {
        return (null != mFrontDeviceId);
    }

    /** isBackCameraActive
     *
     */
    public boolean isBackCameraActive() {
        return ((null != mBackDeviceId) && (mActiveDeviceId.equals(mBackDeviceId)));
    }

    /** isFrontCameraActive
     *
     */
    public boolean isFrontCameraActive() {
        return ((null != mFrontDeviceId) && (mActiveDeviceId.equals(mFrontDeviceId)));
    }

    /** toggleActiveCamera
     *
     */
    public void toggleActiveCamera() {
        if(isBackCameraActive() && isFrontCameraAvailable()) {
            switchToFrontCamera();
        }
        else if(isFrontCameraActive() && isBackCameraAvailable()) {
            switchToBackCamera();;
        }
    }

    /** onAttach
     *
     */
    @Override
    public void onAttach(Activity activity) {
        super.onAttach(activity);
        initializeImpl();
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

    /** onSaveInstanceState
     *
     */
    public void onSaveInstanceState(Bundle outState) {
        super.onSaveInstanceState(outState);
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
