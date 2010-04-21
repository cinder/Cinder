#include "flint/app/AppBasic.h"
#include "flint/Capture.h"
#include "flint/Matrix.h"
#include "HodginParticlesApp.h"
#include "QuickTimeApp.h"
#include "BoxSpinApp.h"
#include "CarModule.h"
#include "object.h"

#include <stdio.h>
#include <stdlib.h>					// malloc(), free()
#include <AR/config.h>
#include <AR/video.h>
#include <AR/param.h>			// arParamDisp()
#include <AR/ar.h>
#include <AR/gsub_lite.h>

using namespace fli;
using namespace fli::app;

#include <list>
using std::list;
using std::vector;

//#define USE_AR_VIDEO


class ARTestApp : public AppBasic {
 public:
	~ARTestApp();
	
	void prepareSettings( Settings *settings );
	void setup();
	void keyDown( KeyEvent event );
	void update();
	void draw();
	
	int		setupCamera( const char *cparam_name, char *vconf, ARParam *cparam );
	
	gl::Texture		*mTexture;
	std::vector<Module*>	mModules;
	ObjectData_T    *object;
	int             objectnum;
	float			mCurrentAlpha;
	int				mLockedMode;
#if ! defined( USE_AR_VIDEO )
	fli::Capture	*mCapture;
#endif
};


// ============================================================================
//	Constants
// ============================================================================

#define VIEW_SCALEFACTOR		2.5		// 1.0 ARToolKit unit becomes 0.025 of my OpenGL units.
#define VIEW_DISTANCE_MIN		2			// Objects closer to the camera than this will not be displayed.
#define VIEW_DISTANCE_MAX		4000.0		// Objects further away from the camera than this will not be displayed.

// ============================================================================
//	Global variables
// ============================================================================

// Preferences.

// Image acquisition.
static ARUint8		*gARTImage = NULL;

// Marker detection.
static int			gARTThreshhold = 80;
static long			gCallCountMarkerDetect = 0;

// Transformation matrix retrieval.
#if 0
static double		gPatt_width     = 80.0;	// Per-marker, but we are using only 1 marker.
static double		gPatt_centre[2] = {0.0, 0.0}; // Per-marker, but we are using only 1 marker.
static double		gPatt_trans[3][4];		// Per-marker, but we are using only 1 marker.
static int			gPatt_found = FALSE;	// Per-marker, but we are using only 1 marker.
static int			gPatt_id;				// Per-marker, but we are using only 1 marker.
#endif

// Drawing.
static ARParam		gARTCparam;
static ARGL_CONTEXT_SETTINGS_REF gArglSettings = NULL;

int ARTestApp::setupCamera( const char *cparam_name, char *vconf, ARParam *cparam )
{	
	ARParam			wparam;
	int				xsize, ysize;

#if defined( USE_AR_VIDEO )
	if( arVideoOpen( vconf ) < 0 ) {
		fprintf(stderr, "setupCamera(): Unable to open connection to camera.\n");
		return (FALSE);
	}

	if( arVideoInqSize(&xsize, &ysize) < 0 ) return (FALSE);
	fprintf(stdout, "Camera image size (x,y) = (%d,%d)\n", xsize, ysize);
#else
	xsize = 800;
	ysize = 600;
	mCapture = new fli::Capture( xsize, ysize, SurfaceChannelOrder::ARGB );
	mCapture->startCapture();
#endif

	if (arParamLoad( cparam_name, 1, &wparam ) < 0) {
		fprintf(stderr, "setupCamera(): Error loading parameter file %s for camera.\n", cparam_name);
		return (FALSE);
	}
	arParamChangeSize( &wparam, xsize, ysize, cparam );
	fprintf( stdout, "*** Camera Parameter ***\n" );
	arParamDisp( cparam );

	arInitCparam( cparam );

#if defined( USE_AR_VIDEO )
	if( arVideoCapStart() != 0 ) {
		fprintf(stderr, "setupCamera(): Unable to begin camera data capture.\n");
		return (FALSE);		
	}
#endif
	mTexture = new gl::Texture( xsize, ysize, GL_RGBA, false, false );

	return (TRUE);
}

static int setupMarker(const char *patt_name, int *patt_id)
{
	// Loading only 1 pattern in this example.
	if ((*patt_id = arLoadPatt(patt_name)) < 0) {
		fprintf(stderr, "setupMarker(): pattern load error !!\n");
		return (FALSE);
	}

	return (TRUE);
}

ARTestApp::~ARTestApp()
{
	arglCleanup(gArglSettings);
	arVideoCapStop();
	arVideoClose();
}

void ARTestApp::prepareSettings( Settings *settings )
{
	settings->setFrameRate( 24 );
}

void ARTestApp::setup()
{
	const char *cparam_name = "camera_para.dat";

#ifdef _WIN32
	char			*vconf = "Data\\WDM_camera_flipV.xml";
#else
	char			*vconf = "";
#endif
//	const char *patt_name  = "patt.kanji";
	const char *model_name = "object_data2";
	
	std::string modelPath = getAppPath() + std::string( "/Contents/Resources/" ) + std::string( model_name );
	std::cout << "Reading model at " << modelPath << std::endl;
    if( (object=read_ObjData( ( getAppPath() + std::string( "/Contents/Resources/" ) ).c_str(), modelPath.c_str(), &objectnum)) == NULL )
		exit(0);
    printf("Objectfile num = %d\n", objectnum);

	
	std::string camPath = getAppPath() + std::string( "/Contents/Resources/" ) + std::string( cparam_name );
	if( ! setupCamera( camPath.c_str(), vconf, &gARTCparam) ) {
		fprintf(stderr, "main(): Unable to set up AR camera.\n");
		exit(-1);
	}

	// Setup argl library for current context.
	if ((gArglSettings = arglSetupForCurrentContext()) == NULL) {
		fprintf(stderr, "main(): arglSetupForCurrentContext() returned error.\n");
		exit(-1);
	}

	arUtilTimerReset();

/*	std::string pattPath = getAppPath() + std::string( "/Contents/Resources/" ) + std::string( patt_name );		
	if( ! setupMarker( pattPath.c_str(), &gPatt_id) ) {
		fprintf(stderr, "main(): Unable to set up AR marker.\n");
		quit();
	}*/

	mCurrentAlpha = 1.0f;
	mLockedMode = -1;

	mModules.push_back( new QuickTimeApp() );
	mModules.push_back( new HodginParticlesApp() );	
	mModules.push_back( new CarModule() );		

	for( vector<Module*>::iterator modIt = mModules.begin(); modIt != mModules.end(); ++modIt )
		(*modIt)->setup( this );
}

void ARTestApp::keyDown( KeyEvent event )
{
	switch( event.getChar() ) {
		case 'f':
			setFullScreen( ! isFullScreen() );
		break;
		case 'l':
			if( mLockedMode >= 0 )
				mLockedMode = -1;
			else {
				for( int i = 0; i < objectnum; ++i )
					if( object[i].visible )
						mLockedMode = i;
			}
		break;
	}
}

void ARTestApp::update()
{

	ARMarkerInfo    *marker_info;					// Pointer to array holding the details of detected markers.
    int             marker_num;						// Count of number of markers detected.
    int             j, k;
	
	// Grab a video frame.
#if defined( USE_AR_VIDEO )
	ARUint8 *image;
	if ((image = arVideoGetImage()) != NULL) {
#else
	if( mCapture->checkNewFrame() ) {
#endif

#if defined( USE_AR_VIDEO )
		gARTImage = image;	// Save the fetched image.
		mTexture->enableAndBind();
#else
		const fli::Surface8u &surface( mCapture->getSurface() );
		mTexture->update( surface );
		gARTImage = const_cast<uint8_t*>( surface.getData() );
#endif

		gCallCountMarkerDetect++; // Increment ARToolKit FPS counter.
		
		// Detect the markers in the video frame.
		if (arDetectMarker(gARTImage, gARTThreshhold, &marker_info, &marker_num) < 0) {
			exit(-1);
		}
		
		// check for known patterns
		for( int i = 0; i < objectnum; i++ ) {
			k = -1;
			for( j = 0; j < marker_num; j++ ) {
				if( object[i].id == marker_info[j].id) {

					/* you've found a pattern */
					if( k == -1 ) k = j;
					else /* make sure you have the best pattern (highest confidence factor) */
						if( marker_info[k].cf < marker_info[j].cf ) k = j;
				}
			}
			if( k == -1 ) {
				object[i].visible = 0;
				continue;
			}
			/* calculate the transform for each marker */
			if( object[i].visible == 0 ) {
				arGetTransMat(&marker_info[k],
							  object[i].marker_center, object[i].marker_width,
							  object[i].trans);
			}
			else {
				arGetTransMatCont(&marker_info[k], object[i].trans,
							  object[i].marker_center, object[i].marker_width,
							  object[i].trans);
			}
			object[i].visible = 1;
		}
	}

	if( mLockedMode >= 0 ) {
		for( int i = 0; i < objectnum; i++ ) {
			object[i].visible = 0;
		}
		object[mLockedMode].visible = 1;
	}

	for( int mod = 0; mod < objectnum; ++mod )
		mModules[mod]->update( this, object[mod].visible );
}

void ARTestApp::draw()
{
    GLdouble p[16];
	GLdouble m[16];
	
	// Select correct buffer for this context.
	glClearColor( 0, 0, 0, 1 ); // Clear the buffers for new frame.
	gl::enableDepthWrite();
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ); // Clear the buffers for new frame.
	
	gl::disableDepthRead();
	gl::disableDepthWrite();
	gl::enableAlphaBlending();

	if( object[0].visible || object[1].visible || object[2].visible ) 
		mCurrentAlpha += ( 0.0f - mCurrentAlpha ) * 0.05f;
	else
		mCurrentAlpha += ( 1.0f - mCurrentAlpha ) * 0.05f;
	
	gl::setMatricesScreenOrtho( getWindowWidth(), getWindowHeight() );
	// draw the camera image centered
	glColor4f( 1, 1, 1, 1 );//0.2f + mCurrentAlpha * 0.8f );
	float width = ( getWindowHeight() * ( mTexture->getWidth() / (float)mTexture->getHeight() ) );
	mTexture->draw( ( getWindowWidth() - width ) / 2.0f, 0, width, getWindowHeight() );
	glDisable( mTexture->getTarget() );
	
#if defined( USE_AR_VIDEO )	
	arVideoCapNext();
	gARTImage = NULL; // Image data is no longer valid after calling arVideoCapNext().
#endif				

	// Projection transformation.
	arglCameraFrustumRH( &gARTCparam, VIEW_DISTANCE_MIN, VIEW_DISTANCE_MAX, p );
	glMatrixMode( GL_PROJECTION );
	glLoadMatrixd( p );
		
	// Calculate the camera position relative to the marker.
	// Replace VIEW_SCALEFACTOR with 1.0 to make one drawing unit equal to 1.0 ARToolKit units (usually millimeters).
	for( int mod = 0; mod < objectnum; ++mod ) {
		if( object[mod].visible ) {
			arglCameraViewRH( object[mod].trans, m, VIEW_SCALEFACTOR );
			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixd( m );
			fli::Matrix44d mvd( m );
			mModules[mod]->draw( this, mvd * Vec4d( 0, 0, 0, 1 ) );
		}
	}
}

// This line tells Flint to actually create the application
FLI_APP_BASIC( ARTestApp )
