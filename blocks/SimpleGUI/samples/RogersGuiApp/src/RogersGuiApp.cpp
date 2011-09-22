#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "SimpleGUI.h"
#include <iostream>
#include <sstream>
#include <fstream>

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace mowa::sgui;

#define RENDER_TYPE_GROUP 1

#define CONFIG_FILE "settings.sgui.txt"

class RogersGuiApp : public AppBasic {
private:
    SimpleGUI* gui;
    Timer timer;
    float prevTime;
    
    ButtonControl* takeScreenshotButton;
    CallbackId cbTakeScreenshotButtonClick;
        
    TextureVarControl* screenshotTextureControl;
    gl::Texture screenshot;
    gl::Texture noTex;
	
	// ROGER:: From Grouping example
	PanelControl* strokePanel;
	float rotation;
	int size;
	ColorA color;
	Color colorBack;
	ColorA colorHSV;
	Color colorBackHSV;
	bool autoRotation;	
	bool fill;
	bool stroke;
	float thickness;
	
	// ROGER:: NEW
	std::map<int,std::string> valueLabels;
	int listVal;
	ListVarControl *theListControl;
	float FPS;
	int windowWidth;
	int windowHeight;
	
public:
	void setup();
	void mouseDown( MouseEvent event );	
    void keyDown( KeyEvent event );
    bool takeScreenshotButtonClick( MouseEvent event ); //this event handlers return bool because if they return false the event is marked as intercepted
    bool restartButtonClick( MouseEvent event );    //and your application mouse events handlers won't be called (which is usefull)
	void update();
	void draw();
	// ROGER
	bool addToList( MouseEvent event ); //this event handlers return bool because if they return false the event is marked as intercepted
	bool removeFromList( MouseEvent event ); //this event handlers return bool because if they return false the event is marked as intercepted
};

void RogersGuiApp::setup() {
    rotation = 0;
    screenshot = gl::Texture(getWindowWidth(), getWindowHeight()); //uninitialized texture with random pixels from GPU memory
        
    gui = new SimpleGUI(this);
	gui->lightColor = ColorA(1, 1, 0, 1);
	gui->addColumn();
	gui->addLabel("CONTROLS");
    
    //let's add a button
    gui->addButton("Restart")->registerClick(this, &RogersGuiApp::restartButtonClick);
    
    gui->addSeparator();
    
    screenshotTextureControl = gui->addParam("Screenshot", &screenshot);
    
    //you can also store reference to it and add callback as a second step
    takeScreenshotButton = gui->addButton("Take screenshot");    
	takeScreenshotButton->registerClick(this, &RogersGuiApp::takeScreenshotButtonClick);    
    //to stop listening to click do this:
    //takeScreenshotButton->unregisterClick(cbTakeScreenshotButtonClick);

	// ROGER
    screenshotTextureControl = gui->addParam("Empty Texture", &noTex);

    //
	// ROGER:: From Grouping example
	gui->addColumn();
	gui->addLabel("CONTROLS");
	gui->addParam("Auto Rotation", &autoRotation, true); 	
	gui->addParam("Rotation", &rotation, 0, 360, 0)->setDisplayValue();
	gui->addParam("Size", &size, 100, 600, 200)->setDisplayValue();
	gui->addParam("Background", &colorBack, Color(0.1, 0.1, 0.5), SimpleGUI::RGB); //use R,G,B,A sliders
	gui->addParam("Color", &color, ColorA(1, 0.2, 0.2, 0.1), SimpleGUI::RGB); //use R,G,B,A sliders
	gui->addParam("HSV", &colorBackHSV, Color(0.1, 0.1, 0.5), SimpleGUI::HSV);
	gui->addParam("HSV+A", &colorHSV, ColorA(1, 0.2, 0.2, 0.1), SimpleGUI::HSV);
    gui->addSeparator();
	gui->addLabel("RENDER TYPE");
	gui->addParam("Fill", &fill, true, RENDER_TYPE_GROUP); //if we specify group id, we create radio button set
	gui->addParam("Stroke", &stroke, false, RENDER_TYPE_GROUP); //i.e. only one of the buttons can be active at any time	
	strokePanel = gui->addPanel();
	gui->addParam("Thickness", &thickness, 1, 10);
	
	//
	// ROGER:: NEW
	gui->addColumn();
	gui->addLabel("ROGER LIST");
    gui->addButton("Increase List")->registerClick(this, &RogersGuiApp::addToList);
    gui->addButton("Decrease List")->registerClick(this, &RogersGuiApp::removeFromList);
	valueLabels[0] = "val 0";
	valueLabels[1] = "val 1";
	valueLabels[2] = "val 2";
	theListControl = gui->addParam("Roger List", &listVal, valueLabels);
    gui->addLabel("justa nother label");
	//
	// Other
	gui->addColumn();
	gui->addLabel("ROGER READONLY");
	gui->addParam("FPS", &FPS, 0, 60, 0)->setReadOnly();
	gui->addParam("Window Width", &windowWidth, 0, 1920, getWindowWidth())->setReadOnly();
	gui->addParam("Window Height", &windowHeight, 0, 1080, getWindowHeight())->setReadOnly();

	gui->load(CONFIG_FILE); //we load settings after specifying all the 
    //params because we need to know their name and type
	
	timer.start();
	prevTime = timer.getSeconds();

}

void RogersGuiApp::mouseDown( MouseEvent event ) {
}

bool RogersGuiApp::takeScreenshotButtonClick( MouseEvent event ) {
    screenshot.update(copyWindowSurface());
    return false;
}

// ROGER
bool RogersGuiApp::addToList( MouseEvent event ) {
	int i = valueLabels.size();
	std::stringstream ss;
	ss << "new " << i;
	valueLabels[i] = ss.str();
	theListControl->update( valueLabels );
    return false;
}
bool RogersGuiApp::removeFromList( MouseEvent event ) {
	if (valueLabels.size())
	{
		valueLabels.erase( valueLabels.rbegin()->first );
		theListControl->update( valueLabels );
	}
    return false;
}

bool RogersGuiApp::restartButtonClick( MouseEvent event ) {
    rotation = 0;
    return false;
}

void RogersGuiApp::keyDown( KeyEvent event ) {
	switch(event.getChar()) {				
		case 'd': gui->dump(); break; //prints values of all the controls to the console			
		case 'l': gui->load(CONFIG_FILE); break;
		case 's': gui->save(CONFIG_FILE); break;					
	}
	switch(event.getCode()) {
		case KeyEvent::KEY_ESCAPE: quit(); break;
	}
}

void RogersGuiApp::update() {
}

void RogersGuiApp::draw(){
    float currTime = timer.getSeconds();
	float deltaTime = currTime - prevTime;
	prevTime = currTime;
    
	if (autoRotation)
		rotation += deltaTime/2;
    
    gl::clear( colorBack );
    gl::enableAdditiveBlending();
    gl::disableDepthRead();    
    gl::pushMatrices();
    gl::translate(Vec3f(getWindowWidth()/2, getWindowHeight()/2, 0));
    
    gl::color( color );    
    for(int i=0;i<50; i++) {
        gl::rotate(Vec3f(0, 0, rotation));
        gl::drawSolidRect(Rectf(20, 20, 140, 140));
    }
    
    gl::color(ColorA(0.5, 0.25, 0.0, 0.05));    
    for(int i=0;i<50; i++) {
        gl::rotate(Vec3f(0, 0, rotation/4));
        gl::drawSolidRect(Rectf(50, 50, 340, 340));
    }
    
    gl::enableDepthRead();    
    gl::disableAlphaBlending();
    gl::popMatrices();	

	FPS = getAverageFps();
	strokePanel->enabled = stroke ? true : false;
	gui->draw();
	
	//std::cout << "LIST = " << listVal << std::endl;
}


CINDER_APP_BASIC( RogersGuiApp, RendererGl )
