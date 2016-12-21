package org.libcinder.video;

import android.media.MediaPlayer;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;
import android.view.Surface;

import java.io.FileDescriptor;

/** \class ExMediaPlayer
 *
 */
public class ExMediaPlayer {
    private static final String TAG = "ExMediaPlayer";

    private PlayerThread mPlayerThread;

    public ExMediaPlayer() {
        MediaPlayer mediaPlayer = new MediaPlayer();
        mPlayerThread = new PlayerThread(mediaPlayer);
        mPlayerThread.start();
    }

    public boolean isReady() {
        return (null != mPlayerThread.mHandler);
    }

    public int getVideoWidth() {
        int result = 0;
        if(null != mPlayerThread.getMediaPlayer()) {
            result = mPlayerThread.getMediaPlayer().getVideoWidth();
        }
        return result;
    }

    public int getVideoHeight() {
        int result = 0;
        if(null != mPlayerThread.getMediaPlayer()) {
            result = mPlayerThread.getMediaPlayer().getVideoHeight();
        }
        return result;
    }

    public void start() {
        mPlayerThread.post(new Runnable() {
            @Override
            public void run() {
                MediaPlayer mp = mPlayerThread.getMediaPlayer();
                if (null == mp) {
                    return;
                }

                mp.start();
            }
        });
    }

    public void stop() {
        mPlayerThread.post(new Runnable() {
            @Override
            public void run() {
                MediaPlayer mp = mPlayerThread.getMediaPlayer();
                if(null == mp) {
                    return;
                }

                mp.stop();
            }
        });
    }

    public void reset() {
        mPlayerThread.post(new Runnable() {
            @Override
            public void run() {
                MediaPlayer mp = mPlayerThread.getMediaPlayer();
                if(null == mp) {
                    return;
                }

                mp.reset();
            }
        });
    }

    public boolean isPlaying() {
        boolean result = false;
        if(null != mPlayerThread.getMediaPlayer()) {
            result = mPlayerThread.getMediaPlayer().isPlaying();
        }
        return result;
    }

    public void setLooping(final boolean looping) {
        mPlayerThread.post(new Runnable() {
            @Override
            public void run() {
                MediaPlayer mp = mPlayerThread.getMediaPlayer();
                if(null == mp) {
                    return;
                }

                mp.setLooping(looping);
            }
        });
    }

    public void setSurface(final Surface surface) {
        mPlayerThread.post(new Runnable() {
            @Override
            public void run() {
                MediaPlayer mp = mPlayerThread.getMediaPlayer();
                if(null == mp) {
                    return;
                }

                mp.setSurface(surface);
            }
        });
    }

    public void prepare() {
        mPlayerThread.post(new Runnable() {
            @Override
            public void run() {
                MediaPlayer mp = mPlayerThread.getMediaPlayer();
                if(null == mp) {
                    return;
                }

                try {
                    mp.prepare();
                }
                catch(Exception e) {
                    Log.e(TAG, "MediaPlayer.prepare failed: " + e.toString());
                }
            }
        });
    }

    public void prepareAsync() {
        mPlayerThread.post(new Runnable() {
            @Override
            public void run() {
                MediaPlayer mp = mPlayerThread.getMediaPlayer();
                if(null == mp) {
                    return;
                }

                try {
                    mp.prepareAsync();
                }
                catch(Exception e) {
                    Log.e(TAG, "MediaPlayer.prepare failed: " + e.toString());
                }
            }
        });
    }

    public void setDataSource(final FileDescriptor fd, final long offset, final long length) {
        mPlayerThread.post(new Runnable() {
            @Override
            public void run() {
                MediaPlayer mp = mPlayerThread.getMediaPlayer();
                if(null == mp) {
                    return;
                }

                try {
                    mp.setDataSource(fd, offset, length);
                }
                catch(Exception e) {
                    Log.e(TAG, "MediaPlayer.setDataSource failed: " + e.toString());
                }
            }
        });
    }

    public void setDataSource(final FileDescriptor fd) {
        mPlayerThread.post(new Runnable() {
            @Override
            public void run() {
                MediaPlayer mp = mPlayerThread.getMediaPlayer();
                if(null == mp) {
                    return;
                }

                try {
                    mp.setDataSource(fd);
                }
                catch(Exception e) {
                    Log.e(TAG, "MediaPlayer.setDataSource failed: " + e.toString());
                }
            }
        });
    }

    public void setOnVideoSizeChangedListener(final MediaPlayer.OnVideoSizeChangedListener listener) {
        mPlayerThread.post(new Runnable() {
            @Override
            public void run() {
                MediaPlayer mp = mPlayerThread.getMediaPlayer();
                if(null == mp) {
                    return;
                }

                mp.setOnVideoSizeChangedListener(listener);
            }
        });

    }

    /** \class PlayerThread
     *
     */
    private class PlayerThread extends Thread {

        private MediaPlayer mMediaPlayer;
        private Handler     mHandler;

        public PlayerThread(MediaPlayer mediaPlayer) {
            mMediaPlayer = mediaPlayer;
        }

        final public MediaPlayer getMediaPlayer() {
            return mMediaPlayer;
        }

        final boolean post(Runnable r) {
            boolean result = false;
            if(null != mHandler) {
                result = mHandler.post(r);
            }
            return result;
        }

        @Override
        public void run() {
            try {
                Looper.prepare();
                mHandler = new Handler();
                Looper.loop();
            }
            catch(Exception e) {
                Log.e(TAG, "thread halted: " + e.toString());
            }

            if(mMediaPlayer.isPlaying()) {
                mMediaPlayer.stop();
            }
        }
    }
}

