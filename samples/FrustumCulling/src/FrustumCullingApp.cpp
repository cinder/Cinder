#include "cinder/app/AppBasic.h"
#include "cinder/Camera.h"
#include "cinder/Frustum.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/Vector.h"
#include "cinder/gl/gl.h"
#include "InfoPanel.h"

using namespace ci;
using namespace ci::app;

class FrustumCullingApp 
	: public AppBasic 
{
private:
	static enum { SPHERE, POINT, CUBE };
public:
	void prepareSettings( Settings *settings );
	void setup();
	void update();
	void draw();

	void keyDown( KeyEvent event );
protected:
	CameraPersp mCam;
	CameraPersp mRenderCam;

	Frustum		mFrustum;
	
	float mFov, mNear, mFar;
	float mDecay;
	
	Vec3f mEye, mEyeDest, mCenter, mEyeNormal;
	float mDistance;
	float mAngle, mAngleDest;
	
	Vec3f mREye, mRCenter;
	Vec3f mUp;

	Vec3f mColor;
	
	float mCounter;	
		
	int mGridSize;
	int mHalfGrid;
	float mGridSpace;
	
	int mShapeType;
	
	bool mIsWatchingCam;
	bool mRenderPoints;
	bool mRenderSpheres;
	bool mRenderCubes;
	
	InfoPanel mInfoPanel;
};

void FrustumCullingApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 1280, 720 );
	settings->setFrameRate( 60.0f );
	settings->setFullScreen( false );
}

void FrustumCullingApp::setup()
{
	mInfoPanel.createTexture();

	mFrustum.set(mCam);
	
	mIsWatchingCam	= true;
	
	mShapeType		= SPHERE;
	
	mGridSize		= 31;
	mHalfGrid		= (int)(mGridSize/2);
	mGridSpace		= 15.0f;
	
	mCounter		= 0.0f;
	
	mColor			= Vec3f( 0.0f, 0.0f, 0.0f );	
	
	mFov		= 60.0f;
	mNear		= 10.0f;
	mFar		= 1500.0f;
	mDecay		= 0.25f;
	// mClipCam Eye and Center
	mEye		= Vec3f( 0.0f, 0.0f, -100.0f );
	mEyeDest	= Vec3f( 0.0f, 0.0f, -100.0f );
	mCenter		= Vec3f( 0.0f, 0.0f, 100.0f );
	mEyeNormal	= Vec3f::zAxis();
	
	mDistance	= 500.0f;
	mAngle		= 0.0f;
	mAngleDest	= 0.0f;
	
	
	// mRenderCam Eye and Center
	mREye		= Vec3f( 10.0f, 10.0f, 10.0f );
	mRCenter	= Vec3f::zero();
	
	mUp			= Vec3f::yAxis();	

	glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );
	gl::enableDepthRead( true );
	gl::enableDepthWrite( true );
	gl::enableAlphaBlending();
}

void FrustumCullingApp::update()
{
	mAngle -= ( mAngle - mAngleDest ) * 0.1f;
	mEye -= ( mEye - mEyeDest ) * 0.2f;
	
	mEyeNormal = Vec3f( sin( mAngle ), 0.0f, cos( mAngle ) ); 
	mCenter = mEye + mEyeNormal * 50.0f;
	
	mCam.setPerspective( 25.0f, getWindowAspectRatio(), 100.0f, 350.0f );
	mCam.lookAt( mEye, mCenter, mUp );
	mFrustum.set( mCam );	
	
	if( mIsWatchingCam ){
		mREye.lerpEq( mDecay, Vec3f( mEye.x + cos( mCounter * 0.003f ) * 300.0f, 100.0f, mEye.y + sin( mCounter * 0.003f ) * 300.0f ) );
		mRCenter.lerpEq( mDecay, Vec3f( mEye + mEyeNormal * 250.0f ) );
		mFov -= ( mFov - 60.0f ) * mDecay;
		mNear -= ( mNear - 10.0f ) * mDecay;
		mFar -= ( mFar - 1500.0f ) * mDecay;
	} else {
		mREye.lerpEq( mDecay, mEye );
		mRCenter.lerpEq( mDecay, mCenter );
		mFov -= ( mFov - mCam.getFov() ) * mDecay;
		mNear -= ( mNear - mCam.getNearClip() ) * mDecay;
		mFar -= ( mFar - mCam.getFarClip() ) * mDecay;
	}	

	mRenderCam.setPerspective( mFov, getWindowAspectRatio(), 10.0f, 1500.0f );
	mRenderCam.lookAt( mREye, mRCenter, mUp );
		
	gl::setMatrices( mRenderCam );	
	
	mCounter += 1.0f;
}

void FrustumCullingApp::draw()
{
	gl::enableDepthWrite( true );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	if( mIsWatchingCam )
		gl::drawFrustum( mCam );

	for( int x=0; x<mGridSize; x++ ){
		for( int y=0; y<mGridSize; y++ ){
			for( int z=0; z<mGridSize; z++ ){
				Vec3f loc( (x-mHalfGrid)*mGridSpace, (y-mHalfGrid/2)*mGridSpace, (z-mHalfGrid)*mGridSpace );
				
				if( mShapeType == POINT ){
					if( mFrustum.isPointInFrustum( loc ) ){
						glColor3f( 0.0f, 1.0f, 0.0f );
						gl::drawCube( loc, Vec3f( 0.5f, 0.5f, 0.5f ) );
					}
					
				} else if( mShapeType == SPHERE ){
					if( mFrustum.isSphereInFrustum( loc, 5.0f ) ){
						glColor3f( 0.0f, 1.0f, 0.0f );
						gl::drawSphere( loc, 5.0f );
					}
					
				} else if( mShapeType == CUBE ){
					Vec3f cubeSize = Vec3f::one() * 6.0f;
					AxisAlignedBox3f bounds( loc - 0.5f * cubeSize, loc + 0.5f * cubeSize );
					if( mFrustum.isBoxInFrustum( bounds ) ){
						glColor3f( 0.0f, 1.0f, 0.0f );
						gl::drawCube( loc, cubeSize );						
					}
				}
			}
		}
	}
	
	float size = mGridSize * mGridSpace;
	glColor4f( 0.4f, 0.4f, 0.4f, 1.0f );
	gl::drawStrokedCube( Vec3f::zero(), Vec3f( size, size, size ) );
		
	gl::enableDepthWrite( false );
	gl::setMatricesWindow( getWindowSize() );

	glEnable( GL_TEXTURE_2D );
	mInfoPanel.update( mCounter );
	glDisable( GL_TEXTURE_2D );
}

void FrustumCullingApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' ){
		setFullScreen( ! isFullScreen() );
	} else if( event.getChar() == '1' ){
		mShapeType = SPHERE;
	} else if( event.getChar() == '2' ){
		mShapeType = CUBE;
	} else if( event.getChar() == '3' ){
		mShapeType = POINT;
	} else if( event.getChar() == 'c' ){
		mIsWatchingCam = ! mIsWatchingCam;
	} else if( event.getChar() == '/' || event.getChar() == '?' ){
		mInfoPanel.toggleState();
	}	
	
	if( event.getCode() == KeyEvent::KEY_UP ){
		mEyeDest += mEyeNormal * 40.0f;
	} else if( event.getCode() == KeyEvent::KEY_DOWN ){
		mEyeDest -= mEyeNormal * 40.0f;
	}
	
	if( event.getCode() == KeyEvent::KEY_LEFT ){
		mAngleDest += 0.1f;
	} else if( event.getCode() == KeyEvent::KEY_RIGHT ){
		mAngleDest -= 0.1f;
	}
}

CINDER_APP_BASIC( FrustumCullingApp, RendererGl )
