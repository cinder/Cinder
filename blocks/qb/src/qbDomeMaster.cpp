//
//  qbDomeMaster.h
//
//  Created by Roger Sodre on 01/07/2011
//  Copyright 2011 Studio Avante. All rights reserved.
//

#include "qbDomeMaster.h"
#include "cinder/Rand.h"
#include "cinder/CinderMath.h"
#include "qb.h"


namespace cinder { namespace qb {
	
	
	void qbDomeMaster::setup( int w, int h )
	{
		mSize = Vec2i( w, h );
		mCenter = mSize * 0.5;
		mCenterf = Vec3f( mCenter.x, mCenter.y, 0 );
		mDiameter = h;
		mRadius = h * 0.5;
		
		// Make Length of each pixel to the center
		mLength = (float*) realloc( mLength, sizeof(float) * w * h );
		for ( int y = 0 ; y < h ; y++ )
			for ( int x = 0 ; x < w ; x++ )
				mLength[DXYI(x,y)] = mCenter.distance( Vec2i( x, y ) );
		
		// Make mask
		{
			mMask = (bool*) realloc( mMask, sizeof(bool) * w * h );
			Surface8u mMaskSurface( w, h, true ); // width, height, alpha?
			Surface8u::Iter iter = mMaskSurface.getIter();
			while( iter.line() ) {
				while( iter.pixel() ) {
					int i = DXYI(iter.x(),iter.y());
					mMask[i] = ( mLength[i] <= mRadius ? true : false );
					iter.r() = iter.g() = iter.b() = 0;
					iter.a() = ( mMask[i] ? 255 : 0 );
				}
			}
			mMaskTexture = gl::Texture( mMaskSurface );
		}
		
		// Make Vector of each pixel in the dome
		{
			mVectors = (Vec3f*) realloc( mVectors, sizeof(Vec3f) * w * h );
			mNormals = (Vec3f*) realloc( mNormals, sizeof(Vec3f) * w * h );
			mProjection = (Vec3f*) realloc( mProjection, sizeof(Vec3f) * w * h );
			Surface8u mVectorSurface( w, h, false ); // width, height, alpha?
			Surface8u::Iter iter = mVectorSurface.getIter();
			while( iter.line() ) {
				while( iter.pixel() ) {
					int i = DXYI(iter.x(),iter.y());
					if ( mMask[i] )
					{
						float d = mCenter.distance( Vec2i( iter.x(), iter.y() ) );
						mVectors[i].x = iter.x() - mCenter.x;
						mVectors[i].y = iter.y() - mCenter.y;
						mVectors[i].z = sqrt( mRadius * mRadius - d * d );
						mProjection[i] = mVectors[i];
						mVectors[i].normalize();
						mNormals[i].x = mVectors[i].x;
						mNormals[i].y = mVectors[i].y;
						mNormals[i].z = mVectors[i].z;
						iter.r() = lmap( mNormals[i].x, -1.0f, 1.0f, 0.0f, 1.0f ) * 255;
						iter.g() = lmap( mNormals[i].y, -1.0f, 1.0f, 0.0f, 1.0f ) * 255;
						iter.b() = lmap( mNormals[i].z, -1.0f, 1.0f, 0.0f, 1.0f ) * 255;
						
					}
					else
					{
						mVectors[i] = Vec3f::zero();
						mNormals[i] = Vec3f::zero();
						iter.r() = iter.g() = iter.b() = 0;
					}
				}
			}
			mNormalTexture = gl::Texture( mVectorSurface );
		}
	}
	
	
	//
	// Project a vector in space into the dome
	// p       = ( 0..w, 0..h)
	// returns = ( 0..w, 0..h)
	Vec3f qbDomeMaster::project2D( const Vec3f & p )
	{
		return (p - mCenterf).normalized() * mRadius + mCenterf;
	}
	// Mid-point between 2 points
	Vec3f qbDomeMaster::lerp2D( const float fact, const Vec3f & p0, const Vec3f & p1 )
	{
		Vec3f p = p0.lerp( fact, p1 );
		return this->project2D( p );
	}
	void qbDomeMaster::drawLineSegmented2D( const Vec3f & p0, const Vec3f & p1, float segmentSize )
	{
		glBegin( GL_LINE_STRIP );
		int segments = (int)( p0.distance( p1 ) / segmentSize ) + 1;
		for (int i = 0 ; i <= segments ; i++)
		{
			float prog = (1.0f / segments) * i;
			Vec3f mid = this->lerp2D( prog, p0, p1 );
			//glNormal3f( this->getNormal( Vec2i(mid.x, mid.y) ) );
			glVertex3f( mid );
		}
		glEnd();
	}
	
	
	
	//
	// Draw segmented line in 3D space with minimum segment size
	// p0/p1 = ( 0..w, 0..h)
	void qbDomeMaster::drawLineSegmented( const Vec3f & p0, const Vec3f & p1, float segmentSize )
	{
		glBegin( GL_LINE_STRIP );
		int segments = (int)( p0.distance( p1 ) / segmentSize ) + 1;
		for (int i = 0 ; i <= segments ; i++)
		{
			float prog = (1.0f / segments) * i;
			Vec3f mid = p0.lerp( prog, p1 ).normalized();
			//glNormal3f( this->getNormal( Vec2i(mid.x, mid.y) ) );
			glVertex3f( mid );
		}
		glEnd();
	}
	
	
	
	//
	// Intersection of a line startint from p0 to p1 into the dome
	// http://knol.google.com/k/koen-samyn/line-sphere-intersection-c/2lijysgth48w1/118#
	// http://en.wikipedia.org/wiki/Line%E2%80%93sphere_intersection
	Vec3f qbDomeMaster::getIntersectionFrom( const Vec3f & lp1, const Vec3f & lp2 )
	{
		Vec3f res = Vec3f::zero();
		float R = 1.0f;					// Radius
		Vec3f m = Vec3f::zero();		// dome center
		Vec3f d = lp1 - m;
		Vec3f ldir = lp2- lp1;
		float a = ldir.lengthSquared();
		float b = 2 * ldir.dot(d);
		float c = d.lengthSquared() - R*R;
		float D = b*b - 4*a*c;
		if ( D < 0 )
			res = Vec3f::zero();
		if ( D < FLT_EPSILON ){
			float t = (-b)/(2*a);
			Vec3f p1 = lp1 + t*ldir;
			res = p1;
		}else{
			float sq_D = sqrt(D);
			float t1 = (-b+sq_D)/(2*a);
			Vec3f p1 = lp1 + t1 * ldir;
			float t2 = (-b-sq_D)/(2*a);
			Vec3f p2 = lp1 + t2 * ldir;
			res = ( p1 - lp1 > p2 - lp1 ? p2 : p1 );	// return farthest
		}
		return res;
	}
	void qbDomeMaster::drawLineSegmentedFrom( const Vec3f & from, const Vec3f & p0, const Vec3f & p1, float segmentSize )
	{
		glBegin( GL_LINE_STRIP );
		int segments = (int)( p0.distance( p1 ) / segmentSize ) + 1;
		for (int i = 0 ; i <= segments ; i++)
		{
			float prog = (1.0f / segments) * i;
			Vec3f mid = this->getIntersectionFrom( from, p0.lerp( prog, p1 ) );
			glVertex3f( mid );
		}
		glEnd();
	}
	
	
	
	
	//
	// Draw Normals + mouse normal
	void qbDomeMaster::drawNormals()
	{
		this->drawNormals( QB_BOUNDS );
	}
	void qbDomeMaster::drawNormals( Rectf bounds )
	{
		float sz = QB_HEIGHT / 10.0f;
		gl::color( Color::white() );
		gl::draw( mNormalTexture, bounds );
		Vec2f mouse = _qb.getMousePos();
		Vec3f normal = _dome.getNormal( mouse / QB_SCALE );
		gl::drawLine( Vec3f(mouse,0), Vec3f(mouse,0) + normal * sz  );
	}
	void qbDomeMaster::drawMask()
	{
		gl::color( Color::white() );
		gl::enableAlphaBlending();
		gl::draw( mMaskTexture, QB_BOUNDS );
		gl::disableAlphaBlending();
	}
	void qbDomeMaster::drawGrid()
	{
		glLineWidth( 1 );

		//glPushMatrix();
		//glTranslatef( QB_CENTER );
		float r = QB_HEIGHT * 0.5;
		float seg = r / 45.0f;
		// verticais
		for ( float ang = 0.0f ; ang < 360.0 ; ang += 10.0f )
		{
			gl::color( ((int)ang % 30) == 0 ? Color::yellow()*0.9 : Color::white()*0.5 );
			Vec3f p0 = Vec3f( cos(toRadians(ang)), sin(toRadians(ang)), 0 );
			Vec3f p1 = Vec3f( 0, 0, r );
			this->drawLineSegmented( p0, p1, seg );
		}
		// horizontais
		gl::color( Color::white()*0.75 );
		for ( float az = 0.0f ; az < 90.0f ; az += 10.0f )
		{
			float r = cos( toRadians(az) );
			float z = sin( toRadians(az) );

			for ( float ang = 0.0f ; ang < 360.0 ; ang += 10.0f )
			{
				Vec3f p0 = Vec3f( cos(toRadians(ang)) * r, sin(toRadians(ang)) * r, z );
				Vec3f p1 = Vec3f( cos(toRadians(ang+15)) * r, sin(toRadians(ang+15)) * r, z );
				this->drawLineSegmented( p0, p1, seg );
			}
		}
		//glPopMatrix();
	}
	
	
} } // cinder::qb




