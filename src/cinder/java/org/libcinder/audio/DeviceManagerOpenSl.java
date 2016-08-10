package org.libcinder.audio;

import android.content.Context;
import android.media.AudioManager;
import android.util.Log;

import org.libcinder.app.CinderNativeActivity;

public class DeviceManagerOpenSl {
    private static final String TAG = "cinder|audio";

    private int mSampleRate = 0;
    private int mFramesPerBlock = 0;

    public static DeviceManagerOpenSl create() {
        return new DeviceManagerOpenSl();
    }

    public DeviceManagerOpenSl() {
        CinderNativeActivity activity = CinderNativeActivity.getInstance();
        AudioManager audioManager = (AudioManager)activity.getSystemService( Context.AUDIO_SERVICE );

        int sampleRate = 44100;
        int numFrames = 64;

        try {
            sampleRate = Integer.parseInt( audioManager.getProperty( AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE ) );
            numFrames = Integer.parseInt( audioManager.getProperty( AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER ) );
        }
        catch( NumberFormatException e ) {
            Log.w( TAG, "Failed to read native OpenSL config, using defaults. exception message: " + e );
        }

        mSampleRate = sampleRate;
        mFramesPerBlock = numFrames;
    }

    public int getSampleRate() {
        return mSampleRate;
    }

    public int getFramesPerBlock() {
        return mFramesPerBlock;
    }
}

