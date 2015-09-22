#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"

#include "cinder/audio/Context.h"
#include "cinder/audio/GenNode.h"
#include "cinder/audio/NodeEffects.h"
#include "cinder/audio/MonitorNode.h"
#include "cinder/audio/Utilities.h"

#include "../../common/AudioDrawUtils.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class NodeAdvancedApp : public App {
  public:
	void setup() override;
	void mouseDrag( MouseEvent event ) override;
	void update() override;
	void draw() override;

	audio::GenNodeRef				mGen;		// GenNode's generate audio signals
	audio::FilterLowPassNodeRef		mLowpass;	// lowpass filter to reduce high frequency content.
	audio::GainNodeRef				mGain;		// modifies the volume of the signal
	audio::MonitorNodeRef			mMonitor;	// lets you retrieve audio samples in a thread-safe manner

	vector<size_t>	mCPentatonicScale;

	float mFreqRampTime;
};

void NodeAdvancedApp::setup()
{
	auto ctx = audio::Context::master();

	// Here we're using a GenTriangleNode, which generates a triangle waveform that contains many upper harmonics.
	// To reduce the sharpness, a lowpass filter is used to cut down the higher frequences.
	mGen = ctx->makeNode( new audio::GenTriangleNode( audio::Node::Format().autoEnable() ) );
	mLowpass = ctx->makeNode( new audio::FilterLowPassNode );
	mGain = ctx->makeNode( new audio::GainNode );
	mMonitor = ctx->makeNode( new audio::MonitorNode );

	mLowpass->setFreq( 400 );

	// Below we tell the Gain's Param to ramp from 0 to 0.5 over 2 seconds, making it slowly fade in.!
	mGain->getParam()->applyRamp( 0, 0.5f, 2.0f );

	// make the synthesis connection
	mGen >> mLowpass >> mGain >> ctx->getOutput();

	// Also feed the Gain to our Scope so that we can see what the waveform looks like.
	mGain >> mMonitor;

	ctx->enable();

	// Many times it is easier to specify musical pitches in MIDI format, which is linear rather than in hertz.
	// Below is the pentatonic notes for the C major scale from C3-C5, represented in MIDI values.
	mCPentatonicScale.push_back( 48 );
	mCPentatonicScale.push_back( 50 );
	mCPentatonicScale.push_back( 52 );
	mCPentatonicScale.push_back( 55 );
	mCPentatonicScale.push_back( 57 );
	mCPentatonicScale.push_back( 60 );
	mCPentatonicScale.push_back( 62 );
	mCPentatonicScale.push_back( 64 );
	mCPentatonicScale.push_back( 67 );
	mCPentatonicScale.push_back( 69 );
	mCPentatonicScale.push_back( 72 );

	mFreqRampTime = 0.015f;
}

void NodeAdvancedApp::mouseDrag( MouseEvent event )
{
	if( ! getWindowBounds().contains( event.getPos() ) )
		return;

	float yPercent = 1.0f - (float)event.getPos().y / (float)getWindowHeight();
	mLowpass->setFreq( 200 + yPercent * 800 );

	mFreqRampTime = 0.010f + event.getX() / 5000.0f;
}

void NodeAdvancedApp::update()
{
	size_t seqPeriod = 10 * randInt( 1, 4 );

	if( getElapsedFrames() % seqPeriod == 0 ) {
		size_t index = randInt( mCPentatonicScale.size() );
		size_t midiPitch = mCPentatonicScale.at( index );
		mGen->getParamFreq()->applyRamp( audio::midiToFreq( midiPitch ), mFreqRampTime );
	}
}

void NodeAdvancedApp::draw()
{
	gl::clear();

	// Draw the Scope's recorded Buffer in the upper right.
	if( mMonitor && mMonitor->getNumConnectedInputs() ) {
		Rectf scopeRect( getWindowWidth() - 210, 10, getWindowWidth() - 10, 110 );
		drawAudioBuffer( mMonitor->getBuffer(), scopeRect, true );
	}

	// Visualize the Gen's current pitch with a circle.

	float pitchMin = mCPentatonicScale.front();
	float pitchMax = mCPentatonicScale.back();
	float currentPitch = audio::freqToMidi( mGen->getFreq() ); // MIDI values do not have to be integers for us.

	float percent = ( currentPitch - pitchMin ) / ( pitchMax - pitchMin );

	float circleX = percent * getWindowWidth();

	gl::color( 0, 0.8f, 0.8f );
	gl::drawSolidCircle( vec2( circleX, getWindowCenter().y ), 50 );
}

CINDER_APP( NodeAdvancedApp, RendererGl( RendererGl::Options().msaa( 4 ) ), []( App::Settings *settings ) {
	settings->setMultiTouchEnabled( false );
} )
