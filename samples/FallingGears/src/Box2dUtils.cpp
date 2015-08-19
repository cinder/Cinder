
#include "Box2dUtils.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace std;

namespace box2d {

//! the radius in physics dims is sometimes less than 1, so the number of circle segments needs to be chosen manually.
const int NUM_CIRCLE_SEGMENTS = 20;
//! scalar for drawing a body's transform axis.
const float TRANSFORM_AXIS_SCALE = 0.4f;

BodyRef makeBodyShared( b2World *world, const b2BodyDef &bodyDef )
{
	return BodyRef( world->CreateBody( &bodyDef ), [world]( b2Body *body ) { world->DestroyBody( body ); } );
}

vec2 toCinder( const b2Vec2 &vec )			{ return vec2( vec.x, vec.y ); }
Color toCinder( const b2Color &color )		{ return Color( color.r, color.g, color.b ); }

DebugDraw::DebugDraw()
{
	SetFlags( e_shapeBit );
}

void DebugDraw::setFlagBit( bool enable, uint32 bit )
{
	uint32 flags = GetFlags();
	flags = enable ? ( flags | bit ) : ( flags & ~bit );
	SetFlags( flags );
}

bool DebugDraw::isFlagBitEnabled( uint32 bit ) const
{
	return GetFlags() & bit;
}

void DebugDraw::DrawPolygon( const b2Vec2 *vertices, int32 vertexCount, const b2Color &color )
{
	gl::color( toCinder( color ) );

	Path2d path;
	path.moveTo( toCinder( vertices[0] ) );
	for( size_t i = 1; i < vertexCount; i++ )
		path.lineTo( toCinder( vertices[i] ) );

	gl::draw( path );
}

void DebugDraw::DrawSolidPolygon( const b2Vec2 *vertices, int32 vertexCount, const b2Color &color )
{
	gl::color( toCinder( color ) );

	Path2d path;
	path.moveTo( toCinder( vertices[0] ) );
	for( size_t i = 1; i < vertexCount; i++ )
		path.lineTo( toCinder( vertices[i] ) );

	gl::drawSolid( path );
}

void DebugDraw::DrawCircle( const b2Vec2 &center, float32 radius, const b2Color &color )
{
	gl::color( toCinder( color ) );
	gl::drawStrokedCircle( toCinder( center ), radius, NUM_CIRCLE_SEGMENTS );
}

void DebugDraw::DrawSolidCircle( const b2Vec2 &center, float32 radius, const b2Vec2 &axis, const b2Color &color )
{
	gl::color( toCinder( color ) );
	gl::drawSolidCircle( toCinder( center ), radius, NUM_CIRCLE_SEGMENTS );
}

void DebugDraw::DrawSegment( const b2Vec2 &p1, const b2Vec2 &p2, const b2Color &color )
{
	gl::color( toCinder( color ) );
	gl::drawLine( toCinder( p1 ), toCinder( p2 ) );
}

void DebugDraw::DrawTransform( const b2Transform &xf )
{
	gl::color( 1, 1, 0 );
	gl::drawLine( toCinder( xf.p ), toCinder( xf.p + TRANSFORM_AXIS_SCALE * xf.q.GetXAxis() ) );

	gl::color( 0, 1, 1 );
	gl::drawLine( toCinder( xf.p ), toCinder( xf.p + TRANSFORM_AXIS_SCALE * xf.q.GetYAxis() ) );
}

} // namespace box2d
