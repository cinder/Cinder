#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/qtime/QuickTimeGl.h"
#include "cinder/Log.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class Slider {
    public:
        Slider() = default;
        Slider( Rectf boundingRect ) 
        : mBoundingRect( boundingRect )
        , mSliderRect( Rectf{ mBoundingRect.getUpperLeft(), mBoundingRect.getUpperLeft() + vec2( 25.0f,
        mBoundingRect.getHeight() ) } )
        {
            mMouseDownConn  = getWindow()->getSignalMouseDown().connect( std::bind( &Slider::mouseDown, this, std::placeholders::_1 ) );
            mMouseUpConn    = getWindow()->getSignalMouseDrag().connect( std::bind( &Slider::mouseDrag, this, std::placeholders::_1 ) );
            mMouseUpConn    = getWindow()->getSignalMouseUp().connect( std::bind( &Slider::mouseUp, this, std::placeholders::_1 ) );
        }

        ~Slider() 
        {
            mMouseDownConn.disconnect();
            mMouseDragConn.disconnect();
            mMouseUpConn.disconnect();
        }
        void setSliderCb( std::function<void(float)> sliderCb ) { mSliderCb = sliderCb; }

        void updateValue( float sliderValue )
        {
            mSliderValue = sliderValue;
        }

        void draw()
        {
            {
                gl::ScopedColor scopedColor( Color( 1.0f, .0f, .0f ) );
                gl::drawStrokedRect( mBoundingRect );
            }
            {
                gl::ScopedColor scopedColor( Color( .0f, 1.0f, 1.0f ) );
                mSliderRect.moveULTo( vec2( mBoundingRect.getUpperLeft().x + mSliderValue * ( mBoundingRect.getWidth() -
                mSliderRect.getWidth() ), mBoundingRect.getUpperLeft().y ) );
                gl::drawSolidRect( mSliderRect );
            }
        }

        void mouseDown( MouseEvent event )
        {
            const vec2& mousePos = event.getPos();
            if( mSliderRect.contains( mousePos ) ){
                mIsDragging = true;
            }
        }

        void mouseDrag( MouseEvent event )
        {
            const vec2& mousePos = event.getPos();
            if( mIsDragging && mBoundingRect.contains( mousePos ) ) {
                auto mappedX = lmap( mousePos.x, mBoundingRect.getUpperLeft().x, mBoundingRect.getUpperRight().x,
                0.0f, mBoundingRect.getWidth() );
                mSliderValue = mappedX / mBoundingRect.getWidth();
                if( mSliderCb )
                    mSliderCb( mSliderValue );
            }
        }

        void mouseUp( MouseEvent event )
        {
            mIsDragging = false;
        }

        bool isScrubbing() { return mIsDragging; }

    private:
        std::function<void(float)>  mSliderCb;
        Rectf                       mBoundingRect;
        Rectf                       mSliderRect;
        bool                        mIsDragging = false;
        float                       mSliderValue = 0.0f;
        ci::signals::Connection     mMouseDownConn;
        ci::signals::Connection     mMouseDragConn;
        ci::signals::Connection     mMouseUpConn;
};


const int kSliderHeight = 70.0f;

class VideoPlayerScrubTestApp : public App {
    public:
        VideoPlayerScrubTestApp() : mSlider( { Rectf( 0.0f, getWindowHeight() - kSliderHeight, getWindowWidth(),
        getWindowHeight() ) } ) {}
        static void prepareSettings( Settings *settings ) { }

        void setup() override;

        void keyDown( KeyEvent event ) override;
        void mouseDown( MouseEvent event ) override;
        void fileDrop( FileDropEvent event ) override;

        void update() override;
        void draw() override;

        void sliderCb( float sliderValue );

        void loadMovieFile( const fs::path &path );

        gl::TextureRef      mFrameTexture;
        qtime::MovieGlRef   mMovie;
        Slider              mSlider;
};

void VideoPlayerScrubTestApp::sliderCb( float sliderValue )
{
    CI_LOG_V( " Slider value : " << sliderValue );
    if( mMovie ) {
        auto seekTo = mMovie->getDuration() * sliderValue;
        mMovie->seekToTime( seekTo );
    }
}

void VideoPlayerScrubTestApp::setup()
{
    mSlider.setSliderCb( std::bind( &VideoPlayerScrubTestApp::sliderCb, this, std::placeholders::_1 ) );

    fs::path moviePath = getOpenFilePath();
    if( ! moviePath.empty() )
        loadMovieFile( moviePath );
}

void VideoPlayerScrubTestApp::update()
{
    if( ! mSlider.isScrubbing() && mMovie && mMovie->isLoaded() ) {
        mSlider.updateValue( mMovie->getCurrentTime() / mMovie->getDuration() );
    }

    if( mMovie ) {
        mFrameTexture = mMovie->getTexture();
    }

    static bool sPrintedDone = false;
    if( ! sPrintedDone && mMovie->isDone() ) {
        CI_LOG_I( "Done Playing" );
        sPrintedDone = true;
    }
}

void VideoPlayerScrubTestApp::draw()
{
    gl::clear( Color( 0, 0, 0 ) );

    if( mFrameTexture ) {
        Rectf centeredRect = Rectf( mFrameTexture->getBounds() ).getCenteredFit( getWindowBounds(), true );
        gl::draw( mFrameTexture, centeredRect );
    }

    mSlider.draw();
}

void VideoPlayerScrubTestApp::keyDown( KeyEvent event )
{
    if( event.getCode() == KeyEvent::KEY_o ) {
        fs::path moviePath = getOpenFilePath();
        if( ! moviePath.empty() )
            loadMovieFile( moviePath );
    }
    else if( event.getCode() == KeyEvent::KEY_p ) {
        if( mMovie ) mMovie->stop();
    }
    else if( event.getCode() == KeyEvent::KEY_l ) {
        if( mMovie ) mMovie->play();
    }
}

void VideoPlayerScrubTestApp::loadMovieFile( const fs::path &moviePath )
{
    try {
        // load up the movie, and begin playing
        mMovie = qtime::MovieGl::create( moviePath );
        mMovie->play();
        console() << "Playing: " << mMovie->isPlaying() << std::endl;
    }
    catch( ci::Exception &exc ) {
        console() << "Exception caught trying to load the movie from path: " << moviePath << ", what: " << exc.what() << std::endl;
        mMovie.reset();
    }

    mFrameTexture.reset();
}

void VideoPlayerScrubTestApp::mouseDown( MouseEvent event )
{

}

void VideoPlayerScrubTestApp::fileDrop( FileDropEvent event )
{
    loadMovieFile( event.getFile( 0 ) );
}

CINDER_APP( VideoPlayerScrubTestApp, RendererGl, VideoPlayerScrubTestApp::prepareSettings );
