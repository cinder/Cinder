#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/Shader.h"

#include "cinder/qtime/AvfWriter.h"

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
	
	float mAngle;
	qtime::MovieWriterRef mMovieExporter;
};

void MovieWriterApp::setup()
{
	mAngle = 0;
	
	fs::path path = getSaveFilePath();
	if( ! path.empty() ) {
//		path += "Movie.mov";
		mMovieExporter = qtime::MovieWriter::create( path, getWindowWidth(), getWindowHeight(), qtime::MovieWriter::Format().codec( qtime::MovieWriter::JPEG ).fileType( qtime::MovieWriter::M4V ) );
	}
	
	gl::bindStockShader( gl::ShaderDef().color() );
}

void MovieWriterApp::mouseDown( MouseEvent event )
{
}

void MovieWriterApp::update()
{
	if( mMovieExporter )
		mMovieExporter->addFrame( copyWindowSurface() );
}

void MovieWriterApp::draw()
{
	gl::clear();
	gl::enableDepthRead();
	gl::color( Color( CM_HSV, fmod( getElapsedSeconds() / 10, 1.0f ), 1, 1 ) );
	gl::draw( geom::Circle().center( getWindowCenter() ).radius( getElapsedSeconds() * 4 ) );
}

CINDER_APP( MovieWriterApp, RendererGl )
