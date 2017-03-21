package org.libcinder.exampleapp;

import android.app.Activity;
import android.app.FragmentManager;
import android.app.FragmentTransaction;
import android.os.Bundle;
import android.text.method.ScrollingMovementMethod;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import org.libcinder.exampleapp.widgets.DebugTextView;
import org.libcinder.net.UrlLoader;

import java.util.ArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class UrlLoaderActivity extends Activity implements Button.OnClickListener {

    private static final String TAG = "UrlLoaderActivity";

    //private ComponentManager mComponentManager;

    private DebugTextView mLog;

    private EditText mEditText;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_url_loader);

        /*
        mComponentManager = new ComponentManager();
        FragmentManager fragmentManager = getFragmentManager();
        FragmentTransaction fragmentTransaction = fragmentManager.beginTransaction();
        fragmentTransaction.add(mComponentManager, ComponentManager.FRAGMENT_TAG);
        fragmentTransaction.commit();
        */

        mLog = (DebugTextView)findViewById(R.id.textView);
        mLog.setMovementMethod(new ScrollingMovementMethod());

        Button btn = (Button)findViewById(R.id.button);
        btn.setOnClickListener(this);

        mEditText = (EditText)findViewById(R.id.editText);
        mEditText.setOnEditorActionListener(new TextView.OnEditorActionListener() {
            @Override
            public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
                boolean result = false;
                if((EditorInfo.IME_ACTION_DONE == actionId) ||
                   (EditorInfo.IME_NULL == actionId) ||
                   (KeyEvent.KEYCODE_ENTER == event.getKeyCode())
                )
                {
                    loadUrl();
                    result = true;
                }
                return result;
            }
        });
    }

    @Override
    public void onClick(View view) {
        Log.i(TAG, "clicked");

        loadUrl();
    }

    private void loadUrl() {

        mLog.appendLine("Spawning load url threads...");

        class RunThread implements Runnable {
            public String url;
            public byte[] data;
            public String summary;

            RunThread(String url) {
                this.url = url;
            }

            @Override
            public void run() {
                try {
                    UrlLoader loader = new UrlLoader();
                    data = loader.loadUrl(url);
                    if (null != data) {
                        summary = data.length + " bytes";
                    } else {
                        summary = "result is null!";
                    }
                }
                catch(Exception e) {
                    summary = "error: " + e.getMessage();
                }
            }
        }

        String[] urls = {
            "http://www.google.com",
            "http://www.amazon.com",
            "http://www.twitter.com",
            "http://www.facebook.com",
            "http://www.cnet.com",
            "http://www.freshnews.org",
            "http://www.theverge.com/",
            "http://www.gizmodo.com",
            "http://www.engadget.com"
        };

        ArrayList<RunThread> runners = new ArrayList<>();

        ExecutorService executor = Executors.newFixedThreadPool(urls.length);
        for(String url : urls) {
            RunThread rt = new RunThread(url);
            runners.add(rt);
            executor.execute(rt);
        }

        executor.shutdown();
        while(! executor.isTerminated());

        for(RunThread rt : runners) {
            mLog.appendLine("Loaded: " + rt.url);
            mLog.appendLine("result: " + rt.summary, 1);
        }
    }

    /*
    private void loadUrl() {
        InputMethodManager inputManager = (InputMethodManager)getSystemService(Context.INPUT_METHOD_SERVICE);
        inputManager.hideSoftInputFromWindow(getCurrentFocus().getWindowToken(), InputMethodManager.HIDE_NOT_ALWAYS);

        String url = mEditText.getText().toString();
        if(! url.startsWith("http://")) {
            url = "http://" + url;
            mEditText.setText(url);
        }

        mLog.setText("");
        mLog.appendLine("Loading: " + url);

        UrlLoader urlLoader = new UrlLoader();
        byte[] data = urlLoader.loadUrl(url);

        if(null != data) {
            mLog.appendLine("Data size: " + data.length + " bytes");

            try {
                String str = new String(data, "UTF-8");
                mLog.appendLine("Data: " + str);
            }
            catch(Exception e) {
                mLog.appendLine("Conversion from byte[] to String failed!");
            }

        }
        else {
            mLog.appendLine("Result was null!");
            mLog.appendLine("Response code: " + urlLoader.getResponseCode());
            mLog.appendLine("Response msg: " + urlLoader.getResponseMsg());
            mLog.appendLine("Exception msg: " + urlLoader.getExceptionMsg());
        }
    }
    */
}
