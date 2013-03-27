#include "cinder/app/AppBasic.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"
#include "cinder/Perlin.h"
#include "cinder/Font.h"

#include "cinder/dx/DxTexture.h"
#include "cinder/dx/dx.h"

#include "Particle.h"

#define TOTAL_PARTICLES 1000
#define NEW_PARTICLES 5
#define FLOOR_LEVEL 900

using namespace ci;
using namespace ci::app;
using namespace std;

class BasicApp : public AppBasic {
  public:
	void prepareSettings( Settings *settings );
	void setup();
	void update();
	void draw();
	
	void keyDown( KeyEvent event);
	void mouseDown( MouseEvent event );
	void mouseMove( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void mouseUp( MouseEvent event );
	void updateParticleIndex();

	Perlin		mPerlin;

	bool		mIsPressed;
	bool		mIsFloorActive;
	bool		mIsPerlinActive;
	bool		mIsDirty;

	Vec2f		mPointerTarget;
	Vec2f		mPointerPosition;
	Vec2f		mPointerVelocity;

	int			mParticleIndex;
	float		mGravity;


	Font		mFont;
	dx::Texture imgParticle, imgEmitter;

	vector<Particle>	mParticles;

	dx::TextureFontRef	mTextureFont;
};

void BasicApp::prepareSettings(Settings *settings)
{
	settings->setFrameRate(60.0f);
}

void BasicApp::mouseDown( MouseEvent event )
{
	mIsPressed = true;
	mPointerTarget = event.getPos();
}

void BasicApp::mouseUp( MouseEvent event )
{
	mIsPressed = false;
}

void BasicApp::mouseMove( MouseEvent event )
{
	mPointerTarget = event.getPos();
}

void BasicApp::mouseDrag( MouseEvent event )
{
	mPointerTarget = event.getPos();
}

void BasicApp::keyDown( KeyEvent event )
{
	switch ( event.getChar() )
	{
		case 'G':
			if ( mGravity == 0.0f) { mGravity = 0.45f; } 
			else { mGravity = 0.0f; }
			break;
		case 'F':
			mIsFloorActive = !mIsFloorActive;
			break;
		case 'P':
			mIsPerlinActive = !mIsPerlinActive;
			break;
	}
}

void BasicApp::setup()
{
	mPerlin = Perlin();

	mFont = Font( "Arial", 24.0f );

	imgParticle = dx::Texture( loadImage( loadAsset("particle.png")));
	imgEmitter  = dx::Texture( loadImage( loadAsset("emitter.png")));

	mGravity = 0.0f;
	mIsFloorActive = true;
	mIsPerlinActive = true;
	mIsDirty = false;	

	for ( int i = 0; i < TOTAL_PARTICLES; i++) 
	{
		mParticles.push_back(Particle((float)FLOOR_LEVEL));
	}

	mParticleIndex = 0;

	mPointerPosition = Vec2f::zero();
	mPointerVelocity = Vec2f::zero();
	mPointerTarget = getWindowCenter();

	mIsPressed = false;	
}

void BasicApp::update()
{
	Vec2f delta = mPointerTarget - mPointerPosition;
	delta *= .25;
	mPointerVelocity += delta;
	mPointerPosition += mPointerVelocity;
	mPointerVelocity *= .83f;

	if ( mIsFloorActive && mPointerPosition.y > FLOOR_LEVEL )
	{
		mPointerPosition.y = FLOOR_LEVEL;
		mPointerVelocity.y = 0;
	}

	for ( Particle &particle: mParticles )  
	{
		Vec2f noiseVector = Vec2f::zero();

		if ( !particle.isDead ) 

			if ( mIsPerlinActive ) 
			{	
				float noise = mPerlin.fBm( Vec3f( particle.position.x *.005f,  particle.position.y *.005f, particle.life / 200.0f ) );
				float angle = noise * 15.0f;
				noiseVector = Vec2f( cos(angle), sin(angle) );
			}

			particle.Update(mGravity, mIsFloorActive, noiseVector);
	}
	
	if ( mIsPressed )
	{
		for ( int i = 0; i < NEW_PARTICLES; i++) 
		{
			Vec2f newPosition( mPointerPosition.x + Rand::randFloat(-2.0f, 2.0f), mPointerPosition.y + Rand::randFloat(-2.0f, 2.0f) );
			Vec2f newVelocity( mPointerVelocity.x * .25f + Rand::randFloat(-5.0f, 5.0f), mPointerVelocity.y * .25f + Rand::randFloat(-5.0f, 5.0f) );
			mParticles[mParticleIndex].Reset(newPosition, newVelocity, 100 + Rand::randInt(100));
			updateParticleIndex();
		}
	}

}

void BasicApp::updateParticleIndex() 
{
	mParticleIndex++;
	if ( mParticleIndex >= TOTAL_PARTICLES ) 
		mParticleIndex = 0; 
}

void BasicApp::draw()
{
	auto count = 0;
	dx::clear( Color( 0, 0, 0 ), true );

	//dx::enableAlphaBlending();
	dx::enableAdditiveBlending();

	float particleSize = 50.0f;

	if( imgEmitter )
	{
		Vec2f emitterRadius( particleSize*2.0f, particleSize*2.0f);
		dx::draw( imgEmitter, Rectf( mPointerPosition - emitterRadius, mPointerPosition + emitterRadius) );
	}

	if ( imgParticle )
	{
		dx::batchTextureBegin();
		for ( Particle particle : mParticles )
		{
			if ( !particle.isDead ) 
			{
				particleSize = 50.0f * particle.life/200.0f;
				Vec2f particleRadius(particleSize, particleSize);
				dx::draw( imgParticle, Rectf( particle.position - particleRadius, particle.position + particleRadius ) );
				count++;
			}
		}
		dx::batchTextureEnd();
	}

	std::stringstream s;
	s << "Framerate:" << getAverageFps() << "  Particles: " << count;
	dx::drawString(s.str(),Vec2f(10.0f,10.0f),Color::white(),mFont);


}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( BasicApp, RendererDx )