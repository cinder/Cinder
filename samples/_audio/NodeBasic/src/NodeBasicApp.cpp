#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#include "cinder/audio/Context.h"
#include "cinder/audio/GenNode.h"
#include "cinder/audio/GainNode.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class NodeBasic : public AppNative {
  public:
	void prepareSettings( Settings *settings )	{ settings->enableMultiTouch( false ); }
	void setup();
	void mouseDrag( MouseEvent event ) override;
	void draw();

	audio::GenNodeRef	mGen;	// Gen's generate audio signals
	audio::GainNodeRef	mGain;	// Gain modifies the volume of the signal
};

void NodeBasic::setup()
{
	// a Context is required for making new audio Node's.
	auto ctx = audio::master();
	mGen = ctx->makeNode( new audio::GenSineNode );
	mGain = ctx->makeNode( new audio::GainNode );

	mGen->setFreq( 220 );
	mGain->setValue( 0.5f );

	// connections can be made this way or with connect(). The master Context's getOutput() is the speakers by default.
	mGen >> mGain >> ctx->getOutput();

	// Node's need to be enabled to process audio. EffectNode's are enabled by default, while NodeSource's (like Gen) need to be switched on.
	mGen->enable();

	// Context also must be started. Starting and stopping this controls the entire DSP graph.
	ctx->enable();
}

void NodeBasic::mouseDrag( MouseEvent event )
{
	mGen->setFreq( event.getPos().x );
	mGain->setValue( 1.0f - (float)event.getPos().y / (float)getWindowHeight() );
}

void NodeBasic::draw()
{
	gl::clear( Color( 0, mGain->getValue(), 0.2f ) );
}

CINDER_APP_NATIVE( NodeBasic, RendererGl )
