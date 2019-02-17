package org.libcinder.net;

import java.io.BufferedInputStream;
import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.UnknownServiceException;
import java.net.URL;
import java.net.URLDecoder;

import android.util.Log;

import org.libcinder.app.CinderNativeActivity;

/** \class UrlLoader
 *
 */
public class UrlLoader {

    private static final String TAG = "UrlLoader";

    private String mUrl;
    private int    mResponseCode = -1;
    private String mResponseMsg;
    private String mExceptionMsg;

    /** \class LoaderThread
     *
     */
    private class LoaderThread extends Thread {

        private byte[] mData;

        public byte[] getData() {
            return mData;
        }

        @Override
        public void run() {
            try {
                if(null != mUrl) {
                    int connectTimeout = 5000;
                    int readTimeout = 5000;

                    // java.net.URL doesn't seem to want to deal with encoded URLs
                    String decodedUrl = URLDecoder.decode(mUrl, "UTF-8");

                    URL url = new URL(decodedUrl);
                    HttpURLConnection connection = (HttpURLConnection)url.openConnection();
                    connection.setInstanceFollowRedirects(true);
                    connection.setConnectTimeout(connectTimeout);
                    connection.setReadTimeout(readTimeout);

                    // Handle redirection
                    switch(connection.getResponseCode()) {
                        case HttpURLConnection.HTTP_MOVED_PERM:
                        case HttpURLConnection.HTTP_MOVED_TEMP:
                        case HttpURLConnection.HTTP_SEE_OTHER:
                        {
                            String redirectLocation = connection.getHeaderField("Location");
                            String cookies = connection.getHeaderField("Set-Cookie");

                            connection = (HttpURLConnection)(new URL(redirectLocation)).openConnection();
                            if(null != cookies) {
                                connection.setRequestProperty("Cookie", cookies);
                            }
                        }
                        break;
                    }

                    int responseCode = connection.getResponseCode();
                    String responseMsg = connection.getResponseMessage();
                    mResponseCode = responseCode;
                    mResponseMsg = responseMsg;

                    try {
                        // Throw if we don't getInstance a 200
                        if(HttpURLConnection.HTTP_OK != responseCode) {
                            throw new UnknownServiceException(responseCode + ":" + responseMsg);
                        }

                        InputStream inputStream = new BufferedInputStream(connection.getInputStream());
                        byte[] buffer = new byte[1024];
                        int numBytesRead = 0;

                        ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
                        while(-1 != (numBytesRead = inputStream.read(buffer))) {
                            outputStream.write(buffer, 0, numBytesRead);
                        }

                        mData = outputStream.toByteArray();
                    }
                    finally {
                        connection.disconnect();
                    }
                }
            } catch(Exception e) {
                mExceptionMsg = e.getMessage();
                Log.i(TAG, mExceptionMsg);
            }
        }
    }

    // =============================================================================================
    // =============================================================================================

    public UrlLoader() {

    }

    public static UrlLoader create() {
        return new UrlLoader();
    }

    public byte[] loadUrl(String url) {
        byte[] result = null;

        if(CinderNativeActivity.permissions().INTERNET()) {
            try {
                mUrl = url;

                LoaderThread loader = new LoaderThread();
                loader.start();
                loader.join();

                result = loader.getData();
            } catch (Exception e) {
                mExceptionMsg = e.getMessage();
                Log.e(TAG, mExceptionMsg);
            }
        }
        else {
            mExceptionMsg = CinderNativeActivity.permissions().missing().INTERNET();
        }

        return result;
    }

    public int getResponseCode() {
        return mResponseCode;
    }

    public String getResponseMsg() {
        return mResponseMsg;
    }

    public String getExceptionMsg() {
        return mExceptionMsg;
    }
}

/*
public class UrlLoader {
    static private class Result {
        public String mUrl          = null;
        public int    mResponseCode = -1;
        public String mResponseMsg  = null;
        public String mExceptionMsg = null;
        public byte[] mData         = null;

        public Result(String url) {
            this.mUrl = url;
        }
    }

    static private ThreadLocal<Result> sResult = new ThreadLocal<Result>();

    static private class Impl extends Thread {
        private Result mResult = null;

        public Impl(Result result) {
            this.mResult = result;
        }

        @Override
        public void run() {
            try {
                if((null != mResult) && (null != mResult.mUrl)) {
                    int connectTimeout = 5000;
                    int readTimeout = 5000;

                    // java.net.URL doesn't seem to want to deal with encoded URLs
                    String decodedUrl = URLDecoder.decode(mResult.mUrl, "UTF-8");

                    URL url = new URL(decodedUrl);
                    HttpURLConnection connection = (HttpURLConnection)url.openConnection();
                    connection.setInstanceFollowRedirects(true);
                    connection.setConnectTimeout(connectTimeout);
                    connection.setReadTimeout(readTimeout);

                    // Handle redirection
                    switch(connection.getResponseCode()) {
                        case HttpURLConnection.HTTP_MOVED_PERM:
                        case HttpURLConnection.HTTP_MOVED_TEMP:
                        case HttpURLConnection.HTTP_SEE_OTHER:
                        {
                            String redirectLocation = connection.getHeaderField("Location");
                            String cookies = connection.getHeaderField("Set-Cookie");

                            connection = (HttpURLConnection)(new URL(redirectLocation)).openConnection();
                            if(null != cookies) {
                                connection.setRequestProperty("Cookie", cookies);
                            }
                        }
                        break;
                    }

                    int responseCode = connection.getResponseCode();
                    String responseMsg = connection.getResponseMessage();
                    mResult.mResponseCode = responseCode;
                    mResult.mResponseMsg = responseMsg;


                    try {
                        // Throw if we don't getInstance a 200
                        if(HttpURLConnection.HTTP_OK != responseCode) {
                            throw new UnknownServiceException(responseCode + ":" + responseMsg);
                        }

                        InputStream inputStream = new BufferedInputStream(connection.getInputStream());
                        byte[] buffer = new byte[1024];
                        int numBytesRead = 0;

                        ByteArrayOutputStream outputStream = new ByteArrayOutputStream();
                        while(-1 != (numBytesRead = inputStream.read(buffer))) {
                            outputStream.write(buffer, 0, numBytesRead);
                        }

                        mResult.mData = outputStream.toByteArray();
                    }
                    finally {
                        connection.disconnect();
                    }
                }
            } catch(Exception e) {
                mResult.mExceptionMsg = e.toString();
            }
        }
    }

    public static byte[] loadUrl(String url) {
        byte[] result = null;

        sResult.set(new Result(url));

        try {
            Log.i(Cinder.TAG, " (Java) Loading: " + URLDecoder.decode(url, "UTF-8"));

            Impl loader = new Impl(sResult.getInstance());
            loader.start();
            loader.join();
            result = sResult.getInstance().mData;
        }
        catch(Exception e) {
            sResult.getInstance().mExceptionMsg = e.toString();
        }

        return result;
    }

    public static int getResponseCode() {
        return (null != sResult.getInstance()) ? sResult.getInstance().mResponseCode : -1;
    }

    public static String getResponseMsg() {
        return (null != sResult.getInstance()) ? sResult.getInstance().mResponseMsg : null;
    }

    public static String getExceptionMsg() {
        return (null != sResult.getInstance()) ? sResult.getInstance().mExceptionMsg : null;
    }


    // -------------------------------------------------------------------------

    private static class Tester extends Thread {
        private String mUrl;
        public Tester(String url) {
            this.mUrl = url;
        }

        @Override
        public void run() {
            String threadId = Thread.currentThread().getName();
            System.out.println("Running (" + threadId + ")");
            byte[] result = UrlLoader.loadUrl(this.mUrl);
            System.out.println("Resulting data size (" + threadId + ") : " + ((null != result) ? result.length : 0));
//            if( null != UrlLoader.getResponseMsg() ) {
//                System.out.println("Response Msg (" + threadId + ") : " + UrlLoader.getResponseMsg());
//            }
            if( null != UrlLoader.getExceptionMsg() ) {
                System.out.println("Exception (" + threadId + ") : " + UrlLoader.getExceptionMsg());
            }
        }
    }

    static public void runTests() {
        ArrayList<Tester> testers = new ArrayList<Tester>();
        testers.add(new Tester("http://www.google.com"));
        testers.add(new Tester("http://www.amazon.com"));
        testers.add(new Tester("http://www.yahoo.com"));
        testers.add(new Tester("http://www.cnet.com"));
        testers.add(new Tester("http://www.slashdot.org"));
        testers.add(new Tester("http://upload.wikimedia.org/wikipedia/commons/d/d4/New_York_City_at_night_HDR_edit1.jpg"));

        for(Tester tester : testers) {
            tester.start();
        }

        for(Tester tester : testers) {
            try {
                tester.join();
            }
            catch(Exception e ) {

            }
        }
    }
}
*/