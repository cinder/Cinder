
#pragma once

#include "cinder/Vector.h"
#include "Particle.h"
#include "cinder/gl/Texture.h"

#include <list>

class Force {
 public:
	Force( int type, float mag );
	virtual void update();
	virtual void apply( std::list<Particle> &particles, int counter );
	virtual void enableIfType( int type, bool enable );
	
	void setMagnitude( float mag );

	int mType;
	bool mEnabled;
	
 protected:
	float mMagnitude;
};
