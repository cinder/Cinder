/*
 * Code Copyright 2011 Robert Hodgin ( http://roberthodgin.com )
 * Used with permission for the Cinder Project ( http://libcinder.org )
 */

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/Url.h"
#include "cinder/System.h"
#include "boost/filesystem.hpp"
#include <iostream>
#include <fstream>
#include <string>

#include "Item.h"

#define APP_WIDTH 1024
#define APP_HEIGHT 768

using namespace ci;
using namespace ci::app;
using namespace std;


class PaletteBrowserApp : public AppBasic {
  public:
	void prepareSettings( Settings *settings );
	void setup();
	void initData( const fs::path &path );
	void createItem( const std::string &line, int lineNumber );
	void mouseMove( MouseEvent event );
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
	
	vector<Item>			mItems;
	vector<Item>::iterator	mMouseOverItem;
	vector<Item>::iterator	mNewMouseOverItem;
	vector<Item>::iterator	mSelectedItem;
	
	vector<gl::Texture>		mImages;
	
	float mLeftBorder;
	float mTopBorder;
	float mItemSpacing;
	
	gl::Texture mTitleTex;
	gl::Texture mBgImage;
	gl::Texture mFgImage;
	gl::Texture mSwatchSmallTex, mSwatchLargeTex;
	
	Anim<float> mFgAlpha;
	Anim<Color> mBgColor;
};

void PaletteBrowserApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( APP_WIDTH, APP_HEIGHT );
}


void PaletteBrowserApp::setup()
{
	// fonts
	Font smallFont = Font( loadAsset( "Signika-Regular.ttf" ), 16 );
	Font bigFont   = Font( loadAsset( "Signika-Regular.ttf" ), 100 );
	Item::setFonts( smallFont, bigFont );
	
	// title text
	TextLayout layout;
	layout.clear( ColorA( 1, 1, 1, 0 ) );
	layout.setFont( bigFont );
	layout.setColor( Color::white() );
	layout.addLine( "Icelandic Colors" );
	mTitleTex		= gl::Texture( layout.render( true ) );
	
	// positioning
	mLeftBorder		= 50.0f;
	mTopBorder		= 375.0f;
	mItemSpacing	= 22.0f;
	
	// create items
	initData( getAssetPath( "iceland.txt" ) );
	
	// textures and colors
	mFgImage		= gl::Texture( loadImage( loadAsset( "Colors of Iceland.jpg" ) ) );
	mBgImage		= gl::Texture( APP_WIDTH, APP_HEIGHT );
	mFgAlpha		= 1.0f;
	mBgColor		= Color::white();
	
	// swatch graphics (square and blurry square)
	mSwatchSmallTex	= gl::Texture( loadImage( loadAsset( "swatchSmall.png" ) ) );
	mSwatchLargeTex	= gl::Texture( loadImage( loadAsset( "swatchLarge.png" ) ) );
	
	// state
	mMouseOverItem		= mItems.end();
	mNewMouseOverItem	= mItems.end();
	mSelectedItem		= mItems.begin();
	mSelectedItem->select( timeline(), mLeftBorder );
}

void PaletteBrowserApp::initData( const fs::path &path )
{
	vector<string> lines = split( loadString( loadFile( path ) ), "\r\n" );

	for( vector<string>::const_iterator lineIt = lines.begin(); lineIt != lines.end(); ++lineIt )
		createItem( *lineIt, lineIt - lines.begin() );
}

void PaletteBrowserApp::createItem( const std::string &line, int lineNumber )
{
	string title			= line.substr( 0, line.find_last_of( ':' ) );
	string desc				= line.substr( line.find_last_of( ':' ) +1 );
	string imageFilename	= toString( lineNumber + 1 ) + ".jpg";
	string paletteFilename	= toString( lineNumber + 1 ) + ".png";
	gl::Texture image		= gl::Texture( loadImage( loadAsset( fs::path("images") / imageFilename ) ) );
	Surface palette			= Surface( loadImage( loadAsset( fs::path("palettes") / paletteFilename ) ) );
	
	Vec2f pos( mLeftBorder, mTopBorder + lineNumber * mItemSpacing );
	Item item = Item( lineNumber, pos, title, desc, palette );
	
	mItems.push_back( item );
	mImages.push_back( image );
}

void PaletteBrowserApp::mouseMove( MouseEvent event )
{
	mNewMouseOverItem = mItems.end();
	
	for( vector<Item>::iterator itemIt = mItems.begin(); itemIt != mItems.end(); ++itemIt ) {
		if( itemIt->isPointIn( event.getPos() ) && !itemIt->getSelected() ) {
			mNewMouseOverItem = itemIt;
			break;
		}
	}
	
	if( mNewMouseOverItem == mItems.end() ){
		if( mMouseOverItem != mItems.end() && mMouseOverItem != mSelectedItem ){
			mMouseOverItem->mouseOff( timeline() );
			mMouseOverItem = mItems.end();
		}
	} else {
	
		if( mNewMouseOverItem != mMouseOverItem && !mNewMouseOverItem->getSelected() ){
			if( mMouseOverItem != mItems.end() && mMouseOverItem != mSelectedItem )
				mMouseOverItem->mouseOff( timeline() );
			mMouseOverItem = mNewMouseOverItem;
			mMouseOverItem->mouseOver( timeline() );
		}
	}
}

void PaletteBrowserApp::mouseDown( MouseEvent event )
{
	if( mMouseOverItem != mItems.end() ){
		vector<Item>::iterator prevSelected = mSelectedItem;
		mSelectedItem = mMouseOverItem;
		
		// deselect previous selected item
		if( prevSelected != mItems.end() && prevSelected != mMouseOverItem ){
			prevSelected->deselect( timeline() );
			mBgImage = mFgImage;
			mBgColor = Color::white();
			timeline().apply( &mBgColor, Color::black(), 0.4f, EaseInQuad() );
		}
		
		// select current mouseover item
		mSelectedItem->select( timeline(), mLeftBorder );
		mFgImage = mImages[mSelectedItem->mIndex];
		mFgAlpha = 0.0f;
		timeline().apply( &mFgAlpha, 1.0f, 0.4f, EaseInQuad() );
		
		mMouseOverItem = mItems.end();
		mNewMouseOverItem = mItems.end();
	}
}

void PaletteBrowserApp::update()
{
	for( vector<Item>::iterator itemIt = mItems.begin(); itemIt != mItems.end(); ++itemIt ) {
		itemIt->update();
	}
}

void PaletteBrowserApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) );
	gl::enableAlphaBlending();
	
	gl::setMatricesWindowPersp( getWindowSize() );
	
	glEnable( GL_TEXTURE_2D );
	
	// draw background image
	if( mBgImage ){
		gl::color( mBgColor );
		gl::draw( mBgImage, getWindowBounds() );
	}
	
	// draw foreground image
	if( mFgImage ){
		gl::color( ColorA( 1.0f, 1.0f, 1.0f, mFgAlpha ) );
		gl::draw( mFgImage, getWindowBounds() );
	}
	
	// draw swatches
	mSwatchLargeTex.enableAndBind();
	if( mSelectedItem != mItems.end() )
		mSelectedItem->drawSwatches();
	
	mSwatchSmallTex.bind();
	for( vector<Item>::const_iterator itemIt = mItems.begin(); itemIt != mItems.end(); ++itemIt ) {
		if( ! itemIt->getSelected() )
			itemIt->drawSwatches();
	}
	
	// turn off textures and draw bgBar
	glDisable( GL_TEXTURE_2D );
	for( vector<Item>::const_iterator itemIt = mItems.begin(); itemIt != mItems.end(); ++itemIt ) {
		itemIt->drawBgBar();
	}
	
	// turn on textures and draw text
	gl::color( Color( 1.0f, 1.0f, 1.0f ) );
	for( vector<Item>::const_iterator itemIt = mItems.begin(); itemIt != mItems.end(); ++itemIt ) {
		itemIt->drawText();
	}
}


CINDER_APP_BASIC( PaletteBrowserApp, RendererGl )
