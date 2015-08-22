#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ImageIo.h"
#include "cinder/Rand.h"
#include "cinder/Timeline.h"
#include "cinder/Utilities.h"

#include "TrianglePiece.h"
#include "InstagramStream.h"
#include "TextRibbon.h"

using namespace ci;
using namespace ci::app;
using namespace std;

static const int MIRROR_DUR = 5;	// Duration of the mirror/kaleidoscope animation
static const int STILL_DUR = 5;		// Duration of the still image
static const string TAG = "";		// Instagram tag to search for

// Instagram Client Id - DO NOT USE THIS ONE!!! 
// Replace with your own client ID after registering your
// instagram application here http://instagram.com/developer/register/
static const string CLIENT_ID = "def20410b5134f7d9b828668775aee4a";	 

static const bool PREMULT = false;

class InstascopeApp : public App {
  private:
	void	setup();
	void	prepareSettings( Settings *settings );
	void	update();
	void	draw();
	
	void	updateMirrors(vector<TrianglePiece> *vec);
	void	drawMirrors(vector<TrianglePiece> *vec);
	void	checkImageLoaded();
	void	defineMirrorGrid();
	void	transitionMirrorIn(vector<TrianglePiece> *vec);
	
	void	changePhase(int newPhase);
	void	resetSample();
	void	newInstagram();
	void	continueCycle();
	void	mirrorOut();
	void	mirrorIn();
	void	imageLoaded();
	
	gl::TextureRef				mNewTex;				// the loaded texture
	gl::TextureRef				mBgTexture;				// texture for the still image
	gl::TextureRef				mMirrorTexture;			// texture for the mirror
	
	vector<TrianglePiece>		mTriPieces;				// stores alll of the kaleidoscope mirror pieces
	Anim<vec2>					mSamplePt;				// location of the piece of the image that is being sampled for the kaleidoscope
	int							mPhase;					// current phase of the app (0 or 1)
	Instagram					mCurInstagram;			// current instagram info
	
	shared_ptr<InstagramStream> mInstaStream;			// stream and loader for instagram data
	bool						mLoadingTexture;		// If the texture image is currently loading
	bool						mTextureLoaded;			// If the new texture has finished loading
	float						mSampleSize;			// Size of the image sample to grab for the kaleidoscope		   
	TextRibbon					*mTextRibbon;			// The text ribbon that shows up in "sill image" mode
	Anim<float>					mMirrorRot;				// rotation of the canvas in kaleidoscope mode
	bool						mPiecesIn;				// whether all of the mirror pieces are showing or not
	bool						mPhaseChangeCalled;		// if the app has been told to change phases or not
	bool						mFirstRun;				// if the app is on its first cycle
};

void InstascopeApp::prepareSettings( Settings *settings )
{
	settings->setWindowSize( 612, 612 );
#if ! defined( CINDER_COCOA_TOUCH )
	//settings->setResizable( false );
#endif
	settings->setFrameRate( 60 );
}

void InstascopeApp::setup()
{
	mPhase = 1;
	mFirstRun = true;
	mLoadingTexture = false;
	mTextureLoaded = false;
	mPhaseChangeCalled = false;
	
	mTextRibbon = new TextRibbon();
	
	// Popular images stream
	mInstaStream = make_shared<InstagramStream>( CLIENT_ID );
	// Image stream of a particular tag
	// mInstaStream = make_shared<InstagramStream>( TAG, CLIENT_ID );
	// Image stream in a particular area
	// mInstaStream = make_shared<InstagramStream>( vec2(40.720467,-74.00603), 5000, CLIENT_ID );

	continueCycle();
}

// This is where the new cycle should be started.
// A cycle consists of an instagram loading, the kaleidoscope animating for a few seconds, 
// transitioning out and landing on the still image for a few seconds. 
// This happens once a cycle is complete, meaning that the mirrors have transitioned out.
void InstascopeApp::continueCycle()
{
	mPhaseChangeCalled = false;		// it has net been told to change phases yet. It should only do that once shit is loaded
	defineMirrorGrid();				// redefine the kaleidoscope grid
	mTextureLoaded = false;			// This will trigger checking if an image has loaded in the update function
	newInstagram();					// grab the next instagram item
}

// Creates the grid of kaleidoscope mirrored triangles
void InstascopeApp::defineMirrorGrid()
{
	const int r = 1;
	const float tri_scale = (float)randInt(120, 400);
	
	// delete any previous pieces and clear the old vector
	mTriPieces.clear();
	
	vec2 pt1(0.0, 0.0);
	vec2 pt2(r, 0.0);
	vec2 pt3((cos(M_PI/3) * r), (sin(M_PI/3) * r));
    
	const float tri_width = distance( pt1, pt2 ) * tri_scale;
	const float tri_height = std::sqrt((tri_width*tri_width) - ((tri_width/2) * (tri_width/2)));
    
	const int amtX = ceil((((getWindowWidth()*2) - .5) / (1.5*(tri_width))) + 0.5f );
	const float w = ((amtX*1.5) + .5) * tri_width;
	const float xOffset = -(w-getWindowWidth())/2;
	
	const int amtY = ceil((getWindowHeight()*2) / (tri_height) + 0.5f );
	const float yOffset = -((amtY*(tri_height) - getWindowHeight())/2);
	
	// creates a series of hexagons composed of 6 triangles each
	for( int i = 0; i < amtX; i++ ) {
		float startX = ((tri_width) * 1.5 * i);
		startX += xOffset;
		for( int j = 0; j < amtY; j++ ) {
			float startY = (i%2==0) ? (tri_height*2*j) - (tri_height) : tri_height*2*j;
			startY += yOffset;
			
			for( int k = 0; k < 6; k++ ) {
				// because every other pieces is a mirror of the one next to it, every other has to be reversed on the x scale
				int scaleX = (k%2==0) ? 1 : -1;
				vec2 scale( scaleX * tri_scale, tri_scale );
				
				vec2 start( startX, startY );
				
				TrianglePiece tri = TrianglePiece(vec2(startX, startY), pt1, pt2, pt3, M_PI / 3 * k, scale);
				mTriPieces.push_back(tri);
			}
		}
	}
}

void InstascopeApp::newInstagram()
{
	if( mInstaStream->hasInstagramAvailable() )
		mCurInstagram = mInstaStream->getNextInstagram();
}

void InstascopeApp::changePhase( int newPhase )
{	
	mPhase = newPhase;
	
	switch( mPhase ) {
		// Mirror Mode
		case 0: {
			// transition all of the mirror pieces in
			transitionMirrorIn( &mTriPieces );
			resetSample(); 
			
			mMirrorRot = randFloat(M_PI, M_PI * 2);
			float newRot = mMirrorRot + randFloat(M_PI, M_PI/4);
			timeline().apply(&mMirrorRot, newRot, MIRROR_DUR, EaseInOutQuad());
			mPiecesIn = false;
		}
		break;
		// Still Image Mode
		case 1:
			// transition all of the mirror pieces out
			for( vector<TrianglePiece>::iterator piece = mTriPieces.begin(); piece != mTriPieces.end(); ++piece ){
				(*piece).setTransitionOut(.25);
			}
		break;
	}
}

void InstascopeApp::checkImageLoaded()
{
	mLoadingTexture = true;
	mTextureLoaded = false;
	
	gl::TextureRef tex = gl::Texture::create( mCurInstagram.getImage() );
	if( ! mCurInstagram.getImage().getData() )
		return;
	
	// THE IMAGE HAS BEEN LOADED 
	mLoadingTexture = false;
	mTextureLoaded = true;
	
	mNewTex = tex;
	mMirrorTexture = mNewTex;
}

void InstascopeApp::transitionMirrorIn( vector<TrianglePiece> *vec )
{
	for( int i = 0; i < vec->size(); i++ ) {
		float delay = randFloat( 0.1f, 0.5f );
		(*vec)[i].reset( delay, mMirrorTexture );
	}
	mTextRibbon->ribbonOut(0);
}

void InstascopeApp::imageLoaded()
{
	mPhaseChangeCalled = true;
	
	// we don't want to wait on the first go around
	int delayOffset = STILL_DUR;
	if( mFirstRun ) {
		mFirstRun = false;
		delayOffset = 0;
	}
	
	// This defines the length of time that we're in each phase
	timeline().add( [&] { changePhase(0); }, timeline().getCurrentTime() + delayOffset);
	timeline().add( [&] { changePhase(1); }, timeline().getCurrentTime() + delayOffset + MIRROR_DUR);
}

void InstascopeApp::resetSample()
{
	// reset sample pos
	mSampleSize = randInt(100, 300);
	mSamplePt.value().y = randFloat(0, getWindowWidth() - mSampleSize);
	mSamplePt.value().x = randFloat(0, getWindowHeight() - mSampleSize);
	
	vec2 newPos;
	int count = 0;
	// Try to find a good sample location thats within the window's frame.
	// Give up if we try and settle after a bunch of times, no big deal.
	do {
		newPos.x = randFloat(0, getWindowWidth() - mSampleSize/2);
		newPos.y = randFloat(0, getWindowHeight() - mSampleSize/2);
		count++;
	} while(count < 150	&& ((mSamplePt.value().x - newPos.x) < 100 || (mSamplePt.value().y - newPos.y) < 100));
	timeline().apply(&mSamplePt, newPos, MIRROR_DUR - 1, EaseInOutQuad()).delay(.5);
}

void InstascopeApp::update()
{
	// if mCurInstagram is undefined, then don't do anything else since there's nothing else to do
	if( mCurInstagram.isNull() ) {
		newInstagram();
		return;
	}
	
	// if the new texture is loading, but hasn't loaded yet, check again
	if( ! mTextureLoaded ) {
		checkImageLoaded();
	}
	else {
		// we want to call this only once the image has been loaded
		// if the texture has been loaded and the phase hasn't been called to change yet...
		if( ! mPhaseChangeCalled)
			imageLoaded();
		updateMirrors( &mTriPieces );
	}
}

void InstascopeApp::updateMirrors( vector<TrianglePiece> *vec )
{
	if( ! mMirrorTexture )
		return;
	
	vec2 mSamplePt1( -0.5, -(sin(M_PI/3)/3) );
	vec2 mSamplePt2( mSamplePt1.x + 1, mSamplePt1.y);
	vec2 mSamplePt3( mSamplePt1.x + (cos(M_PI/3)), mSamplePt1.y + (sin(M_PI/3)));
	
	mat3 mtrx( 1.0f );
	mtrx = glm::translate( mtrx, mSamplePt.value() );
	mtrx = glm::scale( mtrx, vec2( mSampleSize ) );
	mtrx = glm::rotate( mtrx, float((getElapsedFrames()*4)/2*M_PI) );
	
	mSamplePt1 = vec2( mtrx * vec3( mSamplePt1, 1.0 ) );
	mSamplePt2 = vec2( mtrx * vec3( mSamplePt2, 1.0 ) );
	mSamplePt3 = vec2( mtrx * vec3( mSamplePt3, 1.0 ) );
	
	mSamplePt1 /= mMirrorTexture->getSize();
	mSamplePt2 /= mMirrorTexture->getSize();
	mSamplePt3 /= mMirrorTexture->getSize();
	
	// loop through all the pieces and pass along the current texture and it's coordinates
	int outCount = 0;
	int inCount = 0;
	for( int i = 0; i < vec->size(); i++ ) {
		(*vec)[i].update( mMirrorTexture, mSamplePt1, mSamplePt2, mSamplePt3 );
		if( (*vec)[i].isOut() ) outCount++;
		if( (*vec)[i].isIn() ) inCount++;
	}
	
	// if all are out, then make a new mirror grid
	if( outCount > 0 && outCount == mTriPieces.size() ) {
		mirrorOut();
	}
	
	// if all the pieces are in
	if( inCount > 0 && inCount == mTriPieces.size() && ! mPiecesIn ) {
		mPiecesIn = true;
		mirrorIn();
	}
}

void InstascopeApp::mirrorOut()
{
	continueCycle();
}

void InstascopeApp::mirrorIn()
{
	// redefine the bg texture
	mBgTexture = mNewTex;
	mTextRibbon->update( TAG, mCurInstagram.getUser() );
}

void InstascopeApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::enableAlphaBlending( PREMULT );
	
	if( mBgTexture )
		gl::draw( mBgTexture, Rectf( mBgTexture->getBounds() ).getCenteredFit( getWindowBounds(), true ) );
	
	drawMirrors( &mTriPieces );
	mTextRibbon->draw();
}

void InstascopeApp::drawMirrors( vector<TrianglePiece> *vec ) 
{
	gl::ScopedModelMatrix scopedMat;
	gl::translate( getWindowCenter() );
	gl::rotate( mMirrorRot );
	for( int i = 0; i < vec->size(); i++ ) {
		(*vec)[i].draw();
	}
}

CINDER_APP( InstascopeApp, RendererGl( RendererGl::Options().msaa( 16 ) ) )
