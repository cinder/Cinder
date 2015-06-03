#include "Tube.h" 
#include "cinder/gl/gl.h"

void addQuadToMesh( TriMesh& mesh, const vec3& P0, const vec3& P1, const vec3& P2, const vec3& P3 )
{
	mesh.appendPosition( P0 );
	mesh.appendPosition( P1 );
	mesh.appendPosition( P2 );
	mesh.appendPosition( P3 );
	int vert0 = mesh.getNumVertices() - 4;
	int vert1 = mesh.getNumVertices() - 1;
	int vert2 = mesh.getNumVertices() - 2;
	int vert3 = mesh.getNumVertices() - 3;
	mesh.appendTriangle( vert0, vert1, vert3 );
	mesh.appendTriangle( vert3, vert1, vert2 );
}

Tube::Tube() 
: mNumSegs( 16 ),
  mScale0( 1 ),
  mScale1( 1 )
{
	makeCircleProfile( mProf );
}

Tube::Tube( const Tube& obj )
: mNumSegs( obj.mNumSegs ),
  mBSpline( obj.mBSpline ),	
  mProf( obj.mProf ),
  mScale0( obj.mScale0 ),
  mScale1( obj.mScale1 ),
  mPs( obj.mPs ),
  mTs( obj.mTs ),
  mFrames( obj.mFrames )
{
}

Tube::Tube( const BSpline3f& bspline, const std::vector<vec3>& prof ) 
	: mNumSegs( 16 ), mBSpline( bspline ), mProf( prof ), mScale0( 1 ), mScale1( 1 )
{
}

Tube& Tube::operator=( const Tube& rhs ) 
{
	if( &rhs != this ) {
		mNumSegs	= rhs.mNumSegs;
		mBSpline	= rhs.mBSpline;
		mProf		= rhs.mProf;
		mScale0		= rhs.mScale0;
		mScale1		= rhs.mScale1;
		mPs			= rhs.mPs;
		mTs			= rhs.mTs;
		mFrames		= rhs.mFrames;
	}
	return *this;
}

void Tube::sampleCurve()
{
	mPs.clear();
	mTs.clear();

	float dt = 1.0f/(float)mNumSegs;
	for( int i = 0; i < mNumSegs; ++i ) {
		float t = i*dt;
	
		vec3 P = mBSpline.getPosition( t );
		mPs.push_back( P );
		
		vec3 T = mBSpline.getDerivative( t );
		mTs.push_back( normalize( T ) );
	}
}

void Tube::buildPTF() 
{
	mFrames.clear();

	int n = mPs.size();
	// Make sure we have at least 3 points because the first frame requires it
	if( n >= 3 ) {
		mFrames.resize( n );
		// Make the parallel transport frame
		mFrames[0] = firstFrame( mPs[0], mPs[1],  mPs[2] );
		// Make the remaining frames - saving the last
		for( int i = 1; i < n - 1; ++i ) {
			vec3 prevT = mTs[i - 1];
			vec3 curT  = mTs[i];
			mFrames[i] = nextFrame( mFrames[i - 1], mPs[i - 1], mPs[i], prevT, curT );
		}
		// Make the last frame
		mFrames[n - 1] = lastFrame( mFrames[n - 2], mPs[n - 2], mPs[n - 1] );
	}
}

void Tube::buildFrenet()
{
	mFrames.clear();
	
	int n = mPs.size();
	mFrames.resize( n );
	
	for( int i = 0; i < n; ++i ) {
		vec3 p0, p1, p2;		
		if( i < (n - 2) ) {
			p0 = mPs[i];
			p1 = mPs[i + 1];
			p2 = mPs[i + 2];
		}
		else if( i == (n - 2) ) {
			p0 = mPs[i - 1];
			p1 = mPs[i];
			p2 = mPs[i + 1];
		}	
		else if( i == (n - 1) ) {
			p0 = mPs[i - 3];
			p1 = mPs[i - 2];
			p2 = mPs[i - 1];
		}
		
	    vec3 t = normalize( p1 - p0 );
		vec3 n = normalize( cross( t, p2 - p0 ) );
		if( length( n ) == 0.0f ) {
			int i = fabs( t[0] ) < fabs( t[1] ) ? 0 : 1;
			if( fabs( t[2] ) < fabs( t[i] ) ) 
				i = 2;
				
			vec3 v( 0.0f, 0.0f, 0.0f ); 
			v[i] = 1.0f;
			n = normalize( cross( t, v ) );
		}
		vec3 b = cross( t, n );
	
		mat4& m = mFrames[i];
		m[0] = vec4( b, 0 );
		m[1] = vec4( n, 0 );
		m[2] = vec4( t, 0 );
		m[3] = vec4( mPs[i], 1 );
	}
}

void Tube::buildMesh( ci::TriMesh *tubeMesh  )
{
	if( ( mPs.size() != mFrames.size() ) || mFrames.size() < 3 || mProf.empty() ) 
		return;
		
	tubeMesh->clear();

	for( int i = 0; i < mPs.size() - 1; ++i ) {
		mat4 mat0 = mFrames[i];
		mat4 mat1 = mFrames[i + 1];

		float r0 = sin( (float)(i + 0)/(float)(mPs.size() - 1)*3.141592f );
		float r1 = sin( (float)(i + 1)/(float)(mPs.size() - 1)*3.141592f );
		float rs0 = (mScale1 - mScale0)*r0 + mScale0;
		float rs1 = (mScale1 - mScale0)*r1 + mScale0;

		for( int ci = 0; ci < mProf.size(); ++ci ) {
			int idx0 = ci;
			int idx1 = (ci == (mProf.size() - 1)) ? 0 : ci + 1;
			vec3 P0 = vec3( mat0 * vec4(mProf[idx0]*rs0, 1) );
			vec3 P1 = vec3( mat0 * vec4(mProf[idx1]*rs0, 1) );
			vec3 P2 = vec3( mat1 * vec4(mProf[idx1]*rs1, 1) );
			vec3 P3 = vec3( mat1 * vec4(mProf[idx0]*rs1, 1) );
			addQuadToMesh( *tubeMesh, P0, P3, P2, P1 );
		}
	}
}

void Tube::drawPs( float lineWidth )
{
	if( mPs.empty() )
		return;

	gl::lineWidth( lineWidth );
	gl::begin( GL_LINES );
	for( int i = 0; i < ( mPs.size() - 1 ); ++i ) {
		gl::vertex( mPs[i] );
		gl::vertex( mPs[i + 1] );
	}
	gl::end();
}

void Tube::drawTs( float lineLength , float lineWidth )
{
	if( mPs.empty() || mTs.empty() )
		return;

	gl::lineWidth( lineWidth );
	gl::begin( GL_LINES );
	for( int i = 0; i < ( mPs.size() - 1 ); ++i ) {
		gl::vertex( mPs[i] );
		gl::vertex( mPs[i] + mTs[i]*lineLength );
	}
	gl::end();
}

void Tube::drawFrames( float lineLength, float lineWidth )
{
	if( mPs.empty() || mFrames.empty() )
		return;

	gl::lineWidth( lineWidth );
	gl::begin( GL_LINES );
	for( int i = 0; i < ( mPs.size() - 1 ); ++i ) {	
	
		vec3 xAxis = vec3( mFrames[i] * vec4( 1, 0, 0, 0 ) );
		vec3 yAxis = vec3( mFrames[i] * vec4( 0, 1, 0, 0 ) );
		vec3 zAxis = vec3( mFrames[i] * vec4( 0, 0, 1, 0 ) );

		gl::lineWidth( lineWidth );
		gl::color( Color( 1, 0.5f, 0 ) );
 		gl::vertex( mPs[i] );
 		gl::vertex( mPs[i] + xAxis * lineLength );
		
		gl::color( Color( 1, 0, 1 ) );
 		gl::vertex( mPs[i] );
 		gl::vertex( mPs[i] + yAxis * lineLength );
	
		gl::lineWidth( 2*lineWidth );
		gl::color( Color( 0, 1, 1 ) );
 		gl::vertex( mPs[i] );
 		gl::vertex( mPs[i] + zAxis * lineLength );
	}
	gl::end();
}

void Tube::drawFrameSlices( float scale )
{
	gl::color( ColorA( 1, 1, 1, 0.15f ) );
	for( int i = 0; i < mFrames.size(); ++i ) {	
		gl::pushModelView();
		gl::multModelMatrix( mFrames[i] );

		gl::begin( GL_QUADS );

		gl::vertex( vec3( -1,  1, 0 )*scale );
		gl::vertex( vec3(  1,  1, 0 )*scale );
		gl::vertex( vec3(  1, -1, 0 )*scale );
		gl::vertex( vec3( -1, -1, 0 )*scale );

		gl::end();
		gl::popModelView();
	}

	gl::color( ColorA( 1, 1, 1, 0.75f ) );
	for( int i = 0; i <  mFrames.size(); ++i ) {	
		gl::pushModelView();
		gl::multModelMatrix( mFrames[i] );
		
		gl::begin( GL_LINES );
		
		gl::vertex( vec3( -1,  1, 0 )*scale );
		gl::vertex( vec3(  1,  1, 0 )*scale );

		gl::vertex( vec3(  1,  1, 0 )*scale );
		gl::vertex( vec3(  1, -1, 0 )*scale );

		gl::vertex( vec3(  1, -1, 0 )*scale );
		gl::vertex( vec3( -1, -1, 0 )*scale );

		gl::vertex( vec3( -1, -1, 0 )*scale );
		gl::vertex( vec3( -1,  1, 0 )*scale );			
		
		gl::end();
		gl::popModelView();
	}
}

void makeCircleProfile( std::vector<vec3>& prof, float rad, int segments )
{
	prof.clear();
	float dt = 6.28318531f/(float)segments;
	for( int i = 0; i < segments; ++i ) {
		float t = i*dt;
		prof.push_back( vec3( cos( t )*rad, sin( t )*rad, 0 ) );
	}
}

void makeStarProfile( std::vector<vec3>& prof, float rad )
{
	vec3 A(  0.0f,  1.0f, 0.0f );
	vec3 B(  0.5f, -1.0f, 0.0f );
	vec3 C(  0.0f, -0.5f, 0.0f );
	vec3 D( -0.5f, -1.0f, 0.0f );

	prof.clear();
	prof.push_back( A );
	prof.push_back( A + (B-A)*0.3f );
	prof.push_back( A + (B-A)*0.3f + vec3( 0.75f, 0, 0 ) );
	prof.push_back( A + (B-A)*0.6f );
	prof.push_back( B );
	prof.push_back( C );
	prof.push_back( D );
	prof.push_back( A + (D-A)*0.6f );
	prof.push_back( A + (D-A)*0.3f - vec3( 0.75f, 0, 0 ) );
	prof.push_back( A + (D-A)*0.3f );
	
	for( int i = 0; i < prof.size(); ++i ) {
		prof[i] *= rad;
	}
}

void makeHypotrochoid( std::vector<vec3>& prof, float rad )
{
	float a = 1;
	float b = 0.142857f;
	float h = b;
	int n = 32;
	prof.clear();
	float dt = 6.28318531f/(float)n;
	for( int i = 0; i < n; ++i ) {
		float t = i*dt;
		float x = (a - b)*cos( t ) + h*cos( t*(a - b)/b );
		float y = (a - b)*sin( t ) - h*sin( t*(a - b)/b );
		prof.push_back( vec3( x*rad, y*rad, 0 ) );
	}
}

void makeEpicycloid( std::vector<vec3>& prof, float rad )
{
	float a = 1;
	float b = 0.125f;
	int n = 48;
	prof.clear();
	float dt = 6.28318531f/(float)n;
	for( int i = 0; i < n; ++i ) {
		float t = i*dt;
		float x = (a + b)*cos( t ) + b*cos( t*(a + b)/b );
		float y = (a + b)*sin( t ) + b*sin( t*(a + b)/b );
		prof.push_back( vec3( x*rad, y*rad, 0 ) );
	}
}
