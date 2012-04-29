//
//  qbPoly.cpp
//
//  Created by Roger Sodre on 01/07/2011
//  Copyright 2011 Studio Avante. All rights reserved.
//

#include "qbPoly.h"
#include "qbCube.h"
#include "cinder/Utilities.h"
#include "cinder/Rand.h"
#include "cinder/TriMesh.h"
#include "cinder/Triangulate.h"

#include <errno.h>

namespace cinder { namespace qb {
	
	//////////////////////////////////////////////////////////
	//
	// POLY GROUPS
	//
	void qbPolyGroup::addPoly( qbPoly & _poly )
	{
		if (_poly.getVertexCount() > 0)
		{
			mPolys.push_back( _poly );
			mPerimeter += _poly.getPerimeter();
			// new layer?
			if ( std::find(mLayers.begin(), mLayers.end(), _poly.getLayerName()) == mLayers.end() )
			{
				mLayers.push_back( _poly.getLayerName() );
				//printf("___________qbPolyGroup::addPoly:: NEW LAYER [%s]\n",_poly.getLayerName().c_str());
			}
		}
	}
	void qbPolyGroup::draw()
	{
		for (int p = 0 ; p < mPolys.size() ; p++)
			qb::drawSolidPoly( mPolys[p] );
	}
	void qbPolyGroup::drawStroked()
	{
		for (int p = 0 ; p < mPolys.size() ; p++)
			qb::drawStrokedPoly( mPolys[p] );
	}
	void qbPolyGroup::randomize()
	{
		std::vector<qbPoly> newPolys;
		for (int n = 0 ; n < this->getPolyCount() ; n++)
		{
			qbPoly p;
			p.copyFrom( this->getPoly(n), true );
			p.randomize();
			newPolys.push_back( p );
		}
		mPolys = newPolys;
	}

	
	
	
	//////////////////////////////////////////////////////////
	//
	// POLYS
	//
	Vec3f qbPoly::getPointAt( float prog )
	{
		float p = math<float>::clamp( prog, 0.0f, 1.0f );
		if ( p == 0.0 )
			return mVertices.front();
		for ( int i = 1 ; i < mVertices.size() ; i++ )
		{
			if( p <= mVertices[i].mProg )
			{
				float pp = lmap<float>(p, mVertices[i-1].mProg, mVertices[i].mProg, 0, 1);
				return  mVertices[i-1].lerp( pp, mVertices[i] );
			}
		}
		return mVertices.back();
	}
	float qbPoly::getLoopingProgAt( float prog )
	{
		if ( bClosed )
			return prog;
		else if ( prog <= 0.5f )
			return ( prog * 2.0f );
		else
			return ( 1.0 - ((prog-0.5f) * 2.0f) );
	}
	
	float qbPoly::getLoopingDistanceAt(float dist)
	{
		if ( bClosed )
			return dist;
		// clamp
		float d = dist;
		while ( d < -mPerimeter * 0.5f )
			d += mPerimeter;
		while (d > mPerimeter * 0.5f )
			d -= mPerimeter;
		if ( d < 0.0f )
			return lmap( d, -mPerimeter * 0.5f, 0.0f, mPerimeter, 0.0f );
		else if ( d <= mPerimeter * 0.5f )
			return lmap( d, 0.0f, mPerimeter * 0.5f, 0.0f, mPerimeter );
		else
			return lmap( d, mPerimeter * 0.5f, mPerimeter, mPerimeter, 0.0f );
	}

	Vec3f qbPoly::getPointAtDistance( float dist )
	{
		if (mPerimeter == 0)
			return ( mVertices.size() ? mVertices[0] : Vec3f::zero() );
		// clamp
		float d = dist;
		while ( d < 0 )
			d += mPerimeter;
		while (d > mPerimeter )
			d -= mPerimeter;
		if ( d == 0 )
			return mVertices.front();
		for ( int i = 1 ; i < mVertices.size() ; i++ )
		{
			if( d <= mVertices[i].mDist )
			{
				float dd = lmap<float>(d, mVertices[i-1].mDist, mVertices[i].mDist, 0, 1);
				return  mVertices[i-1].lerp( dd, mVertices[i] );
			}
		}
		return mVertices.back();
	}
	
	void qbPoly::copyFrom( qbPoly & src, bool _asPolygons )
	{
		mVertices.empty();
		bClosed = src.isClosed();
		mLayerName = src.getLayerName();
		mLayer = src.getLayer();
		mName = src.getName();
		int t = src.getType();
		// convert quads to polygon?
		if ( _asPolygons && (t == QBPOLY_QUAD || t == QBPOLY_SQUARE || t == QBPOLY_RECT) )
		{
			mType = QBPOLY_POLYGON;
			this->addVertex( src.getVertex(0) );
			this->addVertex( src.getVertex(1) );
			this->addVertex( src.getVertex(3) );
			this->addVertex( src.getVertex(2) );
			this->close();
			this->finish();
		}
		// copy as is
		else
		{
			for (int v = 0 ; v < src.getVertexCount() ; v++)
				this->addVertex( src.getVertex(v) );
			this->finishOptimized();
		}
	}
	void qbPoly::makeTriangle ( Vec3f v0, Vec3f v1, Vec3f v2 )
	{
		mType = QBPOLY_TRI;
		mVertices.empty();
		this->addVertex( v0 );
		this->addVertex( v1 );
		this->addVertex( v2 );
		this->close();
		this->finish();
	}
	void qbPoly::makeRect( Vec3f v, float w, float h )
	{
		mType = ( w == h ? QBPOLY_SQUARE : QBPOLY_RECT );
		mVertices.empty();
		this->addVertex( v + Vec3f( 0, 0, 0) );
		this->addVertex( v + Vec3f( w, 0, 0) );
		this->addVertex( v + Vec3f( w, h, 0) );
		this->addVertex( v + Vec3f( 0, h, 0) );
		this->close();
		//mBounds = Rectf( x, y, x+w, y+h );
		//mPerimeter = (w * 2) + (h * 2);
		this->finish();
	}
	void qbPoly::makeCircle( Vec3f v, float rx, float ry )
	{
		mType = QBPOLY_POLYGON;
		mVertices.empty();
		int pts = 8 + ( std::max<float>(rx,ry) / 20.0f ) * 4;
		for ( int n = 0 ; n < pts ; n++ )
		{
			float a = n * (M_TWO_PI / pts);
			float xx = v.x + rx * sin(a);
			float yy = v.y + ry * cos(a);
			this->addVertex( Vec3f( xx, yy, v.z) );
		}
		this->close();
		//mBounds = Rectf( x-rx, y-ry, x+rx, y+ry );
		//mPerimeter = 2.0f * M_PI * sqrt( (rx*rx + ry*ry) / 2.0f );	// http://www.mathsisfun.com/geometry/ellipse-perimeter.html
		this->finish();
	}
	void qbPoly::makeCurve ( Vec3f p0, Vec3f p1, Vec3f c0, Vec3f c1 )
	{
		bClosed = false;
		mType = QBPOLY_POLYGON;
		mVertices.empty();
		int pts = 2 + ( p0.distance( p1 ) / 21.0f );
		for (int n = 0 ; n < pts ; n++)
		{
			float t =  n / (float)(pts-1);
			Vec3f v = bezierInterpRef<Vec3f,float>( p0, c0, c1, p1, t );
			this->addVertex( v );
			//printf("  bezier %d / %.2f = %.1f %.1f %.1f\n",n,t,v.x,v.y,v.z);
		}
	}

	//
	// Poly Construction
	void qbPoly::addVertex (Vec3f _v )
	{
		bool dup = false;
		for (int v = 0 ; v < mVertices.size() ; v++)
		{
			if ( mVertices[v] == _v )
			{
				dup = true;
				// close if got back to 1st point
				if ( v == 0 && mVertices.size() > 2 )
					bClosed = true;
				//printf("...duplicated vertex!\n");
				//return;
			}
		}
		mVertices.push_back( qbPolyVertex( _v, dup ) );
	}
	void qbPoly::close()
	{
		if ( mVertices.front() != mVertices.back() )
			this->addVertex( this->getVertex(0) );
		bClosed = true;
	}
	
	//
	// FINISHING A POLY
	void qbPoly::finish()
	{
		this->calcBounds();
		this->calcPerimeter();
		// Make Cinder Mesh
		this->makeMesh();
	}
	
	// Optimize will 
	void qbPoly::finishOptimized()
	{
		this->calcBounds();
		
		// Set TYPE
		if ( mVertices.size() == 0 )
		{
			mType = QBPOLY_NONE;
		}
		else if ( mVertices.size() == 1 )
		{
			mType = QBPOLY_POINT;
		}
		else if ( mVertices.size() == 2 )
		{
			mType = QBPOLY_POLYLINE;
		}
		else
		{
			if ( ! bClosed )
			{
				mType = QBPOLY_POLYLINE;
			}
			else
			{
				if ( mVertices.size() == 3 )
				{
					mType = QBPOLY_TRI;
				}
				else if (mVertices.size() == 4)
				{
					// order vertices
					// let's try to find a vertex for each corner
					// 0 > 1
					//   / 
					// 2 > 3
					float dist = 0.0;
					int order_i[4];
					Vec3f order_v[4];
					for (int o = 0 ; o < 4 ; o++)
					{
						// get corner vertex
						// TODO:: incluir isto em Rectf como mBounds[o]
						Vec2f p = ( o==0 ? mBounds.getUpperLeft() : 
								   ( o==1 ? mBounds.getUpperRight() : 
									( o==2 ? mBounds.getLowerLeft() : mBounds.getLowerRight() ) ) );
						// find closer vertex from poly
						for (int v = 0 ; v < 4 ; v++)
						{
							float d = p.distance( mVertices[v].xy() );
							if ( v == 0 || d < dist )
							{
								dist = d;
								order_i[o] = v;
								order_v[o] = mVertices[v];
							}
						}
					}
					// ordering does not give us a vertex per corner: make polygon
					if ( order_i[0] == order_i[1] || 
						order_i[0] == order_i[2] || 
						order_i[0] == order_i[3] || 
						order_i[1] == order_i[2] || 
						order_i[1] == order_i[3] || 
						order_i[2] == order_i[3] )
					{
						mType = QBPOLY_POLYGON;
					}
					// perfect square/rectangle
					else if ( mVertices[0].x == mVertices[1].x && 
							 mVertices[2].x == mVertices[3].x && 
							 mVertices[0].y == mVertices[2].y && 
							 mVertices[1].y == mVertices[3].y )
					{
						mType = ( mBounds.getWidth() == mBounds.getHeight() ? QBPOLY_SQUARE : QBPOLY_RECT );
					}
					// optimized quad for qb::drawQuad()
					else
					{
						// save reorder
						for (int v = 0 ; v < 4 ; v++)
							mVertices[v] = order_v[v];
						mType = QBPOLY_QUAD;
					}
				}
				else
				{
					mType = QBPOLY_POLYGON;
				}
			}
		}
		
		this->calcPerimeter();
		
		// Make Cinder Mesh
		this->makeMesh();
	}
	
	void qbPoly::calcPerimeter()
	{
		// calc perimeter
		float perim = 0;
		for (int n = 0 ; n < mVertices.size() ; n++)
		{
			if ( n > 0 )
				perim += mVertices[n].distance( mVertices[n-1] );
			mVertices[n].mDist = perim;
		}
		mPerimeter = perim;
		// calc vertexes distance based on perimeter
		for (int n = 0 ; n < mVertices.size() ; n++)
		{
			mVertices[n].mProg = ( mVertices[n].mDist / mPerimeter );
			//printf("__prog  perim %.2f  vert %d  dist %.2f  prog %.2f\n",mPerimeter,n,mVertices[n].mDist,mVertices[n].mProg);
		}
	}
	void qbPoly::calcBounds()
	{
		float dMin, dMax;
		for (int v = 0 ; v < mVertices.size() ; v++)
		{
			if ( v == 0 )
			{
				mBounds = Rectf( mVertices[v].xy(), mVertices[v].xy() );
				dMin = dMax = mVertices[v].z;
			}
			else
			{
				if (mVertices[v].x < mBounds.x1)
					mBounds.x1 = mVertices[v].x;
				if (mVertices[v].x > mBounds.x2)
					mBounds.x2 = mVertices[v].x;
				if (mVertices[v].y < mBounds.y1)
					mBounds.y1 = mVertices[v].y;
				if (mVertices[v].y > mBounds.y2)
					mBounds.y2 = mVertices[v].y;
				if (mVertices[v].z < dMin)
					dMin = mVertices[v].z;
				if (mVertices[v].z > dMax)
					dMax = mVertices[v].z;
			}
		}
		mCenter = Vec3f( mBounds.getUpperLeft() + mBounds.getSize() * 0.5f, (dMax-dMin) * 0.5f );
	}
	
	//
	// Make Noise
	void qbPoly::randomize( int _max )
	{
		std::vector<qbPolyVertex> newVertices;
		for (int n = 0 ; n < mVertices.size() ; n++)
		{
			Vec3f off = Vec3f( Rand::randFloat( -_max, _max ), Rand::randFloat( -_max, _max ), 0 );
			newVertices.push_back( qbPolyVertex( mVertices[n] + off ) );
		}
		mVertices = newVertices;
		this->finishOptimized();
	}
	
	
	
	
	//
	// Coinder Mesh
	void qbPoly::makeMesh()
	{
		Path2d path;
		for (int v = 0 ; v < mVertices.size() ; v++)
		{
			if (v == 0)
				path.moveTo( mVertices[v].xy() );
			else
				path.lineTo( mVertices[v].xy() );
		}
		if (bClosed)
			path.close();
		
		TriMesh2d mesh = Triangulator( path ).calcMesh();
		mMesh = gl::VboMesh( mesh );
	}
	
} } // cinder::qb


