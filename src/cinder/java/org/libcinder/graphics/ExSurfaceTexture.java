package org.libcinder.graphics;

/** \class SurfaceTexture
 *
 */
public class ExSurfaceTexture extends android.graphics.SurfaceTexture {

    private int mTexName = -1;
    private boolean mAttached = false;

    public ExSurfaceTexture(int texName) {
        super(texName, false);
        mTexName = texName;

        /*
        if(-1 == mTexName) {
            super.detachFromGLContext();
            mAttached = false;
        }
        */
    }

    public ExSurfaceTexture(int texName, boolean singleBufferMode) {
        super(texName, singleBufferMode);
        mTexName = texName;
    }

    public int getTexName() {
        return mTexName;
    }

    @Override
    public void attachToGLContext(int texName) {
        super.attachToGLContext(texName);
        mTexName = texName;
        mAttached = true;
    }

    public void attachToGLContext() {
        if(mAttached) {
            return;
        }
        this.attachToGLContext(mTexName);
    }

    @Override
    public void detachFromGLContext() {
        super.detachFromGLContext();
        mAttached = false;
    }

    public boolean isAttached() {
        return mAttached;
    }

    @Override
    public void updateTexImage() {
        if((-1 == mTexName) || (! mAttached)) {
            return;
        }
        super.updateTexImage();
    }
}

