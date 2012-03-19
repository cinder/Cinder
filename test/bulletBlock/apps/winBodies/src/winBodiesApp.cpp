#include <vector>
#include "cinder/app/AppBasic.h"
#include "cinder/Surface.h"
#include "cinder/gl/Material.h"
#include "cinder/gl/Light.h"
#include "cinder/Camera.h"
#include "cinder/AxisAlignedBox.h"
#include "cinder/Sphere.h"
#include "cinder/gl/GlslProg.h"
#include "../../Cinder/test/bulletBlock/CinderBullet.h"
using namespace ci;
using namespace ci::app;

#include <list>
using std::list;

const int SPHERE_SEGS = 48;
const double MAX_LIFETIME = 5.0;

#if 0
void activateAllObjects(btDynamicsWorld *world)
{
	int i;
	for (i=world->getNumCollisionObjects()-1; i>=0 ;i--)
	{
		btCollisionObject* obj = world->getCollisionObjectArray()[i];
		btRigidBody* b = btRigidBody::upcast(obj);
		if (b)
		{
			b->activate(true);
		}
		
	}
}

class RigidObject {
public:
	btRigidBody*		body;
	btDynamicsWorld*	world;
	double				lifetime;
	
	
	virtual void draw() = 0;
	
	virtual void update(double dt)
	{
		lifetime += dt;
	}

	virtual ~RigidObject() 
	{
		if(body)
		{
			if(body->getMotionState() != NULL)
				delete body->getMotionState();
	//		if(body->getCollisionShape() != NULL)
	//			delete body->getCollisionShape();
			world->removeRigidBody(body);
			delete body;
		}
	}
	
protected:
	RigidObject(btRigidBody *_body, btDynamicsWorld *_world) 
	{
		body = _body;
		world = _world;
		lifetime = 0.0;
	}
};

class RigidCube : public RigidObject {
public:
	
	Vec3f			dimensions;
	
	RigidCube(btRigidBody* _body, Vec3f _dimensions, btDynamicsWorld *_world) 
		: RigidObject(_body, _world), dimensions(_dimensions) { }
	
	virtual void draw() 
	{
		gl::drawCube(Vec3f::zero(), dimensions);
	}
};

class RigidSphere : public RigidObject {
public:
	
	float			radius;
	
	RigidSphere(btRigidBody* _body, float _radius, btDynamicsWorld *_world) 
		: RigidObject(_body, _world), radius(_radius) { }
	
	virtual void draw() 
	{
		gl::drawSphere(Vec3f::zero(), radius, SPHERE_SEGS);
	}
};



class winBodiesApp : public AppBasic {
public:
	
	void draw();
	
	void setup();
	void initPhysics();
	void exitPhysics();
	void resize( int width, int height );
	
	void update();
	void mouseDown( MouseEvent event );
	void keyDown(KeyEvent event);
	
	void shutdown();
	
	double lastTime;
	
	CameraPersp			*mCam;
	gl::Material		mMaterial1;
	gl::Material		mMaterial1b;
	gl::Material		mMaterial2;
	gl::Material		mMaterialG;
	gl::Light			*mLight;
	
	btDiscreteDynamicsWorld* m_dynamicsWorld;	
	btBroadphaseInterface*	m_broadphase;	
	btCollisionDispatcher*	m_dispatcher;	
	btConstraintSolver*	m_solver;
	btDefaultCollisionConfiguration* m_collisionConfiguration;

	std::vector< std::shared_ptr<RigidObject> > mObjects;
	std::shared_ptr<btRigidBody>	m_groundRigidBody;
};



void winBodiesApp::setup(){
	// cam, lights material
	mCam = new CameraPersp(getWindowWidth(), getWindowHeight(), 45.0f);
	mCam->lookAt( Vec3f(100, 400, -400), Vec3f(0,0,0) );
	mCam->setPerspective( 60.0f, getWindowAspectRatio(), 0.1f, 5000.0f );
	
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);

	mLight = new gl::Light(gl::Light::DIRECTIONAL, 0);
	mLight->setDirection( Vec3f(0,0.1,0.3));
	mLight->setAmbient( Color( 0.2f, 0.2f, 0.2f ) );
	mLight->setDiffuse( Color( 1.0f, 1.0f, 1.0f ) );
	mLight->update( *mCam );
	mLight->enable();
	
	mMaterial1.setSpecular( Color(0.5,0.5,0.5) );
	mMaterial1.setDiffuse( Color( 0.33f, 0.24f, 1.0f ) );
	mMaterial1.setAmbient( Color( 0.1f, 0.1f, 0.1f ) );
	mMaterial1.setShininess( 20.0f );
	mMaterial1.apply();
	
	mMaterial2.setSpecular( Color(0,0,0) );
	mMaterial2.setDiffuse( Color(1,0,0) );
	mMaterial2.setAmbient( Color( 0.4f, 0.0f, 0.0f ) );
	mMaterial2.setEmission(Color(1,0,0));
	
	mMaterialG.setSpecular(Color(0,0,0));
	mMaterialG.setDiffuse(Color(0,0,0.03));
	mMaterialG.setAmbient(Color(0.1,0.1,0.1));
	
	initPhysics();
	
	lastTime = getElapsedSeconds();
}

void winBodiesApp::initPhysics()
{
	// setup physics environment. for all basic rigid body physics this can be left as it is
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
	m_broadphase = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver* sol = new btSequentialImpulseConstraintSolver;
	m_solver = sol;
	
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);
	m_dynamicsWorld->setGravity(btVector3(0,-10,0));
	
	
	// make a ground plane that cannot be moved
	btCollisionShape * groundShape	= new btStaticPlaneShape(btVector3(0,1,0),1);
	
	btDefaultMotionState * groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,-1,0)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0,groundMotionState,groundShape,btVector3(0,0,0));
	
	m_groundRigidBody = std::shared_ptr<btRigidBody>(new btRigidBody(groundRigidBodyCI));
	m_dynamicsWorld->addRigidBody(m_groundRigidBody.get());
	
	
}

void winBodiesApp::resize(int w, int h)
{
	glViewport( 0, 0, w, h );
	mCam->setPerspective(60.0f, getWindowAspectRatio(), 0.1f, 5000.0f);
	gl::setMatrices(*mCam);
}

void winBodiesApp::update()
{

	
	m_dynamicsWorld->stepSimulation(1.0f, 10);
	
	double now = getElapsedSeconds();
	double dt = now - lastTime;
	lastTime = now;
	
	bool objectsErased = false;
	
	for(int i = 0; i < mObjects.size(); i++)
	{
		mObjects[i]->update(dt);
		
		if(mObjects[i]->lifetime > MAX_LIFETIME)
		{
			mObjects.erase(mObjects.begin() + i);
			objectsErased = true;
		}
	}
	
	if(objectsErased)
		activateAllObjects(m_dynamicsWorld);
}

void winBodiesApp::draw()
{

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	
	for(int i = 0; i < mObjects.size(); i++) {
		mMaterial1.apply();
		Matrix44f tf = bullet::getWorldTransform(mObjects[i]->body);
		
		glPushMatrix();
			glMultMatrixf(tf.m);
			mObjects[i]->draw();
		glPopMatrix();
		
		glPolygonOffset(1.0f, 1.0f);
		mMaterial2.apply();
		glPolygonMode(GL_FRONT, GL_LINE);
		
		glPushMatrix();
			glMultMatrixf(tf.m);
			mObjects[i]->draw();
		glPopMatrix();
		
		glPolygonMode(GL_FRONT, GL_FILL);
		mMaterial1.apply();
		
	}
	
	mMaterialG.apply();
	glPushMatrix();
	gl::drawCube( Vec3f::zero(), Vec3f(1000, 0.1f, 1000) );
	glPopMatrix();
	
}

void winBodiesApp::keyDown( KeyEvent event )
{
	// nothing
}
 
void winBodiesApp::mouseDown( MouseEvent event )
{	
	if( event.isLeftDown() && !event.isAltDown() ) {
		int sizex = (rand()%49)+30;
		int sizey = (rand()%49)+30;
		int sizez = (rand()%49)+30;
		int height = (rand()%200)+300;
		int xoff = (rand()%10)-5;
		int zoff = (rand()%10)-5;
		
		Vec3f dimensions = Vec3f(sizex,sizey,sizez);
		btRigidBody *body = bullet::createBox(m_dynamicsWorld, dimensions, Quatf(), Vec3f((float)xoff, height, (float)zoff));
		
		std::shared_ptr<RigidObject> c( new RigidCube(body, dimensions, m_dynamicsWorld) );
		mObjects.push_back(c);
		
	} 
	
	if (event.isLeftDown() && event.isAltDown()) {
		int radius = (rand()%29)+15;
		int height = (rand()%200)+300;
		int xoff = (rand()%10)-5;
		int zoff = (rand()%10)-5;
		
		float fradius = (float) radius;
		btRigidBody *body = bullet::createSphere(m_dynamicsWorld, fradius, Quatf(), Vec3f(float(xoff), height,float(zoff)));
		
		std::shared_ptr<RigidObject> s( new RigidSphere(body, fradius, m_dynamicsWorld) );
		mObjects.push_back(s);
	}

}

void winBodiesApp::shutdown()
{
	delete mCam;
	delete mLight;
	mObjects.clear();
	
	exitPhysics();
}

void winBodiesApp::exitPhysics()
{
	//cleanup in the reverse order of creation/initialization
	
	
	delete m_dynamicsWorld;
	
	delete m_solver;
	
	delete m_broadphase;
	
	delete m_dispatcher;
	
	delete m_collisionConfiguration;
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( winBodiesApp, RendererGl )
#endif