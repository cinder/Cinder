//
//  qbConfig.cpp
//
//  Created by Roger Sodre on 01/07/2011
//  Copyright 2011 Studio Avante. All rights reserved.
//

#include "qbConfig.h"
#include "qb.h"
#include <sstream>

using namespace ci;
using namespace ci::sgui;

//
// Config Class
qbConfig::qbConfig() : ciConfigGuiSimple()
{
	// Camera
	this->addInt(QBCFG_CAMERA_TYPE, "QBCFG_CAMERA_TYPE", CAMERA_TYPE_PERSP, 0, CAMERA_TYPE_COUNT-1);
	this->addBool(QBCFG_CAMERA_GROUND, "QBCFG_CAMERA_GROUND", false);
	this->addFloat(QBCFG_METRIC_THROW, "QBCFG_METRIC_THROW", 1.0f, 0.1f, 1.0f);
	// Animation
	this->addBool(QBCFG_PLAYING, "QBCFG_PLAYING", true);
	this->addBool(QBCFG_PLAY_BACKWARDS, "QBCFG_PLAY_BACKWARDS", false);
	this->addBool(QBCFG_REALTIME_PREVIEW, "QBCFG_REALTIME_PREVIEW", true);
	this->addBool(QBCFG_PREVIEW_FIT, "QBCFG_PREVIEW_FIT", true);
	this->addFloat(QBCFG_CURRENT_TIME, "QBCFG_CURRENT_TIME", 0.0);
	//this->setDummy(QBCFG_CURRENT_TIME);
	// Render
	this->addInt(QBCFG_TARGET_FRAMERATE, "QBCFG_TARGET_FRAMERATE", 30, 1, 60);
	this->addInt(QBCFG_RENDER_SECONDS, "QBCFG_RENDER_SECONDS", 10, 0, 120);
	this->addInt(QBCFG_RENDER_STILL_SECONDS, "QBCFG_RENDER_STILL_SECONDS", 0, 0, 60);
	this->addBool(QBCFG_RENDER_REWIND, "QBCFG_RENDER_REWIND", true);
	this->addBool(QBCFG_FIT_SOURCES_TO_RENDER, "QBCFG_FIT_SOURCES_TO_RENDER", true);
	this->addBool(QBCFG_MODUL8_INPUT, "QBCFG_MODUL8_INPUT", false);
	this->addBool(QBCFG_SYPHON_OUTPUT, "QBCFG_SYPHON_OUTPUT", false);
	
	// Readonly
	this->addInt(DUMMY_RENDER_WIDTH, "DUMMY_RENDER_WIDTH", 0);
	this->addInt(DUMMY_RENDER_HEIGHT, "DUMMY_RENDER_HEIGHT", 0);
	this->addInt(DUMMY_QB_WIDTH, "DUMMY_QB_WIDTH", 0);
	this->addInt(DUMMY_QB_HEIGHT, "DUMMY_QB_HEIGHT", 0);
	this->addInt(DUMMY_QB_THROW, "DUMMY_QB_THROW", 0);
	this->addFloat(DUMMY_MOUSE_X, "DUMMY_MOUSE_X", 0, -10000.0, 10000.0);
	this->addFloat(DUMMY_MOUSE_Y, "DUMMY_MOUSE_Y", 0, -10000.0, 10000.0);
	this->addFloat(DUMMY_PREVIEW_SCALE, "DUMMY_PREVIEW_SCALE", 1.0, 0.01, 1000.0);
	this->addFloat(DUMMY_CURRENT_FPS, "DUMMY_CURRENT_FPS", 0.0);
	this->addInt(DUMMY_CURRENT_FRAME, "DUMMY_CURRENT_FRAME", 0);
	this->addString(DUMMY_RENDER_STATUS, "DUMMY_RENDER_STATUS", "");
	this->addString(DUMMY_RENDER_PROGRESS, "DUMMY_RENDER_PROGRESS", "");
	//
	this->setDummy(DUMMY_RENDER_WIDTH);
	this->setDummy(DUMMY_RENDER_HEIGHT);
	this->setDummy(DUMMY_QB_WIDTH);
	this->setDummy(DUMMY_QB_HEIGHT);
	this->setDummy(DUMMY_QB_THROW);
	this->setDummy(DUMMY_MOUSE_X);
	this->setDummy(DUMMY_MOUSE_Y);
	this->setDummy(DUMMY_PREVIEW_SCALE);
	this->setDummy(DUMMY_CURRENT_FPS);
	this->setDummy(DUMMY_CURRENT_FRAME);
	this->setDummy(DUMMY_RENDER_STATUS);
	this->setDummy(DUMMY_RENDER_PROGRESS);
	
	//
	// Midi
	//
	// sliders
	//this->setMidi(XXXXXXX, 2);
	//this->setMidi(XXXXXXX, 3);
	//this->setMidi(XXXXXXX, 4);
	//this->setMidi(XXXXXXX, 5);
	//this->setMidi(XXXXXXX, 12);
	//this->setMidi(XXXXXXX, 13);
	//
	// knobs
	//this->setMidi(CFG_TRACE, 14);
	//this->setMidi(XXXXXXX, 15);
	//this->setMidi(XXXXXXX, 16);
	//this->setMidi(CFG_TINT_COLOR, 17, 18, 19);
	//this->setMidi(CFG_BACKGROUND_COLOR, 20, 21, 22);
	//
	// buttons
	//this->setMidiSwitch(XXXXXXX, 34);
	
	
	//////////////////////////////////////
	//
	// SETUP GUI
	//
	this->mGui->thumbnailSize = QBCFG_THUMB_SIZE;
	//Control *c;
	FloatVarControl *cf;
	// QB Tab
	this->guiAddGroup("> QB");
	this->guiAddButton("Load Config",		(ciConfigGuiBase*)this, (bool(ciConfigGuiBase::*)(ci::app::MouseEvent)) &qbConfig::cbLoad)->setTriggerUp();
	this->guiAddButton("Save Config",		(ciConfigGuiBase*)this, (bool(ciConfigGuiBase::*)(ci::app::MouseEvent)) &qbConfig::cbSave)->setTriggerUp();
	this->guiAddParam(QBCFG_MODUL8_INPUT,			"Modul8 Input" );
	this->guiAddParam(QBCFG_SYPHON_OUTPUT,			"Syphon Output" );
	// Readonly
	this->guiAddSeparator();
	this->guiAddParam(DUMMY_RENDER_WIDTH,			"Render Width" );
	this->guiAddParam(DUMMY_RENDER_HEIGHT,			"Render Height" );
	this->guiAddParam(DUMMY_QB_WIDTH,				"Metric Width" );
	this->guiAddParam(DUMMY_QB_HEIGHT,				"Metric Height" );
	this->guiAddParam(DUMMY_QB_THROW,				"Metric Throw" );
	this->guiAddParam(DUMMY_PREVIEW_SCALE,			"Preview Scale", 2 );
	this->guiAddParam(DUMMY_MOUSE_X,				"Mouse X" );
	this->guiAddParam(DUMMY_MOUSE_Y,				"Mouse Y" );
	this->guiAddSeparator();
	this->guiAddParam(QBCFG_REALTIME_PREVIEW,		"Realtime Preview");
	this->guiAddParam(QBCFG_PREVIEW_FIT,			"Fit Preview");
	// Camera
	this->guiAddSeparator();
	this->setValueLabels(QBCFG_CAMERA_TYPE, LABELS_CAMERA_TYPE);
	//this->setFlag(QBCFG_CAMERA_TYPE, CFG_FLAG_DROP_DOWN);
	this->guiAddParam(QBCFG_CAMERA_TYPE,			"> CAMERA" );
	this->guiAddParam(QBCFG_METRIC_THROW,			"Camera Throw" );
	this->guiAddParam(QBCFG_CAMERA_GROUND,			"Camera on Ground" );
	// Animation
	this->guiAddSeparator();
	this->guiAddText("> ANIMATION");
	this->guiAddButton("Rewind",			(ciConfigGuiBase*)this, (bool(ciConfigGuiBase::*)(ci::app::MouseEvent)) &qbConfig::cbRewind);
	// play/pause panels
	buttonPlaySwitch = this->guiAddButton("Pause",	(ciConfigGuiBase*)this, (bool(ciConfigGuiBase::*)(ci::app::MouseEvent)) &qbConfig::cbPlaySwitch);
	// info
	this->guiAddParam(QBCFG_PLAY_BACKWARDS,			"Play Backwards");
	this->guiAddParam(QBCFG_RENDER_REWIND,			"Rewind Sources for Render");
	this->guiAddParam(QBCFG_FIT_SOURCES_TO_RENDER,	"Fit Sources into Render Time");
	this->guiAddParam(QBCFG_RENDER_SECONDS,			"Animation Seconds");
	cf = (FloatVarControl*) this->guiAddParam(QBCFG_CURRENT_TIME,	"Animation Time", 2 );
	cf->setReadOnly();
	cf->setFormatAsTime();
	//this->guiAddParam(QBCFG_CURRENT_TIME,		"Current Time", 2 );
	//this->guiAddParam(DUMMY_CURRENT_FRAME,		"Current Frame" );
	//
	// RENDER Tab
	this->guiAddSeparator();
	this->guiAddText("> RENDER");
	this->guiAddButton("Save Screenshot",	(ciConfigGuiBase*)this, (bool(ciConfigGuiBase::*)(ci::app::MouseEvent)) &qbConfig::cbScreenshot);
	this->guiAddParam(QBCFG_TARGET_FRAMERATE,		"Render Framerate");
	// play/pause panels
	buttonRenderSwitch = this->guiAddButton("Start Render",	(ciConfigGuiBase*)this, (bool(ciConfigGuiBase::*)(ci::app::MouseEvent)) &qbConfig::cbRenderSwitch);
	this->guiAddButton("Finish Render", (ciConfigGuiBase*)this, (bool(ciConfigGuiBase::*)(ci::app::MouseEvent)) &qbConfig::cbRenderFinish);
	// Render texture
	this->clearRenderTexture( Color::red() );
	mTextureControl = (TextureVarControl*) this->mGui->addParam("", &mNullTexture);
	mTextureControl->refreshRate = 0;
	this->guiAddParam(DUMMY_RENDER_STATUS,			"");
	this->guiAddParam(DUMMY_RENDER_PROGRESS,		"");
	this->guiAddSeparator();
	this->guiAddParam(DUMMY_CURRENT_FPS,			"Current FPS", 1 );
	
	// Read last config
	std::stringstream os;
	os << QB_APP_NAME << "_qb.cfg";
	this->useFile( os.str().c_str() );
	this->makePresets();
}

//
// Refresh GUI from 
void qbConfig::update() {
	// Call Suuuper
	this->ciConfig::update();

	// buttons
	buttonPlaySwitch->name = ( _qb.isPlaying() ? "Pause" : "Play" );
	buttonRenderSwitch->name = ( _qb.isRendering() ? "Stop Render" : "Start Render" );

	// Render Labels
	this->set( DUMMY_RENDER_STATUS, _qb.getRenderStatus() );
	this->set( DUMMY_RENDER_PROGRESS, _qb.getRenderProgress() );
}



//
// BUTTON CALLBACKS
//

// Save/Load
bool qbConfig::cbLoad( ci::app::MouseEvent event ) {
	/*
	std::vector<std::string> extensions;
	extensions.push_back("cfg");
	std::string path = app::getOpenFilePath( app::getAppPath()+"/..", extensions );
	if ( path.length() )
		this->readFile( path.c_str() );
	*/
	this->load();
	return true;
}
bool qbConfig::cbSave( ci::app::MouseEvent event ) {
	/*
	std::vector<std::string> extensions;
	extensions.push_back("cfg");
	std::string path = app::getSaveFilePath( app::getAppPath()+"/..", extensions );
	if ( path.length() )
		this->saveFile( path.c_str() );
	*/
	this->save();
	return false;
}

// Rewind Animation
bool qbConfig::cbRewind( ci::app::MouseEvent event ) {
	_qb.rewind();
	return true;
}

// Play/Stop
bool qbConfig::cbPlaySwitch( ci::app::MouseEvent event ) {
	_qb.playSwitch();
	return true;
}

// Render
bool qbConfig::cbRenderSwitch( ci::app::MouseEvent event ) {
	_qb.renderSwitch();
	return true;
}
bool qbConfig::cbRenderFinish( ci::app::MouseEvent event ) {
	_qb.renderStop();
	_qb.renderFinish();
	return true;
}

// Screenshot
bool qbConfig::cbScreenshot( ci::app::MouseEvent event ) {
	_qb.takeScreenshot();
	return true;
}


//
// Render Texture
void qbConfig::clearRenderTexture( const Color & c )
{
	if ( mRenderTexFbo )
	{
		mRenderTexFbo.bindFramebuffer();
		gl::clear( ColorA(c,0.2) );
		mRenderTexFbo.unbindFramebuffer();
		mTextureControl->refresh();
	}
}
void qbConfig::setRenderTexture( gl::Texture & frame )
{
	if ( ! mRenderTexFbo )
	{
		int h = (int) ( QBCFG_THUMB_SIZE.x * ( frame.getHeight() / (float) frame.getWidth() ) );
		mRenderTexFbo = gl::Fbo( QBCFG_THUMB_SIZE.x, h, true, true, false );
		CHECK_GL_ERROR;
		mRenderTexFbo.getTexture().setFlipped(true);
		mTextureControl->var = &(mRenderTexFbo.getTexture());
	}
	// Draw to FBO
	glEnable( GL_TEXTURE_2D );
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_LIGHTING );
	gl::disableAlphaBlending();
	gl::setMatricesWindow( mRenderTexFbo.getTexture().getSize() );
	gl::setViewport( mRenderTexFbo.getTexture().getBounds() );
	mRenderTexFbo.bindFramebuffer();
	gl::color( ColorA::white() );
	gl::draw( frame, mRenderTexFbo.getBounds() );
	mRenderTexFbo.unbindFramebuffer();
	mTextureControl->refresh();
}


//
// ciConfig CALLBACK
void qbConfig::postSetCallback(int id, int i)
{
	switch (id)
	{
		case QBCFG_CAMERA_TYPE:
			_qb.setCameraType( this->get(QBCFG_CAMERA_TYPE) );
			break;
		case QBCFG_CAMERA_GROUND:
			_qb.resizeCameras();
			break;
		case QBCFG_TARGET_FRAMERATE:
			_qb.setFrameRate( this->get(QBCFG_TARGET_FRAMERATE) );
			break;
		case QBCFG_METRIC_THROW:
			_qb.resizeCameras();
			break;
		case QBCFG_PREVIEW_FIT:
			_qb.resizePreview();
			break;
	}
}
