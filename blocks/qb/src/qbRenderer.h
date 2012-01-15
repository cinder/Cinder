//
//  qbRenderer.h
//
//  Created by Roger Sodre on 01/07/2011
//  Copyright 2011 Studio Avante. All rights reserved.
//
#pragma once

#include "cinder/Cinder.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
//#include "cinder/audio/Output.h"
//#include "cinder/audio/Io.h"
#include "cinder/qtime/QuickTime.h"
#include "cinder/qtime/MovieWriter.h"

namespace cinder { namespace qb {
	
	class qbRenderer {
	public:
		qbRenderer();
		~qbRenderer();
		
		std::string &		getStatus()			{ return mStatus; }
		std::string &		getProgress()		{ return mProgress; }
		bool				isRendering()		{ return bIsRendering; }
		float				getProg();
		
		void				setFolder( const std::string & _folder );
		void				setFileMovie( const std::string & _f );
		void				setFileImage( const std::string & _f );
		
		void				start( const std::string & _f="" );
		void				stop();
		void				finish();
		
		void				takeScreenshot( const ImageSourceRef & _aframe );
		void				takeScreenshot( const ImageSourceRef & _aframe, const std::string _filename );
		
		void				add();
		void				add( const ImageSourceRef & _aframe );
		
		
	private:
		
		void				reset();
		std::string			makeFileNameSerial( std::string _p, std::string _f );
		std::string			makeFileNameTime( std::string _p, std::string _f );
		std::string			makeFrameNameSerial();
		void				open();
		void				commit( const ImageSourceRef & _aframe );
		void				updateStatus();
		
		qtime::MovieWriter	mMovieWriter;
		std::string			mStatus;
		std::string			mProgress;
		std::string			mFolder;
		std::string			mFileBaseMovie;
		std::string			mFileBaseImage;
		std::string			mFile;
		int					mFramesMax;
		int					mFramesStill;
		int					mFramesAdded;
		int					mFramesStarted;		// for progress only
		bool				bOpened;
		bool				bIsRendering;
		
		//audio::SourceRef	mAudioStop;
		//audio::SourceRef	mAudioFinish;
		//audio::SourceRef	mAudioScreenshot;
		
	};
	
	
} } // cinder::qb


