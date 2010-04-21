#pragma once

#include "resource.h"
#include "flint/app/App.h"
#include "Particle.h"
#include "Emitter.h"
#include "Module.h"

class HodginParticlesApp : public Module {
 public:	
	void setup( fli::app::App *app );
	void update( fli::app::App *app, bool visible );
	void draw( fli::app::App *app, const fli::Vec4d &camLoc );
	
	Emitter emitter;
};
