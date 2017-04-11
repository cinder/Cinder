/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.


 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "cinder/Cinder.h"
#include "cinder/DataSource.h"
#include "cinder/DataTarget.h"
#include "cinder/Surface.h"
#include "cinder/Exception.h"

#include <vector>
#include <map>
#include <utility>

namespace cinder {

typedef std::shared_ptr<class ImageSource>		ImageSourceRef;
typedef std::shared_ptr<class ImageLoader>		ImageLoaderRef;
typedef std::shared_ptr<class ImageTarget>		ImageTargetRef;
typedef std::shared_ptr<class ImageTargetFile>	ImageTargetFileRef;

class CI_API ImageIo {
  public:
	typedef enum ColorModel { CM_RGB, CM_GRAY, CM_UNKNOWN } ColorModel;
	typedef enum DataType { UINT8, UINT16, FLOAT32, FLOAT16, DATA_UNKNOWN } DataType;
	typedef enum ChannelType { CHAN_RGB_R, CHAN_RGB_G, CHAN_RGB_B, CHAN_GRAY, CHAN_ALPHA, CHAN_MASK, CHAN_LAB_L, CHAN_LAB_A, CHAN_LAB_B,
					CHAN_YUV_Y, CHAN_YUV_U, CHAN_YUV_V, CHAN_CMYK_C, CHAN_CMYK_M, CHAN_CMYK_Y, CHAN_CMYK_K,
					CHAN_UNKNOWN } ChannelType;
	typedef enum ChannelOrder { RGBA, BGRA, ARGB, ABGR, RGBX, BGRX, XRGB, XBGR, RGB, BGR, Y, YA, CUSTOM } ChannelOrder; 	// Y = Gray/Luminance, X = ignored

	int32_t			getWidth() const { return mWidth; }
	int32_t			getHeight() const { return mHeight; }
	ColorModel		getColorModel() const { return mColorModel; }
	DataType		getDataType() const { return mDataType; }
	ChannelOrder	getChannelOrder() const { return mChannelOrder; }
	virtual bool	hasAlpha() const { return channelOrderHasAlpha( mChannelOrder ); }

	static void		translateRgbColorModelToOffsets( ChannelOrder channelOrder, int8_t *red, int8_t *green, int8_t *blue, int8_t *alpha, int8_t *inc );
	static void		translateGrayColorModelToOffsets( ChannelOrder channelOrder, int8_t *gray, int8_t *alpha, int8_t *inc );
	static bool		channelOrderHasAlpha( ChannelOrder channelOrder );
	static int8_t	channelOrderNumChannels( ChannelOrder channelOrder );
	static uint8_t	dataTypeBytes( DataType dataType );

	/** Returns a vector of the extensions ImageIo supports for loading. Suitable for the \a extensions parameters of getOpenFilePath() **/
	static std::vector<std::string>	getLoadExtensions();
	/** Returns a vector of the extensions ImageIo supports for writing. Suitable for the \a extensions parameters of getSaveFilePath() **/
	static std::vector<std::string>	getWriteExtensions();

  protected:
	ImageIo();

	void	setSize( int32_t width, int32_t height ) { mWidth = width; mHeight = height; }
	void	setColorModel( ColorModel colorModel ) { mColorModel  = colorModel; }
	void	setDataType( DataType aDataType ) { mDataType = aDataType; }	
	void	setChannelOrder( ChannelOrder aChannelOrder ) { mChannelOrder = aChannelOrder; }

	int32_t						mWidth, mHeight;
	ColorModel					mColorModel;
	DataType					mDataType;
	ChannelOrder				mChannelOrder;
};

class CI_API ImageSource : public ImageIo {
  public:
	ImageSource() : ImageIo(), mIsPremultiplied( false ), mPixelAspectRatio( 1 ), mCustomPixelInc( 0 ), mFrameCount( 1 ) {}
	virtual ~ImageSource() {}  

	//! Optional parameters passed when creating an Image. \see loadImage()
	class Options {
	  public:
		Options() : mIndex( 0 ), mThrowOnFirstException( false ) {}

		//! Specifies an image index for multi-part images, like animated GIFs. 0-based index.
		Options& index( int32_t index )						{ mIndex = index; return *this; }
		//! If an exception occurs, enabling this will prevent any attempts at using other handlers to load the image. Default = false, all handlers are tried and if none succeed, the last exception is rethrown. \see ImageIoException
		Options& throwOnFirstException( bool b = true )		{ mThrowOnFirstException = b; return *this; }

		//! Returns image index. \see index()
		int32_t				getIndex() const				{ return mIndex; }
		//! Returns whether throwOnFirstException() is enabled or not.
		bool				getThrowOnFirstException()		{ return mThrowOnFirstException; }
		
	  protected:
		int32_t			mIndex;
		bool			mThrowOnFirstException;
	};

	//! Returns the aspect ratio of individual pixels to accommodate non-square pixels
	float		getPixelAspectRatio() const;
	//! Returns whether the ImageSource's color data has been premultiplied by its alpha channel
	bool		isPremultiplied() const;
	//! Returns the number of bytes necessary to represent a row of the ImageSource
	size_t		getRowBytes() const;	
	//! Returns the number of images. Generally \c 1 but may not be in the case of animated GIFs. \see Options::index()
	int32_t		getCount() const { return mFrameCount; }

	virtual void	load( ImageTargetRef target ) = 0;

	typedef void (ImageSource::*RowFunc)(ImageTargetRef, int32_t, const void*);

  protected:
	void		setPixelAspectRatio( float pixelAspectRatio ) { mPixelAspectRatio = pixelAspectRatio; }
	void		setPremultiplied( bool premult = true ) { mIsPremultiplied = premult; }
	//! Allows declaration of a pixel increment different from what its ColorModel would imply. For example a non-planar Channel.
	void		setCustomPixelInc( int8_t customPixelInc ) { mCustomPixelInc = customPixelInc; }
	void		setFrameCount( int32_t frameCount ) { mFrameCount = frameCount; }

	RowFunc		setupRowFunc( ImageTargetRef target );
	void		setupRowFuncRgbSource( ImageTargetRef target );
	void		setupRowFuncGraySource( ImageTargetRef target );
	template<typename SD, typename TD, ColorModel TCS>
	RowFunc		setupRowFuncForTypesAndTargetColorModel( ImageTargetRef target );
	template<typename SD, typename TD>
	RowFunc		setupRowFuncForTypes( ImageTargetRef target );
	template<typename SD>
	RowFunc		setupRowFuncForSourceType( ImageTargetRef target );

	template<typename SD, typename TD, ImageIo::ColorModel TCM, bool ALPHA>
	void		rowFuncSourceRgb( ImageTargetRef target, int32_t row, const void *data );
	template<typename SD, typename TD, ColorModel TCM, bool ALPHA>
	void		rowFuncSourceGray( ImageTargetRef target, int32_t row, const void *data );

	float						mPixelAspectRatio;
	bool						mIsPremultiplied;
	int8_t						mCustomPixelInc;
	int32_t						mFrameCount;
	
	int8_t						mRowFuncSourceRed, mRowFuncSourceGreen, mRowFuncSourceBlue, mRowFuncSourceAlpha;
	int8_t						mRowFuncTargetRed, mRowFuncTargetGreen, mRowFuncTargetBlue, mRowFuncTargetAlpha;
	int8_t						mRowFuncSourceGray, mRowFuncTargetGray;
	int8_t						mRowFuncSourceInc, mRowFuncTargetInc;
};

class CI_API ImageTarget : public ImageIo {
  public:
	virtual ~ImageTarget() {};

	virtual void*	getRowPointer( int32_t row ) = 0;
	virtual void	setRow( int32_t /*row*/, const void * /*data*/ ) { throw; }
	virtual void	finalize() { }
	
	class Options {
	  public:
		Options() : mQuality( 0.9f ), mColorModelDefault( true ) {}
		
		Options& quality( float quality ) { mQuality = quality; return *this; }
		Options& colorModel( ImageIo::ColorModel cm ) { mColorModelDefault = false; mColorModel = cm; return *this; }
		
		void	setColorModelDefault() { mColorModelDefault = true; }
		
		float				getQuality() const { return mQuality; }
		bool				isColorModelDefault() const { return mColorModelDefault; }
		ImageIo::ColorModel	getColorModel() const { return mColorModel; }
		
	  protected:
		float					mQuality;
		bool					mColorModelDefault;
		ImageIo::ColorModel		mColorModel;
	};
	
  protected:
	ImageTarget() {}	
};

#if defined( CINDER_UWP )
//! Asynchronously loads an image from the file path \a path. Callback function \a callback will be called on main UI thread. Optional \a extension parameter allows specification of a file type. For example, "jpg" would force the file to load as a JPEG

void loadImageAsync(const fs::path path, std::function<void (ImageSourceRef)> callback, ImageSource::Options options = ImageSource::Options(), std::string extension = "" );
#endif


//! Loads an image from the file path \a path. Optional \a extension parameter allows specification of a file type. For example, "jpg" would force the file to load as a JPEG
CI_API ImageSourceRef	loadImage( const fs::path &path, ImageSource::Options options = ImageSource::Options(), std::string extension = "" );
//! Loads an image from \a dataSource. Optional \a extension parameter allows specification of a file type. For example, "jpg" would force the file to load as a JPEG
CI_API ImageSourceRef	loadImage( DataSourceRef dataSource, ImageSource::Options options = ImageSource::Options(), std::string extension = "" );
/** \brief Writes \a imageSource to \a dataTarget. Optional \a extension parameter allows specification of a file type. For example, "jpg" would force the file to load as a JPEG **/
CI_API void				writeImage( DataTargetRef dataTarget, const ImageSourceRef &imageSource, ImageTarget::Options options = ImageTarget::Options(), std::string extension = "" );
/** Writes \a imageSource to file path \a path. Optional \a extension parameter allows specification of a file type. For example, "jpg" would force the file to load as a JPEG
 * \note If \a path does not exist, the necessary directories are created automatically. **/
CI_API void				writeImage( const fs::path &path, const ImageSourceRef &imageSource, ImageTarget::Options options = ImageTarget::Options(), std::string extension = "" );
/** \brief Writes \a imageSource to \a imageTarget. **/
CI_API void				writeImage( ImageTargetRef imageTarget, const ImageSourceRef &imageSource );

class CI_API ImageIoException : public Exception {
  public:
	ImageIoException( const std::string &description = "" ) : Exception( description ) {}
};

class CI_API ImageIoExceptionFailedLoad : public ImageIoException {
  public:
	ImageIoExceptionFailedLoad( const std::string &description = "" ) : ImageIoException( description ) {}
};

class CI_API ImageIoExceptionFailedWrite : public ImageIoException {
  public:
	ImageIoExceptionFailedWrite( const std::string &description = "" ) : ImageIoException( description ) {}
};

class CI_API ImageIoExceptionUnknownExtension : public ImageIoException {
  public:
	ImageIoExceptionUnknownExtension( const std::string &description = "" ) : ImageIoException( description ) {}
};

class CI_API ImageIoExceptionIllegalColorModel : public ImageIoException {
  public:
	ImageIoExceptionIllegalColorModel( const std::string &description = "" ) : ImageIoException( description ) {}
};

class CI_API ImageIoExceptionIllegalDataType : public ImageIoException {
  public:
	ImageIoExceptionIllegalDataType( const std::string &description = "" ) : ImageIoException( description ) {}
};

class CI_API ImageIoExceptionIllegalChannelOrder : public ImageIoException {
  public:
	ImageIoExceptionIllegalChannelOrder( const std::string &description = "" ) : ImageIoException( description ) {}
};


struct CI_API ImageIoRegistrar {
	typedef ImageSourceRef (*SourceCreationFunc)( DataSourceRef, ImageSource::Options options );
	typedef ImageTargetRef (*TargetCreationFunc)( DataTargetRef, ImageSourceRef, ImageTarget::Options options, const std::string& );

	static ImageSourceRef	createSource( DataSourceRef dataSource, ImageSource::Options options, std::string extension );
	static ImageTargetRef	createTarget( DataTargetRef dataTarget, ImageSourceRef imageSource, ImageTarget::Options options, std::string extension );
	
	static void		registerSourceType( std::string extension, SourceCreationFunc func, int32_t priority = 2 );
	static void		registerSourceGeneric( SourceCreationFunc func, int32_t priority = 2 );
	
	static void		registerTargetType( std::string extension, TargetCreationFunc func, int32_t priority, const std::string &extensionData );
	
  private:
	
	struct CI_API Inst {
		void	registerSourceType( std::string extension, SourceCreationFunc func, int32_t priority );
		void	registerSourceGeneric( SourceCreationFunc func, int32_t priority );
		void	registerTargetType( std::string extension, TargetCreationFunc func, int32_t priority, const std::string &extensionData );		

		ImageSourceRef	createSource( DataSourceRef dataSource, ImageSource::Options options, std::string extension );
		ImageTargetRef	createTarget( DataTargetRef dataTarget, ImageSourceRef imageSource, ImageTarget::Options options, std::string extension );
	
		std::map<std::string, std::multimap<int32_t,SourceCreationFunc> >	mSources;
		std::map<int32_t, SourceCreationFunc>								mGenericSources;
		std::map<std::string, std::multimap<int32_t,std::pair<TargetCreationFunc,std::string> > >	mTargets;
	};

	static ImageIoRegistrar::Inst*	instance();		
		
	friend class ImageIo;
};

} // namespace cinder
