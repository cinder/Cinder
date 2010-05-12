/*
 *  Particle.cpp
 *
 *  Created by Mehmet Akten on 02/05/2009.
 *  Copyright 2009 MSA Visuals Ltd.. All rights reserved.
 *
 */

#include "Particle.h"
#include "cinder/Rand.h"
#include "cinder/gl/gl.h"

#define MOMENTUM		0.5
#define FLUID_FORCE		0.6

using namespace ci;

void Particle::init(float x, float y) {
	this->x = x;
	this->y = y;
	vx = 0;
	vy = 0;
	radius = 5;
	alpha  = Rand::randFloat( 0.3f, 1 );
	mass = Rand::randFloat( 0.1f, 1 );
}

void Particle::update( const ciMsaFluidSolver &solver, const Vec2f &windowSize, const Vec2f &invWindowSize ) {
	// only update if particle is visible
	if(alpha == 0) return;
	
	// read fluid info and add to velocity
	int fluidIndex = solver.getIndexForNormalizedPosition(x * invWindowSize.x, y * invWindowSize.y);
	vx = solver.uv[fluidIndex].x * windowSize.x * mass * FLUID_FORCE + vx * MOMENTUM;
	vy = solver.uv[fluidIndex].y * windowSize.y * mass * FLUID_FORCE + vy * MOMENTUM;
	
//	color.r = myApp->fluidSolver.r[fluidIndex] + 0.5;
//	color.g = myApp->fluidSolver.g[fluidIndex] + 0.5;
//	color.b = myApp->fluidSolver.b[fluidIndex] + 0.5;
	
	
	// update position
	x += vx;
	y += vy;
	
	// bounce of edges
	if(x<0) {
		x = 0;
		vx *= -1;
	}
	else if(x > windowSize.x) {
		x = windowSize.x;
		vx *= -1;
	}
	
	if(y<0) {
		y = 0;
		vy *= -1;
	}
	else if(y > windowSize.y) {
		y = windowSize.y;
		vy *= -1;
	}
	
	// hackish way to make particles glitter when the slow down a lot
	if(vx * vx + vy * vy < 1) {
		vx = Rand::randFloat(-0.5, 0.5);
		vy = Rand::randFloat(-0.5, 0.5);
	}
	
	// fade out a bit (and kill if alpha == 0);
	alpha *= 0.999;
	if(alpha < 0.01) alpha = 0;
}


void Particle::updateVertexArrays( bool drawingFluid, const Vec2f &invWindowSize, int i, float* posBuffer, float* colBuffer) {
	int vi = i * 4;
	posBuffer[vi++] = x - vx;
	posBuffer[vi++] = y - vy;
	posBuffer[vi++] = x;
	posBuffer[vi++] = y;
	
	int ci = i * 6;
	if( drawingFluid ) {
		// if drawing fluid, draw lines as black & white
		colBuffer[ci++] = alpha;
		colBuffer[ci++] = alpha;
		colBuffer[ci++] = alpha;
		colBuffer[ci++] = alpha;
		colBuffer[ci++] = alpha;
		colBuffer[ci++] = alpha;
	} else {
		// otherwise, use color
		float vxNorm = vx * invWindowSize.x;
		float vyNorm = vy * invWindowSize.y;
		float v2 = vxNorm * vxNorm + vyNorm * vyNorm;
#define VMAX 0.013f
		if(v2>VMAX*VMAX) v2 = VMAX*VMAX;
		float satInc = mass > 0.5 ? mass * mass * mass : 0;
		satInc *= satInc * satInc * satInc;
		//color.setHSV(0, ofMap(v2, 0, VMAX*VMAX, 0, 1) + satInc, ofLerp(0.5, 1, mass) * alpha);
		Color color( CM_HSV, 0, v2 / ( VMAX * VMAX ), lerp( 0.5f, 1.0f, mass ) * alpha );

		colBuffer[ci++] = color.r;
		colBuffer[ci++] = color.g;
		colBuffer[ci++] = color.b;
		colBuffer[ci++] = color.r;
		colBuffer[ci++] = color.g;
		colBuffer[ci++] = color.b;
	}
}
