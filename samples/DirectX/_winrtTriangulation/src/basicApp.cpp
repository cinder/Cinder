#include "cinder/app/AppBasic.h"
#include "cinder/Font.h"
#include "cinder/TriMesh.h"
#include "cinder/Triangulate.h"

#include "cinder/dx/dx.h"
#include "cinder/dx/DxVbo.h"
#include "cinder/dx/DxTexture.h"
#include "cinder/Rand.h"

//#include "cinder/params/Params.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BasicApp : public AppBasic {

  public:

	void			setup();
	void			draw();
	void		    keyDown( KeyEvent event ) { setRandomFont(); }

	void			recalcMesh();
	void			setRandomGlyph();
	void			setRandomFont();

	Font			mFont;
	Font			mOutputFont;

	//params::InterfaceGl	mParams;

	Shape2d			mShape;
	vector<string>	mFontNames;

	dx::VboMesh		mVboMesh;

	bool			mDrawWireframe;
	int				mFontSize;
	float			mZoom;
	float			mPrecision, mOldPrecision;
	int32_t			mNumPoints;

	int32_t			mFontIndex;
};

void BasicApp::setup()
{
	srand (time(NULL));

	mFontSize = 256;
	mDrawWireframe = true;
	mZoom = 3.0f;
	mOldPrecision = mPrecision = 2.0f;
	mNumPoints = 0;
	mFontNames = Font::getNames();
	
	mFont = Font( "Segoe UI", mFontSize );
	mOutputFont = Font("Arial", 18);
	mFontIndex = 0;

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
	size_t glyphIndex = mFont.getNumGlyphs();
		
	try {
		//mShape = mFont.getGlyphShape( glyphIndex );
		mShape = mFont.getGlyphShape( mFont.getGlyphChar('r'));

		recalcMesh();
	}
	catch ( FontGlyphFailureExc &exc ) 
	{
		console() << "Looks like glyph " << glyphIndex << " doesn't exist in this font." << std::endl;
	}
}

void BasicApp::setRandomFont()
{

	//mFont = Font( mFontNames[rand() % mFontNames.size()], mFontSize );

	mFontIndex++;
	mFontIndex %= 5;
	mFontIndex = 100;

	mFont = Font( "Segoe UI", mFontSize );
	setRandomGlyph();

	return;

	mShape = mFont.getGlyphShape( mFont.getGlyphChar('B'));
	recalcMesh();

	switch ( mFontIndex )
	{
		case 0:
	mFont = Font( "Segoe UI", mFontSize );
		break;
		case 1:
				mFont = Font( "Segoe UI", mFontSize );

			mFont = Font( "Cambria", mFontSize );
		break;
		case 2:
		mFont = Font( "Segoe UI", mFontSize );
	mFont = Font( "Consolas", mFontSize );
		break;
		case 3:
		mFont = Font( "Arial", mFontSize );
		break;
		case 4:
		mFont = Font( "Impact", mFontSize );
		break;
	}

	//setRandomGlyph();
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

	dx::clear( Color( 0.1, 0, 0 ), true );
	dx::pushModelView();

	dx::translate(getWindowCenter() * Vec2f( 0.7f, 0.7f ));
	// glyph is currenty returned upside down, so you can invert like this for now
	// ( notice the Y is scaled at negative 100% suggested value ):
	// dx::scale( Vec3f( mZoom, -mZoom, mZoom ) );

	dx::scale( Vec3f( mZoom, mZoom, mZoom ) );

	dx::color( Color( 0.65f, 0.4f, 0.0f ) );
	dx::draw( mVboMesh );

	dx::translate(150.0f, 0.0f);

	if ( mDrawWireframe ) {
		dx::enableWireframe();
		dx::color( Color(1.0f, 0.0f, 0.0f) );
		dx::draw( mVboMesh );
		dx::disableWireframe();
	}

	dx::popModelView();

	std::stringstream s;
	s << "Press any key to toggle Font Face and Glyph  [ " << getAverageFps() << " FPS ]";
	dx::color( Color::white() );
	dx::drawString(s.str(),Vec2f(10.0f,10.0f),Color(0.0f, 0.0f, 0.1f) ,mOutputFont);

	//mParams.draw();
}

CINDER_APP_BASIC( BasicApp, RendererDx )