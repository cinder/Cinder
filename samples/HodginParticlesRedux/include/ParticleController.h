
#pragma once
#include "Particle.h"
#include "Force.h"
#include "Constraint.h"
#include "Emitter.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/gl/Texture.h"
#include <list>
#include <vector>

class ParticleController {
 public:
	ParticleController();
	void update( Emitter &emitter, int counter  );
	void render( float heat );
	void renderParticles( float heat );
	void renderTrails();

	void addParticles( int amt, ci::Vec3f loc, ci::Vec3f vel, float heat, float radius );
	void addGravity( ci::Vec3f loc, float radius );
	void createConstraints( ci::Vec2f windowDim );
	void adjustForcesOfType( int type, bool enable );
	
	std::list<Particle>			mParticles;
	std::vector<Force*>			mForces;
	std::vector<Constraint*>	mConstraints;
	
	int			FORCEGRAVITY;
	int			FORCEPERLIN;
	int			FORCEREPULSION;
	
	bool		mEnableConstraints;
	bool		mEnableGravity;
	bool		mEnablePerlin;
	bool		mEnableRepulsion;
	bool		mEnableTail;
	bool		mEnableQuad;
	
	ci::gl::Texture mParticleTexture;
};