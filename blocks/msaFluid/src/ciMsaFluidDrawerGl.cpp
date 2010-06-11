/***********************************************************************

 This class is for drawing a fluidsolver using the openFrameworks texture

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

#include "ciMsaFluidDrawerGl.h"

using namespace cinder;

#define FLUID_TEXTURE

ciMsaFluidDrawerGl::ciMsaFluidDrawerGl() {
	//	printf("ciMsaFluidDrawerGl::ciMsaFluidDrawer()\n");
	_pixels				= NULL;
	_byteCount			= 0;
	_fluidSolver		= NULL;
	_didICreateTheFluid	= false;
	alpha				= 1;
	doInvert			= false;
	
	enableAlpha(false);
	
	setDrawMode(FLUID_DRAW_COLOR);
}

ciMsaFluidDrawerGl::~ciMsaFluidDrawerGl() {
	//	printf("ciMsaFluidDrawerGl::~ciMsaFluidDrawer()\n");
	deleteFluidSolver();
}



ciMsaFluidSolver* ciMsaFluidDrawerGl::setup(int NX, int NY) {
	//	printf("ciMsaFluidDrawerGl::createFluidSolver(%i, %i, %.6f, %.6f, %.6f)\n", NX, NY, dt, visc, fadeSpeed);
	deleteFluidSolver();
	_fluidSolver = new ciMsaFluidSolver;
	_fluidSolver->setup(NX, NY);
	createTexture();

	return _fluidSolver;
}

ciMsaFluidSolver* ciMsaFluidDrawerGl::setup(ciMsaFluidSolver* f) {
	deleteFluidSolver();
	_fluidSolver = f;
	createTexture();

	return _fluidSolver;
}

ciMsaFluidSolver* ciMsaFluidDrawerGl::getFluidSolver() {
	return _fluidSolver;
}

void ciMsaFluidDrawerGl::enableAlpha(bool b) {
	_alphaEnabled = b;
	if(_alphaEnabled) {
		_glType = GL_RGBA;
		_bpp = 4;
	} else {
		_glType = GL_RGB;
		_bpp = 3;
	}
}

void ciMsaFluidDrawerGl::createTexture() {
	if(_pixels) delete []_pixels;
	int texWidth = _fluidSolver->getWidth()-2;
	int texHeight =_fluidSolver->getHeight()-2;

	_pixels = new unsigned char[texWidth * texHeight * _bpp];
	_surface = Surface8u( _pixels, texWidth, texHeight, false, SurfaceChannelOrder::RGB );

#ifdef FLUID_TEXTURE
	gl::Texture::Format format;
	format.setInternalFormat( _glType );
	tex = gl::Texture( texWidth, texHeight, format );
#endif
}


void ciMsaFluidDrawerGl::reset() {
	if(!isFluidReady()) {
		printf("ciMsaFluidDrawerGl::reset() - Fluid not ready\n");
		return;
	}
	_fluidSolver->reset();
}

void ciMsaFluidDrawerGl::update() {
	if(!isFluidReady()) {
		printf("ciMsaFluidDrawerGl::updateFluid() - Fluid not ready\n");
		return;
	}
	_fluidSolver->update();
}

void ciMsaFluidDrawerGl::setDrawMode(int newDrawMode) {
	drawMode = newDrawMode;
	if(drawMode < 0) drawMode = FLUID_DRAW_MODE_COUNT-1;
	else if(drawMode >= FLUID_DRAW_MODE_COUNT) drawMode = 0;
}

void ciMsaFluidDrawerGl::incDrawMode() {
	setDrawMode(drawMode + 1);
}

void ciMsaFluidDrawerGl::decDrawMode() {
	setDrawMode(drawMode - 1);
}

int ciMsaFluidDrawerGl::getDrawMode() {
	return drawMode;
}

const char* ciMsaFluidDrawerGl::getDrawModeName() {
	switch(drawMode) {
		case 0:
			return "FLUID_DRAW_COLOR";
		case 1:
			return "FLUID_DRAW_MOTION";
		case 2:
			return "FLUID_DRAW_SPEED";
		case 3:
			return "FLUID_DRAW_VECTORS";
		default:
			return "FLUID DRAW MODE NOT FOUND";
	}
}

void ciMsaFluidDrawerGl::draw(float x, float y, float renderWidth, float renderHeight) {
	switch(drawMode) {
		case FLUID_DRAW_COLOR:
			drawColor(x, y, renderWidth, renderHeight);
		break;

		case FLUID_DRAW_MOTION:
			drawMotion(x, y, renderWidth, renderHeight);
		break;

		case FLUID_DRAW_SPEED:
			drawSpeed(x, y, renderWidth, renderHeight);
		break;

		case FLUID_DRAW_VECTORS:
			drawVectors(x, y, renderWidth, renderHeight);
		break;

	}
}

void ciMsaFluidDrawerGl::drawColor(float x, float y, float renderWidth, float renderHeight, bool withAlpha) {
	int fw = _fluidSolver->getWidth();
	int fh = _fluidSolver->getHeight();

	ci::Vec2f vel;
	ci::Color color;
	int index = 0;
	for(int j=1; j < fh-1; j++) {
		for(int i=1; i < fw-1; i++) {
			_fluidSolver->getInfoAtCell(i, j, &vel, &color);
			uint8_t r = (uint8_t)math<float>::min(color.r * 255 * alpha, 255);
			uint8_t g = (uint8_t)math<float>::min(color.g * 255 * alpha, 255);
			uint8_t b = (uint8_t)math<float>::min(color.b * 255 * alpha, 255);
			if(doInvert) {
				r = 255 - r;
				g = 255 - g;
				b = 255 - b;
			}
			_pixels[index++] = r;
			_pixels[index++] = g;
			_pixels[index++] = b;
			if(_alphaEnabled) _pixels[index++] = withAlpha ? math<uint8_t>::min(b, math<uint8_t>::max(r, g)) : 255;
		}
	}

#ifdef FLUID_TEXTURE
	tex.update( _surface );
	gl::draw( tex, Rectf( x, y, x + renderWidth, y + renderHeight ) );
#endif
}

void ciMsaFluidDrawerGl::drawMotion(float x, float y, float renderWidth, float renderHeight, bool withAlpha) {
	int fw = _fluidSolver->getWidth();
	int fh = _fluidSolver->getHeight();

	ci::Vec2f vel;
	int index = 0;
	for(int j=1; j < fh-1; j++) {
		for(int i=1; i < fw-1; i++) {
			_fluidSolver->getInfoAtCell(i, j, &vel, NULL);
			float speed2 = fabs(vel.x) * fw + fabs(vel.y) * fh;
			int speed = (int)math<float>::min(speed2 * 255 * alpha, 255);
			_pixels[index++] = (uint8_t)math<float>::min(fabs(vel.x) * fw * 255 * alpha, 255);
			_pixels[index++] = (uint8_t)math<float>::min(fabs(vel.y) * fh * 255 * alpha, 255);
			_pixels[index++] = (uint8_t)0;
			if(_alphaEnabled) _pixels[index++] = withAlpha ? speed : 255;

		}
	}

#ifdef FLUID_TEXTURE
	tex.update( _surface );
	gl::draw( tex, Rectf( x, y, x + renderWidth, y + renderHeight ) );
#endif
}


void ciMsaFluidDrawerGl::drawSpeed(float x, float y, float renderWidth, float renderHeight, bool withAlpha) {
	int fw = _fluidSolver->getWidth();
	int fh = _fluidSolver->getHeight();

	ci::Vec2f vel;
	int index = 0;
	for(int j=1; j < fh-1; j++) {
		for(int i=1; i < fw-1; i++) {
			_fluidSolver->getInfoAtCell(i, j, &vel, NULL);
			float speed2 = fabs(vel.x) * fw + fabs(vel.y) * fh;
			uint8_t speed = (uint8_t)math<float>::min(speed2 * 255 * alpha, 255);
			_pixels[index++] = speed;
			_pixels[index++] = speed;
			_pixels[index++] = speed;
			if(_alphaEnabled) _pixels[index++] = withAlpha ? speed : 255;
		}
	}

#ifdef FLUID_TEXTURE
	tex.update( _surface );
	gl::draw( tex, Rectf( x, y, x + renderWidth, y + renderHeight ) );
#endif
}


void ciMsaFluidDrawerGl::drawVectors(float x, float y, float renderWidth, float renderHeight, float velThreshold) {
	int fw = _fluidSolver->getWidth();
	int fh = _fluidSolver->getHeight();

//	int xStep = renderWidth / 10;		// every 10 pixels
//	int yStep = renderHeight / 10;		// every 10 pixels

	glPushMatrix();
	glTranslatef(x, y, 0);
	glScalef(renderWidth/(fw-2), renderHeight/(fh-2), 1.0);

	float velMult = 50000;
	float maxVel = 5.0f/20000;

	ci::Vec2f vel;
	glEnable(GL_LINE_SMOOTH);
	glLineWidth(1);
	for (int j=0; j<fh-2; j++ ){
		for (int i=0; i<fw-2; i++ ){
			_fluidSolver->getInfoAtCell(i+1, j+1, &vel, NULL);
			float d2 = vel.x * vel.x + vel.y * vel.y;
			if(d2>velThreshold) {
				if(d2 > maxVel * maxVel) {
					float mult = maxVel * maxVel/ d2;
	//				float mult = (d2 - maxVel * maxVel) / d2;
					vel.x *= mult;
					vel.y *= mult;
				}
				vel *= velMult;

	#ifndef CINDER_GLES
					glBegin(GL_LINES);
					glColor3f(0, 0, 0); glVertex2f(i, j);
					glColor3f(1, 1, 1); glVertex2f(i + vel.x, j + vel.y);
					glEnd();
	#endif
			}
		}
	}
	glPopMatrix();

}

void ciMsaFluidDrawerGl::deleteFluidSolver() {
	//	printf("ciMsaFluidDrawerGl::deleteFluidSolver()\n");
	if(_fluidSolver && _didICreateTheFluid) {
		delete _fluidSolver;
		_fluidSolver = NULL;

		if(_pixels) delete []_pixels;
		_pixels = NULL;

#ifdef FLUID_TEXTURE
		tex.reset();
#endif
	}
}

bool ciMsaFluidDrawerGl::isFluidReady() {
	if(!_fluidSolver) {
		printf("ciMsaFluidDrawerGl::isFluidReady() - No fluid solver\n");
		return false;
	}

	if(!_fluidSolver->isInited()) {
		printf("ciMsaFluidDrawerGl::isFluidReady() - Fluid solver not initialized yet, call init()\n");
		return false;
	}

	return true;
}

