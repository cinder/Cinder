#include "cinder/app/AppBasic.h"
#include "cinder/Timeline.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class LoopingExample : public AppBasic {
public:
    void setup();
    void keyDown( KeyEvent event );
    void draw();
	
    Anim<Vec2f> mAnim;
    TimelineRef mTimeline;
};

void LoopingExample::setup()
{
    mTimeline = timeline().create();
    timeline().add( mTimeline );
    mTimeline->setLoop( true );
}

void LoopingExample::keyDown( KeyEvent event )
{
	switch(event.getCode()){
		case KeyEvent::KEY_a:
			mTimeline->apply( &mAnim, Vec2f( 0,0 ), 4.50f,  EaseOutCubic() );
			break;
			
		case KeyEvent::KEY_s:
			mTimeline->setLoop(false);
			mTimeline->apply( &mAnim, Vec2f( 100,100 ), 4.50f,  EaseOutCubic() ).loop();
			break;
			
		case KeyEvent::KEY_d:
			break;
	}
}

void LoopingExample::draw()
{
    // clear out the window with black
    gl::clear( Color( 0, 0, 0 ) );
	
    gl::color( Color(1.0, 0.0, 0.0) );
    gl::drawSolidCircle( mAnim, 10 );
}

CINDER_APP_BASIC( LoopingExample, RendererGl )
