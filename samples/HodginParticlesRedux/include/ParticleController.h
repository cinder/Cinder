
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

	void addParticles( int amt, ci::vec3 loc, ci::vec3 vel, float heat, float radius );
	void addGravity( ci::vec3 loc, float radius );
	void createConstraints( ci::vec2 windowDim );
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