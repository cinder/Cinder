/*
     File:       QuickTime.h
 
     Contains:   Master include for all of QuickTime on OS X
 
     Version:    QuickTime 7.3
 
     Copyright:  (c) 2007 (c) 1999-2007 by Apple Inc., all rights reserved.
 
     Bugs?:      For bug reports, consult the following page on
                 the World Wide Web:
 
                     http://developer.apple.com/bugreporter/
 
*/
#ifndef __QUICKTIME__
#define __QUICKTIME__

#ifndef __CARBON__
#include <Carbon.h>
#endif



/* QuickTime is not available to 64-bit clients */

#if !__LP64__

#ifndef __QTML__
#include <QTML.h>
#endif

#ifndef __MEDIAHANDLERS__
#include <MediaHandlers.h>
#endif

#ifndef __MOVIES__
#include <Movies.h>
#endif

#ifndef __MOVIESFORMAT__
#include <MoviesFormat.h>
#endif

#ifndef __QUICKTIMEVR__
#include <QuickTimeVR.h>
#endif

#ifndef __QUICKTIMEVRFORMAT__
#include <QuickTimeVRFormat.h>
#endif

#ifndef __IMAGECOMPRESSION__
#include <ImageCompression.h>
#endif

#ifndef __IMAGECODEC__
#include <ImageCodec.h>
#endif

#ifndef __QUICKTIMEMUSIC__
#include <QuickTimeMusic.h>
#endif

#ifndef __QUICKTIMECOMPONENTS__
#include <QuickTimeComponents.h>
#endif

#ifndef __QUICKTIMESTREAMING__
#include <QuickTimeStreaming.h>
#endif

#ifndef __QTSMOVIE__
#include <QTSMovie.h>
#endif

#ifndef __QTSTREAMINGCOMPONENTS__
#include <QTStreamingComponents.h>
#endif

#ifndef __QUICKTIMEERRORS__
#include <QuickTimeErrors.h>
#endif

#ifndef __HIMOVIEVIEW__
#include <HIMovieView.h>
#endif



#endif // !__LP64__


#endif /* __QUICKTIME__ */

