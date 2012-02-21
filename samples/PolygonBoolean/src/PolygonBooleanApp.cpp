#include "cinder/app/AppBasic.h"
#include "cinder/Triangulate.h"
#include "cinder/TriMesh.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class PolygonBooleanApp : public AppBasic {
  public:
	void setup();
	void draw();

	void				doUnion();
	void				doIntersection();
	void				doXor();
	void				doDifference();

	vector<PolyLine2f>	makePolyWithHole() const;
	vector<PolyLine2f>	makeStar() const;
	vector<PolyLine2f>	makeLetterA() const;
	
	vector<PolyLine2f> 	mPolyA, mPolyB, mPolyResult;
	TriMesh2d			mPolyAMesh, mPolyBMesh;
	
	params::InterfaceGl	mParams;
};

vector<PolyLine2f> PolygonBooleanApp::makePolyWithHole() const
{
	vector<PolyLine2f> result;	
	result.push_back( PolyLine2f() );
	result.back().push_back( Vec2f( 200, 130 ) ); result.back().push_back( Vec2f( 240, 170 ) );
	result.back().push_back( Vec2f( 280, 180 ) ); result.back().push_back( Vec2f( 340, 120 ) );
	result.back().push_back( Vec2f( 370, 160 ) ); result.back().push_back( Vec2f( 340, 200 ) );
	result.back().push_back( Vec2f( 410, 300 ) ); result.back().push_back( Vec2f( 530, 260 ) );
	result.back().push_back( Vec2f( 540, 120 ) ); result.back().push_back( Vec2f( 490, 80 ) );
	result.back().push_back( Vec2f( 290, 70 ) ); result.back().push_back( Vec2f( 200, 130 ) );

	result.push_back( PolyLine2f() );
	result.back().push_back( Vec2f( 400, 200 ) ); result.back().push_back( Vec2f( 420, 140 ) );
	result.back().push_back( Vec2f( 480, 190 ) ); result.back().push_back( Vec2f( 440, 220 ) );
	result.back().push_back( Vec2f( 400, 200 ) );
	return result;
}

vector<PolyLine2f> PolygonBooleanApp::makeStar() const
{
	vector<PolyLine2f> result;	
	result.push_back( PolyLine2f() );
	result.back().push_back( Vec2f( 400, -50 ) ); result.back().push_back( Vec2f( 350, 100 ) );
	result.back().push_back( Vec2f( 200, 150 ) ); result.back().push_back( Vec2f( 350, 200 ) );
	result.back().push_back( Vec2f( 400, 350 ) ); result.back().push_back( Vec2f( 450, 200 ) );
	result.back().push_back( Vec2f( 600, 150 ) ); result.back().push_back( Vec2f( 450, 100 ) );
	result.back().push_back( Vec2f( 400, -50 ) );
	return result;
}

vector<PolyLine2f> PolygonBooleanApp::makeLetterA() const
{
	vector<PolyLine2f> result;	
	Font arial( "Arial", 512 );
	Shape2d shape = arial.getGlyphShape( arial.getGlyphChar( 'a' ) );
	for( vector<Path2d>::const_iterator pathIt = shape.getContours().begin(); pathIt != shape.getContours().end(); ++pathIt ) {
		PolyLine2f contour( pathIt->subdivide() );
		contour.offset( Vec2f( 200, 260 ) );
		result.push_back( contour );
	}
	return result;
}


TriMesh2d makeMesh( const vector<PolyLine2f> &polys )
{
	Triangulator triangulator;
	for( vector<PolyLine2f>::const_iterator polyIt = polys.begin(); polyIt != polys.end(); ++polyIt )
		triangulator.addPolyLine( *polyIt );
	
	return triangulator.calcMesh();
}

void PolygonBooleanApp::doIntersection()
{
	mPolyResult = PolyLine2f::calcIntersection( mPolyA, mPolyB );
}

void PolygonBooleanApp::doUnion()
{
	mPolyResult = PolyLine2f::calcUnion( mPolyA, mPolyB );
}

void PolygonBooleanApp::doXor()
{
	mPolyResult = PolyLine2f::calcXor( mPolyA, mPolyB );
}

void PolygonBooleanApp::doDifference()
{
	mPolyResult = PolyLine2f::calcDifference( mPolyA, mPolyB );
}

void PolygonBooleanApp::setup()
{
	mParams = params::InterfaceGl( "App parameters", Vec2i( 180, 100 ) );
	mParams.addButton( "Union", std::bind( &PolygonBooleanApp::doUnion, this ) );
	mParams.addButton( "Intersection", std::bind( &PolygonBooleanApp::doIntersection, this ) );
	mParams.addButton( "XOR", std::bind( &PolygonBooleanApp::doXor, this ) );
	mParams.addButton( "Difference", std::bind( &PolygonBooleanApp::doDifference, this ) );

	mPolyA = makePolyWithHole();
	mPolyAMesh = makeMesh( mPolyA );

	mPolyB = makeLetterA();	//makeStar();
	mPolyBMesh = makeMesh( mPolyB );
}

void PolygonBooleanApp::draw()
{
	gl::enableAlphaBlending();
	gl::clear( Color( 0.24f, 0.24f, 0.24f ) );
	
	gl::color( ColorA( 0.25f, 0.5f, 1.0f, 0.15f ) );
	gl::draw( mPolyAMesh );

	gl::color( ColorA( 0.25f, 1.0f, 0.5f, 0.15f ) );
	gl::draw( mPolyBMesh );
	
	glLineWidth( 2.0f );
	gl::color( ColorA( 1.0f, 0.5f, 0.25f, 1.0f ) );
	for( size_t c = 0; c < mPolyResult.size(); ++c )
		gl::draw( mPolyResult[c] );	
		
	mParams.draw();
}

CINDER_APP_BASIC( PolygonBooleanApp, RendererGl )
