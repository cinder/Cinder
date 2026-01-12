#include "cinder/CanvasUi.h"
#include "cinder/CinderImGui.h"
#include "cinder/CinderMath.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CanvasUiMinimapApp : public App {
  public:
	void setup() override;
	void draw() override;

	// Input
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void mouseUp( MouseEvent event ) override;
	void keyDown( KeyEvent event ) override;
	void resize() override;

	CanvasUi mCanvasUi;

	gl::FboRef mSceneFbo;

	static constexpr int GRID_X = 16;
	static constexpr int GRID_Y = 9;
	static constexpr int SCENE_WIDTH = 1920;
	static constexpr int SCENE_HEIGHT = 1080;
	static constexpr float CUBE_SIZE = 60;
	static constexpr int MINIMAP_WIDTH = 192;
	static constexpr int MINIMAP_HEIGHT = 108;
	static constexpr int MINIMAP_MARGIN = 20;

	bool mMinimapDragging = false;

	// Minimap
	Rectf mMinimapRect;

	// Helpers
	void setupFbo();
	// Removed setupCubes - now stateless
	void setupCanvasUi();
	void renderSceneToFbo();
	void drawMainView();
	void drawMinimap();
	void drawGui();

	// Minimap mapping helpers (window px <-> minimap-local px <-> content)
	inline vec2 windowToMinimap( vec2 win ) const { return win - mMinimapRect.getUpperLeft(); }
	inline vec2 minimapToContent( vec2 mini ) const {
		const float sx = mMinimapRect.getWidth() / SCENE_WIDTH;
		const float sy = mMinimapRect.getHeight() / SCENE_HEIGHT;
		return vec2( mini.x / sx, mini.y / sy );
	}
	inline Rectf contentToMinimap( const Rectf& content ) {
		const float sx = mMinimapRect.getWidth()  / SCENE_WIDTH;
		const float sy = mMinimapRect.getHeight() / SCENE_HEIGHT;

		vec2 tl = mMinimapRect.getUpperLeft() + vec2( content.x1 * sx, content.y1 * sy );
		vec2 br = mMinimapRect.getUpperLeft() + vec2( content.x2 * sx, content.y2 * sy );

		tl = glm::max( tl, mMinimapRect.getUpperLeft() );
		br = glm::min( br, mMinimapRect.getLowerRight() );
		return Rectf{ tl, br };
	}
	inline bool inMinimap( vec2 win ) const { return mMinimapRect.contains( win ); }
};

void CanvasUiMinimapApp::setup()
{
	// Setup ImGui and FBO first - must be before setWindowSize() as resize triggers draw
	ImGui::Initialize();
	setupFbo();

	setWindowSize( 1280, 720 );
	resize(); // force minimap rect calculation

	setupCanvasUi();
}

void CanvasUiMinimapApp::setupFbo()
{
	gl::Fbo::Format fmt;
	fmt.setSamples( 16 );
	fmt.colorTexture( gl::Texture2d::Format()
		.minFilter( GL_LINEAR )
		.magFilter( GL_NEAREST )
		.wrap( GL_CLAMP_TO_EDGE ) );
	mSceneFbo = gl::Fbo::create( SCENE_WIDTH, SCENE_HEIGHT, fmt );
}

void CanvasUiMinimapApp::setupCanvasUi()
{
	// Bounded canvas matching scene
	mCanvasUi.setContentBounds( Rectf( 0, 0, (float)SCENE_WIDTH, (float)SCENE_HEIGHT ) );
	// Lower signal priority so our App handlers can swallow events first if needed
	mCanvasUi.connect( getWindow(), /*signalPriority*/ -1 );
	
	// Start with 720p height centered in the scene
	mCanvasUi.fitRect( Rectf( 320, 180, 1600, 900 ), true /*disableAnimation*/ );
}

void CanvasUiMinimapApp::renderSceneToFbo()
{
	gl::ScopedFramebuffer fb( mSceneFbo );
	gl::ScopedViewport vp( mSceneFbo->getSize() );
	gl::ScopedMatrices mx;
	gl::clear( Color( 0.1f, 0.1f, 0.15f ) );
	gl::setMatricesWindowPersp( mSceneFbo->getSize(), 40.0f, 1.0f, 2000.0f );
	gl::ScopedDepth depth( true );

	const float t = (float)getElapsedSeconds();
	
	for( int y = 0; y < GRID_Y; ++y ) {
		for( int x = 0; x < GRID_X; ++x ) {
			vec3 pos( ( x + 0.5f ) * (float)SCENE_WIDTH / (float)GRID_X, ( y + 0.5f ) * (float)SCENE_HEIGHT / (float)GRID_Y, 0.0f  );
			
			// Calculate color based on grid position
			Color col = Color( CM_HSV, x / (float)(GRID_X - 1), 0.2f + 0.8f * (y / (float)(GRID_Y - 1)), 0.9f );
			
			// Calculate rotation (deterministic based on position)
			float rotSpeed = 0.5f + 1.5f * (float)(x + y) / (float)(GRID_X + GRID_Y - 2);
			vec3 rotAxis = normalize( vec3( sin( x * 0.7f + y * 0.3f ), cos( x * 0.3f + y * 0.7f ), sin( x * 0.5f - y * 0.5f ) ));
			
			gl::ScopedModelMatrix mm;
			gl::translate( pos );
			gl::rotate( t * rotSpeed, rotAxis );
			
			// Draw cube (80% of cell size)
			gl::ScopedColor cubeCol( col );
			gl::drawCube( vec3( 0 ), vec3( CUBE_SIZE ) );
			
			// Draw wireframe
			gl::ScopedColor wireCol( Color::white() * 0.3f );
			gl::drawStrokedCube( vec3( 0 ), vec3( CUBE_SIZE ) );
		}
	}
}

void CanvasUiMinimapApp::mouseDown( MouseEvent e )
{
	if( e.isLeft() && inMinimap( e.getPos() ) ) {
		mMinimapDragging = true;
		mCanvasUi.panToContentPos( minimapToContent( windowToMinimap( e.getPos() ) ), true );
		e.setHandled();
	}
}

void CanvasUiMinimapApp::mouseDrag( MouseEvent e )
{
	if( mMinimapDragging && e.isLeftDown() && inMinimap( e.getPos() ) ) {
		mCanvasUi.panToContentPos( minimapToContent( windowToMinimap( e.getPos() ) ), true );
		e.setHandled();
	}
}

void CanvasUiMinimapApp::mouseUp( MouseEvent )
{
	mMinimapDragging = false;
}

void CanvasUiMinimapApp::keyDown( KeyEvent event )
{
	const float panStep = 30.0f; // pixels to pan per keypress
	
	switch( event.getCode() ) {
		case KeyEvent::KEY_UP:
			mCanvasUi.panBy( vec2( 0, panStep ) );
			event.setHandled();
			break;
			
		case KeyEvent::KEY_DOWN:
			mCanvasUi.panBy( vec2( 0, -panStep ) );
			event.setHandled();
			break;
			
		case KeyEvent::KEY_LEFT:
			mCanvasUi.panBy( vec2( panStep, 0 ) );
			event.setHandled();
			break;
			
		case KeyEvent::KEY_RIGHT:
			mCanvasUi.panBy( vec2( -panStep, 0 ) );
			event.setHandled();
			break;

		case KeyEvent::KEY_q:
			if( event.isControlDown() )
				quit();
			break;
	}
}

void CanvasUiMinimapApp::resize()
{
	// recalculate bounds of minimap
	const float l = (float)getWindowWidth()  - MINIMAP_WIDTH - MINIMAP_MARGIN;
	const float t = (float)getWindowHeight() - MINIMAP_HEIGHT - MINIMAP_MARGIN;
	mMinimapRect = Rectf( l, t, l + MINIMAP_WIDTH, t + MINIMAP_HEIGHT );
}

void CanvasUiMinimapApp::drawMainView()
{
	gl::ScopedMatrices m;
	gl::setMatricesWindow( getWindowSize() );			// 2D, window coords (y down)
	gl::multModelMatrix( mCanvasUi.getModelMatrix() );	// canvas content -> window
	gl::ScopedColor white( Color::white() );
	gl::draw( mSceneFbo->getColorTexture(), Rectf( 0, 0, (float)SCENE_WIDTH, (float)SCENE_HEIGHT ) );
}

void CanvasUiMinimapApp::drawMinimap()
{
	// FBO contents
	gl::ScopedColor dim( Color::white() * 0.7f );
	gl::draw( mSceneFbo->getColorTexture(), mMinimapRect );

	// visible rect outline
	gl::ScopedColor y( Color( 1, 1, 0 ) );
	gl::drawStrokedRect( contentToMinimap( mCanvasUi.getVisibleRect() ), 2.0f );

	// border
	gl::ScopedColor border( Color( 0.5f, 0.5f, 0.6f ) );
	gl::drawStrokedRect( mMinimapRect, 2.0f );
}

void CanvasUiMinimapApp::drawGui()
{
	ImGui::ScopedWindow w( "CanvasUi Minimap" );
	ImGui::Text( "Scene: %dx%d", SCENE_WIDTH, SCENE_HEIGHT );
	ImGui::Text( "Zoom: %.1f%%", (float)(mCanvasUi.getZoom() * 100) );
	
	if( ImGui::Button( "Fit All" ) ) mCanvasUi.fitAll();
	ImGui::SameLine();
	if( ImGui::Button( "Zoom In" ) ) mCanvasUi.zoomIn();
	ImGui::SameLine();
	if( ImGui::Button( "Zoom Out" ) ) mCanvasUi.zoomOut();
	
	ImGui::Separator();
	ImGui::Text( "Navigation:" );
	ImGui::Text( "• Arrow keys: Pan view" );
	ImGui::Text( "• Mouse drag: Pan view" );
	ImGui::Text( "• Mouse wheel: Zoom" );
	ImGui::Text( "• Click minimap: Jump to location" );
}

void CanvasUiMinimapApp::draw()
{
	gl::clear( Color( 0.2f, 0.2f, 0.25f ) );

	renderSceneToFbo();

	drawMainView();
	drawMinimap();
	drawGui();
}

CINDER_APP( CanvasUiMinimapApp, RendererGl( RendererGl::Options().msaa( 16 ) ) )
