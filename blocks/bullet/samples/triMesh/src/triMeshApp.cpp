#include <vector>
#include "cinder/app/AppBasic.h"
#include "cinder/Surface.h"
#include "cinder/gl/Material.h"
#include "cinder/gl/Light.h"
#include "cinder/Camera.h"
#include "cinder/Box.h"
#include "cinder/Sphere.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/ObjLoader.h"
#include "cinder/gl/Vbo.h"
#include "cinder/ImageIo.h"
#include "cinder/Channel.h"
#include "../blocks/bullet/CinderBullet.h"
#include "cinder/TriMesh.h"
#include "Resources.h"
using namespace ci;
using namespace ci::app;

#include <list>
using std::list;

#define LB_ID 128


const int SPHERE_SEGS = 48;
const double MAX_LIFETIME = 5.0;
const float CONVEX_SCALE = 20.5f;
const float CONCAVE_SCALE = 20.0f;
const float HF_SCALE = 24.0f;
const float HF_SCALEY = 400.0f;
const float HF_HEIGHTSCALE = 1.0f;

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
		if(body->getMotionState())
			delete body->getMotionState();
		world->removeRigidBody(body);
		delete body;
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

// We'll create a new Cinder Application by deriving from the BasicApp class
class triMeshApp : public AppBasic {
public:
	// Cinder calls this function 30 times per second by default
	void draw();
	
	void setup();
	void initPhysics();
	void exitPhysics();
	void resize( int width, int height );
	
	void update();
	void mouseDown( MouseEvent event );
	void updateData();
	void shutdown();
	
	double lastTime;
	double yrot;
	
	CameraPersp			*mCam;
	gl::Material		mMaterial1;
	gl::Material		mMaterial2;
	gl::Light			*mLight;
	
	btDiscreteDynamicsWorld* m_dynamicsWorld;	
	btBroadphaseInterface*	m_broadphase;	
	btCollisionDispatcher*	m_dispatcher;	
	btConstraintSolver*	m_solver;
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	
	TriMesh mConvex, mConcave;
	gl::VboMesh		mVBO, mVBOTerrain, mVBOHeightfield;
	
	btRigidBody *m_convexRigidBody;
	btRigidBody *m_concaveRigidBody;
	btRigidBody *m_hfRigidBody;
	
	Surface32f m_hfImage;
	Channel32f* m_hfChannel;
	
	
	typedef struct {
		btRigidBody*	body;
		Vec3f			dimensions;
	} cube;
	
	std::vector< shared_ptr<RigidObject> > mObjects;
};

void triMeshApp::setup(){
	// cam, lights material
	mCam = new CameraPersp(getWindowWidth(), getWindowHeight(), 45.0f);
	mCam->lookAt( Vec3f(300, 700, -600), Vec3f(0,0,0) );
	mCam->setPerspective( 60.0f, getWindowAspectRatio(), 0.1f, 5000.0f );
	yrot = 0;
	
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	mLight = new gl::Light(gl::Light::DIRECTIONAL, 0);
	mLight->setDirection( Vec3f(0,0.1,0.3).normalized());
	mLight->setAmbient( Color( 0.2f, 0.2f, 0.2f ) );
	mLight->setDiffuse( Color( 1.0f, 1.0f, 1.0f ) );
	mLight->update( *mCam );
	mLight->enable();
	
	mMaterial1.setSpecular( Color(0.5,0.5,0.5) );
	mMaterial1.setDiffuse( Color( 0.0f, 1.0f, 0.0f ) );
	mMaterial1.setAmbient( Color( 0.1f, 0.1f, 0.1f ) );
	mMaterial1.setShininess( 20.0f );
	mMaterial1.apply();
	
	mMaterial2.setSpecular( Color(0,0,0) );
	mMaterial2.setDiffuse( Color(1,0,0) );
	mMaterial2.setAmbient( Color( 0.4f, 0.0f, 0.0f ) );
	mMaterial2.setEmission(Color(1,0,0));
	
	initPhysics();
	
	
	ObjLoader loader( loadResource( RES_LOSPHERE )->getStream() );
	loader.load( &mConvex );
	mVBO = gl::VboMesh( mConvex );
	
	ObjLoader loader2( loadResource( RES_TORUS )->getStream() );
	loader2.load( &mConcave );
	mVBOTerrain = gl::VboMesh( mConcave );

	btConvexHullShape* shape = bullet::createConvexHullShape(mConvex, Vec3f(CONVEX_SCALE, CONVEX_SCALE, CONVEX_SCALE));
	m_convexRigidBody = bullet::createConvexHullBody(m_dynamicsWorld, shape, Vec3f(0,500,0), 10000);
	
	btBvhTriangleMeshShape* concaveShape = bullet::createStaticConcaveMeshShape(mConcave, Vec3f(CONCAVE_SCALE, CONCAVE_SCALE, CONCAVE_SCALE), 5.0f);
	m_concaveRigidBody = bullet::createStaticRigidBody(m_dynamicsWorld, concaveShape, Vec3f(0,255,0));
	
	m_hfImage = loadImage(loadResource(RES_HEIGHTMAP));
	m_hfChannel = new Channel32f(m_hfImage);
	// note that HF_HEIGHTSCALE is ignored since we are using float data. we adjust the y-scale with the local scaling parameter only!
	btHeightfieldTerrainShape* hfShape = bullet::createHeightfieldTerrainShape(m_hfChannel, 64, 64, HF_HEIGHTSCALE, -500, 500, 1, Vec3f(HF_SCALE,HF_SCALEY,HF_SCALE));
	m_hfRigidBody = bullet::createStaticRigidBody(m_dynamicsWorld, hfShape, Vec3f(0,0,0));
	
	gl::VboMesh::Layout layout;
	layout.setDynamicColorsRGB();
	layout.setDynamicPositions();
	mVBOHeightfield = gl::VboMesh( m_hfImage.getWidth() * m_hfImage.getHeight(), 0, layout, GL_POINTS );
	
	updateData( );		
}

void triMeshApp::initPhysics()
{
	// setup physics environment. for all basic rigid body physics this can be left as it is
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
	m_broadphase = new btDbvtBroadphase();
	btSequentialImpulseConstraintSolver* sol = new btSequentialImpulseConstraintSolver;
	m_solver = sol;
	
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);
	m_dynamicsWorld->setGravity(btVector3(0,-10,0));
	
}

void triMeshApp::resize(int w, int h)
{
	glViewport( 0, 0, w, h );
	mCam->setPerspective(60.0f, getWindowAspectRatio(), 0.1f, 5000.0f);
	gl::setMatrices(*mCam);
}

void triMeshApp::update()
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
	
	yrot += dt * 36.0;
	if(yrot >=360.0) yrot = 0.0;
	
}

void triMeshApp::draw()
{
	glPushMatrix();
	glRotated(yrot, 0, 1, 0);
	
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	// convex mesh
	glPushMatrix();

	Matrix44f tf = bullet::getWorldTransform(m_convexRigidBody);
	glMultMatrixf(tf.m);
	gl::scale(Vec3f(CONVEX_SCALE, CONVEX_SCALE, CONVEX_SCALE));
	mMaterial1.apply();
	gl::draw(mVBO);
	/*
	glPolygonOffset(1.0f, 1.0f);
	mMaterial2.apply();
	glPolygonMode(GL_FRONT, GL_LINE);
	gl::draw(mVBO);
	glPolygonMode(GL_FRONT, GL_FILL);
	 */
	
	glPopMatrix();
	
	//concave mesh
	glPushMatrix();
	
	Matrix44f tf2 = bullet::getWorldTransform(m_concaveRigidBody);
	glMultMatrixf(tf2.m);
	gl::scale(Vec3f(CONCAVE_SCALE, CONCAVE_SCALE, CONCAVE_SCALE));
	mMaterial1.apply();
	gl::draw(mVBOTerrain);
	/*
	glPolygonOffset(1.0f, 1.0f);
	mMaterial2.apply();
	glPolygonMode(GL_FRONT, GL_LINE);
	gl::draw(mVBOTerrain);
	glPolygonMode(GL_FRONT, GL_FILL);
	 */
	
	glPopMatrix();
	
	// heightfield
	mMaterial2.apply();
	glPushMatrix();
	Matrix44f tf3 = bullet::getWorldTransform(m_hfRigidBody);
	glMultMatrixf(tf3.m);
	gl::draw( mVBOHeightfield );
    glPopMatrix();
	
	
	// falling bodies
	for(int i = 0; i < mObjects.size(); i++) {
		mMaterial1.apply();
		Matrix44f tf = bullet::getWorldTransform(mObjects[i]->body);
		
		glPushMatrix();
		glMultMatrixf(tf.m);
		mObjects[i]->draw();
		glPopMatrix();
		/*
		glPolygonOffset(1.0f, 1.0f);
		mMaterial2.apply();
		glPolygonMode(GL_FRONT, GL_LINE);
		
		glPushMatrix();
		glMultMatrixf(tf.m);
		mObjects[i]->draw();
		glPopMatrix();
		
		glPolygonMode(GL_FRONT, GL_FILL);
		mMaterial1.apply();
		 */
		
	}
	
	glPopMatrix();
	
}

void triMeshApp::mouseDown( MouseEvent event )
{	
	if( event.isLeftDown() && !event.isAltDown() ) {
		int sizex = (rand()%49)+30;
		int sizey = (rand()%49)+30;
		int sizez = (rand()%49)+30;
		int height = (rand()%200)+300;
		int xoff = (rand()%1000)-500;
		int zoff = (rand()%1000)-500;
		
		Vec3f dimensions = Vec3f(sizex,sizey,sizez);
		btRigidBody *body = bullet::createBox(m_dynamicsWorld, dimensions, Quatf(), Vec3f((float)xoff, height, (float)zoff));
		
		shared_ptr<RigidObject> c( new RigidCube(body, dimensions, m_dynamicsWorld) );
		mObjects.push_back(c);
		
	} 
}

void triMeshApp::updateData()
{
	Surface32f::Iter pixelIter = m_hfImage.getIter();
	gl::VboMesh::VertexIter vertexIter( mVBOHeightfield );
	
	while( pixelIter.line() ) {
		while( pixelIter.pixel() ) {
			Color color( pixelIter.r(), pixelIter.g(), pixelIter.b() );
			float height;

			height = color.dot( Color( 0.3333f, 0.3333f, 0.3333f ) );
         

			// the x and the z coordinates correspond to the pixel's x & y
			float x = (pixelIter.x() - m_hfImage.getWidth() / 2.0f);
			float z = (pixelIter.y() - m_hfImage.getHeight() / 2.0f);
			
            vertexIter.setPosition( x * HF_SCALE, height * HF_SCALEY, z * HF_SCALE);
			vertexIter.setColorRGB( color );
			++vertexIter;
		}
	}
}

void triMeshApp::shutdown()
{
	delete mCam;
	delete mLight;
	
	mObjects.clear();
	
	exitPhysics();
}

void triMeshApp::exitPhysics()
{
	//cleanup in the reverse order of creation/initialization
	
	if(m_convexRigidBody->getMotionState())
		delete m_convexRigidBody->getMotionState();
	m_dynamicsWorld->removeRigidBody(m_convexRigidBody);
	delete m_convexRigidBody;
	
	if(m_concaveRigidBody->getMotionState())
		delete m_concaveRigidBody->getMotionState();
	m_dynamicsWorld->removeRigidBody(m_concaveRigidBody);
	delete m_concaveRigidBody;
	
	if(m_hfRigidBody->getMotionState())
		delete m_hfRigidBody->getMotionState();
	m_dynamicsWorld->removeRigidBody(m_hfRigidBody);
	delete m_hfRigidBody;
	
	
	delete m_dynamicsWorld;
	
	delete m_solver;
	
	delete m_broadphase;
	
	delete m_dispatcher;
	
	delete m_collisionConfiguration;
}


// This line tells Cinder to actually create the application
CINDER_APP_BASIC( triMeshApp, RendererGl )
