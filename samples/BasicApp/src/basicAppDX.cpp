#include "cinder/app/AppBasic.h"
#include "cinder/app/RendererDx.h"
#include "cinder/dx/dx.h"
#include "cinder/Surface.h"
#include "cinder/dx/DxTexture.h"
#include <list>

using namespace ci;
using namespace ci::app;
using namespace std;

// We'll create a new Cinder Application by deriving from the AppBasic class
class BasicApp : public AppBasic {
  public:
	void mouseDrag( MouseEvent event );
	void keyDown( KeyEvent event );
	void draw();

	// This will maintain a list of points which we will draw line segments between
	list<Vec2f>		mPoints;
};

void BasicApp::mouseDrag( MouseEvent event )
{
	mPoints.push_back( event.getPos() );
}

void BasicApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' )
		setFullScreen( ! isFullScreen() );
}

void BasicApp::draw()
{
	dx::clear( Color( 0.1f, 0.1f, 0.15f ) );

	dx::color( 1.0f, 0.5f, 0.25f );	
	dx::begin( GL_LINE_STRIP );
	for( auto pointIter = mPoints.begin(); pointIter != mPoints.end(); ++pointIter ) {
		dx::vertex( *pointIter );
	}
	dx::end();
#if 0
	Surface s = getRenderer()->copyWindowSurface(Area(Vec2i(0, 0), Vec2i(500, 500)));
	dx::Texture tex(s);
	dx::draw(tex, Rectf(100, 100, 600, 600));  
#endif // 0

}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( BasicApp, RendererDx )