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

	void	draw();

	void	show( bool visible = true );
	void	hide();
	bool	isVisible() const;
	
	void	maximize( bool maximized = true );
	void	minimize();
	bool	isMaximized() const;
	
	void	addParam( const std::string &name, bool *boolParam, const std::string &optionsStr = "", bool readOnly = false );
	void	addParam( const std::string &name, float *floatParam, const std::string &optionsStr = "", bool readOnly = false );
	void	addParam( const std::string &name, double *doubleParam, const std::string &optionsStr = "", bool readOnly = false );
	void	addParam( const std::string &name, int32_t *intParam, const std::string &optionsStr = "", bool readOnly = false );
	void	addParam( const std::string &name, Vec3f *vectorParam, const std::string &optionsStr = "", bool readOnly = false );
	void	addParam( const std::string &name, Quatf *quatParam, const std::string &optionsStr = "", bool readOnly = false );
	void	addParam( const std::string &name, Color *quatParam, const std::string &optionsStr = "", bool readOnly = false );
	void	addParam( const std::string &name, ColorA *quatParam, const std::string &optionsStr = "", bool readOnly = false );
	void	addParam( const std::string &name, std::string *strParam, const std::string &optionsStr = "", bool readOnly = false );
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

	std::weak_ptr<app::Window>		mWindow;
	std::shared_ptr<TwBar>			mBar;
	int								mTwWindowId;
	
	std::vector<std::shared_ptr<std::function<void()> > >	mButtonCallbacks;
};

} } // namespace cinder::params
