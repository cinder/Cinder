#include "cinder/app/AppBasic.h"
#include "cinder/Perlin.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"
#include "cinder/Font.h"

#include "cinder/Camera.h"

#include "cinder/dx/dx.h"
#include "cinder/dx/DxTexture.h"

#include "Billboard.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class BasicApp : public AppBasic {
  public:
	void prepareSettings( Settings *settings );
	void setup();
	
	void mouseDown( MouseEvent event );	
	void mouseUp( MouseEvent event );
	void mouseMove( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void keyDown( KeyEvent event );
	
	void update();
	void draw();
	void addBillboards();

	Vec2i		mPointerTarget;
	Vec2f		mPointerPosition;
	Vec2f		mPointerVelocity;

	Font		mFont;
	dx::Texture *BillboardImg;
	vector<Billboard>	mBillboards;

	Matrix44f		mModelView;
	CameraPersp		mCam;

	float			mFOV;	

};

void BasicApp::prepareSettings( Settings *settings )
{
	settings->setFrameRate( 60.0f );
}

void BasicApp::setup()
{
	BillboardImg = new dx::Texture( loadImage(loadAsset("imgBillboard.png")));

	mFont = Font( "Arial", 12.0f );
	addBillboards();
	mPointerPosition = Vec2f::zero();
	mPointerTarget = getWindowCenter();
	mPointerVelocity = Vec2f::zero();

	mFOV = 90.0f;

	mCam.setPerspective( mFOV, 1.77f, 1, 1000 );
	mCam.lookAt( Vec3f( 0, 0, -200 ), Vec3f::zero(), -Vec3f::yAxis() );
}

void BasicApp::keyDown( KeyEvent event)
{

	char key = event.getChar();
	
	if( ( key == 'q' ) || ( key == 'Q' ) )
	{
		addBillboards();
	}

	if ( ( key == 'a' ) || ( key == 'A' ) ) {
		mFOV += 5.0f;
		mCam.setPerspective( mFOV, 1.77f, 1, 1000 );
	}

	if ( ( key == 's'  ) || ( key == 'S' ) ) {
		mFOV -= 5.0f;
		mCam.setPerspective( mFOV, 1.77f, 1, 1000 );
	}

	if ( ( key == 'w'  ) || ( key == 'W' ) ) {
		Vec3f pos = mCam.getEyePoint();
		pos.z += 25.0f;
		mCam.setEyePoint(pos);
	}


	if ( ( key == 'z'  ) || ( key == 'Z' ) ) {
		Vec3f pos = mCam.getEyePoint();
		pos.z -= 25.0f;
		mCam.setEyePoint(pos);
	}
}

void BasicApp::addBillboards() {
	for ( int i = 0; i < 50; i++) 
	{
		mBillboards.push_back( Billboard( Vec3f(Rand::randPosNegFloat(-100,100), Rand::randFloat(-100,100), Rand::randPosNegFloat(-100,100)),  Rand::randPosNegFloat(-1,1), Rand::randPosNegFloat(-.3,.3)  ) );
	}
}

void BasicApp::mouseDown( MouseEvent event )
{
	mPointerTarget = event.getPos();
}

void BasicApp::mouseUp( MouseEvent event )
{
}

void BasicApp::mouseMove( MouseEvent event )
{
	mPointerTarget = event.getPos();
}

void BasicApp::mouseDrag( MouseEvent event )
{
	mPointerTarget = event.getPos();
}

void BasicApp::update()
{
	mPointerVelocity += (mPointerTarget - mPointerPosition )*.35;
	mPointerVelocity *= .7;
	mPointerPosition += mPointerVelocity;

	for ( Billboard &billboard : mBillboards ) 
	{
		billboard.Update();
	}

}

void BasicApp::draw()
{

	auto count = 0;

	dx::clear( Color(0.33f,0.42f,0.184f), true);

	dx::setMatrices( mCam );

	//dx::enableAdditiveBlending();
	//dx::enableAdditiveBlending();
	dx::disableAlphaBlending();

	BillboardImg->bind();

	dx::begin( GL_QUADS );

	float rotationY = 3.14*(.5f-(mPointerPosition.x/1920.0f));
	float rotationX = 3.14*-(.5f-(mPointerPosition.y/1080.0f));

	for ( Billboard &billboard : mBillboards )
	{
		dx::pushMatrices();
		dx::translate( billboard.position.x, billboard.position.y, billboard.position.z );
		dx::rotate(Quatf(billboard.xRotation, billboard.yRotation, 0));

			dx::texCoord(0,0);	dx::vertex( -3.2, -3.2);
			dx::texCoord(1,0);	dx::vertex(  3.2, -3.2);
			dx::texCoord(1,1);	dx::vertex(  3.2,  3.2);
			dx::texCoord(0,1);	dx::vertex( -3.2,  3.2);
		dx::popMatrices();

		count++;
	}
	
	dx::end();


	dx::pushMatrices();	
		dx::translate( -50, -50, 100 );
		//dx::scale(.5f, .5f, .5f);
		std::stringstream s;
		s << "Press 'Q' to add more Quads.   Total Quads: " << count << "    Framerate:" << getAverageFps() << "  FOV:"  << mFOV;
		dx::drawString(s.str(),Vec2f(0.0f,0.0f),Color::white(),mFont);
	dx::popMatrices();

}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( BasicApp, RendererDx )