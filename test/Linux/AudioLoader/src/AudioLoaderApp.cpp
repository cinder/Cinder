#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/app/RendererGl.h"

#include "cinder/audio/audio.h"

#include "../../../../samples/_audio/common/AudioDrawUtils.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class AudioLoaderApp : public App {
public:
	void setup() override;
	void resize() override;
	void fileDrop( FileDropEvent event ) override;
	void keyDown( KeyEvent event ) override;
	void mouseDown( MouseEvent event ) override;
	void mouseDrag( MouseEvent event ) override;
	void draw() override;

	audio::GainNodeRef			mGain;
	audio::BufferPlayerNodeRef	mBufferPlayerNode;

	WaveformPlot				mWaveformPlot;
};

void AudioLoaderApp::setup()
{
	auto ctx = audio::Context::master();

	// create a SourceFile and set its output samplerate to match the Context.
	//audio::SourceFileRef sourceFile = audio::load( loadAsset( "Sirius.mp3" ), ctx->getSampleRate() );
	audio::SourceFileRef sourceFile = audio::load( loadAsset( "Drumstylin.wav" ), ctx->getSampleRate() );

	// load the entire sound file into a BufferRef, and construct a BufferPlayerNode with this.
	audio::BufferRef buffer = sourceFile->loadBuffer();
	mBufferPlayerNode = ctx->makeNode( new audio::BufferPlayerNode( buffer ) );

	// add a Gain to reduce the volume
	mGain = ctx->makeNode( new audio::GainNode( 0.5f ) );

	// connect and enable the Context
	mBufferPlayerNode >> mGain >> ctx->getOutput();
	ctx->enable();

	mBufferPlayerNode->start();
}

void AudioLoaderApp::resize()
{
	// visualize the audio buffer
	if( mBufferPlayerNode ) {
		//auto buf = mBufferPlayerNode->getBuffer();
		mWaveformPlot.load( mBufferPlayerNode->getBuffer(), getWindowBounds() );
	}	
}

void AudioLoaderApp::fileDrop( FileDropEvent event )
{
	fs::path filePath = event.getFile( 0 );
	getWindow()->setTitle( filePath.filename().string() );

	audio::SourceFileRef sourceFile = audio::load( loadFile( filePath ) );

	// BufferPlayerNode can also load a buffer directly from the SourceFile.
	// This is safe to call on a background thread, which would alleviate blocking the UI loop.
	mBufferPlayerNode->loadBuffer( sourceFile );

	mWaveformPlot.load( mBufferPlayerNode->getBuffer(), getWindowBounds() );
}

void AudioLoaderApp::keyDown( KeyEvent event )
{
	if( event.getCode() == KeyEvent::KEY_SPACE ) {
		if( mBufferPlayerNode->isEnabled() )
			mBufferPlayerNode->stop();
		else
			mBufferPlayerNode->start();
	}
	else if( event.getCode() == KeyEvent::KEY_1 ) {
		mBufferPlayerNode->stop();
		auto ctx = audio::Context::master();

		std::string fileName = "Drumstylin.wav";
		std::cout << "Loading " << fileName << " ..." << std::endl;
		audio::SourceFileRef sourceFile = audio::load( loadAsset( fileName ) );
		mBufferPlayerNode->loadBuffer( sourceFile );
		mWaveformPlot.load( mBufferPlayerNode->getBuffer(), getWindowBounds() );
		mBufferPlayerNode->start();
		std::cout << "Playing" << fileName << std::endl;
	}
	else if( event.getCode() == KeyEvent::KEY_2 ) {
		mBufferPlayerNode->stop();
		auto ctx = audio::Context::master();

		std::string fileName = "Sirius.mp3";
		std::cout << "Loading " << fileName << " ..." << std::endl;
		audio::SourceFileRef sourceFile = audio::load( loadAsset( fileName ) );
		mBufferPlayerNode->loadBuffer( sourceFile );
		mWaveformPlot.load( mBufferPlayerNode->getBuffer(), getWindowBounds() );
		mBufferPlayerNode->start();
		std::cout << "Playing " << fileName << std::endl;
	}
}

void AudioLoaderApp::mouseDown( MouseEvent event )
{
	mBufferPlayerNode->start();
}

void AudioLoaderApp::mouseDrag( MouseEvent event )
{
	if( mBufferPlayerNode->isEnabled() )
		mBufferPlayerNode->seek( mBufferPlayerNode->getNumFrames() * event.getX() / getWindowWidth() );
}

void AudioLoaderApp::draw()
{
	gl::clear();
	gl::enableAlphaBlending();

	mWaveformPlot.draw();

	// draw the current play position
	float readPos = (float)getWindowWidth() * mBufferPlayerNode->getReadPosition() / mBufferPlayerNode->getNumFrames();
	gl::color( ColorA( 0, 1, 0, 0.7f ) );
	gl::drawSolidRect( Rectf( readPos - 2, 0, readPos + 2, (float)getWindowHeight() ) );
}

CINDER_APP( AudioLoaderApp, RendererGl( RendererGl::Options().msaa( 8 ) ), []( App::Settings *settings ) {
	settings->setMultiTouchEnabled( false );
} )
