#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Surface.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rand.h"
#include "cinder/qtime/QuickTimeGl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class QTimeAdvApp : public App {
  public:
	void prepareSettings( Settings *settings );
	void setup();

	void keyDown( KeyEvent event );
	void fileDrop( FileDropEvent event );

	void update();
	void draw();

	void addActiveMovie( qtime::MovieGlRef movie );
	void loadMovieUrl( const std::string &urlString );
	void loadMovieFile( const fs::path &path );


	fs::path mLastPath;
	// all of the actively playing movies
	vector<qtime::MovieGlRef> mMovies;
	// movies we're still waiting on to be loaded
	vector<qtime::MovieLoaderRef> mLoadingMovies;
};


void QTimeAdvApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 640, 480 );
	settings->setFullScreen( false );
	settings->setResizable( true );
}

void QTimeAdvApp::setup()
{
	srand( 133 );
	fs::path moviePath = getOpenFilePath();
	if( ! moviePath.empty() )
		loadMovieFile( moviePath );
}

void QTimeAdvApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' ) {
		setFullScreen( ! isFullScreen() );
	}
	else if( event.getChar() == 'o' ) {
		fs::path moviePath = getOpenFilePath();
		if( ! moviePath.empty() )
			loadMovieFile( moviePath );
	}
	else if( event.getChar() == 'O' ) {
		if( ! mLastPath.empty() )
			loadMovieFile( mLastPath );
	}
	else if( event.getChar() == 'x' ) {
		mMovies.clear();
		mLoadingMovies.clear();
	}
	else if( event.getChar() == '2' ) {
		if( ! mMovies.empty() )
			mMovies.back()->setRate( 2.0f );
	}
	else if( event.getChar() == 'd' ) {
		if( ! mMovies.empty() )
			mMovies.erase( mMovies.begin() + ( rand() % mMovies.size() ) );
	}
	else if( event.getChar() == 'u' ) {
		vector<string> randomMovie;
		randomMovie.push_back( "http://movies.apple.com/movies/us/hd_gallery/gl1800/480p/bbc_earth_m480p.mov" );
		randomMovie.push_back( "http://movies.apple.com/media/us/quicktime/guide/hd/480p/noisettes_m480p.mov" );
		randomMovie.push_back( "http://pdl.warnerbros.com/wbol/us/dd/med/northbynorthwest/quicktime_page/nbnf_airplane_explosion_qt_500.mov" );
		loadMovieUrl( randomMovie[rand() % randomMovie.size()] );
	}
}

void QTimeAdvApp::addActiveMovie( qtime::MovieGlRef movie )
{
	console() << "Dimensions:" << movie->getWidth() << " x " << movie->getHeight() << std::endl;
	console() << "Duration:  " << movie->getDuration() << " seconds" << std::endl;
	console() << "Frames:    " << movie->getNumFrames() << std::endl;
	console() << "Framerate: " << movie->getFramerate() << std::endl;
	movie->setLoop( true, false );
	
	mMovies.push_back( movie );
	movie->play();
}

void QTimeAdvApp::loadMovieUrl( const string &urlString )
{
	try {
		mLoadingMovies.push_back( qtime::MovieLoader::create( Url( urlString ) ) );
	}
	catch( ci::Exception &exc ) {
		console() << "Exception caught trying to load the movie from URL: " << urlString << ", what: " << exc.what() << std::endl;
	}
}

void QTimeAdvApp::loadMovieFile( const fs::path &moviePath )
{
	qtime::MovieGlRef movie;
	
	try {
		movie = qtime::MovieGl::create( moviePath );

		addActiveMovie( movie );
		mLastPath = moviePath;
	}
	catch( ci::Exception &exc ) {
		console() << "Exception caught trying to load the movie from path: " << moviePath << ", what: " << exc.what() << std::endl;
		return;
	}
}

void QTimeAdvApp::fileDrop( FileDropEvent event )
{
	for( size_t s = 0; s < event.getNumFiles(); ++s )
		loadMovieFile( event.getFile( s ) );
}

void QTimeAdvApp::update()
{
	// let's see if any of our loading movies have finished loading and can be made active
	for( auto loaderIt = mLoadingMovies.begin(); loaderIt != mLoadingMovies.end(); ) {
		try {
			if( (*loaderIt)->checkPlaythroughOk() ) {
				addActiveMovie( qtime::MovieGl::create( *loaderIt ) );
				loaderIt = mLoadingMovies.erase( loaderIt );
			}
			else
				++loaderIt;
		}
		catch( ci::Exception &exc ) {
			console() << "Exception caught trying to load the movie from URL: " << *loaderIt << ", what: " << exc.what() << std::endl;
			loaderIt = mLoadingMovies.erase( loaderIt );
		}
	}
}

void QTimeAdvApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );

	int totalWidth = 0;
	for( size_t m = 0; m < mMovies.size(); ++m )
		totalWidth += mMovies[m]->getWidth();

	int drawOffsetX = 0;
	for( size_t m = 0; m < mMovies.size(); ++m ) {
		float relativeWidth = mMovies[m]->getWidth() / (float)totalWidth;
		gl::TextureRef texture = mMovies[m]->getTexture();
		if( texture ) {
			float drawWidth = getWindowWidth() * relativeWidth;
			float drawHeight = ( getWindowWidth() * relativeWidth ) / mMovies[m]->getAspectRatio();
			float x = drawOffsetX;
			float y = ( getWindowHeight() - drawHeight ) / 2.0f;			

			gl::color( Color::white() );
			gl::draw( texture, Rectf( x, y, x + drawWidth, y + drawHeight ) );
		}
		drawOffsetX += getWindowWidth() * relativeWidth;
	}
}

CINDER_APP( QTimeAdvApp, RendererGl )