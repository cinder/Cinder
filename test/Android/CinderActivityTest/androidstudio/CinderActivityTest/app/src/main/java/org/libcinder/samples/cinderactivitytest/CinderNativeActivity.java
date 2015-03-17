package org.libcinder.samples.cinderactivitytest;

import org.libcinder.SampleClass;

import android.app.NativeActivity;
import android.os.Bundle;

public class CinderNativeActivity extends NativeActivity {

    final static SampleClass sSampleClass = new SampleClass();

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
