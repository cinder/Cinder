/*
 Copyright (c) 2015, The Cinder Project

 This code is intended to be used with the Cinder C++ library, http://libcinder.org

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

#pragma once

#include "cinder/android/CinderAndroid.h"
#include "cinder/android/JniHelper.h"
#include "cinder/gl/Texture.h" 
#include "cinder/Url.h" 

namespace cinder { namespace android { namespace app {

class CinderNativeActivity;

}}} // namespace cinder::android::app 

namespace cinder { namespace android { namespace video {

class VideoPlayer;
using VideoPlayerRef = std::shared_ptr<VideoPlayer>;

class VideoPlayer {
public:

	VideoPlayer( jobject obj );
	virtual ~VideoPlayer();

	static VideoPlayerRef	create( const ci::Url& url );
	static VideoPlayerRef 	create( const ci::fs::path& path );

	int 					getWidth() const;
	int 					getHeight() const;

	float 					getDuration() const;

	void					seekToTime( float seconds );
	void					seekToStart();
	void					seekToEnd();	

	void 					setLoop( bool loop );

	bool					isNewFrameAvailable() const;

	bool 					isPlaying() const;
	bool 					isDone() const;

	void 					play();
	void 					stop();
	void 					pause();

	ci::gl::Texture2dRef	getTexture() const;

private:
	struct Java {
		static jclassID		ClassName;
		static jclass 		ClassObject;
		static jmethodID 	createFromUrl;
		static jmethodID 	createFromFilePath;
		static jmethodID 	destroy;
		static jmethodID 	initializeTexture;
		static jmethodID	updateTexture;
		static jmethodID 	getWidth;
		static jmethodID 	getHeight;
		static jmethodID 	getDuration;
		static jmethodID 	seekToTime;
		static jmethodID 	seekToStart;
		static jmethodID 	seekToEnd;
		static jmethodID 	setLoop;
		static jmethodID 	isNewFrameAvailable;
		static jmethodID 	getVolume;
		static jmethodID 	setVolume;
		static jmethodID 	isPlaying;
		static jmethodID 	isDone;
		static jmethodID 	play;
		static jmethodID 	stop;
		static jmethodID 	pause;
	};

	static void 			cacheJni();
	static void 			destroyJni();

	JniGlobalObjectRef		mJavaObject;

	ci::gl::Texture2dRef	mCurrentTexture;

private:
	friend class ci::android::app::CinderNativeActivity;
};

}}} // namespace cinder::android::video
