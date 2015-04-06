package org.libcinder.samples.javatextureload;

import android.app.NativeActivity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLES30;
import android.opengl.GLUtils;
import android.os.Bundle;
import android.util.Log;

import org.libcinder.app.Platform;

public class JavaTextureLoadActivity extends NativeActivity {
    static final String TAG = "JavaTextureLoadActivity";

    static NativeActivity sActivity = null;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        sActivity = this;
    }

    public static void loadTexture(int textureId) {
        Log.i(Platform.TAG, "loadTexture: " + textureId);

        try {
            Bitmap bitmap = BitmapFactory.decodeStream(sActivity.getAssets().open("flowers.jpg"));

            GLES30.glBindTexture(GLES30.GL_TEXTURE_2D, textureId);
            GLUtils.texImage2D(GLES30.GL_TEXTURE_2D, 0, bitmap, 0);

            bitmap.recycle();
        }
        catch(Exception e ) {
            Log.e(Platform.TAG, "Error loading texture: " + e);
        }
    }
}
