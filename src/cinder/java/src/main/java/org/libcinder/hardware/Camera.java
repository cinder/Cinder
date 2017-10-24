package org.libcinder.hardware;

import android.app.Activity;
import android.graphics.ImageFormat;
import android.graphics.Matrix;
import android.graphics.SurfaceTexture;
import android.hardware.SensorManager;
import android.os.Build;
import android.util.Log;
import android.view.OrientationEventListener;

import org.libcinder.app.CinderNativeActivity;

import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.locks.ReentrantLock;

/** \class Camera
 *
 *
 */
public abstract class Camera {

    private static final String TAG = "cinder|Camera";

    public interface DisplayLayoutListener {
        void onDisplayLayoutChanged(int width, int height, int orientation, int displayRotation);
    }

    private CinderNativeActivity mActivity = null;
    private boolean mInitialized = false;

    protected String mBackDeviceId = null;
    protected String mFrontDeviceId = null;
    protected String mActiveDeviceId = null;

    private static String sLastDeviceId = null;

    protected int mPreferredPreviewWidth = 0;
    protected int mPreferredPreviewHeight = 0;
    private int mWidth = 0;
    private int mHeight = 0;
    protected byte[] mPixels = null;
    protected ReentrantLock mPixelsMutex = null;

    protected OrientationEventListener mOrientationListener = null;
    protected int mOrientation = -1;
    protected int mDisplayRotation = -1;
    private DisplayLayoutListener mDisplayLayoutListener = null;

    protected SurfaceTexture mPreviewTexture = null;

    protected AtomicBoolean mNewFrameAvailable = new AtomicBoolean(false);

    /** \class DeviceInfo
     *
     *
     */
    public class DeviceInfo {
        public String id;
        public boolean frontFacing = false;
        public int[] resolutions;

        DeviceInfo(String id, boolean frontFacing, int[] resolutions) {
            this.id = id;
            this.frontFacing = frontFacing;
            this.resolutions = resolutions;
        }
    }

    protected DeviceInfo[] mCachedDeviceInfos;

    /**
     * If we're in Java, we might use a TextureView to draw the
     * preview. This matrix is used by CameraV2 to rotate and
     * scale the preview so it looks correct. CameraV1 ignores it.
     *
     */
    protected Matrix mPreviewTransform = new Matrix();

    /**
     * Camera
     *
     */
    public Camera(CinderNativeActivity activity) {
        mActivity = activity;
    }

    /**
     * getActivity
     *
     */
    public CinderNativeActivity getActivity() {
        return mActivity;
    }

    /**
     * create
     *
     */
    public static Camera create(int apiLevel, CinderNativeActivity activity) {
        Camera result = null;
        if (apiLevel >= Build.VERSION_CODES.LOLLIPOP) {
            result = new CameraV2(activity);
        } else {
            result = new CameraV1(activity);
        }
        return result;
    }

    /**
     * checkCameraPresence
     *
     */
    public static void checkCameraPresence(boolean[] back, boolean[] front) {
        Log.i(TAG, "checkCameraPresence: ThreadID=" + Thread.currentThread().getId());

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            CameraV2.checkCameraPresence(back, front);
        } else {
            CameraV1.checkCameraPresence(back, front);
        }
    }

    /**
     * enumerateDevices
     *
     */
    public abstract DeviceInfo[] enumerateDevices();

    /**
     * imageFormatString
     *
     */
    public static String imageFormatString(int imageFormat) {
        String result = null;
        switch (imageFormat) {
            case ImageFormat.RGB_565:
                result = "RGB_565";
                break;
            case ImageFormat.NV16:
                result = "NV16";
                break;
            case ImageFormat.YUY2:
                result = "YUY2";
                break;
            case ImageFormat.YV12:
                result = "YV12";
                break;
            case ImageFormat.JPEG:
                result = "JPEG";
                break;
            case ImageFormat.NV21:
                result = "NV21";
                break;
            case ImageFormat.YUV_420_888:
                result = "YUV_420_888";
                break;
            case ImageFormat.RAW_SENSOR:
                result = "RAW_SENSOR";
                break;
            case ImageFormat.RAW10:
                result = "RAW10";
                break;
        }
        return result;
    }

    /**
     * initialize
     *
     */
    public void initialize() {
        Log.i(TAG, "intialize: ThreadId=" + Thread.currentThread().getId());

        if (!mInitialized) {
            initializeImpl();
            mInitialized = true;
        }

        // Set these in case we're coming back from an activity restart or sleep.
        mOrientation = getActivity().getResources().getConfiguration().orientation;
        mDisplayRotation = getActivity().getDefaultDisplay().getRotation();
    }

    /**
     * setPreferredPreviewSize
     *
     */
    protected void setPreferredPreviewSize(int width, int height) {
        mPreferredPreviewWidth = width;
        mPreferredPreviewHeight = height;
        mWidth = mPreferredPreviewWidth;
        mHeight = mPreferredPreviewHeight;

        if (null == mOrientationListener) {
            startOrientationListener();
        }

        emitDisplayLayoutChanged();
        Log.i(TAG, "Camera.setPreferredPreviewSize: " + mWidth + "x" + mHeight);
    }

    public void setPreviewTexture(SurfaceTexture previewTexture) {
        setPreviewTextureImpl(previewTexture);
        emitDisplayLayoutChanged();
    }

    public SurfaceTexture getPreviewTexture() {
        return mPreviewTexture;
    }

    /**
     * startOrientationListener
     *
     */
    protected void startOrientationListener() {
        // Initialize orientation listener
        if (null == mOrientationListener) {
            mOrientationListener = new OrientationEventListener(getActivity(), SensorManager.SENSOR_DELAY_NORMAL) {
                @Override
                public void onOrientationChanged(int so) {
                    int orientation = getActivity().getResources().getConfiguration().orientation;
                    int rotation = getActivity().getDefaultDisplay().getRotation();
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

        if (mOrientationListener.canDetectOrientation()) {
            mOrientationListener.enable();
        }
    }

    /**
     * stopOrientationListener
     *
     */
    protected void stopOrientationListener() {
        if (null != mOrientationListener) {
            mOrientationListener.disable();
            mOrientationListener = null;
        }
    }

    /**
     * startSession
     *
     */
    public void startSession(String deviceId) {
        Log.i(TAG, "startSession: " + deviceId);

        if (null != mActiveDeviceId) {
            stopSession();
        }

        startSessionImpl(deviceId);
        Camera.sLastDeviceId = mActiveDeviceId;
    }

    /**
     * startSession
     *
     */
    public void startSession() {
        startSession(Camera.sLastDeviceId);
    }

    /**
     * stopSession
     *
     */
    public void stopSession() {
        Log.i(TAG, "Camera.stopSession");
        stopSessionImpl();
        stopOrientationListener();
    }

    /**
     * startCapture
     *
     */
    public void startCapture(String deviceId, int width, int height) {
        Log.i(TAG, "startCapture: " + deviceId);

        mPreferredPreviewWidth = width;
        mPreferredPreviewHeight = height;

        if (! mInitialized) {
            initialize();
        }

        if (null == mPixelsMutex) {
            mPixelsMutex = new ReentrantLock();
        }

        startCaptureImpl(deviceId);
        startSession(deviceId);
    }

//    /**
//     * startCapture
//     */
//    public void startCapture() {
//        Log.i(TAG, "startCapture");
//
//        if (!mInitialized) {
//            initialize();
//        }
//
//        String deviceId = Camera.sLastDeviceId;
//        if (null == deviceId) {
//            deviceId = (null != mBackDeviceId) ? mBackDeviceId : ((null != mFrontDeviceId) ? mFrontDeviceId : null);
//        }
//
//        startCapture(deviceId);
//    }

    /**
     * stopCapture
     *
     * Stops all threads that are used for capturing. Closes cameras and releases the devices
     * associated with the. Sets all classes member variables to null or their default value.
     *
     */
    public void stopCapture() {
        Log.i(TAG, "Camera.stopCapture");
        stopSession();
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

    /*
    public void switchToBackCamera() {
        switchToBackCameraImpl();
        Camera.sLastDeviceId = mActiveDeviceId;
    }

    public void switchToFrontCamera() {
        switchToFrontCameraImpl();
        Camera.sLastDeviceId = mActiveDeviceId;
    }
    */

    protected abstract void initializeImpl();

    protected abstract void setPreviewTextureImpl(SurfaceTexture previewTexture);

    protected abstract void startSessionImpl(String deviceId);

    protected abstract void stopSessionImpl();

    protected abstract void startCaptureImpl(String deviceId);

    protected abstract void stopCaptureImpl();

    //protected abstract void switchToBackCameraImpl();

    //protected abstract void switchToFrontCameraImpl();

    public abstract byte[] lockPixels();

    public abstract void unlockPixels();

    public boolean isNewFrameAvailable() {
        boolean result = mNewFrameAvailable.get();
        return result;
    }

    public void clearNewFrameAvailable() {
        mNewFrameAvailable.set(false);
    }

    /**
     * setDisplayOrientation
     *
     */
    protected void setDisplayOrientation(int displayRotation) {
        // CameraV1 overrides this
    }

    /**
     * setDisplayLayoutListener
     *
     */
    public void setDisplayLayoutListener(DisplayLayoutListener listener) {
        mDisplayLayoutListener = listener;
    }

    /**
     * emitDisplayLayoutChanged
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

        if (null != mDisplayLayoutListener) {
            mDisplayLayoutListener.onDisplayLayoutChanged(mWidth, mHeight, mOrientation, mDisplayRotation);
        }

    }

    /**
     * getWidth
     *
     */
    public int getWidth() {
        return mWidth;
    }

    /**
     * getHeight
     *
     */
    public int getHeight() {
        return mHeight;
    }

    /**
     * updatePreviewTransform
     *
     */
    public void updatePreviewTransform(int viewWidth, int viewHeight, int orientation, int displayRotation) {
        // CameraV2 overrides this.
    }

    /**
     * getPreviewTransform
     *
     */
    public Matrix getPreviewTransform() {
        return mPreviewTransform;
    }

    /**
     * isBackCameraAvailable
     *
     */
    public boolean isBackCameraAvailable() {
        return (null != mBackDeviceId);
    }

    /**
     * isFrontCameraAvailable
     *
     */
    public boolean isFrontCameraAvailable() {
        return (null != mFrontDeviceId);
    }

    /**
     * isBackCameraActive
     *
     */
    public boolean isBackCameraActive() {
        return ((null != mBackDeviceId) && (mActiveDeviceId.equals(mBackDeviceId)));
    }

    /**
     * isFrontCameraActive
     *
     */
    public boolean isFrontCameraActive() {
        return ((null != mFrontDeviceId) && (mActiveDeviceId.equals(mFrontDeviceId)));
    }

    /**
     * toggleActiveCamera
     *
     */
    public void toggleActiveCamera() {
        if (isBackCameraActive() && isFrontCameraAvailable()) {
            startCapture(mFrontDeviceId, mPreferredPreviewWidth, mPreferredPreviewHeight);
        } else if (isFrontCameraActive() && isBackCameraAvailable()) {
            startCapture(mBackDeviceId, mPreferredPreviewWidth, mPreferredPreviewHeight);
        }
    }
}
