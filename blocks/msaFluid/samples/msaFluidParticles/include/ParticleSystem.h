/*
 *  ParticleSystem.h
 *
 *  Created by Mehmet Akten on 02/05/2009.
 *  Copyright 2009 MSA Visuals Ltd.. All rights reserved.
 *
 */
#pragma once

#include "Particle.h"
#include "cinder/Vector.h"

#define MAX_PARTICLES		50000


class ParticleSystem {
public:	
	
    float posArray[MAX_PARTICLES * 2 * 2];
    float colArray[MAX_PARTICLES * 3 * 2];
	ci::Vec2i	windowSize;
	ci::Vec2f	invWindowSize;
	const ciMsaFluidSolver	*solver;
	
    int curIndex;
	
    Particle particles[MAX_PARTICLES];
	
	ParticleSystem();
	void setFluidSolver( const ciMsaFluidSolver *aSolver ) { solver = aSolver; }
	
    void updateAndDraw( bool drawingFluid );
	void addParticles( const ci::Vec2f &pos, int count );
	void addParticle( const ci::Vec2f &pos );
	void setWindowSize( ci::Vec2i winSize );
};




