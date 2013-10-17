#include "cinder/app/AppBasic.h"
#include "cinder/app/RendererDx.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"
#include "cinder/Font.h"
#include "cinder/dx/DxTexture.h"
#include "cinder/dx/dx.h"
#include "Particle.h"

#define TOTAL_PARTICLES 300

using namespace ci;
using namespace ci::app;
using namespace std;

class BasicApp : public AppBasic {
  public:
	void				prepareSettings( Settings *settings );
	void				setup();
	void				update();
	void				draw();

	dx::TextureRef		imgParticle;
	dx::TextureRef		imgBkg;

	vector<Particle>	mParticles;
	vector<Area>		mTextureAreas;

	Font				mFont;
	Area				mBounds;
};

void BasicApp::prepareSettings(Settings *settings)
{
	settings->setFrameRate(60.0f);
}

void BasicApp::setup()
{
	mFont = Font( "Segoe UI", 18.0f );

	imgParticle = dx::Texture::create( loadImage( loadAsset("heptagons.png") ) );
	imgBkg = dx::Texture::create( loadImage( loadAsset("bkg.jpg") ) );

	for ( int i = 0; i < TOTAL_PARTICLES; i++) 
	{
		Particle p = Particle(i);
		Vec2f newPosition( Rand::randFloat(0.0f, (float)getWindowWidth()),  Rand::randFloat(0.0f, (float)getWindowHeight()) );
		Vec2f newVelocity( Rand::randPosNegFloat(-3.0f, 3.0f), Rand::randPosNegFloat(-3.0f, 3.0f ));
		p.Reset(newPosition, newVelocity);
		mParticles.push_back(p);
	}

	mTextureAreas.push_back(Area(Vec2i(0,0), Vec2i(256, 256)));
	mTextureAreas.push_back(Area(Vec2i(256,0), Vec2i(512, 256)));
	mTextureAreas.push_back(Area(Vec2i(0,256), Vec2i(256, 512)));
	mTextureAreas.push_back(Area(Vec2i(256,256), Vec2i(512, 512)));
	mBounds = getWindowBounds();
}

void BasicApp::update()
{
	for ( Particle &particle: mParticles )  
	{
		particle.Update(mBounds.x2, mBounds.y2);
	}

}

void BasicApp::draw()
{
	auto count = 0;
	dx::clear( Color( 0, 0, 0 ), true );

	//dx::enableAlphaBlending();
	dx::enableAdditiveBlending();

	if ( imgBkg )
		dx::draw( imgBkg, getWindowBounds() );

	if( imgParticle ) {
		dx::batchTextureBegin();
		Vec2f particleRadius(64.0f, 64.0f);
		for ( Particle particle : mParticles )
		{
			dx::draw( imgParticle, mTextureAreas[particle.textureID], Rectf( particle.position - particleRadius, particle.position + particleRadius ) );
		}
		dx::batchTextureEnd();
	}

	std::stringstream s;
	s << "Framerate:" << getAverageFps();
	dx::drawString(s.str(),Vec2f(10.0f,10.0f),Color::white(),mFont);
}

// This line tells Cinder to actually create the application
CINDER_APP_BASIC( BasicApp, RendererDx )