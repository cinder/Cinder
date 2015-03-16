package org.libcinder.samples.cinderactivitytest;

import android.app.NativeActivity;
import android.os.Bundle;

public class CinderNativeActivity extends NativeActivity {
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        System.out.println( "CinderNativeActivity.onCreate" );
    }
}
