#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/ConstantConversions.h"
#include "cinder/Log.h"
#include "cinder/ImageIo.h"
#include "cinder/gl/Texture.h"
#include "cinder/CinderImGui.h"
#include "cinder/CanvasUi.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ImageFileBasicApp : public App {
  public:
	void setup() override;
	void fileDrop( FileDropEvent event ) override;
	void draw() override;

	void openImage( const fs::path &path );
	void openImageDialog();
	void saveImageDialog();
	void drawGui();

	gl::TextureRef		mTexture;
	ImageSourceRef		mImageSource;
	fs::path			mImagePath;
	std::string			mErrorMessage;
	CanvasUi			mCanvas;
};

void ImageFileBasicApp::setup()
{
	ImGui::Initialize();

	mCanvas.connect( getWindow() );
}

void ImageFileBasicApp::openImage( const fs::path &path )
{
	try {
		mImagePath = path;
		mImageSource = loadImage( path );
		mTexture = gl::Texture::create( mImageSource );
		mCanvas.setContentBounds( Rectf( mTexture->getBounds() ) );
		mCanvas.fitAll();
		mErrorMessage.clear();
	}
	catch( Exception &exc ) {
		CI_LOG_EXCEPTION( "failed to load image.", exc );
		mTexture.reset();
		mImageSource.reset();
		mImagePath.clear();
		mErrorMessage = exc.what();
	}
}

void ImageFileBasicApp::openImageDialog()
{
	fs::path path = getOpenFilePath( "", ImageIo::getLoadExtensions() );
	if( ! path.empty() )
		openImage( path );
}

void ImageFileBasicApp::saveImageDialog()
{
	try {
		fs::path path = getSaveFilePath( "", ImageIo::getWriteExtensions() );
		if( ! path.empty() )
			writeImage( writeFile( path ), mTexture->createSource() );
	}
	catch( Exception &exc ) {
		CI_LOG_EXCEPTION( "failed to save image.", exc );
	}
}

void ImageFileBasicApp::fileDrop( FileDropEvent event )
{
	openImage( event.getFile( 0 ) );
}

void ImageFileBasicApp::draw()
{
	gl::clear( Color( 0.5f, 0.5f, 0.5f ) );

	// Draw image with CanvasUi transformation
	if( mTexture ) {
		gl::ScopedModelMatrix scopedMatrix( mCanvas.getModelMatrix() );
		gl::enableAlphaBlending();

		gl::draw( mTexture, mTexture->getBounds() );
	}

	drawGui();
}

void ImageFileBasicApp::drawGui()
{
	ImGui::Begin( "Image Info" );

	if( ImGui::Button( "Open Image" ) )
		openImageDialog();

	ImGui::SameLine();

	ImGui::BeginDisabled( ! mTexture );
	if( ImGui::Button( "Save Image" ) )
		saveImageDialog();
	ImGui::EndDisabled();

	ImGui::SameLine();

	ImGui::BeginDisabled( ! mTexture );
	if( ImGui::Button( "Fit to Window" ) ) {
		mCanvas.setContentBounds( Rectf( mTexture->getBounds() ) );
		mCanvas.fitAll();
	}
	ImGui::EndDisabled();

	ImGui::Separator();

	if( mTexture && mImageSource ) {
		ImGui::Text( "File: %s", mImagePath.filename().string().c_str() );
		ImGui::Separator();
		ImGui::Text( "Dimensions: %d x %d", mTexture->getWidth(), mTexture->getHeight() );
		ImGui::Text( "Texture Format: %s", gl::constantToString( mTexture->getInternalFormat() ).c_str() );
		ImGui::Separator();

		// Image source information
		const char* colorModelStr = "Unknown";
		switch( mImageSource->getColorModel() ) {
			case ImageIo::CM_RGB: colorModelStr = "RGB"; break;
			case ImageIo::CM_GRAY: colorModelStr = "Grayscale"; break;
			default: break;
		}
		ImGui::Text( "Color Model: %s", colorModelStr );

		const char* dataTypeStr = "Unknown";
		switch( mImageSource->getDataType() ) {
			case ImageIo::UINT8: dataTypeStr = "UINT8"; break;
			case ImageIo::UINT16: dataTypeStr = "UINT16"; break;
			case ImageIo::FLOAT16: dataTypeStr = "FLOAT16"; break;
			case ImageIo::FLOAT32: dataTypeStr = "FLOAT32"; break;
			default: break;
		}
		ImGui::Text( "Data Type: %s", dataTypeStr );

		const char* channelOrderStr = "Unknown";
		switch( mImageSource->getChannelOrder() ) {
			case ImageIo::Y: channelOrderStr = "Y (Grayscale)"; break;
			case ImageIo::YA: channelOrderStr = "YA (Grayscale + Alpha)"; break;
			case ImageIo::RGB: channelOrderStr = "RGB"; break;
			case ImageIo::RGBA: channelOrderStr = "RGBA"; break;
			case ImageIo::RGBX: channelOrderStr = "RGBX"; break;
			case ImageIo::BGR: channelOrderStr = "BGR"; break;
			case ImageIo::BGRA: channelOrderStr = "BGRA"; break;
			case ImageIo::BGRX: channelOrderStr = "BGRX"; break;
			default: break;
		}
		ImGui::Text( "Channel Order: %s", channelOrderStr );

		ImGui::Text( "Has Alpha: %s", mImageSource->hasAlpha() ? "Yes" : "No" );
		ImGui::Text( "Premultiplied: %s", mImageSource->isPremultiplied() ? "Yes" : "No" );
	}
	else {
		ImGui::Text( "No image loaded" );
		ImGui::Text( "Click 'Open Image' or drag & drop an image file" );

		if( ! mErrorMessage.empty() ) {
			ImGui::Separator();
			ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.0f, 0.3f, 0.3f, 1.0f ) );
			ImGui::TextWrapped( "Error: %s", mErrorMessage.c_str() );
			ImGui::PopStyleColor();
		}
	}

	ImGui::End();
}

CINDER_APP( ImageFileBasicApp, RendererGl )
