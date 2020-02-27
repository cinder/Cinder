#include "imgui_impl_cinder.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Clipboard.h"

#if defined(_WIN32)
#include "Windows.h"
#endif

static ci::signals::ConnectionList sWindowConnections;

struct ImGuiPlatformDataCinder
{
	ci::app::WindowRef	Window;
	bool				WindowOwned;

	ImGuiPlatformDataCinder() { Window = nullptr; WindowOwned = false; }
	~ImGuiPlatformDataCinder() { IM_ASSERT(Window == nullptr); }
};

void ImGui_ImplCinder_InstallCallbacks( const ci::app::WindowRef &window );

static void ImGui_ImplCinder_CreateWindow(ImGuiViewport* viewport)
{
	ImGuiPlatformDataCinder* data = IM_NEW(ImGuiPlatformDataCinder)();
	viewport->PlatformUserData = data;

	ci::app::Window::Format windowFormat;
	windowFormat.setBorderless( (viewport->Flags & ImGuiViewportFlags_NoDecoration) ? true : false );
	windowFormat.setSize( (int)viewport->Size.x, (int)viewport->Size.y );
	windowFormat.setTitle( "No Title Yet" );
	
	data->Window = ci::app::App::get()->createWindow( windowFormat );
	data->Window->hide();
	data->WindowOwned = true;
	viewport->PlatformHandle = (void*) data->Window.get();
	viewport->PlatformHandleRaw = data->Window->getNative();

	std::dynamic_pointer_cast<ci::app::RendererGl>( data->Window->getRenderer() )->setStartDrawFn( []( ci::app::Renderer* renderer ) {} );
	std::dynamic_pointer_cast<ci::app::RendererGl>( data->Window->getRenderer() )->setFinishDrawFn( []( ci::app::Renderer* renderer ) {} );
	
	data->Window->getSignalClose().connect( [viewport]() { 
		viewport->PlatformRequestClose = true;
	} );
	
	ImGui_ImplCinder_InstallCallbacks( data->Window );
}

static void ImGui_ImplCinder_DestroyWindow(ImGuiViewport* viewport)
{
	if (ImGuiPlatformDataCinder* data = (ImGuiPlatformDataCinder*)viewport->PlatformUserData)
	{
		if (data->Window && data->WindowOwned)
			data->Window->close();
		data->Window.reset();
		IM_DELETE(data);
	}
	viewport->PlatformUserData = viewport->PlatformHandle = NULL;
}

static void ImGui_ImplCinder_ShowWindow(ImGuiViewport* viewport)
{
	ImGuiPlatformDataCinder* data = (ImGuiPlatformDataCinder*)viewport->PlatformUserData;
	if( data->Window->isHidden() ) {
		data->Window->show();
	}

#if defined(_WIN32)
	// GLFW hack: Hide icon from task bar
	HWND hwnd = (HWND) data->Window->getNative();
	if( viewport->Flags & ImGuiViewportFlags_NoDecoration ) {
		LONG ex_style = ::GetWindowLong(hwnd, GWL_EXSTYLE);
		ex_style &= ~WS_EX_APPWINDOW;
		ex_style |= WS_EX_TOOLWINDOW;
		::SetWindowLong(hwnd, GWL_EXSTYLE, ex_style);
	}

	if (viewport->Flags & ImGuiViewportFlags_NoFocusOnAppearing)
	{
		::ShowWindow(hwnd, SW_SHOWNA);
		return;
	}
#endif
}

static ImVec2 ImGui_ImplCinder_GetWindowPos(ImGuiViewport* viewport)
{
	ImGuiPlatformDataCinder* data = (ImGuiPlatformDataCinder*)viewport->PlatformUserData;
	return ImVec2( data->Window->getPos().x, data->Window->getPos().y );
}

static void ImGui_ImplCinder_SetWindowPos(ImGuiViewport* viewport, ImVec2 pos)
{
	ImGuiPlatformDataCinder* data = (ImGuiPlatformDataCinder*)viewport->PlatformUserData;
	if( data->Window->getPos().x != pos.x || data->Window->getPos().y != pos.y ) {
		data->Window->setPos( (int)pos.x, (int)pos.y );
	}
}

static ImVec2 ImGui_ImplCinder_GetWindowSize(ImGuiViewport* viewport)
{
	ImGuiPlatformDataCinder* data = (ImGuiPlatformDataCinder*)viewport->PlatformUserData;
	return ImVec2( data->Window->getWidth(), data->Window->getHeight() );
}

static void ImGui_ImplCinder_SetWindowSize(ImGuiViewport* viewport, ImVec2 size)
{
	ImGuiPlatformDataCinder* data = (ImGuiPlatformDataCinder*)viewport->PlatformUserData;
	if( data->Window->getWidth() != size.x || data->Window->getHeight() != size.y ) {
		data->Window->setSize( (int)size.x, (int)size.y );
	}
}

static void ImGui_ImplCinder_SetWindowTitle(ImGuiViewport* viewport, const char* title)
{
	ImGuiPlatformDataCinder* data = (ImGuiPlatformDataCinder*)viewport->PlatformUserData;
	data->Window->setTitle( std::string( title ) );
}

static void ImGui_ImplCinder_RenderWindow( ImGuiViewport* viewport, void* )
{
	ImGuiPlatformDataCinder* data = (ImGuiPlatformDataCinder*)viewport->PlatformUserData;
	data->Window->getRenderer()->makeCurrentContext();
}

static void ImGui_ImplCinder_SwapBuffers( ImGuiViewport* viewport, void* )
{
	ImGuiPlatformDataCinder* data = (ImGuiPlatformDataCinder*)viewport->PlatformUserData;
	data->Window->getRenderer()->makeCurrentContext();
	data->Window->getRenderer()->swapBuffers();
}

static void ImGui_ImplCinder_InitPlatformInterface()
{
	// Register platform interface (will be coupled with a renderer interface)
	ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
	platform_io.Platform_CreateWindow = ImGui_ImplCinder_CreateWindow;
	platform_io.Platform_DestroyWindow = ImGui_ImplCinder_DestroyWindow;
	platform_io.Platform_ShowWindow = ImGui_ImplCinder_ShowWindow;
	platform_io.Platform_SetWindowPos = ImGui_ImplCinder_SetWindowPos;
	platform_io.Platform_GetWindowPos = ImGui_ImplCinder_GetWindowPos;
	platform_io.Platform_SetWindowSize = ImGui_ImplCinder_SetWindowSize;
	platform_io.Platform_GetWindowSize = ImGui_ImplCinder_GetWindowSize;
	platform_io.Platform_SetWindowTitle = ImGui_ImplCinder_SetWindowTitle;
	platform_io.Platform_RenderWindow = ImGui_ImplCinder_RenderWindow;
	platform_io.Platform_SwapBuffers = ImGui_ImplCinder_SwapBuffers;

	// Register main window handle
	ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGuiPlatformDataCinder* data = IM_NEW(ImGuiPlatformDataCinder)();
	data->Window = ci::app::getWindow();
	data->WindowOwned = false;
	main_viewport->PlatformUserData = data;
	main_viewport->PlatformHandle = (void*)data->Window.get();
#ifdef _WIN32
	main_viewport->PlatformHandleRaw = data->Window->getNative();
#endif
}

void ImGui_ImplCinder_MouseDown( ci::app::MouseEvent &event )
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ci::app::toPixels( event.getPos() );
	io.MouseDown[0] = event.isLeftDown();
	io.MouseDown[1] = event.isRightDown();
	io.MouseDown[2] = event.isMiddleDown();
	event.setHandled( io.WantCaptureMouse );
}
void ImGui_ImplCinder_MouseUp( ci::app::MouseEvent &event )
{
	ImGuiIO& io     = ImGui::GetIO();
	io.MouseDown[0] = event.isLeftDown();
	io.MouseDown[1] = event.isRightDown();
	io.MouseDown[2] = event.isMiddleDown();
}
void ImGui_ImplCinder_MouseWheel( ci::app::MouseEvent &event )
{
	ImGuiIO& io     = ImGui::GetIO();
	io.MouseWheel   += event.getWheelIncrement();
	event.setHandled( io.WantCaptureMouse );
}

void ImGui_ImplCinder_MouseMove( ci::app::MouseEvent& event )
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ci::app::toPixels( event.getPos() );
	event.setHandled( io.WantCaptureMouse );
}
//! sets the right mouseDrag IO values in imgui
void ImGui_ImplCinder_MouseDrag( ci::app::MouseEvent& event )
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ci::app::toPixels( event.getPos() );
	event.setHandled( io.WantCaptureMouse );
}

std::vector<int> sAccelKeys;
void ImGui_ImplCinder_KeyDown( ci::app::KeyEvent &event )
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

void ImGui_ImplCinder_KeyUp( ci::app::KeyEvent &event )
{
	ImGuiIO& io = ImGui::GetIO();

	io.KeysDown[event.getCode()] = false;

	for( auto key : sAccelKeys ){
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

void ImGui_ImplCinder_InstallCallbacks( const ci::app::WindowRef &window )
{
	sWindowConnections += window->getSignalMouseDown().connect( ImGui_ImplCinder_MouseDown );
	sWindowConnections += window->getSignalMouseUp().connect( ImGui_ImplCinder_MouseUp );
	sWindowConnections += window->getSignalMouseMove().connect( ImGui_ImplCinder_MouseMove );
	sWindowConnections += window->getSignalMouseDrag().connect( ImGui_ImplCinder_MouseDrag );
	sWindowConnections += window->getSignalMouseWheel().connect( ImGui_ImplCinder_MouseWheel );
	sWindowConnections += window->getSignalKeyDown().connect( ImGui_ImplCinder_KeyDown );
	sWindowConnections += window->getSignalKeyUp().connect( ImGui_ImplCinder_KeyUp );
	sWindowConnections += window->getSignalResize().connect( ImGui_ImplCinder_Resize );
}

bool ImGui_ImplCinder_Init( const ci::app::WindowRef &window, bool install_callbacks )
{
	// Setup back-end capabilities flags
	ImGuiIO& io = ImGui::GetIO();
	//io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
	//io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
	//io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;    // We can create multi-viewports on the Platform side (optional)
	io.BackendPlatformName = "imgui_impl_cinder";

	// Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
	io.KeyMap[ImGuiKey_Tab]             = ci::app::KeyEvent::KEY_TAB;
	io.KeyMap[ImGuiKey_LeftArrow]       = ci::app::KeyEvent::KEY_LEFT;
	io.KeyMap[ImGuiKey_RightArrow]      = ci::app::KeyEvent::KEY_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow]         = ci::app::KeyEvent::KEY_UP;
	io.KeyMap[ImGuiKey_DownArrow]       = ci::app::KeyEvent::KEY_DOWN;
	io.KeyMap[ImGuiKey_Home]            = ci::app::KeyEvent::KEY_HOME;
	io.KeyMap[ImGuiKey_End]             = ci::app::KeyEvent::KEY_END;
	io.KeyMap[ImGuiKey_Delete]          = ci::app::KeyEvent::KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace]       = ci::app::KeyEvent::KEY_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter]           = ci::app::KeyEvent::KEY_RETURN;
	io.KeyMap[ImGuiKey_Escape]          = ci::app::KeyEvent::KEY_ESCAPE;
	io.KeyMap[ImGuiKey_A]               = ci::app::KeyEvent::KEY_a;
	io.KeyMap[ImGuiKey_C]               = ci::app::KeyEvent::KEY_c;
	io.KeyMap[ImGuiKey_V]               = ci::app::KeyEvent::KEY_v;
	io.KeyMap[ImGuiKey_X]               = ci::app::KeyEvent::KEY_x;
	io.KeyMap[ImGuiKey_Y]               = ci::app::KeyEvent::KEY_y;
	io.KeyMap[ImGuiKey_Z]               = ci::app::KeyEvent::KEY_z;
	io.KeyMap[ImGuiKey_Insert]			= ci::app::KeyEvent::KEY_INSERT;
	io.KeyMap[ImGuiKey_Space]			= ci::app::KeyEvent::KEY_SPACE;

#ifndef CINDER_LINUX
	// clipboard callbacks
	io.SetClipboardTextFn = []( void* user_data, const char* text ) {
		ci::Clipboard::setString( std::string( text ) );
	};
	io.GetClipboardTextFn = []( void* user_data ) {
		std::string str = ci::Clipboard::getString();
		static std::vector<char> strCopy;
		strCopy = std::vector<char>(str.begin(), str.end());
		strCopy.push_back('\0');
		return (const char *) &strCopy[0];
	};
#endif
	if (install_callbacks)
		ImGui_ImplCinder_InstallCallbacks( window );

	// Our mouse update function expect PlatformHandle to be filled for the main viewport
	ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	main_viewport->PlatformHandle = (void*) window.get();
#ifdef _WIN32
	main_viewport->PlatformHandleRaw = window->getNative();
#endif
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
		ImGui_ImplCinder_InitPlatformInterface();

	return true;
}

static void ImGui_ImplCinder_UpdateMouse()
{
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
	io.MouseHoveredViewport = 0;
	ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
	for( int n = 0; n < platform_io.Viewports.Size; n++ )
	{
		ImGuiViewport* viewport = platform_io.Viewports[n];
		ci::app::Window* window = (ci::app::Window*) viewport->PlatformHandle;
		IM_ASSERT(window != NULL);
		
		if( window == ci::app::getWindow().get() ) {
			ci::ivec2 mouse = ci::app::App::get()->getMousePos() - window->getPos();
			io.MousePos = ImVec2( (float) mouse.x + viewport->Pos.x, (float) mouse.y + viewport->Pos.y);
			io.MouseHoveredViewport = 0;
		}
	}
}

void ImGui_ImplCinder_NewFrame()
{
	ImGuiIO& io = ImGui::GetIO();
	IM_ASSERT(io.Fonts->IsBuilt());     // Font atlas needs to be built, call renderer _NewFrame() function e.g. ImGui_ImplOpenGL3_NewFrame() 

										// Setup display size
	int w = ci::app::getWindowIndex( 0 )->getWidth(), h = ci::app::getWindowIndex( 0 )->getHeight();
	int display_w = w, display_h = h;
	
	io.DisplaySize = ImVec2((float)w, (float)h);
	io.DisplayFramebufferScale = ImVec2(w > 0 ? ((float)display_w / w) : 0, h > 0 ? ((float)display_h / h) : 0);

	// Setup time step
	static double g_Time = 0.0f;
	double current_time = ci::app::getElapsedSeconds();
	io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f/60.0f);
	g_Time = current_time;

	ImGui_ImplCinder_UpdateMouse();
}

void ImGui_ImplCinder_Shutdown()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	main_viewport->PlatformHandle = NULL;
	main_viewport->PlatformHandleRaw = NULL;
}
