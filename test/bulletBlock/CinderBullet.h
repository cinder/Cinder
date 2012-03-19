/*
 *  CinderBullet.h
 *  fallingBodies
 *
 *  Created by Peter Holzkorn on 2/16/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "btBulletDynamicsCommon.h"
#include "bullet/src/BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h"
#include "cinder/app/AppBasic.h"
#include "cinder/TriMesh.h"
#include "cinder/AxisAlignedBox.h"

const float PI = 3.14159265;

namespace cinder { 
	
//	class TriMesh;
	
	namespace bullet {

	Matrix44f getWorldTransform(const btRigidBody* body)
	{
		btTransform trans;
		body->getMotionState()->getWorldTransform(trans);
		Matrix44f matrix;
		trans.getOpenGLMatrix(matrix.m);
		return matrix;
	}
	
	inline btVector3 toBulletVector3(const Vec3f &v)
	{
		return btVector3(v.x, v.y, v.z);
	}
	
	inline btQuaternion toBulletQuaternion(const Quatf &q)
	{
		return btQuaternion(q.v.x, q.v.y, q.v.z, q.w);
	}
	
	btHeightfieldTerrainShape* createHeightfieldTerrainShape(Channel32f* chan, int stickWidth, int stickLength, float heightScale, float minHeight, float maxHeight, int upAxis, Vec3f scale)
	{
		btHeightfieldTerrainShape* hfShape = new btHeightfieldTerrainShape(	stickWidth, stickLength, chan->getData(), 
																			heightScale, minHeight, maxHeight, upAxis, PHY_FLOAT, false);
		
		hfShape->setLocalScaling(toBulletVector3(scale));
		return hfShape;
		
	}
	

		
	btBvhTriangleMeshShape* createStaticConcaveMeshShape(TriMesh mesh, Vec3f scale, float margin=0.05f)
	{
		std::vector<Vec3f> vertices = mesh.getVertices();
		std::vector<uint32_t> indices = mesh.getIndices();
		
		btTriangleMesh *tmesh = new btTriangleMesh(true, false);
		
		for(int i = 0; i < mesh.getNumTriangles(); i += 3)
		{
			tmesh->addTriangle(bullet::toBulletVector3(vertices[indices[i]]), 
								bullet::toBulletVector3(vertices[indices[i+1]]), 
								bullet::toBulletVector3(vertices[indices[i+2]]), 
								true);
		}

		btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(tmesh, true, true);
		shape->setLocalScaling(toBulletVector3(scale));
		shape->setMargin(margin);
		
		return shape;
	}
		
	btRigidBody* createStaticRigidBody(btDynamicsWorld* world, btCollisionShape* shape, Vec3f position)
	{
		btDefaultMotionState *motionState = new btDefaultMotionState(btTransform(bullet::toBulletQuaternion(Quatf()),toBulletVector3((position))));
		btRigidBody::btRigidBodyConstructionInfo bodyCI(0,motionState,shape,btVector3(0,0,0));
		btRigidBody* body = new btRigidBody(bodyCI);
		world->addRigidBody(body);
		
		return body;
		
	}
	
	btConvexHullShape* createConvexHullShape(TriMesh mesh, Vec3f scale)
	{
		std::vector<Vec3f> vertices = mesh.getVertices();
		
		btConvexHullShape *btHull = new btConvexHullShape();
		
		for(int i = 0; i < mesh.getNumVertices(); i++)
			btHull->addPoint(bullet::toBulletVector3(vertices[i]));
		
		btHull->setLocalScaling(toBulletVector3(scale));
		
		return btHull;
	}
	
	btRigidBody* createConvexHullBody(btDynamicsWorld* world, btConvexHullShape* shape, Vec3f position, float mass)
	{
		btDefaultMotionState *motionState = new btDefaultMotionState(btTransform(toBulletQuaternion(Quatf()),toBulletVector3(position)));
		btRigidBody::btRigidBodyConstructionInfo bodyCI(mass,motionState,shape,btVector3(0,0,0));
		btRigidBody* body = new btRigidBody(bodyCI);
		world->addRigidBody(body);
		
		return body;
	}
	
	btRigidBody* createBox(btDynamicsWorld *world, Vec3f size, Quatf rotation, Vec3f position)
	{
		btCollisionShape *box = new btBoxShape(toBulletVector3(size) / 2.0f);
		btDefaultMotionState *motionState = new btDefaultMotionState(btTransform(toBulletQuaternion(rotation),toBulletVector3(position)));
		
		btVector3 inertia(0,0,0);
		float mass = size.x * size.y * size.z;
		box->calculateLocalInertia(mass, inertia);
		btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, box, inertia);
		
		btRigidBody *rigidBody = new btRigidBody(rigidBodyCI);
		world->addRigidBody(rigidBody);
		
		return rigidBody;
	}
	
	btRigidBody* createSphere(btDynamicsWorld *world, float radius, Quatf rotation, Vec3f position)
	{
		btCollisionShape *sphere = new btSphereShape((btScalar) radius);
		btDefaultMotionState *motionState = new btDefaultMotionState(btTransform(toBulletQuaternion(rotation),toBulletVector3(position)));
	
		btVector3 inertia(0,0,0);
		float mass = radius * radius * radius * PI * 4.0f/3.0f;
		sphere->calculateLocalInertia(mass, inertia);
		btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, sphere, inertia);
		
		btRigidBody *rigidBody = new btRigidBody(rigidBodyCI);
		world->addRigidBody(rigidBody);
		
		return rigidBody;
	}
	
		btRigidBody* create(btDynamicsWorld *world, AxisAlignedBox3f box, Quatf rotation, Vec3f position)
	{
		const Vec3f& min = box.getMin();
		const Vec3f& max = box.getMax();
		
	//	Vec3d position = Vec3f( (min.x + max.x)/2.0f, (min.y + max.y) / 2.0f, (min.z + max.z) / 2.0f );
		Vec3f size = Vec3f( abs(max.x - min.x), abs(max.y - min.y), abs(max.z - min.z) );
		
		return createBox(world, size, rotation, position);
		
	}
	
	btRigidBody* create(btDynamicsWorld *world, Sphere sphere, Quatf rotation, Vec3f position)
	{
		return createSphere(world, sphere.getRadius(), rotation, position);		
	}
	
	

} } // namespace cinder::bullet