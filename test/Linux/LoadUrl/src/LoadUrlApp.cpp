#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Utilities.h"

using namespace ci;
using namespace ci::app;

class LoadUrlApp : public App {
  public:
	void setup() override;
	void draw() override;
};

void LoadUrlApp::setup()
{
	std::vector<std::string> urls = {
		"http://www.amazon.com",
		"http://www.google.com",
		"http://www.ebay.com",
		"http://www.facebook.com",
		"http://www.twitter.com",
	};

	for( const auto& url : urls ) {
		try {
			auto source = loadUrl( url );
			auto s = loadString( source );
			std::cout << "Loaded " << s.length() << " bytes from " << url << std::endl;
		}
		catch( const std::exception& e ) {
			std::cout << "Failed! url=" << url << ", err=" << e.what() << std::endl;
		}
	}
}

void LoadUrlApp::draw()
{
}

CINDER_APP( LoadUrlApp, RendererGl )
