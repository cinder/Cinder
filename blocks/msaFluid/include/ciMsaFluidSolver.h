/***********************************************************************
 
 * this is a class for solving real-time fluid dynamics simulations based on Navier-Stokes equations 
 * and code from Jos Stam's paper "Real-Time Fluid Dynamics for Games" http://www.dgp.toronto.edu/people/stam/reality/Research/pdf/GDC03.pdf
 * Other useful resources and implementations I looked at while building this lib: 
 * Mike Ash (C) - http://mikeash.com/?page=pyblog/fluid-simulation-for-dummies.html
 * Alexander McKenzie (Java) - http://www.multires.caltech.edu/teaching/demos/java/stablefluids.htm
 * Pierluigi Pesenti (AS3 port of Alexander's) - http://blog.oaxoa.com/2008/01/21/actionscript-3-fluids-simulation/
 * Gustav Taxen (C) - http://www.nada.kth.se/~gustavt/fluids/
 * Dave Wallin (C++) - http://nuigroup.com/touchlib/ (uses portions from Gustav's)
 
 
 /***********************************************************************
 
 Copyright (c) 2008, 2009, Memo Akten, www.memo.tv
 *** The Mega Super Awesome Visuals Company ***
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of MSA Visuals nor the names of its contributors 
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE. 
 *
 * ***********************************************************************/ 
//	April-Mai 2009 optimized and extended by Maa (http://www.lagraine.com/ - new content coming soon)

 /* Portions Copyright (c) 2010, The Cinder Project, http://libcinder.org */

#pragma once

#include "cinder/Vector.h"
#include "cinder/Color.h"

// do not change these values, you can override them using the solver methods
#define		FLUID_DEFAULT_NX					100
#define		FLUID_DEFAULT_NY					100
#define     FLUID_DEFAULT_DT				.04f	//Maa	25fps
#define		FLUID_DEFAULT_VISC					0.0001f
#define     FLUID_DEFAULT_COLOR_DIFFUSION	0
#define     FLUID_DEFAULT_FADESPEED         .03
#define		FLUID_DEFAULT_SOLVER_ITERATIONS		10

#define		FLUID_IX(i, j)		((i) + (_NX + 2)  *(j))

class ciMsaFluidSolver {
public:	
	ciMsaFluidSolver();
	virtual ~ciMsaFluidSolver();
	
	ciMsaFluidSolver& setup(int NX = FLUID_DEFAULT_NX, int NY = FLUID_DEFAULT_NY);
	ciMsaFluidSolver& setSize(int NX = FLUID_DEFAULT_NX, int NY = FLUID_DEFAULT_NY);
	
	// solve one step of the fluid solver
	void update();
	
	// clear all forces in fluid and reset
	void reset();	
	
	// get index for cell position
	inline int getIndexForCellPosition(int x, int y) const;
	
	// get index for normalized position
	inline int getIndexForNormalizedPosition(float x, float y) const;
	inline int getIndexForNormalizedPosition( const ci::Vec2f &pos ) const { return getIndexForNormalizedPosition( pos.x, pos.y ); }
	
	// get color and/vel at any point in the fluid.
	// pass pointers to ci::Vec2f (for velocity) and ci::Vec2f (for color) and they get filled with the info
	// leave any pointer NULL if you don't want that info
	
	// get info at normalized (x, y) coordinates. range :(0..1), (0..1)
	inline void getInfoAtPos(float x, float y, ci::Vec2f *vel, ci::Color *color = NULL) const;
	
	inline ci::Vec2f getVelocityAtPos( const ci::Vec2f &pos ) const;
	
	// get info at fluid cell pixels (i, j) if you know it. range: (0..NX-1), (0..NY-1)
	inline	void getInfoAtCell(int i, int j, ci::Vec2f *vel, ci::Color *color = NULL) const;
	
	// get info at fluid cell index if you know it. range: (0..numCells)
	inline	void getInfoAtCell(int index, ci::Vec2f *vel, ci::Color *color = NULL) const;
	
	// add force at normalized (x, y) coordinates
	inline void addForceAtPos( const ci::Vec2f &pos, const ci::Vec2f &force );
	
	// add force at (i, j) fluid cell coordinates
	inline	void addForceAtCell(int i, int j, const ci::Vec2f &force);
	
	// add color at normalized (x, y) coordinates
	// r, g, b: normalized R, G, B components of color to inject
	// for monochrome, g & b are ignore, only r is used
	inline void addColorAtPos(float x, float y, float r, float g=0, float b=0);
	inline void addColorAtPos( const ci::Vec2f &pos, const ci::Color &color ) { addColorAtPos( pos.x, pos.y, color.r, color.g, color.b ); }
	
	// add color and force at (i, j) fluid cell coordinates
	// r, g, b: normalized R, G, B components of color to inject
	// for monochrome, g & b are ignore, only r is used
	inline void addColorAtCell(int i, int j, float r, float g=0, float b=0 );
	inline void addColorAtCell(int i, int j, float* rgb );
	
	// fill with random color at every cell
	void randomizeColor();
		
	// return number of cells and dimensions
	int getNumCells() const;
	int getWidth() const;
	int getHeight() const;
	
	bool isInited() const;
	
	// accessors for  viscocity, it will lerp to the target at lerpspeed
	ciMsaFluidSolver& setVisc(float newVisc); 
	float getVisc() const;
	
	// accessors for  color diffusion
	// if diff == 0, color diffusion is not performed
	// ** COLOR DIFFUSION IS SLOW!
	ciMsaFluidSolver& setColorDiffusion( float diff );
	float				getColorDiffusion();
	
	ciMsaFluidSolver& enableRGB(bool isRGB);
	ciMsaFluidSolver& setDeltaT(float dt = FLUID_DEFAULT_DT);
	ciMsaFluidSolver& setFadeSpeed(float fadeSpeed = FLUID_DEFAULT_FADESPEED);
	ciMsaFluidSolver& setSolverIterations(int solverIterations = FLUID_DEFAULT_SOLVER_ITERATIONS);
	ciMsaFluidSolver& enableVorticityConfinement(bool b);
	bool getVorticityConfinement();
	ciMsaFluidSolver& setWrap( bool bx, bool by );
	
	// returns average density of fluid 
	float getAvgDensity() const;
	
	// returns average _uniformity
	float getUniformity() const;
	
	// returns average speed of fluid
	float getAvgSpeed() const;

  protected:			
	// allocate an array large enough to hold information for u, v, r, g, OR b
	float* alloc()	{ return new float[_numCells];	}

	float	*r, *rOld;
	float	*g, *gOld;
	float	*b, *bOld;
	
	ci::Vec2f	*uv, *uvOld;

	float	*curl;
	
	bool	doRGB;				// for monochrome, only update r
	bool	doVorticityConfinement;
	int		solverIterations;
	
	float	colorDiffusion;
	float	viscocity;
	float	fadeSpeed;
	
	bool	wrap_x;
	bool	wrap_y;
	
	float	width;
	float	height;
	float	invWidth;
	float	invHeight;

	
	int		_NX, _NY, _numCells;
	float	_invNX, _invNY, _invNumCells;
	float	_dt;
	bool	_isInited;
	float	*_tmp;
	
	float	_avgDensity;			// this will hold the average color of the last frame (how full it is)
	float	_uniformity;			// this will hold the _uniformity of the last frame (how uniform the color is);
	float	_avgSpeed;
	
	void	destroy();
	
	inline	float	calcCurl(int i, int j);
	void	vorticityConfinement(ci::Vec2f *Fvc_xy);
	
	void	addSource(float *x, float *x0);
	void	addSourceUV();		// does both U and V in one go
	void	addSourceRGB();	// does R, G, and B in one go
	
	void	advect(int b, float *d, const float *d0, const ci::Vec2f *duv);
	void	advect2d( ci::Vec2f *uv, const ci::Vec2f *duv );
	void	advectRGB(int b, const ci::Vec2f *duv);
	
	void	diffuse(int b, float *c, float *c0, float diff);
	void	diffuseRGB(int b, float diff);
	void	diffuseUV(float diff);
	
	void	project(ci::Vec2f *xy, ci::Vec2f *pDiv);
	void	linearSolver(int b, float *x, const float *x0, float a, float c);
	void	linearSolverProject( ci::Vec2f *pdiv );
	void	linearSolverRGB( float a, float c);
	void	linearSolverUV(float a, float c);
	
	void	setBoundary(int b, float *x);
	void	setBoundary02d(ci::Vec2f* x);
	void	setBoundary2d(int b, ci::Vec2f *xy );
	void	setBoundaryRGB();
	
	void	swapUV();
	void	swapU(); 
	void	swapV(); 
	void	swapR();
	void	swapRGB();
	
	void	fadeR();
	void	fadeRGB();
};


// the functions below are here for optimization purposes
 
inline int ciMsaFluidSolver::getIndexForCellPosition(int i, int j) const {
	if(i < 1) i=1; else if(i > _NX) i = _NX;
	if(j < 1) j=1; else if(j > _NY) j = _NY;
	return FLUID_IX(i, j);
}

inline int ciMsaFluidSolver::getIndexForNormalizedPosition(float x, float y) const {
	return getIndexForCellPosition((int)floor(x * (_NX+2)), (int)floor(y * (_NY+2)));
}

inline	void ciMsaFluidSolver::getInfoAtCell(int i, ci::Vec2f *vel, ci::Color *color) const {
	if(vel)
		vel->set(uv[i].x * _invNX, uv[i].y * _invNY);
	if(color)
	{
		if(doRGB)
			color->set( ci::CM_RGB, ci::Vec3f( r[i], g[i], b[i] ) );
		else
			color->set( ci::CM_RGB, ci::Vec3f( r[i], r[i], r[i] ) );
	}
}

inline void ciMsaFluidSolver::getInfoAtPos(float x, float y, ci::Vec2f *vel, ci::Color *color) const {
	int i= (int)(x * (_NX+2));
	int j= (int)(y * (_NY+2));
	getInfoAtCell(i, j, vel, color);
}

inline ci::Vec2f ciMsaFluidSolver::getVelocityAtPos( const ci::Vec2f &pos ) const {
	int i = (int)(pos.x * (_NX+2));
	int j = (int)(pos.y * (_NY+2));
	i = ci::constrain<int>( i, 0, _NX+1 );
	j = ci::constrain<int>( j, 0, _NY+1 );
	int o = FLUID_IX( i, j );
	return ci::Vec2f( uv[o].x, uv[o].y );	
}

inline	void ciMsaFluidSolver::getInfoAtCell(int i, int j, ci::Vec2f *vel, ci::Color *color) const {
	if(i<0) i = 0; else if(i > _NX+1) i = _NX+1;
	if(j<0) j = 0; else if(j > _NY+1) j = _NY+1;
	getInfoAtCell(FLUID_IX(i, j), vel, color);
}

inline void ciMsaFluidSolver::addForceAtPos(const ci::Vec2f &pos, const ci::Vec2f &force ) {
	int i = (int) (pos.x * _NX + 1);
	if( i<0 || _NX+1<i ) return;
	int j = (int) (pos.y * _NY + 1);
	if( j<0 || _NY+1<j ) return;
	addForceAtCell( i, j, force );
}

inline	void ciMsaFluidSolver::addForceAtCell(int i, int j, const ci::Vec2f &force )
{
	int index = FLUID_IX(i, j);
	uv[index].x += force.x;
	uv[index].y += force.y;
}

inline void ciMsaFluidSolver::addColorAtCell(int i, int j, float r, float g, float b )
{
	//      if(safeToRun()){
	int index = FLUID_IX(i, j);
	rOld[index] += r;
	if(doRGB)
	{
		gOld[index] += g;
		bOld[index] += b;
	}
	//              unlock();
	//      }
}

inline void ciMsaFluidSolver::addColorAtPos(float x, float y, float r, float g, float b) {
	int i = (int) (x * _NX + 1);
	if( i<0 || _NX+1<i ) return;
	int j = (int) (y * _NY + 1);
	if( j<0 || _NY+1<j ) return;
	addColorAtCell(i, j, r, g, b);	
}


inline void ciMsaFluidSolver::addColorAtCell(int i, int j, float* rgb )
{
	addColorAtCell( i, j, rgb[0], rgb[1], rgb[2] );
}

