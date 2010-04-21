#include "BulletCube.h"

void BulletCube::create() 
{
	btCollisionShape * box				= new btBoxShape(btVector3(mSize,mSize,mSize));
	btDefaultMotionState * motionState	= new btDefaultMotionState(btTransform(btQuaternion(Rand::randFloat(2.0),1,1,1).normalize(),btVector3(0,mHeight,0)));
	
	btVector3 inertia(0,0,0);
	box->calculateLocalInertia( mMass, inertia );
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI( mMass, motionState, box, inertia );
	
	rigidBody = new btRigidBody(rigidBodyCI);
	mDynamicsWorld->addRigidBody(rigidBody);
}

void BulletCube::draw() {
	btTransform trans;
	rigidBody->getMotionState()->getWorldTransform(trans);
	
	glPushMatrix();
	fli::gl::translate( Vec3f( trans.getOrigin() ) );
	fli::gl::rotate( Quatf( trans.getRotation() ) );
	fli::gl::drawCube( Vec3f::zero(), Vec3f( mSize, mSize, mSize ) * 2 );
	glPopMatrix();
}

