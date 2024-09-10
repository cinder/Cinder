
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Timeline.h"
#include "cinder/Log.h"
#include "cinder/Utilities.h"
#include "cinder/CinderImGui.h"

using namespace ci;
using namespace std;
namespace im = ImGui;

#define	LIVEPP_ENABLED 0

const int	SCREEN_INDEX	= 2;
const ivec2 WINDOW_POS		= { 2000, 50 };
const ivec2 WINDOW_SIZE		= { 1600, 1000 };

class AudioTests : public app::App {
  public:
	void setup() override;
	void keyDown( app::KeyEvent event ) override;
	void resize() override;
	void update() override;
	void draw() override;

	void reload();
	void initImGui();
	void updateImGui();

	bool mImGuiEnabled = true;

	//ma::TestSuite	mSuite;
};

void prepareSettings( app::App::Settings *settings )
{
#if LIVEPP_ENABLED
	{
		liveppEnabled = ma::initLivePP( "../../../../tools/LivePP", "AudioTests" );
		CI_LOG_I( "Live++ " << string( liveppEnabled ? "enabled" : "disabled" ) );
	}
#endif

	settings->setWindowPos( WINDOW_POS );
	settings->setWindowSize( WINDOW_SIZE );
	if( SCREEN_INDEX > 0 && Display::getDisplays().size() >= SCREEN_INDEX ) {
		settings->setDisplay( Display::getDisplays()[SCREEN_INDEX] );
	}
}

void AudioTests::setup()
{
	initImGui();

	//mSuite.addTest<MiscTest>( "misc" );
	//mSuite.addTest<FluidSmokeTest>( "fluid smoke" );
	//mSuite.addTest<TemporalRevealTest>( "temporal reveal" );

	reload();
}

void AudioTests::reload()
{
	CI_LOG_I( "reloading" );

}

void AudioTests::keyDown( app::KeyEvent event )
{
	bool handled = false;
	if( event.isControlDown() ) {
		handled = true;
		if( tolower( event.getChar() ) == 'r' ) {
			reload();
		}
		else if( event.getChar() == 'f' ) {
			setFullScreen( ! isFullScreen() );
		}
		else if( event.getChar() == 'g' ) {
			mImGuiEnabled = ! mImGuiEnabled;
			CI_LOG_I( "ImGui enabled: " << mImGuiEnabled );
		}
		else if( event.getChar() == 'q' ) {
			CI_LOG_I( "Ctrl + q: quitting app." );
			quit();
		}
		else {
			handled = false;
		}
	}

	//if( ! handled ) {
	//	mSuite.keyDown( event );
	//}
}

void AudioTests::resize()
{
	//mSuite.layout();
}

void AudioTests::update()
{
	//mSuite.update();

	if( mImGuiEnabled ) {
		updateImGui();	
	}
}

void AudioTests::draw()
{
	gl::clear( Color( 0, 0, 0 ) );

	//mSuite.draw();

}

// ----------------------------------------------------------------------------------------------------
// ImGui
// ----------------------------------------------------------------------------------------------------

static bool sThemeIsDark = true;
static float sThemeAlpha = 0.85f;

// called on first render loop, because we need a valid gl::context
void AudioTests::initImGui()
{
	IMGUI_CHECKVERSION();
	im::Initialize();
}

// only called from main thread, after ImGui has been initialized on the first Window's render thread
void AudioTests::updateImGui()
{
	//im::GetStyle().WindowRounding = 4.0f;
	//im::GetStyle().Alpha = 0.85f;

	//im::GetStyle().Colors[ImGuiCol_Text] = Color( 0.187f, 1, 0.269f );
	//im::GetStyle().Colors[ImGuiCol_Text] = Color( 0.187f, 1, 1 );

	im::GetIO().FontGlobalScale = getWindowContentScale();
	im::GetIO().FontAllowUserScaling = true;

	//im::GetIO().ConfigWindowsMoveFromTitleBarOnly = false;
	//im::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	if( im::Begin( "General", nullptr ) ) {
		im::Text( "fps: %0.2f, seconds running: %0.1f", app::App::get()->getAverageFps(), app::App::get()->getElapsedSeconds() );
		im::Separator();
		//im::SameLine(); im::Text( "(ctrl + s)" );

		if( im::Button( "reload" ) ) {
			reload();
		}
		im::SameLine();
		im::Text( "user settings" );

		im::Checkbox( "GUI (ctrl + g)", &mImGuiEnabled );
	}
	im::End(); // "General"

	//mSuite.updateUI();
}

CINDER_APP( AudioTests, app::RendererGl( app::RendererGl::Options().msaa( 2 ) ), prepareSettings )
