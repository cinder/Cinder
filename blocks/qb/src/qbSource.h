//
//  qbSource.h
//
//  Created by Roger Sodre on 01/07/2011
//  Copyright 2011 Studio Avante. All rights reserved.
//
#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/App.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/qtime/QuickTime.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"

#include "cinderSyphon.h"

#include "qbCube.h"

namespace cinder {
class ciConfig;
}

//#define VERBOSE_SOURCE

//////////////////////////////////////
//
// Source Classes
//
namespace cinder { namespace qb {
	
	//
	// Source Base -- VIRTUAL
	class qbSourceBase {
	public:
		qbSourceBase();
		virtual ~qbSourceBase();

		void	makeUV( float _u, float _v );

		virtual void	updateFrame( bool _force=false )	{}	// update Pool
		
		virtual bool	load(const std::string & _f, char _flags=0)	{ return false; }
		const bool		isFresh()				{ return (mSpawnedAtFrame >= 0 && mSpawnedAtFrame == app::getElapsedFrames()); }

		virtual void	bind(int unit=0);
		virtual void	unbind();
		
		virtual void	enableAlphaBlending()	{ if (bHasAlpha) gl::enableAlphaBlending(); }
		virtual void	disableAlphaBlending()	{ if (bHasAlpha) gl::disableAlphaBlending(); }
		virtual void	drawInfo(const Vec2f & p);
		
		virtual void		play(bool _p=true)	{ bPlaying = _p; }
		virtual void		stop()				{ bPlaying = false; }
		virtual void		rewind()			{}
		bool				isPlaying()			{ return bPlaying; }

		virtual const int	getFrameCount()		{ return 1; }
		virtual const int	getCurrentFrame()	{ return 0; }
		virtual const float	getDuration()		{ return 0.0; }
		virtual const float	getCurrentTime()	{ return 0.0; }
		virtual const float	getFrameRate()		{ return 0.0; }
		
		const gl::Texture	& getTexture()
		{
			if ( ! mTex && mSurface )
				mTex = gl::Texture( mSurface );
			return mTex;
		}
		const Surface8u		& getSurface()
		{
			if ( ! mSurface && mTex )
				mSurface = Surface8u( mTex );
			return mSurface;
		}
		const GLfloat		* getTexes()		{ return (GLfloat*) mCubeTexes; }
		const std::string	& getName()			{ return mName; }
		const std::string	& getDesc()			{ return mDesc; }
		const bool			hasAlpha()			{ return bHasAlpha; }
		const bool			isRect()			{ return ( mTex ? (mTex.getTarget() == GL_TEXTURE_RECTANGLE_ARB) : false ); }
		const bool			isFlipped()			{ return ( mTex ? mTex.isFlipped() : false ); }
		const int			getWidth()			{ return (int)getSize().x; }
		const int			getHeight()			{ return (int)getSize().y; }
		const Vec2i			& getSize()			{ return mSize; }
		const Vec2f			& getUV()			{ return mUV; }
		Rectf				getBounds()			{ return Rectf(Vec2f::zero(),mSize); }
		
		ColorA				getColor(float _px, float _py);

	protected:
		bool				bPlaying;
		bool				bBundled;
		bool				bHasAlpha;
		int					mBoundUnit;
		int					mSpawnedAtFrame;	// frame number where source was loaded
		Vec2i				mSize;
		std::string			mName;
		std::string			mDesc;
		std::string			mDesc2;
		gl::Texture::Format mTexFormat;
		gl::Texture			mTex;
		Surface8u			mSurface;			// filled only if we need to read the texture
		Vec2f				mUV;
		GLfloat				mCubeTexes[QB_CUBE_FACES][QB_QUAD_VERTICES][2];
		GLboolean			mOldTargetBinding;
	};
	
	//
	// IMAGE Source
	class qbSourceImage : public qbSourceBase {
	public:
		//qbSourceImage()	{}
		//~qbSourceImage()	{}
		
		bool load(const std::string & _f, char _flags=0);
		
	private:
	};
	
	//
	// MOVIE Source
	class qbSourceMovie : public qbSourceBase {
	public:
		qbSourceMovie() : qbSourceBase()
		{
			mMovie = NULL;
#ifdef VERBOSE_SOURCE
			std::cout << ">>>>>>> qbSourceMovie CONSTRUCT this [" << this << "] " << mName << std::endl;
#endif
		}
		~qbSourceMovie()
		{
			if (mMovie)
				mMovie->stop();
#ifdef VERBOSE_SOURCE
			std::cout << ">>>>>>> qbSourceMovie DESTRUCT this [" << this << "] " << mName << std::endl;
#endif
		}
		
		bool		load(const std::string & _f, char _flags=0);
		void		updateFrame( bool _force=false );
		
		void		rewind()					{ if (mMovie) mMovie->seekToStart(); }

		const int			getFrameCount()		{ return mFrameCount; }
		const int			getCurrentFrame()	{ return mCurrentFrame; }
		const float			getDuration()		{ return mDuration; }
		const float			getCurrentTime()	{ return mCurrentTime; }
		const float			getFrameRate()		{ return mFrameRate; }
		
	private:
		qtime::MovieGl		mMovieGl;
		qtime::MovieSurface	mMovieSurface;
		qtime::MovieBase *	mMovie;
		int					mFrameCount;
		int					mCurrentFrame;
		float				mDuration;
		float				mCurrentTime;
		float				mFrameRate;
		
		double	mTimeProfiler;
	};
	
	//
	// SYPHON Source
	class qbSourceSyphon : public qbSourceBase {
	public:
		qbSourceSyphon() : qbSourceBase()
		{
			mSyphonClient.setup();
			mSyphonClient.setApplicationName( "" );
			mSyphonClient.setServerName("");
			mDesc = "Syphon";
		}
		//~qbSourceSyphon()
		
		bool load(const std::string & _app, char _flags=0);
		bool load(const std::string & _app, const std::string & _tex, char _flags=0);
		void updateFrame( bool _force=false );
		
		void bind(int unit=0)	{ mSyphonClient.bind(unit); }
		void unbind()			{ mSyphonClient.unbind(); }
		
	private:
		syphonClient			mSyphonClient;
	};
	
	
	//////////////////////////////////////
	//
	// SOURCE WRAPPER
	//
	class qbSourceSelector {
	public:
		qbSourceSelector() : mSrc( std::shared_ptr<qbSourceBase>( new qbSourceBase() ) )
		{
			mFlags = 0;
			mConfigSelectorId = -1;
			mConfigTriggerId = -1;
		}
		qbSourceSelector(const std::string & _f) : mSrc( std::shared_ptr<qbSourceBase>( new qbSourceBase() ) )
		{
			this->load(_f);
		}
		//~qbSourceSelector();
		
		// setup
		bool		load( const std::string & _f );
		bool		loadSyphon( const std::string & _app, const std::string & _tex="" );
		const bool	isFresh()						{ return ( mSrc ? mSrc->isFresh() : false ); }
		/// Flags
		char		setFlags( const char _flags)	{ return mFlags = _flags; }
		const char	getFlags()						{ return mFlags; }
		// Setters
		void		setSource( qbSourceBase * newSrc );
		// ciConfig integration
		void			setList( int _key, std::string _name );
		void			useConfigSelector( int _id, ci::ciConfig *_ptr=NULL );
		void			useConfigTrigger( int _id, ci::ciConfig *_ptr=NULL );
		std::string		* getNamePointer()		{ return &mConfigName; }
		std::string		* getDescPointer()		{ return &mConfigDesc; }
		gl::Texture		* getTexturePointer()	{ return &mConfigTexture; }

		// From actual source
		void			update();
		const void		bind(int unit=0)			{ if (mSrc) mSrc->bind(unit); }
		const void		unbind()					{ if (mSrc) mSrc->unbind(); }
		const void		enableAlphaBlending()		{ if (mSrc) mSrc->enableAlphaBlending(); }
		const void		disableAlphaBlending()		{ if (mSrc) mSrc->disableAlphaBlending(); }
		const void		drawInfo(const Vec2f & p)	{ if (mSrc) mSrc->drawInfo(p); }
		// movies
		const void		play(bool _p=true)		{ if (mSrc) mSrc->play(_p); }
		const void		stop()					{ if (mSrc) mSrc->stop(); }
		const void		rewind()				{ if (mSrc) mSrc->rewind(); }
		const bool		isPlaying()				{ return ( mSrc ? mSrc->isPlaying() : false ); }
		// movie getters
		const int		getFrameCount()			{ return ( mSrc ? mSrc->getFrameCount() : 0 ); }
		const int		getCurrentFrame()		{ return ( mSrc ? mSrc->getCurrentFrame() : 0 ); }
		const float		getDuration()			{ return ( mSrc ? mSrc->getDuration() : 0 ); }
		const float		getCurrentTime()		{ return ( mSrc ? mSrc->getCurrentTime() : 0 ); }
		const float		getFrameRate()			{ return ( mSrc ? mSrc->getFrameRate() : 0 ); }
		// generic getters
		const gl::Texture	& getTexture()		{ return ( mSrc ? mSrc->getTexture() : mNullTex ); }
		//const Surface8u		& getSurface()		{ return ( mSrc ? mSrc->getSurface() : mNullSurf ); }
		const GLfloat		* getTexes()		{ return ( mSrc ? mSrc->getTexes() : NULL ); }
		const std::string	getName()			{ return ( mSrc ? mSrc->getName() : "Null" ); }
		const std::string	getDesc()			{ return ( mSrc ? mSrc->getDesc() : "Null" ); }
		const bool		hasAlpha()				{ return ( mSrc ? mSrc->hasAlpha() : false ); }
		const bool		isRect()				{ return ( mSrc ? mSrc->isRect() : false ); }
		const bool		isFlipped()				{ return ( mSrc ? mSrc->isFlipped() : false ); }
		const int		getWidth()				{ return ( mSrc ? mSrc->getWidth() : 0 ); }
		const int		getHeight()				{ return ( mSrc ? mSrc->getHeight() : 0 ); }
		const Vec2i		& getSize()				{ return ( mSrc ? mSrc->getSize() : mNullVec2i ); }
		const Vec2f		& getUV()				{ return ( mSrc ? mSrc->getUV() : mNullVec2f ); }
		const Rectf		getBounds()				{ return ( mSrc ? mSrc->getBounds() : Rectf() ); }
		// from surface
		ColorA			getColor(float _px, float _py)	{ return ( mSrc ? mSrc->getColor(_px,_py) : ColorA::black() ); }

	protected:
		std::shared_ptr<qbSourceBase>		mSrc;
		std::map<int,std::string>			mList;
		char								mFlags;
		// ciConfig integration
		ci::ciConfig						*mConfigSelectorPtr, *mConfigTriggerPtr;
		int									mConfigSelectorId, mConfigTriggerId;
		std::string							mConfigName, mConfigDesc;
		gl::Texture							mConfigTexture;
		// dummies
		gl::Texture			mNullTex;
		Surface8u			mNullSurf;
		Vec2i				mNullVec2i;
		Vec2f				mNullVec2f;

	public:
	 	//@{
		//! Emulates shared_ptr-like behavior
		typedef std::shared_ptr<qbSourceBase> qbSourceSelector::*unspecified_bool_type;
		operator unspecified_bool_type() const { return ( mSrc.get() == 0 ) ? 0 : &qbSourceSelector::mSrc; }
		void reset() { mSrc.reset(); }
		//@}
	};
	
	
} } // cinder::qb

