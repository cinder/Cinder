// MowaLibs
//
// Copyright (c) 2011, Marcin Ignac / marcinignac.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//
// This code is intended to be used with the Cinder C++ library, http://libcinder.org
//
// Temptesta Seven font by Yusuke Kamiyamane http://p.yusukekamiyamane.com/fonts/
// "The fonts can be used free for any personal or commercial projects."

#pragma once

#include <vector>
#include "cinder/Text.h"
#include "cinder/gl/Texture.h"
#include "cinder/app/AppCocoaTouch.h"
#include "cinder/System.h"
#include <vector>



using namespace ci;
using namespace ci::app;
using namespace std;
namespace mowa { namespace sgui {
	
    //-----------------------------------------------------------------------------
	
    class Control;
    class FloatVarControl;
    class IntVarControl;
    class BoolVarControl;
    class ButtonControl;
    class LabelControl;
    class SeparatorControl;
    class ColumnControl;
    class PanelControl;
    class TextureVarControl;
    class ColorVarControl;
    class SwitcherVarControl;
	
    //-----------------------------------------------------------------------------
    
    class SimpleGUI {
    private:
        bool enabled;
        Vec2f	mousePos;
        std::vector<Control*> controls;
        Control* selectedControl;
        
        CallbackId	cbMouseDown;
        CallbackId	cbMouseUp;
        CallbackId  cbMouseDrag;
        
        CallbackId	cbTouchesBegan;
        CallbackId	cbTouchesEnded;
        CallbackId  cbTouchesMoved;
        
        void	init(AppCocoaTouch* app);	
    public:
        static ColorA darkColor;
        static ColorA lightColor;
        static ColorA bgColor;
        static ColorA textColor;
        static float spacing;
        static Vec2f padding;
        static Vec2f sliderSize;
        static Vec2f labelSize;
        static Vec2f separatorSize;
        static Font textFont;
        
        enum {
            RGB,
            HSV
        };
    public:
        SimpleGUI(AppCocoaTouch* app);
        bool	isSelected() { return selectedControl != NULL; }
        std::vector<Control*>& getControls() { return controls; }	
        
        bool	onMouseDown(MouseEvent event);
        bool	onMouseUp(MouseEvent event);
        bool	onMouseDrag(MouseEvent event);
        
        bool	onTouchesBegan(TouchEvent event);
        bool	onTouchesEnded(TouchEvent event);
        bool	onTouchesMoved(TouchEvent event);
        
        void	draw();
        void	dump();
        void	save(std::string fileName = "");
        void	load(std::string fileName = "");
        
        bool	isEnabled();
        void	setEnabled(bool state);
        
        void	setGuiValue(float width, float height, bool isRetina);
        
        FloatVarControl* 	addParam(const std::string& paramName, float* var, float min=0, float max=1, float defaultValue = 0);
        IntVarControl*		addParam(const std::string& paramName, int* var, int min=0, int max=1, int defaultValue = 0);
        BoolVarControl*		addParam(const std::string& paramName, bool* var, bool defaultValue = false, int groupId = -1);
        ColorVarControl*	addParam(const std::string& paramName, ColorA* var, ColorA const defaultValue = ColorA(0, 1, 1, 1), int colorModel = RGB);
        TextureVarControl*	addParam(const std::string& paramName, gl::Texture* var, int scale = 1, bool flipVert = false);
        
        ButtonControl*		addButton(const std::string& buttonName);
        LabelControl*		addLabel(const std::string& labelName);	
        SeparatorControl*	addSeparator();	
        ColumnControl*		addColumn(int x = 0, int y = 0);	
        PanelControl*		addPanel();
        
        SwitcherVarControl*	addParam(const std::string& Name, bool* var, bool dValue = false, int groupId = -1, bool finished=true );
        
        Control*			getControlByName(const std::string& name);
        
        static Vec2f		getStringSize(const std::string& str);		
        static Rectf		getScaledWidthRectf(Rectf rect, float scale);
    };
	
    //-----------------------------------------------------------------------------
	
	
    class Control {
    public:
        enum Type {
            FLOAT_VAR,
            INT_VAR,
            BOOL_VAR,
            COLOR_VAR,
            TEXTURE_VAR,
            BUTTON,
            LABEL,
            SEPARATOR,
            COLUMN,
            PANEL,
            Switcher_VAR
        };
        
        Vec2f	position;
        Rectf	activeArea;
        ColorA	bgColor;
        Type	type;
        std::string name;
        SimpleGUI* parentGui;
        
        Control();	
        virtual ~Control() {};
        void setBackgroundColor(ColorA color);	
        void notifyUpdateListeners();
        virtual Vec2f draw(Vec2f pos) = 0;
        virtual std::string toString() { return ""; };
        virtual void fromString(std::string& strValue) {};
        virtual void onMouseDown(MouseEvent event) {};
        virtual void onMouseUp(MouseEvent event) {};
        virtual void onMouseDrag(MouseEvent event) {};
        virtual void onTouchesBegan(TouchEvent event) {};
        virtual void onTouchesEnded(TouchEvent event) {};
        virtual void onTouchesMoved(TouchEvent event) {};
    };
	
    //-----------------------------------------------------------------------------
    
    class FloatVarControl : public Control {
    public:	
        float* var;
        float min;
        float max;
    public:
        FloatVarControl(const std::string& name, float* var, float min=0, float max=1, float defaultValue = 0);
        float getNormalizedValue();
        void setNormalizedValue(float value);
        Vec2f draw(Vec2f pos);
        std::string toString();
        void fromString(std::string& strValue);
        void onMouseDown(MouseEvent event);	
        void onMouseDrag(MouseEvent event);
        void onTouchesBegan(TouchEvent event);
        void onTouchesMoved(TouchEvent event);
    };
	
    //-----------------------------------------------------------------------------
	
    class IntVarControl : public Control {
    public:
        int* var;
        int min;
        int max;
    public:
        IntVarControl(const std::string& name, int* var, int min=0, int max=1, int defaultValue = 0);
        float getNormalizedValue();
        void setNormalizedValue(float value);
        Vec2f draw(Vec2f pos);
        std::string toString();	
        void fromString(std::string& strValue);
        void onMouseDown(MouseEvent event);	
        void onMouseDrag(MouseEvent event);
        void onTouchesBegan(TouchEvent event);
        void onTouchesMoved(TouchEvent event);
    };
	
    //-----------------------------------------------------------------------------
    
    class BoolVarControl : public Control {
    public:
        bool* var;
        int groupId;
    public:
        BoolVarControl(const std::string& name, bool* var, bool defaultValue, int groupId);
        Vec2f draw(Vec2f pos);	
        std::string toString();	
        void fromString(std::string& strValue);
        void onMouseDown(MouseEvent event);
        void onTouchesBegan(TouchEvent event);
    };
	
    //-----------------------------------------------------------------------------
    
    class ColorVarControl : public Control {
    public:
        ColorA* var;
        Rectf	activeArea1;
        Rectf	activeArea2;
        Rectf	activeArea3;
        Rectf	activeArea4;	
        int		activeTrack;
        int		colorModel;
    public:
        ColorVarControl(const std::string& name, ColorA* var, ColorA defaultValue, int colorModel);
        Vec2f draw(Vec2f pos);
        std::string toString();	//saved as "r g b a"
        void fromString(std::string& strValue); //expecting "r g b a"
        void onMouseDown(MouseEvent event);	
        void onMouseDrag(MouseEvent event);
        void onTouchesBegan(TouchEvent event);
        void onTouchesMoved(TouchEvent event);
    };
	
    //-----------------------------------------------------------------------------
    
    class ButtonControl : public Control {
    private:
        bool pressed;
        CallbackMgr<bool (MouseEvent)>		callbacksClick;
        CallbackMgr<bool (TouchEvent)>		callbacksTouch;
        
    public:
        ButtonControl(const std::string& name);
        Vec2f draw(Vec2f pos);
        void onMouseDown(MouseEvent event);
        void onMouseUp(MouseEvent event);
        
        void onTouchesBegan(TouchEvent event);
        void onTouchesEnded(TouchEvent event);
        
        //! Registers a callback for mouseUp events. Returns a unique identifier which can be used as a parameter to unregisterMouseUp().
        CallbackId		registerClick( std::function<bool (MouseEvent)> callback ) { return callbacksClick.registerCb( callback ); }
        //! Registers a callback for mouseUp events. Returns a unique identifier which can be used as a parameter to unregisterMouseUp().
        template<typename T>
        CallbackId		registerClick( T *obj, bool (T::*callback)(MouseEvent) ) { return callbacksClick.registerCb( std::bind1st( std::mem_fun( callback ), obj ) ); }
        //! Unregisters a callback for mouseUp events.
        void			unregisterClick( CallbackId id ) { callbacksClick.unregisterCb( id ); }
        
        void fireClick();
        
        //----TouchEvent
        
        //! Registers a callback for mouseUp events. Returns a unique identifier which can be used as a parameter to unregisterMouseUp().
        CallbackId		registerTouch( std::function<bool (TouchEvent)> callback ) { return callbacksTouch.registerCb( callback ); }
        //! Registers a callback for mouseUp events. Returns a unique identifier which can be used as a parameter to unregisterMouseUp().
        template<typename T>
        CallbackId		registerTouch( T *obj, bool (T::*callback)(TouchEvent) ) { return callbacksTouch.registerCb( std::bind1st( std::mem_fun( callback ), obj ) ); }
        //! Unregisters a callback for mouseUp events.
        void			unregisterTouch( CallbackId id ) { callbacksTouch.unregisterCb( id ); }
        
        
        void fireTouch();
        
        
    };
    
    //-----------------------------------------------------------------------------
	
    class LabelControl : public Control {
    public:
        LabelControl(const std::string& name);
        void setText(const std::string& text);
        Vec2f draw(Vec2f pos);	
    };
	
    //-----------------------------------------------------------------------------
	
    class SeparatorControl : public Control {
    public:
        SeparatorControl();
        Vec2f draw(Vec2f pos);	
    };
    
    //-----------------------------------------------------------------------------
    
    class ColumnControl : public Control {
    public:
        int x;
        int y;
        ColumnControl(int x = 0, int y = 0);
        Vec2f draw(Vec2f pos);	
    }; 
	
    //-----------------------------------------------------------------------------
    
    class PanelControl : public Control {
    public:
        bool enabled;
        PanelControl();
        Vec2f draw(Vec2f pos);
    };
	
    
    //-----------------------------------------------------------------------------
    
    class TextureVarControl : public Control {
    public:
        gl::Texture* var;
        float scale;
        bool flipVert;	
        TextureVarControl(const std::string& name, gl::Texture* var, int scale, bool flipVert = false);
        Vec2f draw(Vec2f pos);	
    };
    
    //-----------------------------------------------------------------------------
    
    class SwitcherVarControl : public Control {
    public:
        bool* var;
        int groupId;
        bool finished;
    public:
        SwitcherVarControl(const std::string& name, bool* var, bool defaultValue, int groupId, bool finished);
        Vec2f draw(Vec2f pos);	
        std::string toString();	
        void fromString(std::string& strValue);
        void onMouseDown(MouseEvent event);
        void onTouchesBegan(TouchEvent event);
    };
    
    //-----------------------------------------------------------------------------
} //namespace sgui
} //namespace vrg