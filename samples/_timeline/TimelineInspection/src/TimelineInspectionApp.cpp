#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include "cinder/params/Params.h"
#include "cinder/Rand.h"
#include "cinder/Timeline.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class TimelineInspectionApp : public App {
  public:
	void setup() override;
	
    void mouseDown( MouseEvent event ) override;
    void mouseUp( MouseEvent event ) override;
    void mouseDrag( MouseEvent event ) override;
    void keyUp( KeyEvent event ) override;
    
	void update() override;
	void draw() override;
    void drawTimeline();
    
    void buildTimelineItems();
    
    
    void changeColor();
    
    
    TimelineRef mTimeline;
    bool doTimelineUpdate = true;
    
    ci::Anim<float> scale;
    ci::Anim<float> rotation;
    ci::Anim<vec2> position;
    
    
    Color mColor;
    
};

void TimelineInspectionApp::setup()
{

    mTimeline = Timeline::create();
    mTimeline->setInfinite(false);
    mTimeline->setDefaultAutoRemove(false);
    

    mColor = Color(0.7, 0.1, 0.4);
    
    position = getWindowCenter();
    scale = 1.0f;
    rotation = 0.0f;
    
    buildTimelineItems();
}

void TimelineInspectionApp::buildTimelineItems(){
    
    mTimeline->apply(&scale, 3.0f, 5.0f).autoRemove(false);
    mTimeline->apply(&position, vec2(30, getWindowCenter().y), 1.0f).delay(2.5f) ;
    mTimeline->apply(&rotation, float(M_PI * 2) , 2.0f).delay(5.f) ;

    mTimeline->appendTo(&position, vec2(getWindowWidth()  -  30, getWindowCenter().y), 5.0f).delay(5.0f);
    mTimeline->appendTo(&scale, 0.5f, 3.0f).delay(4.0f);
    
    mTimeline->appendTo(&rotation, float(M_PI_4), 3.0f).delay(3.0f);
    
    
    mTimeline->add([&]{
        
        changeColor();
        
    }, mTimeline->getDuration() * 0.5f );
}

void TimelineInspectionApp::changeColor(){
    
    // change the overral color to something random
    mColor =  ColorT<float>(CM_HSV, randFloat(0, 1), 0.75, 0.75);
}


void TimelineInspectionApp::mouseDown( MouseEvent event )
{
    doTimelineUpdate = false;
}

void TimelineInspectionApp::mouseUp(cinder::app::MouseEvent event)
{
    doTimelineUpdate = true;
}

void TimelineInspectionApp::mouseDrag( MouseEvent event )
{
    
    float timelineWindowWidth = getWindowWidth();
    
    auto screenToTimeFn = [&](float x) -> float {
        float posNorm = x / timelineWindowWidth;
        return posNorm * mTimeline->getDuration();
    };
    
    
    mTimeline->stepTo( screenToTimeFn( event.getX() ) );
    
}

void TimelineInspectionApp::keyUp(cinder::app::KeyEvent event){
    
    if( event.getChar() == 's' ){
        mTimeline->appendTo(&scale, randFloat(0.1f, 5.0f), randFloat( 0.5f, 1.0f)).delay(0.5f);
    }
    
    
    else if( event.getChar() == 'r' ){
        mTimeline->appendTo(&rotation, randFloat(0.0f, M_PI_2), randFloat( 0.5f, 1.0f)).delay(0.5f);
    }
    
    else if( event.getChar() == 'p' ){
        
        auto pos = getWindowCenter() + randVec2() * 50.f;
        mTimeline->appendTo(&position, pos, randFloat( 0.5f, 1.0f)).delay(0.5f);
    }
    
    
    else if( event.getChar() == ' ' ){
        
        
        mTimeline->add([&]{
            
            changeColor();
            
        }, mTimeline->getCurrentTime() );
        
    }
    
    
}

void TimelineInspectionApp::update()
{
    
    if( doTimelineUpdate )
        mTimeline->step(1.f/60.f);
}

void TimelineInspectionApp::drawTimeline()
{
    
    float timelineWindowHeight = 100.0f;
    float timelineWindowWidth = getWindowWidth();
    
    float itemHeight = 20.0f;
    
    
    // lambda functions to transform the timeline time to a screen position
    auto timeToScreenFn = [&]( float t ) -> float {
        float normTime = t / mTimeline->getDuration();
        return normTime * timelineWindowWidth;
    };

    
    auto items = mTimeline->getItems();

    
    //lampada function that draws a target
    auto drawTimelineItemFn = [&]( void* target ){
        
        auto itearator = items.equal_range( target );
    
        for (std::multimap<void*, TimelineItemRef>::iterator it=itearator.first; it!=itearator.second; ++it){
            
            auto startPos = timeToScreenFn(it->second->getStartTime());
            auto endPos = timeToScreenFn(it->second->getEndTime());
            
            Rectf r( startPos, 0, endPos, itemHeight );
            gl::drawSolidRect( r );
        }
    };

    
    
    { // draw Anims ( position, rotation, scale )
        gl::ScopedColor sc;
        
        
        {// draw scale
            gl::ScopedModelMatrix sm;
            gl::color( Color(1.0f, 0.3, 0.35) );
            gl::translate( vec2(0, 5) );
            drawTimelineItemFn( scale.ptr() );
            
            gl::drawString("scale", vec2(0,5));
            
            gl::color( Color::gray(0.75) );
            gl::drawLine( vec2(0, itemHeight), vec2(timelineWindowWidth, itemHeight ) );
        }
        
        
        // draw rotation
        {
            gl::ScopedModelMatrix sm;
            gl::color( Color(0.7f, 0.7, 0.1) );
            gl::translate( vec2(0, itemHeight + 5) );
            drawTimelineItemFn( rotation.ptr() );
            gl::drawString("rotation", vec2(0,5));
            
            gl::color( Color::gray(0.75) );
            gl::drawLine( vec2(0, itemHeight), vec2(timelineWindowWidth, itemHeight ) );
        }
        
        {// draw Position
            gl::ScopedModelMatrix sm;
            gl::color( Color(0.3f, 0.3, 0.9) );
            gl::translate( vec2(0, itemHeight * 2 + 5) );
            drawTimelineItemFn( position.ptr() );
            gl::drawString("position", vec2(0,5));
            
            gl::color( Color::gray(0.75) );
            gl::drawLine( vec2(0, itemHeight), vec2(timelineWindowWidth, itemHeight ) );
        }
        
    }
    
    
    
    {// iterate  every item and draw Cues
        for( auto& item : items ){
     
            auto cue = dynamic_pointer_cast<Cue>( item.second );
            if( cue ){
                auto pos = vec2( timeToScreenFn( cue->getStartTime() ), itemHeight * 4);
                gl::drawSolidCircle( pos , 3 );
            }
        }
        
        gl::drawString("cues: ", vec2(0, itemHeight * 3.8) );
        gl::drawLine(vec2(0, itemHeight * 4 + 10), vec2(timelineWindowWidth, itemHeight * 4 + 10) );
    }
    
    

    { // draw timeline cursor
        
        gl::ScopedColor sc( Color::white() );
        float cursorXPos = timeToScreenFn( mTimeline->getCurrentTime() );
        gl::drawLine( vec2( cursorXPos, 0.f) , vec2( cursorXPos,timelineWindowHeight ) );
    }
}


void TimelineInspectionApp::draw()
{
    gl::clear( Color::gray(0.4) );
    
    drawTimeline();
    
    
    {
        ci::Rectf rect( -10, -10, 10, 10 );
        
        gl::ScopedColor sc( mColor );
        gl::ScopedModelMatrix sm;
        
        gl::translate( position() );
        gl::rotate( rotation() );
        gl::scale( vec3(scale()) );
        
        gl::drawSolidRect( rect );
    }
}

CINDER_APP( TimelineInspectionApp, RendererGl )
