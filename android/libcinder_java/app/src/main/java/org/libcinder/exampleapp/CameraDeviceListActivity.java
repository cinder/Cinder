package org.libcinder.exampleapp;

import android.annotation.TargetApi;
import android.app.Activity;
import android.content.Context;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.os.Build;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.util.Size;
import android.view.View;
import android.widget.Button;

import org.libcinder.exampleapp.widgets.DebugTextView;

import java.util.Arrays;
import java.util.List;

public class CameraDeviceListActivity extends Activity implements Button.OnClickListener {

    private static final String TAG = "CameraDeviceListActivity";

    private DebugTextView mLog;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera_devices_list);

        mLog = (DebugTextView)findViewById(R.id.textView);
        mLog.setMovementMethod(new ScrollingMovementMethod());

        Button btn = (Button)findViewById(R.id.button);
        btn.setOnClickListener(this);
    }

    @Override
    public void onClick(View view) {
        Log.i("CameraDevicesActivity", "clicked");

        if(Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            logCameraInfoV2();
        }
        else {
            logCameraInfoV1();
        }
    }

    private void logCameraInfoV1() {
        int numberOfCameras = Camera.getNumberOfCameras();
        for( int i = 0; i < numberOfCameras; ++i ) {
            mLog.appendLine("Camera " + i + ":");
            android.hardware.Camera.CameraInfo info = new android.hardware.Camera.CameraInfo();
            Camera.getCameraInfo(i, info);
            if(Camera.CameraInfo.CAMERA_FACING_FRONT == info.facing) {
                mLog.appendLine("facing: front", 1);
            }
            else if(Camera.CameraInfo.CAMERA_FACING_BACK == info.facing) {
                mLog.appendLine("facing: back", 1);
            }

            mLog.appendLine("canDisableShutterSound: " + info.canDisableShutterSound, 1);

            Camera cam = Camera.open(i);
            Camera.Parameters params = cam.getParameters();
            cam.release();

            mLog.appendLine("supported preview sizes:", 1);
            for(Camera.Size size : params.getSupportedPreviewSizes()) {
                mLog.appendLine(size.width + "x" + size.height, 2);
            }

            mLog.appendLine("supported formats:", 1);
        }
    }

    @TargetApi(21)
    private void logCameraInfoV2() {
        CameraManager mCameraManager = (CameraManager)getSystemService(Context.CAMERA_SERVICE);

        try {
            for (String cameraId : mCameraManager.getCameraIdList()) {
                mLog.appendLine("Camera " + cameraId + ":");

                CameraCharacteristics info = mCameraManager.getCameraCharacteristics(cameraId);
                int facing = info.get(CameraCharacteristics.LENS_FACING);
                if(CameraCharacteristics.LENS_FACING_FRONT == facing) {
                    mLog.appendLine("facing: front", 1);
                }
                else if(CameraCharacteristics.LENS_FACING_BACK == facing) {
                    mLog.appendLine("facing: back", 1);
                }

                mLog.appendLine("canDisableShutterSound: " + "n/a", 1);

                StreamConfigurationMap scm = info.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
                List<Size> sizes = Arrays.asList(scm.getOutputSizes(ImageFormat.YUV_420_888));
                for(Size size : sizes) {
                    mLog.appendLine(size.getWidth() + "x" + size.getHeight(), 2);
                }

                mLog.appendLine("supported formats:", 1);
                int[] formats = scm.getOutputFormats();
                for(int fmt : formats) {
                    String fmtStr = org.libcinder.hardware.Camera.imageFormatString(fmt);
                    if(null != fmtStr) {
                        mLog.appendLine(fmtStr, 2);
                    }
                    else {
                        mLog.appendLine("UNKNOWN FORMAT: " + Integer.toString(fmt), 2);
                    }
                }
            }
        }
        catch(Exception e ) {
            throw new UnsupportedOperationException("Failed getting camera: " + e);
        }
    }
}
