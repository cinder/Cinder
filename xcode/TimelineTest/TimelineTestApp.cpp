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
    Anim<float> mAnim2;
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
	mAnim2 = 0;
	
	mTimeline = Timeline::create();
	mTimeline->setDefaultAutoRemove( false );
	mTimeline->setAutoRemove(false);
	
	mTimeline2 = Timeline::create();
	mTimeline2->setDefaultAutoRemove( false );
	mTimeline2->setAutoRemove(false);
	
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
			timeline().add( mTimeline2 );
//			mTimeline->apply( &mAnim, Vec2f( 0,0 ), 4.50f,  EaseOutCubic() );
//			mTimeline->apply( &mAnim, Vec2f( 200, 50 ), Vec2f( 200, 200 ), 2.0f ).delay( 1.0f ).loop( true );
			mTimeline2->apply( &mAnim2, 300.0f, 100.0f, 2.0f );
			mTimeline2->appendTo( &mAnim2, 100.0f, 175.0f, 2.0f );
			mTimeline2->appendTo( &mAnim2, 175.0f, 50.0f, 2.0f );
			mTimeline2->appendPingPong();
			break;
			
		case KeyEvent::KEY_s:
			timeline().add( mTimeline );
			mTimeline->apply( &mAnim, Vec2f( 10,10 ), Vec2f( 100,100 ), 1.0f, EaseOutCubic());
			mTimeline->appendTo( &mAnim, Vec2f( 100,100 ), Vec2f( 100,300 ), 1.0f, EaseNone());
			mTimeline->appendTo( &mAnim, Vec2f( 100,300 ), Vec2f( 10,10 ), 1.0f, EaseNone());
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
//	console() << "mAnim = " << mAnim.ptr() << " -> " << *(mAnim.ptr()) << " ==? " << mAnim << std::endl;
}

void TimelineTest::draw()
{
    // clear out the window with black
    gl::clear( Color( 0, 0, 0 ) );
	
    gl::color( Color(1.0, 0.0, 0.0) );
    gl::drawSolidCircle( mAnim, 10 );
	
    gl::color( Color(1.0, 1.0, 0.0) );
    gl::drawSolidCircle( Vec2f(mAnim2, 150), 20 );
}

CINDER_APP_BASIC( TimelineTest, RendererGl )
