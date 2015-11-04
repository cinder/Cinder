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
    std::vector<string> movieStreams;
    MovieGlRef movie;
};

void CinderGStreamerApp::setup()
{
    movie = MovieGl::create( "http://pdl.warnerbros.com/wbol/us/dd/med/northbynorthwest/quicktime_page/nbnf_airplane_explosion_qt_500.mov" );
    movie->play();
    movie->setVolume(0);
    movieStreams.push_back(getAssetPath("bbb.mp4").string());
    movieStreams.push_back("http://pdl.warnerbros.com/wbol/us/dd/med/northbynorthwest/quicktime_page/nbnf_airplane_explosion_qt_500.mov");
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
        else if( event.getChar() == '5' ) {
            movie->setRate( 1.0 );
        }
        else if( event.getChar() == '6' ) {
            movie->setRate( -1.0 );
        }
        else if( event.getChar() == '7' ) {
            movie->setLoop( true, false );
        }
        else if( event.getChar() == '8' ) {
            movie->setLoop( true, true );
        }
        else if( event.getChar() == '9' ) {
            movie->load(movieStreams[rand()%movieStreams.size()]);
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
        ci::gl::Texture2dRef tex = movie->getTexture();
        if( tex ) {
           gl::draw( tex );
        }
    }
}

CINDER_APP( CinderGStreamerApp, RendererGl )
