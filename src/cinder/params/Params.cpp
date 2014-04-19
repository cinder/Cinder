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

#include "cinder/app/App.h"
#include "cinder/params/Params.h"
#include "cinder/Utilities.h"

#include "AntTweakBar.h"

#include <boost/assign/list_of.hpp>

#if defined( USE_DIRECTX )
#include "cinder/dx/dx.h"
#include "cinder/app/AppImplMswRendererDx.h"
#endif

using namespace std;

namespace cinder { namespace params {

namespace {

#undef DELETE

#define SYNONYM(ck,ak) ((int)cinder::app::KeyEvent::KEY_ ## ck, TW_KEY_ ## ak)
#define HOMONYM(k) SYNONYM(k,k)
    std::map<int,TwKeySpecial> specialKeys = boost::assign::map_list_of
        HOMONYM(RIGHT)
        HOMONYM(LEFT)
        HOMONYM(BACKSPACE)
        HOMONYM(DELETE)
        HOMONYM(TAB)
        HOMONYM(F1)
        HOMONYM(F2)
        HOMONYM(F3)
        HOMONYM(F4)
        HOMONYM(F5)
        HOMONYM(F6)
        HOMONYM(F7)
        HOMONYM(F8)
        HOMONYM(F9)
        HOMONYM(F10)
        HOMONYM(F11)
        HOMONYM(F12)
        HOMONYM(F13)
        HOMONYM(F14)
        HOMONYM(F15)
        HOMONYM(HOME)
        HOMONYM(END)
        SYNONYM(PAGEUP,PAGE_UP)
        SYNONYM(PAGEDOWN,PAGE_DOWN)
        ;
#undef SYNONYM
#undef HOMONYM

void mouseDown( int twWindowId, app::MouseEvent &event )
{
	TwSetCurrentWindow( twWindowId );

	TwMouseButtonID button;
	if( event.isLeft() )
		button = TW_MOUSE_LEFT;
	else if( event.isRight() )
		button = TW_MOUSE_RIGHT;
	else
		button = TW_MOUSE_MIDDLE;
	event.setHandled( TwMouseButton( TW_MOUSE_PRESSED, button ) != 0 );
}

void mouseUp( int twWindowId, app::MouseEvent &event )
{
	TwSetCurrentWindow( twWindowId );

	TwMouseButtonID button;
	if( event.isLeft() )
		button = TW_MOUSE_LEFT;
	else if( event.isRight() )
		button = TW_MOUSE_RIGHT;
	else
		button = TW_MOUSE_MIDDLE;
	event.setHandled( TwMouseButton( TW_MOUSE_RELEASED, button ) != 0 );
}

void mouseWheel( int twWindowId, app::MouseEvent &event )
{
	TwSetCurrentWindow( twWindowId );

	static float sWheelPos = 0;
	sWheelPos += event.getWheelIncrement();
	event.setHandled( TwMouseWheel( (int)(sWheelPos) ) != 0 );
}

void mouseMove( weak_ptr<app::Window> winWeak, int twWindowId, app::MouseEvent &event )
{
	TwSetCurrentWindow( twWindowId );

	auto win = winWeak.lock();
	if( win ) {
		event.setHandled( TwMouseMotion( win->toPixels( event.getX() ), win->toPixels( event.getY() ) ) != 0 );
	}
}

void keyDown( int twWindowId, app::KeyEvent &event )
{
	TwSetCurrentWindow( twWindowId );

	int kmod = 0;
	if( event.isShiftDown() )
		kmod |= TW_KMOD_SHIFT;
	if( event.isControlDown() )
		kmod |= TW_KMOD_CTRL;
	if( event.isAltDown() )
		kmod |= TW_KMOD_ALT;
	event.setHandled( TwKeyPressed(
            (specialKeys.count( event.getCode() ) > 0)
                ? specialKeys[event.getCode()]
                : event.getChar(),
            kmod ) != 0 );
}

void resize( weak_ptr<app::Window> winWeak, int twWindowId )
{
	TwSetCurrentWindow( twWindowId );

	auto win = winWeak.lock();
	if( win )
		TwWindowSize( win->toPixels( win->getWidth() ), win->toPixels( win->getHeight() ) );
}

void TW_CALL implStdStringToClient( std::string& destinationClientString, const std::string& sourceLibraryString )
{
  // copy strings from the library to the client app
  destinationClientString = sourceLibraryString;
}

class AntMgr {
  public:
	AntMgr( int fontScale ) {
		// we have to do a fontscale set *before* TwInit:
		if( fontScale > 1 )
			TwDefine( (string(" GLOBAL fontscaling= ") + toString( fontScale )).c_str() );
#if defined( USE_DIRECTX )
		if( ! TwInit( TW_DIRECT3D11, dx::getDxRenderer()->md3dDevice ) )
			throw Exception();
#else
		if( ! TwInit( TW_OPENGL, NULL ) ) {
			throw Exception();
		}		
#endif
	}
	
	~AntMgr() {
		TwTerminate();
	}
};

void tweakBarDeleter( int windowId, TwBar *bar )
{
	TwSetCurrentWindow( windowId );
	TwDeleteBar( bar );
}

// callback used for addButton
void TW_CALL buttonCallback( void *clientData )
{
	std::function<void ()> *fn = reinterpret_cast<std::function<void ()>*>( clientData );
	(*fn)();
}

// struct used to start getter/setter pair
template <class T>
struct Accessors {
	Accessors( std::function<void( T )> setter, std::function<T ()> getter )
		: mSetter( setter )	, mGetter( getter )
	{}

	std::function<void( T )> mSetter;
	std::function<T ()> mGetter;
};

// callback for the getter in accessor-based params
template <class T>
void TW_CALL setterCallback( const void *value, void *clientData )
{
	Accessors<T> *twc = reinterpret_cast<Accessors<T>*>( clientData );
	twc->mSetter( *(const T *)value );
}

// callback for the setter in accessor-based params
template <class T>
void TW_CALL getterCallback( void *value, void *clientData )
{
	Accessors<T> *twc = reinterpret_cast<Accessors<T>*>( clientData );
	*(T *)value = twc->mGetter();
}

} // anonymous namespace

int initAntGl( weak_ptr<app::Window> winWeak )
{
	static std::shared_ptr<AntMgr> sMgr;
	static int sWindowId = 0;
	static std::map<app::Window *, int> sWindowIds;
	auto win = winWeak.lock();
	if( ! sMgr )
		sMgr = std::shared_ptr<AntMgr>( new AntMgr( (int)win->getContentScale() ) );
	app::Window *winPtr = win.get();
	auto it = sWindowIds.find( winPtr );
	if( it == sWindowIds.end() )
		sWindowIds[ winPtr ] = sWindowId++;
	return sWindowIds[ winPtr ];
}

InterfaceGl::InterfaceGl( const std::string &title, const Vec2i &size, const ColorA &color )
{
	init( app::App::get()->getWindow(), title, size, color );
}

InterfaceGl::InterfaceGl( app::WindowRef window, const std::string &title, const Vec2i &size, const ColorA &color )
{
	init( window, title, size, color );
}

InterfaceGlRef InterfaceGl::create( const std::string &title, const Vec2i &size, const ColorA &color )
{
	return shared_ptr<InterfaceGl>( new InterfaceGl( title, size, color ) );
}

InterfaceGlRef InterfaceGl::create( cinder::app::WindowRef window, const std::string &title, const Vec2i &size, const ColorA &color )
{
	return shared_ptr<InterfaceGl>( new InterfaceGl( window, title, size, color ) );
}

void InterfaceGl::init( app::WindowRef window, const std::string &title, const Vec2i &size, const ColorA color )
{
	mTwWindowId = initAntGl( window );
	TwSetCurrentWindow( mTwWindowId );
		
	mWindow = window;

	mBar = std::shared_ptr<TwBar>( TwNewBar( title.c_str() ), std::bind( tweakBarDeleter, mTwWindowId, std::placeholders::_1 ) );
	TwWindowSize( window->toPixels( window->getWidth() ), window->toPixels( window->getHeight() ) );
	char optionsStr[1024];
	sprintf( optionsStr, "`%s` size='%d %d' color='%d %d %d' alpha=%d", title.c_str(), size.x, size.y, (int)(color.r * 255), (int)(color.g * 255), (int)(color.b * 255), (int)(color.a * 255) );
	TwDefine( optionsStr );
	
	TwCopyStdStringToClientFunc( implStdStringToClient );	

	window->getSignalMouseDown().connect( std::bind( mouseDown, mTwWindowId, std::placeholders::_1 ) );
	window->getSignalMouseUp().connect( std::bind( mouseUp, mTwWindowId, std::placeholders::_1 ) );
	window->getSignalMouseWheel().connect( std::bind( mouseWheel, mTwWindowId, std::placeholders::_1 ) );
	window->getSignalMouseMove().connect( std::bind( mouseMove, mWindow, mTwWindowId, std::placeholders::_1 ) );
	window->getSignalMouseDrag().connect( std::bind( mouseMove, mWindow, mTwWindowId, std::placeholders::_1 ) );
	window->getSignalKeyDown().connect( std::bind( keyDown, mTwWindowId, std::placeholders::_1 ) );
	window->getSignalResize().connect( std::bind( resize, mWindow, mTwWindowId ) );
}

void InterfaceGl::draw()
{
	TwSetCurrentWindow( mTwWindowId );
	
	TwDraw();
}

void InterfaceGl::show( bool visible )
{
	TwSetCurrentWindow( mTwWindowId );
	
	int32_t visibleInt = ( visible ) ? 1 : 0;
	TwSetParam( mBar.get(), NULL, "visible", TW_PARAM_INT32, 1, &visibleInt );
}

void InterfaceGl::hide()
{
	show( false );
}

bool InterfaceGl::isVisible() const
{
	TwSetCurrentWindow( mTwWindowId );
	
	int32_t visibleInt;
	TwGetParam( mBar.get(), NULL, "visible", TW_PARAM_INT32, 1, &visibleInt );
	return visibleInt != 0;
}
	
void InterfaceGl::maximize( bool maximized )
{
	TwSetCurrentWindow( mTwWindowId );
	
	int32_t maximizedInt = ( maximized ) ? 0 : 1;
	TwSetParam( mBar.get(), NULL, "iconified", TW_PARAM_INT32, 1, &maximizedInt );
}

void InterfaceGl::minimize()
{
	maximize( false );
}

bool InterfaceGl::isMaximized() const
{
	TwSetCurrentWindow( mTwWindowId );
	
	int32_t maximizedInt;
	TwGetParam( mBar.get(), NULL, "iconified", TW_PARAM_INT32, 1, &maximizedInt );
	return maximizedInt == 0;
}

void InterfaceGl::OptionsBase::setMin( float minVal )
{
	assert( mParent );

	string optionsStr = "min=" + to_string( minVal );
	mParent->setOptions( getName(), optionsStr );
}

void InterfaceGl::OptionsBase::setMax( float maxVal )
{
	assert( mParent );

	string optionsStr = "max=" + to_string( maxVal );
	mParent->setOptions( getName(), optionsStr );
}

void InterfaceGl::OptionsBase::setStep( float stepVal )
{
	assert( mParent );

	string optionsStr = "step=" + to_string( stepVal );
	mParent->setOptions( getName(), optionsStr );
}

void InterfaceGl::OptionsBase::setPrecision( int precVal )
{
	assert( mParent );
	
	string optionsStr = "precision=" + to_string( precVal );
	mParent->setOptions( getName(), optionsStr );
}

void InterfaceGl::OptionsBase::setKeyIncr( const string &keyIncr )
{
	assert( mParent );

	string optionsStr = "keyIncr=" + keyIncr;
	mParent->setOptions( getName(), optionsStr );
}

void InterfaceGl::OptionsBase::setKeyDecr( const string &keyDecr )
{
	assert( mParent );

	string optionsStr = "keyDecr=" + keyDecr;
	mParent->setOptions( getName(), optionsStr );
}

//template <typename T>
//Param<T> InterfaceGl::implAddParam( const Param<T> &param, int type )
//{
//	string optionsStr = getParamOptions( param );
//
//	if( param.getUpdateFn() ) {
//		assert( param.getTarget() && ! param.getSetterFn() && ! param.getGetterFn() );
//
//		T* target = param.getTarget();
//		auto updateFn = param.getUpdateFn();
//		std::function<void( T )> setter =	[target, updateFn]( T var )	{ *target = var; updateFn(); };
//		std::function<T ()> getter =		[target]()					{ return *target; };
//
//		implAddParamCb<T>( param.getName(), type, optionsStr, setter, getter );
//	}
//	else if( param.getSetterFn() && param.getGetterFn() ) {
//		assert( 0 && "cannot have a target and accessors" );
//		implAddParamCb<T>( param.getName(), type, optionsStr, param.getSetterFn(), param.getGetterFn() );
//	}
//	else {
//		assert( param.getTarget() && "must provide a target or accessors" );
//
//		implAddParam( param.getName(), param.getVoidPtr(), type, optionsStr, param.isReadOnly() );
//	}
//}

void InterfaceGl::implAddParamDeprecated( const std::string &name, void *param, int type, const std::string &optionsStr, bool readOnly )
{
	TwSetCurrentWindow( mTwWindowId );
		
	if( readOnly )
		TwAddVarRO( mBar.get(), name.c_str(), (TwType)type, param, optionsStr.c_str() );
	else
		TwAddVarRW( mBar.get(), name.c_str(), (TwType)type, param, optionsStr.c_str() );
}

void InterfaceGl::addParam( const std::string &name, bool *param, const std::string &optionsStr, bool readOnly )
{
	implAddParamDeprecated( name, param, TW_TYPE_BOOLCPP, optionsStr, readOnly );
} 

void InterfaceGl::addParam( const std::string &name, float *param, const std::string &optionsStr, bool readOnly )
{
	implAddParamDeprecated( name, param, TW_TYPE_FLOAT, optionsStr, readOnly );
} 

void InterfaceGl::addParam( const std::string &name, double *param, const std::string &optionsStr, bool readOnly )
{
	implAddParamDeprecated( name, param, TW_TYPE_DOUBLE, optionsStr, readOnly );
} 

void InterfaceGl::addParam( const std::string &name, int32_t *param, const std::string &optionsStr, bool readOnly )
{
	implAddParamDeprecated( name, param, TW_TYPE_INT32, optionsStr, readOnly );
} 

void InterfaceGl::addParam( const std::string &name, Vec3f *param, const std::string &optionsStr, bool readOnly )
{
	implAddParamDeprecated( name, param, TW_TYPE_DIR3F, optionsStr, readOnly );
} 

void InterfaceGl::addParam( const std::string &name, Quatf *param, const std::string &optionsStr, bool readOnly )
{
	implAddParamDeprecated( name, param, TW_TYPE_QUAT4F, optionsStr, readOnly );
} 

void InterfaceGl::addParam( const std::string &name, Color *param, const std::string &optionsStr, bool readOnly )
{
	implAddParamDeprecated( name, param, TW_TYPE_COLOR3F, optionsStr, readOnly );
} 

void InterfaceGl::addParam( const std::string &name, ColorA *param, const std::string &optionsStr, bool readOnly )
{
	implAddParamDeprecated( name, param, TW_TYPE_COLOR4F, optionsStr, readOnly );
} 

void InterfaceGl::addParam( const std::string &name, std::string *param, const std::string &optionsStr, bool readOnly )
{
	implAddParamDeprecated( name, param, TW_TYPE_STDSTRING, optionsStr, readOnly );
}

void InterfaceGl::addParam( const std::string &name, const std::vector<std::string> &enumNames, int *param, const std::string &optionsStr, bool readOnly )
{
	TwSetCurrentWindow( mTwWindowId );
	
	TwEnumVal *ev = new TwEnumVal[enumNames.size()];
	for( size_t v = 0; v < enumNames.size(); ++v ) {
		ev[v].Value = v;
		ev[v].Label = const_cast<char*>( enumNames[v].c_str() );
	}

	TwType evType = TwDefineEnum( (name + "EnumType").c_str(), ev, enumNames.size() );

	if( readOnly )
		TwAddVarRO( mBar.get(), name.c_str(), evType, param, optionsStr.c_str() );
	else
		TwAddVarRW( mBar.get(), name.c_str(), evType, param, optionsStr.c_str() );
		
	delete [] ev;
}

template <typename T>
InterfaceGl::Options<T> InterfaceGl::addParamImpl( const std::string &name, T *param, int type, bool readOnly )
{
	auto options = Options<T>( name, param );
	options.mParent = this;
	options.mReadOnly = readOnly;

	TwSetCurrentWindow( mTwWindowId );

	if( readOnly )
		TwAddVarRO( mBar.get(), name.c_str(), (TwType)type, param, NULL );
	else
		TwAddVarRW( mBar.get(), name.c_str(), (TwType)type, param, NULL );

	return options;
}

template <class T>
void InterfaceGl::addParamCallbackImpl( const string &name, int type, const string &optionsStr, const function<void (T)> &setter, const function<T ()> &getter )
{
	TwSetCurrentWindow( mTwWindowId );
	
	auto callbackPtr = std::make_shared<Accessors<T>>( setter, getter );
	mStoredCallbacks.push_back( callbackPtr );
	
	TwAddVarCB( mBar.get(), name.c_str(), (TwType) type, setterCallback<T>, getterCallback<T>, (void *)callbackPtr.get(), optionsStr.c_str() );
}

void InterfaceGl::addSeparator( const std::string &name, const std::string &optionsStr )
{
	TwSetCurrentWindow( mTwWindowId );
	
	TwAddSeparator( mBar.get(), name.c_str(), optionsStr.c_str() );
}

void InterfaceGl::addText( const std::string &name, const std::string &optionsStr )
{
	TwSetCurrentWindow( mTwWindowId );
	
	TwAddButton( mBar.get(), name.c_str(), NULL, NULL, optionsStr.c_str() );
}

void InterfaceGl::addButton( const std::string &name, const std::function<void ()> &callback, const std::string &optionsStr )
{
	TwSetCurrentWindow( mTwWindowId );
	
	auto callbackPtr = std::make_shared<std::function<void ()>>( callback );
	mStoredCallbacks.push_back( callbackPtr );
	TwAddButton( mBar.get(), name.c_str(), buttonCallback, (void*)callbackPtr.get(), optionsStr.c_str() );
}

void InterfaceGl::removeParam( const std::string &name )
{
	TwSetCurrentWindow( mTwWindowId );
	
	TwRemoveVar( mBar.get(), name.c_str() );
}

void InterfaceGl::clear()
{
	TwSetCurrentWindow( mTwWindowId );
	
	TwRemoveAllVars( mBar.get() );
}

void InterfaceGl::setOptions( const std::string &name, const std::string &optionsStr )
{
	TwSetCurrentWindow( mTwWindowId );
	
	std::string target = "`" + (std::string)TwGetBarName( mBar.get() ) + "`";
	if( ! name.empty() )
		target += "/`" + name + "`";

	TwDefine( ( target + " " + optionsStr ).c_str() );
}

template <> InterfaceGl::Options<bool>		InterfaceGl::addParam( const std::string &name, bool *param, bool readOnly )		{ return addParamImpl( name, param, TW_TYPE_BOOLCPP, readOnly ); }
template <> InterfaceGl::Options<char>		InterfaceGl::addParam( const std::string &name, char *param, bool readOnly )		{ return addParamImpl( name, param, TW_TYPE_CHAR, readOnly ); }
template <> InterfaceGl::Options<int8_t>	InterfaceGl::addParam( const std::string &name, int8_t *param, bool readOnly )		{ return addParamImpl( name, param, TW_TYPE_INT8, readOnly ); }
template <> InterfaceGl::Options<uint8_t>	InterfaceGl::addParam( const std::string &name, uint8_t *param, bool readOnly )		{ return addParamImpl( name, param, TW_TYPE_UINT8, readOnly ); }
template <> InterfaceGl::Options<int16_t>	InterfaceGl::addParam( const std::string &name, int16_t *param, bool readOnly )		{ return addParamImpl( name, param, TW_TYPE_INT16, readOnly ); }
template <> InterfaceGl::Options<uint16_t>	InterfaceGl::addParam( const std::string &name, uint16_t *param, bool readOnly )	{ return addParamImpl( name, param, TW_TYPE_UINT16, readOnly ); }
template <> InterfaceGl::Options<int32_t>	InterfaceGl::addParam( const std::string &name, int32_t *param, bool readOnly )		{ return addParamImpl( name, param, TW_TYPE_INT32, readOnly ); }
template <> InterfaceGl::Options<uint32_t>	InterfaceGl::addParam( const std::string &name, uint32_t *param, bool readOnly )	{ return addParamImpl( name, param, TW_TYPE_UINT32, readOnly ); }
template <> InterfaceGl::Options<float>		InterfaceGl::addParam( const std::string &name, float *param, bool readOnly )		{ return addParamImpl( name, param, TW_TYPE_FLOAT, readOnly ); }
template <> InterfaceGl::Options<double>	InterfaceGl::addParam( const std::string &name, double *param, bool readOnly )		{ return addParamImpl( name, param, TW_TYPE_DOUBLE, readOnly ); }
template <> InterfaceGl::Options<string>	InterfaceGl::addParam( const std::string &name, string *param, bool readOnly )		{ return addParamImpl( name, param, TW_TYPE_STDSTRING, readOnly ); }
template <> InterfaceGl::Options<Color>		InterfaceGl::addParam( const std::string &name, Color *param, bool readOnly )		{ return addParamImpl( name, param, TW_TYPE_COLOR3F, readOnly ); }
template <> InterfaceGl::Options<ColorA>	InterfaceGl::addParam( const std::string &name, ColorA *param, bool readOnly )		{ return addParamImpl( name, param, TW_TYPE_COLOR4F, readOnly ); }
template <> InterfaceGl::Options<Quatf>		InterfaceGl::addParam( const std::string &name, Quatf *param, bool readOnly )		{ return addParamImpl( name, param, TW_TYPE_QUAT4F, readOnly ); }
template <> InterfaceGl::Options<Quatd>		InterfaceGl::addParam( const std::string &name, Quatd *param, bool readOnly )		{ return addParamImpl( name, param, TW_TYPE_QUAT4D, readOnly ); }
template <> InterfaceGl::Options<Vec3f>		InterfaceGl::addParam( const std::string &name, Vec3f *param, bool readOnly )		{ return addParamImpl( name, param, TW_TYPE_DIR3F, readOnly ); }
template <> InterfaceGl::Options<Vec3d>		InterfaceGl::addParam( const std::string &name, Vec3d *param, bool readOnly )		{ return addParamImpl( name, param, TW_TYPE_DIR3D, readOnly ); }

} } // namespace cinder::params
