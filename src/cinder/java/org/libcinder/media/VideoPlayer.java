package org.libcinder.media;

import android.app.Activity;
import android.app.Fragment;
import android.os.Bundle;

public class VideoPlayer extends Fragment {

    private ExMediaPlayer mVideoPlayer;

    /** onAttach
     *
     */
    @Override
    public void onAttach(Activity activity) {
        super.onAttach(activity);
    }

    /** onCreate
     *
     */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setRetainInstance(true);
    }

    /** onActivityCreated
     *
     */
    @Override
    public void onActivityCreated(Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
    }

    /** onStart
     *
     */
    @Override
    public void onStart() {
        super.onStart();
    }

    /** onResume
     *
     */
    @Override
    public void onResume() {
        super.onResume();
    }

    /** onPause
     *
     */
    @Override
    public void onPause() {
        super.onPause();
    }

    /** onStop
     *
     */
    @Override
    public void onStop() {
        super.onStop();
    }

    /** onDestroy
     *
     */
    @Override
    public void onDestroy() {
        super.onDestroy();
    }

    /** onDetach
     *
     */
    @Override
    public void onDetach() {
        super.onDestroy();
    }
}
