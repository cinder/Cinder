#include "cinder/app/AppBasic.h"
#include "SimpleGUI.h"
#include <list>
using namespace ci;
using namespace ci::app;
using namespace std;
using namespace mowa::sgui;

#define RENDER_TYPE_GROUP 1
#define CONFIG_FILE "settings.sgui.txt"

class BasicApp : public AppBasic {
private:
	SimpleGUI* gui;
	float rotation;
	int size;
	ColorA color;
	bool autoRotation;	
	bool fill;
	bool stroke;
	
	Timer timer;
	float prevTime;
	
public:
	void mouseDown( MouseEvent event );
	void mouseDrag( MouseEvent event );
	void mouseUp( MouseEvent event );
	void keyDown( KeyEvent event );
	void draw();	
	void setup();
};

void BasicApp::setup() {
	gui = new SimpleGUI(this);
	gui->lightColor = ColorA(1, 1, 0, 1);	
	gui->addLabel("CONTROLS");
	gui->addParam("Rotation", &rotation, 0, 360, 0);
	gui->addParam("Size", &size, 100, 600, 200); 	
	gui->addParam("Color", &color, ColorA(0,0.5,1,0.5), SimpleGUI::RGB); //use R,G,B,A sliders
	gui->addSeparator();
	gui->addLabel("OPTIONS");
	gui->addParam("Auto Rotation", &autoRotation, false); 
	gui->addSeparator();	
	gui->addLabel("RENDER TYPE");
	gui->addParam("Fill", &fill, true, RENDER_TYPE_GROUP); //if we specify group id, we create radio button set
	gui->addParam("Stroke", &stroke, false, RENDER_TYPE_GROUP); //i.e. only one of the buttons can be active at any time
	
	gui->load(CONFIG_FILE); //we load settings after specifying all the 
	                       //params because we need to know their name and type
	
	timer.start();
	prevTime = timer.getSeconds();
}

void BasicApp::mouseDown( MouseEvent event ) {	
}

void BasicApp::mouseDrag( MouseEvent event ) {
}

void BasicApp::mouseUp( MouseEvent event ) {

}

void BasicApp::keyDown( KeyEvent event ) {
	switch(event.getChar()) {				
		case 'd': gui->dump(); break; //prints values of all the controls to the console			
		case 'l': gui->load(CONFIG_FILE); break;
		case 's': gui->save(CONFIG_FILE); break;					
	}
	switch(event.getCode()) {
		case KeyEvent::KEY_ESCAPE: quit(); break;
	}
}

void BasicApp::draw() {
	float currTime = timer.getSeconds();
	float deltaTime = currTime - prevTime;
	prevTime = currTime;

	if (autoRotation) {
		rotation += deltaTime * 60;
		rotation = fmod(rotation, 360);
	}
	
	gl::pushMatrices();
	gl::clear(ColorA(0.0, 0.0, 0.0, 0.92));
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
	}
	gl::popMatrices();	
	gui->draw();
}


CINDER_APP_BASIC( BasicApp, RendererGl )
