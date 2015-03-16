package org.libcinder.samples.cinderactivitytest;

import android.app.NativeActivity;
import android.os.Bundle;

public class CinderNativeActivity extends NativeActivity {

    static void staticJavaFunction() {
        System.out.println( "CinderNativeActivity.staticJavaFunction" );
    }

    void simpleJavaFunction() {
        System.out.println( "CinderNativeActivity.simpleJavaFunction" );
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        System.out.println( "CinderNativeActivity.onCreate" );
    }
}
