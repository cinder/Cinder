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

#include "cinder/android/video/VideoPlayer.h"
#include "cinder/android/AndroidDevLog.h"
using namespace ci::android::app;

#include <sstream>

namespace cinder { namespace android { namespace video {

jclassID	VideoPlayer::Java::ClassName			= "org/libcinder/video/VideoPlayer";
jclass 		VideoPlayer::Java::ClassObject			= nullptr;
jmethodID 	VideoPlayer::Java::createFromFilePath	= nullptr;
jmethodID 	VideoPlayer::Java::createFromUrl		= nullptr;
jmethodID 	VideoPlayer::Java::destroy				= nullptr;
jmethodID 	VideoPlayer::Java::initializeTexture	= nullptr;
jmethodID  	VideoPlayer::Java::updateTexture		= nullptr;
jmethodID 	VideoPlayer::Java::getWidth				= nullptr;
jmethodID 	VideoPlayer::Java::getHeight			= nullptr;
jmethodID 	VideoPlayer::Java::getDuration			= nullptr;
jmethodID 	VideoPlayer::Java::seekToTime			= nullptr;
jmethodID 	VideoPlayer::Java::seekToStart			= nullptr;
jmethodID 	VideoPlayer::Java::seekToEnd			= nullptr;
jmethodID 	VideoPlayer::Java::setLoop				= nullptr;
jmethodID 	VideoPlayer::Java::isNewFrameAvailable	= nullptr;
jmethodID 	VideoPlayer::Java::getVolume			= nullptr;
jmethodID 	VideoPlayer::Java::setVolume			= nullptr;
jmethodID 	VideoPlayer::Java::isPlaying			= nullptr;
jmethodID 	VideoPlayer::Java::isDone				= nullptr;
jmethodID 	VideoPlayer::Java::play					= nullptr;
jmethodID 	VideoPlayer::Java::stop					= nullptr; 
jmethodID 	VideoPlayer::Java::pause				= nullptr; 

VideoPlayer::VideoPlayer( jobject obj )
{
	if(nullptr == obj) {
		dbg_app_log("c'tor: Mark 2");
		throw std::runtime_error( "parameter 'obj' is null (" + std::string( __PRETTY_FUNCTION__ ) + ")" );		
	}

	mJavaObject = JniHelper::Get()->TrackedNewGlobalRef( obj );
	if( ! mJavaObject ) {
		throw std::runtime_error( "Error initializing mJavaObject (" + std::string( __PRETTY_FUNCTION__ ) + ")" );
	}

	int width = getWidth();
	int height = getHeight();
	ci::gl::Texture2d::Format texFmt;
	texFmt.target( GL_TEXTURE_EXTERNAL_OES );
	texFmt.minFilter( GL_LINEAR );
	texFmt.magFilter( GL_LINEAR );
	texFmt.wrap( GL_CLAMP_TO_EDGE );
	mCurrentTexture = gl::Texture2d::create( width, height, texFmt );	

	{
		std::stringstream ss;
		ss << "mCurrentTexture id: " << mCurrentTexture->getId() << std::endl;
		dbg_app_log( ss.str() );
	}

	JniHelper::Get()->CallVoidMethod( mJavaObject->getObject(), Java::initializeTexture, (jint)(mCurrentTexture->getId()) );
}

VideoPlayer::~VideoPlayer()
{
	JniHelper::Get()->TrackedDeleteGlobalRef( mJavaObject );
}

VideoPlayerRef VideoPlayer::create( const ci::Url& url )
{
	jstring jstrUrl = JniHelper::Get()->NewStringUTF( url.str() );	
	jobject obj = JniHelper::Get()->CallStaticObjectMethod( Java::ClassObject, Java::createFromUrl, jstrUrl );
	JniHelper::Get()->DeleteLocalRef( jstrUrl );

	VideoPlayerRef result = VideoPlayerRef( new VideoPlayer( obj ) );
	return result;
}

VideoPlayerRef VideoPlayer::create( const ci::fs::path& path )
{
	jstring jstrPath = JniHelper::Get()->NewStringUTF( path.string() );	
	jobject obj = JniHelper::Get()->CallStaticObjectMethod( Java::ClassObject, Java::createFromFilePath, jstrPath );
	JniHelper::Get()->DeleteLocalRef( jstrPath );

	VideoPlayerRef result = std::make_shared<VideoPlayer>( obj );
	JniHelper::Get()->DeleteLocalRef( obj );

	if( result ) {
		std::stringstream ss;
		ss << "Video Size: " << result->getWidth() << "x" << result->getHeight();
		dbg_app_log( ss.str() );
	}

	return result;
}

void VideoPlayer::cacheJni()
{
	if( JniHelper::Get()->AttachCurrentThread() ) {
		try {
			jclass javaClass = JniHelper::Get()->RetrieveClass( Java::ClassName );
			if( nullptr != javaClass ) {
				Java::ClassObject = reinterpret_cast<jclass>( JniHelper::Get()->NewGlobalRef( javaClass ) );
				jni_obtained_check( VideoPlayer::Java::ClassObject );
			}

			if( nullptr != Java::ClassObject ) {
				Java::createFromUrl			= JniHelper::Get()->GetStaticMethodId( Java::ClassObject, "createFromUrl", "(Ljava/lang/String;)Lorg/libcinder/video/VideoPlayer;" );
				Java::createFromFilePath	= JniHelper::Get()->GetStaticMethodId( Java::ClassObject, "createFromFilePath", "(Ljava/lang/String;)Lorg/libcinder/video/VideoPlayer;" );
				Java::destroy				= JniHelper::Get()->GetMethodId( Java::ClassObject, "destroy", "()V" );
				Java::initializeTexture		= JniHelper::Get()->GetMethodId( Java::ClassObject, "initializeTexture", "(I)V" );
				Java::updateTexture			= JniHelper::Get()->GetMethodId( Java::ClassObject, "updateTexture", "()V" );
				Java::getWidth				= JniHelper::Get()->GetMethodId( Java::ClassObject, "getWidth", "()I" );
				Java::getHeight				= JniHelper::Get()->GetMethodId( Java::ClassObject, "getHeight", "()I" );
				Java::getDuration			= JniHelper::Get()->GetMethodId( Java::ClassObject, "getDuration", "()F" );
				Java::seekToTime			= JniHelper::Get()->GetMethodId( Java::ClassObject, "seekToTime", "(F)V" );
				Java::seekToStart			= JniHelper::Get()->GetMethodId( Java::ClassObject, "seekToStart", "()V" );
				Java::seekToEnd				= JniHelper::Get()->GetMethodId( Java::ClassObject, "seekToEnd", "()V" );
				Java::setLoop				= JniHelper::Get()->GetMethodId( Java::ClassObject, "setLoop", "(Z)V" );
				Java::isNewFrameAvailable	= JniHelper::Get()->GetMethodId( Java::ClassObject, "isNewFrameAvailable", "()Z" );
				Java::getVolume				= JniHelper::Get()->GetMethodId( Java::ClassObject, "getVolume", "()F" );
				Java::setVolume				= JniHelper::Get()->GetMethodId( Java::ClassObject, "setVolume", "(F)V" );
				Java::isPlaying				= JniHelper::Get()->GetMethodId( Java::ClassObject, "isPlaying", "()Z" );
				Java::isDone				= JniHelper::Get()->GetMethodId( Java::ClassObject, "isDone", "()Z" );
				Java::play					= JniHelper::Get()->GetMethodId( Java::ClassObject, "play", "()V" );
				Java::stop					= JniHelper::Get()->GetMethodId( Java::ClassObject, "stop", "()V" ); 
				Java::pause					= JniHelper::Get()->GetMethodId( Java::ClassObject, "pause", "()V" ); 
				jni_obtained_check( VideoPlayer::Java::createFromUrl );
				jni_obtained_check( VideoPlayer::Java::createFromFilePath );
				jni_obtained_check( VideoPlayer::Java::destroy );
				jni_obtained_check( VideoPlayer::Java::initializeTexture );
				jni_obtained_check( VideoPlayer::Java::updateTexture );
				jni_obtained_check( VideoPlayer::Java::getWidth );
				jni_obtained_check( VideoPlayer::Java::getHeight );
				jni_obtained_check( VideoPlayer::Java::getDuration );
				jni_obtained_check( VideoPlayer::Java::seekToTime );
				jni_obtained_check( VideoPlayer::Java::seekToStart );
				jni_obtained_check( VideoPlayer::Java::seekToEnd );
				jni_obtained_check( VideoPlayer::Java::setLoop );
				jni_obtained_check( VideoPlayer::Java::isNewFrameAvailable );
				jni_obtained_check( VideoPlayer::Java::getVolume );
				jni_obtained_check( VideoPlayer::Java::setVolume );
				jni_obtained_check( VideoPlayer::Java::isPlaying );
				jni_obtained_check( VideoPlayer::Java::isDone );
				jni_obtained_check( VideoPlayer::Java::play );
				jni_obtained_check( VideoPlayer::Java::stop );
				jni_obtained_check( VideoPlayer::Java::pause );
			}
		}
		catch( const std::exception& e ) {
			dbg_app_error( std::string( "VideoPlayer::cacheJni error: " ) + e.what() );
		}
	}
}

void VideoPlayer::destroyJni()
{
	if( JniHelper::Get()->AttachCurrentThread() ) {	
		JniHelper::Get()->DeleteGlobalRef( Java::ClassObject  );
		Java::ClassObject			= nullptr;
		Java::createFromUrl			= nullptr;
		Java::createFromFilePath	= nullptr;
		Java::initializeTexture		= nullptr;
		Java::destroy				= nullptr;
		Java::getWidth				= nullptr;
		Java::getHeight				= nullptr;
		Java::getDuration			= nullptr;
		Java::seekToTime			= nullptr;
		Java::seekToStart			= nullptr;
		Java::seekToEnd				= nullptr;
		Java::setLoop				= nullptr;
		Java::isNewFrameAvailable	= nullptr;
		Java::getVolume				= nullptr;
		Java::setVolume				= nullptr;
		Java::isPlaying				= nullptr;
		Java::isDone				= nullptr;
		Java::play					= nullptr;
		Java::stop					= nullptr;
		Java::pause					= nullptr;
	}
}

int VideoPlayer::getWidth() const
{
	jint result = JniHelper::Get()->CallIntMethod( mJavaObject->getObject(), Java::getWidth );
	return (int)result;
}

int VideoPlayer::getHeight() const
{
	jint result = JniHelper::Get()->CallIntMethod( mJavaObject->getObject(), Java::getHeight );
	return (int)result;
}

float VideoPlayer::getDuration() const
{
	jfloat result = JniHelper::Get()->CallFloatMethod( mJavaObject->getObject(), Java::getDuration );
	return result;
}

void VideoPlayer::seekToTime( float seconds )
{
	JniHelper::Get()->CallVoidMethod( mJavaObject->getObject(), Java::seekToTime, (jfloat)seconds );
}

void VideoPlayer::seekToStart()
{
	JniHelper::Get()->CallVoidMethod( mJavaObject->getObject(), Java::seekToStart );
}

void VideoPlayer::seekToEnd()
{
	JniHelper::Get()->CallVoidMethod( mJavaObject->getObject(), Java::seekToEnd );
}

void VideoPlayer::setLoop( bool loop )
{
	JniHelper::Get()->CallVoidMethod( mJavaObject->getObject(), Java::setLoop, (jboolean)loop );
}

bool VideoPlayer::isNewFrameAvailable() const
{
	jboolean result = JniHelper::Get()->CallBooleanMethod( mJavaObject->getObject(), Java::isNewFrameAvailable );
	return (bool)result;
}

bool VideoPlayer::isPlaying() const
{
	jboolean result = JniHelper::Get()->CallBooleanMethod( mJavaObject->getObject(), Java::isPlaying );
	return (bool)result;
}

bool VideoPlayer::isDone() const
{
	jboolean result = JniHelper::Get()->CallBooleanMethod( mJavaObject->getObject(), Java::isDone );
	return (bool)result;
}

void VideoPlayer::play()
{
	if( ! isPlaying() ) {
		JniHelper::Get()->CallVoidMethod( mJavaObject->getObject(), Java::play );
	}
}

void VideoPlayer::stop()
{
	JniHelper::Get()->CallVoidMethod( mJavaObject->getObject(), Java::stop );
}

void VideoPlayer::pause()
{
	JniHelper::Get()->CallVoidMethod( mJavaObject->getObject(), Java::pause );
}

ci::gl::Texture2dRef VideoPlayer::getTexture() const
{
	if( isNewFrameAvailable() ) {
		JniHelper::Get()->CallVoidMethod( mJavaObject->getObject(), Java::updateTexture );
	}
	return mCurrentTexture;
}


}}} // namespace cinder::android::video
