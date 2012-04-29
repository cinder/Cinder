//
//  qb.h
//
//  Created by Roger Sodre on 01/07/2011
//  Copyright 2011 Studio Avante. All rights reserved.
//
#pragma once

#include "cinder/Cinder.h"
#include "cinder/CinderMath.h"
#include "cinder/gl/gl.h"

namespace cinder { namespace qb {

///////////////////////////////////
//
// MISC DEFINES
// 
// Global access to QB Manager and QB Config
#define _qb						(qb::__qb)
#define _cfg					(*(qb::__qb.mConfig))
#define _dome					(qb::__qb.mDomeMaster)

// Render unit: The rendered scene size in pixels
#define QB_RENDER_WIDTH			(_qb.getRenderWidth())
#define QB_RENDER_HEIGHT		(_qb.getRenderHeight())
#define QB_RENDER_ASPECT		(_qb.getRenderAspect())
#define QB_RENDER_BOUNDS		(_qb.getRenderBounds())

// Render unit: The rendered scene size in pixels
#define QB_WINDOW_WIDTH			(_qb.getWindowWidth())
#define QB_WINDOW_HEIGHT		(_qb.getWindowHeight())

// Metric unit: Screen size, your actual scene measure, in any unit you like
#define QB_WIDTH				(_qb.getMetricWidth())
#define QB_HEIGHT				(_qb.getMetricHeight())
#define QB_THROW				(_qb.getMetricThrow())
#define QB_ASPECT				(_qb.getMetricAspect())
#define QB_SCALE				(_qb.getMetricScale())
#define QB_SIZE					(_qb.getMetricSize())
#define QB_BOUNDS				(_qb.getMetricBounds())
#define QB_CENTER				(_qb.getMetricCenter())

// Animation
#define QB_FRAMERATE			(_qb.getFrameRate())
#define QB_FRAME_DURATION		(1.0/_qb.getFrameRate())
#define QB_TIME					(_qb.getTime())
#define QB_ANIM_PROG			fmod(_qb.getTime()/_qb.getRenderSeconds(),1.0)
#define QB_ANIM_FRAMES			(QB_FRAMERATE*_qb.getRenderSeconds())

// misc
#define QB_VERBOSE				(_qb.bVerbose)
#define QB_APP_NAME				(_qb.getAppName())
#define QB_CAPTURE_FOLDER		"/Users/Roger/Desktop/CAPTURE/"
#define QB_FONT					(_qb.mFont)

// Flags
#define TEST_FLAG(src,f)		((src)&(f))
#define QBFAG_SURFACE			0x01
#define QBFAG_2					0x02
#define QBFAG_3					0x04
#define QBFAG_4					0x08
#define QBFAG_5					0x10
#define QBFAG_6					0x20
#define QBFAG_7					0x40
#define QBFAG_8					0x80
	
	
///////////////////////////////////
//
// MULTI PURPOSE DEFINES
// 
#ifndef TWO_PI
#define TWO_PI (M_PI*2.0f)
#endif
#ifndef HALF_PI
#define HALF_PI (M_PI/2.0f)
#endif
	

	
	///////////////////////////////////
	//
	// MISC LIB
	// 
	void	qbLog(std::string s);
	void	qbCheckGLError(const char *file, int line, const char *func);
#define CHECK_GL_ERROR qb::qbCheckGLError(__FILE__, __LINE__, __FUNCTION__)
	//
	// Text
	void	writeFPS();
	void	writeStuff(std::string text, int x, int y);
	//
	// Transformations
	Rectf	rectToFit( Rectf src, Rectf dst );
	void	transformToFit( Area src, Area dst );
	void	transformToFit( Vec2i src, Vec2i dst );
	//
	// fx
	void superFastBlur(unsigned char *pix, int w, int h, int radius);


} } // cinder::qb

#include "qbMain.h"
#include "qbCube.h"




