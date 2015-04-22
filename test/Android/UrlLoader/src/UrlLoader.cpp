#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/Surface.h"
#include "cinder/Thread.h"
#include <list>
using namespace ci;
using namespace ci::app;
using namespace std;

#include <fstream>
#include <sstream>

// We'll create a new Cinder Application by deriving from the App class
class UrlLoader : public App {
  public:
    void setup();
	void mouseDrag( MouseEvent event );
#if defined( CINDER_ANDROID )
    void touchesBegan( TouchEvent event );
#endif
	void keyDown( KeyEvent event );
	void draw();

	// This will maintain a list of points which we will draw line segments between
	list<vec2>		mPoints;
	gl::TextureRef  mTex;
};

void writeToFile( const std::string& filePath, const uint8_t* dataBytes, uint32_t dataLength )
{
    std::ofstream os( filePath.c_str(), std::ios::binary );
    os.write( (const char*)dataBytes, dataLength );
    os.close();
    console() << "Wrote: " << filePath << std::endl;
}

class DataReport {
public:
    std::string     url;
    std::string     summary;

    DataReport( const std::string& aUrl ) : url( aUrl ) {}

    void load() {
        auto ds = loadUrl( url );

        if( ds && ds->getBuffer() ) {
            auto buf = ds->getBuffer();

            std::stringstream ss;
            ss << buf.getDataSize() << " bytes";
            summary = ss.str();
        }
        else {
            summary = "result is null!";
        }
    }
};

void UrlLoader::setup()
{
    std::vector<std::string> urls;
    urls.push_back( "http://upload.wikimedia.org/wikipedia/commons/d/d4/New_York_City_at_night_HDR_edit1.jpg" );
    urls.push_back( "http://www.google.com" );
    urls.push_back( "http://www.amazon.com" );
    urls.push_back( "http://www.twitter.com" );
    urls.push_back( "http://www.facebook.com" );
    urls.push_back( "http://www.cnet.com" );
    urls.push_back( "http://www.freshnews.org" );
    urls.push_back( "http://www.theverge.com/" );
    urls.push_back( "http://www.gizmodo.com" );
    urls.push_back( "http://www.engadget.com" );
    urls.push_back( "http://www.slashdot.org" );
    urls.push_back( "http://www.pinboard.in" );
    urls.push_back( "http://www.osnews.com" );

    std::vector<std::shared_ptr<DataReport>> reports;
    for( auto& url : urls ) {
        reports.push_back( std::make_shared<DataReport>( url ) );
    }

    std::vector<std::shared_ptr<std::thread>> threads;
    for( auto& report : reports  ) {
        std::shared_ptr<std::thread> t( new std::thread( &DataReport::load, report ) );
        threads.push_back( t );
    }

    for( auto& t : threads ) {
        t->join();
    }

    for( auto& report : reports  ) {
        console() << "Loaded: " + report->url << std::endl;
        console() << "   " << report->summary << std::endl;
    }

/*
    auto ds = loadUrl( "http://www.google.com" );
    if( ds && ds->getBuffer() ) {
        auto buf = ds->getBuffer();
        console() << "Loaded " << buf.getDataSize() << " bytes" << std::endl;
    }
*/

/*
    try {
        mTex = gl::Texture::create( loadImage( loadUrl( "http://upload.wikimedia.org/wikipedia/commons/d/d4/New_York_City_at_night_HDR_edit1.jpg" ) ) );

        //ci::android::UrlLoader loader( "http://upload.wikimedia.org/wikipedia/commons/d/d4/New_York_City_at_night_HDR_edit1.jpg" );
        //ci::Buffer buf = loader.getData();
        //if( buf ) {
        //     writeToFile( "/storage/emulated/0/my_data.jpg", (const uint8_t*)buf.getData(), buf.getDataSize() );
        //}
    }
    catch( const std::exception& e ) {
        console() << "Failed getting data: " << e.what() << std::endl;
    }
*/
}

void UrlLoader::mouseDrag( MouseEvent event )
{
	mPoints.push_back( event.getPos() );
}

#if defined( CINDER_ANDROID )
void UrlLoader::touchesBegan( TouchEvent event )
{
	for( vector<TouchEvent::Touch>::const_iterator touchIt = event.getTouches().begin(); touchIt != event.getTouches().end(); ++touchIt ) {
        mPoints.push_back( touchIt->getPos() );
	}
}
#endif

void UrlLoader::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' )
		setFullScreen( ! isFullScreen() );
}

void UrlLoader::draw()
{
	gl::clear( Color( 0.1f, 0.1f, 0.15f ) );

    if( mTex ) {
 	    gl::color( 1.0f, 1.0f, 1.0f );
        gl::draw( mTex );
    }

	gl::color( 1.0f, 0.5f, 0.25f );	
	gl::begin( GL_LINE_STRIP );
	for( auto pointIter = mPoints.begin(); pointIter != mPoints.end(); ++pointIter ) {
		gl::vertex( *pointIter );
	}
	gl::end();
}

// This line tells Cinder to actually create the application
CINDER_APP( UrlLoader, RendererGl )
