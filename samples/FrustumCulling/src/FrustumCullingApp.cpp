#include "cinder/app/AppBasic.h"
#include "cinder/app/RendererGl.h"

#include "cinder/gl/gl.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/Shader.h"

#include "cinder/Log.h"
#include "cinder/Camera.h"
#include "cinder/Utilities.h"
#include "cinder/ImageIo.h"
#include "cinder/Frustum.h"

#include "InfoPanel.h"

using namespace std;
using namespace ci;
using namespace ci::app;

const int GridSize			= 31;
const int HalfGrid			= (int)(GridSize/2);
const float GridSpace		= 15.0f;
const float BoundingSize	= GridSize * GridSpace;

const float SphereSize		= 5.0f;
const float CubeSize		= 4.0f;
const float PointSize		= 0.5f;

class FrustumCullingApp : public AppBasic {
 private:
	enum ShapeType { SPHERE, CUBE, POINT };
	
 public:
	void prepareSettings( Settings *settings );
	void setup();
	void keyDown( KeyEvent event );
	void update();
	void draw();
	
	
	gl::BatchRef	mSphere, mCube;
	InfoPanel		mInfoPanel;
	
	CameraPersp		mCam;
	CameraPersp		mRenderCam;
	
	ci::Frustumf	mFrustum;
	
	vec3			mREye, mRCenter, mEye,
					mEyeDest, mCenter, mEyeNormal;
	float			mFov, mNear, mFar,
					mDecay, mAngle, mAngleDest;
	
	ShapeType		mType;
	bool			mIsWatchingCam;
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
	
	mIsWatchingCam	= true;
	
	mType		= SPHERE;
	
	mFov		= 60.0f;
	mNear		= 10.0f;
	mFar		= 1500.0f;
	mDecay		= 0.25f;
	// mClipCam Eye and Center
	mEye		= vec3( 0.0f, 0.0f, -100.0f );
	mEyeDest	= vec3( 0.0f, 0.0f, -100.0f );
	mCenter		= vec3( 0.0f, 0.0f, 100.0f );
	mEyeNormal	= vec3();
	
	mAngle		= 0.0f;
	mAngleDest	= 0.0f;
	
	// mRenderCam Eye and Center
	mREye		= vec3( 10.0f, 10.0f, 10.0f );
	mRCenter	= vec3( 0.0f );
	
	auto glsl = gl::getStockShader( gl::ShaderDef().color() );
	
	mSphere = gl::Batch::create( geom::Sphere()
										.segments( 60 )
										.disable( geom::NORMAL )
										.disable( geom::COLOR ), glsl );
	mCube = gl::Batch::create( geom::Cube()
										.disable( geom::NORMAL )
										.disable( geom::COLOR ), glsl );

	gl::clearColor( ColorA( 0.1f, 0.1f, 0.1f, 1.0f ) );
	gl::enableDepthRead( true );
	gl::enableAlphaBlending();
}


void FrustumCullingApp::update()
{
	auto counter = getElapsedFrames();
	mAngle -= ( mAngle - mAngleDest ) * 0.1f;
	mEye -= ( mEye - mEyeDest ) * 0.2f;
	
	mEyeNormal = vec3( sin( mAngle ), 0.0f, cos( mAngle ) ); 
	mCenter = mEye + mEyeNormal * 50.0f;
	
	mCam.setPerspective( 25.0f, getWindowAspectRatio(), 100.0f, 350.0f );
	mCam.lookAt( mEye, mCenter );
	
	// After adjusting the camera, update the Frustum
	mFrustum.set( mCam );
	
	if( mIsWatchingCam ){
		mREye = lerp( mREye, vec3( mEye.x + cos( counter * 0.003f ) * 300.0f, 100.0f, mEye.y + sin( counter * 0.003f ) * 300.0f ), mDecay );
		mRCenter = lerp( mRCenter, vec3( mEye + mEyeNormal * 250.0f ), mDecay );
		mFov -= ( mFov - 60.0f ) * mDecay;
		mNear -= ( mNear - 10.0f ) * mDecay;
		mFar -= ( mFar - 1500.0f ) * mDecay;
	} else {
		mREye = lerp( mREye, mEye, mDecay );
		mRCenter = lerp( mRCenter, mCenter, mDecay );
		mFov -= ( mFov - mCam.getFov() ) * mDecay;
		mNear -= ( mNear - mCam.getNearClip() ) * mDecay;
		mFar -= ( mFar - mCam.getFarClip() ) * mDecay;
	}

	mRenderCam.setPerspective( mFov, getWindowAspectRatio(), 10.0f, 1500.0f );
	mRenderCam.lookAt( mREye, mRCenter );
}

void FrustumCullingApp::draw()
{
	gl::enableDepthWrite( true );
	gl::clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	gl::color( 1.0f, 1.0f, 1.0f, 1.0f );
	
	gl::setMatrices( mRenderCam );
	
	if( mIsWatchingCam )
		gl::drawFrustum( mCam );

	for( int x = 0; x < GridSize; x++ ){
		for( int y = 0; y < GridSize; y++ ){
			for( int z = 0; z < GridSize; z++ ){
				vec3 loc( ( x - HalfGrid) * GridSpace,
						  ( y - HalfGrid / 2 ) * GridSpace,
						  ( z - HalfGrid ) * GridSpace );
				
				if( mType == POINT ){
					// check if the point is contained
					// in the frustum, if it is it will
					// return true.
					if( mFrustum.contains( loc ) ){
						gl::ScopedModelMatrix scopeModel;
						gl::multModelMatrix( translate( loc ) * scale( vec3( PointSize ) ) );
						gl::color( 0.0f, 1.0f, 0.0f );
						mCube->draw();
					}
					
				} else if( mType == SPHERE ){
					// check if the sphere is contained
					// in the frustum, if it is it will
					// return true
					if( mFrustum.contains( loc, SphereSize ) ){
						gl::ScopedModelMatrix scopeModel;
						gl::multModelMatrix( translate( loc ) * scale( vec3( SphereSize ) ) );
						gl::color( 0.0f, 1.0f, 0.0f );
						mSphere->draw();
					}
					
				} else if( mType == CUBE ){
					// check if the cube is contained
					// in the frustum, if it is it will
					// return true
					if( mFrustum.contains( loc, vec3( CubeSize ) ) ){
						gl::ScopedModelMatrix scopeModel;
						gl::multModelMatrix( translate( loc ) * scale( vec3( CubeSize ) ) );
						gl::color( 0.0f, 1.0f, 0.0f );
						mCube->draw();
						
					}
				}

			}
		}
	}

	gl::color( 0.4f, 0.4f, 0.4f, 1.0f );
	gl::drawStrokedCube( vec3( 0.0f ), vec3( BoundingSize ) );
	
	gl::enableDepthWrite( false );
	gl::setMatricesWindow( getWindowSize() );

	mInfoPanel.update( getElapsedFrames() );
	
	/*
	std::stringstream ss;
	ss << getHomeDirectory() << "FrustumCulling/image_" << mCounter << ".png";
	writeImage( ss.str(), copyWindowSurface() );
	*/
}

void FrustumCullingApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' ){
		setFullScreen( ! isFullScreen() );
	} else if( event.getChar() == '1' ){
		mType = SPHERE;
	} else if( event.getChar() == '2' ){
		mType = CUBE;
	} else if( event.getChar() == '3' ){
		mType = POINT;
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