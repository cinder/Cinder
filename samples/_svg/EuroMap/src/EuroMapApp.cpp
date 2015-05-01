// Map of Europe copyright Wikiepedia:
// http://en.wikipedia.org/wiki/File:Blank_map_of_Europe_-_Atelier_graphique_colors.svg

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/TextureFont.h"
#include "cinder/svg/Svg.h"
#include "cinder/cairo/Cairo.h"
#include "cinder/Timeline.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class EuroMapApp : public App {
  public:
	static void prepareSettings( Settings *settings );

	void		setup() override;
	void		mouseMove( MouseEvent event ) override;
	void		draw() override;

	gl::TextureRef		mMapTex;
	gl::TextureFontRef	mFont;
	svg::DocRef			mMapDoc;
	svg::Node 			*mCurrentCountry;
	Anim<float>			mCurrentCountryAlpha;
};

gl::TextureRef renderSvgToTexture( svg::DocRef doc, ivec2 size )
{
	cairo::SurfaceImage srf( size.x, size.y, false );
	cairo::Context ctx( srf );
	ctx.render( *doc );
	srf.flush();
	return gl::Texture::create( srf.getSurface() );
}

void EuroMapApp::prepareSettings( Settings *settings )
{
	settings->setResizable( false );
	settings->setWindowSize( 840, 730 );
}

void EuroMapApp::setup()
{
	mMapDoc = svg::Doc::create( loadAsset( "Europe.svg" ) );
	mMapTex = renderSvgToTexture( mMapDoc, getWindowSize() );
	
	mFont = gl::TextureFont::create( Font( loadAsset( "Dosis-Medium.ttf" ), 36 ) );
	
	mCurrentCountry = 0;
}

void EuroMapApp::mouseMove( MouseEvent event )
{
	svg::Node *newNode = mMapDoc->nodeUnderPoint( event.getPos() );
	if( newNode != mCurrentCountry )
		timeline().apply( &mCurrentCountryAlpha, 0.0f, 1.0f, 0.35f );
	mCurrentCountry = newNode;
	// if the current node has no name just set it to NULL
	if( mCurrentCountry && mCurrentCountry->getId().empty() )
		mCurrentCountry = NULL;	
}

void EuroMapApp::draw()
{
	gl::clear();
	gl::enableAlphaBlending();
	glLineWidth( 2.0f );
	
	if( mMapTex ) {
		gl::color( Color::white() );
		gl::draw( mMapTex );
	}

	if( mCurrentCountry ) {
		// draw the outline
		gl::color( 1, 0.5f, 0.25f, mCurrentCountryAlpha );
		gl::draw( mCurrentCountry->getShapeAbsolute() );
	
		// draw the name
		string countryName = mCurrentCountry->getId();
		vec2 pos = mCurrentCountry->getBoundingBoxAbsolute().getCenter();
		pos.x -= mFont->measureString( countryName ).x / 2;
		
		gl::color( ColorA( 1, 1, 1, mCurrentCountryAlpha ) );
		mFont->drawString( countryName, pos + vec2( 2, 2 ) );
		gl::color( ColorA( 0, 0, 0, mCurrentCountryAlpha ) );
		mFont->drawString( countryName, pos );		
	}
}


CINDER_APP( EuroMapApp, RendererGl, EuroMapApp::prepareSettings )
