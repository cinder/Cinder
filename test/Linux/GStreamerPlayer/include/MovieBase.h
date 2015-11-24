#pragma once 

#include "GstPlayer.h"
#include "cinder/Surface.h"
#include "cinder/Url.h"
#include "cinder/gl/gl.h"

using namespace cinder;

namespace gst { namespace video {

typedef std::shared_ptr<GstPlayer> GstPlayerRef;

class MovieBase
{
    public:
        virtual ~MovieBase();

        void    setCustomPipeline( const GstCustomPipelineData &customPipeline );
    
        void    load( const ci::fs::path &path );
        void    load( const ci::Url &url );
    
        int     getWidth() const;
        int     getHeight() const;

        ivec2   getSize() const { return ivec2( getWidth(), getHeight() ); }
    
        Area    getBounds() const{ return Area( 0, 0, getWidth(), getHeight() ); }

        ci::SurfaceChannelOrder getChannelOrder() const;

        bool    isPlayable() const;
        bool    isLoaded() const;

        float   getDuration() const;
        float   getCurrentTime() const;

        bool    checkNewFrame() const;

        void    seekToTime( float seconds );
        void    seekToStart();
        void    seekToEnd();

        void	setLoop( bool loop = true, bool palindrome = false );
        void    setRate( float rate );

        void    setVolume( float targetVolume ) const;
        float   getVolume() const;
    
        bool    isPlaying() const;
        bool    isDone() const;

        void    play();
        void    stop();

    protected:
        MovieBase();
        GstPlayerRef    mGstPlayer;

    private:
        void    loadNewMovie( std::string path );
};

class MovieSurface;
typedef std::shared_ptr<MovieSurface> MovieSurfaceRef;
class MovieSurface : public MovieBase {
    public:
        virtual ~MovieSurface(){}
        static MovieSurfaceRef create( const ci::fs::path &path ){ return MovieSurfaceRef( new MovieSurface( path ) ); }
        static MovieSurfaceRef create( const ci::Url &url ){ return MovieSurfaceRef( new MovieSurface( url ) ); }
        static MovieSurfaceRef create( const GstCustomPipelineData &customPipeline ){ return MovieSurfaceRef( new MovieSurface( customPipeline ) ); }

    
        ci::Surface8uRef getSurface();

    protected:
        MovieSurface() : MovieBase(){}
        MovieSurface( const ci::fs::path &path );
        MovieSurface( const ci::Url &url );
        MovieSurface( const GstCustomPipelineData &customPipeline );

};

class MovieGl;
typedef std::shared_ptr<MovieGl> MovieGlRef;
class MovieGl : public MovieBase {
public:
    virtual ~MovieGl(){}
    static MovieGlRef create( const ci::fs::path &path ){ return MovieGlRef( new MovieGl( path ) ); }
    static MovieGlRef create( const ci::Url &url ){ return MovieGlRef( new MovieGl( url ) ); }
    static MovieGlRef create( const GstCustomPipelineData &customPipeline ){ return MovieGlRef( new MovieGl( customPipeline ) ); }

    ci::gl::Texture2dRef getTexture();
    
protected:
    MovieGl() : MovieBase(){}
    MovieGl( const ci::fs::path &path );
    MovieGl( const ci::Url &url );
    MovieGl( const GstCustomPipelineData &customPipeline );

};
    
} }
