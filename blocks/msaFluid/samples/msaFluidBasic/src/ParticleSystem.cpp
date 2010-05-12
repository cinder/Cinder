/*
 *  ParticleSystem.cpp
 *
 *  Created by Mehmet Akten on 02/05/2009.
 *  Copyright 2009 MSA Visuals Ltd.. All rights reserved.
 *
 */

#include "ParticleSystem.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"
#include "cinder/app/App.h"

using namespace ci;

ParticleSystem::ParticleSystem() 
{
	curIndex = 0;
	setWindowSize( Vec2i( 1, 1 ) );
}

void ParticleSystem::setWindowSize( Vec2i winSize )
{
	windowSize = winSize;
	invWindowSize = Vec2f( 1.0f / winSize.x, 1.0f / winSize.y );
}

void ParticleSystem::updateAndDraw( bool drawingFluid ){
	glEnable(GL_BLEND);
	glDisable( GL_TEXTURE_2D );
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);       
	
	for(int i=0; i<MAX_PARTICLES; i++) {
		if(particles[i].alpha > 0) {
			particles[i].update( *solver, windowSize, invWindowSize );
			particles[i].updateVertexArrays( drawingFluid, invWindowSize, i, posArray, colArray);
		}
	}    
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, posArray);
	
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(3, GL_FLOAT, 0, colArray);
	
	glDrawArrays(GL_LINES, 0, MAX_PARTICLES * 2);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glDisable(GL_BLEND);
}


void ParticleSystem::addParticles(float x, float y, int count ){
	for(int i=0; i<count; i++)
		addParticle(x + Rand::randFloat(-15, 15), y + Rand::randFloat(-15, 15));
}


void ParticleSystem::addParticle(float x, float y) {
	particles[curIndex].init(x, y);
	curIndex++;
	if(curIndex >= MAX_PARTICLES) curIndex = 0;
}
