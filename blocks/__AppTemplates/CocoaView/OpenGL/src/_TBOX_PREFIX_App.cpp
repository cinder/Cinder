#include "_TBOX_PREFIX_App.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;

void _TBOX_PREFIX_App::setup()
{
	mRadius = 50;
	mColor = Color( 1.0f, 0.5f, 0.25f );
}

void _TBOX_PREFIX_App::draw()
{
	gl::clear();

	gl::color( mColor );
	gl::drawSolidCircle( getWindowCenter(), mRadius );
}
