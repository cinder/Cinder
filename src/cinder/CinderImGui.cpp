#include "cinder/CinderImGui.h"
#include "imgui_impl_opengl3.h"

#include "cinder/app/App.h"
#include "cinder/Log.h"
#include "cinder/app/Window.h"
#include "cinder/app/MouseEvent.h"
#include "cinder/app/KeyEvent.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Context.h"
#include "cinder/Clipboard.h"

#if defined( CINDER_MSW )
	#include "cinder/app/msw/PlatformMsw.h"
#endif

#include <unordered_map>

static bool																   sInitialized = false;
static bool																   sTriggerNewFrame = false;
static ci::signals::ConnectionList										   sAppConnections;
static std::unordered_map<ci::app::WindowRef, ci::signals::ConnectionList> sWindowConnections;

namespace ImGui {
Options::Options()
	: mWindow( ci::app::getWindow() )
	, mAutoRender( true )
	, mIniPath()
	, mSignalPriority( 1 )
	, mKeyboardEnabled( true )
	, mGamepadEnabled( true )
{
	//! Default Cinder styling
	mStyle.Alpha = 1.0f;							// Global alpha applies to everything in ImGui
	mStyle.WindowPadding = ImVec2( 8, 8 );			// Padding within a window
	mStyle.WindowRounding = 7.0f;					// Radius of window corners rounding. Set to 0.0f to have rectangular windows
	mStyle.WindowBorderSize = 1.0f;					// Thickness of border around windows. Generally set to 0.0f or 1.0f. Other values not well tested.
	mStyle.WindowMinSize = ImVec2( 32, 32 );		// Minimum window size
	mStyle.WindowTitleAlign = ImVec2( 0.0f, 0.5f ); // Alignment for title bar text
	mStyle.ChildRounding = 0.0f;					// Radius of child window corners rounding. Set to 0.0f to have rectangular child windows
	mStyle.ChildBorderSize = 1.0f;					// Thickness of border around child windows. Generally set to 0.0f or 1.0f. Other values not well tested.
	mStyle.PopupRounding = 0.0f;					// Radius of popup window corners rounding. Set to 0.0f to have rectangular child windows
	mStyle.PopupBorderSize = 1.0f;					// Thickness of border around popup or tooltip windows. Generally set to 0.0f or 1.0f. Other values not well tested.
	mStyle.FramePadding = ImVec2( 4, 3 );			// Padding within a framed rectangle (used by most widgets)
	mStyle.FrameRounding = 0.0f;					// Radius of frame corners rounding. Set to 0.0f to have rectangular frames (used by most widgets).
	mStyle.FrameBorderSize = 1.0f;					// Thickness of border around frames. Generally set to 0.0f or 1.0f. Other values not well tested.
	mStyle.ItemSpacing = ImVec2( 8, 4 );			// Horizontal and vertical spacing between widgets/lines
	mStyle.ItemInnerSpacing = ImVec2( 4, 4 );		// Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label)
	mStyle.TouchExtraPadding = ImVec2( 0, 0 ); // Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first
											   // widget. So don't grow this too much!
	mStyle.IndentSpacing = 21.0f;			   // Horizontal spacing when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
	mStyle.ColumnsMinSpacing = 6.0f;		   // Minimum horizontal spacing between two columns
	mStyle.ScrollbarSize = 16.0f;			   // Width of the vertical scrollbar, Height of the horizontal scrollbar
	mStyle.ScrollbarRounding = 9.0f;		   // Radius of grab corners rounding for scrollbar
	mStyle.GrabMinSize = 10.0f;				   // Minimum width/height of a grab box for slider/scrollbar
	mStyle.GrabRounding = 0.0f;				   // Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
	mStyle.TabRounding = 4.0f;				   // Radius of upper corners of a tab. Set to 0.0f to have rectangular tabs.
	mStyle.TabBorderSize = 1.0f;			   // Thickness of border around tabs.
	mStyle.ButtonTextAlign = ImVec2( 0.5f, 0.5f );	// Alignment of button text when button is larger than text.
	mStyle.DisplayWindowPadding = ImVec2( 20, 20 ); // Window position are clamped to be visible within the display area or monitors by at least this amount. Only applies to regular windows.
	mStyle.DisplaySafeAreaPadding = ImVec2( 3, 3 ); // If you cannot see the edge of your screen (e.g. on a TV) increase the safe area padding. Covers popups/tooltips as well regular windows.
	mStyle.MouseCursorScale = 1.0f;					// Scale software rendered mouse cursor (when io.MouseDrawCursor is enabled). May be removed later.
	mStyle.AntiAliasedLines = true;					// Enable anti-aliasing on lines/borders. Disable if you are really short on CPU/GPU.
	mStyle.AntiAliasedFill = true;					// Enable anti-aliasing on filled shapes (rounded rectangles, circles, etc.)
	mStyle.CurveTessellationTol = 1.25f; // Tessellation tolerance when using PathBezierCurveTo() without a specific number of segments. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.


	ImVec4 grayDarkBg{ 0.07f, 0.09f, 0.08f, 0.96f };
	ImVec4 grayBg{ 0.13f, 0.15f, 0.14f, 0.96f };
	ImVec4 grayLight{ 0.29f, 0.31f, 0.30f, 0.93f };
	ImVec4 grayInactive{ 0.22f, 0.25f, 0.24f, 0.95f };
	ImVec4 orangeDimmed = grayLight;
	ImVec4 orangeActive{ 0.663f, 0.235f, 0.082f, 0.96f };

	mStyle.Colors[ImGuiCol_Text] = ImVec4( 0.93f, 0.96f, 0.95f, 0.88f );
	mStyle.Colors[ImGuiCol_TextDisabled] = ImVec4( 0.93f, 0.96f, 0.95f, 0.28f );
	mStyle.Colors[ImGuiCol_WindowBg] = grayDarkBg;
	mStyle.Colors[ImGuiCol_ChildBg] = grayDarkBg;
	mStyle.Colors[ImGuiCol_PopupBg] = ImVec4( 0.16f, 0.18f, 0.17f, 0.96f );
	mStyle.Colors[ImGuiCol_Border] = ImVec4( grayLight.x, grayLight.y, grayLight.z, 0.6f );
	mStyle.Colors[ImGuiCol_BorderShadow] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
	mStyle.Colors[ImGuiCol_FrameBg] = grayBg;
	mStyle.Colors[ImGuiCol_FrameBgHovered] = grayInactive;
	mStyle.Colors[ImGuiCol_FrameBgActive] = orangeActive;
	mStyle.Colors[ImGuiCol_TitleBg] = grayInactive;
	mStyle.Colors[ImGuiCol_TitleBgActive] = orangeActive;
	mStyle.Colors[ImGuiCol_TitleBgCollapsed] = orangeActive;
	mStyle.Colors[ImGuiCol_MenuBarBg] = grayDarkBg;
	mStyle.Colors[ImGuiCol_ScrollbarBg] = grayBg;
	mStyle.Colors[ImGuiCol_ScrollbarGrab] = grayLight;
	mStyle.Colors[ImGuiCol_ScrollbarGrabHovered] = grayInactive;
	mStyle.Colors[ImGuiCol_ScrollbarGrabActive] = orangeActive;
	mStyle.Colors[ImGuiCol_CheckMark] = ImVec4( 0.93f, 0.96f, 0.95f, 0.88f );
	mStyle.Colors[ImGuiCol_SliderGrab] = grayLight;
	mStyle.Colors[ImGuiCol_SliderGrabActive] = grayBg;
	mStyle.Colors[ImGuiCol_Button] = grayBg;
	mStyle.Colors[ImGuiCol_ButtonHovered] = grayInactive;
	mStyle.Colors[ImGuiCol_ButtonActive] = orangeActive;
	mStyle.Colors[ImGuiCol_Header] = grayInactive;
	mStyle.Colors[ImGuiCol_HeaderHovered] = orangeDimmed; // orangeActive;
	mStyle.Colors[ImGuiCol_HeaderActive] = orangeActive;  // orangeBright;
	mStyle.Colors[ImGuiCol_Separator] = ImVec4( 0.14f, 0.16f, 0.19f, 1.00f );
	mStyle.Colors[ImGuiCol_SeparatorHovered] = grayInactive;
	mStyle.Colors[ImGuiCol_SeparatorActive] = orangeActive;
	mStyle.Colors[ImGuiCol_ResizeGrip] = grayBg;
	mStyle.Colors[ImGuiCol_ResizeGripHovered] = grayInactive;
	mStyle.Colors[ImGuiCol_ResizeGripActive] = orangeActive;
	mStyle.Colors[ImGuiCol_Tab] = grayInactive;
	mStyle.Colors[ImGuiCol_TabHovered] = orangeDimmed;
	mStyle.Colors[ImGuiCol_TabActive] = orangeActive; // orangeBright;
	mStyle.Colors[ImGuiCol_TabUnfocused] = orangeDimmed;
	mStyle.Colors[ImGuiCol_TabUnfocusedActive] = orangeActive;
	// mStyle.Colors[ImGuiCol_DockingPreview] = grayInactive;
	// mStyle.Colors[ImGuiCol_DockingEmptyBg] = ImVec4( 0.20f, 0.20f, 0.20f, 1.00f );
	mStyle.Colors[ImGuiCol_PlotLines] = ImVec4( 0.93f, 0.96f, 0.95f, 0.80f );
	mStyle.Colors[ImGuiCol_PlotLinesHovered] = grayInactive;
	mStyle.Colors[ImGuiCol_PlotHistogram] = ImVec4( 0.93f, 0.96f, 0.95f, 0.80f );
	mStyle.Colors[ImGuiCol_PlotHistogramHovered] = grayInactive;
	mStyle.Colors[ImGuiCol_TextSelectedBg] = grayInactive;
	mStyle.Colors[ImGuiCol_DragDropTarget] = grayInactive;
	mStyle.Colors[ImGuiCol_NavHighlight] = grayInactive;
	mStyle.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4( 1.00f, 1.00f, 1.00f, 0.70f );
	mStyle.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4( 0.80f, 0.80f, 0.80f, 0.20f );
	mStyle.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4( 0.20f, 0.22f, 0.27f, 0.73f );
}

Options& Options::window( const ci::app::WindowRef& window, int signalPriority )
{
	mWindow = window;
	mSignalPriority = signalPriority;
	return *this;
}

Options& Options::autoRender( bool autoRender )
{
	mAutoRender = autoRender;
	return *this;
}

Options& Options::iniPath( const ci::fs::path& path )
{
	mIniPath = path;
	return *this;
}

Options& Options::enableKeyboard( bool enable )
{
	mKeyboardEnabled = enable;
	return *this;
}

Options& Options::enableGamepad( bool enable )
{
	mGamepadEnabled = enable;
	return *this;
}

Options& Options::signalPriority( int signalPriority )
{
	mSignalPriority = signalPriority;
	return *this;
}

Options& Options::style( const ImGuiStyle& style )
{
	mStyle = style;
	return *this;
}

ScopedId::ScopedId( int int_id )
{
	ImGui::PushID( int_id );
}
ScopedId::ScopedId( const char* label )
{
	ImGui::PushID( label );
}
ScopedId::ScopedId( const void* ptrId )
{
	ImGui::PushID( ptrId );
}
ScopedId::~ScopedId()
{
	ImGui::PopID();
}
ScopedWindow::ScopedWindow( const char* label )
{
	ImGui::Begin( label );
}
ScopedWindow::~ScopedWindow()
{
	ImGui::End();
}

ScopedMainMenuBar::ScopedMainMenuBar()
	: mOpened{ ImGui::BeginMainMenuBar() }
{
}
ScopedMainMenuBar::~ScopedMainMenuBar()
{
	if( mOpened )
		ImGui::EndMainMenuBar();
}

ScopedMenuBar::ScopedMenuBar()
	: mOpened{ ImGui::BeginMenuBar() }
{
}
ScopedMenuBar::~ScopedMenuBar()
{
	if( mOpened )
		ImGui::EndMenuBar();
}

ScopedGroup::ScopedGroup()
{
	ImGui::BeginGroup();
}
ScopedGroup::~ScopedGroup()
{
	ImGui::EndGroup();
}
ScopedColumns::ScopedColumns( int count, const char* id, bool border )
{
	ImGui::Columns( count, id, border );
}
ScopedColumns::~ScopedColumns()
{
	ImGui::NextColumn();
}
ScopedTreeNode::ScopedTreeNode( const std::string& name )
	: mOpened( ImGui::TreeNode( name.c_str() ) )
{
}
ScopedTreeNode::~ScopedTreeNode()
{
	if( mOpened )
		ImGui::TreePop();
}

bool DragFloat2( const char* label, glm::vec2* v2, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags )
{
	return DragFloat2( label, glm::value_ptr( *v2 ), v_speed, v_min, v_max, format, flags );
}

bool DragFloat3( const char* label, glm::vec3* v3, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags )
{
	return DragFloat3( label, glm::value_ptr( *v3 ), v_speed, v_min, v_max, format, flags );
}

bool DragFloat4( const char* label, glm::vec4* v4, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags )
{
	return DragFloat4( label, glm::value_ptr( *v4 ), v_speed, v_min, v_max, format, flags );
}

bool DragInt2( const char* label, glm::ivec2* v2, float v_speed, int v_min, int v_max, const char* format )
{
	return DragInt2( label, glm::value_ptr( *v2 ), v_speed, v_min, v_max, format );
}

bool DragInt3( const char* label, glm::ivec3* v3, float v_speed, int v_min, int v_max, const char* format )
{
	return DragInt3( label, glm::value_ptr( *v3 ), v_speed, v_min, v_max, format );
}

bool DragInt4( const char* label, glm::ivec4* v4, float v_speed, int v_min, int v_max, const char* format )
{
	return DragInt4( label, glm::value_ptr( *v4 ), v_speed, v_min, v_max, format );
}

bool SliderFloat2( const char* label, glm::vec2* v2, float v_min, float v_max, const char* format, ImGuiSliderFlags flags )
{
	return SliderFloat2( label, glm::value_ptr( *v2 ), v_min, v_max, format, flags );
}

bool SliderFloat3( const char* label, glm::vec3* v3, float v_min, float v_max, const char* format, ImGuiSliderFlags flags )
{
	return SliderFloat3( label, glm::value_ptr( *v3 ), v_min, v_max, format, flags );
}

bool SliderFloat4( const char* label, glm::vec4* v4, float v_min, float v_max, const char* format, ImGuiSliderFlags flags )
{
	return SliderFloat4( label, glm::value_ptr( *v4 ), v_min, v_max, format, flags );
}

bool SliderInt2( const char* label, glm::ivec2* v2, int v_min, int v_max, const char* format )
{
	return SliderInt2( label, glm::value_ptr( *v2 ), v_min, v_max, format );
}

bool SliderInt3( const char* label, glm::ivec3* v3, int v_min, int v_max, const char* format )
{
	return SliderInt3( label, glm::value_ptr( *v3 ), v_min, v_max, format );
}

bool SliderInt4( const char* label, glm::ivec4* v4, int v_min, int v_max, const char* format )
{
	return SliderInt4( label, glm::value_ptr( *v4 ), v_min, v_max, format );
}

bool InputInt2( const char* label, glm::ivec2* v2, ImGuiInputTextFlags flags )
{
	return InputInt2( label, glm::value_ptr( *v2 ), flags );
}

bool InputInt3( const char* label, glm::ivec3* v3, ImGuiInputTextFlags flags )
{
	return InputInt3( label, glm::value_ptr( *v3 ), flags );
}

bool InputInt4( const char* label, glm::ivec4* v4, ImGuiInputTextFlags flags )
{
	return InputInt4( label, glm::value_ptr( *v4 ), flags );
}

bool ColorEdit3( const char* label, ci::Colorf* color, ImGuiColorEditFlags flags )
{
	return ColorEdit3( label, color->ptr(), flags );
}

bool ColorEdit4( const char* label, ci::ColorAf* color, ImGuiColorEditFlags flags )
{
	return ColorEdit4( label, color->ptr(), flags );
}

bool ColorPicker3( const char* label, ci::Colorf* color, ImGuiColorEditFlags flags )
{
	return ColorPicker3( label, color->ptr(), flags );
}

bool ColorPicker4( const char* label, ci::ColorAf* color, ImGuiColorEditFlags flags )
{
	return ColorPicker4( label, color->ptr(), flags );
}

bool Combo( const char* label, int* currIndex, const std::vector<std::string>& values, ImGuiComboFlags flags )
{
	if( values.empty() )
		return false;

	bool		changed = false;
	int			itemsCount = (int)values.size();
	const char* previewItem = NULL;
	if( *currIndex >= 0 && *currIndex < itemsCount ) {
		previewItem = values.at( *currIndex ).c_str();
	}

	if( ImGui::BeginCombo( label, previewItem, flags ) ) {
		for( int i = 0; i < itemsCount; ++i ) {
			ImGui::PushID( (void*)(intptr_t)i );
			bool selected = ( *currIndex == i );
			if( ImGui::Selectable( values.at( i ).c_str(), selected ) ) {
				*currIndex = i;
				changed = true;
			}
			if( selected )
				ImGui::SetItemDefaultFocus();
			ImGui::PopID();
		}
		ImGui::EndCombo();
	}
	return changed;
}

bool ListBox( const char* label, int* currIndex, const std::vector<std::string>& values, int height_in_items )
{
	if( values.empty() )
		return false;

	bool changed = false;
	if( ImGui::BeginListBox( label, ImVec2( 0, height_in_items * ImGui::GetTextLineHeightWithSpacing() ) ) ) {
		for( int i = 0; i < (int)values.size(); ++i ) {
			ImGui::PushID( (void*)(intptr_t)i );
			bool selected = ( *currIndex == i );
			if( ImGui::Selectable( values.at( i ).c_str(), selected ) ) {
				*currIndex = i;
				changed = true;
			}
			if( selected )
				ImGui::SetItemDefaultFocus();
			ImGui::PopID();
		}
		ImGui::EndListBox();
	}
	return changed;
}

void Image( const ci::gl::Texture2dRef& texture, const ci::vec2& size, const ci::vec2& uv0, const ci::vec2& uv1, const ci::vec4& tint_col, const ci::vec4& border_col )
{
	Image( (ImTextureID)(intptr_t)texture->getId(), size, uv0, uv1, tint_col, border_col );
}
} // namespace ImGui


static void ImGui_ImplCinder_MouseDown( ci::app::MouseEvent& event )
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = event.getWindow()->toPixels( event.getPos() );
	if( event.isLeft() )
		io.AddMouseButtonEvent( 0, true );
	if( event.isRight() )
		io.AddMouseButtonEvent( 1, true );
	if( event.isMiddle() )
		io.AddMouseButtonEvent( 2, true );
	io.AddKeyEvent( ImGuiMod_Ctrl, event.isControlDown() );
	io.AddKeyEvent( ImGuiMod_Shift, event.isShiftDown() );
	io.AddKeyEvent( ImGuiMod_Alt, event.isAltDown() );
	io.AddKeyEvent( ImGuiMod_Super, event.isMetaDown() );
	event.setHandled( io.WantCaptureMouse );
}
static void ImGui_ImplCinder_MouseUp( ci::app::MouseEvent& event )
{
	ImGuiIO& io = ImGui::GetIO();
	if( event.isLeft() )
		io.AddMouseButtonEvent( 0, false );
	if( event.isRight() )
		io.AddMouseButtonEvent( 1, false );
	if( event.isMiddle() )
		io.AddMouseButtonEvent( 2, false );
	io.AddKeyEvent( ImGuiMod_Ctrl, event.isControlDown() );
	io.AddKeyEvent( ImGuiMod_Shift, event.isShiftDown() );
	io.AddKeyEvent( ImGuiMod_Alt, event.isAltDown() );
	io.AddKeyEvent( ImGuiMod_Super, event.isMetaDown() );
	event.setHandled( io.WantCaptureMouse );
}
static void ImGui_ImplCinder_MouseWheel( ci::app::MouseEvent& event )
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddMouseWheelEvent( 0.0f, event.getWheelIncrement() );
	io.AddKeyEvent( ImGuiMod_Ctrl, event.isControlDown() );
	io.AddKeyEvent( ImGuiMod_Shift, event.isShiftDown() );
	io.AddKeyEvent( ImGuiMod_Alt, event.isAltDown() );
	io.AddKeyEvent( ImGuiMod_Super, event.isMetaDown() );
	event.setHandled( io.WantCaptureMouse );
}

static void ImGui_ImplCinder_MouseMove( ci::app::MouseEvent& event )
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddMousePosEvent( event.getWindow()->toPixels( event.getPos() ).x, event.getWindow()->toPixels( event.getPos() ).y );
	io.AddKeyEvent( ImGuiMod_Ctrl, event.isControlDown() );
	io.AddKeyEvent( ImGuiMod_Shift, event.isShiftDown() );
	io.AddKeyEvent( ImGuiMod_Alt, event.isAltDown() );
	io.AddKeyEvent( ImGuiMod_Super, event.isMetaDown() );
	event.setHandled( io.WantCaptureMouse );
}
//! sets the right mouseDrag IO values in imgui
static void ImGui_ImplCinder_MouseDrag( ci::app::MouseEvent& event )
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddMousePosEvent( event.getWindow()->toPixels( event.getPos() ).x, event.getWindow()->toPixels( event.getPos() ).y );
	io.AddKeyEvent( ImGuiMod_Ctrl, event.isControlDown() );
	io.AddKeyEvent( ImGuiMod_Shift, event.isShiftDown() );
	io.AddKeyEvent( ImGuiMod_Alt, event.isAltDown() );
	io.AddKeyEvent( ImGuiMod_Super, event.isMetaDown() );
	event.setHandled( io.WantCaptureMouse );
}

static ImGuiKey CinderKeyToImGuiKey( int cinderKey )
{
	// Handle letter keys with range check
	if( cinderKey >= ci::app::KeyEvent::KEY_a && cinderKey <= ci::app::KeyEvent::KEY_z ) {
		return (ImGuiKey)( ImGuiKey_A + ( cinderKey - ci::app::KeyEvent::KEY_a ) );
	}
	// Handle number keys with range check
	else if( cinderKey >= ci::app::KeyEvent::KEY_0 && cinderKey <= ci::app::KeyEvent::KEY_9 ) {
		return (ImGuiKey)( ImGuiKey_0 + ( cinderKey - ci::app::KeyEvent::KEY_0 ) );
	}
	// Handle function keys with range check
	else if( cinderKey >= ci::app::KeyEvent::KEY_F1 && cinderKey <= ci::app::KeyEvent::KEY_F15 ) {
		return (ImGuiKey)( ImGuiKey_F1 + ( cinderKey - ci::app::KeyEvent::KEY_F1 ) );
	}
	// Handle numpad digits with range check
	else if( cinderKey >= ci::app::KeyEvent::KEY_KP0 && cinderKey <= ci::app::KeyEvent::KEY_KP9 ) {
		return (ImGuiKey)( ImGuiKey_Keypad0 + ( cinderKey - ci::app::KeyEvent::KEY_KP0 ) );
	}

	// Handle remaining keys with switch
	switch( cinderKey ) {
		case ci::app::KeyEvent::KEY_TAB:
			return ImGuiKey_Tab;
		case ci::app::KeyEvent::KEY_LEFT:
			return ImGuiKey_LeftArrow;
		case ci::app::KeyEvent::KEY_RIGHT:
			return ImGuiKey_RightArrow;
		case ci::app::KeyEvent::KEY_UP:
			return ImGuiKey_UpArrow;
		case ci::app::KeyEvent::KEY_DOWN:
			return ImGuiKey_DownArrow;
		case ci::app::KeyEvent::KEY_PAGEUP:
			return ImGuiKey_PageUp;
		case ci::app::KeyEvent::KEY_PAGEDOWN:
			return ImGuiKey_PageDown;
		case ci::app::KeyEvent::KEY_HOME:
			return ImGuiKey_Home;
		case ci::app::KeyEvent::KEY_END:
			return ImGuiKey_End;
		case ci::app::KeyEvent::KEY_INSERT:
			return ImGuiKey_Insert;
		case ci::app::KeyEvent::KEY_DELETE:
			return ImGuiKey_Delete;
		case ci::app::KeyEvent::KEY_BACKSPACE:
			return ImGuiKey_Backspace;
		case ci::app::KeyEvent::KEY_SPACE:
			return ImGuiKey_Space;
		case ci::app::KeyEvent::KEY_RETURN:
			return ImGuiKey_Enter;
		case ci::app::KeyEvent::KEY_ESCAPE:
			return ImGuiKey_Escape;
		case ci::app::KeyEvent::KEY_QUOTE:
			return ImGuiKey_Apostrophe;
		case ci::app::KeyEvent::KEY_COMMA:
			return ImGuiKey_Comma;
		case ci::app::KeyEvent::KEY_MINUS:
			return ImGuiKey_Minus;
		case ci::app::KeyEvent::KEY_PERIOD:
			return ImGuiKey_Period;
		case ci::app::KeyEvent::KEY_SLASH:
			return ImGuiKey_Slash;
		case ci::app::KeyEvent::KEY_SEMICOLON:
			return ImGuiKey_Semicolon;
		case ci::app::KeyEvent::KEY_EQUALS:
			return ImGuiKey_Equal;
		case ci::app::KeyEvent::KEY_LEFTBRACKET:
			return ImGuiKey_LeftBracket;
		case ci::app::KeyEvent::KEY_BACKSLASH:
			return ImGuiKey_Backslash;
		case ci::app::KeyEvent::KEY_RIGHTBRACKET:
			return ImGuiKey_RightBracket;
		case ci::app::KeyEvent::KEY_BACKQUOTE:
			return ImGuiKey_GraveAccent;
		case ci::app::KeyEvent::KEY_KP_PERIOD:
			return ImGuiKey_KeypadDecimal;
		case ci::app::KeyEvent::KEY_KP_DIVIDE:
			return ImGuiKey_KeypadDivide;
		case ci::app::KeyEvent::KEY_KP_MULTIPLY:
			return ImGuiKey_KeypadMultiply;
		case ci::app::KeyEvent::KEY_KP_MINUS:
			return ImGuiKey_KeypadSubtract;
		case ci::app::KeyEvent::KEY_KP_PLUS:
			return ImGuiKey_KeypadAdd;
		case ci::app::KeyEvent::KEY_KP_ENTER:
			return ImGuiKey_KeypadEnter;
		case ci::app::KeyEvent::KEY_KP_EQUALS:
			return ImGuiKey_KeypadEqual;
		case ci::app::KeyEvent::KEY_LSHIFT:
			return ImGuiKey_LeftShift;
		case ci::app::KeyEvent::KEY_LCTRL:
			return ImGuiKey_LeftCtrl;
		case ci::app::KeyEvent::KEY_LALT:
			return ImGuiKey_LeftAlt;
		case ci::app::KeyEvent::KEY_LSUPER:
			return ImGuiKey_LeftSuper;
		case ci::app::KeyEvent::KEY_RSHIFT:
			return ImGuiKey_RightShift;
		case ci::app::KeyEvent::KEY_RCTRL:
			return ImGuiKey_RightCtrl;
		case ci::app::KeyEvent::KEY_RALT:
			return ImGuiKey_RightAlt;
		case ci::app::KeyEvent::KEY_RSUPER:
			return ImGuiKey_RightSuper;
		case ci::app::KeyEvent::KEY_LMETA:
			return ImGuiKey_LeftSuper;
		case ci::app::KeyEvent::KEY_RMETA:
			return ImGuiKey_RightSuper;
		case ci::app::KeyEvent::KEY_MENU:
			return ImGuiKey_Menu;
		case ci::app::KeyEvent::KEY_CAPSLOCK:
			return ImGuiKey_CapsLock;
		case ci::app::KeyEvent::KEY_SCROLLOCK:
			return ImGuiKey_ScrollLock; // Note: Cinder has typo SCROLLOCK
		case ci::app::KeyEvent::KEY_NUMLOCK:
			return ImGuiKey_NumLock;
		case ci::app::KeyEvent::KEY_PRINT:
			return ImGuiKey_PrintScreen;
		case ci::app::KeyEvent::KEY_PAUSE:
			return ImGuiKey_Pause;
		default:
			return ImGuiKey_None;
	}
}

static void ImGui_ImplCinder_KeyDown( ci::app::KeyEvent& event )
{
	ImGuiIO& io = ImGui::GetIO();

#if defined CINDER_LINUX
	auto character = event.getChar();
#else
	uint32_t character = event.getCharUtf32();
#endif

	ImGuiKey key = CinderKeyToImGuiKey( event.getCode() );
	if( key != ImGuiKey_None )
		io.AddKeyEvent( key, true );

	if( ! event.isAccelDown() && character > 0 && character <= 255 ) {
		io.AddInputCharacter( (char)character );
	}

	io.AddKeyEvent( ImGuiMod_Ctrl, event.isControlDown() );
	io.AddKeyEvent( ImGuiMod_Shift, event.isShiftDown() );
	io.AddKeyEvent( ImGuiMod_Alt, event.isAltDown() );
	io.AddKeyEvent( ImGuiMod_Super, event.isMetaDown() );

	event.setHandled( io.WantCaptureKeyboard );
}

static void ImGui_ImplCinder_KeyUp( ci::app::KeyEvent& event )
{
	ImGuiIO& io = ImGui::GetIO();

	ImGuiKey key = CinderKeyToImGuiKey( event.getCode() );
	if( key != ImGuiKey_None )
		io.AddKeyEvent( key, false );

	io.AddKeyEvent( ImGuiMod_Ctrl, event.isControlDown() );
	io.AddKeyEvent( ImGuiMod_Shift, event.isShiftDown() );
	io.AddKeyEvent( ImGuiMod_Alt, event.isAltDown() );
	io.AddKeyEvent( ImGuiMod_Super, event.isMetaDown() );

	event.setHandled( io.WantCaptureKeyboard );
}

static void ImGui_ImplCinder_NewFrameGuard( const ci::app::WindowRef& window );

static void ImGui_ImplCinder_Resize( const ci::app::WindowRef& window )
{
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ci::vec2( window->toPixels( window->getSize() ) );

	ImGui_ImplCinder_NewFrameGuard( window );
}

static void ImGui_ImplCinder_NewFrameGuard( const ci::app::WindowRef& window )
{
	if( ! sTriggerNewFrame )
		return;

	ImGui_ImplOpenGL3_NewFrame();

	ImGuiIO& io = ImGui::GetIO();

	// Setup display size
	io.DisplaySize = window->toPixels( window->getSize() );

	// Setup time step
	static double g_Time = 0.0f;
	double		  current_time = ci::app::getElapsedSeconds();
	io.DeltaTime = g_Time > 0.0 ? (float)( current_time - g_Time ) : (float)( 1.0f / 60.0f );
	g_Time = current_time;

	ImGui::NewFrame();

	sTriggerNewFrame = false;
}

static void ImGui_ImplCinder_PostDraw()
{
#if defined( CINDER_MSW )
	// Skip ImGui rendering when inside modal dialog loop, but reset trigger flag
	if( static_cast<ci::app::PlatformMsw*>( ci::app::Platform::get() )->isInsideModalLoop() ) {
		sTriggerNewFrame = true;
		return;
	}
#endif

	ImGui::Render();
	ImDrawData* drawData = ImGui::GetDrawData();

	// Only call OpenGL render if there's actually something to draw
	if( drawData && drawData->CmdListsCount > 0 ) {
		ImGui_ImplOpenGL3_RenderDrawData( drawData );
	}

	sTriggerNewFrame = true;
}

static bool ImGui_ImplCinder_Init( const ci::app::WindowRef& window, const ImGui::Options& options )
{
	// Setup back-end capabilities flags
	ImGuiIO& io = ImGui::GetIO();
	io.BackendPlatformName = "imgui_impl_cinder";

	ImGuiStyle& imGuiStyle = ImGui::GetStyle();
	imGuiStyle = options.getStyle();

#ifndef CINDER_LINUX
	// clipboard callbacks
	io.SetClipboardTextFn = []( void* user_data, const char* text ) { ci::Clipboard::setString( std::string( text ) ); };
	io.GetClipboardTextFn = []( void* user_data ) {
		std::string				 str = ci::Clipboard::getString();
		static std::vector<char> strCopy;
		strCopy = std::vector<char>( str.begin(), str.end() );
		strCopy.push_back( '\0' );
		return (const char*)&strCopy[0];
	};
#endif
	int signalPriority = options.getSignalPriority();
	sWindowConnections[window] += window->getSignalMouseDown().connect( signalPriority, ImGui_ImplCinder_MouseDown );
	sWindowConnections[window] += window->getSignalMouseUp().connect( signalPriority, ImGui_ImplCinder_MouseUp );
	sWindowConnections[window] += window->getSignalMouseMove().connect( signalPriority, ImGui_ImplCinder_MouseMove );
	sWindowConnections[window] += window->getSignalMouseDrag().connect( signalPriority, ImGui_ImplCinder_MouseDrag );
	sWindowConnections[window] += window->getSignalMouseWheel().connect( signalPriority, ImGui_ImplCinder_MouseWheel );
	sWindowConnections[window] += window->getSignalKeyDown().connect( signalPriority, ImGui_ImplCinder_KeyDown );
	sWindowConnections[window] += window->getSignalKeyUp().connect( signalPriority, ImGui_ImplCinder_KeyUp );
	sWindowConnections[window] += window->getSignalResize().connect( signalPriority, std::bind( ImGui_ImplCinder_Resize, window ) );
	if( options.isAutoRenderEnabled() ) {
		sWindowConnections[window] += ci::app::App::get()->getSignalUpdate().connect( std::bind( ImGui_ImplCinder_NewFrameGuard, window ) );
		sWindowConnections[window] += window->getSignalDraw().connect( std::bind( ImGui_ImplCinder_NewFrameGuard, window ) );
		sWindowConnections[window] += window->getSignalPostDraw().connect( ImGui_ImplCinder_PostDraw );
	}

	sWindowConnections[window] += window->getSignalClose().connect( [=] {
		sWindowConnections.erase( window );
		sTriggerNewFrame = false;
	} );

	return true;
}

static void ImGui_ImplCinder_Shutdown()
{
	sWindowConnections.clear();
}

bool ImGui::Initialize( const ImGui::Options& options )
{
	if( sInitialized )
		return false;

	IMGUI_CHECKVERSION();
	auto context = ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	if( options.isKeyboardEnabled() ) {
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	 // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavNoCaptureKeyboard; // Don't capture keyboard when navigation is active
	}
	if( options.isGamepadEnabled() )
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
	ci::app::WindowRef window = options.getWindow();
	io.DisplaySize = ci::vec2( window->toPixels( window->getSize() ) );
	io.DeltaTime = 1.0f / 60.0f;
	io.WantCaptureMouse = true;

	static std::string path;
	if( options.getIniPath().empty() ) {
		path = ( ci::app::getAssetPath( "" ) / "imgui.ini" ).string();
	}
	else {
		path = options.getIniPath().string().c_str();
	}
	io.IniFilename = path.c_str();

#if ! defined( CINDER_GL_ES )
	ImGui_ImplOpenGL3_Init( "#version 150" );
#else
	ImGui_ImplOpenGL3_Init();
#endif

	ImGui_ImplCinder_Init( window, options );
	if( options.isAutoRenderEnabled() ) {
		ImGui_ImplCinder_NewFrameGuard( window );
		sTriggerNewFrame = true;
	}

	sAppConnections += ci::app::App::get()->getSignalCleanup().connect( [context]() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplCinder_Shutdown();
		ImGui::DestroyContext( context );
	} );

	sInitialized = true;
	return sInitialized;
}
