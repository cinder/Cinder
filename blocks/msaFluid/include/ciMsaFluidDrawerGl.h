/***********************************************************************
 
 This class is for drawing a fluidsolver using the OpenFrameworks texture
 
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

/* Portions Copyright (c) 2010, The Cinder Project, http://libcinder.org */

#pragma once
#include "cinder/gl/Texture.h"
#include "ciMsaFluidSolver.h"

#define FLUID_DRAW_COLOR		0
#define FLUID_DRAW_MOTION		1
#define FLUID_DRAW_SPEED		2
#define FLUID_DRAW_VECTORS		3
#define FLUID_DRAW_MODE_COUNT	4

#define FLUID_TEXTURE


class ciMsaFluidDrawerGl {
  public:
	float alpha;
	bool doInvert;
	
	ciMsaFluidDrawerGl();
	virtual ~ciMsaFluidDrawerGl();
	
	ciMsaFluidSolver* setup(int NX = FLUID_DEFAULT_NX, int NY = FLUID_DEFAULT_NY);
	ciMsaFluidSolver* setup(ciMsaFluidSolver* f);
	ciMsaFluidSolver* getFluidSolver();
	
	void enableAlpha(bool b);
	
	void update();
	
	virtual void draw(float x, float y, float renderWidth, float renderHeight);				// this one does chooses one of the below based on drawmode
	void drawColor(float x, float y, float renderWidth, float renderHeight, bool withAlpha = false);
	void drawMotion(float x, float y, float renderWidth, float renderHeight, bool withAlpha = false);
	void drawSpeed(float x, float y, float renderWidth, float renderHeight, bool withAlpha = false);
	void drawVectors(float x, float y, float renderWidth, float renderHeight, float velThreshold = 0);
	void reset();
	
	float getWidth() {
#ifdef FLUID_TEXTURE
		return tex.getWidth();
#endif
	}
	
	float getHeight() {
#ifdef FLUID_TEXTURE
		return tex.getHeight();
#endif
	}
	
	void setDrawMode(int newDrawMode);
	void incDrawMode();
	void decDrawMode();
	int getDrawMode();
	const char* getDrawModeName();

	
	int					drawMode;

protected:	
	ci::Surface8u		_surface;						// pixels array to be drawn
	ci::uint8_t			*_pixels;
	int					_byteCount;						// number of byes in the pixel array (size * 3)

#ifdef FLUID_TEXTURE
	int					_glType;						// GL_RGB or GL_RGBA
	bool				_alphaEnabled;
	int					_bpp;							// 3 or 4
	ci::gl::Texture		tex;
#endif	
	
	ciMsaFluidSolver	*_fluidSolver;
	bool				_didICreateTheFluid;
	
	virtual void		createTexture();
	
	void deleteFluidSolver();
	bool isFluidReady();
	
};
