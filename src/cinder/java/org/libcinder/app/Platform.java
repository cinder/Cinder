package org.libcinder.app;

import android.app.Activity;
import android.content.Context;
import android.graphics.Point;
import android.view.Display;
import android.view.WindowManager;

/** \class Platform
 *
 */
public class Platform {
    public final static String TAG = "cinder";

    private static Activity sActivity = null;

    public static Activity getActivity() {
        return sActivity;
    }

    public static void setActivity(Activity activity) {
        if(null != sActivity) {
            return;
        }
        sActivity = activity;
    }

    public static Point getDisplaySize() {
        Point result = null;
        if(null != sActivity) {
            WindowManager wm = (WindowManager)sActivity.getSystemService(Context.WINDOW_SERVICE);
            Display display = wm.getDefaultDisplay();
            Point size = new Point();
            display.getRealSize(size);
            result = size;
        }
        return result;
    }
}