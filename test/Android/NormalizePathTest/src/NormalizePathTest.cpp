#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/android/CinderAndroid.h"
using namespace ci;
using namespace ci::app;
using namespace std;

// We'll create a new Cinder Application by deriving from the App class
class NormalizePathTest : public App {
  public:
	void setup();
	void draw();
};

void NormalizePathTest::setup()
{
	std::vector<fs::path> paths;

	paths.push_back( "view/milk/../some/path/./././../001//02//03" );

	for( const auto& p : paths ) {
		console() << ci::android::fs::normalizePath( p ) << std::endl;
	}
}

void NormalizePathTest::draw()
{
}

// This line tells Cinder to actually create the application
CINDER_APP( NormalizePathTest, RendererGl )
