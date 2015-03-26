package org.libcinder.exampleapp;

import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;

import org.libcinder.app.Platform;
import org.libcinder.hardware.CinderCamera;
import org.libcinder.hardware.CinderCamera2;
import org.libcinder.net.UrlLoader;

public class SampleActivity extends ActionBarActivity implements View.OnClickListener {

    protected void runTests() {
        //UrlLoader.runTests();

        CinderCamera2.initialize();
        CinderCamera2.startCapture();
    }

    @Override
    public void onClick(View view) {
        Log.i("libcinder", "onClick called");
        runTests();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Platform.setActivity(this);

        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        Button button = (Button)findViewById(R.id.button);
        button.setOnClickListener(this);
    }

    @Override
    protected void onPause() {
        super.onPause();

        CinderCamera2.stopCapture();
    }

    @Override
    protected void onStop() {
        super.onStop();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
