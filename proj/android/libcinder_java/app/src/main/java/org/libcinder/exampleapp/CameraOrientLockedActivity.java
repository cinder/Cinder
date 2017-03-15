package org.libcinder.exampleapp;

import android.app.Activity;
import android.app.Fragment;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.graphics.Matrix;
import android.graphics.SurfaceTexture;
import android.os.Build;
import android.os.Handler;
import android.os.Looper;
import android.os.Bundle;
import android.util.Log;
import android.view.TextureView;
import android.view.View;
import android.widget.Button;

import org.libcinder.hardware.Camera;

import java.sql.BatchUpdateException;
import java.util.Set;

public class CameraOrientLockedActivity extends Activity implements TextureView.SurfaceTextureListener, Button.OnClickListener, Camera.DisplayLayoutListener {

    private static final String TAG = "CameraOrientLockedActivity";

    //private ComponentManager mComponentManager;

    private Camera mCamera;

    private TextureView mTextureView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera_preview);

        /*
        FragmentManager fragmentManager = getFragmentManager();
        Fragment fragment = fragmentManager.findFragmentByTag(ComponentManager.FRAGMENT_TAG);
        if(null == fragment) {
            Log.i(TAG, "ModulesFragment not found - creating");
            mComponentManager = new ComponentManager();
            FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
            fragmentTransaction.add(mComponentManager, ComponentManager.FRAGMENT_TAG);
            fragmentTransaction.commit();
        }
        else {
            Log.i(TAG, "ModulesFragment found - casting");
            mComponentManager = (ComponentManager)fragment;
        }
        */

        //mCamera = mComponentManager.getCamera(Build.VERSION_CODES.KITKAT);
        //mCamera = mModulesFragment.getCamera(1);
        //mCamera.setDisplayLayoutListener(this);

        mTextureView = (TextureView)findViewById(R.id.textureView);
        mTextureView.setSurfaceTextureListener(this);

        Button btn = (Button)findViewById(R.id.button);
        btn.setOnClickListener(this);
    }

    @Override
    protected void onStart() {
        super.onStart();

        //mCamera.setDisplayLayoutListener(this);
        //mCamera.startCapture();
    }

    @Override
    protected void onStop() {
        super.onStop();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
    }

    @Override
    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
        Log.i(TAG, "onSurfaceTextureAvailable");
        mCamera.setPreviewTexture(surface);

        Log.i(TAG, "Thread ID: " + Thread.currentThread().getId());

        Set<Thread> threadSet = Thread.getAllStackTraces().keySet();
        Thread[] threadArray = threadSet.toArray(new Thread[threadSet.size()]);
        for(Thread t : threadArray) {
            Log.i(TAG, "Thread : " + t.getId() + ", name=" + t.getName());
        }
    }

    @Override
    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
    }

    @Override
    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        mCamera.stopCapture();
        return true;
    }

    @Override
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {
    }

    @Override
    public void onDisplayLayoutChanged(int width, int height, int orientation, int displayRotation) {

        Log.i(TAG, "onDisplayLayoutChanged: " + width +", " + height + ", " + orientation + ", " + displayRotation);

        mCamera.updatePreviewTransform(mTextureView.getWidth(), mTextureView.getHeight(), orientation, displayRotation);
        Matrix transformMatrix = mCamera.getPreviewTransform();

        class TransformUpdater implements Runnable {
            TextureView mView;
            Matrix mMatrix;
            TransformUpdater(TextureView view, Matrix matrix) {
                mView = view;
                mMatrix = matrix;
            }

            @Override
            public void run() {
                mView.setTransform(mMatrix);
            }
        }

        (new Handler(Looper.getMainLooper())).post(new TransformUpdater(mTextureView, transformMatrix));

    }

    @Override
    public void onClick(View view) {
        mCamera.toggleActiveCamera();
    }
}
