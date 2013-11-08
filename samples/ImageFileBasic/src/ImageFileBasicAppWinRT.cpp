#include "cinder/app/AppBasic.h"
#include "cinder/Utilities.h"
#include "cinder/ImageIo.h"
#include "cinder/Surface.h"
#include "cinder/dx/DxTexture.h"
#include "cinder/dx/dx.h"


// Uncomment this line to enable specialized PNG handling
//#include "cinder/ImageSourcePng.h"

using namespace ci;
using namespace ci::app;
using namespace std;





class ImageFileBasicApp : public AppBasic {
  public:
	void setup();
	void keyDown( KeyEvent event );
	void draw();
	dx::Texture		mTexture;	


};


void ImageFileBasicApp::setup()
{
	try {
		/* On WinRT it is required that use use the async version of getOpenFilePath() 
		   since Windows 8 Store Apps only have an async version of the Open File dialog.
		   You will need to provide a callback function or lamba that will receive the fs::path to
		   the selected file.
		*/
		std::vector<std::string> extensions;
		extensions.push_back(".png");
		extensions.push_back(".jpg");


		getOpenFilePath( "", extensions, [this](fs::path path){
			if( ! path.empty() ) {
				/*	Windows 8 Store Apps file access is highly sandboxed. In order to open 
					a file outside of your Application's directory (such as the Pictures Directory), 
					you will need to use the loadImageAsync()  method. If necessary, it will copy
					the selected image into the Apps temp directory, load the image into the texture, 
					and then delete the temporary copy of the image.
				*/

				loadImageAsync(path, [this](ImageSourceRef imageRef){
					this->mTexture = dx::Texture( imageRef );
				});


				/* You can also load a texture asychronously like this
				//dx::Texture::loadImageAsync(path, this->mTexture);

				/*	Note: if you are loading images from your Assets directory, then it is okay
					to use: dx::Texture( loadImage( path ) );
					this->mTexture = dx::Texture( loadImage( path ) );
				*/

				/*	FYI: This is how you would load an image outside of the Application folder into a Surface 
					Surface::loadImageAsync(path, this->mSurface);
				*/
			}
		});
	}
	catch( ... ) {
		console() << "unable to load the texture file!" << std::endl;
	}
}

void ImageFileBasicApp::keyDown( KeyEvent event )
{
	int c = event.getChar();

	if( event.getChar() == 'f' ) {
		setFullScreen( ! isFullScreen() );
	}
	else if( event.getCode() == app::KeyEvent::KEY_ESCAPE ) {
		setFullScreen( false );
	}
	else if( event.getChar() == 'O' ) {
		setup();
	}
}

void ImageFileBasicApp::draw()
{
	dx::clear( Color( 0.5f, 0.5f, 0.5f ) );
	dx::enableAlphaBlending();
	
	/*	Note: Since textures may be loaded asynchronously in WinRT, it is very important to test if 
		your texture is not empty before trying to use it!
	*/
	if( mTexture )
		dx::draw( mTexture, Vec2f( 0, 0 ) );
}



CINDER_APP_BASIC( ImageFileBasicApp, RendererDx )
