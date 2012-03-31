//
//	qbConfig.h
//
//  Created by Roger Sodre on 01/07/2011
//	Copyright 2011 Studio Avante. All rights reserved.
//
#pragma once

#include "ciConfigGuiSimple.h"

#define QBCFG_THUMB_SIZE			Vec2f(80,80*0.75)

//////////////////////////////
//
// Config IDs
//
enum enumQBCfg
{
	QBCFG_NONE = -1,
	// Camera
	QBCFG_CAMERA_TYPE,
	QBCFG_CAMERA_GROUND,
	QBCFG_METRIC_THROW,
	// Animation
	QBCFG_PLAYING,
	QBCFG_PLAY_BACKWARDS,
	QBCFG_REALTIME_PREVIEW,
	QBCFG_PREVIEW_DOWNSCALE,
	QBCFG_PREVIEW_UPSCALE,
	QBCFG_CURRENT_TIME,
	// Render
	QBCFG_TARGET_FRAMERATE,
	QBCFG_RENDER_SECONDS,
	QBCFG_RENDER_STILL_SECONDS,
	QBCFG_RENDER_REWIND,
	QBCFG_FIT_SOURCES_TO_RENDER,
	QBCFG_MODUL8_INPUT,
	QBCFG_SYPHON_OUTPUT,
	//
	// READ ONLY
	DUMMY_RENDER_WIDTH,
	DUMMY_RENDER_HEIGHT,
	DUMMY_QB_WIDTH,
	DUMMY_QB_HEIGHT,
	DUMMY_QB_THROW,
	DUMMY_MOUSE_X,
	DUMMY_MOUSE_Y,
	DUMMY_PREVIEW_SCALE,
	DUMMY_CURRENT_FPS,
	DUMMY_CURRENT_FRAME,
	DUMMY_RENDER_STATUS,
	DUMMY_RENDER_PROGRESS,
	DUMMY_RENDER_TIME,
	// # of params
	QBCFG_COUNT
};

//
// Camera Types
enum enum_qbCameraType {
	CAMERA_TYPE_ORTHO,
	CAMERA_TYPE_PERSP,
	CAMERA_TYPE_STEREO,
	//CAMERA_TYPE_FISHEYE,
	// count
	CAMERA_TYPE_COUNT
};
//
// Loop names
#define LABELS_CAMERA_TYPE \
"Orthographic",\
"Perspective",\
"Stereo",\
/*"Dome",*/\
NULL



//
// Config Class
class qbConfig : public ci::ciConfigGuiSimple {
public:
	qbConfig();
	~qbConfig() {}

	void	update();
	
	void	enableRenderControls( bool e=true )		{ renderPanel->enabled = e; }
	
	void	clearRenderTexture( const Color & c=Color::white() );
	void	setRenderTexture( gl::Texture & frame );

	gl::Fbo						mRenderTexFbo;
	gl::Texture					mNullTexture;

private:
	
	sgui::PanelControl*			renderPanel;
	sgui::ButtonControl*		buttonPlaySwitch;
	sgui::ButtonControl*		buttonRenderSwitch;
	sgui::TextureVarControl*	mTextureControl;

	bool	cbLoad( ci::app::MouseEvent event );
	bool	cbSave( ci::app::MouseEvent event );
	bool	cbPlaySwitch( ci::app::MouseEvent event );
	bool	cbRewind( ci::app::MouseEvent event );
	bool	cbRenderSwitch( ci::app::MouseEvent event );
	bool	cbRenderFinish( ci::app::MouseEvent event );
	bool	cbScreenshot( ci::app::MouseEvent event );

	void	postSetCallback(int id, int i);
	
};

