#pragma once

#include "cinder/BSpline.h"
#include "cinder/Matrix.h"
#include "cinder/TriMesh.h"
using namespace cinder;

class Tube {
public:
	
	Tube();	
	Tube( const Tube& obj );
	Tube( const BSpline3f& bspline, const std::vector<Vec3f>& prof );	
	~Tube() {}
	
	Tube& operator=( const Tube& rhs );

	void					setBSpline( const BSpline3f& bspline ) { mBSpline = bspline; }	
	void					setProfile( const std::vector<Vec3f>& prof ) { mProf = prof; }	
	void					setNumSegments( int n ) { mNumSegs = n; }

	void					sampleCurve();
	
	// Builds parallel transport frames for each curve sample
	void					buildPTF();
	
	// Builds frenet frames for each curve sample - optional method
	void					buildFrenet();
	
	// Creates a 'tubed' mesh using the profile points
	void					buildMesh( ci::TriMesh *tubeMesh );

	void					draw();
	void					drawPs( float lineWidth = 1.0f );	
	void					drawTs( float lineLength = 1.0f, float lineWidth = 1.0f );
	void					drawFrames( float lineLength = 1.0f, float lineWidth = 1.0f  );
	
	//void					drawNs( float lineLength = 1.0f, float lineWidth = 1.0f );
	//void					drawBs( float lineLength = 1.0f, float lineWidth = 1.0f );
	
	void					drawFrameSlices( float scale = 1.0f );

private:
	int						mNumSegs;		// Number of segments to sample on the b-spline
	
	BSpline3f				mBSpline;		// b-spline path
	std::vector<Vec3f>		mProf;			// points to tube (read extrude)
	float					mScale0;		// min scale of profile along curves
	float					mScale1;		// max scale of profile along curves
		
	std::vector<Vec3f>		mPs;			// Points in b-spline sample
	std::vector<Vec3f>		mTs;			// Tangents in b-spline sample
	std::vector<Matrix44f>	mFrames;		// Coordinate frame at each b-spline sample
};


void makeCircleProfile( std::vector<Vec3f>& prof, float rad = 0.25f, int segments = 16 );
void makeStarProfile( std::vector<Vec3f>& prof, float rad = 0.25f );
void makeHypotrochoid( std::vector<Vec3f>& prof, float rad = 0.25f );
void makeEpicycloid( std::vector<Vec3f>& prof, float rad = 0.25f );
