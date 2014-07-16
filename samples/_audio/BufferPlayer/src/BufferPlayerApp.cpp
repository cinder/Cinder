#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

#include "cinder/audio/Context.h"
#include "cinder/audio/NodeEffects.h"
#include "cinder/audio/SamplePlayerNode.h"

#include "Resources.h"
#include "../../common/AudioDrawUtils.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BufferPlayerNodeApp : public AppNative {
public:
	void prepareSettings( Settings *settings );
	void setup();
	void fileDrop( FileDropEvent event );
	void keyDown( KeyEvent event );
	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void draw();

	audio::GainNodeRef				mGain;
	audio::BufferPlayerNodeRef		mBufferPlayerNode;

	WaveformPlot				mWaveformPlot;
};

void BufferPlayerNodeApp::prepareSettings( Settings *settings )
{
	settings->enableMultiTouch( false );
}

void BufferPlayerNodeApp::setup()
{
	auto ctx = audio::Context::master();

	// create a SourceFile and set its output samplerate to match the Context.
	audio::SourceFileRef sourceFile = audio::load( loadResource( RES_DRAIN_OGG ), ctx->getSampleRate() );

	// load the entire sound file into a BufferRef, and construct a BufferPlayerNode with this.
	audio::BufferRef buffer = sourceFile->loadBuffer();
	mBufferPlayerNode = ctx->makeNode( new audio::BufferPlayerNode( buffer ) );

	// add a Gain to reduce the volume
	mGain = ctx->makeNode( new audio::GainNode( 0.5f ) );

	// connect and enable the Context
	mBufferPlayerNode >> mGain >> ctx->getOutput();
	ctx->enable();

	// also load the buffer into our waveform visual util.
	mWaveformPlot.load( buffer, getWindowBounds() );
}

void BufferPlayerNodeApp::fileDrop( FileDropEvent event )
{
	fs::path filePath = event.getFile( 0 );
	getWindow()->setTitle( filePath.filename().string() );

	audio::SourceFileRef sourceFile = audio::load( loadFile( filePath ) );

	// BufferPlayerNode can also load a buffer directly from the SourceFile.
	// This is safe to call on a background thread, which would alleviate blocking the UI loop.
	mBufferPlayerNode->loadBuffer( sourceFile );

	mWaveformPlot.load( mBufferPlayerNode->getBuffer(), getWindowBounds() );
}

void BufferPlayerNodeApp::keyDown( KeyEvent event )
{
	if( event.getCode() == KeyEvent::KEY_SPACE ) {
		if( mBufferPlayerNode->isEnabled() )
			mBufferPlayerNode->stop();
		else
			mBufferPlayerNode->start();
	}
}

void BufferPlayerNodeApp::mouseDown( MouseEvent event )
{
	mBufferPlayerNode->start();
}

void BufferPlayerNodeApp::mouseDrag( MouseEvent event )
{
	if( mBufferPlayerNode->isEnabled() )
		mBufferPlayerNode->seek( mBufferPlayerNode->getNumFrames() * event.getX() / getWindowWidth() );
}

void BufferPlayerNodeApp::draw()
{
	gl::clear();
	gl::enableAlphaBlending();

	mWaveformPlot.draw();

	// draw the current play position
	float readPos = (float)getWindowWidth() * mBufferPlayerNode->getReadPosition() / mBufferPlayerNode->getNumFrames();
	gl::color( ColorA( 0, 1, 0, 0.7f ) );
	gl::drawSolidRoundedRect( Rectf( readPos - 2, 0, readPos + 2, (float)getWindowHeight() ), 2 );
}

CINDER_APP_NATIVE( BufferPlayerNodeApp, RendererGl )