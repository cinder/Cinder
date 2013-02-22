#include "cinder/Cinder.h"
#include "cinder/app/AppScreenSaver.h"
#include "cinder/Color.h"

using namespace ci;
using namespace ci::app;

class _TBOX_PREFIX_App : public AppScreenSaver {
  public:
	virtual void setup();
	virtual void update();
	virtual void draw();
	
  protected:
	ci::Color	mColor, mBackgroundColor;
	float		mRadius;
};


void _TBOX_PREFIX_App::setup()
{
	mColor = Color( 1.0f, 0.5f, 0.25f );
	mBackgroundColor = Color( 0.25f, 0.0f, 0.0f );
}

void _TBOX_PREFIX_App::update()
{
	mRadius = abs( cos( getElapsedSeconds() ) * 200 );
}

void _TBOX_PREFIX_App::draw()
{
	gl::clear( mBackgroundColor );
	gl::color( mColor );
	gl::drawSolidCircle( getWindowCenter(), mRadius );
}


CINDER_APP_SCREENSAVER( _TBOX_PREFIX_App, RendererGl )
