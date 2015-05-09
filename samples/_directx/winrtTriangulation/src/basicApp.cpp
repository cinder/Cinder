#include "cinder/app/AppBasic.h"
#include "cinder/app/RendererDx.h"
#include "cinder/Font.h"
#include "cinder/TriMesh.h"
#include "cinder/Triangulate.h"

#include "cinder/dx/dx.h"
#include "cinder/dx/DxVbo.h"
#include "cinder/dx/DxTexture.h"
#include "cinder/Rand.h"

#include "cinder/params/Params.h"

using namespace ci;
using namespace ci::app;
using namespace std;

const int	MAX_ZOOM = 3;
const int	MAX_PRECISION = 6;

class BasicApp : public AppBasic {

  public:

	void			setup();
	void			draw();
	void		    keyDown( KeyEvent event );
	void			randomizeFont(char keyTyped);
	void			recalcMesh();
	void			setRandomFont();
	void			setRandomGlyph();

	Font			mGlyphFont;
	Font			mOutputFont;
	Font			mOutputTitle;

	Shape2d			mShape;
	vector<string>	mFontNames;
	dx::VboMesh		mVboMesh;
	params::InterfaceGl	mParams;

	void			cyclePrecision();
	void			cycleZoom();
	void			toggleWireframe();
	void			toggleFill();

	bool			mDrawWireframe;
	bool			mDrawFill;
	int				mFontSize;
	float			mZoom;
	float			mPrecision;

	int32_t			mNumPoints;
	int32_t			mFontIndex;
};

void BasicApp::setup()
{
	mFontSize = 256;
	mDrawWireframe = true;
	mDrawFill = true;
	mZoom = 2.0f;
	mPrecision = 2.0f;

	mNumPoints = 0;
	mOutputTitle = Font("Segoe UI", 40);
	mOutputFont = Font("Segoe UI", 20);

	mFontIndex = 0;
	mFontNames.push_back("Franklin Gothic");
	mFontNames.push_back("Times New Roman");
	mFontNames.push_back("Impact");
	mFontNames.push_back("Segoe UI");
	mFontNames.push_back("Consolas");

	randomizeFont('A');
	recalcMesh();

	mParams = params::InterfaceGl( "Parameters", Vec2i( 220, 170 ) );
	mParams.addParam( "Draw Wireframe", &mDrawWireframe, "min=1 max=2000 keyIncr== keyDecr=-" );
	//mParams.addButton( "Random Font", bind( &BasicApp::setRandomFont, this ), "key=f" );
	//mParams.addButton( "Random Glyph", bind( &BasicApp::setRandomGlyph, this ) );
	mParams.addParam( "Zoom", &mZoom, "min=0.01 max=20 keyIncr=z keyDecr=Z" );
	mParams.addParam( "Precision", &mPrecision, "min=0.01 max=20 keyIncr=p keyDecr=P" );
	mParams.addParam( "Num Points", &mNumPoints, "", true );
}

//void BasicApp::setRandomFont()
//{
//	// select a random font from those available on the system
//	mFont = Font( mFontNames[rand() % mFontNames.size()], mFontSize );
//	setRandomGlyph();
//}
//
//void BasicApp::setRandomGlyph()
//{
//	size_t glyphIndex = rand() % mFont.getNumGlyphs();
//	try {
//		mShape = mFont.getGlyphShape( glyphIndex );
//		recalcMesh();
//	}
//	catch( FontGlyphFailureExc &exc  ) {
//		console() << "Looks like glyph " << glyphIndex << " doesn't exist in this font." << std::endl;
//	}
//}

void BasicApp::recalcMesh() 
{
	TriMesh2d mesh = Triangulator( mShape, mPrecision ).calcMesh( Triangulator::WINDING_ODD );
	mNumPoints = mesh.getNumIndices();
	mVboMesh = dx::VboMesh( mesh );
}

void BasicApp::randomizeFont(char keyTyped)
{
	mFontIndex++;
	mFontIndex %= mFontNames.size();
	mGlyphFont = Font( mFontNames[mFontIndex], (float)mFontSize );
	mShape = mGlyphFont.getGlyphShape( mGlyphFont.getGlyphChar(keyTyped));
	recalcMesh();
}

void BasicApp::cyclePrecision() {
	mPrecision += 1.0;
	if ( mPrecision > MAX_PRECISION ) 
		mPrecision = 1.0f;
	recalcMesh();
}

void BasicApp::toggleWireframe() {
	mDrawWireframe = !mDrawWireframe;
}

void BasicApp::toggleFill() {
	mDrawFill = !mDrawFill;
}

void BasicApp::cycleZoom() {
	mZoom += 1.0;
	if ( mZoom > MAX_ZOOM ) {
		mZoom = 1.0f;
	}
}

void BasicApp::keyDown( KeyEvent event )
{
	switch ( event.getChar() )
	{
		case 'p':
		case 'P':
			cyclePrecision();
			return;
		case 'w':
		case 'W':
			toggleWireframe();
			return;
		case 'z':
		case 'Z':
			cycleZoom();
			return;
		case 'f':
		case 'F':
			toggleFill();
			return;

	}

	if ( event.getCode() >= 33 && event.getCode() <= 126 ) 
	{
		randomizeFont(event.getChar());
	}
}


void BasicApp::draw()
{

	dx::clear( Color( 0.1, 0.1, 0.1 ), true );
	dx::enableAlphaBlending();

	dx::pushModelView();

	dx::translate(getWindowCenter() * Vec2f( 1.06f - (mZoom*.12f), 1.1f + (mZoom*.12f) ));

	dx::scale( Vec3f( mZoom, mZoom, mZoom ) );
   
	if ( mDrawFill ) {
		dx::color( Color( 1.0f, 0.0f, 0.0f ) );
		dx::draw( mVboMesh );
	}

	if ( mDrawWireframe ) {
		dx::enableWireframe();
		dx::color( Color(1.0f, 1.0f, 1.0f) );
		dx::draw( mVboMesh );
		dx::disableWireframe();
	}

	dx::popModelView();

	std::stringstream s1;
	s1 << "Press 'w' to toggle the wireframe, 'f' to toggle the fill, 'p' to cycle through precisions, and 'z' to cycle through zoom levels. All other letters will cycle through selected fonts. ";
	std::stringstream s2;
	s2 << "Precision: " << mPrecision << "     Indices: " << mNumPoints << "     Fill: " << mDrawFill << "     Wireframe: " << mDrawWireframe << "      Zoom: " << mZoom << "     FPS: " << getAverageFps() << "     Font: " << mFontNames[mFontIndex];

	dx::drawString("WinRT: Testing getGlyphShape(), Shape2D and VBOMesh", Vec2f(30.0f, 60.f), Color::white(), mOutputTitle) ;
	dx::drawString(s1.str(), Vec2f(30.0, 100.0f), Color::white(), mOutputFont);
	dx::drawString(s2.str(), Vec2f(30.0, 130.0f), Color(0.0f, 1.0f, 0.6f), mOutputFont);
	
	mParams.draw();
}

CINDER_APP_BASIC( BasicApp, RendererDx )