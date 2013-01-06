//
//	TextRibbon.cpp
//	Instascope
//
//	Created by Greg Kepler on 6/26/12.
//	Copyright (c) 2012 The Barbarian Group. All rights reserved.
//

#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/Text.h"
#include "cinder/gl/Texture.h"
#include "cinder/Timeline.h"

#include "TextRibbon.h"
#include "Resources.h"

static const int TEXT_PADDING_X = 15;

using namespace std;
using namespace ci;
using namespace ci::app;

TextRibbon::TextRibbon()
	: mCol(Color::black()), mTextCol(Color::white())
{
	mUserFont = Font( loadResource( RES_OPEN_SANS ), 16 );
	mTagFont = Font( loadResource( RES_KREON_BOLD ), 16 );
}

void TextRibbon::update( string tag, string user )
{
	mTag = (tag!="") ? "#" + tag : "";
	mUser = user;
	
	mCurPos.value() = Vec2f(-60, 0);
	mCurAlpha = 0.0f;
	
	makeText();
	
	ribbonIn( 4.0 );
}

void TextRibbon::ribbonIn( float delay )
{
	// animate ribbon in
	app::timeline().apply( &mCurAlpha, 1.0f, 0.4f, EaseOutQuint()).delay(delay);
	app::timeline().apply( &mCurPos, Vec2f::zero(), 0.4f, EaseOutQuint()).delay(delay);	
}

void TextRibbon::ribbonOut( float delay )
{
	// animate ribbon out
	app::timeline().apply( &mCurAlpha, 0.0f, 0.4f, EaseInQuint()).delay(delay);
	app::timeline().apply( &mCurPos, Vec2f(-60, 0), 0.4f, EaseInQuint()).delay(delay);
}

void TextRibbon::makeText()
{
	// reset the textures
	mTagTex.reset();
	mUserTex.reset();
	
	// Create the texture for the text
	if( ! mTag.empty() ) {
		mTagBox = TextBox().alignment(TextBox::LEFT).font(mTagFont).size(Vec2i(200, TextBox::GROW)).text(mTag);
		mTagBox.setColor(ColorA(mTextCol.r, mTextCol.g, mTextCol.b, 1));
		mTagBox.setBackgroundColor(ColorA(0, 0, 0, 0));
		mTagTex = mTagBox.render();
	}
	
	mUserBox = TextBox().alignment( TextBox::LEFT ).font( mUserFont ).size( Vec2i( 250, TextBox::GROW ) ).text( "@" + mUser );
	mUserBox.setColor(ColorA(mTextCol.r, mTextCol.g, mTextCol.b, 1));
	mUserBox.setBackgroundColor( ColorA( 0, 0, 0, 0) );
	mUserTex = mUserBox.render();
	
	float ribbonWidth = mTagBox.measure().x + mUserBox.measure().x + 30;
	mRibbonSize = Vec2f(ribbonWidth, 50);
	mTextPos = Vec2f(0, getWindowHeight() - mRibbonSize.y - 20);
}

// Draws the solid shape behind the text
void TextRibbon::drawTextShape()
{
	int boxH = (int)mRibbonSize.y;
	int boxW = (int)mRibbonSize.x;
	
	float farPt = sin((M_PI/3)*2) * boxH;
	float offsetX = mCurPos.value().x;
	float offsetY = mCurPos.value().y;
	
	Vec2f points[4];
	points[0] = Vec2f(offsetX + 0,				offsetY + 0);
	points[1] = Vec2f(offsetX + 0,				offsetY + boxH);
	points[2] = Vec2f(offsetX + boxW,			offsetY + 0);
	points[3] = Vec2f(offsetX + boxW + farPt,	offsetY + boxH);
	
	glColor4f( mCol.r, mCol.g, mCol.b, mCurAlpha );
	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 2, GL_FLOAT, 0, &points[0].x );
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	glDisableClientState( GL_VERTEX_ARRAY );
	glColor4f( 1, 1, 1, 1 );
}

void TextRibbon::draw()
{
	glPushMatrix();
	gl::translate(mTextPos);
  
	drawTextShape();
	
	float spacing = 0;
	gl::color( 1, 1, 1, mCurAlpha );
	
	// Now draw the text textures:
	// check it the texture exists and if mTagBox has a height (meaning that there's something in that texture)
	if( mTagTex && mTagBox.measure().y > 0 ) {
		spacing = 5;
		gl::draw( mTagTex, Vec2f(mCurPos.value().x + TEXT_PADDING_X, (mRibbonSize.y - mTagBox.measure().y)/2) );
	}
	
	if( mUserTex )
		gl::draw( mUserTex, Vec2f(mCurPos.value().x + TEXT_PADDING_X + mTagBox.measure().x + spacing, (mRibbonSize.y - mUserBox.measure().y)/2) );
	gl::color( 1, 1, 1 );
	glPopMatrix();
}
