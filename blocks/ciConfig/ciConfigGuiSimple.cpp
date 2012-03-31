//
//  ciConfigGuiSimple.cpp
//
//  Created by Roger Sodre on 08/04/2010
//  Copyright 2011 Studio Avante. All rights reserved.
//

#include "ciConfigGuiSimple.h"
#include <sys/time.h>

#ifdef QB
#include "qbSource.h"
#endif


using namespace cinder;
using namespace cinder::app;
using namespace std;

namespace cinder {
	
	//
	// CUSTOM CONFIG CLASS
	ciConfigGuiSimple::ciConfigGuiSimple() : ciConfigGuiBase()
	{
		mGui = new SimpleGUI( App::get() );
		mGui->lightColor = ColorA(0, 1, 1, 1);
		// Use FBO by default
		//this->guiSetUseFbo();
	}
	ciConfigGuiSimple::~ciConfigGuiSimple()
	{
		delete mGui;
	}

	////////////////////////////////////////////////////////
	//
	// SETTERS
	//
	// Virtual
	void ciConfigGuiSimple::guiAddGroup( const std::string & _name )
	{
		mGui->addColumn();
		if ( _name.length() )
			mGui->addLabel(_name);
		mGroupCount++;
	}
	// Virtual
	void* ciConfigGuiSimple::guiAddPanel( const std::string & _name )
	{
		return (void*) mGui->addPanel(_name);
	}
	// Virtual
	void ciConfigGuiSimple::guiAddSeparator()
	{
		mGui->addSeparator();
	}
	// Virtual
	void* ciConfigGuiSimple::guiAddText( std::string text )
	{
		return mGui->addLabel( text );
	}
	// Virtual
	ButtonControl* ciConfigGuiSimple::guiAddButton(const std::string &label, ciConfigGuiBase *obj, bool (ciConfigGuiBase::*callback)(ci::app::MouseEvent) )
	//template<typename T> CallbackId ciConfigGuiSimple::guiAddButton(const std::string &label, T *obj, bool (T::*callback)(MouseEvent) )
	{
		ButtonControl *c = mGui->addButton(label);
		c->registerClick( obj, callback );
		return c;
	}
	// Private
	void* ciConfigGuiSimple::guiAddParamPrivate( int id, int i, const std::string &label, int precision )
	{
		if ( params[id] == NULL )
			return NULL;
		
		Control *c = NULL;
		
		if (i >= 0)
		{
			c = mGui->addParam(label, this->getPointer(id,i), this->getMin(id,i), this->getMax(id,i), this->get(id,i));
			c->setDisplayValue();
			if (precision >= 0)
				((FloatVarControl*)c)->setPrecision( precision );
		}
		else if (params[id]->valueLabels.size() > 0)
		{
			if ( this->testFlag(id,CFG_FLAG_DROP_DOWN) )
				c = mGui->addParamDropDown(label, this->getPointerInt(id), params[id]->valueLabels );
			else
				c = mGui->addParamList(label, this->getPointerInt(id), params[id]->valueLabels );
		}
		else if (this->isColor(id))
			c = mGui->addParam(label, this->getPointerColor(id), Color( this->getColor(id) ));
		else if (this->isVector(id))
			c = mGui->addParam(label, this->getPointerVector(id), this->getVector(id) );
		else if (this->isBool(id))
			c = mGui->addParam(label, this->getPointerBool(id), this->getBool(id));
		else if (this->isString(id))
			c = mGui->addParam(label, this->getPointerString(id));
		else if (this->isByte(id))
		{
			c = mGui->addParamFlag(label, this->getPointerByte(id), 8, this->getByte(id));
			((IntVarControl*)c)->setDisplayValue();
		}
		else if (this->isInt(id))
		{
			c = mGui->addParam(label, this->getPointerInt(id), (int)this->getMin(id), (int)this->getMax(id), this->getInt(id));
			c->setDisplayValue();
		}
		else
		{
			c = mGui->addParam(label, this->getPointer(id), this->getMin(id), this->getMax(id), this->get(id));
			c->setDisplayValue();
			if (precision >= 0)
				((FloatVarControl*)c)->setPrecision( precision );
		}
		// TODO...
		//options << "key='" << params[id]->vec[i].keySwitch << "' ";
		//options << "keyincr='" << params[id]->vec[i].keyInc << "' ";
		//options << "keydecr='" << params[id]->vec[i].keyDec << "' ";
		
		// readonly
		if ( ! params[id]->editable )
			c->setReadOnly( true );

		// Save reference
		params[id]->guiControl = (void*) c;
		
		return (void*) c;
	}

	
	// Virtual
	void* ciConfigGuiSimple::guiAddParam( int *val, const std::string &label, bool readOnly )
	{
		IntVarControl *c = mGui->addParam(label, val, 0, 1, 0);
		c->setReadOnly( readOnly );
		return (void*) c;
	}
	// Virtual
	void* ciConfigGuiSimple::guiAddParam( float *val, const std::string &label, int precision, bool readOnly )
	{
		FloatVarControl *c = mGui->addParam(label, val, 0, 1, 0);
		c->setPrecision( precision );
		c->setReadOnly( readOnly );
		return (void*) c;
	}
	// Virtual
	void ciConfigGuiSimple::guiSetLabel( int id, int i, const std::string &label )
	{
		// TODO...
	}
	// Virtual
	void ciConfigGuiSimple::guiUpdateValueLabels( int id )
	{
		ListVarControl *c = (ListVarControl*) (params[id]->guiControl);
		if (c)
			c->update( params[id]->valueLabels );
	}

#ifdef QB
	void ciConfigGuiSimple::guiAddParam( qb::qbSourceSelector * src, const std::string &label, int cfgIdSelector, int cfgIdTrigger )
	{
		mGui->addParam(label, this->getPointerInt(cfgIdSelector), (int)this->getMin(cfgIdSelector), (int)this->getMax(cfgIdSelector), this->getInt(cfgIdSelector));
		
		TextureVarControl* tc = (TextureVarControl*) mGui->addParam("", src->getTexturePointer());
		tc->refreshRate = 0.1;
		
		mGui->addParam( "", src->getNamePointer() );
		mGui->addParam( "", src->getDescPointer() );

		// Triggers
		src->useConfigSelector( cfgIdSelector, this );
		if ( cfgIdTrigger >= 0 )
			src->useConfigTrigger( cfgIdTrigger, this );
	}
#endif

	
	////////////////////////////////////////////////////////
	//
	// DRAW
	//
	// Virtual
	void ciConfigGuiSimple::drawGui()
	{
		mGui->draw();
	}
	
} // cinder::


