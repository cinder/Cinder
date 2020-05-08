#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Font.h"
#include "cinder/TriMesh.h"
#include "cinder/Triangulate.h"
#include "cinder/gl/gl.h"
#include "cinder/CinderImGui.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class TriangulationApp : public App {
  public:
	void		setup();
	void		update();
	void		draw();

	void		keyDown( KeyEvent event ) { setRandomGlyph(); }

	void		recalcMesh();
	
	void		setRandomFont();
	void		setRandomGlyph();
	
	Font				mFont;
	Shape2d				mShape;
	vector<string>		mFontNames;
	gl::VboMeshRef		mVboMesh;
	bool				mDrawWireframe = true;
	int					mFontSize = 256;
	float				mZoom = 1.0f;
	float				mPrecision = 1.0f;
	float				mOldPrecision = 1.0f;
	int					mNumPoints = 0;
};

void TriangulationApp::setup()
{
	ImGui::Initialize();
	
	mFontNames = Font::getNames();
	mFont = Font( "Times", mFontSize );
	mShape = mFont.getGlyphShape( mFont.getGlyphChar( 'A' ) );
	
	// load VBO
	recalcMesh();
}

void TriangulationApp::recalcMesh()
{
	TriMesh mesh = Triangulator( mShape, mPrecision ).calcMesh( Triangulator::WINDING_ODD );
	mNumPoints = mesh.getNumIndices();
	mVboMesh = gl::VboMesh::create( mesh ); 
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

void TriangulationApp::update()
{
	ImGui::SetNextWindowCollapsed( true, ImGuiCond_FirstUseEver );
	ImGui::Begin( "Settings" );
	ImGui::Checkbox( "Draw Wireframe", &mDrawWireframe );
	if( ImGui::Button( "Random Font" ) ) setRandomFont();
	if( ImGui::Button( "Random Glyph" ) ) setRandomGlyph();
	ImGui::DragFloat( "Zoom", &mZoom, 0.1f, 0.25f, 20.0f );
	ImGui::DragFloat( "Precision", &mPrecision, 0.25f, 0.001f, 10.0f );
	ImGui::Text( "Num Points %i", &mNumPoints );
	ImGui::End();
}

void TriangulationApp::draw()
{
	if( mOldPrecision != mPrecision )
		recalcMesh();

	gl::clear();
	gl::pushModelView();
		gl::translate( getWindowCenter() * vec2( 0.8f, 1.2f ) );
		gl::scale( vec3( mZoom, mZoom, mZoom ) );
		gl::color( Color( 0.8f, 0.4f, 0.0f ) );
		gl::draw( mVboMesh );
		if( mDrawWireframe ) {
			gl::enableWireframe();
			gl::color( Color::white() );
			gl::draw( mVboMesh );
			gl::disableWireframe();
		}
	gl::popModelView();
}


CINDER_APP( TriangulationApp, RendererGl )