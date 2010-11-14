#pragma once
#include "cinder/CinderResources.h"

#define RES_DUCKY_MESH		CINDER_RESOURCE( ../resources/,	ducky.msh, 128, MESH )
#define RES_DUCKY_TEX		CINDER_RESOURCE( ../resources/, ducky.png, 128, IMAGE )
#define RES_SHADER_FRAG		CINDER_RESOURCE( ../resources/, phong_frag.glsl, 128, GLSL )
#define RES_SHADER_VERT		CINDER_RESOURCE( ../resources/, phong_vert.glsl, 129, GLSL )
