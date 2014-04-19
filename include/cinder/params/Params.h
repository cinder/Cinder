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

#pragma once

#include "cinder/Color.h"
#include "cinder/Quaternion.h"
#include "cinder/Function.h"

#include <string>
#include <map>
#include <memory>

typedef struct CTwBar TwBar;

namespace cinder {

namespace app {
  class Window;
  typedef std::shared_ptr<Window>		WindowRef;
}

namespace params {
  
typedef std::shared_ptr<class InterfaceGl>	InterfaceGlRef;

class InterfaceGl {
  public:

	InterfaceGl() {}
	InterfaceGl( const std::string &title, const Vec2i &size, const ColorA &color = ColorA( 0.3f, 0.3f, 0.3f, 0.4f ) );
	InterfaceGl( cinder::app::WindowRef window, const std::string &title, const Vec2i &size, const ColorA &color = ColorA( 0.3f, 0.3f, 0.3f, 0.4f ) );
	
	static InterfaceGlRef create( const std::string &title, const Vec2i &size, const ColorA &color = ColorA( 0.3f, 0.3f, 0.3f, 0.4f ) );
	static InterfaceGlRef create( cinder::app::WindowRef window, const std::string &title, const Vec2i &size, const ColorA &color = ColorA( 0.3f, 0.3f, 0.3f, 0.4f ) );

	// TODO: hide if possible
	class OptionsBase {
	public:
		const std::string&	getName() const				{ return mName; }
		void*				getVoidPtr() const			{ return mVoidPtr; }
		const std::string&	getKeyIncr() const			{ return mKeyIncr; }
		const std::string&	getKeyDecr() const			{ return mKeyDecr; }

	protected:
		OptionsBase( const std::string &name, void *targetVoidPtr, InterfaceGl *parent );

		void setMin( float minVal );
		void setMax( float maxVal );
		void setStep( float maxVal );
		void setPrecision( int precVal );
		void setKeyIncr( const std::string &keyIncr );
		void setKeyDecr( const std::string &keyDecr );

		void reAddOptions();

		std::string mName, mKeyIncr, mKeyDecr;
		void*		mVoidPtr;

		float		mMin, mMax, mStep;
		int			mPrecision;
		bool		mMinSet, mMaxSet, mStepSet, mPrecisionSet;

		InterfaceGl*	mParent;

		friend class InterfaceGl;
	};
	
	// TODO: add optionsStr() for anything we don't cover
	template <typename T>
	class Options : public OptionsBase {
  	public:
		Options( const std::string &name, T *target, int type, InterfaceGl *parent );

		typedef std::function<void ( T )>	SetterFn;
		typedef std::function<T ()>			GetterFn;
		typedef std::function<void ()>		UpdateFn;

		Options&	min( float minVal )						{ setMin( minVal ); return *this; }
		Options&	max( float maxVal )						{ setMax( maxVal ); return *this; }
		Options&	step( float stepVal )					{ setStep( stepVal ); return *this; }
		Options&	precision( int precVal )				{ setPrecision( precVal ); return *this; }
		Options&	keyIncr( const std::string &keyIncr )	{ setKeyIncr( keyIncr ); return *this; }
		Options&	keyDecr( const std::string &keyDecr )	{ setKeyDecr( keyDecr ); return *this; }

		Options&	accessors( const SetterFn &setterFn, const GetterFn &getterFn );
		Options&	updateFn( const UpdateFn &updateFn );

		const std::string&	getName() const				{ return mName; }
		const std::string&	getKeyIncr() const			{ return mKeyIncr; }
		const std::string&	getKeyDecr() const			{ return mKeyDecr; }

	private:

		friend class InterfaceGl;

		T*				mTarget;
		int				mTwType;
	};

	void	draw();
	void	show( bool visible = true );
	void	hide();
	bool	isVisible() const;
	
	void	maximize( bool maximized = true );
	void	minimize();
	bool	isMaximized() const;

	template <typename T>
	Options<T>	addParam( const std::string &name, T *target = nullptr, bool readOnly = false );

	// Deprecated, will be removed in the future (use addParam<T>())
	void	addParam( const std::string &name, bool *boolParam, const std::string &optionsStr, bool readOnly = false );
	void	addParam( const std::string &name, float *floatParam, const std::string &optionsStr, bool readOnly = false );
	void	addParam( const std::string &name, double *doubleParam, const std::string &optionsStr, bool readOnly = false );
	void	addParam( const std::string &name, int32_t *intParam, const std::string &optionsStr, bool readOnly = false );
	void	addParam( const std::string &name, Vec3f *vectorParam, const std::string &optionsStr, bool readOnly = false );
	void	addParam( const std::string &name, Quatf *quatParam, const std::string &optionsStr, bool readOnly = false );
	void	addParam( const std::string &name, Color *quatParam, const std::string &optionsStr, bool readOnly = false );
	void	addParam( const std::string &name, ColorA *quatParam, const std::string &optionsStr, bool readOnly = false );
	void	addParam( const std::string &name, std::string *strParam, const std::string &optionsStr, bool readOnly = false );

	//! Adds enumerated parameter. The value corresponds to the indices of \a enumNames.
	void	addParam( const std::string &name, const std::vector<std::string> &enumNames, int *param, const std::string &optionsStr = "", bool readOnly = false );
	
	void	addSeparator( const std::string &name = "", const std::string &optionsStr = "" );
	void	addText( const std::string &name = "", const std::string &optionsStr = "" );
	void	addButton( const std::string &name, const std::function<void()> &callback, const std::string &optionsStr = "" );
	void	removeParam( const std::string &name );
	//! Removes all the variables, buttons and separators previously added.
	void	clear();
	void	setOptions( const std::string &name = "", const std::string &optionsStr = "" );

  protected:
	void	init( app::WindowRef window, const std::string &title, const Vec2i &size, const ColorA color );
	void	implAddParamDeprecated( const std::string &name, void *param, int type, const std::string &optionsStr, bool readOnly );

	template <typename T>
	Options<T>	addParamImpl( const std::string &name, T *param, int type, bool readOnly );

	template <class T>
	void addParamCallback( const std::function<void (T)> &setter, const std::function<T ()> &getter, const Options<T> &options );
	template <class T>
	void addParamCallbackImpl( const std::function<void (T)> &setter, const std::function<T ()> &getter, const Options<T> &options );

	std::weak_ptr<app::Window>		mWindow;
	std::shared_ptr<TwBar>			mBar;
	int								mTwWindowId;
	
	std::map<std::string, std::shared_ptr<void> >	mStoredCallbacks; // key = name, value = memory managed pointer
};

template <typename T>
InterfaceGl::Options<T>& InterfaceGl::Options<T>::accessors( const SetterFn &setterFn, const GetterFn &getterFn )
{
	if( mTarget )
		mParent->removeParam( getName() );

	mParent->addParamCallbackImpl( setterFn, getterFn, *this );

	reAddOptions();
	return *this;
}

template <typename T>
InterfaceGl::Options<T>& InterfaceGl::Options<T>::updateFn( const UpdateFn &updateFn )
{
	T* target = mTarget;

	std::function<void( T )> setter =	[target, updateFn]( T var )	{ *target = var; updateFn(); };
	std::function<T ()> getter =		[target]()					{ return *target; };

	return accessors( setter, getter );
}

} } // namespace cinder::params
