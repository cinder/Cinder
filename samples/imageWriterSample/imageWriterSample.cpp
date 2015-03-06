#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ImageWriterSampleApp : public App {
 public:	
	void setup()
	{
		try {
			// load a source image into a Surface to be written later
			Surface srf( loadImage( loadResource( RES_IMAGE ) ), SurfaceConstraintsDefault(), false );
			
			fs::path pngPath = getSaveFilePath( getHomeDirectory() );
			if( ! pngPath.empty() ) {
				writeImage( pngPath, srf, ImageTarget::Options().colorModel( ImageIo::CM_GRAY ).quality( 0.5f ) );
			}
		}
		catch( ci::Exception &exc ) {
			console() << "unable to save the image file, what: " << exc.what() << std::endl;
		}		
	}
};


CINDER_APP( ImageWriterSampleApp, RendererGl )
