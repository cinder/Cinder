//
//	qbPoly.h
//
//  Created by Roger Sodre on 01/07/2011
//	Copyright 2011 Studio Avante. All rights reserved.
//
#pragma once

#include "cinder/Cinder.h"
#include "cinder/Vector.h"
#include "cinder/Xml.h"
#include "cinder/DataSource.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Vbo.h"
#import <string>
#import <vector>
#import <list>


namespace cinder { namespace qb {
	
	enum enumPolytype {
		QBPOLY_NONE = -1,	// not a polygon (0 vertices)
		QBPOLY_POINT,		// single point
		QBPOLY_POLYLINE,	// open polygon (line strip)
		QBPOLY_POLYGON,		// closed generic polygon (line loop)
		QBPOLY_TRI,			// triangle
		QBPOLY_QUAD,		// generic quad
		QBPOLY_SQUARE,		// square
		QBPOLY_RECT,		// rectangle
		// count
		QBPOLY_COUNT
	};
	
	
	//
	// A Poly Vertex
	class qbPolyVertex : public Vec3f
	{
	public:
		
		qbPolyVertex() : Vec3f(), mDist(0), mProg(0), mDup(false) {}
		qbPolyVertex( const Vec3f & src, bool dup=false ) : Vec3f(src), mDist(0), mProg(0), mDup(dup)  {}
		
		float		mDist;		// The distance from poly start
		float		mProg;		// The prog from poly start	( distance / perimeter )
		bool		mDup;		// duplicated?
		
	protected:
		
	};
	
	//
	// A Poly
	class qbPoly
	{
	public:
		
		qbPoly() : mType(QBPOLY_NONE), bClosed(false), mLayer(0) {}
		
		void copyFrom ( qbPoly & src, bool _asPolygons=false );
		void addVertex (Vec3f _v );
		void finish();
		void finishOptimized();
		void randomize( int _max=10 );
		void close();
		void setLayer( const int n )					{ mLayer = n; }
		void setLayerName( const std::string & n )		{ mLayerName = n; }
		void setName( const std::string & n )			{ mName = n; }
		
		void makeRect ( float x, float y, float w, float h )		{ this->makeRect( Vec3f(x,y,0), w, h); }
		void makeCircle ( float x, float y, float rx, float ry )	{ this->makeCircle( Vec3f(x,y,0), rx, ry); }
		//void makeLine ( float x0, float y0, float x1, float y1 )	{ this->makeLine( Vec3f(x0,y0,0), Vec3f(x1,y1,0)); }

		void makeTriangle ( Vec3f v0, Vec3f v1, Vec3f v2 );
		void makeRect ( Vec3f v, float w, float h );
		void makeCircle ( Vec3f v, float rx, float ry );
		//void makeLine ( Vec3f v0, Vec3f v0 );
		void makeCurve ( Vec3f p0, Vec3f p1, Vec3f c0, Vec3f c1 );

		int				getLayer()					{ return mLayer; }
		std::string &	getLayerName()				{ return mLayerName; }
		std::string &	getName()					{ return mName; }
		Rectf &			getBounds()					{ return mBounds; }
		Vec3f &			getCenter()					{ return mCenter; }
		std::vector<qbPolyVertex> &	getVertices()	{ return mVertices; }
		qbPolyVertex &	getVertex(int v)			{ return mVertices[v]; }
		int				getVertexCount()			{ return (int) mVertices.size(); }
		int				getType()					{ return mType; }
		float			getPerimeter()				{ return mPerimeter; }
		bool			isClosed()					{ return bClosed; }

		float			getLoopingProgAt(float prog);
		float			getDistanceAt(float prog)			{ return (mPerimeter * prog); }
		float			getLoopingDistanceAt(float prog);

		Vec3f			getPointAt(float prog);
		Vec3f			getPointAtDistance(float dist);

	protected:
		
		void calcPerimeter();
		void calcBounds();
		void makeBounds (float x1, float y1, float x2, float y2);
		
		void makeMesh();
		
		std::vector<qbPolyVertex>	mVertices;
		std::string					mLayerName;
		std::string					mName;
		Rectf						mBounds;
		Vec3f						mCenter;
		int							mLayer;
		int							mType;
		bool						bClosed;
		float						mPerimeter;
		//int						numLayer;
		//int						numObj;

		gl::VboMesh					mMesh;
	};
	
	/////////////////////////////////
	//
	// POLY GROUP
	//
	class qbPolyGroup {
	public:
		qbPolyGroup() : mPerimeter(0) {};
		
		void		clear()						{ mPolys.clear(); }
		void		addPoly( qbPoly & _poly );
		void		randomize();
		void		draw();
		void		drawStroked();
		
		int			getLayerCount()				{ return mLayers.size(); }
		float		getPerimeter()				{ return mPerimeter; }
		int			getPolyCount()				{ return mPolys.size(); }
		qbPoly &	getPoly( int n )			{ return mPolys[n]; }
		
	protected:

		std::vector<std::string>	mLayers;		// Layer names
		std::vector<qbPoly>			mPolys;			// Polys
		float						mPerimeter;		// Length from start to finish
		Rectf						mViewBox;		// Viewport
	};

} } // cinder::qb

