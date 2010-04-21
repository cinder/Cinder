#pragma once
#include "cinder/CinderResources.h"

#define RES_NORMAL_MAP_VERT			CINDER_RESOURCE( ../shaders/, normalMap.vert, 128, GLSL )
#define RES_NORMAL_MAP_FRAG			CINDER_RESOURCE( ../shaders/, normalMap.frag, 129, GLSL )
#define RES_DIFFUSE_MAP				CINDER_RESOURCE( ../resources/, diffuseMap_3.jpg, 130, IMAGE )
#define RES_SPEC_STRENGTH_MAP		CINDER_RESOURCE( ../resources/, specStrengthMap_3.jpg, 131, IMAGE )
#define RES_NORMAL_MAP_PNG			CINDER_RESOURCE( ../resources/, normalMap_3.png, 132, IMAGE )
