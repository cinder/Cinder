#include "cinder/app/AppBasic.h"
#include "cinder/Vector.h"
#include "cinder/Camera.h"
#include "cinder/Utilities.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "InfoPanel.h"

using namespace ci;
using namespace ci::app;

class FrustumCullingApp : public AppBasic {
 private:
	enum { TOP, BOT, LEF, RIG, NEA, FARP };
	enum { SPHERE, CUBE, POINT };
	
 public:
	void prepareSettings( Settings *settings );
	void setup();
	void keyDown( KeyEvent event );
	void update();
	void draw();
	void drawLineCube( const Vec3f &center, const Vec3f &size );
	
	bool isPointInFrustum( const Vec3f &loc );
	bool isSphereInFrustum( const Vec3f &loc, float radius );
	bool isBoxInFrustum( const Vec3f &loc, const Vec3f &size );
	
	bool mFrustumPlaneCached;

	void calcFrustumPlane( Vec3f &fNormal, Vec3f &fPoint, float &fDist, const Vec3f &v1, const Vec3f &v2, const Vec3f &v3 );
	void calcNearAndFarClipCoordinates( const Camera &cam );	
	
	CameraPersp mCam;
	CameraPersp mRenderCam;
	
	Vec3f mFrustumPlaneNormals[6];
	Vec3f mFrustumPlanePoints[6];
	float mFrustumPlaneDists[6];
	
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

	// should this be a struct?
	for( int i=0; i<6; i++ ){
		mFrustumPlaneNormals[i] = Vec3f::zero();
		mFrustumPlanePoints[i] = Vec3f::zero();
		mFrustumPlaneDists[i] = 0.0f;
	}
	mFrustumPlaneCached = false;
	
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
	mFrustumPlaneCached = false;
	mAngle -= ( mAngle - mAngleDest ) * 0.1f;
	mEye -= ( mEye - mEyeDest ) * 0.2f;
	
	mEyeNormal = Vec3f( sin( mAngle ), 0.0f, cos( mAngle ) ); 
	mCenter = mEye + mEyeNormal * 50.0f;
	
	mCam.setPerspective( 25.0f, getWindowAspectRatio(), 100.0f, 350.0f );
	mCam.lookAt( mEye, mCenter, mUp );
	calcNearAndFarClipCoordinates( mCam );
	
	
	
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
					if( isPointInFrustum( loc ) ){
						glColor3f( 0.0f, 1.0f, 0.0f );
						gl::drawCube( loc, Vec3f( 0.5f, 0.5f, 0.5f ) );
					}
					
				} else if( mShapeType == SPHERE ){
					if( isSphereInFrustum( loc, 5.0f ) ){
						glColor3f( 0.0f, 1.0f, 0.0f );
						gl::drawSphere( loc, 5.0f );
					}
					
				} else if( mShapeType == CUBE ){
					int cubeSize = 6.0f;
					if( isBoxInFrustum( loc, Vec3f( cubeSize, cubeSize, cubeSize ) ) ){
						glColor3f( 0.0f, 1.0f, 0.0f );
						gl::drawCube( loc, Vec3f( cubeSize, cubeSize, cubeSize ) );
						
					}
				}

			}
		}
	}
	
	float size = mGridSize * mGridSpace;
	glColor4f( 0.4f, 0.4f, 0.4f, 1.0f );
	drawLineCube( Vec3f::zero(), Vec3f( size, size, size ) );
	
	
	gl::enableDepthWrite( false );
	gl::setMatricesWindow( getWindowSize() );

	glEnable( GL_TEXTURE_2D );
	mInfoPanel.update( mCounter );
	glDisable( GL_TEXTURE_2D );
	
	
	/*
	std::stringstream ss;
	ss << getHomeDirectory() << "FrustumCulling/image_" << mCounter << ".png";
	writeImage( ss.str(), copyWindowSurface() );
	*/
}






void FrustumCullingApp::drawLineCube( const Vec3f &center, const Vec3f &size )
{
	static GLfloat vertices[] = {	1, 1, 1,  1,-1, 1,  1,-1,-1,  1, 1,-1,
								   -1, 1, 1, -1,-1, 1, -1,-1,-1, -1, 1,-1 };


	static GLubyte elements[] = {	 0, 1, 1, 2, 2, 3, 3, 0,
									 4, 5, 5, 6, 6, 7, 7, 4,
									 0, 4, 1, 5, 2, 6, 3, 7 };
	
	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 3, GL_FLOAT, 0, vertices );

	gl::pushModelView();
		gl::translate( center );
		gl::scale( size * 0.5f );
		glDrawElements( GL_LINES, 24, GL_UNSIGNED_BYTE, elements );
	gl::popModelView();

	glDisableClientState( GL_VERTEX_ARRAY );
}



void FrustumCullingApp::calcNearAndFarClipCoordinates( const Camera &cam )
{
	Vec3f ntl, ntr, nbl, nbr;
	cam.getNearClipCoordinates( &ntl, &ntr, &nbl, &nbr );

	Vec3f ftl, ftr, fbl, fbr;
	cam.getFarClipCoordinates( &ftl, &ftr, &fbl, &fbr );
	
	if( ! mFrustumPlaneCached ){
		calcFrustumPlane( mFrustumPlaneNormals[TOP], mFrustumPlanePoints[TOP], mFrustumPlaneDists[TOP], ntr, ntl, ftl );
		calcFrustumPlane( mFrustumPlaneNormals[BOT], mFrustumPlanePoints[BOT], mFrustumPlaneDists[BOT], nbl, nbr, fbr );
		calcFrustumPlane( mFrustumPlaneNormals[LEF], mFrustumPlanePoints[LEF], mFrustumPlaneDists[LEF], ntl, nbl, fbl );
		calcFrustumPlane( mFrustumPlaneNormals[RIG], mFrustumPlanePoints[RIG], mFrustumPlaneDists[RIG], ftr, fbr, nbr );
		calcFrustumPlane( mFrustumPlaneNormals[NEA], mFrustumPlanePoints[NEA], mFrustumPlaneDists[NEA], ntl, ntr, nbr );
		calcFrustumPlane( mFrustumPlaneNormals[FARP], mFrustumPlanePoints[FARP], mFrustumPlaneDists[FARP], ftr, ftl, fbl );
	}
	
	mFrustumPlaneCached = true;
}

// need 'const'?
void FrustumCullingApp::calcFrustumPlane( Vec3f &fNormal, Vec3f &fPoint, float &fDist, const Vec3f &v1, const Vec3f &v2, const Vec3f &v3 )
{
	Vec3f aux1, aux2;

	aux1 = v1 - v2;
	aux2 = v3 - v2;

	fNormal = aux2.cross( aux1 );
	fNormal.normalize();
	fPoint.set( v2 );
	fDist = -( fNormal.dot( fPoint ) );
}



bool FrustumCullingApp::isPointInFrustum( const Vec3f &loc )
{
	float d;
	bool result = true;
			
	for( int i=0; i<6; i++ ){
		d = mFrustumPlaneDists[i] + mFrustumPlaneNormals[i].dot( loc );
		if( d < 0 )
			return( false );
	}

	return( result );
}


bool FrustumCullingApp::isSphereInFrustum( const Vec3f &loc, float radius )
{
	float d;
	bool result = true;
	
	for(int i=0; i<6; i++ ){
		d = mFrustumPlaneDists[i] + mFrustumPlaneNormals[i].dot( loc );
		if( d < -radius )
			return( false );
	}
	
	return( result );
}


bool FrustumCullingApp::isBoxInFrustum( const Vec3f &loc, const Vec3f &size )
{
	float d;
	int out, in;
	bool result = true;
	
	Vec3f vertex[8];
	
	vertex[0] = Vec3f( loc.x + size.x, loc.y + size.y, loc.z + size.z );
	vertex[1] = Vec3f( loc.x + size.x, loc.y - size.y, loc.z + size.z );
	vertex[2] = Vec3f( loc.x + size.x, loc.y - size.y, loc.z - size.z );
	vertex[3] = Vec3f( loc.x + size.x, loc.y + size.y, loc.z - size.z );
	vertex[4] = Vec3f( loc.x - size.x, loc.y + size.y, loc.z + size.z );
	vertex[5] = Vec3f( loc.x - size.x, loc.y - size.y, loc.z + size.z );
	vertex[6] = Vec3f( loc.x - size.x, loc.y - size.y, loc.z - size.z );
	vertex[7] = Vec3f( loc.x - size.x, loc.y + size.y, loc.z - size.z );
	
	for( int i=0; i<6; i++ ){
		out = 0;
		in = 0;

		for( int k=0; k<8 && ( in==0 || out==0 ); k++ ){
			d = mFrustumPlaneDists[i] + mFrustumPlaneNormals[i].dot( vertex[k] );
			
			if( d < 0 )
				out ++;
			else
				in ++;
		}

		if( !in )
			return( false );
	}
	
	return( result );
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
