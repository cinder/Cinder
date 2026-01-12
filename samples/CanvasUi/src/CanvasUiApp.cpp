/*
CanvasUi Sample

Comprehensive demonstration of CanvasUi features including:
- Basic pan and zoom interaction
- Bounded vs unbounded content modes
- Custom viewport support (restricting canvas to window regions)
- Animation controls and settings
- Zoom constraints and pan limitations
- Mouse and keyboard input customization

Content: 10 circles with varying polygon complexity, useful for observing 
zoom behavior as detail changes with scale.

Controls: See the ImGui panel for interactive settings and examples.
*/

#include "cinder/CanvasUi.h"
#include "cinder/CinderImGui.h"
#include "cinder/CinderMath.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CanvasUiSampleApp : public App {
  public:
	void setup() override;
	void update() override;
	void draw() override;
	void keyDown( KeyEvent event ) override;
	void mouseMove( MouseEvent event ) override;
	void resize() override;

  private:
	CanvasUi mCanvas;

	// Content
	vector<vec2> mCircleCenters;
	vector<int>	 mCircleSides;
	Rectf		 mContentBounds;

	// UI state
	bool				   mShowImGui{ true };

	bool				   mSplitScreenMode{ false };

	// Hover state
	int mHoveredCircle{ -1 }; // Index of currently hovered circle, -1 for none

	static constexpr int   NUM_CIRCLES = 10;
	static constexpr float CIRCLE_RADIUS = 40.0f;

	void generateCircles();
	void drawContent();
	void drawGui();
	void updateCanvasSettings( bool bounded );
	void updateViewportSettings();
};

void CanvasUiSampleApp::setup()
{
	// Setup ImGui first - must be before setWindowSize() as resize triggers draw
	ImGui::Initialize();

	setWindowSize( 1280, 800 );

	// Generate circle content
	generateCircles();

	// Setup canvas, default bounded
	mCanvas.setContentBounds( mContentBounds );

	// Connect to window
	mCanvas.connect( getWindow() );

	// Setup viewport
	updateViewportSettings();

	// Initial fit
	mCanvas.fitAll();
}

void CanvasUiSampleApp::generateCircles()
{
	mCircleCenters.clear();
	mCircleSides.clear();

	const float spacing = 120.0f;

	for( int i = 0; i < NUM_CIRCLES; ++i ) {
		float x = i * spacing;
		float y = 0;
		mCircleCenters.push_back( vec2( x, y ) );

		// Simple power function: 3 to 40 sides
		float t = i / (float)( NUM_CIRCLES - 1 );			   // 0 to 1
		int	  sides = (int)( 3.0f + 37.0f * powf( t, 1.5f ) ); // 3 to 40
		mCircleSides.push_back( sides );
	}

	// Calculate content bounds with padding
	if( ! mCircleCenters.empty() ) {
		vec2 minPos = mCircleCenters.front();
		vec2 maxPos = mCircleCenters.back();

		float padding = CIRCLE_RADIUS + 20;
		mContentBounds = Rectf( minPos.x - padding, minPos.y - padding, maxPos.x + padding, maxPos.y + padding );
	}
}

void CanvasUiSampleApp::update()
{
	// Update title with current state
	string title = "CanvasUi Sample - ";
	title += "Zoom: " + to_string( (int)( mCanvas.getZoom() * 100 ) ) + "% | ";
	title += mCanvas.isBounded() ? "Bounded" : "Unbounded";
	if( mSplitScreenMode )
		title += " | Viewport R";
	title += " | Press H for help";
	getWindow()->setTitle( title );
}

void CanvasUiSampleApp::draw()
{
	gl::clear( Color( 0.2f, 0.2f, 0.25f ) );

	// In split-screen mode, draw different content on left half
	if( mSplitScreenMode ) {
		// Draw solid color background on left half
		Rectf			leftHalf( 0, 0, getWindowWidth() * 0.5f, (float)getWindowHeight() );
		gl::ScopedColor leftColor( Color( 0.3f, 0.4f, 0.6f ) );
		gl::drawSolidRect( leftHalf );
	}

	// Apply canvas transform and draw content with scissor test for viewport
	if( mCanvas.hasCustomViewport() ) {
		Rectf viewport = mCanvas.getViewport();
		// Convert from Cinder coordinates (top-left origin) to OpenGL coordinates (bottom-left origin)
		int					  glY = getWindowHeight() - (int)viewport.y2;
		gl::ScopedScissor	  scissor( (int)viewport.x1, glY, (int)viewport.getWidth(), (int)viewport.getHeight() );
		gl::ScopedModelMatrix scpMatrix( mCanvas.getModelMatrix() );
		drawContent();
	}
	else {
		gl::ScopedModelMatrix scpMatrix( mCanvas.getModelMatrix() );
		drawContent();
	}

	// Draw ImGui panel
	if( mShowImGui )
		drawGui();
}

void CanvasUiSampleApp::drawContent()
{
	// Draw content bounds if bounded (shows the defined canvas area)
	if( mCanvas.isBounded() ) {
		gl::ScopedColor scopedBg( ColorA( 0.1f, 0.1f, 0.1f, 0.3f ) );
		gl::drawSolidRect( mContentBounds );

		gl::ScopedColor scopedBorder( Color( 0.5f, 0.5f, 0.6f ) );
		gl::drawStrokedRect( mContentBounds, 2.0f );
	}

	// Draw grid for reference (helps visualize coordinate system and zoom level)
	gl::ScopedColor scopedGrid( ColorA( 0.4f, 0.4f, 0.4f, 0.3f ) );
	const float		gridSpacing = 50.0f; // Grid lines every 50 content units

	// Only draw grid within reasonable bounds to avoid performance issues
	Rectf drawBounds = mCanvas.isBounded() ? mContentBounds : Rectf( -500, -300, 1500, 300 );

	// Vertical grid lines
	for( float x = floor( drawBounds.x1 / gridSpacing ) * gridSpacing; x <= drawBounds.x2; x += gridSpacing ) {
		gl::drawLine( vec2( x, drawBounds.y1 ), vec2( x, drawBounds.y2 ) );
	}

	// Horizontal grid lines
	for( float y = floor( drawBounds.y1 / gridSpacing ) * gridSpacing; y <= drawBounds.y2; y += gridSpacing ) {
		gl::drawLine( vec2( drawBounds.x1, y ), vec2( drawBounds.x2, y ) );
	}

	// Draw the main demo: circles with increasing subdivision
	// This demonstrates how geometric detail becomes visible/invisible at different zoom levels
	for( int i = 0; i < mCircleCenters.size(); ++i ) {
		vec2 center = mCircleCenters[i];
		int	 sides = mCircleSides[i];

		// Color based on polygon complexity (3 sides = red, 200 sides = purple)
		float hue = i / (float)mCircleCenters.size();
		Color fillColor = Color( CM_HSV, hue, 0.8f, 0.9f );

		// Each circle rotates slowly - rotation speed varies by complexity
		float rotationSpeed = 0.1f + ( i * 0.05f ); // Faster rotation for more complex shapes
		float rotation = (float)getElapsedSeconds() * rotationSpeed;

		// Apply rotation transform for this circle
		gl::ScopedModelMatrix scpMatrix;
		gl::translate( center );
		gl::rotate( rotation );

		// Draw filled circle using simple Cinder functions
		{
			gl::ScopedColor scopedColor( fillColor );
			gl::drawSolidCircle( vec2( 0 ), CIRCLE_RADIUS, sides ); // Use sides parameter for subdivision
		}

		// Draw outline - brighter version of fill color for normal circles, special for hovered
		{
			Color outlineColor;
			float strokeWidth = 2.0f;

			if( i == mHoveredCircle ) {
				// Highlighted circle gets bright white outline and thicker stroke
				outlineColor = Color::white();
				strokeWidth = 4.0f;
			}
			else {
				// Normal circles get brighter version of their fill color
				outlineColor = Color( CM_HSV, hue, 0.6f, 1.0f ); // Same hue, less saturation, full brightness
			}

			gl::ScopedColor scopedOutline( outlineColor );
			gl::drawStrokedCircle( vec2( 0 ), CIRCLE_RADIUS, strokeWidth, sides ); // Same subdivision for outline
		}
	}

	// Draw coordinate axes at origin (helps understand the coordinate system)
	gl::ScopedColor scopedAxes( Color( 0.7f, 0.7f, 0.7f ) );
	gl::drawLine( vec2( -30, 0 ), vec2( 30, 0 ) ); // X axis (horizontal)
	gl::drawLine( vec2( 0, -30 ), vec2( 0, 30 ) ); // Y axis (vertical)
}

void CanvasUiSampleApp::drawGui()
{
	ImGui::ScopedWindow window( "CanvasUi Controls" );

	// === STATUS (Always Visible) ===
	ImGui::Text( "Current Zoom: %.1f%%", mCanvas.getZoom() * 100.0f );
	Rectf visibleRect = mCanvas.getVisibleRect();
	ImGui::Text( "Visible: (%.0f,%.0f) to (%.0f,%.0f)", visibleRect.x1, visibleRect.y1, visibleRect.x2, visibleRect.y2 );
	if( mHoveredCircle >= 0 ) {
		ImGui::Text( "Hovered: Circle #%d (%d sides)", (mHoveredCircle+1), mCircleSides[mHoveredCircle] );
	}
	else {
		ImGui::Text( "Hovered: None" );
	}
	ImGui::Separator();

	// === CONTENT MODE ===
	if( ImGui::CollapsingHeader( "Content Mode" ) ) {
		bool isBounded = mCanvas.isBounded();
		if( ImGui::Checkbox( "Bounded Canvas", &isBounded ) )
			updateCanvasSettings( isBounded );
		if( ImGui::IsItemHovered() )
			ImGui::SetTooltip( "Bounded: Content has defined limits\nUnbounded: Infinite scrollable canvas" );
		
		ImGui::SameLine();
		ImGui::Text( mCanvas.isBounded() ? "(Content has limits)" : "(Infinite canvas)" );

		// Viewport demo
		bool wasSplitScreen = mSplitScreenMode;
		ImGui::Checkbox( "Viewport Right Half Only", &mSplitScreenMode );
		if( mSplitScreenMode != wasSplitScreen ) {
			updateViewportSettings();
		}
		if( ImGui::IsItemHovered() ) {
			ImGui::SetTooltip( "Demonstrates that CanvasUi can work\nwith a custom viewport area" );
		}
	}

	// === NAVIGATION ===
	if( ImGui::CollapsingHeader( "Navigation", ImGuiTreeNodeFlags_DefaultOpen ) ) {
		// Fit operations (bounded content only)
		if( mCanvas.isBounded() ) {
			if( ImGui::Button( "Fit All" ) ) mCanvas.fitAll();
			ImGui::SameLine();
			if( ImGui::Button( "Fit Width" ) ) mCanvas.fitWidth(); 
			ImGui::SameLine();
			if( ImGui::Button( "Fit Height" ) ) mCanvas.fitHeight();
		}
		else {
			ImGui::TextDisabled( "Fit operations require bounded content" );
		}
		
		// Always available operations
		if( ImGui::Button( "Zoom In" ) ) mCanvas.zoomIn();
		ImGui::SameLine();
		if( ImGui::Button( "Zoom Out" ) ) mCanvas.zoomOut();
		ImGui::SameLine();
		if( ImGui::Button( "1:1" ) ) mCanvas.viewOnetoOne();
		if( ImGui::IsItemHovered() )
			ImGui::SetTooltip( "View at 100% zoom (1:1 pixel mapping)" );
		ImGui::SameLine();
		
		// Demo operations
		if( mCircleCenters.size() > 3 ) {
			if( ImGui::Button( "Frame Circle #4" ) ) {
				vec2 center = mCircleCenters[3];
				Rectf circleRect( center - vec2( CIRCLE_RADIUS ), center + vec2( CIRCLE_RADIUS ) );
				mCanvas.fitRect( circleRect );
			}
			ImGui::SameLine();
			if( ImGui::Button( "Pan to Circle #5" ) && mCircleCenters.size() > 4 ) {
				mCanvas.panToContentPos( mCircleCenters[4] );
			}
		}
	}

	// === INPUT SETTINGS ===
	if( ImGui::CollapsingHeader( "Input Settings" ) ) {
		bool enabled = mCanvas.isEnabled();
		if( ImGui::Checkbox( "Canvas Enabled", &enabled ) )
			mCanvas.enable( enabled );

		bool keyboardEnabled = mCanvas.isKeyboardEnabled();
		if( ImGui::Checkbox( "Keyboard Shortcuts", &keyboardEnabled ) )
			mCanvas.setKeyboardEnabled( keyboardEnabled );

		bool mouseWheelEnabled = mCanvas.isMouseWheelEnabled();
		if( ImGui::Checkbox( "Mouse Wheel Zoom", &mouseWheelEnabled ) )
			mCanvas.setMouseWheelEnabled( mouseWheelEnabled );
			
		ImGui::Separator();
		ImGui::Text( "Mouse: Left/Middle drag = pan, Wheel = zoom" );
		ImGui::Text( "Keys: Cmd+0=reset, Cmd+1=1:1, Cmd+/- = zoom" );
	}

	// === BEHAVIOR SETTINGS ===
	if( ImGui::CollapsingHeader( "Behavior Settings" ) ) {
		// Animation settings
		bool animationEnabled = mCanvas.isAnimationEnabled();
		if( ImGui::Checkbox( "Enable Animations", &animationEnabled ) ) {
			mCanvas.setAnimationEnabled( animationEnabled );
			mCanvas.connect( getWindow() );
		}
		if( ImGui::IsItemHovered() ) {
			ImGui::SetTooltip( "Master animation toggle - when disabled, overrides all other animation settings" );
		}
		
		if( animationEnabled ) {
			ImGui::SameLine();
			bool animateMousePan = mCanvas.hasAnimateMousePan();
			if( ImGui::Checkbox( "Animate Mouse Pan", &animateMousePan ) ) {
				mCanvas.setAnimateMousePan( animateMousePan );
			}
			if( ImGui::IsItemHovered() ) {
				ImGui::SetTooltip( "Animate mouse drag operations (only effective when animations are enabled)" );
			}
			
			float duration = mCanvas.getAnimationDuration();
			if( ImGui::SliderFloat( "Animation Duration", &duration, 0.1f, 2.0f, "%.1fs" ) ) {
				mCanvas.setAnimationDuration( duration );
			}
			if( ImGui::IsItemHovered() ) {
				ImGui::SetTooltip( "How long animations take to complete" );
			}
			
			float easing = mCanvas.getAnimationEasing();
			if( ImGui::SliderFloat( "Animation Easing", &easing, 1.0f, 5.0f, "%.1f" ) ) {
				mCanvas.setAnimationEasing( easing );
			}
			if( ImGui::IsItemHovered() ) {
				ImGui::SetTooltip( "Animation curve steepness\n1.0 = linear, higher = more curved ease-out" );
			}
		}
		else {
			ImGui::TextDisabled( "Animation settings disabled" );
		}
		
		ImGui::Separator();
		
		// Zoom settings
		float zoomFactor = mCanvas.getZoomFactor();
		if( ImGui::SliderFloat( "Zoom Factor", &zoomFactor, 0.05f, 2.0f, "%.2f" ) ) {
			mCanvas.setZoomFactor( zoomFactor );
		}
		if( ImGui::IsItemHovered() ) {
			ImGui::SetTooltip( "Controls zoom in/out step size (default: 0.1)" );
		}
		
		float minZoom = mCanvas.getMinZoom();
		float maxZoom = mCanvas.getMaxZoom();
		if( ImGui::SliderFloat( "Min Zoom", &minZoom, 0.01f, 1.0f, "%.2f" ) ) {
			mCanvas.setZoomLimits( minZoom, maxZoom );
		}
		if( ImGui::SliderFloat( "Max Zoom", &maxZoom, 1.0f, 50.0f, "%.1f" ) ) {
			mCanvas.setZoomLimits( minZoom, maxZoom );
		}
		
		ImGui::Separator();
		
		// Mouse wheel settings
		bool zoomToCursor = mCanvas.isZoomToCursor();
		if( ImGui::Checkbox( "Zoom to Cursor", &zoomToCursor ) ) {
			mCanvas.setZoomToCursor( zoomToCursor );
		}
		
		ImGui::SameLine();
		bool inverted = mCanvas.isMouseWheelInverted();
		if( ImGui::Checkbox( "Invert Wheel", &inverted ) ) {
			mCanvas.setMouseWheelInverted( inverted );
		}
		
		float multiplier = mCanvas.getMouseWheelMultiplier();
		if( ImGui::SliderFloat( "Wheel Speed", &multiplier, 0.1f, 5.0f, "%.1f" ) ) {
			mCanvas.setMouseWheelMultiplier( multiplier );
		}
		if( ImGui::IsItemHovered() ) {
			ImGui::SetTooltip( "Multiplier on zoom factor for mouse wheel" );
		}
		
		// Bounded canvas behavior (only show if bounded)
		if( mCanvas.isBounded() ) {
			ImGui::Separator();
			ImGui::Text( "Bounded Canvas:" );
			
			bool allowPanOutside = mCanvas.getAllowPanOutside();
			if( ImGui::Checkbox( "Allow Pan Outside", &allowPanOutside ) ) {
				mCanvas.setAllowPanOutside( allowPanOutside );
			}
			if( ImGui::IsItemHovered() ) {
				ImGui::SetTooltip( "Whether user can pan content outside viewport bounds" );
			}
			
			bool constrainZoomToContent = mCanvas.getConstrainZoomToContent();
			if( ImGui::Checkbox( "Constrain Zoom to Content", &constrainZoomToContent ) ) {
				mCanvas.setConstrainZoomToContent( constrainZoomToContent );
			}
			if( ImGui::IsItemHovered() ) {
				ImGui::SetTooltip( "Prevent zooming out beyond the point where content fits entirely in viewport" );
			}
			
		}
	}

	// === HELP & EXAMPLES ===
	if( ImGui::CollapsingHeader( "Help & Examples" ) ) {
		ImGui::Text( "Application Keys:" );
		ImGui::Text( "• Ctrl+G: Toggle this GUI panel" );
		ImGui::Text( "• Ctrl+Q: Quit" );
		
		ImGui::Separator();
		
		ImGui::Text( "Input Customization Examples:" );
		if( ImGui::Button( "Right-Only Pan" ) ) {
			mCanvas.setPanMouseButtons( { { app::MouseEvent::RIGHT_DOWN, 0 } } );
		}
		ImGui::SameLine();
		if( ImGui::Button( "Left+Shift Pan" ) ) {
			mCanvas.setPanMouseButtons( { { app::MouseEvent::LEFT_DOWN, app::MouseEvent::SHIFT_DOWN } } );
		}
		ImGui::SameLine();
		if( ImGui::Button( "Reset Mouse" ) ) {
			mCanvas.setPanMouseButtons( { { app::MouseEvent::LEFT_DOWN, app::MouseEvent::ALT_DOWN }, { app::MouseEvent::MIDDLE_DOWN, 0 }, { app::MouseEvent::RIGHT_DOWN, 0 } } );
		}

		if( ImGui::Button( "F1=Zoom In" ) ) {
			mCanvas.setHotkeysZoomIn( { { app::KeyEvent::KEY_F1, 0 } } );
		}
		ImGui::SameLine();
		if( ImGui::Button( "Space=Reset" ) ) {
			mCanvas.setHotkeysReset( { { app::KeyEvent::KEY_SPACE, 0 } } );
		}
		ImGui::SameLine();
		if( ImGui::Button( "Reset Keys" ) ) {
			mCanvas.setHotkeysZoomIn( { { app::KeyEvent::KEY_PLUS, app::KeyEvent::ACCEL_DOWN }, { app::KeyEvent::KEY_EQUALS, app::KeyEvent::ACCEL_DOWN } } );
			mCanvas.setHotkeysZoomOut( { { app::KeyEvent::KEY_MINUS, app::KeyEvent::ACCEL_DOWN } } );
			mCanvas.setHotkeysReset( { { app::KeyEvent::KEY_0, app::KeyEvent::ACCEL_DOWN } } );
		}
	}
}

void CanvasUiSampleApp::keyDown( KeyEvent event )
{
	switch( event.getCode() ) {
		case KeyEvent::KEY_g:
			mShowImGui = ! mShowImGui;
			event.setHandled();
			break;
		case KeyEvent::KEY_q:
			quit();
			break;
	}
}

void CanvasUiSampleApp::updateCanvasSettings( bool bounded )
{
	if( bounded )
		mCanvas.setContentBounds( mContentBounds );
	else
		mCanvas.setUnbounded();

	updateViewportSettings();
}

void CanvasUiSampleApp::updateViewportSettings()
{
	if( mSplitScreenMode ) {
		// Set CanvasUi to operate only in right half of window
		float windowWidth = (float)getWindowWidth();
		float windowHeight = (float)getWindowHeight();
		Rectf rightHalf( windowWidth * 0.5f, 0, windowWidth, windowHeight );
		mCanvas.setCustomViewport( rightHalf );
	}
	else {
		// Use full window
		mCanvas.disableCustomViewport();
	}
}

void CanvasUiSampleApp::mouseMove( MouseEvent event )
{
	// Check if mouse is within CanvasUi viewport
	vec2 mousePos = vec2( event.getPos() );
	if( ! mCanvas.getViewport().contains( mousePos ) ) {
		mHoveredCircle = -1; // Clear hover when outside viewport
		return;
	}

	vec2 contentPos = mCanvas.toContent( event.getPos() );

	// Hit-test against all circles
	mHoveredCircle = -1;

	for( int i = 0; i < mCircleCenters.size(); ++i ) {
		vec2  center = mCircleCenters[i];
		float distance = glm::distance( contentPos, center );

		// Perfect circle hit test
		if( distance <= CIRCLE_RADIUS ) {
			mHoveredCircle = i;
			break; // Take the first hit (could prioritize by z-order if needed)
		}
	}
}

void CanvasUiSampleApp::resize()
{
	// Update viewport settings when window is resized
	updateViewportSettings();
}

CINDER_APP( CanvasUiSampleApp, RendererGl( RendererGl::Options().msaa( 16 ) ) )