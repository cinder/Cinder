#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Utilities.h"
#include "cinder/gl/Shader.h"

#include "cinder/qtime/AvfWriter.h"

#if defined( CINDER_COCOA_TOUCH )
#import <UIKit/UIKit.h>
#endif

using namespace ci;
using namespace ci::app;
using namespace std;

class MovieWriterApp : public App {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
	void cleanup() override { mMovieExporter.reset(); }
	
	qtime::MovieWriterRef mMovieExporter;
};

void prepareSettings( App::Settings *settings )
{
	settings->setHighDensityDisplayEnabled( false );
}

void MovieWriterApp::setup()
{
#if defined( CINDER_COCOA_TOUCH )
	auto format = qtime::MovieWriter::Format().codec( qtime::MovieWriter::H264 ).fileType( qtime::MovieWriter::QUICK_TIME_MOVIE )
					.enableFrameReordering( false ).jpegQuality( 0.09f ).enableFrameReordering( false ).averageBitsPerSecond( 10000000 );
	mMovieExporter = qtime::MovieWriter::create( getDocumentsDirectory() / "test.mov", getWindowWidth(), getWindowHeight(), format );
#else
	fs::path path = getSaveFilePath();
	if( ! path.empty() ) {
		auto format = qtime::MovieWriter::Format().codec( qtime::MovieWriter::H264 ).fileType( qtime::MovieWriter::QUICK_TIME_MOVIE )
						.enableFrameReordering( false ).jpegQuality( 0.09f ).enableFrameReordering( false ).averageBitsPerSecond( 10000000 );
		mMovieExporter = qtime::MovieWriter::create( path, getWindowWidth(), getWindowHeight(), format );
	}
#endif
	gl::bindStockShader( gl::ShaderDef().color() );
}

void MovieWriterApp::mouseDown( MouseEvent event )
{
}

void MovieWriterApp::update()
{
	const int maxFrames = 100;
	if( mMovieExporter && getElapsedFrames() > 1 && getElapsedFrames() < maxFrames )
		mMovieExporter->addFrame( copyWindowSurface() );
	else if( mMovieExporter && getElapsedFrames() >= maxFrames ) {
		mMovieExporter->finish();
#if defined( CINDER_COCOA_TOUCH )
		fs::path path = getDocumentsDirectory() / "test.mov";
		::UISaveVideoAtPathToSavedPhotosAlbum( [NSString stringWithUTF8String:path.c_str()], nil, nullptr, nullptr );
#endif
	}
}

void MovieWriterApp::draw()
{
	gl::clear();
	gl::enableDepthRead();
	gl::color( Color( CM_HSV, fmod( getElapsedFrames() / 30.0f, 1.0f ), 1, 1 ) );
	gl::draw( geom::Circle().center( getWindowCenter() ).radius( getElapsedFrames() ) );
}

CINDER_APP( MovieWriterApp, RendererGl, prepareSettings )
