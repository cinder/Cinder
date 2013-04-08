#include "cinder/app/AppBasic.h"
#include "cinder/Font.h"
#include "cinder/TriMesh.h"
#include "cinder/Triangulate.h"

#include "cinder/dx/dx.h"
#include "cinder/dx/DxVbo.h"
#include "cinder/dx/DxTexture.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BasicApp : public AppBasic {

  public:

	void			setup();
	void			draw();
	void		    keyDown( KeyEvent event ) { setRandomGlyph(); }

	void			recalcMesh();
	void			setRandomGlyph();
	void			setRandomFont();

	Font			mFont;
	Shape2d			mShape;
	vector<string>	mFontNames;
	dx::VboMesh		mVboMesh;

	bool			mDrawWireframe;
	int				mFontSize;
	float			mZoom;
	float			mPrecision, mOldPrecision;
	int32_t			mNumPoints;
};

void BasicApp::setup()
{
	mFontSize = 256;
	mDrawWireframe = true;
	mZoom = 1.0f;
	mOldPrecision = mPrecision = 1.0f;
	mNumPoints = 0;
	mFontNames = Font::getNames();
	mFont = Font( "Arial", mFontSize );
	mShape = mFont.getGlyphShape( mFont.getGlyphChar('A'));

	// load VBO
	recalcMesh();
}

void BasicApp::recalcMesh() 
{
	TriMesh2d mesh = Triangulator( mShape, mPrecision ).calcMesh( Triangulator::WINDING_ODD );
	mNumPoints = mesh.getNumIndices();
	mVboMesh = dx::VboMesh( mesh );
	mOldPrecision = mPrecision;
}

void BasicApp::setRandomGlyph()
{
	size_t glyphIndex = rand() % mFont.getNumGlyphs();
	try {
		mShape = mFont.getGlyphShape( glyphIndex );
		recalcMesh();
	}
	catch ( FontGlyphFailureExc &exc ) 
	{
		console() << "Looks like glyph " << glyphIndex << " doesn't exist in this font." << std::endl;
	}
}

void BasicApp::setRandomFont()
{
	mFont = Font( mFontNames[rand() % mFontNames.size()], mFontSize );
	setRandomGlyph();
}

/*
void BasicApp::keyDown( KeyEvent event )
{
	switch( event.getChar() ) {
		case 'r':

			break;
	}
}
*/
void BasicApp::draw()
{
	if ( mOldPrecision != mPrecision )
			recalcMesh();

	dx::clear( Color( 0, 0, 0 ), true );
	dx::pushModelView();

	dx::translate(getWindowCenter() * Vec2f( 0.8f, 1.2f ));
	//dx::scale( Vec3f( mZoom, mZoom, mZoom ) );
	dx::color( Color( 0.8f, 0.4f, 0.0f ) );
	dx::draw( mVboMesh );

	if ( mDrawWireframe ) {
		dx::enableWireframe();
		dx::color( Color::white() );
		dx::draw( mVboMesh );
		dx::disableWireframe();
	}

	dx::popModelView();

	std::stringstream s;
	s << "Framerate:" << getAverageFps();
	dx::drawString(s.str(),Vec2f(10.0f,10.0f),Color::white(),mFont);
}

CINDER_APP_BASIC( BasicApp, RendererDx )