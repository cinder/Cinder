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

#include "cinder/app/AppBase.h"
#include "cinder/params/Params.h"
#include "cinder/Utilities.h"

#include "AntTweakBar.h"

#if defined( USE_DIRECTX )
	#include "cinder/dx/dx.h"
	#include "cinder/app/RendererImplDx.h"
#else
	#include "cinder/gl/Environment.h"
#endif
using namespace std;

namespace cinder { namespace params {

namespace {

#undef DELETE

#define SYNONYM(ck,ak) {(int)cinder::app::KeyEvent::KEY_ ## ck, TW_KEY_ ## ak}
#define HOMONYM(k) SYNONYM(k,k)
    std::map<int,TwKeySpecial> specialKeys = {
        HOMONYM(RIGHT),
        HOMONYM(LEFT),
        HOMONYM(BACKSPACE),
        HOMONYM(DELETE),
        HOMONYM(TAB),
        HOMONYM(F1),
        HOMONYM(F2),
        HOMONYM(F3),
        HOMONYM(F4),
        HOMONYM(F5),
        HOMONYM(F6),
        HOMONYM(F7),
        HOMONYM(F8),
        HOMONYM(F9),
        HOMONYM(F10),
        HOMONYM(F11),
        HOMONYM(F12),
        HOMONYM(F13),
        HOMONYM(F14),
        HOMONYM(F15),
        HOMONYM(HOME),
        HOMONYM(END),
        SYNONYM(PAGEUP,PAGE_UP),
        SYNONYM(PAGEDOWN,PAGE_DOWN)
        };
#undef SYNONYM
#undef HOMONYM

namespace {
void pushGlState()
{
	auto ctx = gl::context();
	
	ctx->pushGlslProg();
	ctx->pushViewport();
	ctx->pushScissor();
	ctx->pushLineWidth();
	ctx->pushActiveTexture();
	ctx->pushBlendFuncSeparate();
	ctx->pushCullFace();
	ctx->pushFrontFace();
	ctx->pushTextureBinding( GL_TEXTURE_2D, 0 );
	ctx->pushBoolState( GL_SCISSOR_TEST );
	ctx->pushBoolState( GL_CULL_FACE );
#if ! defined( CINDER_GL_ANGLE )
	ctx->pushBoolState( GL_LINE_SMOOTH );
#endif
	ctx->pushBoolState( GL_DEPTH_TEST );
	ctx->pushBoolState( GL_BLEND );
	ctx->pushBoolState( GL_CULL_FACE );
}

void popGlState()
{
	auto ctx = gl::context();
	
	ctx->popBoolState( GL_CULL_FACE, true );
	ctx->popBoolState( GL_BLEND, true );
	ctx->popBoolState( GL_DEPTH_TEST, true );
#if ! defined( CINDER_GL_ANGLE )
	ctx->pushBoolState( GL_LINE_SMOOTH );
#endif
	ctx->popBoolState( GL_CULL_FACE, true );
	ctx->popBoolState( GL_SCISSOR_TEST, true );
	ctx->popTextureBinding( GL_TEXTURE_2D, 0, true );
	ctx->popFrontFace( true );
	ctx->popCullFace( true );
	ctx->popBlendFuncSeparate( true );
	ctx->popActiveTexture( true );
	ctx->popLineWidth( true );
	ctx->popScissor( true );
	ctx->popViewport( true );
	ctx->popGlslProg( true );
	
	ctx->restoreInvalidatedVao();
	ctx->restoreInvalidatedBufferBinding( GL_ARRAY_BUFFER );
	ctx->restoreInvalidatedBufferBinding( GL_ELEMENT_ARRAY_BUFFER );	
}
} // anonymous namespace

void mouseDown( int twWindowId, app::MouseEvent &event )
{
	pushGlState();
	
	TwSetCurrentWindow( twWindowId );

	TwMouseButtonID button;
	if( event.isLeft() )
		button = TW_MOUSE_LEFT;
	else if( event.isRight() )
		button = TW_MOUSE_RIGHT;
	else
		button = TW_MOUSE_MIDDLE;
	event.setHandled( TwMouseButton( TW_MOUSE_PRESSED, button ) != 0 );
	
	popGlState();
}

void mouseUp( int twWindowId, app::MouseEvent &event )
{
	auto oldCtx = gl::context();
	pushGlState();
	
	TwSetCurrentWindow( twWindowId );

	TwMouseButtonID button;
	if( event.isLeft() )
		button = TW_MOUSE_LEFT;
	else if( event.isRight() )
		button = TW_MOUSE_RIGHT;
	else
		button = TW_MOUSE_MIDDLE;
	event.setHandled( TwMouseButton( TW_MOUSE_RELEASED, button ) != 0 );
	
	// The button handler may have tweaked the current GL context, in which case, let's force it back to what it was
	oldCtx->makeCurrent( true );
	
	popGlState();
}

void mouseWheel( int twWindowId, app::MouseEvent &event )
{
	pushGlState();

	TwSetCurrentWindow( twWindowId );

	static float sWheelPos = 0;
	sWheelPos += event.getWheelIncrement();
	event.setHandled( TwMouseWheel( (int)(sWheelPos) ) != 0 );

	popGlState();
}

void mouseMove( weak_ptr<app::Window> winWeak, int twWindowId, app::MouseEvent &event )
{
	pushGlState();
	
	TwSetCurrentWindow( twWindowId );

	auto win = winWeak.lock();
	if( win ) {
		event.setHandled( TwMouseMotion( win->toPixels( event.getX() ), win->toPixels( event.getY() ) ) != 0 );
	}
	
	popGlState();
}

void keyDown( int twWindowId, app::KeyEvent &event )
{
	pushGlState();

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

	popGlState();
}

void resize( weak_ptr<app::Window> winWeak, int twWindowId )
{
	pushGlState();

	TwSetCurrentWindow( twWindowId );

	auto win = winWeak.lock();
	if( win )
		TwWindowSize( win->toPixels( win->getWidth() ), win->toPixels( win->getHeight() ) );
		
	popGlState();
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
		if( ! TwInit( TW_OPENGL_CORE, NULL ) ) {
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

// specialization of getterCallback for std::string (string must be copied using TwCopyStdStringToLibrary)
template <>
void TW_CALL getterCallback<string>( void *value, void *clientData )
{
	Accessors<string> *twc = reinterpret_cast<Accessors<string>*>( clientData );
	string *destPtr = static_cast<string *>( value );
	TwCopyStdStringToLibrary( *destPtr, twc->mGetter() );
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

InterfaceGl::InterfaceGl( const std::string &title, const ivec2 &size, const ColorA &color )
{
	init( app::getWindow(), title, size, color );
}

InterfaceGl::InterfaceGl( const app::WindowRef &window, const std::string &title, const ivec2 &size, const ColorA &color )
{
	init( window, title, size, color );
}

InterfaceGlRef InterfaceGl::create( const std::string &title, const ivec2 &size, const ColorA &color )
{
	return shared_ptr<InterfaceGl>( new InterfaceGl( title, size, color ) );
}

InterfaceGlRef InterfaceGl::create( const cinder::app::WindowRef &window, const std::string &title, const ivec2 &size, const ColorA &color )
{
	return shared_ptr<InterfaceGl>( new InterfaceGl( window, title, size, color ) );
}

void InterfaceGl::init( app::WindowRef window, const std::string &title, const ivec2 &size, const ColorA color )
{
	// preserve the current context
	auto prevCtx = gl::context();
	// set the context to 'window'
	window->getRenderer()->makeCurrentContext();

	mTwWindowId = initAntGl( window );
	// due to a bug in Ant we need to restore the currently bound VAO as well as the buffer bindings
	gl::context()->restoreInvalidatedVao();
	gl::context()->restoreInvalidatedBufferBinding( GL_ARRAY_BUFFER );
	gl::context()->restoreInvalidatedBufferBinding( GL_ELEMENT_ARRAY_BUFFER );
	
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

	prevCtx->makeCurrent();
}

void InterfaceGl::draw()
{
	TwSetCurrentWindow( mTwWindowId );

	pushGlState();
	TwDraw();
	popGlState();
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

ivec2 InterfaceGl::getPosition() const
{
	ivec2 pos;
	TwGetParam( mBar.get(), NULL, "position", TW_PARAM_INT32, 2, &pos );

	return pos;
}

void InterfaceGl::setPosition( const ci::ivec2 &pos )
{
	string posStr = string( "position='" ) + to_string( pos.x ) + " " + to_string( pos.y ) + "'";
	setOptions( "", posStr );
}

ivec2 InterfaceGl::getSize() const
{
	ivec2 size;
	TwGetParam( mBar.get(), NULL, "size", TW_PARAM_INT32, 2, &size );

	return size;
}

void InterfaceGl::setSize( const ci::ivec2 &size )
{
	string sizeStr = string( "size='" ) + to_string( size.x ) + " " + to_string( size.y ) + "'";
	setOptions( "", sizeStr );
}

InterfaceGl::OptionsBase::OptionsBase( const std::string &name, void *targetVoidPtr, InterfaceGl *parent )
	: mName( name ), mVoidPtr( targetVoidPtr ), mParent( parent ), mMinSet( false ), mMaxSet( false ), mStepSet( false ), mPrecisionSet( false )
{}

template <typename T>
InterfaceGl::Options<T>::Options( const std::string &name, T *target, int type, InterfaceGl *parent )
	: OptionsBase( name, target, parent ), mTarget( target ), mTwType( type )
{
}

void InterfaceGl::OptionsBase::setMin( float minVal )
{
	assert( mParent );

	string optionsStr = "min=" + to_string( minVal );
	mParent->setOptions( getName(), optionsStr );

	mMin = minVal;
	mMinSet = true;
}

void InterfaceGl::OptionsBase::setMax( float maxVal )
{
	assert( mParent );

	string optionsStr = "max=" + to_string( maxVal );
	mParent->setOptions( getName(), optionsStr );

	mMax = maxVal;
	mMaxSet = true;
}

void InterfaceGl::OptionsBase::setStep( float stepVal )
{
	assert( mParent );

	string optionsStr = "step=" + to_string( stepVal );
	mParent->setOptions( getName(), optionsStr );

	mStep = stepVal;
	mStepSet = true;
}

void InterfaceGl::OptionsBase::setPrecision( int precVal )
{
	assert( mParent );
	
	string optionsStr = "precision=" + to_string( precVal );
	mParent->setOptions( getName(), optionsStr );

	mPrecision = precVal;
	mPrecisionSet = true;
}

void InterfaceGl::OptionsBase::setKeyIncr( const string &keyIncr )
{
	assert( mParent );

	string optionsStr = "keyIncr=" + keyIncr;
	mParent->setOptions( getName(), optionsStr );
	mKeyIncr = keyIncr;
}

void InterfaceGl::OptionsBase::setKeyDecr( const string &keyDecr )
{
	assert( mParent );

	string optionsStr = "keyDecr=" + keyDecr;
	mParent->setOptions( getName(), optionsStr );
	mKeyDecr = keyDecr;
}

void InterfaceGl::OptionsBase::setKey( const std::string &key )
{
	assert( mParent );

	string optionsStr = "key=" + key;
	mParent->setOptions( getName(), optionsStr );
	mKey = key;
}

void InterfaceGl::OptionsBase::setLabel( const std::string &label )
{
	assert( mParent );

	string optionsStr = "label=`" + label + "`";
	mParent->setOptions( getName(), optionsStr );
	mLabel = label;
}

void InterfaceGl::OptionsBase::setGroup( const std::string &group )
{
	assert( mParent );

	string optionsStr = "group=`" + group + "`";
	mParent->setOptions( getName(), optionsStr );
	mGroup = group;
}

void InterfaceGl::OptionsBase::setVisible( bool visible )
{
	assert( mParent );

	string optionsStr = "visible=" + string( visible ? "true" : "false" );
	mParent->setOptions( getName(), optionsStr );
}

void InterfaceGl::OptionsBase::setOptionsStr( const string &optionsStr )
{
	assert( mParent );
	mParent->setOptions( getName(), optionsStr );
	mOptionsStr = optionsStr;
}

void InterfaceGl::OptionsBase::reAddOptions()
{
	if( mMinSet )
		setMin( mMin );
	if( mMaxSet )
		setMax( mMax );
	if( mStepSet )
		setStep( mStep );
	if( mPrecisionSet )
		setPrecision( mPrecision );
	if( ! mKeyIncr.empty() )
		setKeyIncr( mKeyIncr );
	if( ! mKeyDecr.empty() )
		setKeyDecr( mKeyDecr );
	if( ! mLabel.empty() )
		setLabel( mLabel );
	if( ! mGroup.empty() )
		setGroup( mGroup );
	if( ! mOptionsStr.empty() )
		setOptionsStr( mOptionsStr );
}

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

void InterfaceGl::addParam( const std::string &name, vec3 *param, const std::string &optionsStr, bool readOnly )
{
	implAddParamDeprecated( name, param, TW_TYPE_DIR3F, optionsStr, readOnly );
} 

void InterfaceGl::addParam( const std::string &name, quat *param, const std::string &optionsStr, bool readOnly )
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

// deprecated enum variant
void InterfaceGl::addParam( const std::string &name, const std::vector<std::string> &enumNames, int *param, const std::string &optionsStr, bool readOnly )
{
	TwSetCurrentWindow( mTwWindowId );

	vector<TwEnumVal> ev( enumNames.size() );
	for( size_t v = 0; v < enumNames.size(); ++v ) {
		ev[v].Value = (int)v;
		ev[v].Label = const_cast<char*>( enumNames[v].c_str() );
	}

	TwType evType = TwDefineEnum( (name + "EnumType").c_str(), ev.data(), (unsigned int)ev.size() );

	if( readOnly )
		TwAddVarRO( mBar.get(), name.c_str(), evType, param, optionsStr.c_str() );
	else
		TwAddVarRW( mBar.get(), name.c_str(), evType, param, optionsStr.c_str() );
}

InterfaceGl::Options<int> InterfaceGl::addParam( const std::string &name, const std::vector<std::string> &enumNames, int *param, bool readOnly )
{
	TwSetCurrentWindow( mTwWindowId );

	vector<TwEnumVal> ev( enumNames.size() );
	for( size_t v = 0; v < enumNames.size(); ++v ) {
		ev[v].Value = (int)v;
		ev[v].Label = const_cast<char*>( enumNames[v].c_str() );
	}

	TwType evType = TwDefineEnum( (name + "EnumType").c_str(), ev.data(), (unsigned int)ev.size() );

	if( readOnly )
		TwAddVarRO( mBar.get(), name.c_str(), evType, param, nullptr );
	else
		TwAddVarRW( mBar.get(), name.c_str(), evType, param, nullptr );

	return Options<int>( name, param, evType, this );
}

InterfaceGl::Options<int> InterfaceGl::addParam( const std::string &name, const std::vector<std::string> &enumNames, const std::function<void ( int )> &setterFn, const std::function<int ()> &getterFn )
{
	TwSetCurrentWindow( mTwWindowId );

	vector<TwEnumVal> ev( enumNames.size() );
	for( size_t v = 0; v < enumNames.size(); ++v ) {
		ev[v].Value = (int)v;
		ev[v].Label = const_cast<char*>( enumNames[v].c_str() );
	}

	TwType evType = TwDefineEnum( (name + "EnumType").c_str(), ev.data(), (unsigned int)ev.size() );

	return Options<int>( name, nullptr, evType, this ).accessors( setterFn, getterFn );
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
	mStoredCallbacks.insert( make_pair( name, callbackPtr ) );

	TwAddButton( mBar.get(), name.c_str(), buttonCallback, (void*)callbackPtr.get(), optionsStr.c_str() );
}

void InterfaceGl::removeParam( const std::string &name )
{
	TwSetCurrentWindow( mTwWindowId );
	TwRemoveVar( mBar.get(), name.c_str() );

	mStoredCallbacks.erase( name );
}

void InterfaceGl::clear()
{
	TwSetCurrentWindow( mTwWindowId );
	TwRemoveAllVars( mBar.get() );

	mStoredCallbacks.clear();
}

void InterfaceGl::setOptions( const std::string &name, const std::string &optionsStr )
{
	TwSetCurrentWindow( mTwWindowId );
	
	std::string target = "`" + (std::string)TwGetBarName( mBar.get() ) + "`";
	if( ! name.empty() )
		target += "/`" + name + "`";

	TwDefine( ( target + " " + optionsStr ).c_str() );
}

template <typename T>
InterfaceGl::Options<T> InterfaceGl::addParamImpl( const std::string &name, T *target, int type, bool readOnly )
{
	auto options = Options<T>( name, target, type, this );

	if( target ) {
		TwSetCurrentWindow( mTwWindowId );

		if( readOnly )
			TwAddVarRO( mBar.get(), name.c_str(), (TwType)type, target, NULL );
		else
			TwAddVarRW( mBar.get(), name.c_str(), (TwType)type, target, NULL );
	}

	return options;
}

template <typename T>
void InterfaceGl::addParamCallbackImpl( const function<void (T)> &setter, const function<T ()> &getter, const Options<T> &options )
{
	TwSetCurrentWindow( mTwWindowId );

	const string &name = options.getName();
	int type = options.mTwType;

	auto callbackPtr = std::make_shared<Accessors<T>>( setter, getter );
	mStoredCallbacks.insert( make_pair( name, callbackPtr ) );

	TwAddVarCB( mBar.get(), name.c_str(), (TwType) type, setterCallback<T>, getterCallback<T>, (void *)callbackPtr.get(), NULL );
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
template <> InterfaceGl::Options<quat>		InterfaceGl::addParam( const std::string &name, quat *param, bool readOnly )		{ return addParamImpl( name, param, TW_TYPE_QUAT4F, readOnly ); }
template <> InterfaceGl::Options<dquat>		InterfaceGl::addParam( const std::string &name, dquat *param, bool readOnly )		{ return addParamImpl( name, param, TW_TYPE_QUAT4D, readOnly ); }
template <> InterfaceGl::Options<vec3>		InterfaceGl::addParam( const std::string &name, vec3 *param, bool readOnly )		{ return addParamImpl( name, param, TW_TYPE_DIR3F, readOnly ); }
template <> InterfaceGl::Options<dvec3>		InterfaceGl::addParam( const std::string &name, dvec3 *param, bool readOnly )		{ return addParamImpl( name, param, TW_TYPE_DIR3D, readOnly ); }

template void InterfaceGl::addParamCallbackImpl( const function<void( bool )>		&setter, const function<bool ()>		&getter, const Options<bool>		&options );
template void InterfaceGl::addParamCallbackImpl( const function<void( char )>		&setter, const function<char ()>		&getter, const Options<char>		&options );
template void InterfaceGl::addParamCallbackImpl( const function<void( int8_t )>		&setter, const function<int8_t ()>		&getter, const Options<int8_t>		&options );
template void InterfaceGl::addParamCallbackImpl( const function<void( uint8_t )>	&setter, const function<uint8_t ()>		&getter, const Options<uint8_t>		&options );
template void InterfaceGl::addParamCallbackImpl( const function<void( int16_t )>	&setter, const function<int16_t ()>		&getter, const Options<int16_t>		&options );
template void InterfaceGl::addParamCallbackImpl( const function<void( uint16_t )>	&setter, const function<uint16_t ()>	&getter, const Options<uint16_t>	&options );
template void InterfaceGl::addParamCallbackImpl( const function<void( int32_t )>	&setter, const function<int32_t ()>		&getter, const Options<int32_t>		&options );
template void InterfaceGl::addParamCallbackImpl( const function<void( uint32_t )>	&setter, const function<uint32_t ()>	&getter, const Options<uint32_t>	&options );
template void InterfaceGl::addParamCallbackImpl( const function<void( float )>		&setter, const function<float ()>		&getter, const Options<float>		&options );
template void InterfaceGl::addParamCallbackImpl( const function<void( double )>		&setter, const function<double ()>		&getter, const Options<double>		&options );
template void InterfaceGl::addParamCallbackImpl( const function<void( string )>		&setter, const function<string ()>		&getter, const Options<string>		&options );
template void InterfaceGl::addParamCallbackImpl( const function<void( Color )>		&setter, const function<Color ()>		&getter, const Options<Color>		&options );
template void InterfaceGl::addParamCallbackImpl( const function<void( ColorA )>		&setter, const function<ColorA ()>		&getter, const Options<ColorA>		&options );
template void InterfaceGl::addParamCallbackImpl( const function<void( quat )>		&setter, const function<quat ()>		&getter, const Options<quat>		&options );
template void InterfaceGl::addParamCallbackImpl( const function<void( dquat )>		&setter, const function<dquat ()>		&getter, const Options<dquat>		&options );
template void InterfaceGl::addParamCallbackImpl( const function<void( vec3 )>		&setter, const function<vec3 ()>		&getter, const Options<vec3>		&options );
template void InterfaceGl::addParamCallbackImpl( const function<void( dvec3 )>		&setter, const function<dvec3 ()>		&getter, const Options<dvec3>		&options );

} } // namespace cinder::params
