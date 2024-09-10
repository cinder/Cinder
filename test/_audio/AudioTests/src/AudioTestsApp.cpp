
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Timeline.h"
#include "cinder/Log.h"
#include "cinder/Utilities.h"
#include "cinder/audio/Context.h"
#include "cinder/CinderImGui.h"

#include "Factory.h"
#include "NodeTest.h"

using namespace ci;
using namespace std;
namespace im = ImGui;

// To use Live++, place necessary files in a folder called LivePP next to the cinder folder
#define	LIVEPP_ENABLED 1
#if LIVEPP_ENABLED
#include "LPP_API_x64_CPP.h"
#endif

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
	void printDefaultOutput();

	void initImGui();
	void updateImGui();

	bool mImGuiEnabled = true;

	mason::Factory<AudioTest>	mTestFactory;
	AudioTestRef				mCurrentTest;
};

void prepareSettings( app::App::Settings *settings )
{
#if LIVEPP_ENABLED
	{
		CI_LOG_I( "executable path:" << app::Platform::get()->getExecutablePath() );

		lpp::LppDefaultAgent lppAgent = lpp::LppCreateDefaultAgent( nullptr, L"../../../../../../../../../../LivePP" );
		lppAgent.EnableModule( lpp::LppGetCurrentModulePath(), lpp::LPP_MODULES_OPTION_ALL_IMPORT_MODULES, nullptr, nullptr );
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

	mTestFactory.registerBuilder<NodeTest>( "node basic" );

	//mSuite.addTest<MiscTest>( "misc" );
	//mSuite.addTest<FluidSmokeTest>( "fluid smoke" );
	//mSuite.addTest<TemporalRevealTest>( "temporal reveal" );

	printDefaultOutput();

	mCurrentTest = mTestFactory.build( "node basic" );
}

void AudioTests::reload()
{
	// TODO: reload current test
}

void AudioTests::printDefaultOutput()
{
	audio::DeviceRef device = audio::Device::getDefaultOutput();

	CI_LOG_I( "device name: " << device->getName() );
	console() << "\t input channels: " << device->getNumInputChannels() << endl;
	console() << "\t output channels: " << device->getNumOutputChannels() << endl;
	console() << "\t samplerate: " << device->getSampleRate() << endl;
	console() << "\t frames per block: " << device->getFramesPerBlock() << endl;
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
		else if( event.getChar() == '/' ) {
			audio::master()->setEnabled( ! audio::master()->isEnabled() );
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
	if( mCurrentTest ) {
		mCurrentTest->resize();
	}
}

void AudioTests::update()
{
	if( mCurrentTest ) {
		mCurrentTest->update();
	}

	if( mImGuiEnabled ) {
		updateImGui();	
	}
}

void AudioTests::draw()
{
	gl::clear( Color( 0, 0, 0 ) );

	if( mCurrentTest ) {
		mCurrentTest->draw();
	}
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

		im::Separator();
		im::Text( "Audio" );

		bool audioEnabled = audio::master()->isEnabled();
		if( im::Checkbox( "enabled", &audioEnabled ) ) {
			audio::master()->setEnabled( audioEnabled );
		}

		if( im::Button( "print audio graph" ) ) {
			app::console() << "-------------- Graph configuration: --------------" << endl;
			app::console() << audio::master()->printGraphToString();
			app::console() << "--------------------------------------------------" << endl;
		}
	}
	im::End(); // "General"

	//mSuite.updateUI();
	if( mCurrentTest ) {
		im::SetNextWindowPos( { 400, 100 }, ImGuiCond_FirstUseEver );
		im::SetNextWindowSize( { 800, 600 }, ImGuiCond_FirstUseEver );
		im::Begin( mCurrentTest->getName().c_str() );

		mCurrentTest->updateUI();

		im::End();
	}
}

CINDER_APP( AudioTests, app::RendererGl( app::RendererGl::Options().msaa( 2 ) ), prepareSettings )
