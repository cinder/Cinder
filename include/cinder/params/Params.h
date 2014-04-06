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
#include <list>
#include <boost/any.hpp>

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
		bool				isReadOnly() const			{ return mReadOnly; }
		const std::string&	getKeyIncr() const			{ return mKeyIncr; }
		const std::string&	getKeyDecr() const			{ return mKeyDecr; }

	protected:
		OptionsBase( const std::string &name, void *voidPtr )
			: mName( name ), mVoidPtr( voidPtr ), mParent( nullptr )
		{}

		void setMin( float minVal );
		void setMax( float maxVal );
		void setStep( float maxVal );
		void setKeyIncr( const std::string &keyIncr );
		void setKeyDecr( const std::string &keyDecr );

		std::string mName, mKeyIncr, mKeyDecr;
		void*		mVoidPtr;
		bool		mReadOnly;

		InterfaceGl*	mParent;

		friend class InterfaceGl;
	};
	
	// TODO: add optionsStr() for anything we don't cover
	template <typename T>
	class Options : public OptionsBase {
	public:
		Options( const std::string &name )
		: OptionsBase( name, nullptr ), mTarget( nullptr )
		{}

		Options( const std::string &name, T *target )
		: OptionsBase( name, target ), mTarget( target )
		{}

		typedef std::function<void ( T )>	SetterFn;
		typedef std::function<T ()>			GetterFn;
		typedef std::function<void ()>		UpdateFn;

		Options&	min( float minVal )						{ setMin( minVal ); return *this; }
		Options&	max( float maxVal )						{ setMax( maxVal ); return *this; }
		Options&	step( float stepVal )					{ setStep( stepVal ); return *this; }
		Options&	keyIncr( const std::string &keyIncr )	{ setKeyIncr( keyIncr ); return *this; }
		Options&	keyDecr( const std::string &keyDecr )	{ setKeyDecr( keyDecr ); return *this; }

		Options&	setterFn( const SetterFn &setterFunction )		{ mSetterFn = setterFunction; return *this; }
		Options&	getterFn( const GetterFn &getterFunction )		{ mGetterFn = getterFunction; return *this; }
		Options&	updateFn( const UpdateFn &updateFunction )		{ mUpdateFn = updateFunction; return *this; }

		T*		getTarget() const			{ return mTarget; }

		const SetterFn&	getSetterFn() const	{ return mSetterFn; }
		const GetterFn&	getGetterFn() const	{ return mGetterFn; }
		const UpdateFn& getUpdateFn() const	{ return mUpdateFn; }

	private:

		T*				mTarget;
		SetterFn		mSetterFn;
		GetterFn		mGetterFn;
		UpdateFn		mUpdateFn;
	};

	void	draw();

	void	show( bool visible = true );
	void	hide();
	bool	isVisible() const;
	
	void	maximize( bool maximized = true );
	void	minimize();
	bool	isMaximized() const;

	template <typename T>
	Options<T>	addParam( const std::string &name, T *target, bool readOnly = false );

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
	void	implAddParam( const std::string &name, void *param, int type, const std::string &optionsStr, bool readOnly );

	template <typename T>
	Options<T>	implAddParam( const std::string &name, T *param, int type, bool readOnly );


	template <class T>
	void implAddParamCb( const std::string &name, int type, const std::string &optionsStr, const std::function<void (T)> &setter, const std::function<T ()> &getter );

	std::weak_ptr<app::Window>		mWindow;
	std::shared_ptr<TwBar>			mBar;
	int								mTwWindowId;
	
	std::list<boost::any>			mStoredCallbacks; // TODO: use shared_ptr<void*>
};

} } // namespace cinder::params
