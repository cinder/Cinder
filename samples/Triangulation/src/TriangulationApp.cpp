#include "cinder/app/AppBasic.h"
#include "cinder/Font.h"
#include "cinder/TriMesh.h"
#include "cinder/Triangulate.h"
#include "cinder/gl/Vbo.h"
#include "cinder/params/Params.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class TriangulationApp : public AppBasic {
  public:
	void		setup();
	void		draw();

	void		keyDown( KeyEvent event ) { setRandomGlyph(); }

	void		recalcMesh();
	
	void		setRandomFont();
	void		setRandomGlyph();
	
	Font				mFont;
	Shape2d				mShape;
	vector<string>		mFontNames;
	gl::VboMesh			mVboMesh;
	params::InterfaceGl	mParams;
	bool				mDrawWireframe;
	int					mFontSize;
	float				mZoom;
	float				mPrecision, mOldPrecision;
	int32_t				mNumPoints;
};

void TriangulationApp::setup()
{
	mParams = params::InterfaceGl( "Parameters", Vec2i( 220, 170 ) );
	mFontSize = 256;
	mDrawWireframe = true;
	mParams.addParam( "Draw Wireframe", &mDrawWireframe, "key=w" );
	mParams.addButton( "Random Font", bind( &TriangulationApp::setRandomFont, this ), "key=f" );
	mParams.addButton( "Random Glyph", bind( &TriangulationApp::setRandomGlyph, this ) );
	mZoom = 1.0f;
	mParams.addParam( "Zoom", &mZoom, "min=0.25 max=20 keyIncr=z keyDecr=Z" );
	mOldPrecision = mPrecision = 1.0f;
	mParams.addParam( "Precision", &mPrecision, "min=0.001 max=10 step=0.25 keyIncr=p keyDecr=P" );
	mNumPoints = 0;
	mParams.addParam( "Num Points", &mNumPoints, "", true );

	mFontNames = Font::getNames();
	mFont = Font( "Times", mFontSize );
	mShape = mFont.getGlyphShape( mFont.getGlyphChar( 'A' ) );
	
	// load VBO
	recalcMesh();
}

void TriangulationApp::recalcMesh()
{
	TriMesh2d mesh = Triangulator( mShape, mPrecision ).calcMesh( Triangulator::WINDING_ODD );
	mNumPoints = mesh.getNumIndices();
	mVboMesh = gl::VboMesh( mesh ); 
	mOldPrecision = mPrecision;
}

void TriangulationApp::setRandomFont()
{
	// select a random font from those available on the system
	mFont = Font( mFontNames[rand() % mFontNames.size()], mFontSize );
	setRandomGlyph();
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
	if( mOldPrecision != mPrecision )
		recalcMesh();

	gl::clear();
	gl::pushModelView();
		gl::translate( getWindowCenter() * Vec2f( 0.8f, 1.2f ) );
		gl::scale( Vec3f( mZoom, mZoom, mZoom ) );
		gl::color( Color( 0.8f, 0.4f, 0.0f ) );
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