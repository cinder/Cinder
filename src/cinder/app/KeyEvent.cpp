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

#include "cinder/Cinder.h"
#include "cinder/app/KeyEvent.h"

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

#elif defined( CINDER_MSW )

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
	sKeyTable[0xDD] = KeyEvent::KEY_MINUS;
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
#endif // defined( CINDER_MSW )
	
} } // namespace cinder::app
