package org.libcinder.samples.cameratesterapp;

import android.app.NativeActivity;
import android.os.Bundle;

import org.libcinder.app.Platform;

public class CameraTesterAppActivity extends NativeActivity {
    static final String TAG = "CameraTesterAppActivity";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Platform.setActivity(this);
    }
}
