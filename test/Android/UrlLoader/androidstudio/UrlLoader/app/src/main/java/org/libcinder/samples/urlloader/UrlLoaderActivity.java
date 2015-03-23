package org.libcinder.samples.urlloader;

import android.app.NativeActivity;
//import org.libcinder.netnone.UrlLoader;
//import org.libcinder.netnone.UrlLoaderResult;

public class UrlLoaderActivity extends NativeActivity {
    static final String TAG = "UrlLoaderActivity";


/*
    static UrlLoaderResult sUrlLoaderResult = null;

    static long loadUrlStart(String url) {
        int result = -1;
        sUrlLoaderResult = UrlLoader.loadUrl(url);
        if((null != sUrlLoaderResult) && (null != sUrlLoaderResult.getData())) {
           result = sUrlLoaderResult.getData().length;
        }
        return result;
    }

    static void loadUrlClear() {
        sUrlLoaderResult = null;
    }

    static int loadUrlGetResponseCode() {
        int result = (null != sUrlLoaderResult) ? sUrlLoaderResult.getResponseCode() : -1;
        return result;
    }

    static String loadUrlGetErrorMsg() {
        String result = (null != sUrlLoaderResult) ? sUrlLoaderResult.getErrorMsg() : null;
        return result;
    }

    static byte[] loadUrlGetData() {
        byte[] result = (null != sUrlLoaderResult) ? sUrlLoaderResult.getData() : null;
        return result;
    }
*/
}
