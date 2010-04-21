#include "flint/app/AppScreenSaver.h"
#include "flint/Color.h"

using namespace fli;
using namespace fli::app;

class SimpleApp : public AppScreenSaver {
 public:
	virtual void setup();
	virtual void update();
	virtual void draw();
	
 protected:
	fli::Color	mColor, mBackgroundColor;
	int			mSubdivision;
	float		mRadius;
	float		mAnim;
};


void SimpleApp::setup()
{
	mSubdivision = 50;
	mBackgroundColor = Color( 0.0, 0.0f, 0.15f );
	mColor = Color( 1.0f, 0.5f, 0.25f );
	mAnim = 0.0f;
}

void SimpleApp::update()
{
	mAnim += 0.01f;
	mRadius = abs( cos( mAnim ) * 200 );
}

void SimpleApp::draw()
{
	glClearColor( mBackgroundColor.r, mBackgroundColor.g, mBackgroundColor.b, 1.0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	

	glColor3fv( mColor );
	glBegin( GL_POLYGON );
	for( int s = 0; s < mSubdivision; ++s ) {
		glVertex3f( getWindowWidth() / 2 + mRadius * cos( s / (float)mSubdivision * 6.28f ), getWindowHeight() / 2 + mRadius * sin( s / (float)mSubdivision * 6.28f ) + mRadius, 0 );
	}
	glEnd();
}

FLI_APP_SCREENSAVER( SimpleApp )
