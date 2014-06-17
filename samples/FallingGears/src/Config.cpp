
#include "Config.h"

#include "cinder/app/App.h"
#include "cinder/audio/Debug.h"
#include "cinder/ImageIo.h"
#include "cinder/System.h"

using namespace std;
using namespace ci;

#define LOG_EXCEPTION( exc )	{ CI_LOG_E( "exception caught: " << System::demangleTypeName( typeid( exc ).name() ) << ", what: " << exc.what() ); }

namespace {

inline Vec2f parseVec2f( const JsonTree &json )
{
	return Vec2f( json.getValueAtIndex<float>( 0 ), json.getValueAtIndex<float>( 1 ) );
}

}

Config::Config()
{
	reload();
}

void Config::reload()
{
	try {
		mData = JsonTree( app::loadAsset( "config.json" ) );

		loadGearData();

		CI_LOG_V( "successfully loaded config.json" );
	}
	catch( exception &exc ) {
		LOG_EXCEPTION( exc );
	}
}

float Config::getDecentSpeed() const
{
	float result = 100;
	if( mData.hasChild( "decent-speed" ) )
		result = mData.getValueForKey<float>( "decent-speed" );

	return result;
}

ImageSourceRef Config::getBackgroundImage() const
{
	ImageSourceRef result;

	try {
		string imageName = mData.getValueForKey( "background" );
		result = loadImage( app::loadAsset( imageName ) );
	}
	catch( exception &exc ) {
		LOG_EXCEPTION( exc );
	}

	return result;
}

void Config::loadGearData()
{
	mGears.clear();

	for( const auto &gear : mData["gears"] ) {
		string imageFilename = gear.getValue();

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
