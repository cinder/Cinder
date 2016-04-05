package org.libcinder.video;

import android.content.res.AssetFileDescriptor;
import android.content.res.AssetManager;
import android.graphics.SurfaceTexture;
import android.media.MediaPlayer;
import android.os.ConditionVariable;
import android.os.Handler;
import android.os.HandlerThread;
import android.provider.MediaStore;
import android.util.Log;
import android.view.Surface;

import org.libcinder.app.CinderNativeActivity;

import java.net.URL;
import java.util.concurrent.atomic.AtomicBoolean;

public class VideoPlayer implements SurfaceTexture.OnFrameAvailableListener, MediaPlayer.OnCompletionListener {

    private static final String TAG = "libcinder...VideoPlayer";

    private MediaPlayer mMediaPlayer;
    private Handler mHandler;
    private HandlerThread mHandlerThread;

    private float mVolume = 1.0f;
    private boolean mIsPlaying = false;
    private boolean mIsDone = false;
    private boolean mLoop = false;

    private AtomicBoolean mFrameAvailable = new AtomicBoolean(false);
    private SurfaceTexture mSurfaceTexture;

    public VideoPlayer(URL url) {
        Log.i(TAG, "VideoPlayer(URL url)");
        initialize(url);
    }

    public VideoPlayer(String filePath) {
        Log.i(TAG, "VideoPlayer(String filePath): filePath=" + filePath);
        initialize(filePath);
    }

    private void initializeCommon() {
        try {
            mHandlerThread = new HandlerThread("cinder-media-videoplayer-thread");
            mHandlerThread.start();

            mHandler = new Handler(mHandlerThread.getLooper());

            // Initialize MediaPlayer on the handler thread
            final ConditionVariable condition = new ConditionVariable(false);
            mHandler.post(new Runnable() {
                @Override
                public void run() {
                    mMediaPlayer = new MediaPlayer();
                    condition.open();
                }
            });
            condition.block();
        }
        catch(Exception e) {
            Log.e(TAG, "VideoPlayer.initializeCommon error: " + e.getMessage());
        }
    }

    private void initialize(URL url) {
        initializeCommon();
        if( null != mMediaPlayer ) {
            mMediaPlayer.setOnCompletionListener(this);
        }
    }

    private void initialize(String filePath) {
        initializeCommon();
        try {
            AssetManager am = CinderNativeActivity.getInstance().getAssets();
            AssetFileDescriptor afd = am.openFd(filePath);
            mMediaPlayer.setDataSource(afd.getFileDescriptor(), afd.getStartOffset(), afd.getLength());
            mMediaPlayer.prepare();
        }
        catch(Exception e) {
            Log.e(TAG, "VideoPlayer.initialize(String filePath) error: " + e.getMessage());
        }
        if( null != mMediaPlayer ) {
            mMediaPlayer.setOnCompletionListener(this);
        }
    }

    private void initializeTexture(int texName) {
        try {
            mSurfaceTexture = new SurfaceTexture(texName);
            mSurfaceTexture.setOnFrameAvailableListener(this);

            mMediaPlayer.setSurface(new Surface(mSurfaceTexture));

            Log.i(TAG, "initializeTexture: texName=" + texName);
        }
        catch(Exception e) {
            Log.e(TAG, "VideoPlayer.initializeTexture error: " + e.getMessage());
        }
    }

    public void updateTexture() {
        synchronized (this) {
            if( mFrameAvailable.get() ) {
                mSurfaceTexture.updateTexImage();
                mFrameAvailable.set(false);
            }
        }
    }

    @Override
    public void onCompletion(MediaPlayer mp) {
        mIsPlaying = false;
        mIsDone = true;
    }

    /** createFromUrl
     *
     */
    public static VideoPlayer createFromUrl(String url) {
        Log.i(TAG, "VideoPlayer.createFromUrl");

        VideoPlayer result = null;
        try {
            new VideoPlayer(new URL(url));
        }
        catch(Exception e) {
            Log.e(TAG, "createFromUrl failed: " + e.getMessage() + e.getStackTrace());
        }
        return result;
    }

    /** createFromFilePath
     *
     */
    public static VideoPlayer createFromFilePath(String filePath) {
        Log.i(TAG, "VideoPlayer.createFromFilePath");
        VideoPlayer result = new VideoPlayer(filePath);
        return result;
    }

    /** destroy
     *
     */
    public void destroy() {

    }

    /** getWidth
     *
     */
    public int getWidth() {
        return (null != mMediaPlayer) ? mMediaPlayer.getVideoWidth() : 0;
    }

    /** getHeight
     *
     */
    public int getHeight() {
        return (null != mMediaPlayer) ? mMediaPlayer.getVideoHeight() : 0;
    }

    /** getDuration
     *
     */
    public float getDuration() {
        float msecDuration = (float)((null != mMediaPlayer) ? mMediaPlayer.getDuration() : 0);
        return  msecDuration/1000.0f;
    }

    /** seekToTime
     *
     */
    public void seekToTime(float time) {
        final int msecTime = (int)(time*1000.0f);
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                if(null != mMediaPlayer) {
                    mMediaPlayer.seekTo(msecTime);
                }
            }
        });
    }

    /** seekToStart
     *
     */
    public void seekToStart() {
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                if(null != mMediaPlayer) {
                    mMediaPlayer.seekTo(0);
                }
            }
        });
    }

    /** seekToEnd
     *
     */
    public void seekToEnd() {
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                if(null != mMediaPlayer) {
                    mMediaPlayer.seekTo(mMediaPlayer.getDuration());
                }
            }
        });
    }

    /** setLoop
     *
     */
    public void setLoop(final boolean loop) {
        mLoop = loop;
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                if( null != mMediaPlayer ) {
                    mMediaPlayer.setLooping(loop);
                }
            }
        });
    }

    /** isNewFrameAvailable
     *
     */
    public boolean isNewFrameAvailable() {
        boolean result = mFrameAvailable.get();
        return result;
    }

    /** getVolume
     *
     */
    public float getVolume() {
        return mVolume;
    }

    /** setVolume
     *
     */
    public void setVolume(float volume) {
        mVolume = volume;
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                if( null != mMediaPlayer ) {
                    mMediaPlayer.setVolume(mVolume, mVolume);
                }
            }
        });
    }

    /** isPlaying
     *
     */
    public boolean isPlaying() {
        return mIsPlaying;
        //return (null != mMediaPlayer) && mMediaPlayer.isPlaying();
    }

    /** isDone
     *
     */
    public boolean isDone() {
        return mLoop ? false : mIsDone;
    }

    /** play
     *
     */
    public void play() {
        mIsPlaying = true;
        mIsDone = false;
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                if( null != mMediaPlayer ) {
                    mMediaPlayer.start();
                }
            }
        });
    }

    /** stop
     *
     */
    public void stop() {
        mIsPlaying = false;
        mIsDone = true;
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                if( null != mMediaPlayer ) {
                    mMediaPlayer.stop();
                }
            }
        });
    }

    /** pause
     *
     */
    public void pause() {
        mIsPlaying = false;
        mHandler.post(new Runnable() {
            @Override
            public void run() {
                if( null != mMediaPlayer ) {
                    mMediaPlayer.pause();
                }
            }
        });
    }

    @Override
    public void onFrameAvailable(SurfaceTexture notUsed) {
        synchronized (this) {
            mFrameAvailable.set(true);
        }
    }
}
