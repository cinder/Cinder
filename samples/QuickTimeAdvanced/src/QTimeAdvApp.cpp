#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include "cinder/app/AppBasic.h"
#include "cinder/Surface.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rand.h"
#include "cinder/qtime/QuickTime.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class QTimeAdvApp : public AppBasic {
  public:
	void prepareSettings( Settings *settings );
	void setup();

	void keyDown( KeyEvent event );
	void fileDrop( FileDropEvent event );

	void update();
	void draw();
	void drawFFT( const qtime::MovieBase &movie, float x, float y, float width, float height );

	void addActiveMovie( qtime::MovieGl movie );
	void loadMovieUrl( const std::string &urlString );
	void loadMovieFile( const std::string &path );


	std::string mLastPath;
	// all of the actively playing movies
	vector<qtime::MovieGl> mMovies;
	// movies we're still waiting on to be loaded
	vector<qtime::MovieLoader> mLoadingMovies;
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
	string moviePath = getOpenFilePath();
	if( ! moviePath.empty() )
		loadMovieFile( moviePath );
}

void QTimeAdvApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' ) {
		setFullScreen( ! isFullScreen() );
	}
	else if( event.getChar() == 'o' ) {
		string moviePath = getOpenFilePath();
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

void QTimeAdvApp::addActiveMovie( qtime::MovieGl movie )
{
	console() << "Dimensions:" << movie.getWidth() << " x " << movie.getHeight() << std::endl;
	console() << "Duration:  " << movie.getDuration() << " seconds" << std::endl;
	console() << "Frames:    " << movie.getNumFrames() << std::endl;
	console() << "Framerate: " << movie.getFramerate() << std::endl;
	movie.setLoop( true, false );
	
	mMovies.push_back( movie );
	movie.play();
}

void QTimeAdvApp::loadMovieUrl( const string &urlString )
{
	try {
		mLoadingMovies.push_back( qtime::MovieLoader( Url( urlString ) ) );
	}
	catch( ... ) {
		console() << "Unable to load the movie from URL: " << urlString << std::endl;
	}
}

void QTimeAdvApp::loadMovieFile( const string &moviePath )
{
	qtime::MovieGl movie;
	
	try {
		movie = qtime::MovieGl( moviePath );

		addActiveMovie( movie );
		mLastPath = moviePath;
	}
	catch( ... ) {
		console() << "Unable to load the movie." << std::endl;
		return;
	}
	
	try {
		movie.setupMonoFft( 8 );
	}
	catch( qtime::QuickTimeExcFft & ) {
		console() << "Unable to setup FFT" << std::endl;
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
	for( vector<qtime::MovieLoader>::iterator loaderIt = mLoadingMovies.begin(); loaderIt != mLoadingMovies.end(); ) {
		try {
			if( loaderIt->checkPlaythroughOk() ) {
				addActiveMovie( *loaderIt );
				loaderIt = mLoadingMovies.erase( loaderIt );
			}
			else
				++loaderIt;
		}
		catch( ... ) {
			console() << "There was an error loading a movie." << std::endl;
			loaderIt = mLoadingMovies.erase( loaderIt );
		}
	}
}

void QTimeAdvApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );

	int totalWidth = 0;
	for( size_t m = 0; m < mMovies.size(); ++m )
		totalWidth += mMovies[m].getWidth();

	int drawOffsetX = 0;
	for( size_t m = 0; m < mMovies.size(); ++m ) {
		float relativeWidth = mMovies[m].getWidth() / (float)totalWidth;
		gl::Texture texture = mMovies[m].getTexture();
		if( texture ) {
			float drawWidth = getWindowWidth() * relativeWidth;
			float drawHeight = ( getWindowWidth() * relativeWidth ) / mMovies[m].getAspectRatio();
			float x = drawOffsetX;
			float y = ( getWindowHeight() - drawHeight ) / 2.0f;			

			gl::color( Color::white() );
			gl::draw( texture, Rectf( x, y, x + drawWidth, y + drawHeight ) );
			texture.disable();

			drawFFT( mMovies[m], x, y, drawWidth, drawHeight );
		}
		drawOffsetX += getWindowWidth() * relativeWidth;
	}
}

void QTimeAdvApp::drawFFT( const qtime::MovieBase &movie, float x, float y, float width, float height )
{
	if( ! movie.getNumFftChannels() )
		return;
	
	float bandWidth = width / movie.getNumFftBands();
	float *fftData = movie.getFftData();
	for( uint32_t band = 0; band < movie.getNumFftBands(); ++band ) {
		float bandHeight = height / 3.0f * fftData[band];
		gl::color( Color( 0.1f, 0.8f, 0.1f ) );
		gl::drawSolidRect( ci::Rectf( x + band * bandWidth, y + height - bandHeight, x + band * bandWidth + bandWidth, y + height ) );
	}
}

CINDER_APP_BASIC( QTimeAdvApp, RendererGl )