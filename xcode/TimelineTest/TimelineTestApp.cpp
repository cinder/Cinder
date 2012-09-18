#include "cinder/app/AppBasic.h"
#include "cinder/Timeline.h"
#include "cinder/Tween.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class TimelineTest : public AppBasic {
public:
    void setup();
    void mouseDown( MouseEvent event );
    void keyDown( KeyEvent event );
    void draw();
	
    Anim<Vec2f> mAnim;
    TimelineRef mTimeline;
};

void TimelineTest::setup()
{
    mTimeline = timeline().create();
    timeline().add( mTimeline );
    mTimeline->setLoop( true );
}

void TimelineTest::mouseDown( MouseEvent event )
{
}

void TimelineTest::keyDown( KeyEvent event )
{
	switch(event.getCode()){
		case KeyEvent::KEY_a:
			mTimeline->apply( &mAnim, Vec2f( 0,0 ), 4.50f,  EaseOutCubic() );
			break;
			
		case KeyEvent::KEY_s:
//			mTimeline->setLoop(false);
			mTimeline->apply( &mAnim, Vec2f( 100,100 ), 4.50f,  EaseOutCubic() ).pingPong();
			break;
			
		case KeyEvent::KEY_d:
			break;
	}
}

void TimelineTest::draw()
{
    // clear out the window with black
    gl::clear( Color( 0, 0, 0 ) );
	
    gl::color( Color(1.0, 0.0, 0.0) );
    gl::drawSolidCircle( mAnim, 10 );
}

CINDER_APP_BASIC( TimelineTest, RendererGl )
