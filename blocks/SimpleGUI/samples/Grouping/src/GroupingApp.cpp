#include "cinder/app/AppBasic.h"
#include "cinder/gl/FBO.h"
#include "SimpleGUI.h"
#include <list>
using namespace ci;
using namespace ci::app;
using namespace std;
using namespace mowa::sgui;

#define RENDER_TYPE_GROUP 1
#define CONFIG_FILE "settings.sgui.txt"

class GroupingApp : public AppBasic {
private:
	SimpleGUI* gui;
	PanelControl* strokePanel;
	float rotation;
	int size;
	ColorA color;
	bool autoRotation;	
	bool fill;
	bool stroke;
	float thickness;
	
	Timer timer;
	float prevTime;
	
	gl::Fbo fbo;
	
public:
	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void mouseUp( MouseEvent event );
	void keyDown( KeyEvent event );
	void draw();	
	void setup();
};

void GroupingApp::setup() {
	fbo = gl::Fbo(getWindowWidth(), getWindowHeight());
	
	gui = new SimpleGUI(this);
	gui->lightColor = ColorA(1, 1, 0, 1);	
	gui->addColumn();
	gui->addLabel("CONTROLS");
	gui->addParam("Rotation", &rotation, 0, 360, 0);
	gui->addParam("Size", &size, 100, 600, 200); 	
	gui->addParam("Color", &color, ColorA(0,0.5,1,0.5), SimpleGUI::RGB); //use R,G,B,A sliders
	gui->addColumn();	
	gui->addLabel("RENDER TYPE");
	gui->addParam("Fill", &fill, true, RENDER_TYPE_GROUP); //if we specify group id, we create radio button set
	gui->addParam("Stroke", &stroke, false, RENDER_TYPE_GROUP); //i.e. only one of the buttons can be active at any time	
	strokePanel = gui->addPanel();
	gui->addParam("Thickness", &thickness, 1, 10);
	gui->addColumn();
	gui->addLabel("OPTIONS");
	gui->addParam("Auto Rotation", &autoRotation, false); 	
	gui->addColumn();
	gui->addLabel("PREVIEW");
	gui->addParam("PreviewTex", &fbo.getTexture()); 	
	
	gui->load(CONFIG_FILE); //we load settings after specifying all the 
	//params because we need to know their name and type
	
	timer.start();
	prevTime = timer.getSeconds();
	
	
}

void GroupingApp::mouseDown( MouseEvent event ) {	
}

void GroupingApp::mouseDrag( MouseEvent event ) {
}

void GroupingApp::mouseUp( MouseEvent event ) {
	
}

void GroupingApp::keyDown( KeyEvent event ) {
	switch(event.getChar()) {				
		case 'd': gui->dump(); break; //prints values of all the controls to the console			
		case 'l': gui->load(CONFIG_FILE); break;
		case 's': gui->save(CONFIG_FILE); break;					
	}
	switch(event.getCode()) {
		case KeyEvent::KEY_ESCAPE: quit(); break;
	}
}

void GroupingApp::draw() {
	float currTime = timer.getSeconds();
	float deltaTime = currTime - prevTime;
	prevTime = currTime;
	
	if (autoRotation) {
		rotation += deltaTime * 60;
		rotation = fmod(rotation, 360);
	}
	
	fbo.bindFramebuffer();
	
	gl::pushMatrices();
	gl::clear(ColorA(0.2, 0.2, 0.2, 1.0));
	gl::translate(Vec2f(getWindowWidth()/2, getWindowHeight()/2));
	gl::rotate(rotation);
	gl::color(color);
	gl::enableAlphaBlending();
	gl::disableDepthRead();
	
	if (fill) {
		gl::translate(Vec2f(-50, -50));
		gl::drawSolidRect(Rectf(-size/2, -size/2, size/2, size/2));
		gl::translate(Vec2f(+100, +100));
		gl::drawSolidRect(Rectf(-size/2, -size/2, size/2, size/2));
	}
	else if (stroke) {
		glLineWidth(thickness);
		gl::translate(Vec2f(-50, -50));
		gl::drawLine(Vec2f(-size/2, -size/2), Vec2f( size/2, -size/2));
		gl::drawLine(Vec2f( size/2, -size/2), Vec2f( size/2,  size/2));
		gl::drawLine(Vec2f( size/2,  size/2), Vec2f(-size/2,  size/2));
		gl::drawLine(Vec2f(-size/2,  size/2), Vec2f(-size/2, -size/2));		
		gl::translate(Vec2f(+100, +100));
		gl::drawLine(Vec2f(-size/2, -size/2), Vec2f( size/2, -size/2));
		gl::drawLine(Vec2f( size/2, -size/2), Vec2f( size/2,  size/2));
		gl::drawLine(Vec2f( size/2,  size/2), Vec2f(-size/2,  size/2));
		gl::drawLine(Vec2f(-size/2,  size/2), Vec2f(-size/2, -size/2));
		glLineWidth(1);
	}
	gl::popMatrices();	
	
	fbo.unbindFramebuffer();
	
	gl::color(ColorA(1,1,1,1));
	gl::draw(fbo.getTexture());
	
	strokePanel->enabled = stroke ? true : false;
	gui->draw();
}


CINDER_APP_BASIC( GroupingApp, RendererGl )
