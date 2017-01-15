#include "catch.hpp"

#include "cinder/Cinder.h"
#include "cinder/gl/ShaderPreprocessor.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/Utilities.h"
#include "cinder/app/App.h"

using namespace std;
using namespace ci;

void preprocessAndCompare( gl::ShaderPreprocessor *preprocessor, const fs::path &sourceFilename )
{
	const fs::path shaderAssetFolder = "shader_preprocessor";
	const fs::path sourcePath = shaderAssetFolder / sourceFilename;
	const fs::path expectedPath = shaderAssetFolder / ( sourceFilename.stem().string() + "_expected.frag" );
	const string source = loadString( app::loadAsset( sourcePath ) );
	const string expected = loadString( app::loadAsset( expectedPath ) );

	// compare by passing the source to ShaderPreprocessor
	{
		const string preprocessed = preprocessor->parse( source, sourcePath );
		REQUIRE( preprocessed == expected );
	}

	// compare by passing the asset path to ShaderPreprocessor
	{
		const string preprocessed = preprocessor->parse( sourcePath );
		REQUIRE( preprocessed == expected );
	}
}

TEST_CASE( "ShaderPreprocessor" )
{
	SECTION( "Compare preprocessed output to expected result." )
	{
		SECTION( "has version, no defines" )
		{
			gl::ShaderPreprocessor preprocessor;
			preprocessAndCompare( &preprocessor, "has_version_no_defines.frag" );
		}
	}
}
