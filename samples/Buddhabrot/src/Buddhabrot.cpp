/*
 *  Created by Robert Hodgin on 1/13/10.
 *  Based on the Java implementation by Hans Liss: http://hans.liss.pp.se
 *  available here: http://hans.liss.pp.se/fractals/buddhabrot.java
 */


#include "cinder/app/AppBasic.h"
#include "cinder/gl/Texture.h"
#include "cinder/Rand.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"
#include "cinder/gl/GlslProg.h"


using namespace ci;
using namespace ci::app;
using namespace std;

const int TEXTURE_WIDTH = 1024, TEXTURE_HEIGHT = 1024;
const double TWOPI = 6.28318531;

class BuddhabrotApp : public AppBasic {
 public:
	void prepareSettings( Settings *settings );
	void setup();
	void resetVariables();
	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void mouseUp( MouseEvent event );
	void keyDown( KeyEvent event );
	void update();
	void draw();
	void runBrot();
	int mandel( double cx, double cy );
	void mutate( double &x, double &y );
	double transitionProbability( int n1, int n2 );
	
	int mWidth;
	int mHeight;
	int mTotalPixels;
	
	double mXCorner, mYCorner;	// gc_x gc_y :: the (upperleft?) corner of the scan area
	double mXDim, mYDim;		// gd_x gd_y :: the size of the scan area
	double mXScaler, mYScaler;	// gv_x gv_y :: scale variable(?)
	
	int mBoundingX0, mBoundingY0;
	int mBoundingX1, mBoundingY1;
	int mXLast, mYLast;
	
	int mRLimit;
	int mGLimit;
	int mBLimit;
	
	vector<int> mRedHistory;
	vector<int> mGreenHistory;
	vector<int> mBlueHistory;
	
	int mRedBuffer[ TEXTURE_WIDTH * TEXTURE_HEIGHT ];
	int mGreenBuffer[ TEXTURE_WIDTH * TEXTURE_HEIGHT ];
	int mBlueBuffer[ TEXTURE_WIDTH * TEXTURE_HEIGHT ];
	
	Surface mSurface;
	gl::Texture	mTexture;
	
	int mVMaxR;
	int mVMaxG;
	int mVMaxB;

	int mMaxIterations;
	int mLastHits;
};

void BuddhabrotApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 1024, 1024 );
	settings->setFrameRate( 60.0f );
	settings->setFullScreen( false );
}

void BuddhabrotApp::setup()
{
	Rand::randomize();
	mTotalPixels	= TEXTURE_WIDTH * TEXTURE_HEIGHT;
	mSurface	= Surface( TEXTURE_WIDTH, TEXTURE_HEIGHT, false );
	mTexture	= gl::Texture( mSurface );
	
	mWidth		= TEXTURE_WIDTH;//getWindowWidth();
	mHeight		= TEXTURE_HEIGHT;//getWindowHeight();
	
	mXCorner	= -0.5;
	mYCorner	=  0.0;
	mXDim		=  3.0;
	mYDim		=  3.0;
	
	/*			MINI MANDELBROT ISLAND LEFT
	mXCorner	= -1.25275;
    mYCorner	= -0.343;
    mXDim		= 0.0025;
    mYDim		= 0.0025;
	*/
	
	mRLimit		= 100000;
	mGLimit		= 15000;
	mBLimit		= 1000;
	
	mMaxIterations = max( mRLimit, max( mGLimit, mBLimit ) );
	
	int tmpW = (int)( (double)mWidth  * mXDim/mYDim );
	int tmpH = (int)( (double)mHeight * mYDim/mXDim );
	if( tmpW < mWidth )
		mWidth = tmpW;
	else if( tmpH < mHeight )
		mHeight = tmpH;
	
	mXScaler =  mXDim / (double)mWidth;
	mYScaler =  mYDim / (double)mHeight;
	
	resetVariables();
}

void BuddhabrotApp::resetVariables()
{
	mBoundingX0 = 0;	mBoundingY0 = 0;
	mBoundingX1 = 0;	mBoundingY1 = 0;
	
	for( int i=0; i<mTotalPixels; i++ ){
		mRedBuffer[i]		= 0;
		mGreenBuffer[i]		= 0;
		mBlueBuffer[i]		= 0;
    }
	
	mVMaxR = 0; mVMaxG = 0; mVMaxB = 0;
	
	mRedHistory.clear();
	mGreenHistory.clear();
	mBlueHistory.clear();
}


void BuddhabrotApp::mouseDown( MouseEvent event )
{
	int mouseX  = event.getY() * TEXTURE_HEIGHT;
	int mouseY  = event.getX() * TEXTURE_WIDTH;
	
	mBoundingX0 = mouseX;
	mBoundingY0 = mouseY;
	mBoundingX1 = mouseX;
	mBoundingY1 = mouseY;
	mXLast		= mouseX;
	mYLast		= mouseY;
}

void BuddhabrotApp::mouseDrag( MouseEvent event )
{
	int mouseX = event.getY() * TEXTURE_HEIGHT;
	int mouseY = event.getX() * TEXTURE_WIDTH;
	
	if( mouseX != mBoundingX1 || mouseY != mBoundingY1 ){
		mBoundingX1 = mouseX;
		mBoundingY1 = mouseY;
		
		int dx = abs( mBoundingX1 - mBoundingX0 );
		int dy = abs( mBoundingY1 - mBoundingY0 );
		
		if( dx < dy * TEXTURE_WIDTH/TEXTURE_HEIGHT ){
		
			dx = dy * TEXTURE_WIDTH/TEXTURE_HEIGHT;
			if( mBoundingX1 < mBoundingX0 ){
				mBoundingX1 = mBoundingX0 - dx;
			} else {
				mBoundingX1 = mBoundingX0 + dx;
			}
			
		} else if( dy < dx * TEXTURE_HEIGHT/TEXTURE_WIDTH ){
		
			dy= dx * TEXTURE_HEIGHT/TEXTURE_WIDTH;
			if( mBoundingY1 < mBoundingY0 ){
				mBoundingY1 = mBoundingY0 - dy;
			} else {
				mBoundingY1 = mBoundingY0 + dy;
			}
			
		}
	}
}

void BuddhabrotApp::mouseUp( MouseEvent event )
{
	int mouseX  = event.getY() * TEXTURE_HEIGHT;
	int mouseY  = event.getX() * TEXTURE_WIDTH;
	
	mBoundingX1 = mouseX;
	mBoundingY1 = mouseY;
		
	int dx = abs( mBoundingX1 - mBoundingX0 );
	int dy = abs( mBoundingY1 - mBoundingY0 );
	double x0 = min( mBoundingX0, mBoundingX1 );
	double y0 = min( mBoundingY0, mBoundingY1 );
	
	if( dx == 0 && dy == 0 ){
		mXCorner += mXDim * ( (double)( x0 - TEXTURE_WIDTH/2 ) / (double)TEXTURE_WIDTH );
		mYCorner += mYDim * ( (double)( y0 - TEXTURE_HEIGHT/2 ) / (double)TEXTURE_HEIGHT );
		mXDim /= 2;
		mYDim /= 2;
	} else {
	
		// CODEBLOCK REPEAT... FIX
		if( dx < dy * TEXTURE_WIDTH/TEXTURE_HEIGHT ){
		
			dx = dy * TEXTURE_WIDTH/TEXTURE_HEIGHT;
			if( mBoundingX1 < mBoundingX0 ){
				mBoundingX1 = mBoundingX0 - dx;
			} else {
				mBoundingX1 = mBoundingX0 + dx;
			}
			
		} else if( dy < dx * TEXTURE_HEIGHT/TEXTURE_WIDTH ){
		
			dy= dx * TEXTURE_HEIGHT/TEXTURE_WIDTH;
			if( mBoundingY1 < mBoundingY0 ){
				mBoundingY1 = mBoundingY0 - dy;
			} else {
				mBoundingY1 = mBoundingY0 + dy;
			}
			
		}
		// CODEBLOCK REPEAT... END
		
		mXCorner += mXDim * ( (double)( x0 + dx/2 - TEXTURE_WIDTH/2 ) / (double)TEXTURE_WIDTH );
		mYCorner += mYDim * ( (double)( y0 + dy/2 - TEXTURE_HEIGHT/2 ) / (double)TEXTURE_HEIGHT );
		mXDim /= (double)dx/(double)TEXTURE_WIDTH;
		mYDim /= (double)dy/(double)TEXTURE_HEIGHT;
	}
	
	mXScaler = mXDim/(double)TEXTURE_WIDTH;
	mYScaler = mYDim/(double)TEXTURE_HEIGHT;
	
	resetVariables();
}

void BuddhabrotApp::keyDown( KeyEvent event )
{
	if( event.getChar() == 'f' ) {
		setFullScreen( ! isFullScreen() );
	}
	else if( event.getChar() == 'o' ) {

	}
}

void BuddhabrotApp::runBrot()
{
    int oldhits      = -1;
    uint32_t n       = 2000000000;
    int oldt         = 0;
	
    double cx		 = 0;
    double cy		 = 0;
	
    const int ITERATIONS_PER_RUN	= 10000;
	
	for( int i=0; i < ITERATIONS_PER_RUN; i++ ) {
		if( oldhits == -1 ){
			oldt         = mandel( cx, cy );
			oldhits      = mLastHits;
		}
		double xTemp	= cx;
		double yTemp	= cy;
		mutate( xTemp, yTemp );
		
		int newt		= mandel( xTemp, yTemp );
		int newhits		= mLastHits;
		double t1		= transitionProbability( newt, oldt );
		double t2		= transitionProbability( oldt, newt );
		
		if( oldhits > 0 ){
			double alpha = min( 1.0, exp( log( (double)newhits * t1 ) - log( (double)oldhits * t2 ) ) );
			if( alpha > Rand::randFloat() ){
				cx			= xTemp;
				cy			= yTemp;
				oldt		= newt;
				oldhits		= newhits;
			}
		} else {
			cx			= xTemp;
			cy			= yTemp;
			oldt		= newt;
			oldhits		= newhits;
		}
		if( ( newhits > 0 ) && ( ( ( n-- ) % 20 ) == 0 ) ){
			if( n == 0 ){
				n = 2000000000;
			}
		}
    }
}



void BuddhabrotApp::update()
{
	runBrot();
	
	Surface::Iter iter = mSurface.getIter();
	
	int x = 0;
	int y = 0;
	int index;
	
	while( iter.line() ) {
		y = iter.y();
		
		while( iter.pixel() ) {
			x = iter.x();
			index = y * TEXTURE_WIDTH + x;
			
			double r = 0;
			double g = 0;
			double b = 0;
			
			int rMax = mVMaxR;
			while( rMax > 0 && mRedHistory[ rMax ] <= 4 ){
				rMax --;
			}
			
			int gMax = mVMaxG;
			while( gMax > 0 && mGreenHistory[ gMax ] <= 4 ){
				gMax --;
			}
			
			int bMax = mVMaxB;
			while( bMax > 0 && mBlueHistory[ bMax ] <= 4 ){
				bMax --;
			}
			
			if( rMax > 0 ){
				r = (double)mRedBuffer[index]/(double)rMax;
				if( r > 1 ) r = 1;
			}
			
			if( gMax > 0 ){
				g = (double)mGreenBuffer[index]/(double)gMax;
				if( g > 1 ) g = 1;
			}
			
			if( bMax > 0 ){
				b = (double)mBlueBuffer[index]/(double)bMax;
				if( b > 1 ) b = 1;
			}
			
			iter.r() = (int)( r * 255 );
			iter.g() = (int)( g * 255 );
			iter.b() = (int)( b * 255 );
		}
	}
	mTexture = gl::Texture( mSurface );
}

void BuddhabrotApp::draw()
{
	glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable( GL_TEXTURE_2D );

	gl::draw( mTexture );
}

int BuddhabrotApp::mandel( double cx, double cy )
{
	int i = mMaxIterations;
	int xPos, yPos;
	int hits = 0;
	double zx = cx;
	double zy = cy;
	double zx2 = cx*cx;
	double zy2 = cy*cy;
	
	while( ( i>0 ) && ( zx2 + zy2 < 4.0 ) ){
		zy = 2*zx*zy + cy;
		zx = zx2-zy2 + cx;
		zx2 = zx*zx;
		zy2 = zy*zy;
		
		i --;
	}
	
	if( i>0 ){
		int n	= i;
		i		= mMaxIterations;
		zx		= cx;
		zy		= cy;
		zx2		= cx*cx;
		zy2		= cy*cy;
		
		while( i>0 && ( zx2 + zy2 < 4.0 ) ){
			zy	= 2*zx*zy + cy;
			zx	= zx2-zy2 + cx;
			zx2 = zx*zx;
			zy2 = zy*zy;
			
			i --;
			xPos = (int)( ( zx - ( mXCorner - mXDim/2 ) ) / mXScaler );
			yPos = (int)( ( zy - ( mYCorner - mYDim/2 ) ) / mXScaler ); // <<<<<---- POSSIBLE TYPO
			
			if( xPos >= 0 && xPos < TEXTURE_WIDTH && yPos >= 1 && yPos < TEXTURE_HEIGHT ){
				hits ++;
				int index = yPos + TEXTURE_HEIGHT * xPos;
				
				
				if( n > mMaxIterations - mRLimit ){
					mRedBuffer[index] ++;
					
					if( mRedBuffer[index] > mVMaxR ){
						for( int j=mRedHistory.size(); j<=mRedBuffer[index]; ++j )
							mRedHistory.push_back( 0 );
						
						mVMaxR = mRedBuffer[index];
					}
					
					mRedHistory[ mRedBuffer[index] ] ++;
				}
				
				if( n > mMaxIterations - mGLimit ){
					mGreenBuffer[index] ++;
					
					if( mGreenBuffer[index] > mVMaxG ){
						for( int j=mGreenHistory.size(); j<=mGreenBuffer[index]; ++j )
							mGreenHistory.push_back( 0 );
						
						mVMaxG = mGreenBuffer[index];
					}
					
					mGreenHistory[ mGreenBuffer[index] ] ++;
				}
				
				if( n > mMaxIterations - mBLimit ){
					mBlueBuffer[index] ++;
					
					if( mBlueBuffer[index] > mVMaxB ){
						for( int j=mBlueHistory.size(); j<=mBlueBuffer[index]; ++j )
							mBlueHistory.push_back( 0 );
							
						mVMaxB = mBlueBuffer[index];
					}
					
					mBlueHistory[ mBlueBuffer[index] ] ++;
				}
			}
		}
	}
	
	mLastHits = hits;
	return i;
}

void BuddhabrotApp::mutate( double &x, double &y )
{
	if( Rand::randFloat() > 0.95f ){
		x = Rand::randFloat( -2.0f, 2.0f );
		y = Rand::randFloat( -2.0f, 2.0f );
	}
	else {
		double phi = Rand::randFloat( TWOPI );
		double r = ( mXDim / 100.0 ) * exp( Rand::randFloat( 8.0f ) );

		//double mx = x + r * cos( phi );
		//double my = y + r * sin( phi );		
		//x = mx;
		//y = my;
		
		x += r * cos( phi );
		y += r * sin( phi );
	}
}

double BuddhabrotApp::transitionProbability( int n1, int n2 )
{
	return ( 1 - ( ( double )n1/mMaxIterations ) ) / ( 1 - ( ( double )n2/mMaxIterations ) );
}

CINDER_APP_BASIC( BuddhabrotApp, RendererGl );
