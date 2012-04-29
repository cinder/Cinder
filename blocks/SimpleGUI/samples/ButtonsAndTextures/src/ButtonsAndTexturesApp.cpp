#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "SimpleGUI.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace mowa::sgui;


#define CONFIG_FILE "settings.sgui.txt"

class ButtonsAndTexturesApp : public AppBasic {
private:
    SimpleGUI* gui;
    Timer timer;
    float prevTime;
    float rotation;
    
    ButtonControl* takeScreenshotButton;
    CallbackId cbTakeScreenshotButtonClick;
        
    TextureVarControl* screenshotTextureControl;
    gl::Texture screenshot;
public:
	void setup();
	void mouseDown( MouseEvent event );	
    void keyDown( KeyEvent event );
    bool takeScreenshotButtonClick( MouseEvent event ); //this event handlers return bool because if they return false the event is marked as intercepted
    bool restartButtonClick( MouseEvent event );    //and your application mouse events handlers won't be called (which is usefull)
	void update();
	void draw();
};

void ButtonsAndTexturesApp::setup() {
    rotation = 0;
    screenshot = gl::Texture(getWindowWidth(), getWindowHeight()); //uninitialized texture with random pixels from GPU memory
        
    gui = new SimpleGUI(this);
	gui->lightColor = ColorA(1, 1, 0, 1);		
	gui->addLabel("CONTROLS");
    
    //let's add a button
    gui->addButton("Restart")->registerClick(this, &ButtonsAndTexturesApp::restartButtonClick);
    
    gui->addSeparator();
    gui->addLabel("SCREENSHOT");
    
    screenshotTextureControl = gui->addParam("Screenshot", &screenshot);
    
    //you can also store reference to it and add callback as a second step
    takeScreenshotButton = gui->addButton("Take screenshot");    
	cbTakeScreenshotButtonClick = takeScreenshotButton->registerClick(this, &ButtonsAndTexturesApp::takeScreenshotButtonClick);    
    
    //to stop listening to click do this:
    //takeScreenshotButton->unregisterClick(cbTakeScreenshotButtonClick);
    
	gui->load(CONFIG_FILE); //we load settings after specifying all the 
    //params because we need to know their name and type
	
	timer.start();
	prevTime = timer.getSeconds();

}

void ButtonsAndTexturesApp::mouseDown( MouseEvent event ) {
}

bool ButtonsAndTexturesApp::takeScreenshotButtonClick( MouseEvent event ) {
    screenshot.update(copyWindowSurface());
    return false;
}

bool ButtonsAndTexturesApp::restartButtonClick( MouseEvent event ) {
    rotation = 0;
    return false;
}

void ButtonsAndTexturesApp::keyDown( KeyEvent event ) {
	switch(event.getChar()) {				
		case 'd': gui->dump(); break; //prints values of all the controls to the console			
		case 'l': gui->load(CONFIG_FILE); break;
		case 's': gui->save(CONFIG_FILE); break;					
	}
	switch(event.getCode()) {
		case KeyEvent::KEY_ESCAPE: quit(); break;
	}
}

void ButtonsAndTexturesApp::update() {
}

void ButtonsAndTexturesApp::draw(){
    float currTime = timer.getSeconds();
	float deltaTime = currTime - prevTime;
	prevTime = currTime;
    
    rotation += deltaTime/2;
    
    gl::clear(ColorA(0.1, 0.1, 0.5, 0.92));
    gl::enableAdditiveBlending();
    gl::disableDepthRead();    
    gl::pushMatrices();
    gl::translate(Vec3f(getWindowWidth()/2, getWindowHeight()/2, 0));
    
    gl::color(ColorA(1, 0.2, 0.2, 0.1));    
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
	gui->draw();
}


CINDER_APP_BASIC( ButtonsAndTexturesApp, RendererGl )
