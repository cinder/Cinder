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
#include "cinder/Signals.h"

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

//! Interface for adding params to your window.  Wraps AntTweakBar.
class InterfaceGl {
  public:
	//! Creates and returns an InterfaceGl referenced by \a title and with \a size dimensions. Optionally takes \a color.
	static InterfaceGlRef create( const std::string &title, const ivec2 &size, const ColorA &color = ColorA( 0.3f, 0.3f, 0.3f, 0.4f ) );
	//! Creates and returns an InterfaceGl referenced by \a title that belongs to \a window, and with \a size dimensions. Optionally takes \a color.
	static InterfaceGlRef create( const cinder::app::WindowRef &window, const std::string &title, const ivec2 &size, const ColorA &color = ColorA( 0.3f, 0.3f, 0.3f, 0.4f ) );

	//! \deprecated { use create() }
	InterfaceGl() {}
	//! \deprecated { use create() }
	InterfaceGl( const std::string &title, const ivec2 &size, const ColorA &color = ColorA( 0.3f, 0.3f, 0.3f, 0.4f ) );
	//! \deprecated { use create() }
	InterfaceGl( const cinder::app::WindowRef &window, const std::string &title, const ivec2 &size, const ColorA &color = ColorA( 0.3f, 0.3f, 0.3f, 0.4f ) );

	//! Base class for chainable options. \see Options<T>.
	class OptionsBase {
	  public:
		const std::string&	getName() const				{ return mName; }
		void*				getVoidPtr() const			{ return mVoidPtr; }
		const std::string&	getKeyIncr() const			{ return mKeyIncr; }
		const std::string&	getKeyDecr() const			{ return mKeyDecr; }

		void				setVisible( bool visible = true );

	  protected:
		OptionsBase( const std::string &name, void *targetVoidPtr, InterfaceGl *parent );

		void setMin( float minVal );
		void setMax( float maxVal );
		void setStep( float maxVal );
		void setPrecision( int precVal );
		void setKeyIncr( const std::string &keyIncr );
		void setKeyDecr( const std::string &keyDecr );
		void setKey( const std::string &key );
		void setLabel( const std::string &label );
		void setGroup( const std::string &group );
		void setOptionsStr( const std::string &optionsStr );

		void reAddOptions();

		std::string mName, mKeyIncr, mKeyDecr, mKey, mLabel, mGroup, mOptionsStr;
		void*		mVoidPtr;
		float		mMin, mMax, mStep;
		int			mPrecision;
		bool		mMinSet, mMaxSet, mStepSet, mPrecisionSet;

		InterfaceGl*	mParent;

		friend class InterfaceGl;
	};

	//! Provides chainable options, returned from addParam().
	template <typename T>
	class Options : public OptionsBase {
  	  public:
		Options( const std::string &name, T *target, int type, InterfaceGl *parent );

		typedef std::function<void ( T )>	SetterFn;
		typedef std::function<T ()>			GetterFn;
		typedef std::function<void ()>		UpdateFn;

		//! Sets the maximum value for the associated target param.
		Options&	min( float minVal )							{ setMin( minVal ); return *this; }
		//! Sets the minimum value for the associated target param.
		Options&	max( float maxVal )							{ setMax( maxVal ); return *this; }
		//! Sets step increment for the associated target param.
		Options&	step( float stepVal )						{ setStep( stepVal ); return *this; }
		//! Sets the number of significant digits for floating point variables (float or double type only).
		Options&	precision( int precVal )					{ setPrecision( precVal ); return *this; }
		//! Sets an increment shortcut key
		Options&	keyIncr( const std::string &keyIncr )		{ setKeyIncr( keyIncr ); return *this; }
		//! Sets a decrement shortcut key
		Options&	keyDecr( const std::string &keyDecr )		{ setKeyDecr( keyDecr ); return *this; }
		//! Sets a shortcut key for param types that cannot be incremented / decremented (ex. bool)
		Options&	key( const std::string &key )				{ setKey( key ); return *this; }
		//! Sets the param label. A parameter name must be unique, but you can override it with a 'label', which does not have to be unique.
		Options&	label( const std::string &label )			{ setLabel( label ); return *this; }
		//! Sets the param group
		Options&	group( const std::string &group )			{ setGroup( group ); return *this; }
		//! Sets other implementation defined options via string.
		Options&	optionsStr( const std::string &optionsStr )	{ setOptionsStr( optionsStr ); return *this; }

		//!! Sets \a setterFn and \a getterFn as callbacks for this param. the target is ignored in this case.
		Options&	accessors( const SetterFn &setterFn, const GetterFn &getterFn );
		//!! Sets an update function that will be called after the target param is updated.
		Options&	updateFn( const UpdateFn &updateFn );

		//! Shows or hides this param.
		Options&	visible( bool visible = true ) { setVisible( visible ); return *this; }

	  private:
		T*				mTarget;
		int				mTwType;

		friend class InterfaceGl;
	};

	//! Draw the interface.
	void	draw();
	//! Shows the interface. If \a visible is `false`, hides the interface.
	void	show( bool visible = true );
	//! Hides the interface
	void	hide();
	//! Returns whether the interface is visible or not. \see show(), hide().
	bool	isVisible() const;

	//! Maximizes the interface, making it visible in its default presentation mode. If \a maximized is `false`, minimizes the interface. \see minimize()
	void	maximize( bool maximized = true );
	//! Minimizes the interface face to the bottom left corner of the window.
	void	minimize();
	//! Returns whether the interface is maximized or not. \see maximize(), minimize()
	bool	isMaximized() const;
	//! Gets the position of this interface instance
	ivec2	getPosition() const;
	//! Sets the position of this interface instance
	void	setPosition( const ci::ivec2 &pos );
	//! Gets the width of this interface instance
	int		getWidth() const { return getSize().x; }
	//! Gets the height of this interface instance
	int		getHeight() const { return getSize().y; }
	//! Gets the size of this interface instance
	ivec2	getSize() const;
	//! Sets the size of this interface instance
	void	setSize( const ci::ivec2 &size );
	//! Adds \a target as a param to the interface, referring to it with \a name. \return Options<T> for chaining options to the param.
	template <typename T>
	Options<T>	addParam( const std::string &name, T *target, bool readOnly = false );

	//! Adds a param to the interface with no target, but is instead accessed with \a setterFn and \a getterFn. \return Options<T> for chaining options to the param.
	template <typename T>
	Options<T>	addParam( const std::string &name, const std::function<void ( T )> &setterFn, const std::function<T ()> &getterFn );

	//! Adds enumerated parameter. The value corresponds to the indices of \a enumNames.
	Options<int> addParam( const std::string &name, const std::vector<std::string> &enumNames, int *param, bool readOnly = false );
	//! Adds an enumerated param to the interface with no target, but is instead accessed with \a setterFn and \a getterFn. The input parameter of \a setterFn and the return value of \a getterFn correspond to the indices of \a enumNames. \return Options<T> for chaining options to the param.
	Options<int> addParam( const std::string &name, const std::vector<std::string> &enumNames, const std::function<void ( int )> &setterFn, const std::function<int ()> &getterFn );

	//! Adds a separator to the interface.
	void	addSeparator( const std::string &name = "", const std::string &optionsStr = "" );
	//! Adds text to the interface.
	void	addText( const std::string &name = "", const std::string &optionsStr = "" );
	//! Adds a button that fires \a callback when clicked.
	void	addButton( const std::string &name, const std::function<void()> &callback, const std::string &optionsStr = "" );
	//! Removes the param referred to by \a name.
	void	removeParam( const std::string &name );
	//! Sets runtime options on the param referred to by \a name.
	void	setOptions( const std::string &name = "", const std::string &optionsStr = "" );
	//! Removes all the variables, buttons and separators previously added.
	void	clear();

	//! \deprecated use addParam<T>() instead.
	void	addParam( const std::string &name, bool *boolParam, const std::string &optionsStr, bool readOnly = false );
	//! \deprecated use addParam<T>() instead.
	void	addParam( const std::string &name, bool *boolParam, const char *optionsStr, bool readOnly = false )			{ addParam( name, boolParam, std::string( optionsStr ), readOnly ); }
	//! \deprecated use addParam<T>() instead.
	void	addParam( const std::string &name, float *floatParam, const std::string &optionsStr, bool readOnly = false );
	//! \deprecated use addParam<T>() instead.
	void	addParam( const std::string &name, float *floatParam, const char *optionsStr, bool readOnly = false )		{ addParam( name, floatParam, std::string( optionsStr ), readOnly ); }
	//! \deprecated use addParam<T>() instead.
	void	addParam( const std::string &name, double *doubleParam, const std::string &optionsStr, bool readOnly = false );
	//! \deprecated use addParam<T>() instead.
	void	addParam( const std::string &name, double *doubleParam, const char *optionsStr, bool readOnly = false )		{ addParam( name, doubleParam, std::string( optionsStr ), readOnly ); }
	//! \deprecated use addParam<T>() instead.
	void	addParam( const std::string &name, int32_t *intParam, const std::string &optionsStr, bool readOnly = false );
	//! \deprecated use addParam<T>() instead.
	void	addParam( const std::string &name, int32_t *intParam, const char *optionsStr, bool readOnly = false )		{ addParam( name, intParam, std::string( optionsStr ), readOnly ); }
	//! \deprecated use addParam<T>() instead.
	void	addParam( const std::string &name, vec3 *vectorParam, const std::string &optionsStr, bool readOnly = false );
	//! \deprecated use addParam<T>() instead.
	void	addParam( const std::string &name, vec3 *vectorParam, const char *optionsStr, bool readOnly = false )		{ addParam( name, vectorParam, std::string( optionsStr ), readOnly ); }
	//! \deprecated use addParam<T>() instead.
	void	addParam( const std::string &name, quat *quatParam, const std::string &optionsStr, bool readOnly = false );
	//! \deprecated use addParam<T>() instead.
	void	addParam( const std::string &name, quat *quatParam, const char *optionsStr, bool readOnly = false )			{ addParam( name, quatParam, std::string( optionsStr ), readOnly ); }
	//! \deprecated use addParam<T>() instead.
	void	addParam( const std::string &name, Color *colorParam, const std::string &optionsStr, bool readOnly = false );
	//! \deprecated use addParam<T>() instead.
	void	addParam( const std::string &name, Color *colorParam, const char *optionsStr, bool readOnly = false )		{ addParam( name, colorParam, std::string( optionsStr ), readOnly ); }
	//! \deprecated use addParam<T>() instead.
	void	addParam( const std::string &name, ColorA *colorParam, const std::string &optionsStr, bool readOnly = false );
	//! \deprecated use addParam<T>() instead.
	void	addParam( const std::string &name, ColorA *colorParam, const char *optionsStr, bool readOnly = false )		{ addParam( name, colorParam, std::string( optionsStr ), readOnly ); }
	//! \deprecated use addParam<T>() instead.
	void	addParam( const std::string &name, std::string *strParam, const std::string &optionsStr, bool readOnly = false );
	//! \deprecated use addParam<T>() instead.
	void	addParam( const std::string &name, std::string *strParam, const char *optionsStr, bool readOnly = false )	{ addParam( name, strParam, std::string( optionsStr ), readOnly ); }
	//! \deprecated use addParam() variant that returns Options<int> instead.
	void	addParam( const std::string &name, const std::vector<std::string> &enumNames, int *param, const std::string &optionsStr, bool readOnly = false );
	//! \deprecated use addParam() variant that returns Options<int> instead.
	void	addParam( const std::string &name, const std::vector<std::string> &enumNames, int *param, const char *optionsStr, bool readOnly = false )	{ addParam( name, enumNames, param, std::string( optionsStr ), readOnly ); }

  protected:
	void	init( app::WindowRef window, const std::string &title, const ivec2 &size, const ColorA color );
	void	implAddParamDeprecated( const std::string &name, void *param, int type, const std::string &optionsStr, bool readOnly );

	template <typename T>
	Options<T>	addParamImpl( const std::string &name, T *param, int type, bool readOnly );
	template <class T>
	void addParamCallbackImpl( const std::function<void (T)> &setter, const std::function<T ()> &getter, const Options<T> &options );

	std::weak_ptr<app::Window>		mWindow;
	std::shared_ptr<TwBar>			mBar;
	int								mTwWindowId;
	
	std::map<std::string, std::shared_ptr<void> >	mStoredCallbacks; // key = name, value = memory managed pointer
};

template <typename T>
InterfaceGl::Options<T>	InterfaceGl::addParam( const std::string &name, const std::function<void ( T )> &setterFn, const std::function<T ()> &getterFn )
{
	return addParam<T>( name, nullptr ).accessors( setterFn, getterFn );
}

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
	assert( target );

	std::function<void( T )> setter =	[target, updateFn]( T var )	{ *target = var; updateFn(); };
	std::function<T ()> getter =		[target]()					{ return *target; };

	return accessors( setter, getter );
}

} } // namespace cinder::params
