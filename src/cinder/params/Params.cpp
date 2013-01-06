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

#include "AntTweakBar.h"

#include <boost/assign/list_of.hpp>

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

void mouseDown( app::MouseEvent &event )
{
	TwMouseButtonID button;
	if( event.isLeft() )
		button = TW_MOUSE_LEFT;
	else if( event.isRight() )
		button = TW_MOUSE_RIGHT;
	else
		button = TW_MOUSE_MIDDLE;
	event.setHandled( TwMouseButton( TW_MOUSE_PRESSED, button ) != 0 );
}

void mouseUp( app::MouseEvent &event )
{
	TwMouseButtonID button;
	if( event.isLeft() )
		button = TW_MOUSE_LEFT;
	else if( event.isRight() )
		button = TW_MOUSE_RIGHT;
	else
		button = TW_MOUSE_MIDDLE;
	event.setHandled( TwMouseButton( TW_MOUSE_RELEASED, button ) != 0 );
}

void mouseWheel( app::MouseEvent &event )
{
	static float sWheelPos = 0;
	sWheelPos += event.getWheelIncrement();
	event.setHandled( TwMouseWheel( (int)(sWheelPos) ) != 0 );
}

void mouseMove( app::MouseEvent &event )
{
	event.setHandled( TwMouseMotion( event.getX(), event.getY() ) != 0 );
}

void keyDown( app::KeyEvent &event )
{
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

void resize( cinder::app::WindowRef window )
{
	TwWindowSize( window->getWidth(), window->getHeight() );
}

void TW_CALL implStdStringToClient( std::string& destinationClientString, const std::string& sourceLibraryString )
{
  // copy strings from the library to the client app
  destinationClientString = sourceLibraryString;
}

class AntMgr {
  public:
	AntMgr() {
		if( ! TwInit( TW_OPENGL, NULL ) ) {
			throw Exception();
		}		
	}
	
	~AntMgr() {
		TwTerminate();
	}
};

} // anonymous namespace

void initAntGl()
{
	static std::shared_ptr<AntMgr> mgr;
	if( ! mgr )
		mgr = std::shared_ptr<AntMgr>( new AntMgr );
}

InterfaceGl::InterfaceGl( const std::string &title, const Vec2i &size, const ColorA color )
{
	init( app::App::get()->getWindow(), title, size, color );
}

InterfaceGl::InterfaceGl( app::WindowRef window, const std::string &title, const Vec2i &size, const ColorA color )
{
	init( window, title, size, color );
}

void InterfaceGl::init( app::WindowRef window, const std::string &title, const Vec2i &size, const ColorA color )
{
	initAntGl();
	
	mWindow = window;

	mBar = std::shared_ptr<TwBar>( TwNewBar( title.c_str() ), TwDeleteBar );
	char optionsStr[1024];
	sprintf( optionsStr, "`%s` size='%d %d' color='%d %d %d' alpha=%d", title.c_str(), size.x, size.y, (int)(color.r * 255), (int)(color.g * 255), (int)(color.b * 255), (int)(color.a * 255) );
	TwDefine( optionsStr );
	
	TwCopyStdStringToClientFunc( implStdStringToClient );	

	mWindow->getSignalMouseDown().connect( mouseDown );
	mWindow->getSignalMouseUp().connect( mouseUp );
	mWindow->getSignalMouseWheel().connect( mouseWheel );
	mWindow->getSignalMouseMove().connect( mouseMove );
	mWindow->getSignalMouseDrag().connect( mouseMove );
	mWindow->getSignalKeyDown().connect( keyDown );
	mWindow->getSignalResize().connect( std::bind( resize, mWindow ) );
}

void InterfaceGl::draw()
{
	TwDraw();
}

void InterfaceGl::show( bool visible )
{
	int32_t visibleInt = ( visible ) ? 1 : 0;
	TwSetParam( mBar.get(), NULL, "visible", TW_PARAM_INT32, 1, &visibleInt );
}

void InterfaceGl::hide()
{
	int32_t visibleInt = 0;
	TwSetParam( mBar.get(), NULL, "visible", TW_PARAM_INT32, 1, &visibleInt );
}

bool InterfaceGl::isVisible() const
{
	int32_t visibleInt;
	TwGetParam( mBar.get(), NULL, "visible", TW_PARAM_INT32, 1, &visibleInt );
	return visibleInt != 0;
}

void InterfaceGl::implAddParam( const std::string &name, void *param, int type, const std::string &optionsStr, bool readOnly )
{
	if( readOnly )
		TwAddVarRO( mBar.get(), name.c_str(), (TwType)type, param, optionsStr.c_str() );
	else
		TwAddVarRW( mBar.get(), name.c_str(), (TwType)type, param, optionsStr.c_str() );
}

void InterfaceGl::addParam( const std::string &name, bool *param, const std::string &optionsStr, bool readOnly )
{
	implAddParam( name, param, TW_TYPE_BOOLCPP, optionsStr, readOnly );
} 

void InterfaceGl::addParam( const std::string &name, float *param, const std::string &optionsStr, bool readOnly )
{
	implAddParam( name, param, TW_TYPE_FLOAT, optionsStr, readOnly );
} 

void InterfaceGl::addParam( const std::string &name, double *param, const std::string &optionsStr, bool readOnly )
{
	implAddParam( name, param, TW_TYPE_DOUBLE, optionsStr, readOnly );
} 

void InterfaceGl::addParam( const std::string &name, int32_t *param, const std::string &optionsStr, bool readOnly )
{
	implAddParam( name, param, TW_TYPE_INT32, optionsStr, readOnly );
} 

void InterfaceGl::addParam( const std::string &name, Vec3f *param, const std::string &optionsStr, bool readOnly )
{
	implAddParam( name, param, TW_TYPE_DIR3F, optionsStr, readOnly );
} 

void InterfaceGl::addParam( const std::string &name, Quatf *param, const std::string &optionsStr, bool readOnly )
{
	implAddParam( name, param, TW_TYPE_QUAT4F, optionsStr, readOnly );
} 

void InterfaceGl::addParam( const std::string &name, Color *param, const std::string &optionsStr, bool readOnly )
{
	implAddParam( name, param, TW_TYPE_COLOR3F, optionsStr, readOnly );
} 

void InterfaceGl::addParam( const std::string &name, ColorA *param, const std::string &optionsStr, bool readOnly )
{
	implAddParam( name, param, TW_TYPE_COLOR4F, optionsStr, readOnly );
} 

void InterfaceGl::addParam( const std::string &name, std::string *param, const std::string &optionsStr, bool readOnly )
{
	implAddParam( name, param, TW_TYPE_STDSTRING, optionsStr, readOnly );
}

void InterfaceGl::addParam( const std::string &name, const std::vector<std::string> &enumNames, int *param, const std::string &optionsStr, bool readOnly )
{
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

void InterfaceGl::addSeparator( const std::string &name, const std::string &optionsStr )
{
	TwAddSeparator( mBar.get(), name.c_str(), optionsStr.c_str() );
}

void InterfaceGl::addText( const std::string &name, const std::string &optionsStr )
{
	TwAddButton( mBar.get(), name.c_str(), NULL, NULL, optionsStr.c_str() );
}

namespace { // anonymous namespace
void TW_CALL implButtonCallback( void *clientData )
{
	std::function<void ()> *fn = reinterpret_cast<std::function<void ()>*>( clientData );
	(*fn)(); 
} 
} // anonymous namespace

void InterfaceGl::addButton( const std::string &name, const std::function<void ()> &callback, const std::string &optionsStr )
{
	std::shared_ptr<std::function<void ()> > callbackPtr( new std::function<void ()>( callback ) );
	mButtonCallbacks.push_back( callbackPtr );
	TwAddButton( mBar.get(), name.c_str(), implButtonCallback, (void*)callbackPtr.get(), optionsStr.c_str() );
}

void InterfaceGl::removeParam( const std::string &name )
{
	TwRemoveVar( mBar.get(), name.c_str() );
}

void InterfaceGl::clear()
{
	TwRemoveAllVars( mBar.get() );
}

void InterfaceGl::setOptions( const std::string &name, const std::string &optionsStr )
{
	std::string target = "`" + (std::string)TwGetBarName( mBar.get() ) + "`";
	if( !( name.empty() ) )
		target += "/`" + name + "`";

	TwDefine( ( target + " " + optionsStr ).c_str() );
}

} } // namespace cinder::params
