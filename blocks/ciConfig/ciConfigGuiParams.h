//
//  ciConfigGuiParams.h
//
//  Created by Roger Sodre on 08/04/2010
//  Copyright 2010 Studio Avante. All rights reserved.
//
#pragma once

#include "cinder/params/Params.h"
#include "ciConfigGuiBase.h"

#define GUI_WIDTH_FONT_1		220
#define GUI_WIDTH_FONT_2		250
#define GUI_WIDTH_FONT_3		300

namespace cinder {
	
	//
	// GUI for Cinder based on AntTweakBar
	// 
	// Use Cinder's AntTweakBar
	// http://www.antisphere.com/Wiki/tools:anttweakbar
	// http://www.antisphere.com/Wiki/tools:anttweakbar:twbarparamsyntax?s=position#valueswidth
	// http://www.antisphere.com/Wiki/tools:anttweakbar:examples
	//
	
	
	//
	// Config Clas
	class ciConfigGuiParams : public ciConfigGuiBase {
	public:
		using ciConfigGuiBase::guiAddParam;		// for virtual polyphormism

		ciConfigGuiParams();
		~ciConfigGuiParams();
		
		// Virtuals
		int		guiGetColumnWidth()	{ return ( mFontsize == 1 ? GUI_WIDTH_FONT_1 : ( mFontsize == 2 ? GUI_WIDTH_FONT_2 : GUI_WIDTH_FONT_3 ) ); }
		void	guiAddGroup( std::string _name );
		void	guiAddGroup( std::string _name, int x, int y, int w, int h, int wv=0 );
		void	guiAddSeparator();
		void*	guiAddText( std::string text );
		void	guiUpdateValueLabels(int id);
		void	guiShow( bool b = true )				{ mCinderParam->show(b); };
		bool	guiIsVisible()							{ return mCinderParam->isVisible(); };
		void	drawGui();
		
		// Generic params
		void*	guiAddParam(int *val, const std::string &label, bool readOnly=false);
		void*	guiAddParam(float *val, const std::string &label, int precision, bool readOnly=false);

		// AntTweakBar methods
		std::string getParamName( int id, int i );
		void	setCinderOptions( std::string options )	{ mCinderParam->setOptions(options); }
		void	setFontSize( int s )						{ mFontsize=s; bShouldSetFont=true; }
		
		
	protected:
		
		// Virtuals
		void*	guiAddParamPrivate(int id, int i, const std::string &label, int precision);
		void	guiSetLabel(int id, int i, const std::string &label);
		
		// AntTweakBar Methods
		std::string getOptionsString(int id, int i, const std::string &label, int precision=0);
		
		// AntTweakBar Params
		std::vector<params::InterfaceGl*>	mParams;			// AntTweakBar panels
		params::InterfaceGl					*mCinderParam;		// Current panel
		bool	bShouldSetFont;		// AntTweakBar can set font only after all set
		int		mFontsize;			// AntTweakBar font size
		
		
	};
	
	
} // cinder::

