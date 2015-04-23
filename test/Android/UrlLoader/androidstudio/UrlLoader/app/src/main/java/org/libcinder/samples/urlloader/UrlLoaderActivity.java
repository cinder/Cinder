package org.libcinder.samples.urlloader;

import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.app.NativeActivity;
import android.os.Bundle;

import org.libcinder.app.ModulesFragment;

public class UrlLoaderActivity extends NativeActivity {
    static final String TAG = "UrlLoaderActivity";

    private ModulesFragment mModulesFragment;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mModulesFragment = new ModulesFragment();
        FragmentManager fragmentManager = getFragmentManager();
        FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
        fragmentTransaction.add( mModulesFragment, "cinder-modules-fragment" );
        fragmentTransaction.commit();
    }
}
