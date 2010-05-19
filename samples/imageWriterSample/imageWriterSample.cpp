#include "cinder/app/AppBasic.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"

#include "Resources.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ImageWriterSampleApp : public AppBasic {
 public:	
	void setup()
	{
		try {
			Surface srf( loadImage( loadResource( RES_IMAGE ) ) );
			
			string pngPath = getSaveFilePath( getHomeDirectory() );
			if( ! pngPath.empty() ) {
				writeImage( pngPath, srf );
			}
		}
		catch( ... ) {
			console() << "unable to save the image file!" << std::endl;
		}		
	}
};


CINDER_APP_BASIC( ImageWriterSampleApp, RendererGl )
