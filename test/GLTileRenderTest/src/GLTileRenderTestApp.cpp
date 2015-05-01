#include "cinder/app/App.h"
#include "cinder/Camera.h"
#include "cinder/Utilities.h"
#include "cinder/gl/TileRender.h"

using namespace ci;
using namespace ci::app;

class GLTileRenderTestApp : public App {
 public:
	void setup();
	void keyDown( KeyEvent event );

	void update();
	void drawFrame();
	void draw();
	
	CameraPersp mCam;
	float		mCurrentRotation;
};

static GLubyte smiley[] = // 16x16 smiley face, courtesy of OpenGL Super Bible
{
	0x03, 0xc0, 0, 0, /*       ****       */
	0x0f, 0xf0, 0, 0, /*     ********     */
	0x1e, 0x78, 0, 0, /*    ****  ****    */
	0x39, 0x9c, 0, 0, /*   ***  **  ***   */
	0x77, 0xee, 0, 0, /*  *** ****** ***  */
	0x6f, 0xf6, 0, 0, /*  ** ******** **  */
	0xff, 0xff, 0, 0, /* **************** */
	0xff, 0xff, 0, 0, /* **************** */
	0xff, 0xff, 0, 0, /* **************** */
	0xff, 0xff, 0, 0, /* **************** */
	0x73, 0xce, 0, 0, /*  ***  ****  ***  */
	0x73, 0xce, 0, 0, /*  ***  ****  ***  */
	0x3f, 0xfc, 0, 0, /*   ************   */
	0x1f, 0xf8, 0, 0, /*    **********    */
	0x0f, 0xf0, 0, 0, /*     ********     */
	0x03, 0xc0, 0, 0  /*       ****       */
};

void GLTileRenderTestApp::setup()
{
	CameraPersp cam;
	mCam.lookAt( vec3( 0, 0, 10 ), vec3::zero() );
	mCam.setPerspective( 60.0f, getWindowAspectRatio(), 1, 50 );
}

void GLTileRenderTestApp::keyDown( KeyEvent event )
{
	if( event.getChar() == ' ' ) {
		gl::TileRender tr( getWindowWidth() * 3, getWindowHeight() * 3, 11, 131 );
		tr.setMatrices( mCam );
		while( tr.nextTile() ) {
			drawFrame();
		}
		writeImage( getHomeDirectory() + "tileRenderOutput.png", tr.getSurface() );
	}
}

void GLTileRenderTestApp::update()
{
	mCurrentRotation = getElapsedSeconds() * 3;
}

void GLTileRenderTestApp::drawFrame()
{
	glClearColor( 0.1f, 0.1f, 0.3f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
		
	gl::pushModelView();
		glRotatef( mCurrentRotation, 0, 0, 1.0f );
		glBegin( GL_TRIANGLES );
			glColor3f( 1, 0, 0 );
			glVertex2f( 0, -5 );
			glColor3f( 0, 1, 0 );
			glVertex2f( 5, 5 );
			glColor3f( 0, 0, 1 );
			glVertex2f( -5, 5 );
		glEnd();
	gl::popModelView();
	
	glLineWidth( 17.0f );
	glColor3f( 1, 0.5f, 0.25f );
	glBegin( GL_LINE_LOOP );
		glVertex2f( 0, 5 );
		glVertex2f( -5, -5 );
		glVertex2f( 3, -3 );
		glVertex2f( 7, -7 );
		glVertex2f( 7, 7 );
		glVertex2f( -1, 2 );
	glEnd();	
	
	glColor3f( 1, 1, 0 );
	glRasterPos2i( 0, 0 );
	glBitmap( 16, 16, 0.0, 0.0, 0, 0, smiley );
}

void GLTileRenderTestApp::draw()
{
	gl::setMatrices( mCam );

	drawFrame();
}

CINDER_APP( GLTileRenderTestApp, RendererGL )
