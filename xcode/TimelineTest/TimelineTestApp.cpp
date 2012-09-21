#include <map>

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
	void update();
    void draw();
	
    Anim<Vec2f> mAnim;
    Anim<Vec2f> mAnim2;
    TimelineRef mTimeline;
    TimelineRef mTimeline2;
	
	// std::multimap<void*,uint32_t> mStuff;
};

void TimelineTest::setup()
{
//    mTimeline = timeline().create();
//    timeline().add( mTimeline );
//    mTimeline->setLoop( true );
	
	mAnim = Vec2f(10,10);
	console() << "TimelineTest::setup // mAnim = " << mAnim << " = " << mAnim.ptr() << " -> " << mAnim.value() << std::endl;
	mAnim2 = Vec2f(getWindowSize());
	
	mTimeline = Timeline::create();
	mTimeline->setDefaultAutoRemove( false );
	mTimeline->setAutoRemove(false);
	
	mTimeline2 = Timeline::create();
	mTimeline2->setDefaultAutoRemove( false );
	mTimeline2->setAutoRemove(true);
	
	/*
	std::multimap<void*,uint32_t>::iterator it;
	mStuff.insert( std::pair<void*, uint32_t>(this, 20302) );
	it = mStuff.find(this);
	mStuff.erase(it);
	
	mStuff.insert( std::pair<void*, uint32_t>(this, 523) );
	it = mStuff.find(this);
	console() << it->second << std::endl;
	*/
}

void TimelineTest::mouseDown( MouseEvent event )
{
}

void TimelineTest::keyDown( KeyEvent event )
{
	switch(event.getCode()){
		case KeyEvent::KEY_a:
//			mTimeline->apply( &mAnim, Vec2f( 0,0 ), 4.50f,  EaseOutCubic() );
//			mTimeline->apply( &mAnim, Vec2f( 200, 50 ), Vec2f( 200, 200 ), 2.0f ).delay( 1.0f ).loop( true );
			mTimeline2->apply( &mAnim2, Vec2f( 300, 50 ), Vec2f( 300, 200 ), 2.0f ).pingPong(true);
			break;
			
		case KeyEvent::KEY_s:
			timeline().add( mTimeline );
			assert(mAnim.ptr() == mTimeline->apply( &mAnim, Vec2f( 10,10 ), Vec2f( 100,100 ), 1.0f, EaseOutCubic()).getTarget());
			assert(mAnim.ptr() == mTimeline->appendTo( &mAnim, Vec2f( 100,100 ), Vec2f( 100,300 ), 1.0f, EaseNone()).getTarget());
			assert(mAnim.ptr() == mTimeline->appendTo( &mAnim, Vec2f( 100,300 ), Vec2f( 10,10 ), 1.0f, EaseNone()).getTarget());
//			mTimeline->apply( &mAnim, Vec2f( 100,100 ), 1.0f, EaseOutCubic());
//			mTimeline->appendTo( &mAnim, Vec2f( 100,300 ), 1.0f, EaseNone());
//			mTimeline->appendTo( &mAnim, Vec2f( 300,300 ), 1.0f, EaseNone());
			mTimeline->appendPingPong();
			break;
			
		case KeyEvent::KEY_d:
			timeline().remove( mTimeline );
			break;
			
		case KeyEvent::KEY_f:
			mTimeline->reset(true);
			timeline().add( mTimeline );
			break;
			
		case KeyEvent::KEY_g:
			timeline().remove( mTimeline2 );
			break;
			
		case KeyEvent::KEY_h:
			mTimeline->reset(true);
			timeline().insert( mTimeline2 );
			break;
			
		case KeyEvent::KEY_r:
			mTimeline->reset(true);	// still no way to restart a timeline...
			break;
	}
}

void TimelineTest::update()
{
	console() << "mAnim = " << mAnim.ptr() << " -> " << *(mAnim.ptr()) << " ==? " << mAnim << std::endl;
}

void TimelineTest::draw()
{
    // clear out the window with black
    gl::clear( Color( 0, 0, 0 ) );
	
    gl::color( Color(1.0, 0.0, 0.0) );
    gl::drawSolidCircle( mAnim, 10 );
	
    gl::color( Color(1.0, 1.0, 0.0) );
    gl::drawSolidCircle( mAnim2, 20 );
}

CINDER_APP_BASIC( TimelineTest, RendererGl )
