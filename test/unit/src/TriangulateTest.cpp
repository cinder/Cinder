
#include "catch.hpp"
#include "cinder/Triangulate.h"
#include "cinder/app/App.h"

using namespace cinder;
using namespace app;
using namespace std;


TEST_CASE( "Triangulate" )
{
	SECTION( "No input" )
	{
		Triangulator triangulator;

		auto checkEmptyMesh = []( const TriMesh &mesh, u_int8_t dimensions ) {
			CHECK( mesh.getAttribDims( geom::Attrib::POSITION ) == dimensions );
			CHECK( mesh.getNumIndices() == 0 );
			CHECK( mesh.getNumVertices() == 0 );
		};

		SECTION( "calcMesh" )
		{
			checkEmptyMesh( triangulator.calcMesh(), 2);
		}

		SECTION( "createMesh" )
		{
			checkEmptyMesh( *triangulator.createMesh(), 2);
		}

		SECTION( "calcMesh3d" )
		{
			checkEmptyMesh( triangulator.calcMesh3d(), 3);
		}

		SECTION( "createMesh3d" )
		{
			checkEmptyMesh( *triangulator.createMesh3d(), 3);
		}
	}

	SECTION( "Path2d input" )
	{
		Path2d input;
		input.moveTo( vec2( 0, 0 ) );
		input.lineTo( vec2( 1, 1 ) );
		input.lineTo( vec2( 0, 2 ) );

		auto checkResults = []( const TriMesh &mesh ) {
			REQUIRE( mesh.getAttribDims( geom::Attrib::POSITION ) == 2 );
			REQUIRE( mesh.getNumVertices() == 3 );
			CHECK( mesh.getNumIndices() == 3 );

			const vec2 *positions = mesh.getPositions<2>();
			CHECK( positions[0] == vec2( 1, 1 ) );
			CHECK( positions[1] == vec2( 0, 2 ) );
			CHECK( positions[2] == vec2( 0, 0 ) );
		};

		SECTION( "via constructor" )
		{
			Triangulator triangulator( input );

			SECTION( "calcMesh" )
			{
				checkResults( triangulator.calcMesh() );
			}

			SECTION( "createMesh" )
			{
				checkResults( *triangulator.createMesh() );
			}
		}

		SECTION( "via addPath" )
		{
			Triangulator triangulator;
			triangulator.addPath( input );

			SECTION( "calcMesh" )
			{
				checkResults( triangulator.calcMesh() );
			}

			SECTION( "createMesh" )
			{
				checkResults( *triangulator.createMesh() );
			}
		}
	}

	SECTION( "Shape2d input" )
	{
		Shape2d input;
		input.moveTo( vec2( 0, 0 ) );
		input.lineTo( vec2( 1, 1 ) );
		input.lineTo( vec2( 0, 2 ) );

		auto checkResults = []( const TriMesh &mesh ) {
			REQUIRE( mesh.getAttribDims( geom::Attrib::POSITION ) == 2 );
			REQUIRE( mesh.getNumVertices() == 3 );
			CHECK( mesh.getNumIndices() == 3 );

			const vec2 *positions = mesh.getPositions<2>();
			CHECK( positions[0] == vec2( 1, 1 ) );
			CHECK( positions[1] == vec2( 0, 2 ) );
			CHECK( positions[2] == vec2( 0, 0 ) );
		};

		SECTION( "via constructor" )
		{
			Triangulator triangulator( input );

			SECTION( "calcMesh" )
			{
				checkResults( triangulator.calcMesh() );
			}

			SECTION( "createMesh" )
			{
				checkResults( *triangulator.createMesh() );
			}
		}

		SECTION( "via addShape" )
		{
			Triangulator triangulator;
			triangulator.addShape( input );

			SECTION( "calcMesh" )
			{
				checkResults( triangulator.calcMesh() );
			}

			SECTION( "createMesh" )
			{
				checkResults( *triangulator.createMesh() );
			}
		}
	}

	SECTION( "PolyLine2f input" )
	{
		PolyLine2f input( { vec2( 0, 0 ), vec2( 1, 1 ), vec2( 0, 2 ) } );

		auto checkResults = []( const TriMesh &mesh ) {
			REQUIRE( mesh.getAttribDims( geom::Attrib::POSITION ) == 2 );
			REQUIRE( mesh.getNumVertices() == 3 );
			CHECK( mesh.getNumIndices() == 3 );

			const vec2 *positions = mesh.getPositions<2>();
			CHECK( positions[0] == vec2( 1, 1 ) );
			CHECK( positions[1] == vec2( 0, 2 ) );
			CHECK( positions[2] == vec2( 0, 0 ) );
		};

		SECTION( "via constructor" )
		{
			Triangulator triangulator( input );

			SECTION( "calcMesh" )
			{
				checkResults( triangulator.calcMesh() );
			}

			SECTION( "createMesh" )
			{
				checkResults( *triangulator.createMesh() );
			}
		}

		SECTION( "via addPolyLine(PolyLine2f)" )
		{
			Triangulator triangulator;
			triangulator.addPolyLine( input );

			SECTION( "calcMesh" )
			{
				checkResults( triangulator.calcMesh() );
			}

			SECTION( "createMesh" )
			{
				checkResults( *triangulator.createMesh() );
			}
		}

		SECTION( "via addPolyLine(vec2[])" )
		{
			Triangulator triangulator;
			triangulator.addPolyLine( input.getPoints().data(), input.size() );

			SECTION( "calcMesh" )
			{
				checkResults( triangulator.calcMesh() );
			}

			SECTION( "createMesh" )
			{
				checkResults( *triangulator.createMesh() );
			}
		}
	}

	SECTION( "vec3 input" )
	{
		std::vector<vec3> input( { vec3( 0, 0, 1 ), vec3( 1, 1, 1 ), vec3( 0, 2, 1 ) } );

		auto checkResults = []( const TriMesh &mesh ) {
			REQUIRE( mesh.getAttribDims( geom::Attrib::POSITION ) == 3 );
			REQUIRE( mesh.getNumVertices() == 3 );
			CHECK( mesh.getNumIndices() == 3 );

			const vec3 *positions = mesh.getPositions<3>();
			CHECK( positions[0] == vec3( 1, 1, 1 ) );
			CHECK( positions[1] == vec3( 0, 2, 1 ) );
			CHECK( positions[2] == vec3( 0, 0, 1 ) );
		};

		SECTION( "via constructor" )
		{
			Triangulator triangulator( input );

			SECTION( "calcMesh3d" )
			{
				checkResults( triangulator.calcMesh3d() );
			}

			SECTION( "createMesh3d" )
			{
				checkResults( *triangulator.createMesh3d() );
			}
		}

		SECTION( "via addPolyLine(vec3[])" )
		{
			Triangulator triangulator;
			triangulator.addPolyLine( input.data(), input.size() );

			SECTION( "calcMesh3d" )
			{
				checkResults( triangulator.calcMesh3d() );
			}

			SECTION( "createMesh3d" )
			{
				checkResults( *triangulator.createMesh3d() );
			}
		}
	}
}
