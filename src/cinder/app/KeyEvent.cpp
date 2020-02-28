/*
 Copyright (c) 2012, The Cinder Project, All rights reserved.
 Copyright (c) Microsoft Open Technologies, Inc. All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

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

#include "cinder/Cinder.h"
#include "cinder/app/KeyEvent.h"

#if defined( CINDER_LINUX_EGL_ONLY )
	#include "cinder/linux/input_redef.h"
#endif

#include <map>
using std::map;

namespace cinder {
namespace app {

#if defined( CINDER_COCOA )

static bool sTableInited = false;
map<int,int> sKeyTable;

// Much of this keyTable is courtesy of SDL's keyboard handling code	
static void initKeyTable()
{
	// keys in 0x00 format are from the book
	// keys in the range 0xF700-0xF8FF are from NSEvent.h in Application Kit
	// commented out keys have not been found for Mac OS X
		
	sKeyTable[0x33]			= KeyEvent::KEY_BACKSPACE;
	sKeyTable[0x30]			= KeyEvent::KEY_TAB;
	sKeyTable[0xF739]		= KeyEvent::KEY_CLEAR;
	sKeyTable[0x24]			= KeyEvent::KEY_RETURN;
	sKeyTable[0x71]			= KeyEvent::KEY_PAUSE;
	sKeyTable[0x35]			= KeyEvent::KEY_ESCAPE;
	sKeyTable[0x31]			= KeyEvent::KEY_SPACE;
	sKeyTable[0x27]			= KeyEvent::KEY_QUOTE;
	sKeyTable[0x2B]			= KeyEvent::KEY_COMMA;
	sKeyTable[0x1B]			= KeyEvent::KEY_MINUS;
	sKeyTable[0x2F]			= KeyEvent::KEY_PERIOD;
	sKeyTable[0x2C]			= KeyEvent::KEY_SLASH;
	
	sKeyTable[0x1D]			= KeyEvent::KEY_0;
	sKeyTable[0x12]			= KeyEvent::KEY_1;
	sKeyTable[0x13]			= KeyEvent::KEY_2;
	sKeyTable[0x14]			= KeyEvent::KEY_3;
	sKeyTable[0x15]			= KeyEvent::KEY_4;
	sKeyTable[0x17]			= KeyEvent::KEY_5;
	sKeyTable[0x16]			= KeyEvent::KEY_6;
	sKeyTable[0x1A]			= KeyEvent::KEY_7;
	sKeyTable[0x1C]			= KeyEvent::KEY_8;
	sKeyTable[0x19]			= KeyEvent::KEY_9;
	
	sKeyTable[0x29]			= KeyEvent::KEY_SEMICOLON;
	sKeyTable[0x18]			= KeyEvent::KEY_EQUALS;
	sKeyTable[0x21]			= KeyEvent::KEY_LEFTBRACKET;
	sKeyTable[0x2A]			= KeyEvent::KEY_BACKSLASH;
	//sKeyTable[VK_OEM_102]	= KeyEvent::KEY_LESS;
	sKeyTable[0x1E]			= KeyEvent::KEY_RIGHTBRACKET;
	sKeyTable[0x32]			= KeyEvent::KEY_BACKQUOTE;
	//sKeyTable[0xDF]		= KeyEvent::KEY_BACKQUOTE;
	
	sKeyTable[0x00]			= KeyEvent::KEY_a;
	sKeyTable[0x0B]			= KeyEvent::KEY_b;
	sKeyTable[0x08]			= KeyEvent::KEY_c;
	sKeyTable[0x02]			= KeyEvent::KEY_d;
	sKeyTable[0x0E]			= KeyEvent::KEY_e;
	sKeyTable[0x03]			= KeyEvent::KEY_f;
	sKeyTable[0x05]			= KeyEvent::KEY_g;
	sKeyTable[0x04]			= KeyEvent::KEY_h;
	sKeyTable[0x22]			= KeyEvent::KEY_i;
	sKeyTable[0x26]			= KeyEvent::KEY_j;
	sKeyTable[0x28]			= KeyEvent::KEY_k;
	sKeyTable[0x25]			= KeyEvent::KEY_l;
	sKeyTable[0x2E]			= KeyEvent::KEY_m;
	sKeyTable[0x2D]			= KeyEvent::KEY_n;
	sKeyTable[0x1F]			= KeyEvent::KEY_o;
	sKeyTable[0x23]			= KeyEvent::KEY_p;
	sKeyTable[0x0C]			= KeyEvent::KEY_q;
	sKeyTable[0x0F]			= KeyEvent::KEY_r;
	sKeyTable[0x01]			= KeyEvent::KEY_s;
	sKeyTable[0x11]			= KeyEvent::KEY_t;
	sKeyTable[0x20]			= KeyEvent::KEY_u;
	sKeyTable[0x09]			= KeyEvent::KEY_v;
	sKeyTable[0x0D]			= KeyEvent::KEY_w;
	sKeyTable[0x07]			= KeyEvent::KEY_x;
	sKeyTable[0x10]			= KeyEvent::KEY_y;
	sKeyTable[0x06]			= KeyEvent::KEY_z;
	sKeyTable[0x75]			= KeyEvent::KEY_DELETE;
	
	sKeyTable[0x52] = KeyEvent::KEY_KP0;
	sKeyTable[0x53] = KeyEvent::KEY_KP1;
	sKeyTable[0x54] = KeyEvent::KEY_KP2;
	sKeyTable[0x55] = KeyEvent::KEY_KP3;
	sKeyTable[0x56] = KeyEvent::KEY_KP4;
	sKeyTable[0x57] = KeyEvent::KEY_KP5;
	sKeyTable[0x58] = KeyEvent::KEY_KP6;
	sKeyTable[0x59] = KeyEvent::KEY_KP7;
	sKeyTable[0x5B] = KeyEvent::KEY_KP8;
	sKeyTable[0x5C] = KeyEvent::KEY_KP9;
	sKeyTable[0x41] = KeyEvent::KEY_KP_PERIOD;
	sKeyTable[0x4B] = KeyEvent::KEY_KP_DIVIDE;
	sKeyTable[0x43] = KeyEvent::KEY_KP_MULTIPLY;
	
	sKeyTable[0x1B] = KeyEvent::KEY_KP_MINUS;
	sKeyTable[0x45] = KeyEvent::KEY_KP_PLUS;
	
	sKeyTable[0x7E]	= KeyEvent::KEY_UP;
	sKeyTable[0x7D]	= KeyEvent::KEY_DOWN;
	sKeyTable[0x7C]	= KeyEvent::KEY_RIGHT;
	sKeyTable[0x7B]	= KeyEvent::KEY_LEFT;
	sKeyTable[0xF727] = KeyEvent::KEY_INSERT;
	sKeyTable[0x73]	= KeyEvent::KEY_HOME;
	sKeyTable[0x77]	= KeyEvent::KEY_END;
	sKeyTable[0x74]	= KeyEvent::KEY_PAGEUP;
	sKeyTable[0x79]	= KeyEvent::KEY_PAGEDOWN;
	
	sKeyTable[0x7A] = KeyEvent::KEY_F1;
	sKeyTable[0x78] = KeyEvent::KEY_F2;
	sKeyTable[0x63] = KeyEvent::KEY_F3;
	sKeyTable[0x76] = KeyEvent::KEY_F4;
	sKeyTable[0x60] = KeyEvent::KEY_F5;
	sKeyTable[0x61] = KeyEvent::KEY_F6;
	sKeyTable[0x62] = KeyEvent::KEY_F7;
	sKeyTable[0x64] = KeyEvent::KEY_F8;
	sKeyTable[0x65] = KeyEvent::KEY_F9;
	sKeyTable[0x6D] = KeyEvent::KEY_F10;
	sKeyTable[0x67] = KeyEvent::KEY_F11;
	sKeyTable[0x6F] = KeyEvent::KEY_F12;
	sKeyTable[0x69] = KeyEvent::KEY_F13;
	sKeyTable[0x6B] = KeyEvent::KEY_F14;
	sKeyTable[0x71] = KeyEvent::KEY_F15;
	
	// sKeyTable[NSFNumLock] = KeyEvent::KEY_NUMLOCK;
	sKeyTable[0x39] = KeyEvent::KEY_CAPSLOCK;
	sKeyTable[0x6B] = KeyEvent::KEY_SCROLLOCK;
	sKeyTable[0x3C] = KeyEvent::KEY_RSHIFT;
	sKeyTable[0x38] = KeyEvent::KEY_LSHIFT;
	sKeyTable[0x3E] = KeyEvent::KEY_RCTRL;
	sKeyTable[0x3B] = KeyEvent::KEY_LCTRL;
	sKeyTable[0x3D] = KeyEvent::KEY_RALT;
	sKeyTable[0x3A] = KeyEvent::KEY_LALT;
	sKeyTable[0x37] = KeyEvent::KEY_LMETA;
	sKeyTable[0x36] = KeyEvent::KEY_RMETA;
	// sKeyTable[VK_RWIN] = KeyEvent::KEY_RSUPER;
	// sKeyTable[VK_LWIN] = KeyEvent::KEY_LSUPER;
	
	sKeyTable[0x72]		= KeyEvent::KEY_HELP;
	sKeyTable[0xF738]	= KeyEvent::KEY_PRINT;
	//sKeyTable[VK_SNAPSHOT]		= KeyEvent::KEY_PRINT;
	sKeyTable[0xF732]	= KeyEvent::KEY_BREAK;
	sKeyTable[0xF735]	= KeyEvent::KEY_MENU;
	
	// additions not in WIN version
	sKeyTable[0xF730]	= KeyEvent::KEY_PAUSE;
	sKeyTable[0x7F]		= KeyEvent::KEY_POWER;
	
	sTableInited = true;
}

#if ! defined( CINDER_COCOA_TOUCH )
int	KeyEvent::translateNativeKeyCode( int nativeKeyCode )
{
	if( ! sTableInited )
		initKeyTable();
	
	map<int,int>::iterator iter = sKeyTable.find( nativeKeyCode );
	if( iter != sKeyTable.end() )
		return iter->second;
	else
		return KeyEvent::KEY_UNKNOWN;
}
#endif

#elif defined( CINDER_MSW_DESKTOP )

#include <windows.h>
	
static bool sTableInited = false;
static const int MAX_KEYCODE = 256;
int sKeyTable[MAX_KEYCODE];

// Much of this keyTable is courtesy of SDL's keyboard handling code
static void initKeyTable()
{
	for( int c = 0; c < MAX_KEYCODE; ++c )
		sKeyTable[c] = KeyEvent::KEY_UNKNOWN;

	sKeyTable[VK_BACK] = KeyEvent::KEY_BACKSPACE;
	sKeyTable[VK_TAB] = KeyEvent::KEY_TAB;
	sKeyTable[VK_CLEAR] = KeyEvent::KEY_CLEAR;
	sKeyTable[VK_RETURN] = KeyEvent::KEY_RETURN;
	sKeyTable[VK_PAUSE] = KeyEvent::KEY_PAUSE;
	sKeyTable[VK_ESCAPE] = KeyEvent::KEY_ESCAPE;
	sKeyTable[VK_SPACE] = KeyEvent::KEY_SPACE;
	sKeyTable[0xDE] = KeyEvent::KEY_QUOTE;
	sKeyTable[0xBC] = KeyEvent::KEY_COMMA;
	sKeyTable[0xBD] = KeyEvent::KEY_MINUS;
	sKeyTable[0xBE] = KeyEvent::KEY_PERIOD;
	sKeyTable[0xBF] = KeyEvent::KEY_SLASH;
	sKeyTable['0'] = KeyEvent::KEY_0;
	sKeyTable['1'] = KeyEvent::KEY_1;
	sKeyTable['2'] = KeyEvent::KEY_2;
	sKeyTable['3'] = KeyEvent::KEY_3;
	sKeyTable['4'] = KeyEvent::KEY_4;
	sKeyTable['5'] = KeyEvent::KEY_5;
	sKeyTable['6'] = KeyEvent::KEY_6;
	sKeyTable['7'] = KeyEvent::KEY_7;
	sKeyTable['8'] = KeyEvent::KEY_8;
	sKeyTable['9'] = KeyEvent::KEY_9;
	sKeyTable[0xBA] = KeyEvent::KEY_SEMICOLON;
	sKeyTable[0xBB] = KeyEvent::KEY_EQUALS;
	sKeyTable[0xDB] = KeyEvent::KEY_LEFTBRACKET;
	sKeyTable[0xDC] = KeyEvent::KEY_BACKSLASH;
	sKeyTable[VK_OEM_102] = KeyEvent::KEY_LESS;
	sKeyTable[0xDD] = KeyEvent::KEY_RIGHTBRACKET;
	sKeyTable[0xC0] = KeyEvent::KEY_BACKQUOTE;
	sKeyTable[0xDF] = KeyEvent::KEY_BACKQUOTE;
	sKeyTable['A'] = KeyEvent::KEY_a;
	sKeyTable['B'] = KeyEvent::KEY_b;
	sKeyTable['C'] = KeyEvent::KEY_c;
	sKeyTable['D'] = KeyEvent::KEY_d;
	sKeyTable['E'] = KeyEvent::KEY_e;
	sKeyTable['F'] = KeyEvent::KEY_f;
	sKeyTable['G'] = KeyEvent::KEY_g;
	sKeyTable['H'] = KeyEvent::KEY_h;
	sKeyTable['I'] = KeyEvent::KEY_i;
	sKeyTable['J'] = KeyEvent::KEY_j;
	sKeyTable['K'] = KeyEvent::KEY_k;
	sKeyTable['L'] = KeyEvent::KEY_l;
	sKeyTable['M'] = KeyEvent::KEY_m;
	sKeyTable['N'] = KeyEvent::KEY_n;
	sKeyTable['O'] = KeyEvent::KEY_o;
	sKeyTable['P'] = KeyEvent::KEY_p;
	sKeyTable['Q'] = KeyEvent::KEY_q;
	sKeyTable['R'] = KeyEvent::KEY_r;
	sKeyTable['S'] = KeyEvent::KEY_s;
	sKeyTable['T'] = KeyEvent::KEY_t;
	sKeyTable['U'] = KeyEvent::KEY_u;
	sKeyTable['V'] = KeyEvent::KEY_v;
	sKeyTable['W'] = KeyEvent::KEY_w;
	sKeyTable['X'] = KeyEvent::KEY_x;
	sKeyTable['Y'] = KeyEvent::KEY_y;
	sKeyTable['Z'] = KeyEvent::KEY_z;
	sKeyTable[VK_DELETE] = KeyEvent::KEY_DELETE;

	sKeyTable[VK_NUMPAD0] = KeyEvent::KEY_KP0;
	sKeyTable[VK_NUMPAD1] = KeyEvent::KEY_KP1;
	sKeyTable[VK_NUMPAD2] = KeyEvent::KEY_KP2;
	sKeyTable[VK_NUMPAD3] = KeyEvent::KEY_KP3;
	sKeyTable[VK_NUMPAD4] = KeyEvent::KEY_KP4;
	sKeyTable[VK_NUMPAD5] = KeyEvent::KEY_KP5;
	sKeyTable[VK_NUMPAD6] = KeyEvent::KEY_KP6;
	sKeyTable[VK_NUMPAD7] = KeyEvent::KEY_KP7;
	sKeyTable[VK_NUMPAD8] = KeyEvent::KEY_KP8;
	sKeyTable[VK_NUMPAD9] = KeyEvent::KEY_KP9;
	sKeyTable[VK_DECIMAL] = KeyEvent::KEY_KP_PERIOD;
	sKeyTable[VK_DIVIDE] = KeyEvent::KEY_KP_DIVIDE;
	sKeyTable[VK_MULTIPLY] = KeyEvent::KEY_KP_MULTIPLY;
	sKeyTable[VK_SUBTRACT] = KeyEvent::KEY_KP_MINUS;
	sKeyTable[VK_ADD] = KeyEvent::KEY_KP_PLUS;

	sKeyTable[VK_UP] = KeyEvent::KEY_UP;
	sKeyTable[VK_DOWN] = KeyEvent::KEY_DOWN;
	sKeyTable[VK_RIGHT] = KeyEvent::KEY_RIGHT;
	sKeyTable[VK_LEFT] = KeyEvent::KEY_LEFT;
	sKeyTable[VK_INSERT] = KeyEvent::KEY_INSERT;
	sKeyTable[VK_HOME] = KeyEvent::KEY_HOME;
	sKeyTable[VK_END] = KeyEvent::KEY_END;
	sKeyTable[VK_PRIOR] = KeyEvent::KEY_PAGEUP;
	sKeyTable[VK_NEXT] = KeyEvent::KEY_PAGEDOWN;

	sKeyTable[VK_F1] = KeyEvent::KEY_F1;
	sKeyTable[VK_F2] = KeyEvent::KEY_F2;
	sKeyTable[VK_F3] = KeyEvent::KEY_F3;
	sKeyTable[VK_F4] = KeyEvent::KEY_F4;
	sKeyTable[VK_F5] = KeyEvent::KEY_F5;
	sKeyTable[VK_F6] = KeyEvent::KEY_F6;
	sKeyTable[VK_F7] = KeyEvent::KEY_F7;
	sKeyTable[VK_F8] = KeyEvent::KEY_F8;
	sKeyTable[VK_F9] = KeyEvent::KEY_F9;
	sKeyTable[VK_F10] = KeyEvent::KEY_F10;
	sKeyTable[VK_F11] = KeyEvent::KEY_F11;
	sKeyTable[VK_F12] = KeyEvent::KEY_F12;
	sKeyTable[VK_F13] = KeyEvent::KEY_F13;
	sKeyTable[VK_F14] = KeyEvent::KEY_F14;
	sKeyTable[VK_F15] = KeyEvent::KEY_F15;

	sKeyTable[VK_NUMLOCK] = KeyEvent::KEY_NUMLOCK;
	sKeyTable[VK_CAPITAL] = KeyEvent::KEY_CAPSLOCK;
	sKeyTable[VK_SCROLL] = KeyEvent::KEY_SCROLLOCK;
	sKeyTable[VK_RSHIFT] = KeyEvent::KEY_RSHIFT;
	sKeyTable[VK_LSHIFT] = KeyEvent::KEY_LSHIFT;
	sKeyTable[VK_RCONTROL] = KeyEvent::KEY_RCTRL;
	sKeyTable[VK_LCONTROL] = KeyEvent::KEY_LCTRL;
	sKeyTable[VK_RMENU] = KeyEvent::KEY_RALT;
	sKeyTable[VK_LMENU] = KeyEvent::KEY_LALT;
	sKeyTable[VK_RWIN] = KeyEvent::KEY_RSUPER;
	sKeyTable[VK_LWIN] = KeyEvent::KEY_LSUPER;

	sKeyTable[VK_HELP] = KeyEvent::KEY_HELP;
	sKeyTable[VK_PRINT] = KeyEvent::KEY_PRINT;
	sKeyTable[VK_SNAPSHOT] = KeyEvent::KEY_PRINT;
	sKeyTable[VK_CANCEL] = KeyEvent::KEY_BREAK;
	sKeyTable[VK_APPS] = KeyEvent::KEY_MENU;
	
	sTableInited = true;
}

int	KeyEvent::translateNativeKeyCode( int nativeKeyCode )
{
	if( ! sTableInited )
		initKeyTable();
	
	if( nativeKeyCode >= MAX_KEYCODE )
		return KeyEvent::KEY_UNKNOWN;
	else
		return sKeyTable[nativeKeyCode];
}
#elif defined( CINDER_UWP )

using namespace Windows::System;

static bool sTableInited = false;
static const int MAX_KEYCODE = 256;
int sKeyTable[MAX_KEYCODE];

// Much of this keyTable is courtesy of SDL's keyboard handling code
// http://msdn.microsoft.com/en-US/library/windows/apps/xaml/windows.system.virtualkey
static void initKeyTable()
{
	for( int c = 0; c < MAX_KEYCODE; ++c )
		sKeyTable[c] = KeyEvent::KEY_UNKNOWN;

	sKeyTable[(int)VirtualKey::Back] = KeyEvent::KEY_BACKSPACE;
	sKeyTable[(int)VirtualKey::Tab] = KeyEvent::KEY_TAB;
	sKeyTable[(int)VirtualKey::Clear] = KeyEvent::KEY_CLEAR;
	sKeyTable[(int)VirtualKey::Enter] = KeyEvent::KEY_RETURN;
	sKeyTable[(int)VirtualKey::Pause] = KeyEvent::KEY_PAUSE;
	sKeyTable[(int)VirtualKey::Escape] = KeyEvent::KEY_ESCAPE;
	sKeyTable[(int)VirtualKey::Space] = KeyEvent::KEY_SPACE;
	//sKeyTable[0xDE] = KeyEvent::KEY_QUOTE;
	//sKeyTable[0xBC] = KeyEvent::KEY_COMMA;
	//sKeyTable[0xDD] = KeyEvent::KEY_MINUS;
	//sKeyTable[0xBE] = KeyEvent::KEY_PERIOD;
	//sKeyTable[0xBF] = KeyEvent::KEY_SLASH;
	sKeyTable[(int)VirtualKey::Number0] = KeyEvent::KEY_0;
	sKeyTable[(int)VirtualKey::Number1] = KeyEvent::KEY_1;
	sKeyTable[(int)VirtualKey::Number2] = KeyEvent::KEY_2;
	sKeyTable[(int)VirtualKey::Number3] = KeyEvent::KEY_3;
	sKeyTable[(int)VirtualKey::Number4] = KeyEvent::KEY_4;
	sKeyTable[(int)VirtualKey::Number5] = KeyEvent::KEY_5;
	sKeyTable[(int)VirtualKey::Number6] = KeyEvent::KEY_6;
	sKeyTable[(int)VirtualKey::Number7] = KeyEvent::KEY_7;
	sKeyTable[(int)VirtualKey::Number8] = KeyEvent::KEY_8;
	sKeyTable[(int)VirtualKey::Number9] = KeyEvent::KEY_9;
	//sKeyTable[0xBA] = KeyEvent::KEY_SEMICOLON;
	//sKeyTable[0xBB] = KeyEvent::KEY_EQUALS;
	//sKeyTable[0xDB] = KeyEvent::KEY_LEFTBRACKET;
	//sKeyTable[0xDC] = KeyEvent::KEY_BACKSLASH;
	//sKeyTable[VK_OEM_102] = KeyEvent::KEY_LESS;
	//sKeyTable[0xDD] = KeyEvent::KEY_RIGHTBRACKET;
	//sKeyTable[0xC0] = KeyEvent::KEY_BACKQUOTE;
	//sKeyTable[0xDF] = KeyEvent::KEY_BACKQUOTE;
	sKeyTable[(int)VirtualKey::A] = KeyEvent::KEY_a;
	sKeyTable[(int)VirtualKey::B] = KeyEvent::KEY_b;
	sKeyTable[(int)VirtualKey::C] = KeyEvent::KEY_c;
	sKeyTable[(int)VirtualKey::D] = KeyEvent::KEY_d;
	sKeyTable[(int)VirtualKey::E] = KeyEvent::KEY_e;
	sKeyTable[(int)VirtualKey::F] = KeyEvent::KEY_f;
	sKeyTable[(int)VirtualKey::G] = KeyEvent::KEY_g;
	sKeyTable[(int)VirtualKey::H] = KeyEvent::KEY_h;
	sKeyTable[(int)VirtualKey::I] = KeyEvent::KEY_i;
	sKeyTable[(int)VirtualKey::J] = KeyEvent::KEY_j;
	sKeyTable[(int)VirtualKey::K] = KeyEvent::KEY_k;
	sKeyTable[(int)VirtualKey::L] = KeyEvent::KEY_l;
	sKeyTable[(int)VirtualKey::M] = KeyEvent::KEY_m;
	sKeyTable[(int)VirtualKey::N] = KeyEvent::KEY_n;
	sKeyTable[(int)VirtualKey::O] = KeyEvent::KEY_o;
	sKeyTable[(int)VirtualKey::P] = KeyEvent::KEY_p;
	sKeyTable[(int)VirtualKey::Q] = KeyEvent::KEY_q;
	sKeyTable[(int)VirtualKey::R] = KeyEvent::KEY_r;
	sKeyTable[(int)VirtualKey::S] = KeyEvent::KEY_s;
	sKeyTable[(int)VirtualKey::T] = KeyEvent::KEY_t;
	sKeyTable[(int)VirtualKey::U] = KeyEvent::KEY_u;
	sKeyTable[(int)VirtualKey::V] = KeyEvent::KEY_v;
	sKeyTable[(int)VirtualKey::W] = KeyEvent::KEY_w;
	sKeyTable[(int)VirtualKey::X] = KeyEvent::KEY_x;
	sKeyTable[(int)VirtualKey::Y] = KeyEvent::KEY_y;
	sKeyTable[(int)VirtualKey::Z] = KeyEvent::KEY_z;
	sKeyTable[(int)VirtualKey::Delete] = KeyEvent::KEY_DELETE;

	sKeyTable[(int)VirtualKey::NumberPad0] = KeyEvent::KEY_KP0;
	sKeyTable[(int)VirtualKey::NumberPad1] = KeyEvent::KEY_KP1;
	sKeyTable[(int)VirtualKey::NumberPad2] = KeyEvent::KEY_KP2;
	sKeyTable[(int)VirtualKey::NumberPad3] = KeyEvent::KEY_KP3;
	sKeyTable[(int)VirtualKey::NumberPad4] = KeyEvent::KEY_KP4;
	sKeyTable[(int)VirtualKey::NumberPad5] = KeyEvent::KEY_KP5;
	sKeyTable[(int)VirtualKey::NumberPad6] = KeyEvent::KEY_KP6;
	sKeyTable[(int)VirtualKey::NumberPad7] = KeyEvent::KEY_KP7;
	sKeyTable[(int)VirtualKey::NumberPad8] = KeyEvent::KEY_KP8;
	sKeyTable[(int)VirtualKey::NumberPad9] = KeyEvent::KEY_KP9;
	sKeyTable[(int)VirtualKey::Decimal] = KeyEvent::KEY_KP_PERIOD;
	sKeyTable[(int)VirtualKey::Divide] = KeyEvent::KEY_KP_DIVIDE;
	sKeyTable[(int)VirtualKey::Multiply] = KeyEvent::KEY_KP_MULTIPLY;
	sKeyTable[(int)VirtualKey::Subtract] = KeyEvent::KEY_KP_MINUS;
	sKeyTable[(int)VirtualKey::Add] = KeyEvent::KEY_KP_PLUS;

	sKeyTable[(int)VirtualKey::Up] = KeyEvent::KEY_UP;
	sKeyTable[(int)VirtualKey::Down] = KeyEvent::KEY_DOWN;
	sKeyTable[(int)VirtualKey::Right] = KeyEvent::KEY_RIGHT;
	sKeyTable[(int)VirtualKey::Left] = KeyEvent::KEY_LEFT;
	sKeyTable[(int)VirtualKey::Insert] = KeyEvent::KEY_INSERT;
	sKeyTable[(int)VirtualKey::Home] = KeyEvent::KEY_HOME;
	sKeyTable[(int)VirtualKey::End] = KeyEvent::KEY_END;
	sKeyTable[(int)VirtualKey::PageUp] = KeyEvent::KEY_PAGEUP;
	sKeyTable[(int)VirtualKey::PageDown] = KeyEvent::KEY_PAGEDOWN;

	sKeyTable[(int)VirtualKey::F1] = KeyEvent::KEY_F1;
	sKeyTable[(int)VirtualKey::F2] = KeyEvent::KEY_F2;
	sKeyTable[(int)VirtualKey::F3] = KeyEvent::KEY_F3;
	sKeyTable[(int)VirtualKey::F4] = KeyEvent::KEY_F4;
	sKeyTable[(int)VirtualKey::F5] = KeyEvent::KEY_F5;
	sKeyTable[(int)VirtualKey::F6] = KeyEvent::KEY_F6;
	sKeyTable[(int)VirtualKey::F7] = KeyEvent::KEY_F7;
	sKeyTable[(int)VirtualKey::F8] = KeyEvent::KEY_F8;
	sKeyTable[(int)VirtualKey::F9] = KeyEvent::KEY_F9;
	sKeyTable[(int)VirtualKey::F10] = KeyEvent::KEY_F10;
	sKeyTable[(int)VirtualKey::F11] = KeyEvent::KEY_F11;
	sKeyTable[(int)VirtualKey::F12] = KeyEvent::KEY_F12;
	sKeyTable[(int)VirtualKey::F13] = KeyEvent::KEY_F13;
	sKeyTable[(int)VirtualKey::F14] = KeyEvent::KEY_F14;
	sKeyTable[(int)VirtualKey::F15] = KeyEvent::KEY_F15;

	sKeyTable[(int)VirtualKey::NumberKeyLock] = KeyEvent::KEY_NUMLOCK;
	sKeyTable[(int)VirtualKey::CapitalLock] = KeyEvent::KEY_CAPSLOCK;
	sKeyTable[(int)VirtualKey::Scroll] = KeyEvent::KEY_SCROLLOCK;
	sKeyTable[(int)VirtualKey::RightShift] = KeyEvent::KEY_RSHIFT;
	sKeyTable[(int)VirtualKey::LeftShift] = KeyEvent::KEY_LSHIFT;
	sKeyTable[(int)VirtualKey::RightControl] = KeyEvent::KEY_RCTRL;
	sKeyTable[(int)VirtualKey::LeftControl] = KeyEvent::KEY_LCTRL;
	sKeyTable[(int)VirtualKey::RightMenu] = KeyEvent::KEY_RALT;
	sKeyTable[(int)VirtualKey::LeftMenu] = KeyEvent::KEY_LALT;
	sKeyTable[(int)VirtualKey::RightWindows] = KeyEvent::KEY_RSUPER;
	sKeyTable[(int)VirtualKey::LeftWindows] = KeyEvent::KEY_LSUPER;

	sKeyTable[(int)VirtualKey::Help] = KeyEvent::KEY_HELP;
	sKeyTable[(int)VirtualKey::Print] = KeyEvent::KEY_PRINT;
	sKeyTable[(int)VirtualKey::Snapshot] = KeyEvent::KEY_PRINT;
	sKeyTable[(int)VirtualKey::Cancel] = KeyEvent::KEY_BREAK;
	sKeyTable[(int)VirtualKey::Application] = KeyEvent::KEY_MENU;
	
	sTableInited = true;
}

int	KeyEvent::translateNativeKeyCode( int nativeKeyCode )
{
	if( ! sTableInited )
		initKeyTable();
	
	if( nativeKeyCode >= MAX_KEYCODE )
		return KeyEvent::KEY_UNKNOWN;
	else
		return sKeyTable[nativeKeyCode];
}

#elif defined( CINDER_LINUX )

#if defined( CINDER_LINUX_EGL_ONLY )

static bool sTableInited = false;
static const int MAX_KEYCODE = 1024;
int sKeyTable[MAX_KEYCODE];

static void initKeyTable()
{
	for( int c = 0; c < MAX_KEYCODE; ++c ) {
		sKeyTable[c] = KeyEvent::KEY_UNKNOWN;
	}

	sKeyTable[ci::linux::KEY_BACKSPACE] = KeyEvent::KEY_BACKSPACE;
	sKeyTable[ci::linux::KEY_TAB]       = KeyEvent::KEY_TAB;
	// CLEAR
	sKeyTable[ci::linux::KEY_ENTER]     = KeyEvent::KEY_RETURN;
	sKeyTable[ci::linux::KEY_PAUSE]     = KeyEvent::KEY_PAUSE;
	sKeyTable[ci::linux::KEY_ESC]       = KeyEvent::KEY_ESCAPE;
	sKeyTable[ci::linux::KEY_SPACE]     = KeyEvent::KEY_SPACE;
	// EXCLAIM
	// QUOTEDBL
	// HASH
	sKeyTable[ci::linux::KEY_DOLLAR]     = KeyEvent::KEY_DOLLAR;
	// AMPERSAND
	sKeyTable[ci::linux::KEY_APOSTROPHE] = KeyEvent::KEY_QUOTE;
	// LEFTPAREN
	// RIGHTPAREN
	// ASTERIK
	// PLUS
	sKeyTable[ci::linux::KEY_COMMA]      = KeyEvent::KEY_COMMA;
	sKeyTable[ci::linux::KEY_MINUS]      = KeyEvent::KEY_MINUS;
	sKeyTable[ci::linux::KEY_DOT]        = KeyEvent::KEY_PERIOD;
	sKeyTable[ci::linux::KEY_SLASH]      = KeyEvent::KEY_SLASH;

	sKeyTable[ci::linux::KEY_0] = KeyEvent::KEY_0;
	sKeyTable[ci::linux::KEY_1] = KeyEvent::KEY_1;
	sKeyTable[ci::linux::KEY_2] = KeyEvent::KEY_2;
	sKeyTable[ci::linux::KEY_3] = KeyEvent::KEY_3;
	sKeyTable[ci::linux::KEY_4] = KeyEvent::KEY_4;
	sKeyTable[ci::linux::KEY_5] = KeyEvent::KEY_5;
	sKeyTable[ci::linux::KEY_6] = KeyEvent::KEY_6;
	sKeyTable[ci::linux::KEY_7] = KeyEvent::KEY_7;
	sKeyTable[ci::linux::KEY_8] = KeyEvent::KEY_8;
	sKeyTable[ci::linux::KEY_9] = KeyEvent::KEY_9;
	// COLON
	sKeyTable[ci::linux::KEY_SEMICOLON] = KeyEvent::KEY_SEMICOLON;
	// LESS
	sKeyTable[ci::linux::KEY_EQUAL] = KeyEvent::KEY_EQUALS;
	// CREATER
	// QUESTION
	// AT

	sKeyTable[ci::linux::KEY_LEFTBRACE]  = KeyEvent::KEY_LEFTBRACKET;
	sKeyTable[ci::linux::KEY_BACKSLASH]  = KeyEvent::KEY_BACKSLASH;
	sKeyTable[ci::linux::KEY_RIGHTBRACE] = KeyEvent::KEY_RIGHTBRACKET;
	// CARET
	// UNDERSCORE
	sKeyTable[ci::linux::KEY_GRAVE]      = KeyEvent::KEY_BACKQUOTE;

	sKeyTable[ci::linux::KEY_A] = KeyEvent::KEY_a;
	sKeyTable[ci::linux::KEY_B] = KeyEvent::KEY_b;
	sKeyTable[ci::linux::KEY_C] = KeyEvent::KEY_c;
	sKeyTable[ci::linux::KEY_D] = KeyEvent::KEY_d;
	sKeyTable[ci::linux::KEY_E] = KeyEvent::KEY_e;
	sKeyTable[ci::linux::KEY_F] = KeyEvent::KEY_f;
	sKeyTable[ci::linux::KEY_G] = KeyEvent::KEY_g;
	sKeyTable[ci::linux::KEY_H] = KeyEvent::KEY_h;
	sKeyTable[ci::linux::KEY_I] = KeyEvent::KEY_i;
	sKeyTable[ci::linux::KEY_J] = KeyEvent::KEY_j;
	sKeyTable[ci::linux::KEY_K] = KeyEvent::KEY_k;
	sKeyTable[ci::linux::KEY_L] = KeyEvent::KEY_l;
	sKeyTable[ci::linux::KEY_M] = KeyEvent::KEY_m;
	sKeyTable[ci::linux::KEY_N] = KeyEvent::KEY_n;
	sKeyTable[ci::linux::KEY_O] = KeyEvent::KEY_o;
	sKeyTable[ci::linux::KEY_P] = KeyEvent::KEY_p;
	sKeyTable[ci::linux::KEY_Q] = KeyEvent::KEY_q;
	sKeyTable[ci::linux::KEY_R] = KeyEvent::KEY_r;
	sKeyTable[ci::linux::KEY_S] = KeyEvent::KEY_s;
	sKeyTable[ci::linux::KEY_T] = KeyEvent::KEY_t;
	sKeyTable[ci::linux::KEY_U] = KeyEvent::KEY_u;
	sKeyTable[ci::linux::KEY_V] = KeyEvent::KEY_v;
	sKeyTable[ci::linux::KEY_W] = KeyEvent::KEY_w;
	sKeyTable[ci::linux::KEY_X] = KeyEvent::KEY_x;
	sKeyTable[ci::linux::KEY_Y] = KeyEvent::KEY_y;
	sKeyTable[ci::linux::KEY_Z] = KeyEvent::KEY_z;
	sKeyTable[ci::linux::KEY_DELETE] = KeyEvent::KEY_DELETE;

	sKeyTable[ci::linux::KEY_KP0]        = KeyEvent::KEY_KP0;
	sKeyTable[ci::linux::KEY_KP1]        = KeyEvent::KEY_KP1;
	sKeyTable[ci::linux::KEY_KP2]        = KeyEvent::KEY_KP2;
	sKeyTable[ci::linux::KEY_KP3]        = KeyEvent::KEY_KP3;
	sKeyTable[ci::linux::KEY_KP4]        = KeyEvent::KEY_KP4;
	sKeyTable[ci::linux::KEY_KP5]        = KeyEvent::KEY_KP5;
	sKeyTable[ci::linux::KEY_KP6]        = KeyEvent::KEY_KP6;
	sKeyTable[ci::linux::KEY_KP7]        = KeyEvent::KEY_KP7;
	sKeyTable[ci::linux::KEY_KP8]        = KeyEvent::KEY_KP8;
	sKeyTable[ci::linux::KEY_KP9]        = KeyEvent::KEY_KP9;
	sKeyTable[ci::linux::KEY_KPDOT]      = KeyEvent::KEY_KP_PERIOD;
	sKeyTable[ci::linux::KEY_KPSLASH]    = KeyEvent::KEY_KP_DIVIDE;
	sKeyTable[ci::linux::KEY_KPASTERISK] = KeyEvent::KEY_KP_MULTIPLY;
	sKeyTable[ci::linux::KEY_KPMINUS]    = KeyEvent::KEY_KP_MINUS;
	sKeyTable[ci::linux::KEY_KPPLUS]     = KeyEvent::KEY_KP_PLUS;
	sKeyTable[ci::linux::KEY_KPENTER]    = KeyEvent::KEY_KP_ENTER;
	sKeyTable[ci::linux::KEY_KPEQUAL]    = KeyEvent::KEY_KP_EQUALS;

	sKeyTable[ci::linux::KEY_UP]         = KeyEvent::KEY_UP;
	sKeyTable[ci::linux::KEY_DOWN]       = KeyEvent::KEY_DOWN;
	sKeyTable[ci::linux::KEY_RIGHT]      = KeyEvent::KEY_RIGHT;
	sKeyTable[ci::linux::KEY_LEFT]       = KeyEvent::KEY_LEFT;
	sKeyTable[ci::linux::KEY_INSERT]     = KeyEvent::KEY_INSERT;
	sKeyTable[ci::linux::KEY_HOME]       = KeyEvent::KEY_HOME;
	sKeyTable[ci::linux::KEY_END]        = KeyEvent::KEY_END;
	sKeyTable[ci::linux::KEY_PAGEUP]     = KeyEvent::KEY_PAGEUP;
	sKeyTable[ci::linux::KEY_PAGEDOWN]   = KeyEvent::KEY_PAGEDOWN;

	sKeyTable[ci::linux::KEY_F1]  = KeyEvent::KEY_F1;
	sKeyTable[ci::linux::KEY_F2]  = KeyEvent::KEY_F2;
	sKeyTable[ci::linux::KEY_F3]  = KeyEvent::KEY_F3;
	sKeyTable[ci::linux::KEY_F4]  = KeyEvent::KEY_F4;
	sKeyTable[ci::linux::KEY_F5]  = KeyEvent::KEY_F5;
	sKeyTable[ci::linux::KEY_F6]  = KeyEvent::KEY_F6;
	sKeyTable[ci::linux::KEY_F7]  = KeyEvent::KEY_F7;
	sKeyTable[ci::linux::KEY_F8]  = KeyEvent::KEY_F8;
	sKeyTable[ci::linux::KEY_F9]  = KeyEvent::KEY_F9;
	sKeyTable[ci::linux::KEY_F10] = KeyEvent::KEY_F10;
	sKeyTable[ci::linux::KEY_F11] = KeyEvent::KEY_F11;
	sKeyTable[ci::linux::KEY_F12] = KeyEvent::KEY_F12;
	sKeyTable[ci::linux::KEY_F13] = KeyEvent::KEY_F13;
	sKeyTable[ci::linux::KEY_F14] = KeyEvent::KEY_F14;
	sKeyTable[ci::linux::KEY_F15] = KeyEvent::KEY_F15; 

	sKeyTable[ci::linux::KEY_CAPSLOCK]   = KeyEvent::KEY_CAPSLOCK;
	sKeyTable[ci::linux::KEY_NUMLOCK]    = KeyEvent::KEY_NUMLOCK;
	sKeyTable[ci::linux::KEY_SCROLLLOCK] = KeyEvent::KEY_SCROLLOCK;
	sKeyTable[ci::linux::KEY_RIGHTSHIFT] = KeyEvent::KEY_RSHIFT;
	sKeyTable[ci::linux::KEY_LEFTSHIFT]  = KeyEvent::KEY_LSHIFT;
	sKeyTable[ci::linux::KEY_RIGHTCTRL]  = KeyEvent::KEY_RCTRL;
	sKeyTable[ci::linux::KEY_LEFTCTRL]   = KeyEvent::KEY_LCTRL;
	sKeyTable[ci::linux::KEY_RIGHTALT]   = KeyEvent::KEY_RALT;
	sKeyTable[ci::linux::KEY_LEFTALT]    = KeyEvent::KEY_LALT;
	sKeyTable[ci::linux::KEY_RIGHTMETA]  = KeyEvent::KEY_RMETA;
	sKeyTable[ci::linux::KEY_LEFTMETA]   = KeyEvent::KEY_LMETA;
	// LSUPER
	// RSUPER
	// MODE
	sKeyTable[ci::linux::KEY_COMPOSE]    = KeyEvent::KEY_COMPOSE;

	sKeyTable[ci::linux::KEY_HELP]  = KeyEvent::KEY_HELP;
	sKeyTable[ci::linux::KEY_PRINT] = KeyEvent::KEY_PRINT;
	sKeyTable[ci::linux::KEY_SYSRQ] = KeyEvent::KEY_SYSREQ;
	sKeyTable[ci::linux::KEY_BREAK] = KeyEvent::KEY_BREAK;
	sKeyTable[ci::linux::KEY_MENU]	= KeyEvent::KEY_MENU;
	sKeyTable[ci::linux::KEY_POWER] = KeyEvent::KEY_POWER;
	sKeyTable[ci::linux::KEY_EURO]  = KeyEvent::KEY_EURO;
	sKeyTable[ci::linux::KEY_UNDO]  = KeyEvent::KEY_UNDO;
}

int	KeyEvent::translateNativeKeyCode( int nativeKeyCode )
{
	if( ! sTableInited )
		initKeyTable();
	
	if( nativeKeyCode < 0 || nativeKeyCode >= MAX_KEYCODE )
		return KeyEvent::KEY_UNKNOWN;
	else
		return sKeyTable[nativeKeyCode];
}

#else

#include "glad/glad.h"
#include "glfw/glfw3.h"
static bool sTableInited = false;
static const int MAX_KEYCODE = GLFW_KEY_LAST + 1;
int sKeyTable[MAX_KEYCODE];

static void initKeyTable()
{
	for( int c = 0; c < MAX_KEYCODE; ++c ) {
		sKeyTable[c] = KeyEvent::KEY_UNKNOWN;
	}

	sKeyTable[GLFW_KEY_SPACE] = KeyEvent::KEY_SPACE;
	sKeyTable[GLFW_KEY_APOSTROPHE] = KeyEvent::KEY_QUOTE;  
	sKeyTable[GLFW_KEY_COMMA] = KeyEvent::KEY_COMMA;  
	sKeyTable[GLFW_KEY_MINUS] = KeyEvent::KEY_MINUS;  
	sKeyTable[GLFW_KEY_PERIOD] = KeyEvent::KEY_PERIOD;  
	sKeyTable[GLFW_KEY_SLASH] = KeyEvent::KEY_SLASH;  

	sKeyTable[GLFW_KEY_0] = KeyEvent::KEY_0;
	sKeyTable[GLFW_KEY_1] = KeyEvent::KEY_1;
	sKeyTable[GLFW_KEY_2] = KeyEvent::KEY_2;
	sKeyTable[GLFW_KEY_3] = KeyEvent::KEY_3;
	sKeyTable[GLFW_KEY_4] = KeyEvent::KEY_4;
	sKeyTable[GLFW_KEY_5] = KeyEvent::KEY_5;
	sKeyTable[GLFW_KEY_6] = KeyEvent::KEY_6;
	sKeyTable[GLFW_KEY_7] = KeyEvent::KEY_7;
	sKeyTable[GLFW_KEY_8] = KeyEvent::KEY_8;
	sKeyTable[GLFW_KEY_9] = KeyEvent::KEY_9;

	sKeyTable[GLFW_KEY_SEMICOLON] = KeyEvent::KEY_SEMICOLON;  
	sKeyTable[GLFW_KEY_EQUAL] = KeyEvent::KEY_EQUALS;  

	sKeyTable[GLFW_KEY_A] = KeyEvent::KEY_a;
	sKeyTable[GLFW_KEY_B] = KeyEvent::KEY_b;
	sKeyTable[GLFW_KEY_C] = KeyEvent::KEY_c;
	sKeyTable[GLFW_KEY_D] = KeyEvent::KEY_d;
	sKeyTable[GLFW_KEY_E] = KeyEvent::KEY_e;
	sKeyTable[GLFW_KEY_F] = KeyEvent::KEY_f;
	sKeyTable[GLFW_KEY_G] = KeyEvent::KEY_g;
	sKeyTable[GLFW_KEY_H] = KeyEvent::KEY_h;
	sKeyTable[GLFW_KEY_I] = KeyEvent::KEY_i;
	sKeyTable[GLFW_KEY_J] = KeyEvent::KEY_j;
	sKeyTable[GLFW_KEY_K] = KeyEvent::KEY_k;
	sKeyTable[GLFW_KEY_L] = KeyEvent::KEY_l;
	sKeyTable[GLFW_KEY_M] = KeyEvent::KEY_m;
	sKeyTable[GLFW_KEY_N] = KeyEvent::KEY_n;
	sKeyTable[GLFW_KEY_O] = KeyEvent::KEY_o;
	sKeyTable[GLFW_KEY_P] = KeyEvent::KEY_p;
	sKeyTable[GLFW_KEY_Q] = KeyEvent::KEY_q;
	sKeyTable[GLFW_KEY_R] = KeyEvent::KEY_r;
	sKeyTable[GLFW_KEY_S] = KeyEvent::KEY_s;
	sKeyTable[GLFW_KEY_T] = KeyEvent::KEY_t;
	sKeyTable[GLFW_KEY_U] = KeyEvent::KEY_u;
	sKeyTable[GLFW_KEY_V] = KeyEvent::KEY_v;
	sKeyTable[GLFW_KEY_W] = KeyEvent::KEY_w;
	sKeyTable[GLFW_KEY_X] = KeyEvent::KEY_x;
	sKeyTable[GLFW_KEY_Y] = KeyEvent::KEY_y;
	sKeyTable[GLFW_KEY_Z] = KeyEvent::KEY_z;

	sKeyTable[GLFW_KEY_BACKSLASH]     = KeyEvent::KEY_BACKSLASH;  
	sKeyTable[GLFW_KEY_LEFT_BRACKET]  = KeyEvent::KEY_LEFTBRACKET;  
	sKeyTable[GLFW_KEY_RIGHT_BRACKET] = KeyEvent::KEY_RIGHTBRACKET;  
	sKeyTable[GLFW_KEY_GRAVE_ACCENT]  = KeyEvent::KEY_BACKQUOTE;  
	// sKeyTable[GLFW_KEY_WORLD_1]       = KeyEvent::KEY_WORLD_1; 
	// sKeyTable[GLFW_KEY_WORLD_2]       = KeyEvent::KEY_WORLD_2; 
	sKeyTable[GLFW_KEY_ESCAPE]        = KeyEvent::KEY_ESCAPE;
	sKeyTable[GLFW_KEY_ENTER]         = KeyEvent::KEY_RETURN;
	sKeyTable[GLFW_KEY_TAB]           = KeyEvent::KEY_TAB;
	sKeyTable[GLFW_KEY_BACKSPACE]     = KeyEvent::KEY_BACKSPACE;
	sKeyTable[GLFW_KEY_INSERT]        = KeyEvent::KEY_INSERT;
	sKeyTable[GLFW_KEY_DELETE]        = KeyEvent::KEY_DELETE;
	sKeyTable[GLFW_KEY_RIGHT]         = KeyEvent::KEY_RIGHT;
	sKeyTable[GLFW_KEY_LEFT]          = KeyEvent::KEY_LEFT;
	sKeyTable[GLFW_KEY_DOWN]          = KeyEvent::KEY_DOWN;
	sKeyTable[GLFW_KEY_UP]            = KeyEvent::KEY_UP;
	sKeyTable[GLFW_KEY_PAGE_UP]       = KeyEvent::KEY_PAGEUP;
	sKeyTable[GLFW_KEY_PAGE_DOWN]     = KeyEvent::KEY_PAGEDOWN;
	sKeyTable[GLFW_KEY_HOME]          = KeyEvent::KEY_HOME;
	sKeyTable[GLFW_KEY_END]           = KeyEvent::KEY_END;
	sKeyTable[GLFW_KEY_CAPS_LOCK]     = KeyEvent::KEY_CAPSLOCK;
	sKeyTable[GLFW_KEY_SCROLL_LOCK]   = KeyEvent::KEY_SCROLLOCK;
	sKeyTable[GLFW_KEY_NUM_LOCK]      = KeyEvent::KEY_NUMLOCK;
	sKeyTable[GLFW_KEY_PRINT_SCREEN]  = KeyEvent::KEY_PRINT;
	sKeyTable[GLFW_KEY_PAUSE]         = KeyEvent::KEY_PAUSE;

	sKeyTable[GLFW_KEY_F1] = KeyEvent::KEY_F1;
	sKeyTable[GLFW_KEY_F2] = KeyEvent::KEY_F2;
	sKeyTable[GLFW_KEY_F3] = KeyEvent::KEY_F3;
	sKeyTable[GLFW_KEY_F4] = KeyEvent::KEY_F4;
	sKeyTable[GLFW_KEY_F5] = KeyEvent::KEY_F5;
	sKeyTable[GLFW_KEY_F6] = KeyEvent::KEY_F6;
	sKeyTable[GLFW_KEY_F7] = KeyEvent::KEY_F7;
	sKeyTable[GLFW_KEY_F8] = KeyEvent::KEY_F8;
	sKeyTable[GLFW_KEY_F9] = KeyEvent::KEY_F9;
	sKeyTable[GLFW_KEY_F10] = KeyEvent::KEY_F10;
	sKeyTable[GLFW_KEY_F11] = KeyEvent::KEY_F11;
	sKeyTable[GLFW_KEY_F12] = KeyEvent::KEY_F12;
	sKeyTable[GLFW_KEY_F13] = KeyEvent::KEY_F13;
	sKeyTable[GLFW_KEY_F14] = KeyEvent::KEY_F14;
	sKeyTable[GLFW_KEY_F15] = KeyEvent::KEY_F15;
	// sKeyTable[GLFW_KEY_F16] = KeyEvent::KEY_F16;
	// sKeyTable[GLFW_KEY_F17] = KeyEvent::KEY_F17;
	// sKeyTable[GLFW_KEY_F18] = KeyEvent::KEY_F18;
	// sKeyTable[GLFW_KEY_F19] = KeyEvent::KEY_F19;
	// sKeyTable[GLFW_KEY_F20] = KeyEvent::KEY_F20;
	// sKeyTable[GLFW_KEY_F21] = KeyEvent::KEY_F21;
	// sKeyTable[GLFW_KEY_F22] = KeyEvent::KEY_F22;
	// sKeyTable[GLFW_KEY_F23] = KeyEvent::KEY_F23;
	// sKeyTable[GLFW_KEY_F24] = KeyEvent::KEY_F24;
	// sKeyTable[GLFW_KEY_F25] = KeyEvent::KEY_F25;

	sKeyTable[GLFW_KEY_KP_0] = KeyEvent::KEY_KP0;
	sKeyTable[GLFW_KEY_KP_1] = KeyEvent::KEY_KP1;
	sKeyTable[GLFW_KEY_KP_2] = KeyEvent::KEY_KP2;
	sKeyTable[GLFW_KEY_KP_3] = KeyEvent::KEY_KP3;
	sKeyTable[GLFW_KEY_KP_4] = KeyEvent::KEY_KP4;
	sKeyTable[GLFW_KEY_KP_5] = KeyEvent::KEY_KP5;
	sKeyTable[GLFW_KEY_KP_6] = KeyEvent::KEY_KP6;
	sKeyTable[GLFW_KEY_KP_7] = KeyEvent::KEY_KP7;
	sKeyTable[GLFW_KEY_KP_8] = KeyEvent::KEY_KP8;
	sKeyTable[GLFW_KEY_KP_9] = KeyEvent::KEY_KP9;

	sKeyTable[GLFW_KEY_KP_DECIMAL]  = KeyEvent::KEY_KP_PERIOD;
	sKeyTable[GLFW_KEY_KP_DIVIDE]   = KeyEvent::KEY_KP_DIVIDE;
	sKeyTable[GLFW_KEY_KP_MULTIPLY] = KeyEvent::KEY_KP_MULTIPLY;
	sKeyTable[GLFW_KEY_KP_SUBTRACT] = KeyEvent::KEY_KP_MINUS;
	sKeyTable[GLFW_KEY_KP_ADD]      = KeyEvent::KEY_KP_PLUS;
	sKeyTable[GLFW_KEY_KP_ENTER]    = KeyEvent::KEY_KP_ENTER;
	sKeyTable[GLFW_KEY_KP_EQUAL]    = KeyEvent::KEY_KP_EQUALS;

	sKeyTable[GLFW_KEY_LEFT_SHIFT]   = KeyEvent::KEY_LSHIFT;
	sKeyTable[GLFW_KEY_LEFT_CONTROL] = KeyEvent::KEY_LCTRL;
	sKeyTable[GLFW_KEY_LEFT_ALT]     = KeyEvent::KEY_LALT;
	sKeyTable[GLFW_KEY_LEFT_SUPER]   = KeyEvent::KEY_LSUPER;

	sKeyTable[GLFW_KEY_RIGHT_SHIFT]   = KeyEvent::KEY_RSHIFT;
	sKeyTable[GLFW_KEY_RIGHT_CONTROL] = KeyEvent::KEY_RCTRL;
	sKeyTable[GLFW_KEY_RIGHT_ALT]     = KeyEvent::KEY_RALT;
	sKeyTable[GLFW_KEY_RIGHT_SUPER]   = KeyEvent::KEY_RSUPER;

	sKeyTable[GLFW_KEY_MENU] = KeyEvent::KEY_MENU;
}

int	KeyEvent::translateNativeKeyCode( int nativeKeyCode )
{
	if( ! sTableInited )
		initKeyTable();
	
	if( nativeKeyCode < 0 || nativeKeyCode >= MAX_KEYCODE )
		return KeyEvent::KEY_UNKNOWN;
	else
		return sKeyTable[nativeKeyCode];
}
#endif

#endif // defined( CINDER_LINUX )
	
} } // namespace cinder::app
