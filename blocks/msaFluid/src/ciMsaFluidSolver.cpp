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

#include "ciMsaFluidSolver.h"
#include "cinder/Rand.h"

ciMsaFluidSolver::ciMsaFluidSolver()
:r(NULL)
,rOld(NULL)
,g(NULL)
,gOld(NULL)
,b(NULL)
,bOld(NULL)
,uv(NULL)
,uvOld(NULL)
,curl(NULL)
,_isInited(false)
{
}

ciMsaFluidSolver& ciMsaFluidSolver::setSize(int NX, int NY)
{
	_NX = NX;
	_NY = NY;
	_numCells = (_NX + 2) * (_NY + 2);
	
	_invNX = 1.0f / _NX;
	_invNY = 1.0f / _NY;
	_invNumCells = 1.0f / _numCells;
	
	width           = getWidth();
	height          = getHeight();
	invWidth        = 1.0f/width;
	invHeight       = 1.0f/height;
	
	reset();
	return *this;
}


ciMsaFluidSolver& ciMsaFluidSolver::setup(int NX, int NY)
{
	setDeltaT();
	setFadeSpeed();
	setSolverIterations();
	enableVorticityConfinement(false);
	setWrap( false, false );
	
	//maa
	viscocity =  FLUID_DEFAULT_VISC;
	colorDiffusion = FLUID_DEFAULT_COLOR_DIFFUSION;
	
	return setSize( NX, NY);
}

ciMsaFluidSolver&  ciMsaFluidSolver::setDeltaT(float dt) {
	_dt = dt;
	return *this;	
}

ciMsaFluidSolver&  ciMsaFluidSolver::setFadeSpeed(float fadeSpeed) {
	this->fadeSpeed = fadeSpeed;
	return *this;	
}

ciMsaFluidSolver&  ciMsaFluidSolver::setSolverIterations(int solverIterations) {
	this->solverIterations = solverIterations;
	return *this;	
}


// whether fluid is RGB or monochrome (if only pressure / velocity is needed no need to update 3 channels)
ciMsaFluidSolver&  ciMsaFluidSolver::enableRGB(bool doRGB) {
	this->doRGB = doRGB;
	return *this;
}

ciMsaFluidSolver&  ciMsaFluidSolver::enableVorticityConfinement(bool b) {
	doVorticityConfinement = b;
	return *this;
}

bool ciMsaFluidSolver::getVorticityConfinement() {
	return doVorticityConfinement;
}

ciMsaFluidSolver& ciMsaFluidSolver::setWrap( bool bx, bool by ) {
	wrap_x = bx;
	wrap_y = by;
	return *this;
}

bool ciMsaFluidSolver::isInited() const {
	return _isInited;
}

ciMsaFluidSolver::~ciMsaFluidSolver() {
	destroy();
}

void ciMsaFluidSolver::destroy() {
	_isInited = false;
	
	if(r)		delete []r;
	if(rOld)	delete []rOld;
	
	if(g)		delete []g;
	if(gOld)	delete []gOld;
	
	if(b)		delete []b;
	if(bOld)	delete []bOld;
	
	if(uv)		delete []uv;
	if(uvOld)	delete []uvOld;
	if(curl)       delete []curl;
}


void ciMsaFluidSolver::reset() {
	destroy();
	_isInited = true;
	
	r    = new float[_numCells];
	rOld = new float[_numCells];
	
	g    = new float[_numCells];
	gOld = new float[_numCells];
	
	b    = new float[_numCells];
	bOld = new float[_numCells];
	
	uv    = new ci::Vec2f[_numCells];
	uvOld = new ci::Vec2f[_numCells];
	curl = new float[_numCells];
	
	for ( int i = _numCells-1; i>=0; --i )
	{
		uv[i] = uvOld[i] = ci::Vec2f::zero();
		curl[i] = 0.0f;
		r[i] = rOld[i] = g[i] = gOld[i] = b[i] = bOld[i] = 0;
	}
}

// return total number of cells (_NX+2) * (_NY+2)
int ciMsaFluidSolver::getNumCells() const {
	return _numCells;
}

int ciMsaFluidSolver::getWidth() const {
	return _NX + 2;
}


int ciMsaFluidSolver::getHeight() const {
	return _NY + 2;
}


ciMsaFluidSolver& ciMsaFluidSolver::setVisc(float newVisc) {
	viscocity = newVisc;
	return *this;
}

// returns current viscocity
float ciMsaFluidSolver::getVisc() const {
	return viscocity;
}

ciMsaFluidSolver& ciMsaFluidSolver::setColorDiffusion( float diff )
{
	colorDiffusion = diff;
	return *this;
}

float	ciMsaFluidSolver::getColorDiffusion()
{
	return colorDiffusion;
}

// returns average density of fluid 
float ciMsaFluidSolver::getAvgDensity() const {
	return _avgDensity;
}

// returns average uniformity
float ciMsaFluidSolver::getUniformity() const {
	return _uniformity;
}

float ciMsaFluidSolver::getAvgSpeed() const {
	return _avgSpeed;
}

#ifndef	SWAP
template<class T> void SWAP( T& a, T& b)
{
	T tmp = b;
	b = a;
	a = tmp;
}
#endif

void ciMsaFluidSolver::swapR()		{	SWAP( r, rOld );	}
void ciMsaFluidSolver::swapRGB(){ 
	SWAP( r, rOld );
	SWAP( g, gOld );
	SWAP( b, bOld );
}
void ciMsaFluidSolver::swapUV()	{	SWAP( uv, uvOld ); }

// Curl and vorticityConfinement based on code by Alexander McKenzie
float ciMsaFluidSolver::calcCurl( int i, int j)
{
	float du_dy = uv[FLUID_IX(i, j + 1)].x - uv[FLUID_IX(i, j - 1)].x;
	float dv_dx = uv[FLUID_IX(i + 1, j)].y - uv[FLUID_IX(i - 1, j)].y;
	return (du_dy - dv_dx) * 0.5f;	// for optimization should be moved to later and done with another operation
}

void ciMsaFluidSolver::vorticityConfinement(ci::Vec2f* Fvc_xy) {
	float dw_dx, dw_dy;
	float length;
	float v;
	
	// Calculate magnitude of calcCurl(u,v) for each cell. (|w|)
	for (int j = _NY; j > 0; --j )
	{
		for (int i = _NX; i > 0; --i )
		{
			curl[FLUID_IX(i, j)] = fabs(calcCurl(i, j));
		}
	}
	
	for (int j = _NY-1; j > 1; --j )	//for (int j = 2; j < _NY; j++)
	{
		for (int i = _NX-1; i > 1; --i )		//for (int i = 2; i < _NX; i++)		
		{
			// Find derivative of the magnitude (_N = del |w|)
			dw_dx = (curl[FLUID_IX(i + 1, j)] - curl[FLUID_IX(i - 1, j)]);	// was * 0.5f; now done later with 2./lenght
			dw_dy = (curl[FLUID_IX(i, j + 1)] - curl[FLUID_IX(i, j - 1)]);	// was * 0.5f;
			
			// Calculate vector length. (|_N|)
			// Add small factor to prevent divide by zeros.
			length = (float) sqrt(dw_dx * dw_dx + dw_dy * dw_dy) + 0.000001f;
			
			// N = ( _N/|_N| )
			length = 2./length;	// the 2. come from the previous * 0.5
			dw_dx *= length;
			dw_dy *= length;
			
			v = calcCurl(i, j);
			
			// N x w
			Fvc_xy[FLUID_IX(i, j)].x = dw_dy * -v;
			Fvc_xy[FLUID_IX(i, j)].y = dw_dx *  v;
		}
	}
}

void ciMsaFluidSolver::update() {
	addSourceUV();
	
	if( doVorticityConfinement )
	{
		vorticityConfinement(uvOld);
		addSourceUV();
	}
	
	swapUV();
	
	diffuseUV( viscocity );
	
	project(uv, uvOld);
	
	swapUV();
	
	advect2d(uv, uvOld);
	
	project(uv, uvOld);
	
	if(doRGB)
	{
		addSourceRGB();
		swapRGB();
		
		if( colorDiffusion!=0. && _dt!=0. )
		{
			diffuseRGB(0, colorDiffusion );
			swapRGB();
		}
		
		advectRGB(0, uv);
		fadeRGB();
	} 
	else
	{
		addSource(r, rOld);
		swapR();
		
		if( colorDiffusion!=0. && _dt!=0. )
		{
			diffuse(0, r, rOld, colorDiffusion );
			swapRGB();
		}
		
		advect(0, r, rOld, uv);	
		fadeR();
	}
}

#define ZERO_THRESH		1e-9			// if value falls under this, set to zero (to avoid denormal slowdown)
#define CHECK_ZERO(p)	if(fabsf(p)<ZERO_THRESH) p = 0

void ciMsaFluidSolver::fadeR() {
	// I want the fluid to gradually fade out so the screen doesn't fill. the amount it fades out depends on how full it is, and how uniform (i.e. boring) the fluid is...
	//		float holdAmount = 1 - _avgDensity * _avgDensity * fadeSpeed;	// this is how fast the density will decay depending on how full the screen currently is
	float holdAmount = 1 - fadeSpeed;
	
	_avgDensity = 0;
	_avgSpeed = 0;
	
	float totalDeviations = 0;
	float currentDeviation;
	float tmp_r;
	//	float uniformityMult = uniformity * 0.05f;
	
	_avgSpeed = 0;
	for (int i = _numCells-1; i >=0; --i) {
		// clear old values
		uvOld[i] = ci::Vec2f::zero(); 
		rOld[i] = 0;
		//		gOld[i] = bOld[i] = 0;
		
		// calc avg speed
		_avgSpeed += uv[i].x * uv[i].x + uv[i].y * uv[i].y;
		
		// calc avg density
		tmp_r = ci::math<float>::min( 1.0f, r[i] );
		
		//		g[i] = MIN(1.0f, g[i]);
		//		b[i] = MIN(1.0f, b[i]);
		//		float density = MAX(r[i], MAX(g[i], b[i]));
		_avgDensity += tmp_r;	// add it up
		
		// calc deviation (for uniformity)
		currentDeviation = tmp_r - _avgDensity;
		totalDeviations += currentDeviation * currentDeviation;
		
		// fade out old
		r[i] = tmp_r * holdAmount;
		
		CHECK_ZERO(r[i]);
		CHECK_ZERO(uv[i].x);
		CHECK_ZERO(uv[i].y);
		if(doVorticityConfinement) CHECK_ZERO(curl[i]);
		
	}
	_avgDensity *= _invNumCells;
	//	_avgSpeed *= _invNumCells;
	
	//	println("%.3f\n", _avgSpeed);
	_uniformity = 1.0f / (1 + totalDeviations * _invNumCells);		// 0: very wide distribution, 1: very uniform
}


void ciMsaFluidSolver::fadeRGB() {
	// I want the fluid to gradually fade out so the screen doesn't fill. the amount it fades out depends on how full it is, and how uniform (i.e. boring) the fluid is...
	//		float holdAmount = 1 - _avgDensity * _avgDensity * fadeSpeed;	// this is how fast the density will decay depending on how full the screen currently is
	float holdAmount = 1 - fadeSpeed;
	
	_avgDensity = 0;
	_avgSpeed = 0;
	
	float totalDeviations = 0;
	float currentDeviation;
	//	float uniformityMult = _uniformity * 0.05f;
	float tmp_r, tmp_g, tmp_b;
	_avgSpeed = 0;
	for (int i = _numCells-1; i >=0; --i)
	{
		// clear old values
		uvOld[i] = ci::Vec2f::zero();
		rOld[i] = 0;
		gOld[i] = bOld[i] = 0;
		
		// calc avg speed
		_avgSpeed += uv[i].x * uv[i].x + uv[i].y * uv[i].y;
		
		// calc avg density
		tmp_r = ci::math<float>::min( 1.0f, r[i] );
		tmp_g = ci::math<float>::min( 1.0f, g[i] );
		tmp_b = ci::math<float>::min( 1.0f, b[i] );

		float density = ci::math<float>::max( tmp_r, ci::math<float>::max( tmp_g, tmp_b ) );
		_avgDensity += density;	// add it up
		
		// calc deviation (for _uniformity)
		currentDeviation = density - _avgDensity;
		totalDeviations += currentDeviation * currentDeviation;
		
		// fade out old
		r[i] = tmp_r * holdAmount;
		g[i] = tmp_g * holdAmount;
		b[i] = tmp_b * holdAmount;
		
		CHECK_ZERO(r[i]);
		CHECK_ZERO(g[i]);
		CHECK_ZERO(b[i]);
		CHECK_ZERO(uv[i].x);
		CHECK_ZERO(uv[i].y);
		if(doVorticityConfinement) CHECK_ZERO(curl[i]);
	}
	_avgDensity *= _invNumCells;
	_avgSpeed *= _invNumCells;
	
	//println("%.3f\n", _avgDensity);
	_uniformity = 1.0f / (1 + totalDeviations * _invNumCells);		// 0: very wide distribution, 1: very uniform
}


void ciMsaFluidSolver::addSourceUV()
{
	for (int i = _numCells-1; i >=0; --i)
	{
		uv[i].x += _dt * uvOld[i].x;
		uv[i].y += _dt * uvOld[i].y;
	}
}

void ciMsaFluidSolver::addSourceRGB()
{
	for (int i = _numCells-1; i >=0; --i)
	{
		r[i] += _dt * rOld[i];
		g[i] += _dt * gOld[i];
		b[i] += _dt * bOld[i];		
	}
}

void ciMsaFluidSolver::addSource(float* x, float* x0) {
	for (int i = _numCells-1; i >=0; --i)
	{
		x[i] += _dt * x0[i];
	}
}

void ciMsaFluidSolver::advect( int bound, float* d, const float* d0, const ci::Vec2f* duv) {
	int i0, j0, i1, j1;
	float x, y, s0, t0, s1, t1;
	int	index;
	
	const float dt0x = _dt * _NX;
	const float dt0y = _dt * _NY;
	
	for (int j = _NY; j > 0; --j)
	{
		for (int i = _NX; i > 0; --i)
		{
			index = FLUID_IX(i, j);
			x = i - dt0x * duv[index].x;
			y = j - dt0y * duv[index].y;
			
			if (x > _NX + 0.5) x = _NX + 0.5f;
			if (x < 0.5)     x = 0.5f;
			
			i0 = (int) x;
			i1 = i0 + 1;
			
			if (y > _NY + 0.5) y = _NY + 0.5f;
			if (y < 0.5)     y = 0.5f;
			
			j0 = (int) y;
			j1 = j0 + 1;
			
			s1 = x - i0;
			s0 = 1 - s1;
			t1 = y - j0;
			t0 = 1 - t1;
			
			d[index] = s0 * (t0 * d0[FLUID_IX(i0, j0)] + t1 * d0[FLUID_IX(i0, j1)])
						+ s1 * (t0 * d0[FLUID_IX(i1, j0)] + t1 * d0[FLUID_IX(i1, j1)]);
			
		}
	}
	setBoundary(bound, d);
}

//          d    d0    du    dv
// advect(1, u, uOld, uOld, vOld);
// advect(2, v, vOld, uOld, vOld);
void ciMsaFluidSolver::advect2d( ci::Vec2f *uv, const ci::Vec2f *duv ) {
	int i0, j0, i1, j1;
	float s0, t0, s1, t1;
	int	index;
	
	const float dt0x = _dt * _NX;
	const float dt0y = _dt * _NY;
	
	for (int j = _NY; j > 0; --j)
	{
		for (int i = _NX; i > 0; --i)
		{
			index = FLUID_IX(i, j);
			float x = i - dt0x * duv[index].x;
			float y = j - dt0y * duv[index].y;
			
			if (x > _NX + 0.5) x = _NX + 0.5f;
			if (x < 0.5)     x = 0.5f;
			
			i0 = (int) x;
			i1 = i0 + 1;
			
			if (y > _NY + 0.5) y = _NY + 0.5f;
			if (y < 0.5)     y = 0.5f;
			
			j0 = (int) y;
			j1 = j0 + 1;
			
			s1 = x - i0;
			s0 = 1 - s1;
			t1 = y - j0;
			t0 = 1 - t1;
			
			uv[index].x = s0 * (t0 * duv[FLUID_IX(i0, j0)].x + t1 * duv[FLUID_IX(i0, j1)].x)
						+ s1 * (t0 * duv[FLUID_IX(i1, j0)].x + t1 * duv[FLUID_IX(i1, j1)].x);
			uv[index].y = s0 * (t0 * duv[FLUID_IX(i0, j0)].y + t1 * duv[FLUID_IX(i0, j1)].y)
						+ s1 * (t0 * duv[FLUID_IX(i1, j0)].y + t1 * duv[FLUID_IX(i1, j1)].y);
			
		}
	}
	setBoundary2d(1, uv);
	setBoundary2d(2, uv);	
}

void ciMsaFluidSolver::advectRGB(int bound, const ci::Vec2f* duv) {
	int i0, j0;
	float x, y, s0, t0, s1, t1, dt0x, dt0y;
	int	index;
	
	dt0x = _dt * _NX;
	dt0y = _dt * _NY;
	
	for (int j = _NY; j > 0; --j)
	{
		for (int i = _NX; i > 0; --i)
		{
			index = FLUID_IX(i, j);
			x = i - dt0x * duv[index].x;
			y = j - dt0y * duv[index].y;
			
			if (x > _NX + 0.5) x = _NX + 0.5f;
			if (x < 0.5)     x = 0.5f;
			
			i0 = (int) x;
			
			if (y > _NY + 0.5) y = _NY + 0.5f;
			if (y < 0.5)     y = 0.5f;
			
			j0 = (int) y;
			
			s1 = x - i0;
			s0 = 1 - s1;
			t1 = y - j0;
			t0 = 1 - t1;
			
			i0 = FLUID_IX(i0, j0);	//we don't need col/row index any more but index in 1 dimension
			j0 = i0 + (_NX + 2);
			r[index] = s0 * ( t0 * rOld[i0] + t1 * rOld[j0] ) + s1 * ( t0 * rOld[i0+1] + t1 * rOld[j0+1] );
			g[index] = s0 * ( t0 * gOld[i0] + t1 * gOld[j0] ) + s1 * ( t0 * gOld[i0+1] + t1 * gOld[j0+1] );                  
			b[index] = s0 * ( t0 * bOld[i0] + t1 * bOld[j0] ) + s1 * ( t0 * bOld[i0+1] + t1 * bOld[j0+1] );                          
		}
	}
	setBoundaryRGB();
}

void ciMsaFluidSolver::diffuse( int bound, float* c, float* c0, float diff )
{
	float a = _dt * diff * _NX * _NY;	//todo find the exact strategy for using _NX and _NY in the factors
	linearSolver( bound, c, c0, a, 1.0 + 4 * a );
}

void ciMsaFluidSolver::diffuseRGB( int bound, float diff )
{
	float a = _dt * diff * _NX * _NY;
	linearSolverRGB( a, 1.0 + 4 * a );
}

void ciMsaFluidSolver::diffuseUV( float diff )
{
	float a = _dt * diff * _NX * _NY;
	linearSolverUV( a, 1.0 + 4 * a );
}

void ciMsaFluidSolver::project(ci::Vec2f* xy, ci::Vec2f* pDiv) 
{
	float	h;
	int		index;
	int		step_x = _NX + 2;
	
	h = - 0.5f / _NX;
	for (int j = _NY; j > 0; --j)
	{
		index = FLUID_IX(_NX, j);
		for (int i = _NX; i > 0; --i)
		{
			pDiv[index].x = h * ( xy[index+1].x - xy[index-1].x + xy[index+step_x].y - xy[index-step_x].y );
			pDiv[index].y = 0;
			--index;
		}
	}
	
	setBoundary02d( reinterpret_cast<ci::Vec2f*>( &pDiv[0].x ));
	setBoundary02d( reinterpret_cast<ci::Vec2f*>( &pDiv[0].y ));
	
	linearSolverProject( pDiv );
	
	float fx = 0.5f * _NX;
	float fy = 0.5f * _NY;	//maa	change it from _NX to _NY
	for (int j = _NY; j > 0; --j)
	{
		index = FLUID_IX(_NX, j);
		for (int i = _NX; i > 0; --i)
		{
			xy[index].x -= fx * (pDiv[index+1].x - pDiv[index-1].x);
			xy[index].y -= fy * (pDiv[index+step_x].x - pDiv[index-step_x].x);
			--index;
		}
	}
	
	setBoundary2d(1, xy);
	setBoundary2d(2, xy);
}


//	Gauss-Seidel relaxation
void ciMsaFluidSolver::linearSolver( int bound, float* __restrict x, const float* __restrict x0, float a, float c )
{
	int	step_x = _NX + 2;
	int index;
	c = 1. / c;
	for (int k = solverIterations; k > 0; --k)	// MEMO 
	{
		for (int j = _NY; j > 0 ; --j)
		{
			index = FLUID_IX(_NX, j );
			for (int i = _NX; i > 0 ; --i)
			{
				x[index] = ( ( x[index-1] + x[index+1] + x[index - step_x] + x[index + step_x] ) * a + x0[index] ) * c;
				--index;
			}
		}
		setBoundary( bound, x );
	}
}

void ciMsaFluidSolver::linearSolverProject( ci::Vec2f* __restrict pdiv )
{
	int	step_x = _NX + 2;
	int index;
	for (int k = solverIterations; k > 0; --k) {
		for (int j = _NY; j > 0 ; --j) {
			index = FLUID_IX(_NX, j );
			float prev = pdiv[index+1].x;
			for (int i = _NX; i > 0 ; --i)
			{
				prev = ( pdiv[index-1].x + prev + pdiv[index - step_x].x + pdiv[index + step_x].x + pdiv[index].y ) * .25;
				pdiv[index].x = prev;
				--index;				
			}
		}
		setBoundary02d( reinterpret_cast<ci::Vec2f*>( &pdiv[0].x ) );
	}
}

void ciMsaFluidSolver::linearSolverRGB( float a, float c )
{
	int index3, index4, index;
	int	step_x = _NX + 2;
	c = 1. / c;
	for ( int k = solverIterations; k > 0; --k )	// MEMO
	{           
		for (int j = _NY; j > 0 ; --j)
		{
			index = FLUID_IX(_NX, j );
			//index1 = index - 1;		//FLUID_IX(i-1, j);
			//index2 = index + 1;		//FLUID_IX(i+1, j);
			index3 = index - step_x;	//FLUID_IX(i, j-1);
			index4 = index + step_x;	//FLUID_IX(i, j+1);
			for (int i = _NX; i > 0 ; --i)
			{	
				r[index] = ( ( r[index-1] + r[index+1]  +  r[index3] + r[index4] ) * a  +  rOld[index] ) * c;
				g[index] = ( ( g[index-1] + g[index+1]  +  g[index3] + g[index4] ) * a  +  gOld[index] ) * c;
				b[index] = ( ( b[index-1] + b[index+1]  +  b[index3] + b[index4] ) * a  +  bOld[index] ) * c;                                
				//				x[FLUID_IX(i, j)] = (a * ( x[FLUID_IX(i-1, j)] + x[FLUID_IX(i+1, j)]  +  x[FLUID_IX(i, j-1)] + x[FLUID_IX(i, j+1)])  +  x0[FLUID_IX(i, j)]) / c;
				--index;
				--index3;
				--index4;
			}
		}
		setBoundaryRGB();	
	}
}

void ciMsaFluidSolver::linearSolverUV( float a, float c )
{
	int index;
	int	step_x = _NX + 2;
	c = 1. / c;
	ci::Vec2f* __restrict localUV = uv;
	const ci::Vec2f* __restrict localOldUV = uvOld;

	for (int k = solverIterations; k > 0; --k)	// MEMO
	{           
		for (int j = _NY; j > 0 ; --j)
		{
			index = FLUID_IX(_NX, j );
			float prevU = localUV[index+1].x;
			float prevV = localUV[index+1].y;
			for (int i = _NX; i > 0 ; --i)
			{
				prevU = ( ( localUV[index-1].x + prevU + localUV[index - step_x].x + localUV[index + step_x].x ) * a  + localOldUV[index].x ) * c;
				prevV = ( ( localUV[index-1].y + prevV + localUV[index - step_x].y + localUV[index + step_x].y ) * a  + localOldUV[index].y ) * c;
				localUV[index].x = prevU;
				localUV[index].y = prevV;
				--index;
			}
		}
		setBoundary2d( 1, uv );
	}
}

// specifies simple boundry conditions.
void ciMsaFluidSolver::setBoundary(int bound, float* x)
{
	int dst1, dst2, src1, src2;
	int step = FLUID_IX(0, 1) - FLUID_IX(0, 0);

	dst1 = FLUID_IX(0, 1);
	src1 = FLUID_IX(1, 1);
	dst2 = FLUID_IX(_NX+1, 1 );
	src2 = FLUID_IX(_NX, 1);
	if( wrap_x )
		SWAP( src1, src2 );
	if( bound == 1 && !wrap_x )
		for (int i = _NY; i > 0; --i )
		{
			x[dst1] = -x[src1];	dst1 += step;	src1 += step;	
			x[dst2] = -x[src2];	dst2 += step;	src2 += step;	
		}
	else
		for (int i = _NY; i > 0; --i )
		{
			x[dst1] = x[src1];	dst1 += step;	src1 += step;	
			x[dst2] = x[src2];	dst2 += step;	src2 += step;	
		}

	dst1 = FLUID_IX(1, 0);
	src1 = FLUID_IX(1, 1);
	dst2 = FLUID_IX(1, _NY+1);
	src2 = FLUID_IX(1, _NY);
	if( wrap_y )
		SWAP( src1, src2 );
	if( bound == 2 && !wrap_y )
		for (int i = _NX; i > 0; --i )
		{
			x[dst1++] = -x[src1++];	
			x[dst2++] = -x[src2++];	
		}
	else
		for (int i = _NX; i > 0; --i )
		{
			x[dst1++] = x[src1++];
			x[dst2++] = x[src2++];	
		}
	
	x[FLUID_IX(  0,   0)] = 0.5f * (x[FLUID_IX(1, 0  )] + x[FLUID_IX(  0, 1)]);
	x[FLUID_IX(  0, _NY+1)] = 0.5f * (x[FLUID_IX(1, _NY+1)] + x[FLUID_IX(  0, _NY)]);
	x[FLUID_IX(_NX+1,   0)] = 0.5f * (x[FLUID_IX(_NX, 0  )] + x[FLUID_IX(_NX+1, 1)]);
	x[FLUID_IX(_NX+1, _NY+1)] = 0.5f * (x[FLUID_IX(_NX, _NY+1)] + x[FLUID_IX(_NX+1, _NY)]);
}

void ciMsaFluidSolver::setBoundary02d(ci::Vec2f* x)
{
	int dst1, dst2, src1, src2;
	int step = FLUID_IX(0, 1) - FLUID_IX(0, 0);

	dst1 = FLUID_IX(0, 1);
	src1 = FLUID_IX(1, 1);
	dst2 = FLUID_IX(_NX+1, 1 );
	src2 = FLUID_IX(_NX, 1);
	if( wrap_x )
		SWAP( src1, src2 );
	for (int i = _NY; i > 0; --i )
	{
		x[dst1].x = x[src1].x;	dst1 += step;	src1 += step;	
		x[dst2].x = x[src2].x;	dst2 += step;	src2 += step;	
	}

	dst1 = FLUID_IX(1, 0);
	src1 = FLUID_IX(1, 1);
	dst2 = FLUID_IX(1, _NY+1);
	src2 = FLUID_IX(1, _NY);
	if( wrap_y )
		SWAP( src1, src2 );
	for (int i = _NX; i > 0; --i )
	{
		x[dst1++] = x[src1++];
		x[dst2++] = x[src2++];	
	}
	
	x[FLUID_IX(  0,   0)].x = 0.5f * (x[FLUID_IX(1, 0  )].x + x[FLUID_IX(  0, 1)].x);
	x[FLUID_IX(  0, _NY+1)].x = 0.5f * (x[FLUID_IX(1, _NY+1)].x + x[FLUID_IX(  0, _NY)].x);
	x[FLUID_IX(_NX+1,   0)].x = 0.5f * (x[FLUID_IX(_NX, 0  )].x + x[FLUID_IX(_NX+1, 1)].x);
	x[FLUID_IX(_NX+1, _NY+1)].x = 0.5f * (x[FLUID_IX(_NX, _NY+1)].x + x[FLUID_IX(_NX+1, _NY)].x);
}

void ciMsaFluidSolver::setBoundary2d( int bound, ci::Vec2f *xy )
{
	int dst1, dst2, src1, src2;
	int step = FLUID_IX(0, 1) - FLUID_IX(0, 0);

	dst1 = FLUID_IX(0, 1);
	src1 = FLUID_IX(1, 1);
	dst2 = FLUID_IX(_NX+1, 1 );
	src2 = FLUID_IX(_NX, 1);
	if( wrap_x )
		SWAP( src1, src2 );
	if( bound == 1 && !wrap_x )
		for (int i = _NY; i > 0; --i )
		{
			xy[dst1].x = -xy[src1].x;	dst1 += step;	src1 += step;	
			xy[dst2].x = -xy[src2].x;	dst2 += step;	src2 += step;	
		}
	else
		for (int i = _NY; i > 0; --i )
		{
			xy[dst1].x = xy[src1].x;	dst1 += step;	src1 += step;	
			xy[dst2].x = xy[src2].x;	dst2 += step;	src2 += step;	
		}

	dst1 = FLUID_IX(1, 0);
	src1 = FLUID_IX(1, 1);
	dst2 = FLUID_IX(1, _NY+1);
	src2 = FLUID_IX(1, _NY);
	if( wrap_y )
		SWAP( src1, src2 );
	if( bound == 2 && !wrap_y )
		for (int i = _NX; i > 0; --i )
		{
			xy[dst1++].y = -xy[src1++].y;	
			xy[dst2++].y = -xy[src2++].y;	
		}
	else
		for (int i = _NX; i > 0; --i )
		{
			xy[dst1++].y = xy[src1++].y;
			xy[dst2++].y = xy[src2++].y;	
		}
	
	xy[FLUID_IX(  0,   0)][bound-1] = 0.5f * (xy[FLUID_IX(1, 0  )][bound-1] + xy[FLUID_IX(  0, 1)][bound-1]);
	xy[FLUID_IX(  0, _NY+1)][bound-1] = 0.5f * (xy[FLUID_IX(1, _NY+1)][bound-1] + xy[FLUID_IX(  0, _NY)][bound-1]);
	xy[FLUID_IX(_NX+1,   0)][bound-1] = 0.5f * (xy[FLUID_IX(_NX, 0  )][bound-1] + xy[FLUID_IX(_NX+1, 1)][bound-1]);
	xy[FLUID_IX(_NX+1, _NY+1)][bound-1] = 0.5f * (xy[FLUID_IX(_NX, _NY+1)][bound-1] + xy[FLUID_IX(_NX+1, _NY)][bound-1]);
}

#define CPY_RGB( d, s )		{	r[d] = r[s];	g[d] = g[s];	b[d] = b[s]; }
#define CPY_RGB_NEG( d, s )	{	r[d] = -r[s];	g[d] = -g[s];	b[d] = -b[s]; }

// specifies simple boundry conditions.
void ciMsaFluidSolver::setBoundaryRGB()
{
	int dst1, dst2, src1, src2;
	int step = FLUID_IX(0, 1) - FLUID_IX(0, 0);
	
	dst1 = FLUID_IX(0, 1);
	src1 = FLUID_IX(1, 1);
	dst2 = FLUID_IX(_NX+1, 1 );
	src2 = FLUID_IX(_NX, 1);
	if( wrap_x )
		SWAP( src1, src2 );
		for (int i = _NY; i > 0; --i )
		{
			CPY_RGB( dst1, src1 );	dst1 += step;	src1 += step;	
			CPY_RGB( dst2, src2 );	dst2 += step;	src2 += step;	
		}
	
	dst1 = FLUID_IX(1, 0);
	src1 = FLUID_IX(1, 1);
	dst2 = FLUID_IX(1, _NY+1);
	src2 = FLUID_IX(1, _NY);
	if( wrap_y )
		SWAP( src1, src2 );
		for (int i = _NX; i > 0; --i )
		{
		CPY_RGB( dst1, src1 );	++dst1;	++src1;	
		CPY_RGB( dst2, src2 );	++dst2;	++src2;	
		}
	
	//	x[FLUID_IX(  0,   0)] = 0.5f * (x[FLUID_IX(1, 0  )] + x[FLUID_IX(  0, 1)]);
	//	x[FLUID_IX(  0, _NY+1)] = 0.5f * (x[FLUID_IX(1, _NY+1)] + x[FLUID_IX(  0, _NY)]);
	//	x[FLUID_IX(_NX+1,   0)] = 0.5f * (x[FLUID_IX(_NX, 0  )] + x[FLUID_IX(_NX+1, 1)]);
	//	x[FLUID_IX(_NX+1, _NY+1)] = 0.5f * (x[FLUID_IX(_NX, _NY+1)] + x[FLUID_IX(_NX+1, _NY)]);
	
}

void ciMsaFluidSolver::randomizeColor() {
	for (int i = getWidth()-1; i > 0; --i)
	{
		for (int j = getHeight()-1; j > 0; --j)
		{
			int index = FLUID_IX(i, j);
			r[index] = rOld[index] = ci::Rand::randFloat();
			if(doRGB) {
				g[index] = gOld[index] = ci::Rand::randFloat();
				b[index] = bOld[index] = ci::Rand::randFloat();
			}
		} 
	}
}

