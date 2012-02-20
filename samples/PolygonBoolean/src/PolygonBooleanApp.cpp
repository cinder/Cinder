#include "cinder/app/AppBasic.h"
#include "cinder/Triangulate.h"
#include "cinder/TriMesh.h"
#include "cinder/gl/gl.h"

#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/multi/geometries/multi_polygon.hpp>
#include <boost/geometry/domains/gis/io/wkt/wkt.hpp>
#include <boost/geometry/geometry.hpp>
#include <boost/geometry/geometries/adapted/c_array.hpp>
#include <boost/geometry/multi/multi.hpp>

#include <boost/foreach.hpp>

using namespace ci;
using namespace ci::app;
using namespace std;

class PolygonBooleanApp : public AppBasic {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();

	vector<PolyLine2f>	makePolyWithHole() const;
	vector<PolyLine2f>	makeStar() const;
	vector<PolyLine2f>	makeLetterA() const;
	
	vector<PolyLine2f>	calcUnion( const vector<PolyLine2f> &a, vector<PolyLine2f> &b );
	
	vector<PolyLine2f> 	mPolyA, mPolyB, mPolyResult;
	TriMesh2d			mPolyAMesh, mPolyBMesh, mPolyResultMesh;
};

namespace {
typedef boost::geometry::model::polygon<boost::geometry::model::d2::point_xy<double> > polygon;

vector<PolyLine2f> convertBoostGeometryPolygons( std::vector<polygon> &polygons )
{
	vector<PolyLine2f> result;
	for( vector<polygon>::const_iterator outIt = polygons.begin(); outIt != polygons.end(); ++outIt ) {
		typedef polygon::inner_container_type::const_iterator RingIterator;
		typedef polygon::ring_type::const_iterator PointIterator;

		result.push_back( PolyLine2f() );	
		for( PointIterator pt = outIt->outer().begin(); pt != outIt->outer().end(); ++pt )
			result.back().push_back( Vec2f( boost::geometry::get<0>(*pt), boost::geometry::get<1>(*pt) ) );

		for( RingIterator crunk = outIt->inners().begin(); crunk != outIt->inners().end(); ++crunk ) {
			PolyLine2f contour;
			for( PointIterator pt = crunk->begin(); pt != crunk->end(); ++pt )
				contour.push_back( Vec2f( boost::geometry::get<0>(*pt), boost::geometry::get<1>(*pt) ) );
			result.push_back( contour );
		}
	}
	
	return result;
}

polygon convertPolyLinesToBoostGeometry( const vector<PolyLine2f> &a )
{
	polygon result;
	
	for( vector<Vec2f>::const_iterator ptIt = a[0].getPoints().begin(); ptIt != a[0].getPoints().end(); ++ptIt )
		result.outer().push_back( boost::geometry::make<boost::geometry::model::d2::point_xy<double> >( ptIt->x, ptIt->y ) );
	for( vector<PolyLine2f>::const_iterator plIt = a.begin() + 1; plIt != a.end(); ++plIt ) {
		polygon::ring_type ring;
		for( vector<Vec2f>::const_iterator ptIt = plIt->getPoints().begin(); ptIt != plIt->getPoints().end(); ++ptIt )
			ring.push_back( boost::geometry::make<boost::geometry::model::d2::point_xy<double> >( ptIt->x, ptIt->y ) );
		result.inners().push_back( ring );
	}
	
	boost::geometry::correct( result );
	
	return result;
}
} // anonymous namespace

vector<PolyLine2f> PolygonBooleanApp::calcUnion( const vector<PolyLine2f> &a, vector<PolyLine2f> &b )
{
	typedef boost::geometry::model::polygon<boost::geometry::model::d2::point_xy<double> > polygon;

	if( a.empty() )
		return b;
	else if( b.empty() )
		return a;

	polygon polyA = convertPolyLinesToBoostGeometry( a );
	polygon polyB = convertPolyLinesToBoostGeometry( b );
	
	std::vector<polygon> output;
//	boost::geometry::union_( polyA, polyB, output );
	boost::geometry::intersection( polyA, polyB, output );

	return convertBoostGeometryPolygons( output );
}

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

void PolygonBooleanApp::setup()
{
	mPolyA = makePolyWithHole();
	mPolyAMesh = makeMesh( mPolyA );

	mPolyB = makeLetterA();//makeStar();
	mPolyBMesh = makeMesh( mPolyB );

	mPolyResult = calcUnion( mPolyA, mPolyB );
	mPolyResultMesh = makeMesh( mPolyB );
}

void PolygonBooleanApp::mouseDown( MouseEvent event )
{
}

void PolygonBooleanApp::update()
{
}

void PolygonBooleanApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0.24f, 0.24f, 0.24f ) );
	gl::enableAlphaBlending();
	
	gl::color( ColorA( 0.25f, 0.5f, 1.0f, 0.25f ) );
	gl::draw( mPolyAMesh );

	gl::color( ColorA( 0.25f, 1.0f, 0.5f, 0.25f ) );
	gl::draw( mPolyBMesh );
	
	glLineWidth( 2.0f );
	gl::color( ColorA( 1.0f, 0.5f, 0.25f, 0.9f ) );
	for( size_t c = 0; c < mPolyResult.size(); ++c )
		gl::draw( mPolyResult[c] );	
}

CINDER_APP_BASIC( PolygonBooleanApp, RendererGl )
