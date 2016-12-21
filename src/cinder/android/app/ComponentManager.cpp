///*
// Copyright (c) 2015, The Cinder Project
//
// This code is intended to be used with the Cinder C++ library, http://libcinder.org
//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that
// the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this list of conditions and
//	the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
//	the following disclaimer in the documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//*/
//
//#include "cinder/android/app/ComponentManager.h"
//#include "cinder/android/hardware/Camera.h"
//#include "cinder/android/net/UrlLoader.h"
//#include "cinder/android/AndroidDevLog.h"
//
//namespace cinder { namespace android { namespace app {
//
///*
//const std::string 	ComponentManager::sJavaClassName 	= "org/libcinder/app/ComponentManager";
//jclass 				ComponentManager::sJavaClass 		= nullptr;
//jmethodID			ComponentManager::sJavaStaticMethodGetInstance		= nullptr;
//jmethodID 			ComponentManager::sJavaMethodIsBackCameraAvailable	= nullptr;
//jmethodID 			ComponentManager::sJavaMethodIsFrontCameraAvailable = nullptr;
//jmethodID			ComponentManager::sJavaMethodGetCamera		= nullptr;
//*/
//
//jclassID 	ComponentManager::Java::ClassName 				= "org/libcinder/app/ComponentManager";
//jclass 		ComponentManager::Java::ClassObject				= nullptr;
//jmethodID	ComponentManager::Java::getInstance 			= nullptr;
//jmethodID 	ComponentManager::Java::isBackCameraAvailable 	= nullptr;
//jmethodID 	ComponentManager::Java::isFrontCameraAvailable 	= nullptr;
//jmethodID 	ComponentManager::Java::getCamera 				= nullptr;
//
//std::unique_ptr<ComponentManager> ComponentManager::sInstance;
//
//ComponentManager::ComponentManager( jobject obj )
//{
//	mJavaObject = JniHelper::Get()->TrackedNewGlobalRef( obj );
//	if( ! mJavaObject ) {
//		throw std::runtime_error( "Error initializing mJavaObject (" + std::string( __PRETTY_FUNCTION__ ) + ")" );
//	}
//}
//
//ComponentManager::~ComponentManager() 
//{
//}
//
//void ComponentManager::cacheJni()
//{
//dbg_app_fn_enter( __PRETTY_FUNCTION__ );
//
//	if( JniHelper::Get()->AttachCurrentThread() ) {
//		try {
//			jclass javaClass = JniHelper::Get()->RetrieveClass( Java::ClassName );
//			if( nullptr != javaClass ) {
//				Java::ClassObject = reinterpret_cast<jclass>( JniHelper::Get()->NewGlobalRef( javaClass ) );
//				jni_obtained_check( ComponentManager::Java::ClassObject );
//			}
//
//			if( nullptr != Java::ClassObject ) {
//				Java::getInstance = JniHelper::Get()->GetStaticMethodId( Java::ClassObject, "getInstance", "()Lorg/libcinder/app/ComponentManager;" );
//				jni_obtained_check( ComponentManager::Java::getInstance );
//
//				Java::isBackCameraAvailable 	= JniHelper::Get()->GetMethodId( Java::ClassObject, "isBackCameraAvailable", "()Z" );
//				Java::isFrontCameraAvailable 	= JniHelper::Get()->GetMethodId( Java::ClassObject, "isFrontCameraAvailable", "()Z" );
//				Java::getCamera 				= JniHelper::Get()->GetMethodId( Java::ClassObject, "getCamera", "(I)Lorg/libcinder/hardware/Camera;" );;
//				jni_obtained_check( ComponentManager::Java::isBackCameraAvailable  );
//				jni_obtained_check( ComponentManager::Java::isFrontCameraAvailable );
//				jni_obtained_check( ComponentManager::Java::getCamera );
//			}
//		}
//		catch( const std::exception& e ) {
//			dbg_app_error( e.what() );
//		}
//	}
//
///*
//	if( JniHelper::Get()->AttachCurrentThread() ) {
//
//		try {
//			jclass javaClass = JniHelper::Get()->RetrieveClass( ComponentManager::sJavaClassName );
//			if( nullptr != javaClass ) {
//				ComponentManager::sJavaClass = reinterpret_cast<jclass>( JniHelper::Get()->NewGlobalRef( javaClass ) );
//
//				jni_obtained_check( ComponentManager::sJavaClass );
//			}
//
//			if( nullptr != ComponentManager::sJavaClass ) {
//				ComponentManager::sJavaStaticMethodGetInstance = JniHelper::Get()->GetStaticMethodId( ComponentManager::sJavaClass, "getInstance", "()Lorg/libcinder/app/ComponentManager;" );
//				jni_obtained_check( ComponentManager::sJavaStaticMethodGetInstance );
//
//				ComponentManager::sJavaMethodIsBackCameraAvailable	= JniHelper::Get()->GetMethodId( ComponentManager::sJavaClass, "isBackCameraAvailable", "()Z" );
//				ComponentManager::sJavaMethodIsFrontCameraAvailable = JniHelper::Get()->GetMethodId( ComponentManager::sJavaClass, "isFrontCameraAvailable", "()Z" );
//				ComponentManager::sJavaMethodGetCamera				= JniHelper::Get()->GetMethodId( ComponentManager::sJavaClass, "getCamera", "()Lorg/libcinder/hardware/Camera;" );;
//				jni_obtained_check( ComponentManager::sJavaMethodIsBackCameraAvailable );
//				jni_obtained_check( ComponentManager::sJavaMethodIsFrontCameraAvailable );
//				jni_obtained_check( ComponentManager::sJavaMethodGetCamera );
//			}
//		}
//		catch( const std::exception& e ) {
//			dbg_app_error( e.what() );
//		}
//	}
//*/
//
//dbg_app_fn_exit( __PRETTY_FUNCTION__ );	
//}
//
//void ComponentManager::destroyJni()
//{
//	if( JniHelper::Get()->AttachCurrentThread() ) {	
//		JniHelper::Get()->DeleteGlobalRef( Java::ClassObject  );
//		Java::ClassObject				= nullptr;
//		Java::getInstance  				= nullptr;
//		Java::isBackCameraAvailable 	= nullptr;
//		Java::isFrontCameraAvailable 	= nullptr;
//		Java::getCamera 				= nullptr;		
//	}
//
///*	
//	if( JniHelper::Get()->AttachCurrentThread() ) {	
//		JniHelper::Get()->DeleteGlobalRef( ComponentManager::sJavaClass  );
//		ComponentManager::sJavaClass = nullptr;
//
//		ComponentManager::sJavaMethodIsBackCameraAvailable = nullptr;
//		ComponentManager::sJavaMethodIsFrontCameraAvailable = nullptr;
//		ComponentManager::sJavaMethodGetCamera = nullptr;
//	}
//*/	
//}
//
//ComponentManager* ComponentManager::instance()
//{
//	if( ! ComponentManager::sInstance ) {
//		if( ( nullptr != Java::ClassObject ) && ( nullptr != Java::getInstance ) ) {
//			jobject jobj = JniHelper::Get()->CallStaticObjectMethod( Java::ClassObject, Java::getInstance );
//			if( nullptr != jobj ) {
//				ComponentManager::sInstance = std::unique_ptr<ComponentManager>( new ComponentManager( jobj ) );
//			}
//		}
//	}
//
//	return ComponentManager::sInstance.get();
//}
//
//
//jobject ComponentManager::getCamera()
//{
//	jobject result = nullptr;
//
//	if( ( nullptr != Java::ClassObject ) && ( nullptr != Java::getCamera ) ) {
//		jint apiLevel = __ANDROID_API__;
//		result = JniHelper::Get()->CallObjectMethod( mJavaObject->getObject(), Java::getCamera, apiLevel );
//	}
//
//	return result;
//}
//
//void ComponentManager::registerComponents()
//{
//dbg_app_fn_enter( __PRETTY_FUNCTION__ );
//
//	ComponentManager::cacheJni();
//
//	ci::android::hardware::Camera::cacheJni();
//	ci::android::net::UrlLoader::cacheJni();
//
//dbg_app_fn_exit( __PRETTY_FUNCTION__ );	
//}
//
//void ComponentManager::unregisterComponents()
//{
//dbg_app_fn_enter( __PRETTY_FUNCTION__ );
//
//	ComponentManager::destroyJni();
//
//	ci::android::hardware::Camera::destroyJni();
//	ci::android::net::UrlLoader::destroyJni();
//
//dbg_app_fn_exit( __PRETTY_FUNCTION__ );	
//}
//
//void ComponentManager::deleteGlobalRefs()
//{
//dbg_app_fn_enter( __PRETTY_FUNCTION__ );
//
//dbg_app_fn_exit( __PRETTY_FUNCTION__ );	
//}
//
//}}} // namespace cinder::android::app
