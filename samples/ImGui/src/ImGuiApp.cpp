#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Surface.h"
#include "cinder/CinderImGui.h"
#include "cinder/Log.h"
#include "cinder/ip/Checkerboard.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ImGuiApp : public App {
  public:
	void setup() override;
	void update() override;
	void draw() override;
	void cleanup() override;
	void keyDown( KeyEvent event ) override;

  private:
	void createSampleTexture();
	void drawCinderExtensions();
	void drawImGuiDrawingDemo();

	// Demo window states
	bool mDrawGui = true; // Toggle for ImGui rendering
	bool mShowDemoWindow = false;
	bool mShowTestWindow = true;
	bool mShowExtensions = true;
	bool mShowDrawingDemo = true;

	// Basic test controls (from original)
	float		   mFloatValue = 0.5f;
	int			   mIntValue = 42;
	vec3		   mColorValue = vec3( 0.5f, 0.3f, 0.8f );
	char		   mTextBuffer[256] = "Hello, ImGui v1.92.2b!";
	bool		   mCheckboxValue = false;
	int			   mRadioValue = 0;
	int			   mComboSelection = 0;
	vector<string> mComboItems = { "Option 1", "Option 2", "Option 3", "Option 4" };
	int			   mListBoxSelection = 0;
	vector<string> mListBoxItems = { "Item A", "Item B", "Item C", "Item D", "Item E" };

	// CinderImGui unique API demonstrations
	glm::vec2  mVec2 = glm::vec2( 1.0f, 2.0f );
	glm::vec3  mVec3 = glm::vec3( 1.0f, 2.0f, 3.0f );
	glm::vec4  mVec4 = glm::vec4( 1.0f, 2.0f, 3.0f, 4.0f );
	glm::ivec2 mIVec2 = glm::ivec2( 10, 20 );
	glm::ivec3 mIVec3 = glm::ivec3( 10, 20, 30 );
	glm::ivec4 mIVec4 = glm::ivec4( 10, 20, 30, 40 );

	ci::Colorf	mColorf = ci::Colorf( 0.8f, 0.4f, 0.2f );
	ci::ColorAf mColorAf = ci::ColorAf( 1.0f, 0.5f, 0.3f, 0.8f );

	gl::Texture2dRef mTexture;
	vec2			 mTextureSize = vec2( 128, 128 );
	vec2			 mUV0 = vec2( 0.0f, 0.0f );
	vec2			 mUV1 = vec2( 1.0f, 1.0f );
	vec4			 mTint = vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	vec4			 mBorder = vec4( 1.0f, 0.0f, 0.0f, 1.0f );
};

void ImGuiApp::setup()
{
	CI_LOG_I( "Initializing ImGui Test Application" );

	// Initialize ImGui
	ImGui::Initialize( ImGui::Options().window( getWindow() ).enableKeyboard( true ) );
	ImGui::GetStyle().ScaleAllSizes( getWindowContentScale() ); // for Retina / hi-dpi
	ImGui::GetStyle().FontScaleMain = getWindowContentScale();

	// Create a simple test texture
	createSampleTexture();

	CI_LOG_I( "ImGui Version: " << IMGUI_VERSION );
}

void ImGuiApp::createSampleTexture()
{
	const int size = 128;
	Surface8u surface( size, size, false );
	ip::checkerboard( &surface, Area( 0, 0, size, size ), 16, Color8u( 200, 200, 200 ), Color8u( 50, 50, 50 ) );
	mTexture = gl::Texture2d::create( surface );
}

void ImGuiApp::update()
{
}

void ImGuiApp::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.1f ) );

	// Draw a background circle using GL when GUI is disabled to show the app is still responsive
	if( ! mDrawGui ) {
		gl::color( 0.2f, 0.2f, 0.3f );
        float radius = (float)std::max( getWindowWidth(), getWindowHeight() ) / 2.0f;
		gl::drawSolidCircle( toPixels( vec2( getWindowCenter() ) ), toPixels( radius ) );

		// Draw text to show GUI is disabled
		gl::color( 1.0f, 1.0f, 1.0f );
		return;
	}

	// Use ImGui's drawing functions instead of gl::drawSolidCircle
	// This draws to a background draw list that renders behind all windows
	ImDrawList* backgroundDrawList = ImGui::GetBackgroundDrawList();

	// Draw a large white circle behind everything
	ImVec2 center( toPixels( getWindowCenter().x ), toPixels( getWindowCenter().y ) );
	float  radius = (float)std::max( getWindowWidth(), getWindowHeight() ) / 2.0f;
	backgroundDrawList->AddCircleFilled( center, toPixels( radius ), IM_COL32( 255, 255, 255, 255 ), 64 );

	// Show ImGui demo window
	if( mShowDemoWindow ) {
		ImGui::ShowDemoWindow( &mShowDemoWindow );
	}

	// Basic test window (original functionality)
	if( mDrawGui && mShowTestWindow ) {
		ImGui::Begin( "Basic ImGui Test", &mShowTestWindow );

		ImGui::Text( "ImGui %s with Cinder", IMGUI_VERSION );
		ImGui::Separator();

		ImGui::Checkbox( "Show Demo Window", &mShowDemoWindow );
		ImGui::Checkbox( "Show CinderImGui Extensions", &mShowExtensions );
		ImGui::Checkbox( "Show ImGui Drawing Demo", &mShowDrawingDemo );
		ImGui::Separator();

		// Basic Controls
		if( ImGui::CollapsingHeader( "Standard ImGui Controls", ImGuiTreeNodeFlags_DefaultOpen ) ) {
			ImGui::SliderFloat( "Float Slider", &mFloatValue, 0.0f, 1.0f );
			ImGui::SliderInt( "Int Slider", &mIntValue, 0, 100 );
			ImGui::ColorEdit3( "Color Picker", &mColorValue.x );
			ImGui::InputText( "Text Input", mTextBuffer, sizeof( mTextBuffer ) );
			ImGui::Checkbox( "Checkbox", &mCheckboxValue );
			ImGui::Combo( "Combo Box", &mComboSelection, mComboItems );
			ImGui::ListBox( "List Box", &mListBoxSelection, mListBoxItems, 4 );
		}

		ImGui::End();
	}

	// CinderImGui Extensions Demo
	if( mDrawGui && mShowExtensions ) {
		drawCinderExtensions();
	}

	// ImGui Drawing Demo
	if( mDrawGui && mShowDrawingDemo ) {
		drawImGuiDrawingDemo();
	}
}

void ImGuiApp::drawCinderExtensions()
{
	ImGui::Begin( "CinderImGui Extensions", &mShowExtensions );

	ImGui::Text( "Unique CinderImGui API Functions" );
	ImGui::Separator();

	// GLM Vector Controls
	if( ImGui::CollapsingHeader( "GLM Vector Types", ImGuiTreeNodeFlags_DefaultOpen ) ) {
		ImGui::Text( "These functions work directly with GLM types (showing a couple of each):" );

		// Float vectors - showing DragFloat and SliderFloat variants
		ImGui::DragFloat2( "DragFloat2 (glm::vec2)", &mVec2, 0.1f, -10.0f, 10.0f );
		ImGui::DragFloat3( "DragFloat3 (glm::vec3)", &mVec3, 0.1f, -10.0f, 10.0f );
		ImGui::SliderFloat2( "SliderFloat2 (glm::vec2)", &mVec2, -5.0f, 5.0f );
		ImGui::SliderFloat3( "SliderFloat3 (glm::vec3)", &mVec3, -5.0f, 5.0f );

		ImGui::Spacing();

		// Integer vectors - showing DragInt and InputInt variants
		ImGui::DragInt2( "DragInt2 (glm::ivec2)", &mIVec2, 1.0f, 0, 100 );
		ImGui::DragInt3( "DragInt3 (glm::ivec3)", &mIVec3, 1.0f, 0, 100 );
		ImGui::InputInt2( "InputInt2 (glm::ivec2)", &mIVec2 );
		ImGui::InputInt3( "InputInt3 (glm::ivec3)", &mIVec3 );

		ImGui::Separator();
	}

	// Cinder Color Types
	if( ImGui::CollapsingHeader( "Cinder Color Types", ImGuiTreeNodeFlags_DefaultOpen ) ) {
		ImGui::Text( "These functions work directly with Cinder color types:" );

		ImGui::ColorEdit3( "ColorEdit3 (ci::Colorf)", &mColorf );
		ImGui::ColorEdit4( "ColorEdit4 (ci::ColorAf)", &mColorAf );

		ImGui::Spacing();

		ImGui::Text( "ColorPicker with triangle selector:" );
		ImGui::ColorPicker4( "ColorPicker4 (Triangle)", &mColorAf, ImGuiColorEditFlags_PickerHueWheel | ImGuiColorEditFlags_DisplayRGB );

		ImGui::Separator();
	}

	// Cinder Texture Display
	if( ImGui::CollapsingHeader( "Cinder Texture Display", ImGuiTreeNodeFlags_DefaultOpen ) ) {
		ImGui::Text( "Image() function with ci::gl::Texture2dRef:" );

		if( mTexture ) {
			ImGui::SliderFloat2( "Display Size (px)", &mTextureSize.x, 32.0f, 256.0f );
			ImGui::DragFloat2( "UV Start", &mUV0.x, 0.01f, -1.0f, 2.0f );
			ImGui::DragFloat2( "UV End", &mUV1.x, 0.01f, -1.0f, 2.0f );
			ImGui::ColorEdit4( "Tint Color", &mTint.x );
			ImGui::ColorEdit4( "Border Color", &mBorder.x );

			// The key unique function: ImGui::Image with ci::gl::Texture2dRef
			ImGui::Image( mTexture, mTextureSize, mUV0, mUV1, mTint, mBorder );

			ImGui::Text( "Texture: %dx%d", mTexture->getWidth(), mTexture->getHeight() );
		}

		ImGui::Separator();
	}

	// std::vector Integration
	if( ImGui::CollapsingHeader( "std::vector Integration" ) ) {
		ImGui::Text( "Enhanced Combo/ListBox with std::vector<std::string>:" );

		ImGui::Combo( "Combo (std::vector)", &mComboSelection, mComboItems );
		ImGui::ListBox( "ListBox (std::vector)", &mListBoxSelection, mListBoxItems, 3 );
	}

	ImGui::End();
}

void ImGuiApp::drawImGuiDrawingDemo()
{
	ImGui::Begin( "ImGui Drawing Demo", &mShowDrawingDemo );

	ImGui::Text( "ImGui DrawList API Examples" );
	ImGui::Separator();

	// Get the draw list for this window
	ImDrawList* drawList = ImGui::GetWindowDrawList();

	// Get current cursor position for drawing
	ImVec2 p = ImGui::GetCursorScreenPos();
	float  width = ImGui::GetContentRegionAvail().x;
	float  height = 300.0f;

	// Reserve space for drawing
	ImGui::Dummy( ImVec2( width, height ) );

	// Draw a border around the drawing area
	drawList->AddRect( p, ImVec2( p.x + width, p.y + height ), IM_COL32( 255, 255, 255, 128 ) );

	// Draw various shapes
	float x = p.x + 20;
	float y = p.y + 20;

	// Circle
	drawList->AddCircleFilled( ImVec2( x + 30, y + 30 ), 25.0f, IM_COL32( 255, 100, 100, 255 ) );
	drawList->AddCircle( ImVec2( x + 30, y + 30 ), 25.0f, IM_COL32( 255, 255, 255, 255 ), 32, 2.0f );

	// Rectangle
	drawList->AddRectFilled( ImVec2( x + 80, y + 10 ), ImVec2( x + 130, y + 50 ), IM_COL32( 100, 255, 100, 255 ) );
	drawList->AddRect( ImVec2( x + 80, y + 10 ), ImVec2( x + 130, y + 50 ), IM_COL32( 255, 255, 255, 255 ), 0.0f, 0, 2.0f );

	// Triangle
	drawList->AddTriangleFilled( ImVec2( x + 180, y + 50 ), ImVec2( x + 160, y + 10 ), ImVec2( x + 200, y + 10 ), IM_COL32( 100, 100, 255, 255 ) );
	drawList->AddTriangle( ImVec2( x + 180, y + 50 ), ImVec2( x + 160, y + 10 ), ImVec2( x + 200, y + 10 ), IM_COL32( 255, 255, 255, 255 ), 2.0f );

	// Line
	drawList->AddLine( ImVec2( x + 230, y + 10 ), ImVec2( x + 280, y + 50 ), IM_COL32( 255, 255, 100, 255 ), 3.0f );

	// Polyline (zigzag)
	ImVec2 points[] = { ImVec2( x + 310, y + 30 ), ImVec2( x + 330, y + 10 ), ImVec2( x + 350, y + 30 ), ImVec2( x + 370, y + 10 ), ImVec2( x + 390, y + 30 ) };
	drawList->AddPolyline( points, 5, IM_COL32( 255, 150, 50, 255 ), 0, 2.0f );

	// Bezier curve
	y += 80;
	drawList->AddBezierCubic( ImVec2( x + 10, y + 10 ), ImVec2( x + 50, y - 20 ), ImVec2( x + 100, y + 40 ), ImVec2( x + 140, y + 10 ), IM_COL32( 200, 100, 255, 255 ), 2.0f, 20 );

	// Gradient rectangle
	drawList->AddRectFilledMultiColor( ImVec2( x + 180, y - 10 ), ImVec2( x + 280, y + 30 ), IM_COL32( 255, 0, 0, 255 ), IM_COL32( 0, 255, 0, 255 ), IM_COL32( 0, 0, 255, 255 ), IM_COL32( 255, 255, 0, 255 ) );

	// Text
	y += 60;
	drawList->AddText( ImVec2( x, y ), IM_COL32( 255, 255, 255, 255 ), "ImGui DrawList Text" );

	// Rounded rectangle
	drawList->AddRectFilled( ImVec2( x + 180, y - 10 ), ImVec2( x + 280, y + 20 ), IM_COL32( 100, 200, 100, 255 ), 10.0f );

	// N-gon (hexagon)
	y += 50;
	drawList->AddNgonFilled( ImVec2( x + 50, y + 30 ), 25.0f, IM_COL32( 200, 200, 100, 255 ), 6 );
	drawList->AddNgon( ImVec2( x + 50, y + 30 ), 25.0f, IM_COL32( 255, 255, 255, 255 ), 6, 2.0f );

	// Filled convex polygon (pentagon)
	ImVec2 pentagon[] = { ImVec2( x + 150, y ), ImVec2( x + 180, y + 15 ), ImVec2( x + 170, y + 45 ), ImVec2( x + 130, y + 45 ), ImVec2( x + 120, y + 15 ) };
	drawList->AddConvexPolyFilled( pentagon, 5, IM_COL32( 150, 100, 200, 255 ) );

	ImGui::Separator();
	ImGui::Text( "Background circle is drawn using ImGui::GetBackgroundDrawList()" );
	ImGui::Text( "All shapes above are drawn using ImGui::GetWindowDrawList()" );

	ImGui::End();
}

void ImGuiApp::keyDown( KeyEvent event )
{
	// Toggle GUI rendering with Cmd+G (Mac) or Ctrl+G (Windows/Linux)
	if( event.getChar() == 'g' && event.isAccelDown() ) {
		mDrawGui = ! mDrawGui;
		CI_LOG_I( "ImGui rendering: " << ( mDrawGui ? "ENABLED" : "DISABLED" ) );
	}
}

void ImGuiApp::cleanup()
{
	CI_LOG_I( "Cleaning up ImGui Test Application" );
}

CINDER_APP( ImGuiApp, RendererGl, []( App::Settings* settings ) {
	settings->setWindowSize( 1200, 800 );
	settings->setTitle( "ImGui v1.92.2b Test - CinderImGui Extensions" );
	settings->setHighDensityDisplayEnabled( false ); // to make this 'true' on macOS set the NSHighResolutionCapable Info.plist property to YES
} )
