
#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Timeline.h"
#include "cinder/Log.h"
#include "cinder/Utilities.h"
#include "cinder/audio/Context.h"
#include "cinder/CinderImGui.h"

#include "Factory.h"
#include "NodeBasicTest.h"
#include "NodeEffectsTest.h"
#include "ParamTest.h"

using namespace ci;
using namespace std;
namespace im = ImGui;

#define TEST_LOW_LATENCY 0
#define WASAPI_EXCLUSIVE_MODE 1 // MSW only

#if WASAPI_EXCLUSIVE_MODE && defined( CINDER_MSW )
#include "cinder/audio/msw/ContextWasapi.h"
#endif

// To use Live++, place necessary files in a folder called LivePP next to the cinder folder
#define	LIVEPP_ENABLED 1
#if LIVEPP_ENABLED
#include "LPP_API_x64_CPP.h"
#endif

const int	SCREEN_INDEX	= 1;
const ivec2 WINDOW_POS		= { 2000, 50 };
const ivec2 WINDOW_SIZE		= { 1600, 1000 };

class AudioTestsApp : public app::App {
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
	void updateContextUI();

	bool	mImGuiEnabled		= true;
	bool	mContextUIEnabled	= true;

	mason::Factory<AudioTest>	mTestFactory;
	AudioTestRef				mCurrentTest;
	int							mCurrenTestIndex = 0;
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

void AudioTestsApp::setup()
{
	initImGui();

	mTestFactory.registerBuilder<NodeBasicTest>( "Node Basic" );
	mTestFactory.registerBuilder<NodeEffectsTest>( "Node Effects" );
	mTestFactory.registerBuilder<ParamTest>( "Param" );

	auto ctx = audio::master();

#if WASAPI_EXCLUSIVE_MODE && defined( CINDER_MSW )
	CI_LOG_I( "enabling WASAPI exlusive mode" );
	dynamic_cast<audio::msw::ContextWasapi *>( ctx )->setExclusiveModeEnabled();
#endif
#if TEST_LOW_LATENCY
	auto lineOut = ctx->createOutputDeviceNode();
	lineOut->getDevice()->updateFormat( audio::Device::Format().framesPerBlock( 64 ) );
	ctx->setOutput( lineOut );
#endif

	CI_LOG_I( "Context samplerate: " << ctx->getSampleRate() << ", frames per block: " << ctx->getFramesPerBlock() );
	printDefaultOutput();

	reload();
}

void AudioTestsApp::reload()
{
	auto ctx = audio::master();
	audio::ScopedEnableContext scopedDisableCtx( ctx, false );
	ctx->disconnectAllNodes();

	string testName = mTestFactory.getAllKeys()[mCurrenTestIndex];
	mCurrentTest = mTestFactory.build( testName );
	mCurrentTest->setName( testName );

	CI_LOG_I( "finished building test '" << testName << "', type: " << System::demangleTypeName( typeid( *mCurrentTest.get() ).name() ) );
}

void AudioTestsApp::printDefaultOutput()
{
	audio::DeviceRef device = audio::Device::getDefaultOutput();

	CI_LOG_I( "device name: " << device->getName() );
	console() << "\t input channels: " << device->getNumInputChannels() << endl;
	console() << "\t output channels: " << device->getNumOutputChannels() << endl;
	console() << "\t samplerate: " << device->getSampleRate() << endl;
	console() << "\t frames per block: " << device->getFramesPerBlock() << endl;
}

void AudioTestsApp::keyDown( app::KeyEvent event )
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
	else if( event.getCode() == app::KeyEvent::KEY_SLASH ) {
		audio::master()->setEnabled( ! audio::master()->isEnabled() );
		CI_LOG_I( "ImGui enabled: " << mImGuiEnabled );
	}

	//if( ! handled ) {
	//	mCurrentTest->keyDown( event );
	//}
}

void AudioTestsApp::resize()
{
	if( mCurrentTest ) {
		mCurrentTest->resize();
	}
}

void AudioTestsApp::update()
{
	if( mCurrentTest ) {
		mCurrentTest->update();
	}

	if( mImGuiEnabled ) {
		updateImGui();	
	}
}

void AudioTestsApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );

	if( mCurrentTest ) {
		mCurrentTest->draw();
	}
}

// ----------------------------------------------------------------------------------------------------
// ImGui
// ----------------------------------------------------------------------------------------------------

namespace {

static bool sThemeIsDark = true;
static float sThemeAlpha = 0.85f;

void substituteNodeLabel( std::string &label )
{
	static vector<pair<string,string>> nameSubstitutes = {
		{ "class cinder::", "ci::" },
		{ "class mason::", "ma::" }
	};

	for( const auto &sub : nameSubstitutes ) {
		auto pos = label.find( sub.first, 0 );
		if( pos != string::npos ) {
			label.replace( pos, sub.first.size(), sub.second );
		}
	}
}

// note: recursion level currently only for debugging
void printNodeFn( audio::Node *node, bool expand, audio::Node **hovered )
{
	string label = node->getName();
	substituteNodeLabel( label );

	ImGuiTreeNodeFlags treeNodeFlags = 0;
	im::PushStyleColor( ImGuiCol_Text, node->isEnabled() ? vec4( 1 ) : im::GetStyleColorVec4( ImGuiCol_TextDisabled ) );
	if( node->getNumConnectedInputs() > 0 ) {
		if( expand )
			im::SetNextTreeNodeOpen( true );

		if( im::TreeNodeEx( label.c_str(), treeNodeFlags ) ) {
			if( im::IsItemHovered() ) {
				*hovered = node;
			}

			for( const auto &input : node->getInputs() ) {
				printNodeFn( input.get(), expand, hovered );
			}

			im::TreePop();
		}
	}
	else {
		treeNodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Bullet;
		im::TreeNodeEx( label.c_str(), treeNodeFlags );

		if( im::IsItemHovered() ) {
			*hovered = node;
		}
	}

	im::PopStyleColor();
};

} // anon namespace

// called on first render loop, because we need a valid gl::context
void AudioTestsApp::initImGui()
{
	IMGUI_CHECKVERSION();
	im::Initialize();
}

// only called from main thread, after ImGui has been initialized on the first Window's render thread
void AudioTestsApp::updateImGui()
{
	im::GetIO().FontGlobalScale = getWindowContentScale();
	im::GetIO().FontAllowUserScaling = true;

	//im::GetIO().ConfigWindowsMoveFromTitleBarOnly = false;
	//im::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	if( im::Begin( "General", nullptr ) ) {
		im::Text( "fps: %0.2f, seconds running: %0.1f", app::App::get()->getAverageFps(), app::App::get()->getElapsedSeconds() );
		im::Separator();

		if( im::Button( "reload" ) ) {
			reload();
		}
		im::SameLine();
		im::Text( "user settings" );

		im::Checkbox( "GUI (ctrl + g)", &mImGuiEnabled );
		im::Checkbox( "Show audio::Context UI", &mContextUIEnabled );

		im::Separator();
		im::Text( "Audio" );

		bool audioEnabled = audio::master()->isEnabled();
		if( im::Checkbox( "enabled", &audioEnabled ) ) {
			audio::master()->setEnabled( audioEnabled );
		}

		static vector<string> sTests = mTestFactory.getAllKeys();
		if( im::Combo( "tests", &mCurrenTestIndex, sTests, sTests.size() ) ) {
			reload();
		}

		if( im::Button( "print audio graph" ) ) {
			app::console() << audio::master()->printGraphToString();
			app::console() << "--------------------------------------------------" << endl;
			app::console() << "-------------- Graph configuration: --------------" << endl;
		}
	}
	im::End(); // "General"

	if( mCurrentTest ) {
		im::SetNextWindowPos( { 400, 100 }, ImGuiCond_FirstUseEver );
		im::SetNextWindowSize( { 800, 600 }, ImGuiCond_FirstUseEver );
		im::Begin( mCurrentTest->getName().c_str() );

		mCurrentTest->updateUI();

		im::End();
	}

	if( mContextUIEnabled ) {
		updateContextUI();
	}
}

void AudioTestsApp::updateContextUI()
{
	im::SetNextWindowPos( ivec2( 681, 2 ), ImGuiCond_FirstUseEver );
	im::SetNextWindowSize( ivec2( 330, 420 ), ImGuiCond_FirstUseEver );
	im::SetNextWindowCollapsed( true, ImGuiCond_FirstUseEver );

	if( ! im::Begin( "Audio Context", &mContextUIEnabled ) ) {
		im::End();
		return;
	}

	auto ctx = audio::master();

	im::Text( "dsp %s, samplerate: %d", ( ctx->isEnabled() ? "enabled" : "disabled" ), (int)ctx->getSampleRate() );
	im::Separator();
	im::Text( "Context Graph:" );
	im::SameLine();

	static bool expand = false;
	im::Checkbox( "expand all", &expand );

	im::BeginChild( "Context Graph", vec2( 0, im::GetWindowHeight() - 100 ) );
	audio::Node *hovered = nullptr;
	printNodeFn( ctx->getOutput().get(), expand, &hovered );

	size_t numAutoPulledNodes = ctx->getAutoPulledNodes().size();
	if( numAutoPulledNodes ) {
		im::Separator();
		im::Text( "auto pulled nodes: %d", numAutoPulledNodes );
		for( const auto& node : ctx->getAutoPulledNodes() ) {
			printNodeFn( node, expand, &hovered );
		}
	}

	im::EndChild();

	// show info at bottom of window for hovered audio::Nodes
	im::Separator();
	if( hovered ) {
		string name = hovered->getName();
		substituteNodeLabel( name );

		const char *processMode = hovered->getProcessesInPlace() ? "in-place" : "sum"; 
		string channelMode;
		switch( hovered->getChannelMode() ) {
			case audio::Node::ChannelMode::SPECIFIED:		channelMode = "specified"; break;
			case audio::Node::ChannelMode::MATCHES_INPUT:	channelMode = "matches input"; break;
			case audio::Node::ChannelMode::MATCHES_OUTPUT:	channelMode = "matches output"; break;
			default: CI_ASSERT_NOT_REACHABLE(); break;
		}

		im::Text( "%s: channels: %d, mode: %s (%s)", name.c_str(), hovered->getNumChannels(), processMode, channelMode.c_str() );

		auto mathNode = dynamic_cast<audio::MathNode *>( hovered );
		if( mathNode ) {
			im::SameLine();
			im::Text( "| value: %0.3f", mathNode->getValue() );
		}
	}
	else {
		im::Text( "" );
	}

	im::End(); // Audio Context
}

CINDER_APP( AudioTestsApp, app::RendererGl( app::RendererGl::Options().msaa( 2 ) ), prepareSettings )
