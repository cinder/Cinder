//
//  qbMain.h
//
//  Created by Roger Sodre on 01/07/2011
//  Copyright 2011 Studio Avante. All rights reserved.
//
#pragma once

#include "cinder/Cinder.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Light.h"
#include "cinder/gl/Material.h"
#include "cinder/Camera.h"
#include "cinder/ImageIo.h"
#include "cinder/Utilities.h"

#include "cinderSyphon.h"
#include "qbUpdatePool.h"
#include "qbPlayhead.h"
#include "qbRenderer.h"
#include "qbSource.h"
#include "qbConfig.h"
#include "qbLight.h"
#include "qbDomeMaster.h"

#define QB_MAX_UNITS	8

namespace cinder { namespace qb {

class qbMain {
public:
	
	qbMain();
	~qbMain();
	void	init( int _w, int _h, float _fr=60.0f, bool _autoWindowSize=true );
	void	initDomeMaster();
	bool	onResize( app::ResizeEvent event );
	bool	onKeyDown( app::KeyEvent event );

	// update Pool
	void	updatePoolAdd( qbUpdateObject * o)		{ mUpdatePool.add(o); }
	void	updatePoolRemove( qbUpdateObject * o)	{ mUpdatePool.remove(o); }
	void	update();
	
	// FBOs
	void	makeFbo( int i );
	void	makeFbo( int i, float scale );
	void	makeFbo( int i, int w, int h );
	
	// Sources
	void	makeSource( int i, std::string _name="", char _flags=0 );
	void	makeSourceFromList( int i, const char *name0, ... );
	void	makeSourceFromList( int i, const std::vector<std::string> & names, char _flags=0 );
	void	addPath( const std::string & _path )	{ mPathList.push_back(_path); }
	qbSourceSelector &	source( int i )				{ return mSources[i]; }
	void	bindSource( int i, int unit=0 );
	void	unbindSource(int unit=0);
	void	unbindAllSources();
	
	// FBOs
	void	makeLight( int i, int type, const Vec3f &eye=Vec3f(0,0,1), const Vec3f &target=Vec3f::zero() );
	void	enableLight( int i, float intensity=1.0f );
	void	disableLights();
	gl::Light * light( int i )	{ return mLights[i]; }

	// Setup
	void	setFrameRate( float fr );
	void	resizeRender( int w, int h );
	void	resizePrint( int w, int h );
	void	resizeMetric( float w, float h );
	void	resizeWindow( int w, int h);
	void	resizePreview();
	void	setBackgroundColor( ci::Color _c )		{ mBackgroundColor = _c; }
	void	enableRenderControls( bool e=true )		{ bRenderControls=e; mConfig->enableRenderControls(e); };


	// camera setup
	void	resizeCameras();
	void	setCameraOffset( const Vec3f & off );
	void	setCameraScale( const Vec3f & sc );
	void	setCameraNear( const float n );
	void	setCameraOnTheGround( const bool b=true );
	void	setFarThrowMultiplyer( const float f );
	void	setStereoSeparation( float s );
	void	setCameraType( const int t );
	void	setCameraStereo( bool b=true );
	void	stereoSwitch();

	//
	// GETTERS
	//
	// Misc
	std::string & getAppName()			{ return mAppName; }
	std::string getFilePath( const std::string & _f );
	Vec2f &		getMousePos()			{ return mMousePos; }
	// Render
	float		getFrameRate()			{ return mConfig->getInt(QBCFG_TARGET_FRAMERATE); }
	int			getRenderWidth()		{ return mRenderWidth; }
	int			getRenderHeight()		{ return mRenderHeight; }
	int			getRenderAspect()		{ return mAspectRender; }
	Vec2i &		getRenderSize()			{ return mRenderSize; }
	Rectf &		getRenderBounds()		{ return mRenderBounds; }
	int			getWindowWidth()		{ return mWindowWidth; }
	int			getWindowHeight()		{ return mWindowHeight; }
	int			getPrintWidth()			{ return mPrintWidth; }
	int			getPrintHeight()		{ return mPrintHeight; }
	float		getPreviewScale()		{ return mPreviewScale; }
	Rectf &		getFittingBounds()		{ return mFittingBounds; }
	Area		getFittingArea()		{ return mFittingArea + mFittingAreaPan; }
	float		getMetricWidth()		{ return mMetricWidth; }
	float		getMetricHeight()		{ return mMetricHeight; }
	int			getMetricThrow()		{ return mMetricThrow; }
	float		getMetricAspect()		{ return mMetricAspect; }
	float		getMetricScale()		{ return mMetricScale; }
	Vec3f &		getMetricCenter()		{ return mMetricCenter; }
	Vec2f &		getMetricSize()			{ return mMetricSize; }
	Rectf &		getMetricBounds()		{ return mMetricBounds; }
	// Camera
	Camera*		getCamera()				{ return mCameraActive; }
	float		getCameraNear()			{ return mCameraNear; }
	float		getCameraFar()			{ return mCameraFar; }
	Vec3f		getCameraOffset()		{ return mCameraOffset; }
	Vec3f		getCameraScale()		{ return mCameraScale; }
	float		getStereoSeparation()	{ return mStereoSep; }
	int			getCameraType()			{ return mConfig->getInt(QBCFG_CAMERA_TYPE); }
	bool		isCameraOrtho()			{ return (this->getCameraType() == CAMERA_TYPE_ORTHO); }
	bool		isCameraStereo()		{ return (this->getCameraType() == CAMERA_TYPE_STEREO); }
	bool		isCameraGround()		{ return mConfig->getBool(QBCFG_CAMERA_GROUND); }
	bool		isCameraGroundActive()	{ return (this->isCameraGround() && (this->getCameraType() == CAMERA_TYPE_PERSP || this->getCameraType() == CAMERA_TYPE_STEREO)); }
	
	//
	// Playhead Control
	void		play( bool p=true );
	void		stop();
	void		playSwitch();
	void		rewind();
	// Playhead Getters
	double		getTime()				{ return mPlayhead.getSeconds(); }
	double		getSeconds()			{ return mPlayhead.getSeconds(); }
	int			getCurrentFrame()		{ return mPlayhead.getCurrentFrame(); }
	bool		isPlaying()				{ return mPlayhead.isPlaying(); }
	bool		isPlayingBackwards()	{ return mConfig->getBool(QBCFG_PLAY_BACKWARDS); }
	bool		isPreviewRealtime()		{ return (mConfig->getBool(QBCFG_REALTIME_PREVIEW) && ! this->isRendering()); }
	
	//
	// Renderer Control
	void		renderSetFolder( const std::string & _path );
	void		setFileMovie( const std::string & _folder );
	void		setFileImage( const std::string & _folder );
	
	void		renderSwitch();
	void		renderStart( const std::string & _f="" );
	void		renderStop();
	void		renderFinish();
	void		takeScreenshot()		{ mRenderer.takeScreenshot( mFboRender.getTexture() ); }
	// Renderer Getters
	bool		isRendering()			{ return mRenderer.isRendering(); }
	std::string & getRenderStatus()		{ return mRenderer.getStatus(); }
	std::string & getRenderProgress()	{ return mRenderer.getProgressString(); }
	std::string & getRenderTime()		{ return mRenderer.getTimeString(); }
	int			getRenderSeconds()		{ return mConfig->getInt(QBCFG_RENDER_SECONDS); }
	int			getRenderFrames()		{ return (int)(mConfig->get(QBCFG_RENDER_SECONDS) * mConfig->get(QBCFG_TARGET_FRAMERATE)); }
	int			getRenderStillSeconds()	{ return mConfig->getInt(QBCFG_RENDER_STILL_SECONDS); }
	bool		shouldRenderRewind()	{ return mConfig->getBool(QBCFG_RENDER_REWIND); }
	bool		shouldRenderFitSources(){ return mConfig->getBool(QBCFG_FIT_SOURCES_TO_RENDER); }
	
	//
	// DRAW LOOP
	//
	void	placeCamera( int type=-1 );			// Place METRIC Camera for drawing
	void	placeCameraLeft();					// Place STEREO Camera
	void	placeCameraRight();					// Place STEREO Camera
	void	placeCameraWindow();				// Place WINDOW camera to draw FBOs
	void	bindFbo()							{ mFboRender.bindFramebuffer(); }				// RENDER Fbo
	void	bindFboLeft()						{ if (mConfig->getInt(QBCFG_CAMERA_TYPE) == CAMERA_TYPE_STEREO) mFboLeft.bindFramebuffer(); }	// STEREO Fbo
	void	bindFboRight()						{ if (mConfig->getInt(QBCFG_CAMERA_TYPE) == CAMERA_TYPE_STEREO) mFboRight.bindFramebuffer(); }	// STEREO Fbo
	void	bindFbo( int i )					{ mFbos[i].bindFramebuffer(); }					// Additional FBOs
	void	unbindFbo()							{ mFboRender.unbindFramebuffer(); }				// Any Fbo
	void	bindFboTexture( int unit )			{ mFboRender.getTexture().bind(unit); }
	void	bindFboTexture( int i, int unit )	{ mFbos[i].getTexture().bind(unit); }
	void	unbindFboTexture()					{ mFboRender.getTexture().unbind(); }			// Any Fbo
	void	unbindFboTexture( int i )			{ mFbos[i].getTexture().unbind(); }				// Any Fbo
	void	drawFbo()							{ gl::draw( mFboRender.getTexture() ); }
	void	drawFbo( Rectf b )					{ gl::draw( mFboRender.getTexture(), b ); }
	void	drawFbo( Rectf b, Area a )			{ gl::draw( mFboRender.getTexture(), a, b ); }
	void	drawFbo( int i )					{ gl::draw( mFbos[i].getTexture() ); }
	void	drawFbo( int i, Rectf b )			{ gl::draw( mFbos[i].getTexture(), b ); }
	void	finishAndDraw();

	//
	// UTILITIES
	
	//
	// Public
	qbConfig		*mConfig;
	qbDomeMaster	mDomeMaster;
	Font			mFontHelvetica, mFont;
	std::string		mScreenName;
	bool			bVerbose;
	bool			bSyphonOutput;
	int				mDefaultCamera;

private:
	
	void		resizeFbos();
	void		blendStereo();
	void		setActiveCamera( int type=-1 );				// Select active camera
	void		updateCamera( Vec3f off=Vec3f::zero() );
	void		hideCursorOrNot();

	// misc
	bool						bInited;
	bool						bDrawGui;
	bool						bRenderControls;
	std::string					mAppName;
	std::vector<std::string>	mPathList;
	
	// QB
	qbPlayhead					mPlayhead;
	qbUpdatePool				mUpdatePool;
	qbRenderer					mRenderer;
	std::map<int,qbSourceSelector>	mSources;					// Active sources
	int							mBoundSource[QB_MAX_UNITS];		// currently bound source
	
	// GL
	ci::Color					mBackgroundColor;
	gl::Fbo::Format				mFboFormat;
	gl::Fbo						mFboRender;				// Final render FBO
	gl::Fbo						mFboLeft, mFboRight;	// Stereo FBOs
	std::map<int,gl::Fbo>		mFbos;					// Additional FBOs
	std::map<int,gl::Light*>	mLights;				// Active lights
	gl::Material				mMaterial;
	gl::GlslProg				mShaderStereo;
	
	// Syphon Server
	syphonServer		mSyphonServer;
	syphonClient		mSyphonModul8;
	gl::Texture			mSyphonIcon;
	gl::Texture			mCorner;
	
	// Render dimensions
	int					mRenderWidth;
	int					mRenderHeight;
	float				mAspectRender;
	Vec2i				mRenderSize;
	Rectf				mRenderBounds;
	// Print dimensions
	int					mPrintWidth;
	int					mPrintHeight;
	// Window dimensions
	int					mWindowWidth;
	int					mWindowHeight;
	float				mAspectWindow;
	float				mPreviewScale;			// fullscreen fbo scale
	Rectf				mFittingBounds;			// Bounds to fit render in window
	Area				mFittingArea;			// Bounds to fit render in window
	Vec2f				mFittingAreaPan;
	Vec2f				mMousePos;				// metric unit
	Vec2f				mMousePan;				// metric unit
	// Scene dimensions
	float				mMetricWidth;
	float				mMetricHeight;
	float				mMetricThrow;			// camera throw
	float				mMetricAspect;
	float				mMetricScale;
	Vec2f				mMetricSize;
	Rectf				mMetricBounds;
	Vec3f				mMetricCenter;
	// Cameras
	Vec3f				mCamEye;
	Vec3f				mCamTarget;
	Camera *			mCameraActive;
	CameraOrtho			mCameraOrtho;
	CameraOrtho			mCameraDome;
	CameraPersp			mCameraPersp;
	CameraPersp			mCameraLeft, mCameraRight;	// stero cameras
	float				mCameraNear, mCameraFar;
	float				mFarThrowMultiplyer;		// camera far = metric throw * this
	Vec3f				mCameraOffset;				// camera eye offset
	Vec3f				mCameraScale;				// coordinate scale
	// stereo cam
	float				mStereoSep;
	float				mStereoDelta;

};

// Globals
extern qbMain __qb;


} } // cinder::qb


