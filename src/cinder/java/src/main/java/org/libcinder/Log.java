package org.libcinder;

public class Log {

    private static final String TAG = "libcinder";

    public static void i(String tag, String msg) {
        android.util.Log.e(tag, msg);
    }

    public static void i(String msg) {
        android.util.Log.e(TAG, msg);
    }

    public static void e(String tag, String msg) {

        android.util.Log.e(tag, msg);
    }

    public static void e(String msg) {
        android.util.Log.e(TAG, msg);
    }
}
