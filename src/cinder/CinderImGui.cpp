#include "cinder/CinderImGui.h"
#include "imgui/imgui_impl_opengl3.h"
//#include "imgui_internal.h"

#include "cinder/app/App.h"
#include "cinder/Log.h"
#include "cinder/app/Window.h"
#include "cinder/app/MouseEvent.h"
#include "cinder/app/KeyEvent.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Context.h"
#include "cinder/Clipboard.h"

static bool sInitialized = false;
static bool sTriggerNewFrame = true;
static std::vector<int> sAccelKeys;
static ci::signals::ConnectionList sAppConnections;
static std::unordered_map<ci::app::WindowRef, ci::signals::ConnectionList> sWindowConnections;

//! Used by Combo and ListBox below.
static auto vector_getter = []( void* vec, int idx, const char** out_text )
{
	auto& vector = *static_cast<std::vector<std::string>*>( vec );
	if( idx < 0 || idx >= static_cast<int>( vector.size() ) ) return false;
	*out_text = vector.at( idx ).c_str();
	return true;
};

namespace ImGui {
	Options::Options()
		: mWindow( ci::app::getWindow() ), mAutoRender( true ), mIniPath()
	{
	}

	Options& Options::window( const ci::app::WindowRef& window )
	{
		mWindow = window;
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

	ScopedId::ScopedId( int int_id )
	{
		ImGui::PushID( int_id );
	}
	ScopedId::ScopedId( const char* label )
	{
		ImGui::PushID( label );
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

	ScopedMainMenuBar::ScopedMainMenuBar() : mOpened{ ImGui::BeginMainMenuBar() } { }
	ScopedMainMenuBar::~ScopedMainMenuBar()
	{
		if( mOpened ) ImGui::EndMainMenuBar();
	}
	
	ScopedMenuBar::ScopedMenuBar() : mOpened{ ImGui::BeginMenuBar() } { }
	ScopedMenuBar::~ScopedMenuBar()
	{
		if( mOpened ) ImGui::EndMenuBar();
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
		if( mOpened ) ImGui::TreePop();
	}

	bool DragFloat2( const char* label, glm::vec2* v2, float v_speed, float v_min, float v_max, const char* format, float power ) {
		return DragFloat2( label, glm::value_ptr( *v2 ), v_speed, v_min, v_max, format, power );
	}

	bool DragFloat3( const char* label, glm::vec3* v3, float v_speed, float v_min, float v_max, const char* format, float power ) {
		return DragFloat3( label, glm::value_ptr( *v3 ), v_speed, v_min, v_max, format, power );
	}

	bool DragFloat4( const char* label, glm::vec4* v4, float v_speed, float v_min, float v_max, const char* format, float power ) {
		return DragFloat4( label, glm::value_ptr( *v4 ), v_speed, v_min, v_max, format, power );
	}

	bool DragInt2( const char* label, glm::ivec2* v2, float v_speed, int v_min, int v_max, const char* format ) {
		return DragInt2( label, glm::value_ptr( *v2 ), v_speed, v_min, v_max, format );
	}

	bool DragInt3( const char* label, glm::ivec3* v3, float v_speed, int v_min, int v_max, const char* format ) {
		return DragInt3( label, glm::value_ptr( *v3 ), v_speed, v_min, v_max, format );
	}

	bool DragInt4( const char* label, glm::ivec4* v4, float v_speed, int v_min, int v_max, const char* format ) {
		return DragInt4( label, glm::value_ptr( *v4 ), v_speed, v_min, v_max, format );
	}

	bool SliderFloat2( const char* label, glm::vec2* v2, float v_min, float v_max, const char* format, float power ) {
		return SliderFloat2( label, glm::value_ptr( *v2 ), v_min, v_max, format, power );
	}

	bool SliderFloat3( const char* label, glm::vec3* v3, float v_min, float v_max, const char* format, float power ) {
		return SliderFloat3( label, glm::value_ptr( *v3 ), v_min, v_max, format, power );
	}

	bool SliderFloat4( const char* label, glm::vec4* v4, float v_min, float v_max, const char* format, float power ) {
		return SliderFloat4( label, glm::value_ptr( *v4 ), v_min, v_max, format, power );
	}

	bool SliderInt2( const char* label, glm::ivec2* v2, int v_min, int v_max, const char* format ) {
		return SliderInt2( label, glm::value_ptr( *v2 ), v_min, v_max, format );
	}

	bool SliderInt3( const char* label, glm::ivec3* v3, int v_min, int v_max, const char* format ) {
		return SliderInt3( label, glm::value_ptr( *v3 ), v_min, v_max, format );
	}

	bool SliderInt4( const char* label, glm::ivec4* v4, int v_min, int v_max, const char* format ) {
		return SliderInt4( label, glm::value_ptr( *v4 ), v_min, v_max, format );
	}

	bool InputInt2( const char* label, glm::ivec2* v2, ImGuiInputTextFlags flags ) {
		return InputInt2( label, glm::value_ptr( *v2 ), flags );
	}

	bool InputInt3( const char* label, glm::ivec3* v3, ImGuiInputTextFlags flags ) {
		return InputInt3( label, glm::value_ptr( *v3 ), flags );
	}

	bool InputInt4( const char* label, glm::ivec4* v4, ImGuiInputTextFlags flags ) {
		return InputInt4( label, glm::value_ptr( *v4 ), flags );
	}

	bool ColorEdit3( const char* label, ci::Colorf* color, ImGuiColorEditFlags flags ) {
		return ColorEdit3( label, color->ptr(), flags );
	}

	bool ColorEdit4( const char* label, ci::ColorAf* color, ImGuiColorEditFlags flags ) {
		return ColorEdit4( label, color->ptr(), flags );
	}

	bool ColorPicker3( const char* label, ci::Colorf* color, ImGuiColorEditFlags flags ) {
		return ColorPicker3( label, color->ptr(), flags );
	}

	bool ColorPicker4( const char* label, ci::ColorAf* color, ImGuiColorEditFlags flags ) {
		return ColorPicker4( label, color->ptr(), flags );
	}

	bool Combo( const char* label, int* currIndex, std::vector<std::string>& values )
	{
		if( values.empty() ) return false;
		return Combo( label, currIndex, vector_getter, static_cast<void*>( &values ), static_cast<int>( values.size() ) );
	}

	bool ListBox( const char* label, int* currIndex, std::vector<std::string>& values )
	{
		if( values.empty() ) return false;
		return ListBox( label, currIndex, vector_getter, static_cast<void*>( &values ), static_cast<int>( values.size() ) );
	}

	void Image( const ci::gl::Texture2dRef& texture, const ci::vec2& size, const ci::vec2& uv0, const ci::vec2& uv1, const ci::vec4& tint_col, const ci::vec4& border_col )
	{
		Image( (void*)(intptr_t)texture->getId(), size, uv0, uv1, tint_col, border_col );
	}
}


static void ImGui_ImplCinder_MouseDown( ci::app::MouseEvent& event )
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ci::app::toPixels( event.getPos() );
	io.MouseDown[0] = event.isLeftDown();
	io.MouseDown[1] = event.isRightDown();
	io.MouseDown[2] = event.isMiddleDown();
	event.setHandled( io.WantCaptureMouse );
}
static void ImGui_ImplCinder_MouseUp( ci::app::MouseEvent& event )
{
	ImGuiIO& io = ImGui::GetIO();
	io.MouseDown[0] = event.isLeftDown();
	io.MouseDown[1] = event.isRightDown();
	io.MouseDown[2] = event.isMiddleDown();
}
static void ImGui_ImplCinder_MouseWheel( ci::app::MouseEvent& event )
{
	ImGuiIO& io = ImGui::GetIO();
	io.MouseWheel += event.getWheelIncrement();
	event.setHandled( io.WantCaptureMouse );
}

static void ImGui_ImplCinder_MouseMove( ci::app::MouseEvent& event )
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ci::app::toPixels( event.getPos() );
	event.setHandled( io.WantCaptureMouse );
}
//! sets the right mouseDrag IO values in imgui
static void ImGui_ImplCinder_MouseDrag( ci::app::MouseEvent& event )
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ci::app::toPixels( event.getPos() );
	event.setHandled( io.WantCaptureMouse );
}

static void ImGui_ImplCinder_KeyDown( ci::app::KeyEvent& event )
{
	ImGuiIO& io = ImGui::GetIO();

#if defined CINDER_LINUX
	auto character = event.getChar();
#else
	uint32_t character = event.getCharUtf32();
#endif

	io.KeysDown[event.getCode()] = true;

	if( !event.isAccelDown() && character > 0 && character <= 255 ) {
		io.AddInputCharacter( (char)character );
	}
	else if( event.getCode() != ci::app::KeyEvent::KEY_LMETA
		&& event.getCode() != ci::app::KeyEvent::KEY_RMETA
		&& event.isAccelDown()
		&& find( sAccelKeys.begin(), sAccelKeys.end(), event.getCode() ) == sAccelKeys.end() ) {
		sAccelKeys.push_back( event.getCode() );
	}

	io.KeyCtrl = io.KeysDown[ci::app::KeyEvent::KEY_LCTRL] || io.KeysDown[ci::app::KeyEvent::KEY_RCTRL];
	io.KeyShift = io.KeysDown[ci::app::KeyEvent::KEY_LSHIFT] || io.KeysDown[ci::app::KeyEvent::KEY_RSHIFT];
	io.KeyAlt = io.KeysDown[ci::app::KeyEvent::KEY_LALT] || io.KeysDown[ci::app::KeyEvent::KEY_RALT];
	io.KeySuper = io.KeysDown[ci::app::KeyEvent::KEY_LMETA] || io.KeysDown[ci::app::KeyEvent::KEY_RMETA] || io.KeysDown[ci::app::KeyEvent::KEY_LSUPER] || io.KeysDown[ci::app::KeyEvent::KEY_RSUPER];

	event.setHandled( io.WantCaptureKeyboard );
}

static void ImGui_ImplCinder_KeyUp( ci::app::KeyEvent& event )
{
	ImGuiIO& io = ImGui::GetIO();

	io.KeysDown[event.getCode()] = false;

	for( auto key : sAccelKeys ) {
		io.KeysDown[key] = false;
	}
	sAccelKeys.clear();

	io.KeyCtrl = io.KeysDown[ci::app::KeyEvent::KEY_LCTRL] || io.KeysDown[ci::app::KeyEvent::KEY_RCTRL] || io.KeysDown[ci::app::KeyEvent::KEY_LMETA] || io.KeysDown[ci::app::KeyEvent::KEY_RMETA];
	io.KeyShift = io.KeysDown[ci::app::KeyEvent::KEY_LSHIFT] || io.KeysDown[ci::app::KeyEvent::KEY_RSHIFT];
	io.KeyAlt = io.KeysDown[ci::app::KeyEvent::KEY_LALT] || io.KeysDown[ci::app::KeyEvent::KEY_RALT];

	event.setHandled( io.WantCaptureKeyboard );
}

static void ImGui_ImplCinder_Resize()
{
	int w = ci::app::getWindow()->getWidth();
	int h = ci::app::getWindow()->getHeight();
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = ImVec2( (float)w, (float)h );
}

static void ImGui_ImplCinder_NewFrameGuard( const ci::app::WindowRef& window ) {
	if( ! sTriggerNewFrame )
		return;

	ImGui_ImplOpenGL3_NewFrame();
	
	ImGuiIO& io = ImGui::GetIO();
	IM_ASSERT( io.Fonts->IsBuilt() ); // Font atlas needs to be built, call renderer _NewFrame() function e.g. ImGui_ImplOpenGL3_NewFrame() 

	// Setup display size
	int w = window->getWidth(), h = window->getHeight();
	int display_w = w, display_h = h;
	io.DisplaySize = ImVec2( (float)w, (float)h );
	io.DisplayFramebufferScale = ImVec2( w > 0 ? ( (float)display_w / w ) : 0, h > 0 ? ( (float)display_h / h ) : 0 );

	// Setup time step
	static double g_Time = 0.0f;
	double current_time = ci::app::getElapsedSeconds();
	io.DeltaTime = g_Time > 0.0 ? (float)( current_time - g_Time ) : (float)( 1.0f / 60.0f );
	g_Time = current_time;

	ImGui::NewFrame();

	sTriggerNewFrame = false;
}

static void ImGui_ImplCinder_PostDraw()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );
	sTriggerNewFrame = true;
}

static bool ImGui_ImplCinder_Init( const ci::app::WindowRef& window, bool auto_render )
{
	// Setup back-end capabilities flags
	ImGuiIO& io = ImGui::GetIO();
	io.BackendPlatformName = "imgui_impl_cinder";

	// Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
	io.KeyMap[ImGuiKey_Tab] = ci::app::KeyEvent::KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = ci::app::KeyEvent::KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = ci::app::KeyEvent::KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = ci::app::KeyEvent::KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow] = ci::app::KeyEvent::KEY_DOWN;
	io.KeyMap[ImGuiKey_Home] = ci::app::KeyEvent::KEY_HOME;
	io.KeyMap[ImGuiKey_End] = ci::app::KeyEvent::KEY_END;
	io.KeyMap[ImGuiKey_Delete] = ci::app::KeyEvent::KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = ci::app::KeyEvent::KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter] = ci::app::KeyEvent::KEY_RETURN;
	io.KeyMap[ImGuiKey_Escape] = ci::app::KeyEvent::KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A] = ci::app::KeyEvent::KEY_a;
	io.KeyMap[ImGuiKey_C] = ci::app::KeyEvent::KEY_c;
	io.KeyMap[ImGuiKey_V] = ci::app::KeyEvent::KEY_v;
	io.KeyMap[ImGuiKey_X] = ci::app::KeyEvent::KEY_x;
	io.KeyMap[ImGuiKey_Y] = ci::app::KeyEvent::KEY_y;
	io.KeyMap[ImGuiKey_Z] = ci::app::KeyEvent::KEY_z;
	io.KeyMap[ImGuiKey_Insert] = ci::app::KeyEvent::KEY_INSERT;
	io.KeyMap[ImGuiKey_Space] = ci::app::KeyEvent::KEY_SPACE;

#ifndef CINDER_LINUX
	// clipboard callbacks
	io.SetClipboardTextFn = []( void* user_data, const char* text ) {
		ci::Clipboard::setString( std::string( text ) );
	};
	io.GetClipboardTextFn = []( void* user_data ) {
		std::string str = ci::Clipboard::getString();
		static std::vector<char> strCopy;
		strCopy = std::vector<char>( str.begin(), str.end() );
		strCopy.push_back( '\0' );
		return (const char*)&strCopy[0];
	};
#endif
	sWindowConnections[window] += window->getSignalMouseDown().connect( ImGui_ImplCinder_MouseDown );
	sWindowConnections[window] += window->getSignalMouseUp().connect( ImGui_ImplCinder_MouseUp );
	sWindowConnections[window] += window->getSignalMouseMove().connect( ImGui_ImplCinder_MouseMove );
	sWindowConnections[window] += window->getSignalMouseDrag().connect( ImGui_ImplCinder_MouseDrag );
	sWindowConnections[window] += window->getSignalMouseWheel().connect( ImGui_ImplCinder_MouseWheel );
	sWindowConnections[window] += window->getSignalKeyDown().connect( ImGui_ImplCinder_KeyDown );
	sWindowConnections[window] += window->getSignalKeyUp().connect( ImGui_ImplCinder_KeyUp );
	sWindowConnections[window] += window->getSignalResize().connect( ImGui_ImplCinder_Resize );
	if( auto_render ) {
		sWindowConnections[window] += ci::app::App::get()->getSignalUpdate().connect( std::bind( ImGui_ImplCinder_NewFrameGuard, window ) );
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

void ImGui::Initialize( const ImGui::Options& options )
{
	if( sInitialized ) {
		throw ci::Exception( "ImGui is already initialized." );
	}

	IMGUI_CHECKVERSION();
	auto context = ImGui::CreateContext();
	
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	ci::app::WindowRef window = options.getWindow();
	io.DisplaySize = ci::vec2( window->getSize() );
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

	ImGui_ImplOpenGL3_Init( "#version 150" );
	
	if( options.isAutoRenderEnabled() ) {
		ImGui_ImplCinder_Init( window, true );
		ImGui_ImplCinder_NewFrameGuard( window );
	}
	else {
		ImGui_ImplCinder_Init( window, false );
		sTriggerNewFrame = false; //prevents resize() event from calling begin frame.
	}
	
	sAppConnections += ci::app::App::get()->getSignalCleanup().connect( [context]() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplCinder_Shutdown();
		ImGui::DestroyContext( context );
	} );

	sInitialized = true;
}
