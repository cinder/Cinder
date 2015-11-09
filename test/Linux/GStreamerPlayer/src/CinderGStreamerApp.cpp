#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "MovieBase.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace gst::video;

class CinderGStreamerApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
    void keyDown(KeyEvent event) override;
	void update() override;
	void draw() override;
    std::vector<string> movies;
    MovieSurfaceRef movie;
};

void CinderGStreamerApp::setup()
{
    movie = MovieSurface::create( getAssetPath("bbb.mp4") );
    movie->play();

    movies.push_back("http://movies.apple.com/media/us/quicktime/guide/hd/480p/noisettes_m480p.mov");
    movies.push_back(getAssetPath("bbb.mp4").string());
}

void CinderGStreamerApp::mouseDown( MouseEvent event )
{
}

void CinderGStreamerApp::keyDown(KeyEvent event)
{
    if( movie ) {
        if( event.getChar() == '1' ) {
            movie->play();
        }
        else if( event.getChar() == '2' ) {
            movie->stop();
        }
        else if( event.getChar() == '3' ) {
            movie->seekToTime( movie->getDuration() / 2.0 );
        }
        else if( event.getChar() == '4' ) {
            movie->setRate( 1.0 );
        }
        else if( event.getChar() == '5' ) {
            movie->setRate( -1.0 );
        }
        else if( event.getChar() == '6' ) {
            movie->setLoop( true, false );
        }
        else if( event.getChar() == '7' ) {
            movie->setLoop( true, true );
        }
        else if( event.getChar() == '8' ) {
            movie->load(movies[rand()%movies.size()]);
            movie->play();
        }
        else if( event.getChar() == '9' ) {
            std::string caps = "video/x-raw, format={RGB}";
            GstCustomPipelineData pipelineData;
            pipelineData.pipeline = "uridecodebin uri=http://pdl.warnerbros.com/wbol/us/dd/med/northbynorthwest/quicktime_page/nbnf_airplane_explosion_qt_500.mov name=decode decode. ! audioconvert ! queue ! autoaudiosink decode. ! videoconvert ! queue ! appsink name=videosink caps=\""+caps+"\""; // For now if you use appsink it has to be named 'videosink'. This is still experimental and it will change.
            movie->setCustomPipeline( pipelineData );
            movie->play();
        }
    }
}

void CinderGStreamerApp::update()
{
}

void CinderGStreamerApp::draw()
{
    gl::clear( Color( 0, 0, 0 ) );
    if( movie ){
        ci::SurfaceRef videoSurface = movie->getSurface();
        if( videoSurface ) {
            Rectf centeredRect = Rectf( videoSurface->getBounds() ).getCenteredFit( getWindowBounds(), true );
            gl::draw( ci::gl::Texture::create( *videoSurface ), centeredRect );
        }
    }
}

CINDER_APP( CinderGStreamerApp, RendererGl )
