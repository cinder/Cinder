#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/app/linux/PlatformLinux.h"

using namespace ci;
using namespace ci::app;

class PlatformTestApp : public App {
  public:
	void setup() override;
	void keyDown( KeyEvent event ) override;
	void draw() override;
};

void PlatformTestApp::setup()
{
	auto platform = ci::app::PlatformLinux::get();

	console() << "getHomeDirectory()        : " << platform->getHomeDirectory() << std::endl;
	console() << "getDocumentsDirectory()   : " << platform->getDocumentsDirectory() << std::endl;

	console() << "getEnvironmentVariables() : " << std::endl;
	auto envVars = platform->getEnvironmentVariables();
	for( const auto& kv : envVars ) {
		console() << "   " << kv.first << "=" << kv.second << std::endl;
	}
}

void PlatformTestApp::keyDown( KeyEvent event )
{
	switch( event.getChar() ) {
		case 'W': {
			auto platform = ci::app::PlatformLinux::get();
			platform->launchWebBrowser( Url( "http://www.amazon.com" ) );
		}
		break;

		case '1': {
			auto platform = ci::app::PlatformLinux::get();
			std::vector<std::string> exts = { "jpg", "mp3", "gif", "ogg" };
			auto result = platform->getOpenFilePath( "/", exts );
			if( ! result.empty() ) {
				console() << "getOpenFilePath(): " << result << std::endl;
			}
		}
		break;

		case '2': {
			auto platform = ci::app::PlatformLinux::get();
			std::vector<std::string> exts = { "jpg", "mp3", "gif", "ogg" };
			auto result = platform->getFolderPath( "/home" );
			if( ! result.empty() ) {
				console() << "getFolderPath(): " << result << std::endl;
			}
		}
		break;

		case '3': {
			auto platform = ci::app::PlatformLinux::get();
			std::vector<std::string> exts = { "jpg", "mp3", "gif", "ogg" };
			auto result = platform->getSaveFilePath( "/", exts );
			if( ! result.empty() ) {
				console() << "getSaveFilePath(): " << result << std::endl;
			}
		}
		break;
	}
}

void PlatformTestApp::draw()
{
}

CINDER_APP( PlatformTestApp, RendererGl )
