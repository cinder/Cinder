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
//#include "cinder/android/JniHelper.h"
//
//namespace cinder { namespace app { 
//
//class EventManagerAndroid;
//
//}} // namespace cinder::app
//
//namespace cinder { namespace android { namespace app {
//
///** \class ComponentManager
// *
// */
//class ComponentManager {
//public:
//	
//	virtual ~ComponentManager();
//
//	static ComponentManager* 	instance();
//
//	jobject 					getCamera();
//
//private:
//	struct Java {
//		static jclassID			ClassName;
//		static jclass 			ClassObject;
//		static jmethodID		getInstance;
//		static jmethodID 		isBackCameraAvailable;
//		static jmethodID 		isFrontCameraAvailable;
//		static jmethodID 		getCamera;
//	};
//
//	static void 				cacheJni();
//	static void 				destroyJni();
//
//	/*
//	static const std::string 	sJavaClassName;
//	static jclass 				sJavaClass;
//	static jmethodID 			sJavaStaticMethodGetInstance;
//	static jmethodID 			sJavaMethodIsBackCameraAvailable;
//	static jmethodID 			sJavaMethodIsFrontCameraAvailable;
//	static jmethodID			sJavaMethodGetCamera;
//	*/
//
//	ComponentManager( jobject obj );
//
//	static std::unique_ptr<ComponentManager> sInstance;
//	JniGlobalObjectRef			mJavaObject;
//
//private:
//	static void registerComponents();
//	static void unregisterComponents();
//
//	static void deleteGlobalRefs();
//
//	friend class cinder::app::EventManagerAndroid;
//};
//
//}}} // namespace cinder::android::app
