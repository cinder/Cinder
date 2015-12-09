#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
//#include "MovieBase.h"
#include "cinder/linux/Movie.h"

#include <iomanip>

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace gst::video;

class CinderGStreamerApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
    void keyDown( KeyEvent event ) override;
	void update() override;
	void draw() override;

  private:
    std::vector<string> 	mMovies;
    ci::linux::MovieGlRef 	mMovie;
	ci::Font				mFont;
};

void CinderGStreamerApp::setup()
{
	mFont = ci::Font( "Droid Sans", 30.0f );

    mMovie = ci::linux::MovieGl::create( "http://pdl.warnerbros.com/wbol/us/dd/med/northbynorthwest/quicktime_page/nbnf_airplane_explosion_qt_500.mov" );
    mMovie->play();
    mMovie->setLoop( true);
}

void CinderGStreamerApp::mouseDown( MouseEvent event )
{
}

void CinderGStreamerApp::keyDown(KeyEvent event)
{
    if( mMovie ) {
        if( event.getChar() == '1' ) {
            mMovie->play();
        }
        else if( event.getChar() == '2' ) {
            mMovie->stop();
        }
        else if( event.getChar() == '3' ) {
            mMovie->seekToTime( mMovie->getDuration() / 2.0 );
        }
        else if( event.getChar() == '4' ) {
            mMovie->setRate( 1.0 );
        }
        else if( event.getChar() == '5' ) {
            mMovie->setRate( -1.0 );
        }
        else if( event.getChar() == '6' ) {
            mMovie->setLoop( true, false );
        }
        else if( event.getChar() == '7' ) {
            mMovie = ci::linux::MovieGl::create( "http://pdl.warnerbros.com/wbol/us/dd/med/northbynorthwest/quicktime_page/nbnf_airplane_explosion_qt_500.mov" );
	        mMovie->play();
        }
        else if( event.getChar() == '8' ) {
            mMovie = ci::linux::MovieGl::create( getAssetPath("bbb.mp4") );
	        mMovie->play();
    	    //std::string capsGL = "video/x-raw(memory:GLMemory), format=RGBA";
            //GstCustomPipelineData pipelineData;
    	    //pipelineData.pipeline = "uridecodebin uri=http://movies.apple.com/media/us/quicktime/guide/hd/480p/noisettes_m480p.mov ! glupload ! glcolorconvert ! appsink name=videosink caps=\""+capsGL+"\""; // For now if you use appsink it has to be named 'videosink'. This is still experimental and it will change.
            //movie = MovieGl::create( pipelineData );
            //mMovie->play();
        }
        else if( event.getChar() == '9' ) {
            //pipelineData.pipeline = "uridecodebin uri=http://pdl.warnerbros.com/wbol/us/dd/med/northbynorthwest/quicktime_page/nbnf_airplane_explosion_qt_500.mov name=decode decode. ! audioconvert ! queue ! autoaudiosink decode. ! videoconvert ! queue ! appsink name=videosink caps=\""+caps+"\""; // For now if you use appsink it has to be named 'videosink'. This is still experimental and it will change.
            mMovie = ci::linux::MovieGl::create( "http://movies.apple.com/media/us/quicktime/guide/hd/480p/noisettes_m480p.mov" );
            mMovie->play();
        }
    }
}

void CinderGStreamerApp::update()
{
}

void CinderGStreamerApp::draw()
{
    gl::clear( Color( 0, 0, 0 ) );
    if( mMovie ){
        ci::gl::Texture2dRef videoSurface = mMovie->getTexture();
        if( videoSurface ) {
            Rectf centeredRect = Rectf( videoSurface->getBounds() ).getCenteredFit( getWindowBounds(), true );
            gl::draw( videoSurface, centeredRect );
        }
    }

	gl::color( Color( 1, 1, 1 ) );
	std::stringstream ss;
	ss << "FPS: " << std::setprecision( 2 ) << getAverageFps();
	gl::drawString( ss.str(), vec2( 5, 30 ), Color( 1, 1, 1 ), mFont ); 
}

CINDER_APP( CinderGStreamerApp, RendererGl )
