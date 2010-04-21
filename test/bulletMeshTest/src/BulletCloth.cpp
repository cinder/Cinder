/*
 *  Mesh.cpp
 *  bulletMeshTest
 *
 *  Created by Keith Butters on 11/7/08.
 *  Copyright 2008 the barbarian group. All rights reserved.
 *
 */

#include "btBulletDynamicsCommon.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"

#include "flint/bullet/FlintBullet.h"
#include "BulletCloth.h"
#include "flint/gl/gl.h"

using namespace fli;

void BulletCloth::create ()
{
	const btScalar s = 20;
	const btScalar j = 20;
	psb = btSoftBodyHelpers::CreatePatch(mWorld->getWorldInfo(), 
													  btVector3(+s,0,+j),
													  btVector3(-s,0,+j),
													  btVector3(+s,0,-j),
													  btVector3(-s,0,-j),
													  mXPatches,
													  mYPatches,
													  1+2+4+8,
													  true);
	
	btSoftBody::Material * pm = psb->appendMaterial();
	pm->m_kLST		=	0.4;
	pm->m_kAST		=	0.99;
	pm->m_flags		-=	btSoftBody::fMaterial::DebugDraw;
	psb->generateBendingConstraints(2,pm);
	psb->setTotalMass(150);
	mWorld->addSoftBody(psb);
}

void BulletCloth::draw( bool normals, bool vnormals ) {
	drawCloth();
	if( normals )
		drawNormals();
	if ( vnormals )
		drawVertexNormals();
}

void BulletCloth::drawCloth()  {
	
	int nodeIdx;
	float x, y;
	
	glBegin( GL_TRIANGLES );
	for( int i = 0; i < psb->m_faces.size(); ++i ) {
		const btSoftBody::Face& f = psb->m_faces[i];
		
		nodeIdx = ( f.m_n[0] - &(psb->m_nodes[0]) );
		x = ( nodeIdx % ( mXPatches ) ) / (float)( mXPatches - 1 );
		y = ((int)( nodeIdx / ( mXPatches ) )) / (float)( mYPatches - 1 );				
		glTexCoord2f( 1.0f - x, 1.0 - y );
		glNormal3f( -f.m_n[0]->m_n.x(), -f.m_n[0]->m_n.y(), -f.m_n[0]->m_n.z() );
		glVertex3f( f.m_n[0]->m_x.x(), f.m_n[0]->m_x.y(), f.m_n[0]->m_x.z() );

		nodeIdx = ( f.m_n[1] - &(psb->m_nodes[0]) );
		x = ( nodeIdx % ( mXPatches ) ) / (float)( mXPatches - 1 );
		y = ((int)( nodeIdx / ( mXPatches ) )) / (float)( mYPatches - 1 );		
		glTexCoord2f( 1.0f - x, 1.0 - y );
		glNormal3f( -f.m_n[1]->m_n.x(), -f.m_n[1]->m_n.y(), -f.m_n[1]->m_n.z() );
		glVertex3f( f.m_n[1]->m_x.x(), f.m_n[1]->m_x.y(), f.m_n[1]->m_x.z() );

		nodeIdx = ( f.m_n[2] - &(psb->m_nodes[0]) );
		x = ( nodeIdx % ( mXPatches ) ) / (float)( mXPatches - 1 );
		y = ((int)( nodeIdx / ( mXPatches ) )) / (float)( mYPatches - 1 );				
		glTexCoord2f( 1.0f - x, 1.0 - y );
		glNormal3f( -f.m_n[2]->m_n.x(), -f.m_n[2]->m_n.y(), -f.m_n[2]->m_n.z() );
		glVertex3f( f.m_n[2]->m_x.x(), f.m_n[2]->m_x.y(), f.m_n[2]->m_x.z() );
	}
	glEnd();
	
	glDisable( GL_TEXTURE_GEN_S );
}


void BulletCloth::drawNormals() {
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

void BulletCloth::drawVertexNormals() {

	glDisable( GL_LIGHTING );
	glColor3f( 0.0f, 1.0f, 1.0f );
	glBegin( GL_LINES );
	//	Node::m_x => position
	//  Node::m_n => normal
	for( int i = 0; i < psb->m_nodes.size(); ++i ) {
		const btSoftBody::Node& n = psb->m_nodes[i];
		Vec3f vert = Vec3f( n.m_x.x(), n.m_x.y(), n.m_x.z() );
		gl::vertex( vert );
		gl::vertex( -Vec3f( n.m_n.x(), n.m_n.y(), n.m_n.z() ) + vert );
	}
	glEnd();
}

