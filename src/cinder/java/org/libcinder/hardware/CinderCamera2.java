package org.libcinder.hardware;

import org.libcinder.app.Platform;

import android.annotation.TargetApi;
import android.content.Context;
import android.graphics.Point;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.ImageReader;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.util.Size;
import android.view.Surface;

import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.concurrent.Semaphore;

/** \class CinderCamera2
 *
 */
@TargetApi(21)
public class CinderCamera2 {
    private SurfaceTexture mDummyTexture = null;
    private Surface mDummySurface = null;

    private CameraManager mCameraManager = null;

    private String mFrontCameraId = null;
    private String mBackCameraId = null;

    private String mActiveCameraId = null;
    private CameraDevice mCamera = null;
    private CameraCaptureSession mSession = null;
    private CaptureRequest.Builder mBuilder = null;

    private int mWidth = 0;
    private int mHeight = 0;

    private HandlerThread mBackgroundThread;
    private Handler mBackgroundHandler;

    private ImageReader mImageReader;

    private Semaphore mCameraOpenCloseLock = new Semaphore(1);

    /** \extends CameraDevice.StateCallback
     *
     */
    private final CameraDevice.StateCallback mStateCallback = new CameraDevice.StateCallback() {
        @Override
        public void onOpened(CameraDevice camera) {
            mCameraOpenCloseLock.release();
            mCamera = camera;

            try {
                mBuilder = mCamera.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);

                mDummyTexture = new SurfaceTexture(0);
                mDummySurface = new Surface(mDummyTexture);
                mBuilder.addTarget(mDummySurface);
            }
            catch(Exception e ) {
                Log.e(Platform.TAG, "CameraDevice.StateCallback.onOpened failed: " + e);
            }
        }

        @Override
        public void onDisconnected(CameraDevice camera) {
            mCameraOpenCloseLock.release();
            camera.close();
            mCamera = null;
        }

        @Override
        public void onError(CameraDevice camera, int error) {
            mCameraOpenCloseLock.release();
            camera.close();
            mCamera = null;
        }
    };

    /** \func CinderCamera2
     *
     */
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

        Log.i(Platform.TAG, "Front camera: " + mFrontCameraId);
        Log.i(Platform.TAG, "Back camera: " + mBackCameraId);




        try {
            if(null != mFrontCameraId) {
                Log.i(Platform.TAG, "Front camera preview size: " + getOptimalPreviewSize(mFrontCameraId));
            }

            if(null != mBackCameraId) {
                Log.i(Platform.TAG, "Back camera preview size: " + getOptimalPreviewSize(mBackCameraId));
            }
        }
        catch(Exception e) {
        }
    }

    private Size getOptimalPreviewSize(String cameraId) {
        Size result = null;
        try {
            // Get the raw display size
            Point displaySize = Platform.getDisplaySize();
            int rawDisplayWidth = displaySize.x;
            int rawDisplayHeight = displaySize.y;
            int rawDisplayArea = rawDisplayWidth*rawDisplayHeight;
            float rawAspectRatio = (float)rawDisplayWidth/(float)rawDisplayHeight;
            if(rawAspectRatio < 1.0f) {
                int t = rawDisplayWidth;
                rawDisplayWidth = rawDisplayHeight;
                rawDisplayHeight = t;
                rawAspectRatio = (float)rawDisplayWidth/(float)rawDisplayHeight;
            }

            // Get a list of output sizes and sort it
            CameraCharacteristics info = mCameraManager.getCameraCharacteristics(cameraId);
            StreamConfigurationMap scm = info.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
            List<Size> sizes = Arrays.asList(scm.getOutputSizes(SurfaceTexture.class));
            Collections.sort(sizes, new Comparator<Size>() {
                @Override
                public int compare(Size a, Size b) {
                    int val = (a.getWidth()*a.getHeight()) - (b.getWidth()*b.getHeight());
                    return (val < 0) ? -1 : ((val > 0) ? 1 : 0);
                }
            });

            // Try to match up greatest area and aspect ratio
            for(Size size : sizes) {
                int width = size.getWidth();
                int height = size.getHeight();
                int area = width*height;
                float aspectRatio = (float)width/(float)height;
                if(aspectRatio < 1.0) {
                    int t = width;
                    width = height;
                    height = t;
                    aspectRatio = (float)width/(float)height;
                }

                float aspectRatioDiff = Math.abs(rawAspectRatio - aspectRatio);
                if((rawDisplayArea >= area) && (aspectRatioDiff < 0.001f)) {
                    result = size;
                }
            }

            // If we didn't find a size - use just greatest area
            if(null == result) {
                for(Size size : sizes) {
                    int width = size.getWidth();
                    int height = size.getHeight();
                    int area = width*height;

                    if(rawDisplayArea >= area) {
                        result = size;
                    }
                }
            }
        }
        catch(Exception e) {
            Log.e(Platform.TAG, "getOptimalPreviewSize failed: " + e);
        }
        return result;
    }

    /** \func startCamera
     *
     */
    public void startCamera() {
        // Bail if we don't have a valid camera id or mCamera isn't null
        if ((null == mActiveCameraId) || (null != mCamera)) {
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

    /** \func stopCamera
     *
     */
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
