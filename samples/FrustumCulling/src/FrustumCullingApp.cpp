#include "cinder/app/AppBasic.h"
#include "cinder/app/RendererGl.h"

#include "cinder/gl/gl.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/Vbo.h"

#include "cinder/Log.h"
#include "cinder/Camera.h"
#include "cinder/Utilities.h"
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
	void drawLineCube( const vec3 &center, const vec3 &size );
	void drawFrustum( const Camera &cam );
	
	bool isPointInFrustum( const vec3 &loc );
	bool isSphereInFrustum( const vec3 &loc, float radius );
	bool isBoxInFrustum( const vec3 &loc, const vec3 &size );
	
	bool mFrustumPlaneCached;

	void calcFrustumPlane( vec3 &fNormal, vec3 &fPoint, float &fDist, const vec3 &v1, const vec3 &v2, const vec3 &v3 );
	void calcNearAndFarClipCoordinates( const Camera &cam );	
	
	CameraPersp mCam;
	CameraPersp mRenderCam;
	
	vec3 mFrustumPlaneNormals[6];
	vec3 mFrustumPlanePoints[6];
	float mFrustumPlaneDists[6];
	
	float mFov, mNear, mFar;
	float mDecay;
	
	vec3 mEye, mEyeDest, mCenter, mEyeNormal;
	float mDistance;
	float mAngle, mAngleDest;
	
	vec3 mREye, mRCenter;
	vec3 mUp;

	vec3 mColor;
	
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
	
	gl::BatchRef mSphere, mCube;
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
		mFrustumPlaneNormals[i] = vec3( 0.0f );
		mFrustumPlanePoints[i] = vec3( 0.0f );
		mFrustumPlaneDists[i] = 0.0f;
	}
	mFrustumPlaneCached = false;
	
	mIsWatchingCam	= true;
	
	mShapeType		= SPHERE;
	
	mGridSize		= 31;
	mHalfGrid		= (int)(mGridSize/2);
	mGridSpace		= 15.0f;
	
	mCounter		= 0.0f;
	
	mColor			= vec3( 0.0f, 0.0f, 0.0f );
	
	
	mFov		= 60.0f;
	mNear		= 10.0f;
	mFar		= 1500.0f;
	mDecay		= 0.25f;
	// mClipCam Eye and Center
	mEye		= vec3( 0.0f, 0.0f, -100.0f );
	mEyeDest	= vec3( 0.0f, 0.0f, -100.0f );
	mCenter		= vec3( 0.0f, 0.0f, 100.0f );
	mEyeNormal	= vec3();
	
	mDistance	= 500.0f;
	mAngle		= 0.0f;
	mAngleDest	= 0.0f;
	
	
	// mRenderCam Eye and Center
	mREye		= vec3( 10.0f, 10.0f, 10.0f );
	mRCenter	= vec3( 0.0f );
	
	mUp			= vec3( 0.0f, 1.0f, 0.0f );
	
	auto glsl = gl::getStockShader( gl::ShaderDef().color() );
	
	mSphere = gl::Batch::create( geom::Sphere().segments( 60 ).disable( geom::NORMAL ).disable( geom::COLOR ), glsl );
	mCube = gl::Batch::create( geom::Cube().disable( geom::NORMAL ).disable( geom::COLOR ), glsl );

	gl::clearColor( ColorA( 0.1f, 0.1f, 0.1f, 1.0f ) );
	gl::enableDepthRead( true );
	gl::enableDepthWrite( true );
	gl::enableAlphaBlending();
}


void FrustumCullingApp::update()
{
	mFrustumPlaneCached = false;
	mAngle -= ( mAngle - mAngleDest ) * 0.1f;
	mEye -= ( mEye - mEyeDest ) * 0.2f;
	
	mEyeNormal = vec3( sin( mAngle ), 0.0f, cos( mAngle ) ); 
	mCenter = mEye + mEyeNormal * 50.0f;
	
	mCam.setPerspective( 25.0f, getWindowAspectRatio(), 100.0f, 350.0f );
	mCam.lookAt( mEye, mCenter, mUp );
	calcNearAndFarClipCoordinates( mCam );
	
	
	
	if( mIsWatchingCam ){
		mREye = lerp( mREye, vec3( mEye.x + cos( mCounter * 0.003f ) * 300.0f, 100.0f, mEye.y + sin( mCounter * 0.003f ) * 300.0f ), mDecay );
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
	mRenderCam.lookAt( mREye, mRCenter, mUp );
		
	gl::setMatrices( mRenderCam );

	
	
	mCounter += 1.0f;
}


void FrustumCullingApp::draw()
{
	gl::enableDepthWrite( true );
	gl::clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	gl::color( 1.0f, 1.0f, 1.0f, 1.0f );
	if( mIsWatchingCam )
		drawFrustum( mCam );


	for( int x=0; x<mGridSize; x++ ){
		for( int y=0; y<mGridSize; y++ ){
			for( int z=0; z<mGridSize; z++ ){
				vec3 loc( (x-mHalfGrid)*mGridSpace, (y-mHalfGrid/2)*mGridSpace, (z-mHalfGrid)*mGridSpace );
				
				if( mShapeType == POINT ){
					if( isPointInFrustum( loc ) ){
						gl::ScopedModelMatrix scopeModel;
						gl::color( 0.0f, 1.0f, 0.0f );
						gl::multModelMatrix( translate( loc ) * scale( vec3( 0.5f ) ) );
						mCube->draw();
					}
					
				} else if( mShapeType == SPHERE ){
					if( isSphereInFrustum( loc, 5.0f ) ){
						gl::ScopedModelMatrix scopeModel;
						gl::color( 0.0f, 1.0f, 0.0f );
						gl::multModelMatrix( translate( loc ) * scale( vec3( 5.0f ) ) );
						mSphere->draw();
					}
					
				} else if( mShapeType == CUBE ){
					int cubeSize = 4.0f;
					if( isBoxInFrustum( loc, vec3( cubeSize ) ) ){
						gl::ScopedModelMatrix scopeModel;
						gl::color( 0.0f, 1.0f, 0.0f );
						gl::multModelMatrix( translate( loc ) * scale( vec3( cubeSize ) ) );
						mCube->draw();
						
					}
				}

			}
		}
	}
	
	float size = mGridSize * mGridSpace;
	gl::color( 0.4f, 0.4f, 0.4f, 1.0f );
	drawLineCube( vec3( 0.0f ), vec3( size, size, size ) );
	
	gl::enableDepthWrite( false );
	gl::setMatricesWindow( getWindowSize() );

	mInfoPanel.update( mCounter );
	
	/*
	std::stringstream ss;
	ss << getHomeDirectory() << "FrustumCulling/image_" << mCounter << ".png";
	writeImage( ss.str(), copyWindowSurface() );
	*/
}






void FrustumCullingApp::drawLineCube( const vec3 &center, const vec3 &size )
{
	using namespace std;
	static array<GLfloat, 24> vertices = { 1, 1, 1,  1,-1, 1,  1,-1,-1,  1, 1,-1,
								          -1, 1, 1, -1,-1, 1, -1,-1,-1, -1, 1,-1 };

	static array<GLubyte, 24> elements = { 0, 1, 1, 2, 2, 3, 3, 0,
								           4, 5, 5, 6, 6, 7, 7, 4,
								           0, 4, 1, 5, 2, 6, 3, 7 };
	
	static gl::VboMeshRef mLineCube;
	
	if ( ! mLineCube ) {
		geom::BufferLayout layout( { geom::AttribInfo( geom::Attrib::POSITION, geom::DataType::FLOAT, 3, 0, 0) } );
		auto verts = gl::Vbo::create( GL_ARRAY_BUFFER,
									 vertices.size() * sizeof(GLfloat),
									 vertices.data(),
									 GL_STATIC_DRAW );
		auto vertexArrayBuffer = make_pair( layout, verts );
		
		auto indices = gl::Vbo::create( GL_ELEMENT_ARRAY_BUFFER,
									   elements.size() * sizeof(GLubyte),
									   elements.data(),
									   GL_STATIC_DRAW );
		
		mLineCube = gl::VboMesh::create( 8,
										GL_LINE_LOOP,
										{ vertexArrayBuffer },
										elements.size(),
										GL_UNSIGNED_BYTE,
										indices );
	}

	gl::pushModelView();
		gl::translate( center );
		gl::scale( size * 0.5f );
		gl::draw( mLineCube );
	gl::popModelView();
}



void FrustumCullingApp::calcNearAndFarClipCoordinates( const Camera &cam )
{
	vec3 ntl, ntr, nbl, nbr;
	cam.getNearClipCoordinates( &ntl, &ntr, &nbl, &nbr );

	vec3 ftl, ftr, fbl, fbr;
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
void FrustumCullingApp::calcFrustumPlane( vec3 &fNormal, vec3 &fPoint, float &fDist, const vec3 &v1, const vec3 &v2, const vec3 &v3 )
{
	vec3 aux1, aux2;

	aux1 = v1 - v2;
	aux2 = v3 - v2;

	fNormal = normalize( cross( aux2, aux1 ) );
	fPoint = v2;
	fDist = -( dot( fNormal, fPoint ) );
}



bool FrustumCullingApp::isPointInFrustum( const vec3 &loc )
{
	float d;
	bool result = true;
			
	for( int i=0; i<6; i++ ){
		d = mFrustumPlaneDists[i] + dot( mFrustumPlaneNormals[i], loc );
		if( d < 0 )
			return( false );
	}

	return( result );
}


bool FrustumCullingApp::isSphereInFrustum( const vec3 &loc, float radius )
{
	float d;
	bool result = true;
	
	for(int i=0; i<6; i++ ){
		d = mFrustumPlaneDists[i] + dot( mFrustumPlaneNormals[i], loc );
		if( d < -radius )
			return( false );
	}
	
	return( result );
}


bool FrustumCullingApp::isBoxInFrustum( const vec3 &loc, const vec3 &size )
{
	float d;
	int out, in;
	bool result = true;
	
	vec3 vertex[8];
	
	vertex[0] = vec3( loc.x + size.x, loc.y + size.y, loc.z + size.z );
	vertex[1] = vec3( loc.x + size.x, loc.y - size.y, loc.z + size.z );
	vertex[2] = vec3( loc.x + size.x, loc.y - size.y, loc.z - size.z );
	vertex[3] = vec3( loc.x + size.x, loc.y + size.y, loc.z - size.z );
	vertex[4] = vec3( loc.x - size.x, loc.y + size.y, loc.z + size.z );
	vertex[5] = vec3( loc.x - size.x, loc.y - size.y, loc.z + size.z );
	vertex[6] = vec3( loc.x - size.x, loc.y - size.y, loc.z - size.z );
	vertex[7] = vec3( loc.x - size.x, loc.y + size.y, loc.z - size.z );
	
	for( int i=0; i<6; i++ ){
		out = 0;
		in = 0;

		for( int k=0; k<8 && ( in==0 || out==0 ); k++ ){
			d = mFrustumPlaneDists[i] + dot( mFrustumPlaneNormals[i], vertex[k] );
			
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

void FrustumCullingApp::drawFrustum( const Camera &cam )
{
	vec3 nearTopLeft, nearTopRight, nearBottomLeft, nearBottomRight;
	cam.getNearClipCoordinates( &nearTopLeft, &nearTopRight, &nearBottomLeft, &nearBottomRight );
	
	vec3 farTopLeft, farTopRight, farBottomLeft, farBottomRight;
	cam.getFarClipCoordinates( &farTopLeft, &farTopRight, &farBottomLeft, &farBottomRight );
	
	vec3 eye = cam.getEyePoint();
	
	std::array<vec3, 9> vertices = { eye, nearTopLeft, nearTopRight, nearBottomLeft, nearBottomRight,
		farTopLeft, farTopRight, farBottomLeft, farBottomRight };
	std::array<GLubyte, 32> indices = { 0, 1, 0, 2, 0, 4, 0, 3,
										5, 1, 6, 2, 7, 3, 8, 4,
										1, 2, 2, 4, 4, 3, 3, 1,
										5, 6, 6, 8, 8, 7, 7, 5};
	
	auto ctx = ci::gl::context();
	gl::GlslProgRef curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}
	
	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();
	gl::VboRef defaultVbo = ctx->getDefaultArrayVbo( sizeof(vec3)*9 );
	gl::VboRef elementVbo = ctx->getDefaultElementVbo( 32 );
	gl::ScopedBuffer bufferBindScp( defaultVbo );
	defaultVbo->bufferSubData( 0, sizeof(vec3)*9, vertices.data() );
	
	elementVbo->bind();
	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		gl::enableVertexAttribArray( posLoc );
		gl::vertexAttribPointer( posLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
	}
	
	elementVbo->bufferSubData( 0, 32, indices.data() );
	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	ctx->drawElements( GL_LINES, 32, GL_UNSIGNED_BYTE, 0 );
	ctx->popVao();
}


CINDER_APP_BASIC( FrustumCullingApp, RendererGl )