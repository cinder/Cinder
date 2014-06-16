#include "cinder/app/AppNative.h"
#include "cinder/audio/Context.h"
#include "cinder/audio/GenNode.h"
#include "cinder/audio/MonitorNode.h"
#include "cinder/audio/GainNode.h"
#include "cinder/gl/gl.h"

#include "../../common/AudioDrawUtils.h"

#include "CustomTremoloNode.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class NodeSubclassingApp : public AppNative {
  public:
	void setup();
	void mouseMove( MouseEvent event );
	void update();
	void draw();

  private:
	audio::GenOscNodeRef	mGenNode;
	audio::MonitorNodeRef	mMonitorNode;
	CustomTremoloNodeRef	mCustomTremeloNode;
};

void NodeSubclassingApp::setup()
{
	auto ctx = audio::master();

	mGenNode = ctx->makeNode( new audio::GenOscNode( audio::WaveformType::SQUARE, 220 ) );
	mMonitorNode = ctx->makeNode( new audio::MonitorNode );
	auto gain = ctx->makeNode( new audio::GainNode( 0.2f ) );

	mCustomTremeloNode = ctx->makeNode( new CustomTremoloNode );

	mGenNode >> mCustomTremeloNode >> mMonitorNode >> gain >> ctx->getOutput();

	mGenNode->enable();
	ctx->enable();

	console() << "\naudio graph:\n" << ctx->printGraphToString() << endl;
}

void NodeSubclassingApp::mouseMove( MouseEvent event )
{
	Vec2f pos = event.getPos();

	float rate = 20.0f * pos.x / (float)getWindowWidth();
	float depth = 1.0f - pos.y / (float)getWindowHeight();

	mCustomTremeloNode->setRate( rate );
	mCustomTremeloNode->setDepth( depth );
}

void NodeSubclassingApp::update()
{
}

void NodeSubclassingApp::draw()
{
	gl::clear();

	if( mMonitorNode && mMonitorNode->isEnabled() ) {
		Rectf scopeRect( 10, 10, (float)getWindowWidth() - 10, (float)getWindowHeight() - 10 );
		drawAudioBuffer( mMonitorNode->getBuffer(), scopeRect, true, Color( 0.9f, 0.4f, 0 ) );
	}
}

CINDER_APP_NATIVE( NodeSubclassingApp, RendererGl )
