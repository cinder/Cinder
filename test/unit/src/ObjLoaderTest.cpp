
#include "catch.hpp"
#include "cinder/ObjLoader.h"
#include "cinder/TriMesh.h"

using namespace cinder;

TEST_CASE( "ObjLoader" )
{
const auto planeData = std::string( R"obj(
v 1 1 -1
v 1 1 1
v -1.0 1.0 1.0
v -1.0 1.0 -1.0
vn 0.0 1.0 0.0
f 1 4 3 2
)obj" );

const auto planeDataNewlinesInVertices = std::string( R"obj(
v 1 1 \
 -1
v 1 \
 1 \
 1.0
v -1.0 \
 1.0 1.0
v -1.0 1.0 -1.0
vn 0.0 1.0 0.0
f 1 4 3 2
)obj" );

const auto planeDataNewlinesInFaces = std::string( R"obj(
v 1 1 -1
v 1 1 1
v -1.0 1.0 1.0
v -1.0 1.0 -1.0
vn 0.0 1.0 0.0
f 1 4 \
 3 2
)obj" );

const auto expectedPositions = std::vector<vec3> {
	vec3( 1, 1, -1 ),
	vec3( -1, 1, -1 ), // reordered by face indices
	vec3( -1, 1, 1 ),
	vec3( 1, 1, 1 )
};

const auto matchesExpectedPositions = [expectedPositions] (const ci::vec3 *positions) {
	for( auto i = 0; i < expectedPositions.size(); i += 1 ) {
		if( expectedPositions[i] != positions[i] ) {
			return false;
		}
	}
	return true;
};

SECTION( "Position matcher works properly" )
{
	auto wrong = std::vector<vec3> {
		vec3( 1, 1, -1 ),
		vec3( 1, 1, 1 )
	};
	REQUIRE( matchesExpectedPositions( expectedPositions.data() ) );
	REQUIRE_FALSE( matchesExpectedPositions( wrong.data() ) );
}

SECTION( "ObjLoader can parse simple files." )
{
	auto stream = IStreamMem::create(planeData.c_str(), planeData.size());
	auto obj = ObjLoader(stream);
	auto mesh = TriMesh::create(obj);
	REQUIRE( mesh->getNumTriangles() == 2 );
	REQUIRE( matchesExpectedPositions( mesh->getPositions<3>() ) );
}

SECTION( "ObjLoader correctly parses vertices with line continuations." )
{
	auto stream = IStreamMem::create(planeDataNewlinesInVertices.c_str(), planeDataNewlinesInVertices.size());
	auto obj = ObjLoader(stream);
	auto group = obj.getGroups();
	auto mesh = TriMesh::create(obj);
	REQUIRE( mesh->getNumTriangles() == 2 );
	REQUIRE( matchesExpectedPositions( mesh->getPositions<3>() ) );
}

SECTION( "ObjLoader successfully parses faces with line continuations." )
{
	auto stream = IStreamMem::create(planeDataNewlinesInFaces.c_str(), planeDataNewlinesInFaces.size());
	auto obj = ObjLoader(stream);
	auto group = obj.getGroups();
	auto mesh = TriMesh::create(obj);
	REQUIRE( mesh->getNumTriangles() == 2 );
	REQUIRE( matchesExpectedPositions( mesh->getPositions<3>() ) );
}

} // ObjLoader tests
