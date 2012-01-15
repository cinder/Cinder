//
//  ciConfigGuiBase.h
//
//  Created by Roger Sodre on 08/04/2010
//  Copyright 2011 Studio Avante. All rights reserved.
//
#pragma once

#include "cinder/app/App.h"
#include "cinder/Color.h"
#include "cinder/Text.h"
#include "cinder/gl/gl.h"

#include "ciConfig.h"

namespace cinder {
	
	//
	// GUI for Cinder
	// 
	// Use Cinder's AntTweakBar
	// http://www.antisphere.com/Wiki/tools:anttweakbar
	// http://www.antisphere.com/Wiki/tools:anttweakbar:twbarparamsyntax?s=position#valueswidth
	// http://www.antisphere.com/Wiki/tools:anttweakbar:examples
	//
	
	
	//
	// Config Clas
	class ciConfigGuiBase : public ciConfig {
	public:
		ciConfigGuiBase();
		~ciConfigGuiBase();

		void draw();

		//
		// VIRTUALS
		//
		// setters
		virtual void	guiAddGroup( const std::string & _name )	{}
		virtual void	guiAddGroup( const std::string & _name, int x, int y, int w, int h, int wv=0 )	{}
		virtual void*	guiAddPanel( const std::string & _name="")	{ return NULL; }		// Switchable panel
		virtual void	guiAddSeparator()							{}
		virtual void*	guiAddText( std::string text )				{ return NULL; }
		virtual void	guiAddSaveButton(bool ask=false)			{}
		virtual void	guiAddLoadButton(bool ask=false)			{}
		virtual void	guiUpdateValueLabels( int id )				{}
		virtual void	guiShow( bool s = true )					{}
		virtual void	guiSetAutoDraw( bool b=true )				{ bAutoDraw = b; };
		virtual void	guiSetAutoHide( int s=5 )					{ mAutoHideSeconds = s; };
		virtual void	guiSetOffset( const Vec2f & o )				{ mGuiOffset = o; }
		//virtual CallbackId guiAddButton( const std::string &label, ciConfigGuiBase *obj, bool (ciConfigGuiBase::*callback)(ci::app::MouseEvent) ) { return 0; }
		// Getters
		virtual int		guiGetColumnWidth()					{ return 0; }
		virtual bool	guiIsVisible()						{ return true; };
		virtual Vec2f	guiGetSize()						{ return app::getWindowSize(); };		// For FBO refreshing
		// draw
		virtual bool	guiHasChanged()						{ return true; };		// For FBO refreshing
		virtual void	drawGui()							{}
		
		// Generic params
		virtual void*	guiAddParam(int *val, const std::string &label, bool readOnly=false) { return NULL; }
		virtual void*	guiAddParam(float *val, const std::string &label, int precision, bool readOnly=false) { return NULL;}

		// wrappers
		void	guiShowHide()														{ this->guiShow( !this->guiIsVisible() ); };
		void	guiHide()															{ this->guiShow(false); };
		void	guiSetLabel(int id, const std::string &label)						{ this->guiSetLabel(id, -1, label); }
		void*	guiAddParam(int id, const std::string &label="", int precision=0)	{ return this->guiAddParamPrivate(id, -1, label, precision); }	// -1 = auto
		void*	guiAddParamX(int id, const std::string &label="", int precision=0)	{ return this->guiAddParamPrivate(id,  0, label, precision); }
		void*	guiAddParamY(int id, const std::string &label="", int precision=0)	{ return this->guiAddParamPrivate(id,  1, label, precision); }
		void*	guiAddParamZ(int id, const std::string &label="", int precision=0)	{ return this->guiAddParamPrivate(id,  2, label, precision); }

		
	protected:
		
		// Virtuals
		virtual void*	guiAddParamPrivate(int id, int i, const std::string &label, int precision) { return NULL; }
		virtual void	guiSetLabel(int id, int i, const std::string &label) {}
		
		Vec2f		mGuiOffset;
		int			mGroupCount;
		bool		bAutoDraw;
		int			mAutoHideSeconds;
		
	};
	
	
} // cinder::

