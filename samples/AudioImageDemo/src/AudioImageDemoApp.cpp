#include "cinder/app/AppBasic.h"
#include "cinder/audio/Io.h"
#include "cinder/audio/Output.h"
#include "cinder/audio/FftProcessor.h"
#include "cinder/audio/PcmBuffer.h"

#include "cinder/app/AppBasic.h"
#include "cinder/ArcBall.h"
#include "cinder/Rand.h"
#include "cinder/Camera.h"
#include "cinder/Surface.h"
#include "cinder/ip/Resize.h"
#include "cinder/gl/Vbo.h"
#include "cinder/ImageIo.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

static const int32_t MAX_IMAGE_SIZE = 500;

class AudioImageDemoApp : public AppBasic {
public:
	void setup();
	void resize( ResizeEvent event );
	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void keyDown( KeyEvent event );
	void draw();
	void update();
	void openImageFile();
	void openSoundFile();

private:
	enum ColorSwitch {
		kColor, kRed, kGreen, kBlue
	};
	void updateData( ColorSwitch whichColor, audio::PcmBuffer32fRef pcmBuffer = audio::PcmBuffer32fRef() );

	void drawWaveForm();
	void drawFft();

	CameraPersp mCam;
	Arcball     mArcball;

	uint32_t    mWidth, mHeight;

	Surface32f		mImage;
	gl::VboMesh		mVboMesh;
	ColorSwitch   mVboColor;

	///////////////////////////////////////////////////////////////////////////

	audio::TrackRef mTrack;
	audio::PcmBuffer32fRef mPcmBuffer;
};

void AudioImageDemoApp::setup()
{
	gl::enableAlphaBlending();
	gl::enableDepthRead();
	gl::enableDepthWrite();    

	// initialize the arcball with a semi-arbitrary rotation just to give an interesting angle
	mArcball.setQuat( Quatf( Vec3f( 0.0577576f, -0.956794f, 0.284971f ), 3.68f ) );

	openImageFile();
	openSoundFile();
}

void AudioImageDemoApp::openImageFile()
{
	fs::path path = getOpenFilePath( "", ImageIo::getLoadExtensions() );
	if( ! path.empty() ) {
		try {
			Surface32f image = loadImage( path );

			auto source = image.getBounds();
			Area dest(0, 0, MAX_IMAGE_SIZE, MAX_IMAGE_SIZE);
			auto fit = Area::proportionalFit(source, dest, false, false);

			//if (source.getSize() != fit.getSize())
			mImage = ip::resizeCopy( image, source, fit.getSize() );

			mWidth = mImage.getWidth();
			mHeight = mImage.getHeight();

			gl::VboMesh::Layout layout;
			layout.setDynamicColorsRGB();
			layout.setDynamicPositions();
			mVboMesh = gl::VboMesh( mWidth * mHeight, 0, layout, GL_POINTS );

			mVboColor = kColor;
		}
		catch( ... ) {
			std::cout << "Invalid image file.";
		}
	}
}


void AudioImageDemoApp::openSoundFile()
{
	fs::path path = getOpenFilePath( "" );
	if( ! path.empty() ) {

		try {
			auto newTrack = audio::Output::addTrack( audio::load( path.string() ), false );
			if (mTrack)
			{
				mTrack->stop();
				//audio::Output::removeTrack( 0 );
			}
			mTrack = newTrack;
			mTrack->enablePcmBuffering( true );
			mTrack->play();
		}
		catch( ... ) {
			std::cout << "Invalid audio file.";
		}
	}
}

void AudioImageDemoApp::resize( ResizeEvent event )
{
	mArcball.setWindowSize( getWindowSize() );
	mArcball.setCenter( Vec2f( getWindowWidth() / 2.0f, getWindowHeight() / 2.0f ) );
	mArcball.setRadius( getWindowHeight() / 2.0f );

	mCam.lookAt( Vec3f( 0.0f, 0.0f, -150 ), Vec3f::zero() );
	mCam.setPerspective( 60.0f, getWindowAspectRatio(), 0.1f, 1000.0f );
	gl::setMatrices( mCam );
}

void AudioImageDemoApp::mouseDown( MouseEvent event )
{
	mArcball.mouseDown( event.getPos() );
}

void AudioImageDemoApp::mouseDrag( MouseEvent event )
{
	mArcball.mouseDrag( event.getPos() );
}

void AudioImageDemoApp::keyDown( KeyEvent event )
{
	switch( event.getChar() ) {
	case 'r':
		updateData( kRed );
		break;
	case 'g':
		updateData( kGreen );
		break;
	case 'b':
		updateData( kBlue );
		break;
	case 'c':
		updateData( kColor );
		break;
	case 'o':
		openImageFile();
		break;
	case 's':
		openSoundFile();
		break;
	case 'p':
		if (mTrack)
			( mTrack->isPlaying() ) ? mTrack->stop() : mTrack->play();
	}
}

void AudioImageDemoApp::draw()
{
	glClearColor( 0.0f, 0.0f, 0.0f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	gl::pushModelView();
	gl::translate( Vec3f( 0.0f, 0.0f, mHeight / 2.0f ) );
	gl::rotate( mArcball.getQuat() );
	if( mVboMesh )
		gl::draw( mVboMesh );
	gl::popModelView();

#if 0
	gl::clear( Color( 0.0f, 0.0f, 0.0f ) );

	glPushMatrix();
	glTranslatef( 0.0, 0.0, 0.0 );
	drawWaveForm();
#if defined( CINDER_MAC )
	glTranslatef( 0.0, 200.0, 0.0 );
	drawFft();
#endif
	glPopMatrix();
#endif
}

void AudioImageDemoApp::update()
{
	updateData( mVboColor, mTrack ? mTrack->getPcmBuffer() : audio::PcmBuffer32fRef() );
}

void AudioImageDemoApp::drawWaveForm()
{	
	//if the buffer is null, for example if this gets called before any PCM data has been buffered
	//don't do anything
	if( ! mPcmBuffer ) {
		return;
	}

	uint32_t bufferLength = mPcmBuffer->getSampleCount();
	audio::Buffer32fRef leftBuffer = mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT );
	audio::Buffer32fRef rightBuffer = mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_RIGHT );

	int displaySize = getWindowWidth();
	float scale = displaySize / (float)bufferLength;

	PolyLine<Vec2f>	leftBufferLine;
	PolyLine<Vec2f>	rightBufferLine;

	for( int i = 0; i < bufferLength; i++ ) {
		float x = ( i * scale );

		//get the PCM value from the left channel buffer
		float y = ( ( leftBuffer->mData[i] - 1 ) * - 100 );
		leftBufferLine.push_back( Vec2f( x , y) );

		y = ( ( rightBuffer->mData[i] - 1 ) * - 100 );
		rightBufferLine.push_back( Vec2f( x , y) );
	}
	gl::color( Color( 1.0f, 0.5f, 0.25f ) );
	gl::draw( leftBufferLine );
	gl::draw( rightBufferLine );

}

#if defined(CINDER_MAC)
void AudioImageDemoApp::drawFft()
{
	float ht = 100.0f;
	uint16_t bandCount = 32;

	if( ! mPcmBuffer ) return;

	//use the most recent Pcm data to calculate the Fft
	std::shared_ptr<float> fftRef = audio::calculateFft( mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT ), bandCount );
	if( ! fftRef ) {
		return;
	}

	float * fftBuffer = fftRef.get();

	//draw the bands
	for( int i = 0; i < ( bandCount ); i++ ) {
		float barY = fftBuffer[i] / bandCount * ht;
		glBegin( GL_QUADS );
		glColor3f( 255.0f, 255.0f, 0.0f );
		glVertex2f( i * 3, ht );
		glVertex2f( i * 3 + 1, ht );
		glColor3f( 0.0f, 255.0f, 0.0f );
		glVertex2f( i * 3 + 1, ht - barY );
		glVertex2f( i * 3, ht - barY );
		glEnd();
	}
}
#endif

void AudioImageDemoApp::updateData( AudioImageDemoApp::ColorSwitch whichColor, audio::PcmBuffer32fRef pcmBuffer )
{
	if (!mImage) return;

	mVboColor = whichColor;
	mPcmBuffer = pcmBuffer;

	Surface32f::Iter pixelIter = mImage.getIter();
	gl::VboMesh::VertexIter vertexIter( mVboMesh );

	const uint32_t bufferLength           = mPcmBuffer ? mPcmBuffer->getSampleCount() : 0;
	const audio::Buffer32fRef leftBuffer  = mPcmBuffer ? mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_LEFT ) : audio::Buffer32fRef();
	const audio::Buffer32fRef rightBuffer = mPcmBuffer ? mPcmBuffer->getChannelData( audio::CHANNEL_FRONT_RIGHT ) : audio::Buffer32fRef();

	const float iRadius2 = Vec2f(mImage.getWidth() / 2, mImage.getHeight() / 2).lengthSquared();
	const float scale = (float)bufferLength / iRadius2;

	while( pixelIter.line() ) {
		while( pixelIter.pixel() ) {
			Color color( pixelIter.r(), pixelIter.g(), pixelIter.b() );
			float height;
			const float muteColor = 0.2f;

			// calculate the height based on a weighted average of the RGB, and emphasize either the red green or blue color in each of those modes
			switch( whichColor ) {
			case kColor:
				height = color.dot( Color( 0.3333f, 0.3333f, 0.3333f ) );
				break;
			case kRed:
				height = color.dot( Color( 1, 0, 0 ) );
				color *= Color( 1, muteColor, muteColor );
				break;
			case kGreen:
				height = color.dot( Color( 0, 1, 0 ) );
				color *= Color( muteColor, 1, muteColor );
				break;
			case kBlue:
				height = color.dot( Color( 0, 0, 1 ) );
				color *= Color( muteColor, muteColor, 1 );
				break;
			}

			// the x and the z coordinates correspond to the pixel's x & y
			float x = pixelIter.x() - mWidth / 2.0f;
			float y = height * 60.0f;
			float z = pixelIter.y() - mHeight / 2.0f;

			if ( mPcmBuffer )
			{
				int32_t i = (int32_t)(Vec2f(x,z).lengthSquared() * scale);
				float avgPcm = ( ( (leftBuffer->mData[i] + rightBuffer->mData[i]) * .5f - 1 ) * - 100 ) * .01;
				y *= avgPcm;
			}

			vertexIter.setPosition( x, y, z );
			vertexIter.setColorRGB( color );
			++vertexIter;
		}
	}
}

CINDER_APP_BASIC( AudioImageDemoApp, RendererGl )
