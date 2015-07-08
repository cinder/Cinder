#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/Utilities.h"
#include "cinder/gl/Shader.h"
#include "cinder/qtime/AvfWriter.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class MovieWriterApp : public App {
  public:
	static void prepareSettings( Settings *settings );

	void setup() override;
	void update() override;
	void draw() override;
	void cleanup() override { mMovieExporter.reset(); }
	
	qtime::MovieWriterRef mMovieExporter;
};

void MovieWriterApp::prepareSettings( App::Settings *settings )
{
	settings->setHighDensityDisplayEnabled( false );
}

void MovieWriterApp::setup()
{
#if defined( CINDER_COCOA_TOUCH )
	auto format = qtime::MovieWriter::Format().codec( qtime::MovieWriter::JPEG ).fileType( qtime::MovieWriter::QUICK_TIME_MOVIE ).
					jpegQuality( 0.09f ).averageBitsPerSecond( 10000000 );
	mMovieExporter = qtime::MovieWriter::create( getDocumentsDirectory() / "test.mov", getWindowWidth(), getWindowHeight(), format );
#else
	fs::path path = getSaveFilePath();
	if( ! path.empty() ) {
		auto format = qtime::MovieWriter::Format().codec( qtime::MovieWriter::H264 ).fileType( qtime::MovieWriter::QUICK_TIME_MOVIE )
						.jpegQuality( 0.09f ).averageBitsPerSecond( 10000000 );
		mMovieExporter = qtime::MovieWriter::create( path, getWindowWidth(), getWindowHeight(), format );
	}
#endif
	gl::bindStockShader( gl::ShaderDef().color() );
}

void MovieWriterApp::update()
{
	const int maxFrames = 100;
	if( mMovieExporter && getElapsedFrames() > 1 && getElapsedFrames() < maxFrames )
		mMovieExporter->addFrame( copyWindowSurface() );
	else if( mMovieExporter && getElapsedFrames() >= maxFrames ) {
		mMovieExporter->finish();
		mMovieExporter.reset();
	}
}

void MovieWriterApp::draw()
{
	gl::clear();
	gl::enableDepthRead();
	gl::color( Color( CM_HSV, fmod( getElapsedFrames() / 30.0f, 1.0f ), 1, 1 ) );
	gl::draw( geom::Circle().center( getWindowCenter() ).radius( getElapsedFrames() ).subdivisions( 100 ) );
}

CINDER_APP( MovieWriterApp, RendererGl, MovieWriterApp::prepareSettings )
