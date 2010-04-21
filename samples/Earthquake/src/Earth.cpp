/*
 *  Earth.cpp
 *  Earthquake
 *
 *  Created by Robert Hodgin on 6/30/09.
 *  Copyright 2009 The Barbarian Group. All rights reserved.
 *
 */

#include "Earth.h"
#include "cinder/gl/gl.h"
#include "cinder/Rand.h"

using namespace ci;
using std::list;

Earth::Earth()
{
}

Earth::Earth( ci::gl::Texture aTexDiffuse, ci::gl::Texture aTexNormal, ci::gl::Texture aTexMask )
{
	mLoc			= Vec3f::zero();
	mRadius			= 250.0f;
	mTexDiffuse		= aTexDiffuse;
	mTexNormal		= aTexNormal;
	mTexMask		= aTexMask;
	
	mMinMagToRender = 5.0f;
}

void Earth::setQuakeLocTip()
{
	for( list<Quake>::iterator quake = mQuakes.begin(); quake != mQuakes.end(); ++quake ) {
		quake->mLoc += Rand::randVec3f() * 0.001f;
		quake->mLocTip = mLoc + quake->mLoc * ( mRadius + quake->mMag * quake->mMag );
		quake->mLocTipAnchor = mLoc + quake->mLoc * mRadius;
	}
}

void Earth::update()
{
	repelLocTips();
}


void Earth::repelLocTips()
{
	float E, F;
	Vec3f dir;
	float distSqrd;
	float charge = -2.0f;
	
	for( list<Quake>::iterator quake1 = mQuakes.begin(); quake1 != mQuakes.end(); ++quake1 ) {
		float thisQTimesInvM = quake1->mMag * charge;
		
		for( list<Quake>::iterator quake2 = quake1; quake2 != mQuakes.end(); ++quake2 ) {
			if( quake1 != quake2 && quake1->mMag > mMinMagToRender && quake2->mMag > mMinMagToRender ){
				dir = quake1->mLocTip - quake2->mLocTip;
				distSqrd = dir.lengthSquared();
				
				if( distSqrd < 50.0f && distSqrd > 0.001f ) {
					float per = 1.0f - abs( distSqrd )/50.0f;
					E = charge / distSqrd;
					F = E * thisQTimesInvM;
					
					if( F > 2.0f )
						F = 2.0f;
					
					dir.normalize();
					dir *= F * per;
					
					quake1->mLocTip += dir;
					quake2->mLocTip -= dir;
				}
			}
		}
	}
	
	for( list<Quake>::iterator quake = mQuakes.begin(); quake != mQuakes.end(); ++quake ) {
		dir = quake->mLocTip - quake->mLocTipAnchor;
		float limit = ( 10.0f - quake->mMag ) * ( 10.0f - quake->mMag ) * 0.75f + 15.0f;
		if( dir.length() > limit ){
			dir.normalize();
			quake->mLocTip = quake->mLocTipAnchor + dir * limit;
		}
		
		quake->mLocTip.normalize();
		quake->mLocTip *= mRadius + quake->mMag + 10.0f;
	}
}


void Earth::draw()
{
	mTexDiffuse.bind( 0 );
	mTexNormal.bind( 1 );
	mTexMask.bind( 2 );
	
	gl::drawSphere( mLoc, mRadius, 64 );
}


void Earth::drawQuakes()
{
	for( list<Quake>::iterator it = mQuakes.begin(); it != mQuakes.end(); ++it ) {
		float mag = (it->mMag);
		gl::drawSphere( it->mLocTip, mag * 0.25f, 16 );
	}
}


void Earth::drawQuakeLabelsOnBillboard( const Vec3f &sRight, const Vec3f &sUp )
{
	Vec3f right;
	Vec3f up;
	float perLeft, perRight;
	
	glBegin( GL_QUADS );
	
	for( list<Quake>::iterator it = mQuakes.begin(); it != mQuakes.end(); ++it ) {
		it->mLabel.bind();
		
		float mag = (it->mMag);
		
		if( mag >= mMinMagToRender ){
			
			
			float x = it->mLocTip.x;
			float y = it->mLocTip.y;
			float z = it->mLocTip.z;
			
			float w = it->mLabel.getWidth() * 0.5f;
			float h = it->mLabel.getHeight() * 0.5f;
			
			// perLeft and Right is a partially implemented solution for getting the text
			// to move based on the rotation of the Earth. Will get back to it shortly.
			perLeft		= -1.0f;
			perRight	= 1.0f;
			right	= sRight * w;
			up		= sUp * h;
			
			glTexCoord2f( 0, 1 );
			glVertex3f( x + right.x * perLeft + up.x * perLeft,		y + right.y * perLeft + up.y * perLeft,		z + right.z * perLeft + up.z * perLeft );
			glTexCoord2f( 1, 1 );
			glVertex3f( x + right.x * perRight + up.x * perLeft,	y + right.y * perRight + up.y * perLeft,	z + right.z * perRight + up.z * perLeft );
			glTexCoord2f( 1, 0 );
			glVertex3f( x + right.x * perRight + up.x * perRight,	y + right.y * perRight + up.y * perRight,	z + right.z * perRight + up.z * perRight );
			glTexCoord2f( 0, 0 );
			glVertex3f( x + right.x * perLeft + up.x * perRight,	y + right.y * perLeft + up.y * perRight,	z + right.z * perLeft + up.z * perRight );
			
		}
	}
	
	glEnd();
}



void Earth::drawQuakeLabelsOnSphere( const Vec3f eyeNormal, const float eyeDist )
{
	float distMulti = eyeDist * 0.001f;
	for( list<Quake>::iterator it = mQuakes.begin(); it != mQuakes.end(); ++it ) {
		it->mLabel.bind();
		
		float mag = (it->mMag);
		
		if( mag >= mMinMagToRender ){
			float x = it->mLocTip.x;
			float y = it->mLocTip.y;
			float z = it->mLocTip.z;
			float dp = it->mLoc.dot( eyeNormal ) - 0.85;
			
			if( dp <= 0.0f )
				dp = 0.0f;
				
			float w = it->mLabel.getWidth() * dp * distMulti;
			float h = it->mLabel.getHeight() * dp * distMulti;
			
			Vec3f dir = mLoc - it->mLoc;
			dir.normalize();
			Vec3f perp1 = dir.cross( Vec3f::yAxis() );
			Vec3f perp2 = perp1.cross( dir );
			perp1		= perp2.cross( dir );

			
			glBegin( GL_QUADS );
			glTexCoord2f( 0, 1 );
			glVertex3f( x + perp1.x * w - perp2.x * h,		y + perp1.y * w - perp2.y * h,		z + perp1.z * w - perp2.z * h );
			glTexCoord2f( 1, 1 );
			glVertex3f( x - perp1.x * w - perp2.x * h,		y - perp1.y * w - perp2.y * h,		z - perp1.z * w - perp2.z * h );
			glTexCoord2f( 1, 0 );
			glVertex3f( x - perp1.x * w + perp2.x * h,		y - perp1.y * w + perp2.y * h,		z - perp1.z * w + perp2.z * h );
			glTexCoord2f( 0, 0 );
			glVertex3f( x + perp1.x * w + perp2.x * h,		y + perp1.y * w + perp2.y * h,		z + perp1.z * w + perp2.z * h );
			glEnd();
		}
	}
}



void Earth::drawQuakeVectors()
{
	float radialSubdivisions = 64.0f;
	Vec3f loc;
	Vec3f norm;
	Vec3f normFinal;

	for( list<Quake>::iterator it = mQuakes.begin(); it != mQuakes.end(); ++it ) {
		float mag = (it->mMag );
		
		if( mag >= mMinMagToRender ){
			Vec3f dir = mLoc - it->mLoc;
			dir.safeNormalize();
			Vec3f perp1 = dir.cross( Vec3f::yAxis() );
			Vec3f perp2 = perp1.cross( dir );
			perp1		= perp2.cross( dir );
			
			float largeRadius = mag;
			float smallRadius = 0.1f;
			
			glBegin( GL_TRIANGLE_STRIP );
			for( int i=0; i<radialSubdivisions; i++ ){
				float angle = ( (float)i/(radialSubdivisions-1.0f) - 0.5f ) * 6.283185f;
				float cosa  = cos( angle );
				float sina  = sin( angle );
				Vec3f locOffset = ( perp1 * cosa + perp2 * sina );
				
				norm	= perp1 * -sina + perp2 * cosa;
				normFinal = Vec3f( norm.y * dir.z - dir.y * norm.z, norm.z * dir.x - dir.z * norm.x, norm.x * dir.y - dir.x * norm.y );
				
				loc		= ( it->mLocTip ) + locOffset * smallRadius;
			
				glTexCoord2f( 0.0f, 1.0f );
				glVertex3f( loc.x, loc.y, loc.z );
				glNormal3f( it->mLoc.x, it->mLoc.y, it->mLoc.z );
				
				
				loc		= ( it->mLoc * mRadius ) + locOffset * largeRadius;
				
				glTexCoord2f( 0.0f, 0.0f );
				glVertex3f( loc.x, loc.y, loc.z );
				glNormal3f( normFinal.x, normFinal.y, normFinal.z );
				
				
			}
			glEnd();
		}
	}
}

/*
void Earth::drawQuakeVectors()
{
	float radialSubdivisions = 64.0f;
	Vec3f loc;
	Vec3f norm;
	
	for( list<Quake>::iterator it = mQuakes.begin(); it != mQuakes.end(); ++it ) {
		float mag = (it->mMag );
		
		if( mag >= mMinMagToRender ){
			Vec3f dir = mLoc - it->mLoc;
			dir.normalize();
			Vec3f perp1 = dir.cross( Vec3f::yAxis() );
			Vec3f perp2 = perp1.cross( dir );
			perp1		= perp2.cross( dir );
			
			float largeRadius = mag * mag * 0.35f + 5.0f;
			float smallRadius = 0.1f;
			
			glBegin( GL_TRIANGLE_FAN );
			
			glVertex3f( it->mLocTip.x, it->mLocTip.y, it->mLocTip.z );
			glNormal3f( it->mLoc.x, it->mLoc.y, it->mLoc.z );
				
			for( int i=0; i<radialSubdivisions; i++ ){
				float angle = ( (float)i/(radialSubdivisions-1.0f) ) * 6.28f;
				float cosa  = cos( angle );
				float sina  = sin( angle );
				Vec3f locOffset = ( perp1 * cosa + perp2 * sina );
				
				norm	= perp1 * -sina + perp2 * cosa;
				loc		= ( it->mLoc * mRadius ) + locOffset * largeRadius;
				
				glVertex3f( loc.x, loc.y, loc.z );
				glNormal3f( norm.y * dir.z - dir.y * norm.z, norm.z * dir.x - dir.z * norm.x, norm.z * dir.y - dir.x * norm.y );
			}
			glEnd();
		}

	}
}
*/


void Earth::addQuake( float aLat, float aLong, float aMag, std::string aTitle )
{
	mQuakes.push_back( Quake( aLat, aLong, aMag, aTitle ) );
}


void Earth::setMinMagToRender( float amt )
{
	mMinMagToRender += amt;
	if( mMinMagToRender < 2.0f ){
		mMinMagToRender = 2.0f; 
	} else if( mMinMagToRender > 8.0f ){
		mMinMagToRender = 8.0f ;
	}
}
