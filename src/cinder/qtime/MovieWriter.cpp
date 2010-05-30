/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.
 
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

#if defined( CINDER_COCOA ) && ( ! defined( __OBJC__ ) )
#	error "This file must be compiled as Objective-C++ on the Mac"
#endif

#include "cinder/qtime/MovieWriter.h"

#if defined( CINDER_MAC )
	#include <QTKit/QTKit.h>
	#include "cinder/cocoa/CinderCocoa.h"
#endif


namespace cinder { namespace qtime {

MovieWriter::MovieWriter( const std::string &outPath, MovieWriterCodecType codec, MovieWriterQuality quality )
	: mObj( shared_ptr<Obj>( new Obj( outPath, codec, quality ) ) )
{
}

MovieWriter::Obj::~Obj()
{
	if( ! mFinished )
		finish();

	if( mMovie )
		[mMovie release];
}

MovieWriter::Obj::Obj( const std::string &path, MovieWriterCodecType codec, MovieWriterQuality quality )
	: mPath( path ), mQuality( quality ), mFinished( false )
{	
	switch(codec) {
		case PXLT:
			mCodec = "pxlt";
		break;
		case RAW:
			mCodec = "raw ";
		break;
		case H264:
			mCodec = "avc1";
		break;
		case MP4:
			mCodec = "mp4v";
		break;
		case H263:
			mCodec = "h263";
		break;
		case PNG:
			mCodec = "png ";
		break;			
	}
	
	NSString *tempPath = [NSString stringWithUTF8String:mPath.c_str()];	
	mMovie = [[QTMovie alloc] initToWritableFile:tempPath error:NULL];
	[mMovie setAttribute:[NSNumber numberWithBool:YES] forKey:QTMovieEditableAttribute];
	[mMovie retain];	
}
	
void MovieWriter::addFrame( const ImageSourceRef &imageSource )
{
	CGImageRef cgi = cocoa::createCgImage(imageSource);
	NSBitmapImageRep *bitmapRep = [[NSBitmapImageRep alloc] initWithCGImage:cgi];
	
	NSImage *image = [[NSImage alloc] init];
	[image addRepresentation:bitmapRep];
	[bitmapRep release];
	
	long long timeValue		= 10; // 60fps
	long timeScale			= 600;
	QTTime frameDuration    = QTMakeTime(timeValue, timeScale);
	NSString *nsCodec = [[NSString alloc] initWithUTF8String:mObj->mCodec.c_str()];
	NSNumber *nsQuality = [[NSNumber alloc] initWithLong:mObj->mQuality];
	NSDictionary *opt = [[NSDictionary alloc] initWithObjectsAndKeys:
						   nsCodec,  QTAddImageCodecType,
						   nsQuality,				 QTAddImageCodecQuality,
						   nil];
	
	[mObj->mMovie addImage:image forDuration:frameDuration withAttributes:opt];

	::CGImageRelease( cgi );
	[image release];
	[opt release];
	[nsCodec release];
	[nsQuality release];
}

void MovieWriter::Obj::finish()
{
	[mMovie updateMovieFile];
	mFinished = true;
}
	
} } // namespace cinder::qtime