#include "Tube.h" 
#include "cinder/gl/gl.h"

void addQuadToMesh( TriMesh& mesh, const Vec3f& P0, const Vec3f& P1, const Vec3f& P2, const Vec3f& P3 )
{
	mesh.appendVertex( P0 );
	mesh.appendVertex( P1 );
	mesh.appendVertex( P2 );
	mesh.appendVertex( P3 );
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

Tube::Tube( const BSpline3f& bspline, const std::vector<Vec3f>& prof ) 
: mNumSegs( 16 ),
  mBSpline( bspline ), 
  mProf( prof ),
  mScale0( 1 ),
  mScale1( 1 )
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
	
		Vec3f P = mBSpline.getPosition( t );
		mPs.push_back( P );
		
		Vec3f T = mBSpline.getDerivative( t );
		mTs.push_back( T.normalized() );
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
			Vec3f prevT = mTs[i - 1];
			Vec3f curT  = mTs[i];
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
		Vec3f p0, p1, p2;		
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

		
	    Vec3f t = (p1 - p0).normalized();
		Vec3f n = t.cross(p2 - p0).normalized();
		if( n.length() == 0.0f ) {
			int i = fabs( t[0] ) < fabs( t[1] ) ? 0 : 1;
			if( fabs( t[2] ) < fabs( t[i] ) ) 
				i = 2;
				
			Vec3f v( 0.0f, 0.0f, 0.0f ); 
			v[i] = 1.0;
			n = t.cross( v ).normalized();
		}
		Vec3f b = t.cross( n );	
	
		Matrix44f& m = mFrames[i];
		m.at( 0, 0 ) = b.x;
		m.at( 1, 0 ) = b.y;
		m.at( 2, 0 ) = b.z;
		m.at( 3, 0 ) = 0;
		
		m.at( 0, 1 ) = n.x;
		m.at( 1, 1 ) = n.y;
		m.at( 2, 1 ) = n.z;
		m.at( 3, 1 ) = 0;
		
		m.at( 0, 2 ) = t.x;
		m.at( 1, 2 ) = t.y;
		m.at( 2, 2 ) = t.z;
		m.at( 3, 2 ) = 0;
		
		m.at( 0, 3 ) = mPs[i].x;
		m.at( 1, 3 ) = mPs[i].y;
		m.at( 2, 3 ) = mPs[i].z;
		m.at( 3, 3 ) = 1;
	}
}

void Tube::buildMesh( ci::TriMesh *tubeMesh  )
{
	if( ( mPs.size() != mFrames.size() ) || mFrames.size() < 3 || mProf.empty() ) 
		return;
		
	tubeMesh->clear();

	for( int i = 0; i < mPs.size() - 1; ++i ) {
		Matrix44f mat0 = mFrames[i];
		Matrix44f mat1 = mFrames[i + 1];

		float r0 = sin( (float)(i + 0)/(float)(mPs.size() - 1)*3.141592f );
		float r1 = sin( (float)(i + 1)/(float)(mPs.size() - 1)*3.141592f );
		float rs0 = (mScale1 - mScale0)*r0 + mScale0;
		float rs1 = (mScale1 - mScale0)*r1 + mScale0;

		for( int ci = 0; ci < mProf.size(); ++ci ) {
			int idx0 = ci;
			int idx1 = (ci == (mProf.size() - 1)) ? 0 : ci + 1;
			Vec3f P0 = mat0*(mProf[idx0]*rs0);
			Vec3f P1 = mat0*(mProf[idx1]*rs0);
			Vec3f P2 = mat1*(mProf[idx1]*rs1);
			Vec3f P3 = mat1*(mProf[idx0]*rs1);
			addQuadToMesh( *tubeMesh, P0, P3, P2, P1 );
		}
	}
}

void Tube::drawPs( float lineWidth )
{
	if( mPs.empty() )
		return;

	glLineWidth( lineWidth );
	glBegin( GL_LINES );
	for( int i = 0; i < ( mPs.size() - 1 ); ++i ) {
		glVertex3f( mPs[i] );
		glVertex3f( mPs[i + 1] );
	}
	glEnd();
}

void Tube::drawTs( float lineLength , float lineWidth )
{
	if( mPs.empty() || mTs.empty() )
		return;

	glLineWidth( lineWidth );
	glBegin( GL_LINES );
	for( int i = 0; i < ( mPs.size() - 1 ); ++i ) {
		glVertex3f( mPs[i] );
		glVertex3f( mPs[i] + mTs[i]*lineLength );
	}
	glEnd();
}

void Tube::drawFrames( float lineLength, float lineWidth )
{
	if( mPs.empty() || mFrames.empty() )
		return;

	glLineWidth( lineWidth );
	glBegin( GL_LINES );
	for( int i = 0; i < ( mPs.size() - 1 ); ++i ) {	
	
		Vec3f xAxis = mFrames[i].transformVec( Vec3f::xAxis() );
		Vec3f yAxis = mFrames[i].transformVec( Vec3f::yAxis() );
		Vec3f zAxis = mFrames[i].transformVec( Vec3f::zAxis() );

		glLineWidth( lineWidth );
		gl::color( Color( 1, 0.5f, 0 ) );
 		glVertex3f( mPs[i] );
 		glVertex3f( mPs[i] + xAxis*lineLength );
		
		gl::color( Color( 1, 0, 1 ) );
 		glVertex3f( mPs[i] );
 		glVertex3f( mPs[i] + yAxis*lineLength );
	
		glLineWidth( 2*lineWidth );
		gl::color( Color( 0, 1, 1 ) );
 		glVertex3f( mPs[i] );
 		glVertex3f( mPs[i] + zAxis*lineLength );		
	}
	glEnd();
}

void Tube::drawFrameSlices( float scale )
{
	gl::color( ColorA( 1, 1, 1, 0.15f ) );
	for( int i = 0; i < mFrames.size(); ++i ) {	
		gl::pushModelView();
		gl::multModelView( mFrames[i] );

		glBegin( GL_QUADS );

		glVertex3f( Vec3f( -1,  1, 0 )*scale );
		glVertex3f( Vec3f(  1,  1, 0 )*scale );
		glVertex3f( Vec3f(  1, -1, 0 )*scale );
		glVertex3f( Vec3f( -1, -1, 0 )*scale );

		glEnd();
		gl::popModelView();
	}

	gl::color( ColorA( 1, 1, 1, 0.75f ) );
	for( int i = 0; i <  mFrames.size(); ++i ) {	
		gl::pushModelView();
		gl::multModelView( mFrames[i] );
		
		glBegin( GL_LINES );
		
		glVertex3f( Vec3f( -1,  1, 0 )*scale );
		glVertex3f( Vec3f(  1,  1, 0 )*scale );

		glVertex3f( Vec3f(  1,  1, 0 )*scale );
		glVertex3f( Vec3f(  1, -1, 0 )*scale );

		glVertex3f( Vec3f(  1, -1, 0 )*scale );
		glVertex3f( Vec3f( -1, -1, 0 )*scale );

		glVertex3f( Vec3f( -1, -1, 0 )*scale );
		glVertex3f( Vec3f( -1,  1, 0 )*scale );			
		
		glEnd();
		gl::popModelView();
	}
}

void makeCircleProfile( std::vector<Vec3f>& prof, float rad, int segments )
{
	prof.clear();
	float dt = 6.28318531f/(float)segments;
	for( int i = 0; i < segments; ++i ) {
		float t = i*dt;
		prof.push_back( Vec3f( cos( t )*rad, sin( t )*rad, 0 ) );
	}
}

void makeStarProfile( std::vector<Vec3f>& prof, float rad )
{
	Vec3f A(  0.0f,  1.0f, 0.0f );
	Vec3f B(  0.5f, -1.0f, 0.0f );
	Vec3f C(  0.0f, -0.5f, 0.0f );
	Vec3f D( -0.5f, -1.0f, 0.0f );

	prof.clear();
	prof.push_back( A );
	prof.push_back( A + (B-A)*0.3f );
	prof.push_back( A + (B-A)*0.3f + Vec3f( 0.75f, 0, 0 ) );
	prof.push_back( A + (B-A)*0.6f );
	prof.push_back( B );
	prof.push_back( C );
	prof.push_back( D );
	prof.push_back( A + (D-A)*0.6f );
	prof.push_back( A + (D-A)*0.3f - Vec3f( 0.75f, 0, 0 ) );
	prof.push_back( A + (D-A)*0.3f );
	
	for( int i = 0; i < prof.size(); ++i ) {
		prof[i] *= rad;
	}
}

void makeHypotrochoid( std::vector<Vec3f>& prof, float rad )
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
		prof.push_back( Vec3f( x*rad, y*rad, 0 ) );
	}
}

void makeEpicycloid( std::vector<Vec3f>& prof, float rad )
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
		prof.push_back( Vec3f( x*rad, y*rad, 0 ) );
	}
}
