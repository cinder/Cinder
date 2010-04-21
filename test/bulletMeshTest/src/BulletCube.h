/*
 *  BulletCube.h
 *  bulletMeshTest
 *
 *  Created by Keith Butters on 11/17/08.
 *  Copyright 2008 the barbarian group. All rights reserved.
 *
 */

#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>
#include "flint/app/App.h"
#include "flint/Rand.h"
#include "flint/Surface.h"
#include "flint/gl/GLSLProg.h"
#include "flint/bullet/FlintBullet.h"

#include <btBulletDynamicsCommon.h>

using namespace fli;
using namespace fli::app;

class BulletCube {
public:
	BulletCube( btDiscreteDynamicsWorld	* aWorld, float aSize, float aHeight, gl::GLSLProg * aShader)
	: mDynamicsWorld(aWorld), mSize(aSize), mHeight(aHeight), mMass(aSize*aSize*aSize), shader(aShader){}
	
	void create();
	void draw();
	
	gl::GLSLProg * shader;
	float mSize;
	float mHeight;
	btScalar mMass;
	btRigidBody	* rigidBody;
	btDiscreteDynamicsWorld	* mDynamicsWorld;
};

