/*
 *  Particle.h
 *
 *  Created by Mehmet Akten on 02/05/2009.
 *  Copyright 2009 MSA Visuals Ltd.. All rights reserved.
 *
 */

#pragma once

#include "ciMsaFluidSolver.h"
#include "cinder/Vector.h"

class Particle {
  public:	
    ci::Vec2f	pos, vel;
    float		radius;
    float		alpha;
    float		mass;
	
    void init(float x, float y);
    void update( const ciMsaFluidSolver &solver, const ci::Vec2f &windowSize, const ci::Vec2f &invWindowSize );
	void updateVertexArrays( bool drawingFluid, const ci::Vec2f &invWindowSize, int i, float* posBuffer, float* colBuffer);
};

