//
//  qbSource.h
//
//  Created by Roger Sodre on 01/07/2011
//  Copyright 2011 Studio Avante. All rights reserved.
//

#include "cinder/Filesystem.h"

#include "qb.h"
#include "qbSource.h"

using namespace ci;
using namespace ci::app;
using namespace ci::gl;
using namespace ci::qtime;

namespace cinder { namespace qb {
	
	//////////////////////////////////////////////////////////////////
	//
	// WRAPPER Source
	//
	bool qbSourceSelector::load( const std::string & _f )
	{
		printf("SOURCE [%s]\n",_f.c_str());
		// Syphn prefix
		if ( _f.compare(0,8,"syphon::") == 0 )
			return this->loadSyphon( _f.substr(8), "" );
		
		// Load by extension
		std::string ext = getPathExtension( _f );
		if ( ext.compare(0,3,"mov") == 0 || ext.compare(0,3,"MOV") == 0 )
		{
			qbSourceMovie *newSrc = new qbSourceMovie();
			if ( newSrc->load(_f,mFlags) == false )
			{
				delete newSrc;
				return false;
			}
			this->setSource(newSrc);
		}
		else
		{
			qbSourceImage *newSrc = new qbSourceImage();
			if ( newSrc->load(_f,mFlags) == false )
			{
				delete newSrc;
				return false;
			}
			this->setSource(newSrc);
		}
		return true;
	}
	bool qbSourceSelector::loadSyphon( const std::string & _app, const std::string & _tex )
	{
		qbSourceSyphon *newSrc = new qbSourceSyphon();
		newSrc->load( _app, _tex, mFlags );
		this->setSource(newSrc);
		return true;
	}
	void qbSourceSelector::setSource( qbSourceBase * newSrc )
	{
		mSrc = std::shared_ptr<qbSourceBase>( newSrc );
	}
	
	// Source shortcut list
	void qbSourceSelector::setList( int _key, std::string _name )
	{
		mList[_key] = _name;
		if (mConfigSelectorId >= 0)
			mConfigSelectorPtr->setValueLabel( mConfigSelectorId, _key,  getPathFileName(_name) );
	}
	void qbSourceSelector::useConfigSelector( int _id, ciConfig *_ptr )
	{
		mConfigSelectorId = _id;
		mConfigSelectorPtr = ( _ptr ? _ptr : &_cfg );
		//mConfigSelectorPtr->setValueLabels( mConfigSelectorId, mList );
		std::map<int,std::string>::const_iterator it;
		for ( it = mList.begin() ; it != mList.end(); it++ )
			mConfigSelectorPtr->setValueLabel( mConfigSelectorId, it->first,  getPathFileName(it->second) );
	}
	void qbSourceSelector::useConfigTrigger( int _id, ciConfig *_ptr )
	{
		mConfigTriggerId = _id;
		mConfigTriggerPtr = ( _ptr ? _ptr : &_cfg );
		if (mConfigTriggerPtr->getBool(mConfigTriggerId))
			this->play();
		else
			this->stop();
	}
	//
	// Main Loop update
	void qbSourceSelector::update()
	{
		// Config Selector
		if (mConfigSelectorId >= 0)
		{
			if ( mConfigSelectorPtr->isFresh(mConfigSelectorId) )
			{
				int i = mConfigSelectorPtr->getInt(mConfigSelectorId);
				if (mList.find(i) != mList.end())
				{
					bool playing = ( mSrc ? mSrc->isPlaying() : false );
					if ( this->load( mList[i] ) )
						this->play( playing );
					return;
				}
			}
		}
		// Config Trigger
		if (mConfigTriggerId >= 0)
		{
			bool shouldBePlaying = ( mConfigTriggerPtr->getBool(mConfigTriggerId) != 0);
			if ( ! mSrc->isPlaying() && shouldBePlaying )
				this->play();
			else if ( mSrc->isPlaying() && ! shouldBePlaying )
				this->stop();
		}
		// update Frame
		mSrc->updateFrame();
	}

	
	///////////////////////////////////////////
	//
	// BASE
	//
	qbSourceBase::qbSourceBase()
	{
		bPlaying		= false;
		bBundled		= false;
		bHasAlpha		= false;
		mBoundUnit		= 0;
		mSpawnedAtFrame = -1;
		mSize = Vec2f::zero();
		mTexFormat.setWrapS( GL_REPEAT );
		mTexFormat.setWrapT( GL_REPEAT );
		mTexFormat.enableMipmapping();
		mTexFormat.setMinFilter(GL_LINEAR_MIPMAP_LINEAR);
		mTexFormat.setMagFilter(GL_LINEAR_MIPMAP_LINEAR);
		mTexFormat.setTarget( GL_TEXTURE_RECTANGLE_ARB );	// like MovieGl/Syphon
		mUV = Vec2f::zero();
		this->makeUV( 1.0f, 1.0f );
#ifdef VERBOSE_SOURCE
		std::cout << ">>>>>>> qbSourceBase CONSTRUCT this [" << this << "] " << mName << std::endl;
#endif
	}
	qbSourceBase::~qbSourceBase()
	{
#ifdef VERBOSE_SOURCE
		std::cout << ">>>>>>> qbSourceBase DESTRUCT this [" << this << "] " << mName << std::endl;
#endif
	}
	//
	// Draw Source info on screen
	void qbSourceBase::drawInfo( const Vec2f & p )
	{
		Vec2f pp = p + Vec2f(3,0);
		gl::enableAlphaBlending();
		if ( mName.length() )
			gl::drawString( mName, pp, Color::white(), _qb.mFontBit);
		else
			gl::drawString( "Source not loaded!", p, Color::white(), _qb.mFontBit);
		if ( mDesc.length() )
		{
			pp += Vec2f(0,10);
			gl::drawString( mDesc, pp, Color::white(), _qb.mFontBit);
		}
		if ( mDesc2.length() )
		{
			pp += Vec2f(0,10);
			gl::drawString( mDesc2, pp, Color::white(), _qb.mFontBit);
		}
		gl::disableAlphaBlending();
	}
	//
	// Make cube texture coordinates
	void qbSourceBase::makeUV( float _u, float _v )
	{
		if ( mUV.x != _u || mUV.y != _v )
		{
			mUV = Vec2f( _u, _v );
			for (int f = 0 ; f < QB_CUBE_FACES ; f++)
			{
				for (int v = 0 ; v < QB_QUAD_VERTICES ; v++)
				{
					mCubeTexes[f][v][0] = __qbCubeTexesQuad[f][v][0] * _u;
					mCubeTexes[f][v][1] = __qbCubeTexesQuad[f][v][1] * _v;
					if ( mTex )
						if( mTex.isFlipped() )
							mCubeTexes[f][v][1] = _v - mCubeTexes[f][v][1];
					//printf("makeUV uv[%.1f/%.1f]  f %d  v %d  tex[%.1f/%.1f]\n",_u,_v,f,v,mCubeTexes[f][v][0],mCubeTexes[f][v][1]);
				}
			}
		}
	}
	//
	// Bind saving/retrieving current target
	void qbSourceBase::bind(int unit)
	{
		if (mTex)
		{
			glGetBooleanv( mTex.getTarget(), &mOldTargetBinding );
			mTex.enableAndBind( unit );
			mBoundUnit = unit;
		}
	}
	void qbSourceBase::unbind()
	{
		if (mTex)
		{
			mTex.unbind( mBoundUnit );
			if( mOldTargetBinding )
				glEnable( mTex.getTarget() );
			else
				glDisable( mTex.getTarget() );
		}
	}

	//
	// MOTOLED
	// input x/y as prog (0.0 to 1.0)
	ColorA qbSourceBase::getColor( float _px, float _py )
	{
		const Surface8u & surf = this->getSurface();
		if ( ! surf )
			return ColorAf::white();
		int x = (int) (_px * mSize.x);
		int y = (int) (_py * mSize.y);
		ColorA c = surf.getPixel(Vec2i(x,y));
		//printf("____color %.2f %.2f = %d %d = %.2f %.2f %.2f\n",_px,_py,x,y,c.r,c.g,c.b);
		return ( c.a == 0 ? ColorA::zero() : c );
	}
	
	
	///////////////////////////////////////////
	//
	// IMAGE
	//
	bool qbSourceImage::load( const std::string & _f, char _flags )
	{
		std::string theFile = _qb.getFilePath( _f );
		if ( theFile.length() == 0 )
			return false;

		// try to load new image
		Texture newTex = gl::Texture( loadImage( loadFile( theFile ) ), mTexFormat );
		if ( ! newTex )
			return false;
		
		// save new image
		mTex = newTex;
		mSize = mTex.getSize();
		bHasAlpha = mTex.hasAlpha();
		mSurface = Surface8u();		// clear surface
		this->makeUV( mTex.getMaxU(), mTex.getMaxV() );

		// make name
		std::stringstream os;
		os << getPathFileName( theFile );
		if ( theFile.find(".app/Contents/Resources/") != std::string::npos )
			os << " (Bundle)";
		mName = os.str();

		// make desc
		os.str("");
		os << "Image: " << mSize.x << " x " << mSize.y << " " << ( bHasAlpha ? "RGBA" : "RGB" );
		mDesc = os.str();
		mDesc2 = "";
		mSpawnedAtFrame = getElapsedFrames();
		
		printf("SOURCE Image [%s] loaded!\n",theFile.c_str());
		return true;
	}
	
	
	
	///////////////////////////////////////////
	//
	// MOVIE
	//
	bool qbSourceMovie::load( const std::string & _f, char _flags )
	{
		std::string theFile = _qb.getFilePath( _f );
		if ( theFile.length() == 0 )
			return false;
		
		// try to load new movie
		try {
			printf("SRF FLAG %d [%s]\n",_flags,_f.c_str());
			
			if ( TEST_FLAG( _flags, QBFAG_SURFACE) )
			{
				mMovieGl = qtime::MovieGl();
				mMovieSurface = qtime::MovieSurface( theFile );
				mMovie = & mMovieSurface;
			}
			else
			{
				mMovieGl = qtime::MovieGl( theFile );
				mMovieSurface = qtime::MovieSurface();
				mMovie = & mMovieGl;
			}
			if ( ! mMovie )
				return false;
			mMovie->play();
			mMovie->stop();			// we're gonna play it frame by frame
			mMovie->setLoop();
			mMovie->setVolume(0);
			mSize = mMovie->getSize();
			bHasAlpha = mMovie->hasAlpha();
			mFrameCount = mMovie->getNumFrames() + 1;
			mDuration = mMovie->getDuration();
			mFrameRate = mMovie->getFramerate();
		}
		catch( ... ) {
			printf("ERRO!!! MovieGl throws...\n");
			return false;
		}
		
		// grab a frame
		this->updateFrame(true);
		
		// make name
		std::stringstream os;
		os << getPathFileName( theFile );
		if ( theFile.find(".app/Contents/Resources/") != std::string::npos )
			os << " (Bundle)";
		mName = os.str();
		
		// make desc
		os.str("");
		os.setf(std::ios::fixed);
		os.precision(1);
		os << "Movie: " << mSize.x << " x " << mSize.y << " " << mDuration << "s";
		mDesc = os.str();
		mSpawnedAtFrame = getElapsedFrames();
		
		printf("SOURCE Movie [%s] loaded as %s!\n",theFile.c_str(),(mMovieGl?"MovieGl":"MovieSurface"));
		return true;
	}
	//
	// Get new Frame
	// qbUpdateObject VIRTUAL
	void qbSourceMovie::updateFrame( bool _force )
	{
		if ( ! mMovie )
			return;
		
		// maybe this shouldn't be playing...
		if ( ! _force && ! (bPlaying && _qb.isPlaying()) )
		{
			if ( mMovie->isPlaying() )
				mMovie->stop();
			return;
		}
		
		double d = (app::getElapsedSeconds() - mTimeProfiler);
		//printf("%d) t(%.5f) QB source in %s...\n",app::getElapsedFrames(),(float)d,mName.c_str());
		mTimeProfiler = app::getElapsedSeconds();

		bool newFrame = false;

		// Always play frame by frame
		float t = ( _qb.shouldRenderFitSources() ? QB_ANIM_PROG * mDuration : QB_TIME );
		mMovie->seekToTime( math<float>::fmod( t, mDuration ) );
		//mMovie->seekToFrame( _qb.getCurrentFrame() % mFrameCount );
		newFrame = true;
		//if (_qb.isRendering())
		//	printf("SET FRAME  anim fr %d  t %.3f  >>  t %.3f/%.3f  new %d  (%s)\n",_qb.getCurrentFrame(),t,mMovie->getCurrentTime(),mDuration,(int)newFrame,mName.c_str());
		
		// load new frame?
		if ( newFrame )
		{
			if ( mMovieGl )
			{
				mTex = mMovieGl.getTexture();
				mSurface = Surface8u();
			}
			else
			{
				mSurface = mMovieSurface.getSurface();
				mTex = gl::Texture( mSurface );
			}
			mCurrentTime = mMovie->getCurrentTime();
			mCurrentFrame = (( mCurrentTime / mDuration) * mFrameCount);

			// calc UV?
			this->makeUV( mTex.getMaxU(), mTex.getMaxV() );
			
			// update desc
			std::stringstream os;
			os << mFrameCount << " f .. " << mCurrentFrame << " f / ";
			os.setf(std::ios::fixed);
			os.precision(1);
			os << mDuration << " s .. " << mCurrentTime << " s";
			mDesc2 = os.str();
		}

		
		d = (app::getElapsedSeconds() - mTimeProfiler);
		//rintf("%d) t(%.5f) QB source out...\n",app::getElapsedFrames(),(float)d);
		mTimeProfiler = app::getElapsedSeconds();

	}
	
	
	
	///////////////////////////////////////////
	//
	// SYPHON
	//
	bool qbSourceSyphon::load( const std::string & _app, char _flags )
	{
		return this->load( _app, "", _flags );
	}
	bool qbSourceSyphon::load( const std::string & _app, const std::string & _tex, char _flags )
	{
		mSyphonClient.setApplicationName( _app );
		mSyphonClient.setServerName(_tex);
		this->updateFrame(true);

		// source properties
		bHasAlpha = true;
		
		// Make name
		std::stringstream os;
		os << _app;
		if ( _tex.length() )
			os << " - " << _tex;
		mName = os.str();
		mDesc2 = "";
		mSpawnedAtFrame = getElapsedFrames();

		printf("SOURCE Syphon [%s] loaded!\n",_app.c_str());
		return true;
	}
	//
	// Get new Frame
	// qbUpdateObject VIRTUAL
	void qbSourceSyphon::updateFrame( bool _force )
	{
		if ( ! _force && ! (bPlaying && _qb.isPlaying()) )
			return;
		mSyphonClient.update();
		mTex = mSyphonClient.getTexture();
		mSize = mSyphonClient.getSize();
		mSurface = Surface8u();		// clear surface
		// calc UV?
		if (mTex)
			this->makeUV( mTex.getMaxU(), mTex.getMaxV() );
		// update desc
		std::stringstream os;
		os << "Syphon: " << mSize.x << " x " << mSize.y;
		mDesc = os.str();
	}
	
	
	
} } // cinder::qb







