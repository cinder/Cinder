/*
 *  BulletCloth.h
 *  bulletMeshTest
 *
 *  Created by Keith Butters on 11/17/08.
 *  Copyright 2008 the barbarian group. All rights reserved.
 *
 */


#include "BulletSoftBody/btSoftBody.h"
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"

class BulletCloth {
public:
	BulletCloth( btSoftRigidDynamicsWorld * aWorld, int xPatches = 40, int yPatches = 40 ) 
	: mWorld(aWorld), mXPatches(xPatches), mYPatches(yPatches){};
	
	void create ();
	void draw( bool normals, bool vnormals );
	void drawCloth();
	void drawNormals();
	void drawVertexNormals();
	
	int mSize;
	
	int mXPatches;
	int mYPatches;
	
	btSoftRigidDynamicsWorld * mWorld;
	btSoftBody * psb;
};


