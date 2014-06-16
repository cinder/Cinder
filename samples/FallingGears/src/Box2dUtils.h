
#pragma once

#include "cinder/Vector.h"
#include "cinder/Color.h"

#include <Box2D/Box2D.h>

#include <memory>

namespace box2d {

typedef std::shared_ptr<b2Body>	BodyRef;

BodyRef	makeBodyShared( b2World *world, const b2BodyDef &bodyDef );

ci::Vec2f toCinder( const b2Vec2 &vec );
ci::Color toCinder( const b2Color &color );

class DebugDraw : public b2Draw {
  public:
	DebugDraw();
	virtual ~DebugDraw()	{}

	void setShapeEnabled( bool enable = true )			{ setFlagBit( enable, e_shapeBit ); }
	void setJointEnabled( bool enable = true )			{ setFlagBit( enable, e_jointBit ); }
	void setAabbEnabled( bool enable = true )			{ setFlagBit( enable, e_aabbBit ); }
	void setPairEnabled( bool enable = true )			{ setFlagBit( enable, e_pairBit ); }
	void setCenterOfMassEnabled( bool enable = true )	{ setFlagBit( enable, e_centerOfMassBit ); }

	bool isShapeEnabled() const			{ return isFlagBitEnabled( e_shapeBit ); }
	bool isJointEnabled() const			{ return isFlagBitEnabled( e_jointBit ); }
	bool isAabbEnabled() const			{ return isFlagBitEnabled( e_aabbBit ); }
	bool isPairEnabled() const			{ return isFlagBitEnabled( e_pairBit ); }
	bool isCenterOfMassEnabled() const	{ return isFlagBitEnabled( e_centerOfMassBit ); }

  protected:
	void DrawPolygon( const b2Vec2 *vertices, int32 vertexCount, const b2Color &color )						override;
	void DrawSolidPolygon( const b2Vec2 *vertices, int32 vertexCount, const b2Color &color )				override;
	void DrawCircle( const b2Vec2 &center, float32 radius, const b2Color &color )							override;
	void DrawSolidCircle( const b2Vec2 &center, float32 radius, const b2Vec2 &axis, const b2Color &color )	override;
	void DrawSegment( const b2Vec2 &p1, const b2Vec2 &p2, const b2Color &color )							override;
	void DrawTransform( const b2Transform &xf )																override;
	
  private:
	void setFlagBit( bool enable, uint32 bit );
	bool isFlagBitEnabled( uint32 bit ) const;
};

} // namespace box2d
