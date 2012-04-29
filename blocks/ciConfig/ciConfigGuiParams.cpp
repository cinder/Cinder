//
//  ciConfigGuiParams.cpp
//
//  Created by Roger Sodre on 08/04/2010
//  Copyright 2011 Studio Avante. All rights reserved.
//

#include "ciConfigGuiParams.h"
#include <sys/time.h>

using namespace cinder;
using namespace ci::app;
using namespace std;

namespace cinder {
	
	//
	// CUSTOM CONFIG CLASS
	ciConfigGuiParams::ciConfigGuiParams() : ciConfigGuiBase()
	{
		bShouldSetFont = false;
		mFontsize = 2;	// default
		mCinderParam = NULL;
	}
	ciConfigGuiParams::~ciConfigGuiParams()
	{
		for (int n = 0 ; n < mParams.size() ; n++)
			delete mParams[n];
		mParams.clear();
	}
	
	
	////////////////////////////////////////////////////////
	//
	// SETTERS
	//
	// Virtual
	void ciConfigGuiParams::guiAddGroup( const std::string & _name )
	{
		int w = this->guiGetColumnWidth();
		int h = getWindowHeight();
		int x = ( mGroupCount * w );
		int y = 0;
		this->guiAddGroup( _name, x, y, w, h );
	}
	void ciConfigGuiParams::guiAddGroup( const std::string & _name, int x, int y, int w, int h, int wv )
	{
		static stringstream options;
		options.str("");
		
		options << " position='" << x << " " << y << "' " ;
		options << " refresh=0.1 " ;	// neeeded to update from watchers
		if (wv)
			options << " valueswidth='" << wv << "' " ;
		
		mCinderParam = new params::InterfaceGl( _name, Vec2i( w, h ) );
		mCinderParam->setOptions("", options.str());
		mParams.push_back( mCinderParam );
		
		mGroupCount++;
	}
	// Virtual
	void ciConfigGuiParams::guiAddSeparator()
	{
		mCinderParam->addSeparator();
	}
	// Virtual
	void* ciConfigGuiParams::guiAddText( std::string text )
	{
		mCinderParam->addText( text );
		return NULL;
	}
	// Private
	void* ciConfigGuiParams::guiAddParamPrivate( int id, int i, const std::string &label, int precision )
	{
		if ( params[id] == NULL )
			return NULL;
		
		string options = this->getOptionsString(id, i, label, precision);
		printf("PARAM OPTIONS %s [%s]\n",this->getParamName(id, i).c_str(),options.c_str());
		
		if (i >= 0)
			mCinderParam->addParam( this->getParamName(id, i) , this->getPointer(id,i), options );
		else if (params[id]->valueLabels.size() > 0)
			mCinderParam->addParam( this->getParamName(id, i) , params[id]->valueLabels, this->getPointerInt(id), options );
		else if (this->isColor(id))
			mCinderParam->addParam( this->getParamName(id, i) , this->getPointerColor(id), options );
		else if (this->isVector(id))
			mCinderParam->addParam( this->getParamName(id, i) , this->getPointerVector(id), options );
		else if (this->isBool(id))
			mCinderParam->addParam( this->getParamName(id, i) , this->getPointerBool(id), options );
		else if (this->isInt(id))
			mCinderParam->addParam( this->getParamName(id, i) , this->getPointerInt(id), options );
		else
			mCinderParam->addParam( this->getParamName(id, i) , this->getPointer(id), options );
		
		return NULL;
	}
	// Virtual
	void* ciConfigGuiParams::guiAddParam( int *val, const std::string &label, bool readOnly )
	{
		std::string options = ( readOnly ? "readonly=true" : "" );
		mCinderParam->addParam( label,	val, options);
		return NULL;
	}
	// Virtual
	void* ciConfigGuiParams::guiAddParam( float *val, const std::string &label, int precision, bool readOnly )
	{
		std::stringstream os;
		os << "precision=" << precision;
		if (readOnly)
			os << " readonly=true";
		mCinderParam->addParam( label,	val, os.str());
		return NULL;
	}
	// Virtual
	void ciConfigGuiParams::guiSetLabel( int id, int i, const std::string &label )
	{
		stringstream options;
		options << "label='" << label << "'" ;
		mCinderParam->setOptions( this->getParamName(id, i), options.str() );
	}
	// Virtual
	void ciConfigGuiParams::guiUpdateValueLabels( int id )
	{
		if (params[id] == NULL)
			return;
		if (params[id]->valueLabels.size() == 0)
			return;
		stringstream options;
		options.str("");
		options << " enum='";
		std::map<int,std::string>::const_iterator it;
		for ( it=params[id]->valueLabels.begin() ; it != params[id]->valueLabels.end(); it++ ) {
			if (it != params[id]->valueLabels.begin())
				options << ",";
			options << " " << (int)(it->first) << " {" << (string)(it->second) << "}";
		}
		options << "' ";
		printf("PARAM LABELS [%s]\n",options.str().c_str());
		mCinderParam->setOptions(this->getName(id), options.str());
	}
	
	////////////////////////////////////////////////////////
	//
	// DRAW
	//
	// Virtual
	void ciConfigGuiParams::drawGui()
	{
		params::InterfaceGl::draw();
		if ( bShouldSetFont )
		{
			mCinderParam->setFontSize( mFontsize );
			bShouldSetFont = false;
			
			//TwWindowSize( getWindowWidth()(), getWindowHeight() );
			
			//std::ostringstream options;
			//options << "size='300 " << toString(getWindowHeight()) << "'";
			//this->setCinderOptions( options.str() );
		}
	}
	
	
	//////////////////////////////////////////////////////////////////////
	//
	// AntTweakBar Methods
	//
	string ciConfigGuiParams::getParamName(int id, int i)
	{
		if (i >= 0)
		{
			std::ostringstream os;
			os << this->getName(id) << "_" << i;
			return os.str();
		}
		else
			return this->getName(id);
	}
	
	//
	// Params options
	// "min=0.1 max=20.5 step=0.5 keyIncr=z keyDecr=Z key=SPACE"
	// "label=`This is a label without a parameter.`"
	// http://www.antisphere.com/Wiki/tools:anttweakbar:twbarparamsyntax
	// http://www.antisphere.com/Wiki/tools:anttweakbar:varparamsyntax
	string ciConfigGuiParams::getOptionsString( int id, int i, const std::string &label, int precision )
	{
		static stringstream options;
		options.str("");
		
		if (precision == 0)
			precision = ( this->isInt(id) ? 0 : 1 );
		
		// Range
		if ( (!this->isBool(id) && !this->isColor(id) && !this->isVector(id) && params[id]->valueLabels.size() == 0 )
			|| i >= 0 )
		{
			options 
			<< "min='" << this->getMin(id) << "' "
			<< "max='" << this->getMax(id) << "' "
			// Step
			<< "step='";
			{
				if ( precision > 0 )
				{
					options << "0.";
					for (int n = 0 ; n < (precision-1) ; n++)
						options << "0";
				}
				options << "1";
			}
			options << "' ";
		}
		
		// Default i
		if (i < 0)
			i = 0;
		
		// Keys
		if ( strlen(params[id]->vec[i].keySwitch) )
			options << "key='" << params[id]->vec[i].keySwitch << "' ";
		if ( strlen(params[id]->vec[i].keyInc) )
			options << "keyincr='" << params[id]->vec[i].keyInc << "' ";
		if ( strlen(params[id]->vec[i].keyDec) )
			options << "keydecr='" << params[id]->vec[i].keyDec << "' ";
		
		// Label
		if (label.length())
			options  << "label='" << label << "' ";
		
		return options.str();
	}
	
	
} // cinder::


