
#include "Config.h"

#include "cinder/app/App.h"
#include "cinder/Log.h"
#include "cinder/ImageIo.h"
#include "cinder/System.h"

using namespace std;
using namespace ci;

#define LOG_EXCEPTION( exc )	{ CI_LOG_E( "exception caught: " << System::demangleTypeName( typeid( exc ).name() ) << ", what: " << exc.what() ); }

Config::Config()
{
	reload();
}

void Config::reload()
{
	try {
		mData = loadJson( app::loadAsset( "config.json" ) );

		loadGearData();

		CI_LOG_V( "successfully loaded config.json" );
	}
	catch( Exception &exc ) {
		LOG_EXCEPTION( exc );
	}
}

float Config::getDescentSpeed() const
{
	return mData.value( "descent-speed", 100.0f );
}

ImageSourceRef Config::getBackgroundImage() const
{
	ImageSourceRef result;

	try {
		string imageName = mData["background"];
		result = loadImage( app::loadAsset( imageName ) );
	}
	catch( Exception &exc ) {
		LOG_EXCEPTION( exc );
	}

	return result;
}

void Config::loadGearData()
{
	mGears.clear();

	for( const auto &gear : mData["gears"] ) {
		string imageFilename = gear;

		auto tex = gl::Texture::create( loadImage( app::loadAsset( imageFilename ) ) );

		mGears.push_back( GearData() );
		mGears.back().mImageTex = tex;
	}
}

// ----------------------------------------------------------------------------------------------------
// MARK: - Singleton Access
// ----------------------------------------------------------------------------------------------------

// static
Config* Config::instance()
{
	static Config sInstance;
	return &sInstance;
}
