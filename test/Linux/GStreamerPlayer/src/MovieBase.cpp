#include "MovieBase.h"

namespace gst { namespace video {

// MovieBase
MovieBase::MovieBase()
{
    mGstPlayer = GstPlayerRef( new GstPlayer );
    mGstPlayer->initialize();
}

MovieBase::~MovieBase()
{
}

void MovieBase::load( const ci::fs::path &path )
{
    if( !mGstPlayer ) return;
    loadNewMovie( path.string() );
}

void MovieBase::load( const ci::Url &url )
{
    if( !mGstPlayer ) return;
    loadNewMovie( url.str() );
}

void MovieBase::setCustomPipeline( const GstCustomPipelineData &customPipeline )
{
    if( !mGstPlayer ) return;
    mGstPlayer->setCustomPipeline( customPipeline );
}

void MovieBase::loadNewMovie( std::string _path )
{
    mGstPlayer->load( _path );
}

bool MovieBase::isPlayable() const
{
    return mGstPlayer->isPlayable();
}

bool MovieBase::isLoaded() const
{
    return mGstPlayer->isLoaded();
}
    
void MovieBase::setLoop( bool loop, bool palindrome )
{
    mGstPlayer->setLoop( loop, palindrome );
}
    
void MovieBase::setRate( float rate )
{
    mGstPlayer->setRate( rate );
}
    
void MovieBase::play()
{
    mGstPlayer->play();
}

void MovieBase::stop()
{
    mGstPlayer->stop();
}

int MovieBase::getWidth() const
{
    return mGstPlayer->width();
}

int MovieBase::getHeight() const
{
    return mGstPlayer->height();
}

bool MovieBase::isPlaying() const
{
    return !mGstPlayer->isPaused();
}


ci::SurfaceChannelOrder MovieBase::getChannelOrder() const
{
    switch( mGstPlayer->format() )
    {
        case GST_VIDEO_FORMAT_RGB:
            return ci::SurfaceChannelOrder::RGB;
        case GST_VIDEO_FORMAT_BGR:
            return ci::SurfaceChannelOrder::BGR;
        case GST_VIDEO_FORMAT_BGRx:
            return ci::SurfaceChannelOrder::BGRX;
        case GST_VIDEO_FORMAT_ARGB:
            return ci::SurfaceChannelOrder::ARGB;
        case GST_VIDEO_FORMAT_BGRA:
            return ci::SurfaceChannelOrder::BGRA;
        default:
            return ci::SurfaceChannelOrder::UNSPECIFIED;
    }
}
    
void MovieBase::seekToTime( float seconds )
{
    mGstPlayer->seekToTime( seconds );
}

void MovieBase::seekToStart()
{
    seekToTime(0);
}

void MovieBase::seekToEnd()
{
    // This triggers EOS. Probably needs revision.
    seekToTime(getDuration());
}

float MovieBase::getDuration() const
{
    return mGstPlayer->getDurationSeconds();
}
    
float MovieBase::getCurrentTime() const
{
    return mGstPlayer->getPositionSeconds();
}

bool MovieBase::checkNewFrame() const
{
    return mGstPlayer->hasNewFrame();
}

void MovieBase::setVolume( float targetVolume ) const
{
    return mGstPlayer->setVolume( targetVolume );
}
    
float MovieBase::getVolume() const
{
    return mGstPlayer->getVolume();
}
    
bool MovieBase::isDone() const
{
    return mGstPlayer->isDone();
}
    
// Movie Surface
MovieSurface::MovieSurface( const ci::fs::path &path )
{
    load( path.string() );    
}

MovieSurface::MovieSurface( const ci::Url &url )
{
    load( url.str() );
}
    
MovieSurface::MovieSurface( const GstCustomPipelineData &customPipeline )
{
    setCustomPipeline( customPipeline );
}

ci::Surface8uRef MovieSurface::getSurface()
{ 
    Surface8uRef surf;
    if( mGstPlayer->getVideoTexture() ) {
        surf = Surface8uRef( new ci::Surface8u( mGstPlayer->getVideoTexture()->createSource() ) );
    }
    return surf;
}
    
// MovieGl
MovieGl::MovieGl( const ci::fs::path &path )
{
    load( path.string() );
}

MovieGl::MovieGl( const ci::Url &url )
{
    load( url.str() );
}

MovieGl::MovieGl( const GstCustomPipelineData &customPipeline )
{
    setCustomPipeline( customPipeline );
}

ci::gl::Texture2dRef MovieGl::getTexture()
{
    return mGstPlayer->getVideoTexture();
}

} }
