#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/ip/Fill.h"
#include "cinder/ip/Resize.h"
#include "cinder/Camera.h"
#include "cinder/Surface.h"
#include "cinder/ImageIo.h"

#include "Resources.h"

#include <vector>
#include <memory>

using namespace ci;
using namespace ci::app;
using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// This struct just acts as storage for the filter states and textures.
struct FilterControl {
  public:
	FilterControl( const Rectf &minRect, const Rectf &textureRect, const Rectf &anisoRect, const Rectf &anisoLevelRect,
				  const ivec2 &scissorPos, const ivec2 &scissorDimension, const gl::TextureRef minTexture, const gl::TextureRef textureTexture, float maxAniso )
	:	mMinFilterPushed( false ), mMinFilterChoice( 0 ), mMinRect( minRect ),
		mAnisoFilterPushed( false ), mAnisoFilterMax( maxAniso ), mAnisoRect( anisoRect ),
		mTexturePushed( false ), mTextureChoice( 0 ), mTextureRect( textureRect ),
		mScissorPos( scissorPos ), mScissorDimension( scissorDimension ), mAnisoLevelRect( anisoLevelRect ),
		mMinTexture( minTexture ), mTextureTexture( textureTexture )
	{
	}
	
	void setLevelRect( int x ) { mAnisoLevelRect.x2 = x; };
	
	void setMinTexture( const gl::TextureRef minTexture ) { mMinTexture = minTexture; }
	void setTextureTexture( const gl::TextureRef textureTexture ) { mTextureTexture = textureTexture; }
	
	// Plane Textures
	gl::TextureRef	mUserCreatedMipmap;
	gl::TextureRef	mGlGeneratedMipmap;
	gl::TextureRef	mUserResizedMipmap;
	
	// Button variables
	bool			mMinFilterPushed;
	int				mMinFilterChoice;
	Rectf			mMinRect;
	gl::TextureRef	mMinTexture;
	
	bool			mAnisoFilterPushed;
	float			mAnisoFilterMax;
	Rectf			mAnisoRect;
	Rectf			mAnisoLevelRect;
	
	bool			mTexturePushed;
	int				mTextureChoice;
	Rectf			mTextureRect;
	gl::TextureRef	mTextureTexture;
	
	ivec2			mScissorPos;
	ivec2			mScissorDimension;
};

//////////////////////////////////////////////////////////////////////////////////////////////

typedef std::shared_ptr<FilterControl> FilterControlRef;

//////////////////////////////////////////////////////////////////////////////////////////////

class TextureMipmappingApp : public App {
  public:
	void	setup();
	void	update();
	void	draw();
	
	void	renderPlaneTexture( FilterControlRef f );
	void	renderFilterButtons( FilterControlRef f );
	void	bindMinAndAnisoChange( const gl::TextureRef texture, FilterControlRef f );
	
	void	mouseDrag( MouseEvent event );
	void	mouseUp( MouseEvent event );
	void	upContains( FilterControlRef f, const ivec2 &pos );
	bool	buttonContains( const Rectf &rect, const ivec2 &touch );
	
	void	createAnisoLevelTex();
	void	createGlGenMip( const gl::Texture::Format &format, FilterControlRef f );
	void	createUserGenMip( const gl::Texture::Format &format, FilterControlRef f );
	void	createUserResizedGenMip( const gl::Texture::Format &format, FilterControlRef filter );
	
	Surface					mCheckerBoard;
	
	CameraPersp				mCam;
	mat4					mPlaneRotation;
	mat4					mPlaneTranslation;
	
	gl::TextureRef			mAnisoTexture;
	gl::TextureRef			mAnisoLevelTexture;
	
	float					mMaxAnisoFilterAmount;
	
	FilterControlRef		mLeftControl;
	FilterControlRef		mRightControl;
	
	bool					right;
	float					pan;
};

//////////////////////////////////////////////////////////////////////////////////////////////////

void TextureMipmappingApp::setup()
{
	gl::bindStockShader( gl::ShaderDef().texture().color() );
	
	// This if def is here to make sure whether you're using gl::Es2 or desktop gl.
	// It illustrates the fact that unfortunately Es2 does not support non-power-of-two
	// textures.
#if ! defined( CINDER_GL_ES )
	mCheckerBoard = Surface( loadImage( loadResource( NON_POT_CHECKER ) ) );
#else
	mCheckerBoard = Surface( loadImage( loadResource( CHECKER_BOARD ) ) );
#endif
	
	pan = 0.0f;
	right = false;
	
	mCam.setPerspective( 60, getWindowAspectRatio(), 1, 10000 );
	mCam.lookAt( vec3( 0, 0, -1 ), vec3( 0, 0, 1 ) );
	
	mPlaneTranslation = translate( vec3( - 10000 / 2, - getWindowHeight() / 2, 0 ) );
	mPlaneRotation = rotate( toRadians( 85.0f ), vec3( 1, 0, 0 ) );
	
	int widthFraction = getWindowWidth() / 6;
	int heightFraction = getWindowHeight() / 10;
	
	// getting max Anisotropic maximum sampling available on the graphics card above 1
	mMaxAnisoFilterAmount = gl::Texture::getMaxAnisotropyMax() - 1.0f;
	
	mLeftControl = shared_ptr<FilterControl>( new FilterControl( Rectf( widthFraction - 50, heightFraction * 1, widthFraction + 50, heightFraction * 1 + 30 ),
													Rectf( widthFraction - 50, heightFraction * 2, widthFraction + 50, heightFraction * 2 + 30 ),
													Rectf( widthFraction - 50, heightFraction * 3, widthFraction + 50, heightFraction * 3 + 30 ),
													Rectf( widthFraction - 50, heightFraction * 3, widthFraction + 50, heightFraction * 3 + 30 ),
								  /* scissor */		ivec2( 0, 0 ) /* position */, ivec2( getWindowWidth() / 2, getWindowHeight() ) /* dimension */,
													gl::Texture::create( loadImage( loadResource( MIN_FILTER_LIN_LIN ) ) ),
													gl::Texture::create( loadImage( loadResource( GL_GEN ) ) ),
													mMaxAnisoFilterAmount ) );
	
	mRightControl = shared_ptr<FilterControl>( new FilterControl( Rectf( widthFraction * 5 - 50, heightFraction * 1, widthFraction * 5 + 50, heightFraction * 1 + 30 ),
													 Rectf( widthFraction * 5 - 50, heightFraction * 2, widthFraction * 5 + 50, heightFraction * 2 + 30 ),
													 Rectf( widthFraction * 5 - 50, heightFraction * 3, widthFraction * 5 + 50, heightFraction * 3 + 30 ),
													 Rectf( widthFraction * 5 - 50, heightFraction * 3, widthFraction * 5 + 50, heightFraction * 3 + 30 ),
								  /* scissor */		 ivec2( getWindowWidth() / 2, 0 ) /* position */, ivec2( getWindowWidth() / 2, getWindowHeight() ) /* dimension */,
													 gl::Texture::create( loadImage( loadResource( MIN_FILTER_NEA_NEA ) ) ),
													 gl::Texture::create( loadImage( loadResource( GL_GEN ) ) ),
													 mMaxAnisoFilterAmount ) );
	
	
	
	
	// Creating the 3 Texture formats
	gl::Texture::Format leftFormat = gl::Texture::Format()
		.magFilter( GL_LINEAR )
		.minFilter( GL_LINEAR_MIPMAP_LINEAR )
		.maxAnisotropy( mMaxAnisoFilterAmount )
		.wrapT( GL_REPEAT ).wrapS( GL_REPEAT )
		.target( GL_TEXTURE_2D )
		.mipmap();
	
	gl::Texture::Format rightFormat = leftFormat;
	rightFormat.setMinFilter( GL_NEAREST_MIPMAP_NEAREST );
	
	mAnisoTexture = gl::Texture::create( loadImage( loadResource( ANISOTROPIC ) ) );
	createAnisoLevelTex();
	
	// This function creates a texture refs and allows gl to Generate the mipmaps
	// as you can see below this call, we reset the value of mipmap to be false
	createGlGenMip( leftFormat, mLeftControl );
	createGlGenMip( rightFormat, mRightControl );
	
	// Turning off auto mipmap generation for the next two user generated mipmaps
	leftFormat.enableMipmapping( true );
	rightFormat.enableMipmapping( true );	
	
	// This function creates a texture ref based upon a source that can be dynamically
	// created and it uses cinder tools like ip::fill and Surface to place data in the
	// texture. This is more for demonstration of what mipmapping is than anything else.
	createUserGenMip( leftFormat, mLeftControl );
	createUserGenMip( rightFormat, mRightControl );
	
	// This function creates a texture ref from an existing Surface, namely the one
	// we gave above for the glGeneratedMipmap, and uses ip::resize and user defined
	// filter to create the different mipmap levels
	createUserResizedGenMip( leftFormat, mLeftControl );
	createUserResizedGenMip( rightFormat, mRightControl );
}

void TextureMipmappingApp::update()
{	
	if( pan > 2 ) {
		right = false;
	}
	else if( pan < -2 ) {
		right = true;
	}
	
	if( right ) {
		pan += 0.01f;
	}
	else {
		pan -= 0.01f;
	}
	
	mCam.lookAt( vec3( 0, 0, -1 ), vec3( pan, 0, 1 ) );	
}

void TextureMipmappingApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
	gl::enableAlphaBlending();
	
	gl::pushModelMatrix();
		gl::setMatrices( mCam );
		gl::multModelMatrix( mPlaneTranslation );
		gl::multModelMatrix( mPlaneRotation );
	
		renderPlaneTexture( mLeftControl );
		renderPlaneTexture( mRightControl );
	
	gl::popModelMatrix();
	
	gl::pushMatrices();
		gl::setMatricesWindow( getWindowSize() );
	
		renderFilterButtons( mLeftControl );
		renderFilterButtons( mRightControl );
	
		// Simple seperator
		gl::color(1, 1, 1);
		gl::drawSolidRect( Rectf( getWindowWidth() / 2 - 2, 0, getWindowWidth() / 2 + 2, getWindowHeight() ) );
	gl::popMatrices();
}

void TextureMipmappingApp::renderPlaneTexture( FilterControlRef f )
{
	// This creates the scissor effect of showing both sides for contrast.
	// it takes an area with origin at lower left and width and height
	// like glViewport
	gl::ScopedScissor myScissor( f->mScissorPos, f->mScissorDimension  );
	
	switch( f->mTextureChoice ) {
		case 0:
			bindMinAndAnisoChange( f->mGlGeneratedMipmap, f );
		break;
		case 1:
			bindMinAndAnisoChange( f->mUserCreatedMipmap, f );
		break;
		case 2:
			bindMinAndAnisoChange( f->mUserResizedMipmap, f );
		break;
		default:
		break;
	}
	// Drawing this enormous so that we can see each mip level. 50x50 for texcoords leads to a repeating/tiled texture
	gl::drawSolidRect( Rectf( 0, 0, 10000, 10000 ), vec2( 0, 0 ), vec2( 50, 50 ) );
}

void TextureMipmappingApp::renderFilterButtons( FilterControlRef f )
{
	if( f->mMinFilterPushed ) {
		switch( f->mMinFilterChoice ) {
			case 0:
				f->mMinTexture = gl::Texture::create( loadImage( loadResource( MIN_FILTER_LIN_LIN ) ) );
			break;
			case 1:
				f->mMinTexture = gl::Texture::create( loadImage( loadResource( MIN_FILTER_LIN_NEA ) ) );
			break;
			case 2:
				f->mMinTexture = gl::Texture::create( loadImage( loadResource( MIN_FILTER_NEA_LIN ) ) );
			break;
			case 3:
				f->mMinTexture = gl::Texture::create( loadImage( loadResource( MIN_FILTER_NEA_NEA ) ) );
			break;
			default:
			break;
		}
		f->mMinFilterPushed = false;
	}
	if( f->mTexturePushed ) {
		switch( f->mTextureChoice ) {
			case 0:
				f->mTextureTexture = gl::Texture::create( loadImage( loadResource( GL_GEN ) ) );
			break;
			case 1:
				f->mTextureTexture = gl::Texture::create( loadImage( loadResource( USER_GEN ) ) );
			break;
			case 2:
				f->mTextureTexture = gl::Texture::create( loadImage( loadResource( USER_RESIZE ) ) );
			break;
			default:
			break;
		}
		f->mTexturePushed = false;
	}
	
	f->mMinTexture->bind();
		gl::drawSolidRect( f->mMinRect );
	f->mMinTexture->unbind();
	
	f->mTextureTexture->bind();
		gl::drawSolidRect( f->mTextureRect );
	f->mTextureTexture->unbind();
	
	mAnisoTexture->bind();
		gl::drawSolidRect( f->mAnisoRect );
	mAnisoTexture->unbind();
	
	mAnisoLevelTexture->bind();
		gl::drawSolidRect( f->mAnisoLevelRect );
	mAnisoLevelTexture->unbind();
	
}

void TextureMipmappingApp::bindMinAndAnisoChange( const gl::TextureRef texture, FilterControlRef f )
{
	texture->bind();
	
	if( f->mMinFilterPushed ) {
		switch( f->mMinFilterChoice ) {
			case 0:
				texture->setMinFilter( GL_LINEAR_MIPMAP_LINEAR );
			break;
			case 1:
				texture->setMinFilter( GL_LINEAR_MIPMAP_NEAREST );
			break;
			case 2:
				texture->setMinFilter( GL_NEAREST_MIPMAP_LINEAR );
			break;
			case 3:
				texture->setMinFilter( GL_NEAREST_MIPMAP_NEAREST );
			break;
			default:
			break;
		}
	}
	if( f->mAnisoFilterPushed ) {
		f->setLevelRect( f->mAnisoLevelRect.x1 + ( 100 * ( f->mAnisoFilterMax / mMaxAnisoFilterAmount ) ) );
		texture->setMaxAnisotropy( f->mAnisoFilterMax + 1.0f );
		f->mAnisoFilterPushed = false;
	}
}

void TextureMipmappingApp::mouseDrag( MouseEvent event )
{
	if( buttonContains( mLeftControl->mAnisoRect, event.getPos() ) ) {
		mLeftControl->mAnisoFilterMax = ( static_cast<float>( event.getPos().x - mLeftControl->mAnisoRect.getUpperLeft().x ) / 100 ) * mMaxAnisoFilterAmount;
		mLeftControl->mAnisoFilterPushed = true;
	}
	else if( buttonContains( mRightControl->mAnisoRect, event.getPos() ) ) {
		mRightControl->mAnisoFilterMax = ( static_cast<float>( event.getPos().x - mRightControl->mAnisoRect.getUpperLeft().x ) / 100 ) * mMaxAnisoFilterAmount;
		mRightControl->mAnisoFilterPushed = true;
	}
}

void TextureMipmappingApp::mouseUp( MouseEvent event )
{
	// CREATING A DIVISION IN THE SCREEN TO BIND DIFFERENT TEXTURES
	upContains( mLeftControl, event.getPos() );
	upContains( mRightControl, event.getPos() );
}

void TextureMipmappingApp::upContains( FilterControlRef f, const ivec2 &pos )
{
	if( buttonContains( f->mMinRect, pos ) ) {
		f->mMinFilterPushed = true;
		f->mMinFilterChoice++;
		if( f->mMinFilterChoice > 3 )
			f->mMinFilterChoice = 0;
	}
	else if( buttonContains( f->mAnisoRect, pos ) ) {
		f->mAnisoFilterMax = ( static_cast<float>( pos.x - f->mAnisoRect.getUpperLeft().x ) / 100 ) * mMaxAnisoFilterAmount;
		f->mAnisoFilterPushed = true;
	}
	else if( buttonContains( f->mTextureRect, pos ) ) {
		f->mMinFilterPushed = true;
		f->mTexturePushed = true;
		f->mAnisoFilterPushed = true;
		f->mTextureChoice++;
		if( f->mTextureChoice > 2)
			f->mTextureChoice = 0;
	}
}

bool TextureMipmappingApp::buttonContains( const Rectf &rect, const ivec2 &touch )
{
	return touch.x > rect.getUpperLeft().x && touch.x < rect.getLowerRight().x	&& touch.y > rect.getUpperLeft().y && touch.y < rect.getLowerRight().y;
}

void TextureMipmappingApp::createAnisoLevelTex()
{
	Surface* mSurface = new Surface( 100, 30, true );
	ip::fill( mSurface, ColorA( 0.4f, 0.4f, 1.0f, 0.5f ) );
	mAnisoLevelTexture = gl::Texture::create( *mSurface );
	
	delete mSurface;
}

void TextureMipmappingApp::createGlGenMip( const gl::Texture::Format &format, FilterControlRef f )
{
	// GL CREATED MIPMAP
	// format holds the enable mipmapping flag
	f->mGlGeneratedMipmap = gl::Texture::create( mCheckerBoard, format );
}

void TextureMipmappingApp::createUserGenMip( const gl::Texture::Format &format, FilterControlRef f )
{
	// USER GENERATED MIPMAPS WITH DIFFERENT COLORS TO SHOW THE MIPMAP GENERATION
	Surface surface( 512, 512, false );
	ip::fill( &surface, Color( CM_HSV, 1, 1, 1 ) );
	f->mUserCreatedMipmap = gl::Texture::create( surface, format );
	int mipLevels = gl::TextureBase::requiredMipLevels( surface.getWidth(), surface.getHeight(), 0 );
	
	for( int level = 1; level < mipLevels; ++level ) {
		float hue = static_cast<float>( level ) / static_cast<float>( mipLevels );
		ivec2 mipSize = gl::Texture2d::calcMipLevelSize( level, surface.getWidth(), surface.getHeight() );
		Surface mipSurface( mipSize.x, mipSize.y, false );
		ip::fill( &mipSurface, Color( CM_HSV, hue, 1, 1 ) );
		f->mUserCreatedMipmap->update( mipSurface, level );
	}
}

void TextureMipmappingApp::createUserResizedGenMip( const gl::Texture::Format &format, FilterControlRef f )
{
	// USER RESIZED CHECKERBOARD MIPMAP
	f->mUserResizedMipmap = gl::Texture::create( mCheckerBoard, format );
	int mipLevels = gl::TextureBase::requiredMipLevels( f->mUserResizedMipmap->getWidth(), f->mUserResizedMipmap->getHeight(), 0 );
	
	for( int level = 1; level < mipLevels; level++ ) {
		ivec2 mipSize = gl::Texture::calcMipLevelSize( level, mCheckerBoard.getWidth(), mCheckerBoard.getHeight() );
		f->mUserResizedMipmap->update( ip::resizeCopy( mCheckerBoard, Area( ivec2( 0, 0 ), mCheckerBoard.getSize() ), mipSize, FilterSincBlackman() ), level );
	}
}

CINDER_APP( TextureMipmappingApp, RendererGl( RendererGl::Options().msaa( 16 ) ), []( App::Settings *settings ) {
	settings->setMultiTouchEnabled( false );
} )
