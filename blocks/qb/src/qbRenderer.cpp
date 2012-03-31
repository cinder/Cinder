//
//  qbRenderer.h
//
//  Created by Roger Sodre on 01/07/2011
//  Copyright 2011 Studio Avante. All rights reserved.
//
#include <stdlib.h>
#include "qbRenderer.h"
#include "qb.h"
#include "cinder/Filesystem.h"
#include "QuickTime/ImageCompression.h"

using namespace ci;
using namespace ci::gl;
using namespace ci::qtime;

//#define EXPORT_FRAMES_AS_PNG

namespace cinder { namespace qb {
	
	
	//////////////////////////////////////////////////////////////////
	//
	// QB MANAGER WRAPPERS
	//
	// Play / Pause
	void qbMain::renderSetFolder( const std::string & _folder )
	{
		mRenderer.setFolder( _folder );
	}
	void qbMain::setFileMovie( const std::string & _folder )
	{
		mRenderer.setFileMovie( _folder );
	}
	void qbMain::setFileImage( const std::string & _folder )
	{
		mRenderer.setFileImage( _folder );
	}
	void qbMain::renderSwitch()
	{
		if ( ! mRenderer.isRendering() )
			this->renderStart();
		else 
			this->renderStop();
	}
	void qbMain::renderStart( const std::string & _f )
	{
		mRenderer.start( _f );
		this->play();
		if ( _qb.shouldRenderRewind() )
			this->rewind();
	}
	void qbMain::renderStop()
	{
		mRenderer.stop();
	}
	void qbMain::renderFinish()
	{
		mRenderer.finish();
	}
	
	
	
	//////////////////////////////////////////////////////////////////
	//
	// RENDERER
	//
	qbRenderer::qbRenderer()
	{
		//mAudioStop = audio::load( loadResource( "audio_render_stopped.mp3" ) );
		//mAudioFinish = audio::load( loadResource( "audio_render_finished.mp3" ) );
		//mAudioScreenshot = audio::load( loadResource( "audio_screenshot.mp3" ) );
		// make file name base
		this->setFileMovie( "__render.mov" );
		this->setFileImage( "__screenshot.png" );
		// Reset !
		this->reset();
		mStatus = "Renderer Ready";
		mProgressString = "Frames: 0";
		mTimeString = "Time: 0:00";
		mFolder = QB_CAPTURE_FOLDER;
	}
	qbRenderer::~qbRenderer()
	{
		if ( bOpened )
			this->finish();
	}
	void qbRenderer::setFolder( const std::string & _folder )
	{
		mFolder = _folder;
	}
	void qbRenderer::setFileMovie( const std::string & _f )
	{
		mFileBaseMovie = _f;
	}
	void qbRenderer::setFileImage( const std::string & _f )
	{
		mFileBaseImage = _f;
	}
	void qbRenderer::reset()
	{
		bOpened = false;
		bIsRendering = false;
		mFramesMax = 0;
		mFramesStill = 0;
		mFramesAdded = 0;
		mFramesStarted = 0;
	}
	// Get render progression
	// before: sampling before actual animation
	float qbRenderer::getProg()
	{
		return (mFramesAdded-mFramesStarted) / (float)(mFramesMax-mFramesStarted-1);
	}
	
	//
	// Make name with serial number: mFolder_XX.mov
	std::string qbRenderer::makeFileNameSerial( std::string _p, std::string _f )
	{
		//std::string name = _f.substr(0,_f.rfind("."));
		std::string name = getPathFileName( _f );
		std::string ext = getPathExtension( _f );
		std::ostringstream os;
		bool found = true;
		for (int count = 0 ; found ; count++)
		{
			os.str("");
			os << _p << "/" << name << "_" << count << "." << ext;
			fs::path p( os.str() );
			found = exists( p );
		}
		return os.str();
	}
	std::string qbRenderer::makeFileNameTime( std::string _p, std::string _f )
	{
		time_t now;
		time ( &now );
		struct tm * t = localtime( &now );
		//std::string name = _f.substr(0,_f.rfind("."));
		std::string name = getPathFileName( _f );
		std::string ext = getPathExtension( _f );
		std::ostringstream os;
		os << _p << "/" << name << "_"
			<< (t->tm_year + 1900)
			<< "." << std::setfill('0') << std::setw(2) << (t->tm_mon + 1) 
			<< "." << std::setfill('0') << std::setw(2) << (t->tm_mday)
			<< "-" << std::setfill('0') << std::setw(2) << (t->tm_hour) 
			<< "." << std::setfill('0') << std::setw(2) << (t->tm_min) 
			<< "." << std::setfill('0') << std::setw(2) << (t->tm_sec)
			<< "." << ext;
		return os.str();
	}
	// Make framename for PNG export
	std::string qbRenderer::makeFrameNameSerial()
	{
		char num[6];
		sprintf(num,"%05d",mFramesAdded);
		std::ostringstream os;
		os << mFile << "_" << num << ".png";
		return os.str();
	}
	//
	// Open export file
	void qbRenderer::open()
	{
		if ( mFolder.length() == 0 )
			this->setFolder( app::getAppPath().string() + "/.." );
		
		// Make file name
		mFile = makeFileNameTime( mFolder, mFileBaseMovie );
		
		// Open target file
		MovieWriter::Format format = MovieWriter::Format();
		format.setDefaultDuration( QB_FRAME_DURATION );
		format.setCodec('jpeg');
		mMovieWriter = qtime::MovieWriter( mFile, QB_RENDER_WIDTH, QB_RENDER_HEIGHT, format );
		
		// New render
		mFramesAdded = 0;
		bOpened = true;

		// new status
		std::ostringstream os;
		os << "RENDER OPEN [" << mFile << "] "<<mMovieWriter.getWidth()<<" x "<<mMovieWriter.getHeight();
		printf("%s\n",os.str().c_str());
	}

	//
	// REC-HEAD
	void qbRenderer::start( const std::string & _f )
	{
		if ( _f.length() )
			mFileBaseMovie = _f;
		
		// open file
		if ( ! bOpened )
			this->open();
		
		// Start!
		mFramesStarted = mFramesAdded;
		mFramesMax = ( _qb.getRenderSeconds() ? mFramesAdded+(int)(_qb.getRenderSeconds()*QB_FRAMERATE) : 0 );
		mFramesStill =  (int)(_qb.getRenderStillSeconds()*QB_FRAMERATE);
		mTimeStart = app::getElapsedSeconds();

		// Start!
		App::get()->setFrameRate( 1000.0 );
		bIsRendering = true;
		
		mStatus = "Rendering...";
		printf("RENDER START max %d  still %d\n",mFramesMax,mFramesStill);
	}
	void qbRenderer::stop()
	{
		if ( bIsRendering )
		{
			App::get()->setFrameRate( _qb.getFrameRate() );
			bIsRendering = false;
			mStatus = "Stopped (must finish)";
			//audio::Output::play( mAudioStop );
			printf("RENDER STOPPED!\n");
		}
		this->updateStatus();
	}
	
	//
	// Finish export
	void qbRenderer::finish()
	{
		if ( mFramesAdded == 0 || ! bOpened )
		{
			mStatus = "Nothing to Finish!";
			printf("NO FRAMES TO FINISH!!!\n");
			return;
		}
		mMovieWriter.finish();
		mStatus = "Finished!";
		this->reset();
		_cfg.clearRenderTexture( Color::green() );
		//audio::Output::play( mAudioFinish );
		printf("RENDER FINISHED!!! [%s]\n",mFile.c_str());
	}
	
	//
	// Add frame to movie
	void qbRenderer::add()
	{
		this->add( copyWindowSurface() );
	}
	void qbRenderer::add( const ImageSourceRef & _aframe )
	{
		if ( ! bIsRendering )
			return;
		
		std::ostringstream os;
		if ( mFramesMax == 0)
		{
			os << "RENDER add frame "<<mFramesAdded<<" (ulimited)";
			printf("%s\n",os.str().c_str());
			this->commit( _aframe );
			mFramesAdded++;
		}
		else if (mFramesAdded < mFramesMax)
		{
			os << "RENDER add frame "<<mFramesAdded<<"/"<<mFramesMax<<"  "<<(int)(this->getProg()*100)<<"%";
			printf("%s\n",os.str().c_str());
			this->commit( _aframe );
			mFramesAdded++;
			mTimeElapsed = app::getElapsedSeconds() - mTimeStart;
			// Reached the end
			if ( mFramesAdded == mFramesMax )
			{
				// add still frames
				if ( mFramesStill > 0 )
				{
					//float stillSecs = (mFramesStill * QB_FRAME_DURATION);
					//mMovieWriter.addFrame( _aframe, stillSecs );
					for (int n = 0 ; n < mFramesStill ; n++)
						this->commit( _aframe );
					mFramesAdded += mFramesStill;
					os.str("");
					os << "RENDER still "<<mFramesStill<<" frames";
					printf("%s\n",os.str().c_str());
				}
				// HACK!! Losing last frame, add again!
				else
					this->commit( _aframe );
				// Stop!!
				this->stop();
			}
		}
		this->updateStatus();
	}
	void qbRenderer::commit( const ImageSourceRef & _aframe )
	{
#ifdef EXPORT_FRAMES_AS_PNG
		this->takeScreenshot( _aframe, this->makeFrameNameSerial() );
#else
		mMovieWriter.addFrame( _aframe, QB_FRAME_DURATION );
#endif
	}
	
	void qbRenderer::updateStatus()
	{
		std::ostringstream os;
		if ( mFramesMax == 0)
			os << "Rendering... (unlimited)";
		else
			os << "Rendering... "<<(int)(this->getProg()*100)<<"%";
		mStatus = os.str();
		
		// Frame count
		os.str("");
		if ( bIsRendering )
		{
			// Progress
			os << "Frames: " << mFramesAdded << " / " << ( mFramesMax == 0 ? std::string("?") : toString(mFramesMax) );
			mProgressString = os.str();
			// Time
			double mTimeEstimated = ( mTimeElapsed / this->getProg() );
			double mTimeRemaining = mTimeEstimated - mTimeElapsed;
			os.str("");
			os << "Time: " 
				<< (int)(mTimeEstimated/60.0) << ":" << std::setfill('0') << std::setw(2) << (int)(mTimeEstimated-(int)(mTimeEstimated/60.0)) << " - "
				<< (int)(mTimeElapsed/60.0) << ":" << std::setfill('0') << std::setw(2) << (int)(mTimeElapsed-(int)(mTimeEstimated/60.0)) << " = "
				<< (int)(mTimeRemaining/60.0) << ":" << std::setfill('0') << std::setw(2) << (int)(mTimeRemaining-(int)(mTimeRemaining/60.0)) << " s";
			mTimeString = os.str();
		}
		else
		{
			os.setf(std::ios::fixed);
			os.precision(1);
			os << "Frames: " << mFramesAdded;
			mProgressString = os.str();
			// Time
			os.str("");
			os << "Time: " << (int)mTimeElapsed << ":" << (int)((mTimeElapsed-(int)mTimeElapsed)*60) << " s";
			mTimeString = os.str();
		}
	}

	//
	// Save Screenshot
	void qbRenderer::takeScreenshot( const ImageSourceRef & _aframe)
	{
		std::string filename = this->makeFileNameTime( mFolder, mFileBaseImage );
		this->takeScreenshot( _aframe, filename );
	}
	void qbRenderer::takeScreenshot( const ImageSourceRef & _aframe, const std::string _filename )
	{
		writeImage( _filename, _aframe );
		//audio::Output::play( mAudioScreenshot );
		printf("RENDER SCREENSHOT [%s]\n",_filename.c_str());
	}
	
} } // cinder::qb







