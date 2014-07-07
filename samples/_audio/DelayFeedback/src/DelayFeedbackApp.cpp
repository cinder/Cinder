/*
 * This audio sample firstly makes use of the audio::DelayNode, but also demonstrates some more complex methods of control,
 * like feedback and controlling an audio::Param with other audio::Node's.
 *
 * author: Richard Eakin (2014)
 */

#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/Perlin.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Timeline.h"

#include "cinder/audio/Context.h"
#include "cinder/audio/GenNode.h"
#include "cinder/audio/NodeEffects.h"
#include "cinder/audio/Utilities.h"
#include "cinder/audio/Debug.h"

#include "cinder/gl/Vbo.h"

#include "Resources.h"

const float MAX_VOLUME		= 0.6f;
const size_t MAX_SPLASHES	= 200;
const float MAX_RADIUS		= 300;
const float MAX_PITCH_MIDI	= 80;
const float	MIN_PITCH_MIDI	= 40;

using namespace ci;
using namespace ci::app;
using namespace std;

struct Splash {
	Vec2f		mCenter;
	Vec3f		mColorHsv;
	Anim<float>	mRadius, mAlpha;
};

class DelayFeedback : public AppNative {
public:
	void prepareSettings( Settings *settings );
	void setup();
	void mouseDrag( MouseEvent event ) override;
	void mouseUp( MouseEvent event ) override;
	void keyDown( KeyEvent event )	override;
	void update();
	void draw();

	void	setVariableDelayMod();
	void	addSplash( const Vec2f &pos );
	float	quantizePitch( const Vec2f &pos );
	void	loadMesh();
	void	loadGlsl();

	audio::GenOscNodeRef	mOsc;
	audio::DelayNodeRef		mDelay;
	audio::GainNodeRef		mGain;

	std::list<Splash>		mSplashes;
	Perlin					mPerlin;

	gl::GlslProgRef			mGlsl;
	gl::VboMeshRef			mMesh;
};

void DelayFeedback::prepareSettings( Settings *settings )
{
	settings->setWindowPos( 200, 200 );
	settings->setWindowSize( 1000, 800 );
}

void DelayFeedback::setup()
{
	loadGlsl();
	loadMesh();
	gl::enableAlphaBlending();

	// The basic audio::Node's used here are an oscillator with a triangle waveform, a gain, and a delay.
	// The complexity in the sound comes from how they are connected and controlled.
	
	auto ctx = audio::master();
	mOsc = ctx->makeNode( new audio::GenOscNode );
	mGain = ctx->makeNode( new audio::GainNode( 0 ) );
	mDelay = ctx->makeNode( new audio::DelayNode );

	mOsc->setWaveform( audio::WaveformType::TRIANGLE );

	// The Delay's length Param is itself controlled with Node's, which is configured next.
	setVariableDelayMod();

	// Now we connect up the Node's so that the signal immediately reaches the Context's OutputNode, but it also
	// feedback in a cycle to create an echo. To control the level of feedback and prevent ringing, a one-off GainNode
	// is used with a value of 0.5, which gives a fairly natural sounding decay.

	auto feedbackGain = audio::master()->makeNode( new audio::GainNode( 0.5f ) );
	feedbackGain->setName( "FeedbackGain" );

	mOsc >> mGain >> ctx->getOutput();
	mGain >> mDelay >> feedbackGain >> mDelay >> ctx->getOutput();

	mOsc->enable();
	ctx->enable();

	console() << "--------- context audio graph: --------------------" << endl;
	console() << ctx->printGraphToString();
	console() << "---------------------------------------------------" << endl;
}

// This method adds a low-frequency oscillator to the delay length, which makes a 'flanging' effect.
void DelayFeedback::setVariableDelayMod()
{
	mDelay->setMaxDelaySeconds( 2 );

	auto ctx = audio::master();

	auto osc = ctx->makeNode( new audio::GenSineNode( 0.00113f, audio::Node::Format().autoEnable() ) );
	auto mul = ctx->makeNode( new audio::GainNode( 0.3f ) );
	auto add = ctx->makeNode( new audio::AddNode( 0.343f ) );

	osc >> mul >> add;
	mDelay->getParamDelaySeconds()->setProcessor( add );
}

void DelayFeedback::addSplash( const Vec2f &pos )
{
	mSplashes.push_back( Splash() );

	auto &splash = mSplashes.back();
	splash.mCenter = pos;
	splash.mAlpha = 1;

	float radiusMin = ( 1 - (float)pos.y / (float)getWindowHeight() ) * MAX_RADIUS / 2;
	splash.mRadius = randFloat( radiusMin, 30 );

	float endRadius = randFloat( MAX_RADIUS * 0.9f, MAX_RADIUS );
	timeline().apply( &splash.mRadius, endRadius, 7, EaseOutExpo() );
	timeline().apply( &splash.mAlpha, 0.0f, 7 );

	float h = math<float>::min( 1,  mPerlin.fBm( pos.normalized() ) * 7.0f );
	splash.mColorHsv = Vec3f( fabsf( h ), 1, 1 );
}

// returns a quantized pitch (in hertz) within the lydian dominant scale
float DelayFeedback::quantizePitch( const Vec2f &pos )
{
	const size_t scaleLength = 7;
	float scale[scaleLength] = { 0, 2, 4, 6, 7, 9, 10 };

	int pitchMidi = lroundf( lmap( pos.x, 0.0f, (float)getWindowWidth(), MIN_PITCH_MIDI, MAX_PITCH_MIDI ) );

	bool quantized = false;
	while( ! quantized ) {
		int note = pitchMidi % 12;
		for( size_t i = 0; i < scaleLength; i++ ) {
			if( note == scale[i] ) {
				quantized = true;
				break;
			}
		}
		if( ! quantized )
			pitchMidi--;
	}

	return audio::midiToFreq( pitchMidi );
}

void DelayFeedback::mouseDrag( MouseEvent event )
{
	float freq = quantizePitch( event.getPos() );
	float gain = 1.0f - (float)event.getPos().y / (float)getWindowHeight();

	gain *= MAX_VOLUME;

	mOsc->getParamFreq()->applyRamp( freq, 0.04f );
	mGain->getParam()->applyRamp( gain, 0.1f );

	addSplash( event.getPos() );
}

void DelayFeedback::mouseUp( MouseEvent event )
{
	mGain->getParam()->applyRamp( 0, 1.5, audio::Param::Options().rampFn( &audio::rampOutQuad ) );
}

void DelayFeedback::keyDown( KeyEvent event )
{
	if( event.getChar() == 's' )
		loadGlsl();
	if( event.getChar() == 'f' )
		setFullScreen( ! isFullScreen() );
}

void DelayFeedback::update()
{
	// trim splashes
	if( mSplashes.size() > MAX_SPLASHES ) {
		size_t trimCount = mSplashes.size() - MAX_SPLASHES;
		for( size_t i = 0; i < trimCount; i++ )
			mSplashes.pop_front();
	}
}

void DelayFeedback::draw()
{
	gl::clear();

	if( ! mGlsl || ! mMesh )
		return;

	mGlsl->bind();

	for( const auto &splash : mSplashes ) {
		float radiusNormalized = splash.mRadius / MAX_RADIUS;
		mGlsl->uniform( "uRadius", radiusNormalized );

		Color splashColor( CM_HSV, splash.mColorHsv );
		gl::color( splashColor.r, splashColor.g, splashColor.b, splash.mAlpha() );

		gl::pushModelView();
			gl::translate( splash.mCenter );
			gl::draw( mMesh );
		gl::popModelView();
	}

	mGlsl->unbind();
}

void DelayFeedback::loadMesh()
{
	Rectf boundingBox( - MAX_RADIUS, - MAX_RADIUS, MAX_RADIUS, MAX_RADIUS );

	TriMesh2d mesh;

	mesh.appendVertex( boundingBox.getUpperLeft() );
	mesh.appendTexCoord( Vec2f( -1, -1 ) );

	mesh.appendVertex( boundingBox.getLowerLeft() );
	mesh.appendTexCoord( Vec2f( -1, 1 ) );

	mesh.appendVertex( boundingBox.getUpperRight() );
	mesh.appendTexCoord( Vec2f( 1, -1 ) );

	mesh.appendVertex( boundingBox.getLowerRight() );
	mesh.appendTexCoord( Vec2f( 1, 1 ) );

	mesh.appendTriangle( 0, 1, 2 );
	mesh.appendTriangle( 2, 1, 3 );

	mMesh = gl::VboMesh::create( mesh );
}

void DelayFeedback::loadGlsl()
{
	try {		
		mGlsl = gl::GlslProg::create( loadResource( SMOOTH_CIRCLE_GLSL_VERT ), loadResource( SMOOTH_CIRCLE_GLSL_FRAG ) );

		CI_LOG_V( "loaded glsl" );
	}
	catch( std::exception &exc ) {
		CI_LOG_E( "failed to load shader, what: " << exc.what() );
	}
}

CINDER_APP_NATIVE( DelayFeedback, RendererGl )
