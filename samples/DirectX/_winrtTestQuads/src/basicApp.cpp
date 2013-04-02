#include "cinder/app/AppBasic.h"
#include "cinder/Perlin.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"
#include "cinder/Font.h"

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

};

void BasicApp::prepareSettings( Settings *settings )
{
	settings->setFrameRate( 60.0f );
}

void BasicApp::setup()
{
	BillboardImg = new dx::Texture( loadImage(loadAsset("testPattern.png")));

	mFont = Font( "Arial", 24.0f );
	addBillboards();
	mPointerPosition = Vec2f::zero();
	mPointerTarget = getWindowCenter();
	mPointerVelocity = Vec2f::zero();

}

void BasicApp::keyDown( KeyEvent event)
{
		char key = event.getChar();
	
	if( ( key == 'q' ) || ( key == 'Q' ) )
	{
		addBillboards();
	}
}

void BasicApp::addBillboards() {
	for ( int i = 0; i < 50; i++) 
	{
		mBillboards.push_back( Billboard( Vec3f(Rand::randFloat(560,1360), Rand::randFloat(240,840), Rand::randPosNegFloat(-10,10)),  Rand::randPosNegFloat(-1,1), Rand::randPosNegFloat(-1,1)  ) );
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

	dx::clear( Color(0,0,0), true);
	//dx::enableAdditiveBlending();

	dx::disableAlphaBlending();

	BillboardImg->bind();

	float rotationY = 3.14*(.5f-(mPointerPosition.x/1920.0f));
	float rotationX = 3.14*-(.5f-(mPointerPosition.y/1080.0f));

	for ( Billboard &billboard : mBillboards )
	{
		dx::pushMatrices();
		dx::translate( billboard.position.x, billboard.position.y, billboard.position.z );
		dx::rotate(Quatf(billboard.xRotation, billboard.yRotation, 0));

		dx::begin( GL_QUADS );
			dx::texCoord(0,0);	dx::vertex( -64, -64);
			dx::texCoord(1,0);	dx::vertex(  64, -64);
			dx::texCoord(1,1);	dx::vertex(  64,  64);
			dx::texCoord(0,1);	dx::vertex( -64,  64);
		dx::end();
		dx::popMatrices();

		count++;
	}

	std::stringstream s;
	s << "Press 'Q' to add more Quads.   Total Quads: " << count << "    Framerate:" << getAverageFps();
	dx::drawString(s.str(),Vec2f(10.0f,10.0f),Color::white(),mFont);
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( BasicApp, RendererDx )