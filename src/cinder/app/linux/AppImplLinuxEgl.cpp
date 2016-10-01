/*
 Copyright (c) 2012, The Cinder Project, All rights reserved.

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

#include "cinder/app/linux/AppImplLinux.h"
#include "cinder/app/linux/AppLinux.h"
#include "cinder/app/linux/WindowImplLinux.h"
#include "cinder/linux/input_redef.h"

#include <bcm_host.h>

/*
#include <termio.h>
#include <unistd.h>
*/

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <regex.h>
#include <dirent.h>
#include <fcntl.h>

namespace rpi {

// Raspberry Pi input code borrowed from here:
// https://www.raspberrypi.org/forums/viewtopic.php?f=67&t=89109
//

enum {
	INPUT_UNKNOWN			= 0x000,
    INPUT_KEYUP 			= 0x100,
    INPUT_KEYDOWN 			= 0x101,
    INPUT_KEYREPEAT 		= 0x102,
    INPUT_MOUSEBUTTONUP 	= 0x200,
    INPUT_MOUSEBUTTONDOWN	= 0x201,
    INPUT_MOUSEMOTION 		= 0x300,

	INPUT_MOUSELEFT			= 272,
	INPUT_MOUSEMIDDLE		= 273,
	INPUT_MOUSERIGHT		= 274
};

struct InputEvent
{
    uint16_t type;
    uint16_t code; 		// button and keycodes from linux/input.h
    int32_t  motion[3];	// X, Y, mouse_wheel

	InputEvent() {
		type = 0;
		code = 0;
		motion[0] = 0;
		motion[1] = 0;
		motion[2] = 0;
	}
};

bool InputInit();
void InputClose();
bool InputPollEvent( InputEvent* pEvent );
bool InputIsKeyDown( uint16_t keyCode );
bool InputIsButtonDown( uint16_t buttonCode );


} // namespace rpi

namespace cinder { namespace app {

struct MouseState {
	ivec2 pos 		= ivec2( 0 );
	bool leftDown 	= false;
	bool middleDown	= false;
	bool rightDown 	= false;
};

static MouseState sMouseState;

char keyCodeToChar( uint16_t cinderKeyCode );

//bool keyPressed( int *character );	
//void keyboardReset();

////////////////////////////////////////////////////////////////////////////////
// AppImplLinux
////////////////////////////////////////////////////////////////////////////////
AppImplLinux::AppImplLinux( AppLinux *aApp, const AppLinux::Settings &settings )
	: mApp( aApp )
{
	::bcm_host_init();

	uint32_t displaySizeX = 0;
	uint32_t displaySizeY = 0;
	int32_t success = ::graphics_get_display_size( 0, &displaySizeX, &displaySizeY );
	if( success < 0 ) {
		std::cerr << "graphics_get_display_size failed, returned: " << success << std::endl;
		std::exit( 1 );
	}
	mDefaultDisplaySize = ivec2( displaySizeX, displaySizeY );

	mFrameRate = settings.getFrameRate();
	mFrameRateEnabled = settings.isFrameRateEnabled();
	
	auto formats = settings.getWindowFormats();
	if( formats.empty() ) {
		formats.push_back( settings.getDefaultWindowFormat() );
	}

	for( auto& format : formats ) {
		if( ! format.isTitleSpecified() ) {
			format.setTitle( settings.getTitle() );
		}

		// We need to keep tabs on the main window
		if( ! mMainWindow ) {
			mMainWindow = this->createWindow( format );
		}
		else {
			this->createWindow( format );
		}
	}

	// Set the active window
	if( ! mWindows.empty() ) {
		setWindow( mWindows.back()->getWindow() );
	}
}

AppImplLinux::~AppImplLinux()
{
	::bcm_host_deinit();
}

AppLinux *AppImplLinux::getApp()
{
	return mApp;
}

void AppImplLinux::sleepUntilNextFrame()
{
	// get current time in seconds
	double currentSeconds = getElapsedSeconds();

	// calculate time per frame in seconds
	double secondsPerFrame = 1.0 / mFrameRate;

	// determine if application was frozen for a while and adjust next frame time
	double elapsedSeconds = currentSeconds - mNextFrameTime;
	if( elapsedSeconds > 1.0 ) {
		int numSkipFrames = (int)(elapsedSeconds / secondsPerFrame);
		mNextFrameTime += (numSkipFrames * secondsPerFrame);
	}

	// determine when next frame should be drawn
	mNextFrameTime += secondsPerFrame;

	// sleep and process messages until next frame
	if( ( mFrameRateEnabled ) && ( mNextFrameTime > currentSeconds ) ) {
		double sleepTime = std::max( mNextFrameTime - currentSeconds, 0.0 );
		unsigned long sleepMicroSecs = sleepTime*1000000L;
		usleep(sleepMicroSecs);
	}
}

void AppImplLinux::run()
{
	mApp->privateSetup__();
	mSetupHasBeenCalled = true;

	// issue initial app activation event
	mApp->emitDidBecomeActive();
	
	// issue initial resize revent
	for( auto &window : mWindows ) {
		window->resize();
	}

	// start input 
	if( rpi::InputInit() ) {
		std::cout << "Mouse and keyboard input successfully initialized" << std::endl;
	}
	else {
		std::cout << "Failed to initialize input for mouse and keyboard" << std::endl;
	}

	// initialize our next frame time
	mNextFrameTime = getElapsedSeconds();	

	while( ! mShouldQuit ) {
		// input events
		rpi::InputEvent inputEvent;
		while( rpi::InputPollEvent( &inputEvent ) ) {
			switch( inputEvent.type ) {
				case rpi::INPUT_KEYDOWN: {
					if( ci::linux::KEY_ESC == inputEvent.code ) {
						mShouldQuit = true;
					}

					int cinderKeyCode = KeyEvent::translateNativeKeyCode( inputEvent.code );
					uint32_t char32 = 0;
					char char8 = keyCodeToChar( cinderKeyCode );
					uint32_t modifiers = 0;
					KeyEvent event( mMainWindow, cinderKeyCode, char32, char8, modifiers, inputEvent.code );
					mMainWindow->emitKeyDown( &event );
				}
				break;

				case rpi::INPUT_KEYUP: {
					int cinderKeyCode = KeyEvent::translateNativeKeyCode( inputEvent.code );
					uint32_t char32 = 0;
					char char8 = keyCodeToChar( cinderKeyCode );
					uint32_t modifiers = 0;
					KeyEvent event( mMainWindow, cinderKeyCode, char32, char8, modifiers, inputEvent.code );
					mMainWindow->emitKeyUp( &event );
				}
				break;

				case rpi::INPUT_MOUSEMOTION: {
					sMouseState.pos += ivec2( inputEvent.motion[0], inputEvent.motion[1] );
					sMouseState.pos = clamp( sMouseState.pos, ivec2( 0, 0 ), mMainWindow->getSize() );

					int initiator = 0;
					if( sMouseState.leftDown ) {
						initiator = MouseEvent::LEFT_DOWN;
					}
					else if( sMouseState.middleDown ) {
						initiator = MouseEvent::MIDDLE_DOWN;
					}
					else if( sMouseState.rightDown ) {
						initiator = MouseEvent::RIGHT_DOWN;
					}

					int modifiers = 0;
					if( sMouseState.leftDown ) {
						modifiers |= MouseEvent::LEFT_DOWN;
					}
					if( sMouseState.middleDown ) {
						modifiers |= MouseEvent::MIDDLE_DOWN;
					}
					if( sMouseState.rightDown ) {
						modifiers |= MouseEvent::RIGHT_DOWN;
					}

					MouseEvent event( getWindow(), initiator, sMouseState.pos.x, sMouseState.pos.y, modifiers, 0.0f, 0 );
					if( 0 != initiator ) {
						mMainWindow->emitMouseDrag( &event );
					}
					else {
						mMainWindow->emitMouseMove( &event );
					}
				} 
				break;

				case rpi::INPUT_MOUSEBUTTONDOWN: {
					int initiator = 0;
					switch( inputEvent.code ) {
						case rpi::INPUT_MOUSELEFT:   { sMouseState.leftDown   = true; initiator = MouseEvent::LEFT_DOWN;   } break;
						case rpi::INPUT_MOUSEMIDDLE: { sMouseState.middleDown = true; initiator = MouseEvent::MIDDLE_DOWN; } break;
						case rpi::INPUT_MOUSERIGHT:  { sMouseState.rightDown  = true; initiator = MouseEvent::RIGHT_DOWN;  } break;
					}

					int modifiers = 0;
					if( sMouseState.leftDown ) {
						modifiers |= MouseEvent::LEFT_DOWN;
					}
					if( sMouseState.middleDown ) {
						modifiers |= MouseEvent::MIDDLE_DOWN;
					}
					if( sMouseState.rightDown ) {
						modifiers |= MouseEvent::RIGHT_DOWN;
					}

					MouseEvent event( getWindow(), initiator, sMouseState.pos.x, sMouseState.pos.y, modifiers, 0.0f, 0 );
					mMainWindow->emitMouseDown( &event );
				}
				break;

				case rpi::INPUT_MOUSEBUTTONUP: {
					int initiator = 0;
					switch( inputEvent.code ) {
						case rpi::INPUT_MOUSELEFT:   { sMouseState.leftDown   = false; initiator = MouseEvent::LEFT_DOWN;   } break;
						case rpi::INPUT_MOUSEMIDDLE: { sMouseState.middleDown = false; initiator = MouseEvent::MIDDLE_DOWN; } break;
						case rpi::INPUT_MOUSERIGHT:  { sMouseState.rightDown  = false; initiator = MouseEvent::RIGHT_DOWN;  } break;
					}

					MouseEvent event( getWindow(), initiator, sMouseState.pos.x, sMouseState.pos.y, 0, 0.0f, 0 );
					mMainWindow->emitMouseUp( &event );
				}
				break;
			}
		}

		// update and draw
		mApp->privateUpdate__();
		for( auto &window : mWindows ) {
			window->draw();
		}

		// Sleep until the next frame
		if( ! mShouldQuit ) {
			sleepUntilNextFrame();
		}
	}

	rpi::InputClose();
}

RendererRef AppImplLinux::findSharedRenderer( const RendererRef &searchRenderer )
{
	if( ! searchRenderer ) {
		return RendererRef();
	}

	for( const auto &win : mWindows ) {
		RendererRef renderer = win->getRenderer();
		if( renderer && ( typeid(*renderer) == typeid(*searchRenderer) ) ) {
			return renderer;
		}
	}

	return RendererRef();	
}

WindowRef AppImplLinux::createWindow( Window::Format format )
{
	if( ! format.getRenderer() ) {
		format.setRenderer( mApp->getDefaultRenderer()->clone() );
	}

	mWindows.push_back( new WindowImplLinux( format, findSharedRenderer( format.getRenderer() ), this ) );

	// emit initial resize if we have fired setup
	if( mSetupHasBeenCalled ) {
		mWindows.back()->getWindow()->emitResize();
	}

	return mWindows.back()->getWindow();
}

void AppImplLinux::quit()
{
	mShouldQuit = true;
}

float AppImplLinux::getFrameRate() const 
{ 
	return mFrameRate; 
}

void AppImplLinux::setFrameRate( float frameRate )
{
	mFrameRate = frameRate;
	mFrameRateEnabled = true;
	mNextFrameTime = mApp->getElapsedSeconds();
}

void AppImplLinux::disableFrameRate()
{
	mFrameRateEnabled = false;
}

bool AppImplLinux::isFrameRateEnabled() const
{
	return mFrameRateEnabled;
}

WindowRef AppImplLinux::getWindow() const 
{ 
	return mActiveWindow; 
}

void AppImplLinux::setWindow( WindowRef window ) 
{ 
	mActiveWindow = window; 
}

size_t AppImplLinux::getNumWindows() const
{
	return mWindows.size();
}

WindowRef AppImplLinux::getWindowIndex( size_t index ) const
{
	if( index >= mWindows.size() ) {
		return cinder::app::WindowRef();
	}
	
	auto winIt = mWindows.begin();
	std::advance( winIt, index );
	return (*winIt)->mWindowRef;
}

WindowRef AppImplLinux::getForegroundWindow() const
{
	return mForegroundWindow;
}

void AppImplLinux::setForegroundWindow( WindowRef window )
{
	mForegroundWindow = window;
}

void AppImplLinux::hideCursor()
{
	mActiveWindow->getImpl()->hideCursor();
}

void AppImplLinux::showCursor()
{
	mActiveWindow->getImpl()->showCursor();
}

ivec2 AppImplLinux::getMousePos() const
{
	return sMouseState.pos;	
}

#if defined( CINDER_LINUX_EGL_ONLY )
ivec2 AppImplLinux::getDefaultDisplaySize() const
{
	return mDefaultDisplaySize;
}
#endif

void AppImplLinux::registerWindowEvents( WindowImplLinux* window )
{
}

void AppImplLinux::unregisterWindowEvents( WindowImplLinux* window )
{
}

char keyCodeToChar( uint16_t cinderKeyCode )
{
	char result = 0;
	switch( cinderKeyCode ) {
		case KeyEvent::KEY_TAB         : result = '\t'; break;
		case KeyEvent::KEY_RETURN      : result = '\n'; break;
		case KeyEvent::KEY_SPACE       : result = ' '; break;
		case KeyEvent::KEY_EXCLAIM     : result = '!'; break;
		case KeyEvent::KEY_QUOTEDBL    : result = '"'; break;
		case KeyEvent::KEY_HASH        : result = '#'; break;
		case KeyEvent::KEY_DOLLAR      : result = '$'; break;
		case KeyEvent::KEY_AMPERSAND   : result = '&'; break;
		case KeyEvent::KEY_QUOTE       : result = '\''; break;
		case KeyEvent::KEY_LEFTPAREN   : result = '('; break;
		case KeyEvent::KEY_RIGHTPAREN  : result = ')'; break;
		case KeyEvent::KEY_ASTERISK    : result = '*'; break;
		case KeyEvent::KEY_PLUS        : result = '+'; break;
		case KeyEvent::KEY_COMMA       : result = ','; break;
		case KeyEvent::KEY_MINUS       : result = '-'; break;
		case KeyEvent::KEY_PERIOD      : result = '.'; break;
		case KeyEvent::KEY_SLASH       : result = '/'; break;
		case KeyEvent::KEY_0           : result = '0'; break;
		case KeyEvent::KEY_1           : result = '1'; break;
		case KeyEvent::KEY_2           : result = '2'; break;
		case KeyEvent::KEY_3           : result = '3'; break;
		case KeyEvent::KEY_4           : result = '4'; break;
		case KeyEvent::KEY_5           : result = '5'; break;
		case KeyEvent::KEY_6           : result = '6'; break;
		case KeyEvent::KEY_7           : result = '8'; break;
		case KeyEvent::KEY_8           : result = '0'; break;
		case KeyEvent::KEY_9           : result = '9'; break;
		case KeyEvent::KEY_COLON       : result = ':'; break;
		case KeyEvent::KEY_SEMICOLON   : result = ';'; break;
		case KeyEvent::KEY_LESS        : result = '<'; break;
		case KeyEvent::KEY_EQUALS      : result = '='; break;
		case KeyEvent::KEY_GREATER     : result = '>'; break;
		case KeyEvent::KEY_QUESTION    : result = '?'; break;
		case KeyEvent::KEY_AT          : result = '@'; break;

		case KeyEvent::KEY_LEFTBRACKET : result = '['; break;
		case KeyEvent::KEY_BACKSLASH   : result = '\\'; break;
		case KeyEvent::KEY_RIGHTBRACKET: result = ']'; break;
		case KeyEvent::KEY_CARET       : result = '^'; break;
		case KeyEvent::KEY_UNDERSCORE  : result = '_'; break;
		case KeyEvent::KEY_BACKQUOTE   : result = '`'; break;
		case KeyEvent::KEY_a           : result = 'a'; break;
		case KeyEvent::KEY_b           : result = 'b'; break;
		case KeyEvent::KEY_c           : result = 'c'; break;
		case KeyEvent::KEY_d           : result = 'd'; break;
		case KeyEvent::KEY_e           : result = 'e'; break;
		case KeyEvent::KEY_f           : result = 'f'; break;
		case KeyEvent::KEY_g           : result = 'g'; break;
		case KeyEvent::KEY_h           : result = 'h'; break;
		case KeyEvent::KEY_i           : result = 'i'; break;
		case KeyEvent::KEY_j           : result = 'j'; break;
		case KeyEvent::KEY_k           : result = 'k'; break;
		case KeyEvent::KEY_l           : result = 'l'; break;
		case KeyEvent::KEY_m           : result = 'm'; break;
		case KeyEvent::KEY_n           : result = 'n'; break;
		case KeyEvent::KEY_o           : result = 'o'; break;
		case KeyEvent::KEY_p           : result = 'p'; break;
		case KeyEvent::KEY_q           : result = 'q'; break;
		case KeyEvent::KEY_r           : result = 'r'; break;
		case KeyEvent::KEY_s           : result = 's'; break;
		case KeyEvent::KEY_t           : result = 't'; break;
		case KeyEvent::KEY_u           : result = 'u'; break;
		case KeyEvent::KEY_v           : result = 'v'; break;
		case KeyEvent::KEY_w           : result = 'w'; break;
		case KeyEvent::KEY_x           : result = 'x'; break;
		case KeyEvent::KEY_y           : result = 'y'; break;
		case KeyEvent::KEY_z           : result = 'z'; break;

		case KeyEvent::KEY_KP0         : result = '0'; break;
		case KeyEvent::KEY_KP1         : result = '1'; break;
		case KeyEvent::KEY_KP2         : result = '2'; break;
		case KeyEvent::KEY_KP3         : result = '3'; break;
		case KeyEvent::KEY_KP4         : result = '4'; break;
		case KeyEvent::KEY_KP5         : result = '5'; break;
		case KeyEvent::KEY_KP6         : result = '6'; break;
		case KeyEvent::KEY_KP7         : result = '7'; break;
		case KeyEvent::KEY_KP8         : result = '8'; break;
		case KeyEvent::KEY_KP9         : result = '9'; break;
		case KeyEvent::KEY_KP_PERIOD   : result = '.'; break;
		case KeyEvent::KEY_KP_DIVIDE   : result = '/'; break;
		case KeyEvent::KEY_KP_MULTIPLY : result = '*'; break;
		case KeyEvent::KEY_KP_MINUS    : result = '-'; break;
		case KeyEvent::KEY_KP_PLUS     : result = '+'; break;
		case KeyEvent::KEY_KP_ENTER    : result = '\n'; break;
		case KeyEvent::KEY_KP_EQUALS   : result = '='; break;
	}

	return result;
}

/*
////////////////////////////////////////////////////////////////////////////////
//
// Borrowed from:
//     https://github.com/AndrewFromMelbourne/raspidmx/blob/master/common/key.c
//
////////////////////////////////////////////////////////////////////////////////
static int sStdInFileDesc = -1;
static struct termios sTermiosAttribs;

bool keyPressed(int *character)
{
    // If this is the first time the function is called, change the stdin
    // stream so that we get each character when the keys are pressed and
    // and so that character aren't echoed to the screen when the keys are
    // pressed.
    if (-1 == sStdInFileDesc)
    {
        // Get the file descriptor associated with stdin stream.
        sStdInFileDesc = fileno(stdin);

        // Get the terminal (termios) attritubets for stdin so we can
        // modify them and reset them before exiting the program.
        tcgetattr(sStdInFileDesc, &sTermiosAttribs);

        // Copy the termios attributes so we can modify them.
        struct termios term;
        memcpy(&term, &sTermiosAttribs, sizeof(term));

        // Unset ICANON and ECHO for stdin. When ICANON is not set the
        // input is in noncanonical mode. In noncanonical mode input is
        // available as each key is pressed. In canonical mode input is
        // only available after the enter key is pressed. Unset ECHO so that
        // the characters aren't echoed to the screen when keys are pressed.
        // See the termios(3) man page for more information.
        term.c_lflag &= ~(ICANON|ECHO);
        tcsetattr(sStdInFileDesc, TCSANOW, &term);

        // Turn off buffering for stdin. We want to get the characters
        // immediately. We don't want the characters to be buffered.
        setbuf(stdin, NULL);
    }

    // Get the number of characters that are waiting to be read.
    int charactersBuffered = 0;
    ioctl(sStdInFileDesc, FIONREAD, &charactersBuffered);

    // Set the return value to true if there are any characters to be read.
    bool pressed = (charactersBuffered != 0);

    if (charactersBuffered == 1)
    {
        // There is only one character to be read. Read it in.
        int c = fgetc(stdin);

        // Check if the caller wants the value of character read.
        if (character != NULL)
        {
            *character = c;
        }
    }
    else if (charactersBuffered > 1)
    {
        // There is more than one character to be read. This can be key such
        // as the arrow keys or function keys. This code just reads them in
        // and ignores them. The caller will be informed that a key was
        // pressed, but won't get a value for the key.
        while (charactersBuffered)
        {
            fgetc(stdin);
            --charactersBuffered;
        }
    }

    return pressed;
}

void keyboardReset()
{
    if (-1 == sStdInFileDesc) {
        // If keyPressed() has been called the terminal input has been
        // changed for the stdin stream. Put the attributes back the way
        // we found them.
        tcsetattr(sStdInFileDesc, TCSANOW, &sTermiosAttribs);
    }
}
*/

}} // namespace cinder::app

#include <linux/input.h>

namespace rpi {

#define INPUT_BUFFER_SIZE 64

struct EventBuffer
{
    struct input_event buffer[INPUT_BUFFER_SIZE];
    int first;
    int count;
};

static struct EventBuffer keyboardEventBuffer;
static struct EventBuffer mouseEventBuffer;
static int keyboardFd = -1;
static int mouseFd = -1;
static int mouseMove[3];

#define INPUT_NUM_KEYS 256
#define INPUT_NUM_BUTTONS 32

static bool keyStates[INPUT_NUM_KEYS];
static bool buttonStates[INPUT_NUM_BUTTONS];

bool InputInit(void)
{
    keyboardEventBuffer.first = keyboardEventBuffer.count = 0;
    mouseEventBuffer.first = mouseEventBuffer.count = 0;
    keyboardFd = mouseFd = -1;
    mouseMove[0] = mouseMove[1] = mouseMove[2] = 0;

    regex_t kbd;
    regex_t mouse;
    if( regcomp(&kbd,"event-kbd",0) != 0 ) {
        printf("regcomp for kbd failed\n");
        return false;
    }
    if( regcomp(&mouse,"event-mouse",0) != 0 ) {
        printf("regcomp for mouse failed\n");
        return false;
    }

    DIR *dirp;
    const char *dirName = "/dev/input/by-id";
    if( ( dirp = opendir(dirName) ) == NULL ) {
        perror("couldn't open '/dev/input/by-id'");
        return false;
    }

    // Find any files in the directory that match the regex for keyboard or mouse
    struct dirent *dp;
    do {     
        errno = 0;
        
        if( ( dp = readdir( dirp ) ) != NULL ) {
            //printf("readdir (%s)\n",dp->d_name);

            char fullPath[1024];
            int result;

            if( regexec( &kbd, dp->d_name, 0, NULL, 0 ) == 0 ) {
                //printf("match for the kbd = %s\n",dp->d_name);
                sprintf(fullPath,"%s/%s",dirName,dp->d_name);
                keyboardFd = open(fullPath,O_RDONLY | O_NONBLOCK);
                //printf("%s Fd = %d\n",fullPath,keyboardFd);
                //printf("Getting exclusive access: ");
                result = ioctl(keyboardFd, EVIOCGRAB, 1);
                if (result != 0)
                    printf("Couldn't grab exclusive keyboard access\n");
            }
            if( regexec( &mouse, dp->d_name, 0, NULL, 0) == 0 ) {
                //printf("match for the kbd = %s\n",dp->d_name);
                sprintf(fullPath,"%s/%s",dirName,dp->d_name);
                mouseFd = open(fullPath,O_RDONLY | O_NONBLOCK);
                //printf("%s Fd = %d\n",fullPath,mouseFd);
                //printf("Getting exclusive access: ");
                result = ioctl(mouseFd, EVIOCGRAB, 1);
                if( result != 0 ) {
                    printf("Couldn't grab exclusive mouse access\n");
                }
            }

        }
    } while( dp != NULL );

    closedir(dirp);

    regfree(&kbd);
    regfree(&mouse);

    if( keyboardFd == -1 ) {
        printf( "couldn't find keyboard event file\n" );
        return false;
    }

    if( mouseFd == -1 ) {
        printf( "couldn't find mouse event file\n" );
        return false;
    }

    return true;
}

void InputClose(void)
{
    int result;

    if( mouseFd != -1 ) {
        //printf("Releasing exclusive mouse access: ");
        result = ioctl(mouseFd, EVIOCGRAB, 0);
        if( result != 0 ) {
            printf("Couldn't release exclusive mouse access\n");
        }
        close(mouseFd);
    }

    if( keyboardFd != -1 ) {
        //printf("Releasing exclusive keyboard access: ");
        result = ioctl(keyboardFd, EVIOCGRAB, 0);
        if( result != 0 ) {
            printf("Couldn't release exclusive keyboard access\n");
        }
        close(keyboardFd);
    }
}

bool InputPollEvent( InputEvent* pEvent )
{
    // Read more events from keyboard if the buffer is empty
    if( keyboardEventBuffer.count == 0 && keyboardFd != -1 ) {
        int bytesRead = read(keyboardFd, keyboardEventBuffer.buffer, sizeof(struct input_event)*INPUT_BUFFER_SIZE);
        if( bytesRead > 0 ) {
            keyboardEventBuffer.first = 0;
            keyboardEventBuffer.count = bytesRead / sizeof(struct input_event);
        }
    }

    // Read more events from mouse if the buffer is empty
    if( mouseEventBuffer.count == 0 && mouseFd != -1 ) {
        int bytesRead = read(mouseFd, mouseEventBuffer.buffer, sizeof(struct input_event)*INPUT_BUFFER_SIZE);
        if( bytesRead > 0 ) {
            mouseEventBuffer.first = 0;
            mouseEventBuffer.count = bytesRead / sizeof(struct input_event);
        }
    }

    // Process events until finding one that should be reported
    while( true ) {
        if( keyboardEventBuffer.count == 0 && mouseEventBuffer.count == 0 ) {
            return false;
        }

        // Find the oldest event from either buffer
        struct EventBuffer* pInput;
        if( keyboardEventBuffer.count == 0 ) {
            pInput = &mouseEventBuffer;
        }
        else {
            if( mouseEventBuffer.count == 0 ) {
                pInput = &keyboardEventBuffer;
            }
            else {
                struct timeval* keyboardEventTime = &keyboardEventBuffer.buffer[keyboardEventBuffer.first].time;
                struct timeval* mouseEventTime = &mouseEventBuffer.buffer[mouseEventBuffer.first].time;

                if( ( keyboardEventTime->tv_sec < mouseEventTime->tv_sec ) ||
                    ( ( keyboardEventTime->tv_sec == mouseEventTime->tv_sec ) && 
                      ( keyboardEventTime->tv_usec < mouseEventTime->tv_usec) ) 
                ) {
                
                    pInput = &keyboardEventBuffer;
                }
                else {
                    pInput = &mouseEventBuffer;
                }
            }        
        }
        
        // Get the event from the buffer
        struct input_event* evp = &pInput->buffer[pInput->first];
        pInput->first++;
        pInput->count--;

        // relative mouse movement
        if( evp->type == EV_REL ) {
            if(evp->code == REL_X) {
                mouseMove[0] = evp->value;
            }
            else if(evp->code == REL_Y) {
                mouseMove[1] = evp->value;
            }
            else if(evp->code == REL_WHEEL) {
                mouseMove[2] = evp->value;
            }          
        }
        // sync: end of a logical mouse action
        else if( evp->type == EV_SYN ) {
            if( pInput == &mouseEventBuffer && ( mouseMove[0] || mouseMove[1] || mouseMove[2] ) ) {
                pEvent->type = INPUT_MOUSEMOTION;
                for (int i=0; i<3; i++) {
                    pEvent->motion[i] = mouseMove[i];
                }
                mouseMove[0] = mouseMove[1] = mouseMove[2] = 0;
                return true;
            }
        }
        // mouse button or keypress
        else if( evp->type == EV_KEY ) {
            if( pInput == &mouseEventBuffer ) {
                pEvent->type = INPUT_MOUSEBUTTONUP + evp->value;
                pEvent->code = evp->code;

                uint16_t index = evp->code - BTN_MISC;
                if (index < INPUT_NUM_BUTTONS) {
                    buttonStates[index] = (evp->value != 0);
                }
            }
            else {
                pEvent->type = INPUT_KEYUP + evp->value;
                pEvent->code = evp->code;

                uint16_t index = evp->code;
                if( index < INPUT_NUM_KEYS ) { 
                    keyStates[index] = (evp->value != 0);
                }
            }
            return true;
        }
    }
}

bool InputIsKeyDown(uint16_t keyCode)
{
    return keyStates[keyCode];
}

bool InputIsButtonDown(uint16_t buttonCode)
{
    return buttonStates[buttonCode - BTN_MISC];
}

} // namespace rpi
