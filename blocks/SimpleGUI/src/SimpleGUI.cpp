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

#include <iostream>
#include <sstream>
#include <fstream>
#include <iosfwd>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "SimpleGUI.h"
#include "cinder/Utilities.h"
#include "cinder/Font.h"
#include "cinder/CinderMath.h"

namespace mowa { namespace sgui {
	
//-----------------------------------------------------------------------------

Font SimpleGUI::textFont = Font();
ColorA SimpleGUI::darkColor = ColorA(0.3, 0.3, 0.3, 1);
ColorA SimpleGUI::lightColor = ColorA(1, 1, 1, 1);
ColorA SimpleGUI::bgColor = ColorA(0, 0, 0, 0.5);
ColorA SimpleGUI::textColor = ColorA(1,1,1,1);	
Vec2f SimpleGUI::spacing = Vec2f(2, 0);
Vec2f SimpleGUI::padding = Vec2f(3, 3);
Vec2f SimpleGUI::sliderSize = Vec2f(125, 10);
Vec2f SimpleGUI::labelSize = Vec2f(125, 10);
Vec2f SimpleGUI::separatorSize = Vec2f(125, 3);
	
SimpleGUI::SimpleGUI(App* app) {
	init(app);
	enabled = true;
}
	// ROGER
	SimpleGUI::~SimpleGUI() {
		for (int i = 0 ; i < controls.size() ; i++)
			delete controls[i];
		controls.clear();
	}
	
void SimpleGUI::init(App* app) {	
	textFont = Font(loadResource("pf_tempesta_seven.ttf"), 8);
	//textFont = Font("Arial", 12);
	selectedControl = NULL;
	cbMouseDown = app->registerMouseDown( this, &SimpleGUI::onMouseDown );
	cbMouseUp = app->registerMouseUp( this, &SimpleGUI::onMouseUp );	
	cbMouseDrag = app->registerMouseDrag( this, &SimpleGUI::onMouseDrag );
}

FloatVarControl* SimpleGUI::addParam(const std::string& paramName, float* var, float min, float max, float defaultValue) {
	FloatVarControl* control = new FloatVarControl(paramName, var, min, max, defaultValue);
	control->parentGui = this;
	controls.push_back(control);
	return control;
}

IntVarControl* SimpleGUI::addParam(const std::string& paramName, int* var, int min, int max, int defaultValue) {
	IntVarControl* control = new IntVarControl(paramName, var, min, max, defaultValue);
	control->parentGui = this;	
	controls.push_back(control);
	return control;
}

BoolVarControl* SimpleGUI::addParam(const std::string& paramName, bool* var, bool defaultValue, int groupId) {
	BoolVarControl* control = new BoolVarControl(paramName, var, defaultValue, groupId);
	control->parentGui = this;	
	controls.push_back(control);
	return control;
}
	
	// ROGER
	ColorVarControl* SimpleGUI::addParam(const std::string& paramName, Color* var, Color defaultValue, int colorModel) {
		ColorVarControl* control = new ColorVarControl(paramName, var, defaultValue, colorModel);
		control->parentGui = this;	
		controls.push_back(control);
		return control;
	}
	
ColorVarControl* SimpleGUI::addParam(const std::string& paramName, ColorA* var, ColorA defaultValue, int colorModel) {
	ColorVarControl* control = new ColorVarControl(paramName, var, defaultValue, colorModel);
	control->parentGui = this;	
	controls.push_back(control);
	return control;
}

TextureVarControl* SimpleGUI::addParam(const std::string& paramName, gl::Texture* var, float scale, bool flipVert) {
	TextureVarControl* control = new TextureVarControl(paramName, var, scale, flipVert);
	control->parentGui = this;	
	controls.push_back(control);
	return control;
}

	// ROGER
	ListVarControl*	SimpleGUI::addParam( const std::string &paramName, int* var, const std::map<int,std::string> &valueLabels, int defaultValue ) {
		ListVarControl* control = new ListVarControl(paramName, var, valueLabels, defaultValue);
		control->parentGui = this;	
		controls.push_back(control);
		return control;
	}
	
ButtonControl* SimpleGUI::addButton(const std::string& buttonName) {
	ButtonControl* control = new ButtonControl(buttonName);
	control->parentGui = this;
	controls.push_back(control);
	return control;
}
	
LabelControl* SimpleGUI::addLabel(const std::string& labelName) {
	LabelControl* control = new LabelControl(labelName);
	control->parentGui = this;	
	controls.push_back(control);
	return control;
}

SeparatorControl* SimpleGUI::addSeparator() {
	SeparatorControl* control = new SeparatorControl();
	control->parentGui = this;	
	controls.push_back(control);
	return control;
}
	
ColumnControl* SimpleGUI::addColumn(int x, int y) {
	ColumnControl* control = new ColumnControl(x, y);
	control->parentGui = this;	
	controls.push_back(control);
	return control;
}

PanelControl* SimpleGUI::addPanel() {
	PanelControl* control = new PanelControl();
	control->parentGui = this;	
	controls.push_back(control);
	return control;
}
	
void SimpleGUI::draw() {	
	if (!enabled) return;
	
	mSize = Vec2f();

	gl::pushMatrices();
	gl::setMatricesWindow(getWindowSize());
	gl::disableDepthRead();	
	gl::disableDepthWrite();		
	gl::enableAlphaBlending();

	Vec2f position = spacing + mOffset;
	
	ColumnControl* currColumn = NULL;
	PanelControl* currPanel = NULL;
	
	std::vector<Control*>::iterator it = controls.begin();
	while(it != controls.end()) {
		Control* control = *it++;
		if (control->type == Control::PANEL) {
			currPanel = (PanelControl*)control;			
		}		
		if (control->type == Control::COLUMN) {
			if (currColumn == NULL) { //first column				
				//each column moves everything to the right so we want compensate that 
				position.x -= SimpleGUI::labelSize.x;
			}
			currColumn = (ColumnControl*)control;
			currPanel = NULL;
		}
		if (currPanel != NULL && !currPanel->enabled) {
			continue;
		}
		position = control->draw(position);
		
		// ROGER
		if ( position.y > mSize.y )
			mSize.y = position.y;
		if ( position.x > mSize.x )
			mSize.x = position.x;
	}
	
	// ROGER
	// Finish size by adding the same a column adds
	mSize.x += SimpleGUI::labelSize.x + SimpleGUI::spacing.x + SimpleGUI::padding.x*2;


	gl::disableAlphaBlending();
	gl::enableDepthRead();
	gl::enableDepthWrite();
	gl::color(ColorA(1,1,1,1));
	gl::popMatrices();
}
	
bool SimpleGUI::isEnabled() {
	return enabled;
}

void SimpleGUI::setEnabled(bool state) {
	enabled = state;
}

void SimpleGUI::dump() {
	std::vector<Control*>::iterator it = controls.begin();
	while(it != controls.end()) {
		Control* control = *it++;		
		std::string str = control->toString();
		std::cout << control->name << " : " + str << std::endl;
	}
}
	
void SimpleGUI::save(std::string fileName) {
	console() << "SimpleGUI::save " << fileName << std::endl;
	std::vector<Control*>::iterator it = controls.begin();
	std::fstream file_op(fileName.c_str(), std::ios::out);	
	while(it != controls.end()) {
		Control* control = *it++;		
		std::string str = control->toString();
		file_op << control->name << " : " + str << std::endl;
	}
	file_op.close();
}

void SimpleGUI::load(std::string fileName) {
	console() << "SimpleGUI::load " << fileName << std::endl;
	std::fstream file_op(fileName.c_str(), std::ios::in);
	char str[2000];
	if ((file_op.rdstate() & std::ifstream::failbit ) != 0) {
		console() << "SimpleGUI::load " << "failed" << std::endl;
		return;
	}
	while(!file_op.eof()) {
		file_op.getline(str, 2000);
		std::string line(str);
		size_t colonPos = line.find(":");
		
		if (colonPos == std::string::npos) {
			continue;
		}
		
		std::string name = line.substr(0, colonPos);
		std::string value = line.substr(colonPos + 1);
		
		boost::trim(name);
		boost::trim(value);
		
		Control* control = getControlByName(name);
		if (control) {
			control->fromString(value);
		}		
	}    
	file_op.close();
}
	

bool SimpleGUI::onMouseDown(MouseEvent event) {
	if (!enabled) return false;
	
	// ROGER - ignore disabled panels
	PanelControl* currPanel = NULL;

	std::vector<Control*>::iterator it = controls.begin();	
	while(it != controls.end()) {
		Control* control = *it++;
		// ROGER - ignore disabled panels
		if (control->type == Control::PANEL) {
			currPanel = (PanelControl*)control;
		}
		if (control->type == Control::COLUMN) {
			currPanel = NULL;
		}
		if (currPanel != NULL && !currPanel->enabled) {
			continue;
		}
		
		// pass on mouse event
		if (control->activeArea.contains(event.getPos())) {
			selectedControl = control;
			selectedControl->onMouseDown(event);
			return true;
		}
	}
	return false;
}

bool SimpleGUI::onMouseUp(MouseEvent event) {
	if (!enabled) return false;
	
	if (selectedControl != NULL) {
		selectedControl->onMouseUp(event);
		selectedControl = NULL;
		return true;
	}	
	return false;
}
	
bool SimpleGUI::onMouseDrag(MouseEvent event) {
	if (!enabled) return false;
	
	mousePos = event.getPos();
	
	if (selectedControl) {
		selectedControl->onMouseDrag(event);
		return true;
	}
	return false;
}
	
Vec2f SimpleGUI::getStringSize(const std::string& str) {
	TextLayout text;
	text.addLine(str);
	Surface s = text.render(true);
	return s.getSize();
}
	
Rectf SimpleGUI::getScaledWidthRectf(Rectf rect, float scale) {
	float w = rect.getWidth() * scale;
	return Rectf(
		rect.getX1(),
		rect.getY1(),
		rect.getX1() + ( w ? w : 1.0 ),
		rect.getY1() + rect.getHeight()
	);
}
	
	
	//
	// ROGER
	//
	
	Control* SimpleGUI::getControlByName(const std::string& name) {
		std::vector<Control*>::iterator it = controls.begin();	
		while(it != controls.end()) {
			Control* control = *it++;
			if (control->name == name) {
				return control;
			}		
		}
		return NULL;
	}
	bool SimpleGUI::hasChanged()
	{
		std::vector<Control*>::iterator it = controls.begin();	
		while(it != controls.end()) {
			Control* control = *it++;
			if ( control->hasChanged() ) {
				return true;
			}
		}
		return false;
	}
	
	
//-----------------------------------------------------------------------------
	
Control::Control() {
	bgColor = ColorA(0,0,0,0.5);
}

void Control::setBackgroundColor(ColorA color) {
	bgColor = color;
}
	
//-----------------------------------------------------------------------------
	
FloatVarControl::FloatVarControl(const std::string& name, float* var, float min, float max, float defaultValue) {
	this->type = Control::FLOAT_VAR;
	this->name = name;
	this->var = var;
	this->min = min;
	this->max = max;
	this->precision = ( (max-min) <= 1.0 ? 2 : 1 );
	if (defaultValue < min) {
		*var = min;
	}
	else if (defaultValue > max) {
		*var = max;
	}
	else {
		*var = defaultValue;
	}
	// ROGER
	this->lastValue = *var;
	this->readOnly = false;
	this->displayValue = false;
	this->update();
}

float FloatVarControl::getNormalizedValue() {
	return (*var - min)/(max - min);
}
	
void FloatVarControl::setNormalizedValue(float value) {
	float newValue = min + value*(max - min);
	if (newValue != *var) {
		*var = newValue;
	}
}

	// ROGER
	void FloatVarControl::setReadOnly(bool b)
	{
		readOnly=b;
		if (b)
			displayValue=true;
		this->update();
	}
	void FloatVarControl::update()
	{
		activeAreaBase = Rectf(0, 
							   SimpleGUI::labelSize.y + SimpleGUI::padding.y, 
							   SimpleGUI::sliderSize.x, 
							   SimpleGUI::labelSize.y + ( !readOnly ? SimpleGUI::padding.y + SimpleGUI::sliderSize.y : 0 ) );
		backArea = Rectf((-SimpleGUI::padding).x, 
						 (-SimpleGUI::padding).y, 
						 (SimpleGUI::sliderSize + SimpleGUI::padding).x, 
						 (SimpleGUI::labelSize + SimpleGUI::padding + ( !readOnly ? SimpleGUI::sliderSize + SimpleGUI::padding : Vec2f::zero() ) ).y );	
	}
	
Vec2f FloatVarControl::draw(Vec2f pos) {
	this->lastValue = *var;

	activeArea = activeAreaBase + pos;
	
	gl::color(SimpleGUI::bgColor);
	gl::drawSolidRect( backArea + pos );
	
	gl::drawString(name, pos, SimpleGUI::textColor, SimpleGUI::textFont);
	// ROGER
	if (displayValue)
	{
		std::ostringstream ss;
		ss.setf(std::ios::fixed);
		ss.precision(this->precision);
		ss << *var;
		gl::drawStringRight(ss.str(), pos+Vec2f(SimpleGUI::sliderSize.x,0), SimpleGUI::textColor, SimpleGUI::textFont);
	}
	
	if (!readOnly)
	{
		gl::disableAlphaBlending();
		gl::color(SimpleGUI::darkColor);
		gl::drawSolidRect(activeArea);
		gl::color(SimpleGUI::lightColor);
		gl::drawSolidRect(SimpleGUI::getScaledWidthRectf(activeArea, getNormalizedValue()));
		gl::enableAlphaBlending();
	}
	
	pos.y += backArea.getHeight() + SimpleGUI::spacing.y;	
	return pos;
}
	
std::string FloatVarControl::toString() {
	std::stringstream ss;
	ss << *var;
	return ss.str();
}
	
void FloatVarControl::fromString(std::string& strValue) {
	*var = boost::lexical_cast<float>(strValue);
}
	
void FloatVarControl::onMouseDown(MouseEvent event) {
	onMouseDrag(event);	
}

void FloatVarControl::onMouseDrag(MouseEvent event) {
	float value = (event.getPos().x - activeArea.x1)/(activeArea.x2 - activeArea.x1);
	value = math<float>::max(0.0, math<float>::min(value, 1.0));	
	setNormalizedValue(value);
}
	
//-----------------------------------------------------------------------------
	
IntVarControl::IntVarControl(const std::string& name, int* var, int min, int max, int defaultValue) {
	this->type = Control::INT_VAR;
	this->name = name;
	this->var = var;
	this->min = min;
	this->max = max;
	this->step = 1;
	this->readOnly = false;
	this->displayValue = false;
	if (defaultValue < min) {
		*var = min;
	}
	else if (defaultValue > max) {
		*var = max;
	}
	else {
		*var = defaultValue;
	}
	// ROGER
	this->lastValue = *var;
	this->readOnly = false;
	this->displayValue = false;
	this->update();
}
	
float IntVarControl::getNormalizedValue() {
		return (*var - min)/(float)(max - min);
}

void IntVarControl::setNormalizedValue(float value) {
	if (!readOnly)
	{
		int newValue = min + value*(max - min);
		if (step > 1)
			newValue -= ( newValue % step);
		if (newValue != *var) {
			*var = newValue;
		}
	}
}
	
	// ROGER
	void IntVarControl::setStep(int s) {
		step = s;
		if (step > 1)
			*var -= ( *var % step);
	}
	void IntVarControl::setReadOnly(bool b)
	{
		readOnly=b;
		if (b)
			displayValue=true;
		this->update();
	}
	void IntVarControl::update()
	{
		activeAreaBase = Rectf(0, 
							   SimpleGUI::labelSize.y + SimpleGUI::padding.y, 
							   SimpleGUI::sliderSize.x, 
							   SimpleGUI::labelSize.y + ( !readOnly ? SimpleGUI::padding.y + SimpleGUI::sliderSize.y : 0 ) );
		backArea = Rectf((-SimpleGUI::padding).x, 
						 (-SimpleGUI::padding).y, 
						 (SimpleGUI::sliderSize + SimpleGUI::padding).x, 
						 (SimpleGUI::labelSize + SimpleGUI::padding + ( !readOnly ? SimpleGUI::sliderSize + SimpleGUI::padding : Vec2f::zero() ) ).y );
	}
	
	
Vec2f IntVarControl::draw(Vec2f pos) {
	this->lastValue = *var;

	activeArea = activeAreaBase + pos;
	
	gl::color(SimpleGUI::bgColor);
	gl::drawSolidRect( backArea + pos );

	gl::drawString(name, pos, SimpleGUI::textColor, SimpleGUI::textFont);
	// ROGER
	if (displayValue)
		gl::drawStringRight(this->toString(), pos+Vec2f(SimpleGUI::sliderSize.x,0), SimpleGUI::textColor, SimpleGUI::textFont);

	if (!readOnly)
	{
		gl::disableAlphaBlending();
		gl::color(SimpleGUI::darkColor);
		gl::drawSolidRect(activeArea);
		gl::color(SimpleGUI::lightColor);
		gl::drawSolidRect(SimpleGUI::getScaledWidthRectf(activeArea, getNormalizedValue()));
		gl::enableAlphaBlending();
	}
	
	pos.y += backArea.getHeight() + SimpleGUI::spacing.y;	
	return pos;	
}	
	
std::string IntVarControl::toString() {
	std::stringstream ss;
	ss << *var;
	return ss.str();	
}

void IntVarControl::fromString(std::string& strValue) {
	*var = boost::lexical_cast<int>(strValue);
}

void IntVarControl::onMouseDown(MouseEvent event) {
	onMouseDrag(event);
}

void IntVarControl::onMouseDrag(MouseEvent event) {
	if (!readOnly)
	{
		float value = (event.getPos().x - activeArea.x1)/(activeArea.x2 - activeArea.x1);
		value = math<float>::max(0.0, math<float>::min(value, 1.0));
		setNormalizedValue(value);
	}
}

//-----------------------------------------------------------------------------

BoolVarControl::BoolVarControl(const std::string& name, bool* var, bool defaultValue, int groupId) {
	this->type = Control::BOOL_VAR;
	this->name = name;
	this->var = var;
	this->groupId = groupId;
	*var = defaultValue;
	// ROGER
	this->lastValue = *var;
	activeAreaBase = Rectf(0, 0, SimpleGUI::sliderSize.y, SimpleGUI::sliderSize.y);
	backArea = Rectf((-SimpleGUI::padding).x,
					 (-SimpleGUI::padding).y,
					 (SimpleGUI::sliderSize + SimpleGUI::padding).x,
					 (SimpleGUI::sliderSize + SimpleGUI::padding).y );
}	

Vec2f BoolVarControl::draw(Vec2f pos) {
	this->lastValue = *var;

	activeArea = activeAreaBase + pos;
	
	gl::color(SimpleGUI::bgColor);
	gl::drawSolidRect(backArea + pos);
	gl::drawString(name, Vec2f(pos.x + SimpleGUI::sliderSize.y + SimpleGUI::padding.x*2, pos.y), SimpleGUI::textColor, SimpleGUI::textFont);					
	
	gl::disableAlphaBlending();
	gl::color((*var) ? SimpleGUI::lightColor : SimpleGUI::darkColor);
	gl::drawSolidRect(activeArea);
	gl::enableAlphaBlending();
	
	pos.y += backArea.getHeight() + SimpleGUI::spacing.y;	
	return pos;
}	
	
std::string BoolVarControl::toString() {
	std::stringstream ss;
	ss << *var;
	return ss.str();	
}

void BoolVarControl::fromString(std::string& strValue) {
	int value = boost::lexical_cast<int>(strValue);	
	*var = value ? true : false;
}
	
void BoolVarControl::onMouseDown(MouseEvent event) {
	if (groupId > -1) {
		for(std::vector<Control*>::iterator it2 = parentGui->getControls().begin(); it2 != parentGui->getControls().end(); it2++) {
			if (((*it2)->type == Control::BOOL_VAR) && (((BoolVarControl*)(*it2))->groupId == this->groupId)) {
				*((BoolVarControl*)(*it2))->var = (*it2 == this);
			}						
		}
	}
	else {
		*this->var = ! *this->var;
	}
}

//-----------------------------------------------------------------------------

	// ROGER
	ColorVarControl::ColorVarControl(const std::string& name, Color* var, Color defaultValue, int colorModel) {
		this->type = Control::COLOR_VAR;
		this->name = name;
		this->var = var;
		this->varA = NULL;
		this->colorModel = colorModel;
		*var = defaultValue;
		activeTrack = 0;
		// ROGER
		this->lastValue = *var;
		alphaControl = false;
		this->update();
	}
	
ColorVarControl::ColorVarControl(const std::string& name, ColorA* var, ColorA defaultValue, int colorModel) {
	this->type = Control::COLOR_VAR;
	this->name = name;
	this->var = NULL;
	this->varA = var;
	this->colorModel = colorModel;
	*var = defaultValue;
	activeTrack = 0;
	// ROGER
	this->lastValueA = *var;
	alphaControl = true;
	this->update();
}
	
	void ColorVarControl::update() {
		channelCount = (alphaControl?4:3);
		for (int i = 0 ; i < channelCount ; i++)
			activeAreasBase[i] = Rectf(0,
									   SimpleGUI::labelSize.y + SimpleGUI::sliderSize.y*i + SimpleGUI::padding.y*(i+1),
									   SimpleGUI::sliderSize.x,
									   SimpleGUI::sliderSize.y*(i+1) + SimpleGUI::padding.y*(i+1) + SimpleGUI::labelSize.y );
		activeAreaBase = Rectf(
							   0, 
							   SimpleGUI::labelSize.y, 
							   SimpleGUI::sliderSize.x, 
							   SimpleGUI::sliderSize.y*channelCount + SimpleGUI::padding.y*channelCount + SimpleGUI::labelSize.y );
		backArea = Rectf((-SimpleGUI::padding).x,
						 (-SimpleGUI::padding).y,
						 (SimpleGUI::sliderSize + SimpleGUI::padding).x,
						 (SimpleGUI::labelSize + SimpleGUI::sliderSize*channelCount + SimpleGUI::padding*(channelCount+1)).y );
		previewArea = Rectf((SimpleGUI::sliderSize.x * 0.75),
							0,
							SimpleGUI::sliderSize.x,
							SimpleGUI::sliderSize.y );
	}
	
Vec2f ColorVarControl::draw(Vec2f pos) {
	if (alphaControl)
		this->lastValueA = *varA;
	else
		this->lastValue = *var;

	Vec4f values;
	if (colorModel == SimpleGUI::RGB) {
		if (alphaControl)
			values = Vec4f(varA->r, varA->g, varA->b, varA->a);
		else
			values = Vec4f(var->r, var->g, var->b);
	}
	else {
		Vec3f hsv = ( alphaControl ? rgbToHSV(*varA) : rgbToHSV(*var) );
		values.x = hsv.x;
		values.y = hsv.y;
		values.z = hsv.z;
		if (alphaControl)
			values.w = varA->a;
	}

	activeArea = activeAreaBase + pos;
	for (int i = 0 ; i < channelCount ; i++)
		activeAreas[i] = activeAreasBase[i] + pos;

	gl::color(SimpleGUI::bgColor);
	gl::drawSolidRect(backArea+pos);
	
	gl::drawString(name, pos, SimpleGUI::textColor, SimpleGUI::textFont);
	
	// ROGER
	if (alphaControl)
		gl::color( *varA );
	else
		gl::color( *var );
	gl::drawSolidRect(previewArea+pos);

	gl::disableAlphaBlending();
	gl::color(SimpleGUI::darkColor);
	for (int i = 0 ; i < channelCount ; i++)
		gl::drawSolidRect(activeAreas[i]);
	gl::color(SimpleGUI::lightColor);
	for (int i = 0 ; i < channelCount ; i++)
	{
		Rectf r = SimpleGUI::getScaledWidthRectf(activeAreas[i], values[i]);
		gl::drawLine(Vec2f(r.x2, r.y1), Vec2f(r.x2, r.y2));				
	}
	gl::enableAlphaBlending();

	pos.y += backArea.getHeight() + SimpleGUI::spacing.y;	
	return pos;
}
		
std::string ColorVarControl::toString() {
	std::stringstream ss;
	if (alphaControl)
		ss << varA->r << " " << varA->g << " " << varA->b << " " << varA->a;
	else
		ss << var->r << " " << var->g << " " << var->b;
	return ss.str();
}

void ColorVarControl::fromString(std::string& strValue) {
	std::vector<std::string> strs;
	boost::split(strs, strValue, boost::is_any_of("\t "));
	for (int i = 0 ; i < channelCount ; i++)
		if (alphaControl)
			*varA[i] = boost::lexical_cast<double>(strs[i]);
		else
			*var[i] = boost::lexical_cast<double>(strs[i]);
}
	
	
void ColorVarControl::onMouseDown(MouseEvent event) {
	for (int i = 0 ; i < channelCount ; i++)
	{
		if (activeAreas[i].contains(event.getPos())) {
			activeTrack = i+1;
			break;
		}
	}
	onMouseDrag(event);
}
	
void ColorVarControl::onMouseDrag(MouseEvent event) {	
	float value = (event.getPos().x - activeArea.x1)/(activeArea.x2 - activeArea.x1);
	value = math<float>::max(0.0, math<float>::min(value, 1.0));	
	
	if (colorModel == SimpleGUI::RGB) {
		switch (activeTrack) {
			case 1: ( alphaControl ? varA->r : var->r ) = value; break;
			case 2: ( alphaControl ? varA->g : var->g ) = value; break;
			case 3: ( alphaControl ? varA->b : var->b ) = value; break;
			case 4: varA->a = value; break;				
		}
	}
	else {
		Vec3f hsv = ( alphaControl ? rgbToHSV(*varA) : rgbToHSV(*var) );
		switch (activeTrack) {
			case 1: hsv.x = value; break;
			case 2: hsv.y = value; break;
			case 3: hsv.z = value; break;
			case 4: varA->a = value; break;				
		}
		if (alphaControl)
			*varA = ColorA(CM_HSV, hsv.x, hsv.y, hsv.z, varA->a);
		else
			*var = Color(CM_HSV, hsv.x, hsv.y, hsv.z);
	}
}

	//-----------------------------------------------------------------------------
	// ROGER
	
	ListVarControl::ListVarControl(const std::string& name, int* var, const std::map<int,std::string> &valueLabels, int defaultValue) {
		this->type = Control::LIST_VAR;
		this->name = name;
		this->var = var;
		*var = defaultValue;
		this->lastValue = *var;
		this->update( valueLabels );
	}
	
	void ListVarControl::update(const std::map<int,std::string> &valueLabels) {
		for (int i = 0 ; i < items.size() ; i++)
			delete this->items[i];
		this->items.clear();
		
		bool found = false;
		std::map<int,std::string>::const_iterator it = valueLabels.begin();
		for (int i = 0 ; i < valueLabels.size(); i++) {
			listVarItem *_item = new listVarItem();
			_item->key = (int)(it->first);
			_item->label = const_cast<char*>( (it->second).c_str() );;
			items.push_back( _item );
			//std::cout << "LIST [" << this->valueLabels.size() << "] " << key << " = " << val << std::endl;
			
			// selected value still in the list
			if (_item->key == *var)
				found = true;
			// selected value lost: get previous
			else if (!found && _item->key > *var)
			{
				if (it == valueLabels.begin())
					*var = it->first;
				else {
					advance(it, -1);
					*var = it->first;
					advance(it, 1);
				}
				found = true;
			}

			_item->activeAreaBase = Rectf(0,
										 SimpleGUI::labelSize.y + SimpleGUI::sliderSize.y*i + SimpleGUI::padding.y*(i+1),
										 SimpleGUI::sliderSize.x,
										 SimpleGUI::labelSize.y + SimpleGUI::sliderSize.y*(i+1) + SimpleGUI::padding.y*(i+1) );
			advance(it, 1);
		}
		if(!found && valueLabels.size()>0)
			*var = valueLabels.rbegin()->first;
		
		activeAreaBase = Rectf(0,
							   SimpleGUI::labelSize.y,
							   SimpleGUI::sliderSize.x,
							   SimpleGUI::sliderSize.y*items.size() + SimpleGUI::padding.y*items.size() + SimpleGUI::labelSize.y );
		backArea = Rectf((-SimpleGUI::padding).x,
						 (-SimpleGUI::padding).y,
						 (SimpleGUI::sliderSize + SimpleGUI::padding).x,
						 (SimpleGUI::labelSize + SimpleGUI::sliderSize*items.size() + SimpleGUI::padding*(items.size()+1)).y );
	}
	
	Vec2f ListVarControl::draw(Vec2f pos) {
		this->lastValue = *var;

		activeArea = activeAreaBase + pos;
		for (int i = 0 ; i < items.size() ; i++)
			items[i]->activeArea = items[i]->activeAreaBase + pos;
		
		// background
		gl::color(SimpleGUI::bgColor);
		gl::drawSolidRect(backArea+pos);

		// control name
		gl::drawString(name, pos, SimpleGUI::textColor, SimpleGUI::textFont);
		
		gl::disableAlphaBlending();
		for (int i = 0 ; i < items.size() ; i++) {
			bool selected = ( items[i]->key == *var );
			gl::color(selected ? SimpleGUI::lightColor : SimpleGUI::darkColor);
			gl::drawSolidRect(items[i]->activeArea);
		}
		gl::enableAlphaBlending();
		
		for (int i = 0 ; i < items.size() ; i++) {
			bool selected = ( items[i]->key == *var );
			gl::drawString(items[i]->label, Vec2f(pos.x + SimpleGUI::padding.x * 2, items[i]->activeArea.y1 - floor(SimpleGUI::padding.y * 0.5)), selected ? SimpleGUI::darkColor : SimpleGUI::textColor, SimpleGUI::textFont);
		}
		
		pos.y += backArea.getHeight() + SimpleGUI::spacing.y;	
		return pos;
	}
	
	std::string ListVarControl::toString() {
		std::stringstream ss;
		ss << *var;
		return ss.str();
	}
	
	void ListVarControl::fromString(std::string& strValue) {
		*var = boost::lexical_cast<int>(strValue);
	}
	
	void ListVarControl::onMouseDown(MouseEvent event) {
		for (int i = 0 ; i < items.size() ; i++) {
			if (items[i]->activeArea.contains(event.getPos())) {
				*this->var = items[i]->key;
			}
		}
	}
	
//-----------------------------------------------------------------------------	
	
ButtonControl::ButtonControl(const std::string& name) {
	this->type = Control::BUTTON;
	this->name = name;		
	this->pressed = false;
	this->lastPressed = this->pressed;
	this->lastName = name;
	// ROGER
	this->triggerUp = false;
	activeAreaBase = Rectf(0,
						   0,
						   SimpleGUI::sliderSize.x,
						   SimpleGUI::labelSize.y + SimpleGUI::padding.y*1 );
	backArea = Rectf((-SimpleGUI::padding).x,
					 (-SimpleGUI::padding).y,
					 (SimpleGUI::sliderSize + SimpleGUI::padding).x,
					 (SimpleGUI::sliderSize + SimpleGUI::padding * 2).y );
	
}

Vec2f ButtonControl::draw(Vec2f pos) {
	lastName = name;
	lastPressed = pressed;
	activeArea = activeAreaBase + pos;

	gl::color(SimpleGUI::bgColor);
	gl::drawSolidRect(backArea + pos);
	
	gl::color(pressed ? SimpleGUI::lightColor : SimpleGUI::darkColor);
	gl::disableAlphaBlending();
	gl::drawSolidRect(activeArea);
	gl::enableAlphaBlending();
	gl::drawString(name, Vec2f(pos.x + SimpleGUI::padding.x * 2, pos.y + floor(SimpleGUI::padding.y * 0.5)), pressed ? SimpleGUI::darkColor : SimpleGUI::textColor, SimpleGUI::textFont);

	pos.y += backArea.getHeight() + SimpleGUI::spacing.y;
	return pos;
}
	
void ButtonControl::onMouseDown(MouseEvent event) {
	pressed = true;
	if ( !triggerUp )
		fireClick();
}

void ButtonControl::onMouseUp(MouseEvent event) {
	if ( triggerUp && pressed )
		fireClick();
	pressed = false;
}
	
	// ROGER
	void ButtonControl::onMouseDrag(MouseEvent event) {
		pressed = activeArea.contains(event.getPos());
	}

	
void ButtonControl::fireClick() {
	MouseEvent event;
	bool handled = false;
	for( CallbackMgr<bool (MouseEvent)>::iterator cbIter = callbacksClick.begin(); ( cbIter != callbacksClick.end() ) && ( ! handled ); ++cbIter ) {
		handled = (cbIter->second)( event );
	}
}

//-----------------------------------------------------------------------------	

LabelControl::LabelControl(const std::string& name) {
	this->type = Control::LABEL;
	this->name = name;
	this->lastName = name;
	// ROGER
	backArea = Rectf((-SimpleGUI::padding).x,
					 (-SimpleGUI::padding).y,
					 (SimpleGUI::sliderSize + SimpleGUI::padding).x,
					 (SimpleGUI::labelSize + SimpleGUI::padding).y );
}

void LabelControl::setText(const std::string& text) {
	name = text;
}	

Vec2f LabelControl::draw(Vec2f pos) {
	lastName = name;
	if (bgColor) {
		gl::color(bgColor);
	}
	else {
		gl::color(SimpleGUI::bgColor);
	}
	gl::drawSolidRect(backArea+pos);
	gl::drawString(name, pos, SimpleGUI::textColor, SimpleGUI::textFont);					

	pos.y += backArea.getHeight() + SimpleGUI::spacing.y;
	return pos;
}

//-----------------------------------------------------------------------------		
	
SeparatorControl::SeparatorControl() {
	this->type = Control::SEPARATOR;
	this->name = "Separator";
	// ROGER
	//backArea = Rectf( -SimpleGUI::padding, SimpleGUI::separatorSize + SimpleGUI::padding );
	backArea = Rectf(-SimpleGUI::padding.x,
					 0,
					 SimpleGUI::separatorSize.x + SimpleGUI::padding.x,
					 SimpleGUI::separatorSize.y );
}	

Vec2f SeparatorControl::draw(Vec2f pos) {
	
	// original
	//gl::color(SimpleGUI::bgColor);
	//gl::drawSolidRect(activeArea);		

	//activeArea = backArea+pos-Vec2f(0,SimpleGUI::padding.y);
	//gl::color(SimpleGUI::lightColor);
	//gl::drawSolidRect(activeArea);		
	
	pos.y += backArea.getHeight() + SimpleGUI::spacing.y;
	return pos;
}
	
//-----------------------------------------------------------------------------		

ColumnControl::ColumnControl(int x, int y) {
	this->x = x;
	this->y = y;
	this->type = Control::COLUMN;
	this->name = "Column";	
}	
	
Vec2f ColumnControl::draw(Vec2f pos) {
	if (this->x == 0 && this->y == 0) {
		pos.x += SimpleGUI::labelSize.x + SimpleGUI::spacing.x + SimpleGUI::padding.x*2;
		pos.y = SimpleGUI::spacing.y + SimpleGUI::padding.y*2;
	}
	else {
		pos.x = this->x;
		pos.y = this->y;
	}
	return pos;
}
	
//-----------------------------------------------------------------------------		
	
PanelControl::PanelControl() {
	this->enabled = true;
	this->type = Control::PANEL;
	this->name = "Panel";
}	
	
Vec2f PanelControl::draw(Vec2f pos) {
	return pos;
}
	
//-----------------------------------------------------------------------------
	
TextureVarControl::TextureVarControl(const std::string& name, gl::Texture* var, float scale, bool flipVert) {
	this->type = Control::TEXTURE_VAR;
	this->name = name;
	this->var = var;
	this->scale = scale;
	this->flipVert = flipVert;
	this->update();
}
	
	void TextureVarControl::update()
	{
		Vec2f textureSize;
		if (*var) {
			texSize.x = var->getWidth();
			texSize.y = var->getHeight();
			textureSize.x = math<float>::floor(SimpleGUI::sliderSize.x);
			textureSize.y = math<float>::floor(textureSize.x / var->getAspectRatio());
		}
		else {
			// NO TEXTURE!!
			texSize.x = textureSize.x = SimpleGUI::sliderSize.x;
			texSize.y = textureSize.y = SimpleGUI::sliderSize.x * 0.75;
		}

		activeAreaBase = Rectf(0, 
							   SimpleGUI::labelSize.y + SimpleGUI::padding.y, 
							   textureSize.x, 
							   SimpleGUI::labelSize.y + SimpleGUI::padding.y + textureSize.y );
		backArea = Rectf(
						 (-SimpleGUI::padding).x, 
						 (-SimpleGUI::padding).y, 
						 (textureSize + SimpleGUI::padding).x, 
						 (SimpleGUI::labelSize + SimpleGUI::padding + textureSize + SimpleGUI::padding ).y );	
	}

Vec2f TextureVarControl::draw(Vec2f pos) {
	if (!*var)
		this->update();
	else if (var->getWidth() != texSize.x || var->getHeight() != texSize.y)
		this->update();
	activeArea = activeAreaBase + pos;
	
	gl::color(SimpleGUI::bgColor);
	gl::drawSolidRect(backArea+pos);

	gl::drawString(name, pos, SimpleGUI::textColor, SimpleGUI::textFont);

	gl::color(ColorA(1,1,1,1));
	if (*var)
		gl::draw(*var, activeArea);
	else
	{
		gl::drawLine( activeArea.getUpperLeft(), activeArea.getLowerRight() );
		gl::drawLine( activeArea.getLowerLeft(), activeArea.getUpperRight() );
	}
	
	pos.y += backArea.getHeight() + SimpleGUI::spacing.y;
	return pos;
}

//-----------------------------------------------------------------------------	

} //namespace sgui
} //namespace cinder