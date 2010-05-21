#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include "cinder/app/AppBasic.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/DisplayList.h"
#include "CubeMap.h"

using namespace ci;
using namespace ci::app;
const int		CUBE_MAP_LOC = 4;
using std::vector;

class CubeMappingApp : public AppBasic {
  public:	
	gl::GlslProg		mShader;
	gl::DisplayList		mSphere, mTorus;
	
	//we're gonna have multiple cube maps
	vector<CubeMap>		mMaps;
	vector<CubeMap>::size_type	currentMap;
	float	theta;
	
	void prepareSettings( Settings *settings )
	{
		settings->setWindowSize( 1024, 768 );
		settings->setFullScreenSize( 1024, 768 );
		settings->setFullScreen( false );
		settings->setResizable( true );
		settings->setFrameRate( 30.0f );
	}

	void setup()
	{
		mSphere = gl::DisplayList( GL_COMPILE );
		mSphere.newList();
		gl::drawSphere( Vec3f( getWindowWidth() * 0.33f, getWindowHeight() * 0.5f, 0.0f ), 150.0f, 48 );
		mSphere.endList();
		
		mTorus = gl::DisplayList( GL_COMPILE );
		mTorus.newList();
		gl::drawTorus( 100.0f, 50.0f, 16, 32 );
		mTorus.endList();
		
		glEnable( GL_DEPTH_TEST );
		glDepthMask( GL_TRUE );
		
		try {
			mShader = gl::GlslProg( loadResource( "CubeMapping_vert.glsl" ), loadResource( "CubeMapping_frag.glsl" ) );
		}
		catch( ci::gl::GlslProgCompileExc &exc ) {
			std::cout << "Shader compile error: " << std::endl;
			std::cout << exc.what();
		}
		catch( ... ) {
			std::cout << "Unable to load shader" << std::endl;
		}
		
		currentMap = 0;
		
		mMaps.push_back( CubeMap( GLsizei(512), GLsizei(512),
									 Surface8u( loadImage( loadResource( "grandcanyon_positive_x.jpg" ) ) ),
									 Surface8u( loadImage( loadResource( "grandcanyon_positive_y.jpg" ) ) ),
									 Surface8u( loadImage( loadResource( "grandcanyon_positive_z.jpg" ) ) ),
									 Surface8u( loadImage( loadResource( "grandcanyon_negative_x.jpg" ) ) ),
									 Surface8u( loadImage( loadResource( "grandcanyon_negative_y.jpg" ) ) ),
									 Surface8u( loadImage( loadResource( "grandcanyon_negative_z.jpg" ) ) )
						));
		mMaps.push_back( CubeMap( GLsizei(128), GLsizei(128),
								 Surface8u( loadImage( loadResource( "berkeley_positive_x.png" ) ) ),
								 Surface8u( loadImage( loadResource( "berkeley_positive_y.png" ) ) ),
								 Surface8u( loadImage( loadResource( "berkeley_positive_z.png" ) ) ),
								 Surface8u( loadImage( loadResource( "berkeley_negative_x.png" ) ) ),
								 Surface8u( loadImage( loadResource( "berkeley_negative_y.png" ) ) ),
								 Surface8u( loadImage( loadResource( "berkeley_negative_z.png" ) ) )
						));
		mMaps.push_back( CubeMap( GLsizei(128), GLsizei(128),
								Surface8u( loadImage( loadResource( "solid-blue.png" ) ) ),
								Surface8u( loadImage( loadResource( "solid-cream.png" ) ) ),
								Surface8u( loadImage( loadResource( "solid-green.png" ) ) ),
								Surface8u( loadImage( loadResource( "solid-green-two.png" ) ) ),
								Surface8u( loadImage( loadResource( "solid-red.png" ) ) ),
								Surface8u( loadImage( loadResource( "solid-red-two.png" ) ) )
						));
	}
	
	void update()
	{
		mTorus.getModelMatrix().rotate( Vec3f( 1.0f, 0.5f, 0.2f ), 0.05f );
		theta+= 0.03f;
	}
	
	void draw()
	{
		if( ! mShader )
			return;
		glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		//	glEnable( GL_BLEND );
		glEnable( GL_MULTISAMPLE_ARB );
		//	glHint (GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
		
		mMaps[currentMap].bindMulti(CUBE_MAP_LOC);
		
		mShader.bind();
		
		mShader.uniform( "LightPos", Vec3f( getWindowWidth()/2 + 350.0f, getWindowHeight()/2 - 150.0f, 150.0f ) );
		mShader.uniform( "MixRatio", 0.5f );
		mShader.uniform( "EnvMap", CUBE_MAP_LOC );
		
		mShader.uniform( "BaseColor", Vec3f( 1.0f, 0.95f, 0.05f ) );
		mShader.uniform( "MixRatio", 0.4f );
		glPushMatrix();
		glTranslated( 0.0f, cos( theta )*50.0f, 0.0f );
		mSphere.draw();
		glPopMatrix();
		
		mShader.uniform( "BaseColor", Vec3f( 0.15f, 0.75f, 0.6f ) );
		mShader.uniform( "MixRatio", 0.2f );
		glPushMatrix();
		glTranslated( getWindowWidth() * 0.66f, getWindowHeight() * 0.5f, 0.0f );
		mTorus.draw();
		glPopMatrix();
		
		mMaps[currentMap].unbind();
	}	

	void mouseDown( app::MouseEvent event )
	{		
		if( event.isLeftDown() ) {
			nextMap();
		}
		else if( event.isRightDown() ) {
			hideCursor();
		}
		else if( event.isMiddleDown() ) {
			showCursor();
		}
	}
	
	void keyDown( app::KeyEvent event ) {
		if( event.getCode() == app::KeyEvent::KEY_f ) {
			setFullScreen( true );
		}
		else if( event.getCode() == app::KeyEvent::KEY_ESCAPE ) {
			setFullScreen( false );
		}
		
		if( event.getCode() == app::KeyEvent::KEY_p )
		{
			nextMap();
		}
	}
	
	void nextMap(){
		mMaps[currentMap].unbind();
		currentMap++;
		if( currentMap >= mMaps.size() ) currentMap = 0;
	}

};


CINDER_APP_BASIC( CubeMappingApp, RendererGl )