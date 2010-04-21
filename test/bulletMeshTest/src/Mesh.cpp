/*
 *  Mesh.cpp
 *  bulletMeshTest
 *
 *  Created by Keith Butters on 11/7/08.
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
#include "flint/gl/Texture.h"
#include "flint/gl/GLSLProg.h"
#include "flint/bullet/FlintBullet.h"

#include <btBulletDynamicsCommon.h>
#include "BulletSoftBody/btSoftBody.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"

using namespace fli;
using namespace fli::app;

class Mesh {
public:
	Mesh( btSoftRigidDynamicsWorld * aWorld, gl::GLSLProg * aShader ) 
	: mWorld(aWorld), shader(aShader){}
	
	void create ()
	{
		
		const btScalar s = 10;	
		psb = btSoftBodyHelpers::CreatePatch(mWorld->getWorldInfo(), 
														  btVector3(-s,0,-s),
														  btVector3(+s,0,-s),
														  btVector3(-s,0,+s),
														  btVector3(+s,0,+s),
														  31,
														  31,
														  1+2+4+8,
														  true);
		
		btSoftBody::Material * pm = psb->appendMaterial();
		pm->m_kLST		=	0.4;
		pm->m_flags		-=	btSoftBody::fMaterial::DebugDraw;
		psb->generateBendingConstraints(2,pm);
		psb->setTotalMass(150);
		mWorld->addSoftBody(psb);
	}
	
	void draw( bool normals ) {
		drawMesh();
		if( normals )
			drawNormals();
	}
	
	void drawMesh()  {
		glBegin( GL_TRIANGLES );
		for( int i = 0; i < psb->m_faces.size(); ++i ) {
			const btSoftBody::Face& f = psb->m_faces[i];
			glNormal3f( -f.m_normal.x(), -f.m_normal.y(), -f.m_normal.z() );
			glVertex3f( f.m_n[0]->m_x.x(), f.m_n[0]->m_x.y(), f.m_n[0]->m_x.z() );
			glVertex3f( f.m_n[1]->m_x.x(), f.m_n[1]->m_x.y(), f.m_n[1]->m_x.z() );
			glVertex3f( f.m_n[2]->m_x.x(), f.m_n[2]->m_x.y(), f.m_n[2]->m_x.z() );
		}
		glEnd();
	}
	
	void drawNormals() {
		glDisable( GL_LIGHTING );
		glColor3f( 1.0f, 1.0f, 0.0f );
		glBegin( GL_LINES );
		for( int i = 0; i < psb->m_faces.size(); ++i ) {
			const btSoftBody::Face& f = psb->m_faces[i];
			Vec3f v1( f.m_n[0]->m_x );
			Vec3f v2( f.m_n[1]->m_x );
			Vec3f v3( f.m_n[2]->m_x );
			Vec3f normal( f.m_normal );
			gl::vertex( ( v1 + v2 + v3 ) / 3.0f );
			gl::vertex( ( v1 + v2 + v3 ) / 3.0f + Vec3f( -normal.x, -normal.y, -normal.z ) );
		}
		glEnd();
	}
	
	btSoftRigidDynamicsWorld * mWorld;
	btSoftBody * psb;
	
	gl::GLSLProg * shader;
	
	int mSize;
	GLfloat	mMesh[20][20][3];
};


