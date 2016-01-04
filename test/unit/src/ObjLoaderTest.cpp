
#include "catch.hpp"
#include "cinder/ObjLoader.h"
#include "cinder/TriMesh.h"

using namespace cinder;

TEST_CASE( "ObjLoader" )
{
const auto planeData = std::string( R"obj(
v 1 1 -1
v 1 1 1.0
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
v -1.0 \\
 1.0 1.0
v -1.0 1.0 -1.0
vn 0.0 1.0 0.0
f 1 4 3 2
)obj" );

const auto planeDataNewlinesInFaces = std::string( R"obj(
v 1 1 -1
v 1 1 1.0
v -1.0 1.0 1.0
v -1.0 1.0 -1.0
vn 0.0 1.0 0.0
f 1 4 \
 3 2
)obj" );

const auto correctFirstPosition = vec3( 1, 1, -1 );

SECTION( "ObjLoader can parse simple files." )
{
	auto stream = IStreamMem::create(planeData.c_str(), planeData.size());
	auto obj = ObjLoader(stream);
	auto mesh = TriMesh::create(obj);
	REQUIRE( mesh->getNumTriangles() == 2 );

	auto *positions = mesh->getPositions<3>();

	REQUIRE( positions[0] == correctFirstPosition );
}

SECTION( "ObjLoader incorrectly parses vertices with line continuations." )
{
	auto stream = IStreamMem::create(planeDataNewlinesInVertices.c_str(), planeDataNewlinesInVertices.size());
	auto obj = ObjLoader(stream);
	auto mesh = TriMesh::create(obj);
	REQUIRE( mesh->getNumTriangles() == 2 );

	REQUIRE_FALSE( mesh->getPositions<3>()[0] == correctFirstPosition );
}

SECTION( "ObjLoader throws an exception on faces with line continuations." )
{
	auto stream = IStreamMem::create(planeDataNewlinesInFaces.c_str(), planeDataNewlinesInFaces.size());
	REQUIRE_THROWS( auto obj = ObjLoader(stream) );
}

} // ObjLoader tests
