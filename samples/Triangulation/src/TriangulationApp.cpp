#include "cinder/app/AppBasic.h"
#include "cinder/cairo/Cairo.h"
#include "cinder/Font.h"
#include "cinder/Utilities.h"
#include "cinder/ImageIo.h"
#include "cinder/TriMesh.h"
#include "cinder/Triangulate.h"
#include "cinder/gl/Vbo.h"
#include "cinder/params/Params.h"

using namespace ci;
using namespace ci::app;
using namespace std;
#include <sstream>

class TriangulationApp : public AppBasic {
 public:
	void		setup();
	void		draw();

	void		keyDown( KeyEvent event ) { if( event.getChar() == 'f' ) { setRandomFont(); setRandomGlyph(); } else setRandomGlyph(); }

	void		recalcMesh();
	
	void		setRandomFont();
	void		setRandomGlyph();
	
	Font				mFont;
	Shape2d				mShape;
	vector<string>		mFontNames;
	gl::VboMesh			mVboMesh, mPointsMesh;
	params::InterfaceGl	mParams;
	bool				mDrawWireframe;
	int					mFontSize;
};

void TriangulationApp::setup()
{
	mParams = params::InterfaceGl( "Parameters", Vec2i( 200, 400 ) );
	mFontSize = 256;
	mParams.addParam( "Font Size", &mFontSize, "min=1 max=2000 keyIncr== keyDecr=-" );
	mDrawWireframe = true;
	mParams.addParam( "Draw Wireframe", &mDrawWireframe, "min=1 max=2000 keyIncr== keyDecr=-" );

	mFontNames = Font::getNames();
	mFont = Font( "Times", mFontSize );
	mShape = mFont.getGlyphShape( mFont.getGlyphChar( 'A' ) );
	
	// setup VBO
	gl::VboMesh::Layout layout;
	layout.setStaticPositions();
	recalcMesh();
}

void TriangulationApp::recalcMesh()
{
	mVboMesh = gl::VboMesh( Triangulator( mShape ).calcMesh( Triangulator::WINDING_ODD ) ); 
}

void TriangulationApp::setRandomFont()
{
	// select a random font from those available on the system
	mFont = Font( mFontNames[rand() % mFontNames.size()], mFontSize );

	// exercise the code which determines which glyphs map to a given string
	console() << "In this font, the string 'abc123' is glyphs: " << std::endl;
	vector<Font::Glyph> glyphs = mFont.getGlyphs( "abc123" );
	for( vector<Font::Glyph>::const_iterator it = glyphs.begin(); it != glyphs.end(); ++it )
		console() << *it << " ";
	console() << endl;
}

void TriangulationApp::setRandomGlyph()
{
	size_t glyphIndex = rand() % mFont.getNumGlyphs();
	try {
		mShape = mFont.getGlyphShape( glyphIndex );
		recalcMesh();
	}
	catch( FontGlyphFailureExc &exc  ) {
		console() << "Looks like glyph " << glyphIndex << " doesn't exist in this font." << std::endl;
	}
}

void TriangulationApp::draw()
{
	gl::clear();
	gl::pushModelView();
		gl::translate( getWindowCenter() );
		gl::color( Color( 0, 0, 0.8f ) );
		gl::draw( mVboMesh );
		if( mDrawWireframe ) {
			gl::enableWireframe();
			gl::color( Color::white() );
			gl::draw( mVboMesh );
			gl::disableWireframe();
		}
	gl::popModelView();
	
	mParams.draw();
}


CINDER_APP_BASIC( TriangulationApp, RendererGl )