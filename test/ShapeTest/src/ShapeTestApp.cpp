#include "cinder/Font.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ShapeTestApp : public App {
  public:
	void setup();
	void draw();

	void mouseMove( MouseEvent event ) override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void mouseWheel( MouseEvent event ) override;

	void keyDown( KeyEvent event ) override;

	void setRandomFont();
	void setRandomGlyph();

	void reposition( const vec2 &point );
	void calculate();
	void calculateModelMatrix();

	Font           mFont;
	Shape2d        mShape;
	vector<string> mFontNames;
	int            mFontSize;
	float          mZoom;
	float          mDistance;
	vec2           mMouse, mClick;
	vec2           mClosest;
	vec2           mAnchor, mPosition, mOriginal;
	mat4           mModelMatrix;
	bool           mIsInside;
};

void ShapeTestApp::setup()
{
	mPosition = getWindowCenter() * vec2( 0.8f, 1.2f );
	mDistance = 0;
	mZoom = 1;
	mIsInside = false;

	mFontNames = Font::getNames();
	mFontSize = 256;
	mFont = Font( "Times", (float)mFontSize );
	mShape = mFont.getGlyphShape( mFont.getGlyphChar( 'A' ) );

	calculate();
}

void ShapeTestApp::setRandomFont()
{
	// select a random font from those available on the system
	mFont = Font( mFontNames[rand() % mFontNames.size()], (float)mFontSize );
	setRandomGlyph();
}

void ShapeTestApp::setRandomGlyph()
{
	size_t glyphIndex = rand() % mFont.getNumGlyphs();
	try {
		mShape = mFont.getGlyphShape( glyphIndex );
	}
	catch( FontGlyphFailureExc & ) {
		console() << "Looks like glyph " << glyphIndex << " doesn't exist in this font." << std::endl;
	}
}

void ShapeTestApp::draw()
{
	gl::clear();
	gl::pushModelView();
	gl::setModelMatrix( mModelMatrix );

	gl::color( mIsInside ? Color( 0.4f, 0.8f, 0.0f ) : Color( 0.8f, 0.4f, 0.0f ) );
	gl::draw( mShape );

	gl::popModelView();

	gl::color( 0, 1, 1 );
	gl::drawSolidCircle( mClosest, 5 );

	gl::color( 1, 1, 0 );
	gl::drawStrokedCircle( mMouse, mDistance );
}

void ShapeTestApp::mouseMove( MouseEvent event )
{
	mMouse = event.getPos(); 
	
	calculate();
}

void ShapeTestApp::mouseDown( MouseEvent event )
{
	reposition( event.getPos() );

	mClick = event.getPos();
	mOriginal = mPosition;

	calculate();
}

void ShapeTestApp::mouseDrag( MouseEvent event )
{
	mPosition = mOriginal + vec2( event.getPos() ) - mClick;
	mMouse = event.getPos();

	calculate();
}

void ShapeTestApp::mouseWheel( MouseEvent event )
{
	reposition( event.getPos() );

	mMouse = event.getPos();
	mZoom *= 1.0f + 0.1f * event.getWheelIncrement();

	calculate();
}

void ShapeTestApp::keyDown( KeyEvent event )
{
	setRandomGlyph();

	calculate();
}

void ShapeTestApp::reposition( const vec2 &mouse )
{
	// Convert mouse to object space.
	mat4 invModelMatrix = glm::inverse( mModelMatrix );
	vec2 anchor = vec2( invModelMatrix * vec4( mouse, 0, 1 ) );

	// Calculate new position, anchor and scale.
	mPosition += vec2( mModelMatrix * vec4( anchor - mAnchor, 0, 0 ) );
	mAnchor = anchor;
}

void ShapeTestApp::calculate()
{
	calculateModelMatrix();

	auto local = vec2( glm::inverse( mModelMatrix ) * vec4( mMouse, 0, 1 ) );
	mIsInside = mShape.contains( local );
	mClosest = vec2( mModelMatrix * vec4( mShape.calcClosestPoint( local ), 0, 1 ) );
	mDistance = glm::distance( mClosest, mMouse );
}

void ShapeTestApp::calculateModelMatrix()
{
	// Update model matrix.
	mModelMatrix = glm::translate( vec3( mPosition, 0 ) );
	mModelMatrix *= glm::scale( vec3( mZoom ) );
	mModelMatrix *= glm::translate( vec3( -mAnchor, 0 ) );
}

CINDER_APP( ShapeTestApp, RendererGl )