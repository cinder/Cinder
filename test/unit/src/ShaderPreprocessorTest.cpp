#include "catch.hpp"

#include "cinder/Cinder.h"
#include "cinder/gl/ShaderPreprocessor.h"
#include "cinder/Utilities.h"
#include "cinder/app/App.h"

using namespace std;
using namespace ci;

TEST_CASE( "ShaderPreprocessor" )
{
	SECTION( "test simple parse filepath with include" )
	{
		gl::ShaderPreprocessor preprocessor;

		fs::path sourcePath = app::getAssetPath( "shader_preprocessor/simple.frag" );

		std::set<fs::path> includedFiles;
		const string result = preprocessor.parse( sourcePath, &includedFiles );

		REQUIRE( result.find( "#include" ) == string::npos );
		REQUIRE( result.find( "fract( sin( n ) * 43758.5453 )" ) != string::npos );

		fs::path commonSimplePath = fs::canonical( app::getAssetPath( "shader_preprocessor/commonSimple.glsl" ) );
		REQUIRE( includedFiles.size() == 1 );
		REQUIRE( includedFiles.count( commonSimplePath ) == 1 );
	}

	SECTION( "test simple parse filepath with include" )
	{
		gl::ShaderPreprocessor preprocessor;

		fs::path sourcePath = app::getAssetPath( "shader_preprocessor/simple.frag" );
		const string source = loadString( loadFile( sourcePath ) );

		std::set<fs::path> includedFiles;
		const string result = preprocessor.parse( source, sourcePath, &includedFiles );

		REQUIRE( result.find( "#include" ) == string::npos );
		REQUIRE( result.find( "fract( sin( n ) * 43758.5453 )" ) != string::npos );

		fs::path commonSimplePath = fs::canonical( app::getAssetPath( "shader_preprocessor/commonSimple.glsl" ) );
		REQUIRE( includedFiles.size() == 1 );
		REQUIRE( includedFiles.count( commonSimplePath ) == 1 );
	}

	SECTION( "test defines" )
	{
		gl::ShaderPreprocessor preprocessor;
		preprocessor.addDefine( "BLARG" );
		preprocessor.addDefine( "SOMEVAR", "2" );

		const string result = preprocessor.parse( app::getAssetPath( "shader_preprocessor/simple.frag" ) );

		REQUIRE( result.find( "#define BLARG" ) != string::npos );
		REQUIRE( result.find( "#define SOMEVAR 2" ) != string::npos );
	}

	SECTION( "test remove define" )
	{
		gl::ShaderPreprocessor preprocessor;
		preprocessor.addDefine( "BLARG" );
		preprocessor.addDefine( "SOMEVAR", "2" );

		preprocessor.removeDefine( "BLARG" );
		preprocessor.removeDefine( "SOMEVAR" );

		REQUIRE( preprocessor.getDefines().size() == 0 );

		const string result = preprocessor.parse( app::getAssetPath( "shader_preprocessor/simple.frag" ) );

		REQUIRE( result.find( "#define BLARG" ) == string::npos );
		REQUIRE( result.find( "#define SOMEVAR 2" ) == string::npos );
	}
	
	SECTION( "test overwrite define" )
	{
		gl::ShaderPreprocessor preprocessor;
		preprocessor.addDefine( "SOMEVAR", "1" );
		preprocessor.addDefine( "SOMEVAR", "2" );

		REQUIRE( preprocessor.getDefines().size() == 1 );

		const string result = preprocessor.parse( app::getAssetPath( "shader_preprocessor/simple.frag" ) );

		REQUIRE( result.find( "#define SOMEVAR 1" ) == string::npos );
		REQUIRE( result.find( "#define SOMEVAR 2" ) != string::npos );
	}

	SECTION( "test nested includes" )
	{
		gl::ShaderPreprocessor preprocessor;

		fs::path sourcePath = app::getAssetPath( "shader_preprocessor/shaderWithNestedIncludes.frag" );

		std::set<fs::path> includedFiles;
		const string result = preprocessor.parse( sourcePath, &includedFiles );

		REQUIRE( result.find( "#include" ) == string::npos );
		REQUIRE( result.find( "fract( sin( n ) * 43758.5453 )" ) != string::npos );
		REQUIRE( result.find( "float noise( in vec2 x )" ) != string::npos );

		fs::path commonPath = fs::canonical( app::getAssetPath( "shader_preprocessor/common.glsl" ) );
		fs::path hashPath = fs::canonical( app::getAssetPath( "shader_preprocessor/hash.glsl" ) );
		REQUIRE( includedFiles.size() == 2 );
		REQUIRE( includedFiles.count( commonPath ) == 1 );
		REQUIRE( includedFiles.count( hashPath ) == 1 );
	}

	SECTION( "test include handler" )
	{
		// this string should be loaded into result instead of the simpleCommon.glsl on file

		const std::string commonGlsl = R"(
		float hash( float n )
		{
			return 0.5f;
		}
		)";

		gl::ShaderPreprocessor preprocessor;
			
		preprocessor.getSignalInclude().connect(
			[&commonGlsl]( const fs::path &path, std::string *source ) {
				if( path == fs::path( "commonSimple.glsl" ) ) {
					*source = commonGlsl;
					return true;
				}

				return false;
			}
		);

		fs::path sourcePath = app::getAssetPath( "shader_preprocessor/simple.frag" );

		std::set<fs::path> includedFiles;
		const string result = preprocessor.parse( sourcePath, &includedFiles );

		REQUIRE( result.find( "return 0.5f;" ) != string::npos );

		REQUIRE( includedFiles.size() == 1 );
		REQUIRE( includedFiles.count( "commonSimple.glsl" ) == 1 );
	}
}
