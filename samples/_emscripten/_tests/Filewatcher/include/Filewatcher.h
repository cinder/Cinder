#include <emscripten.h>
#include <emscripten/val.h>
#include "WatcherFunctions.h"
#include "cinder/emscripten/CinderEmscripten.h"

using namespace ci::em::watcher;
/**
 * A basic file watcher for Cinder Emscripten. 
 * Note that this will not be suitable for .cpp files and other lower-level source code but
 * is instead intended for any higher level sort files you might want to keep an eye on.
 */
class Filewatcher 
{


    //! string url of the socket server to connect to. 
    std::string url;

    public:
    Filewatcher( int port=3400 )
    {
        url = "ws://localhost:" + std::to_string( port );
     
        // we connect to the server right away assuming that the server is already running. 
        connect_to_server( url.c_str() );   
    }

    void addFile( std::string path,std::function<void( emscripten::val )> cb ) 
    {
        // add file to callbacks so we can lookup the 
        emscripten::val watcher = emscripten::val::global( "window" )[ "CINDER_WATCHER" ];

        // add entry to global callbacks array so we can trigger different functions
        // depending on which file was changed. 
        watcher.call<void>( "addFile",emscripten::val( path ),ci::em::helpers::generateCallback( cb ) );

        // add file to chokidar(the filewatcher) 
        add_file_to_watcher( path.c_str() );
    }
    
    //! For when you want to add a bunch of files to watch. 
    void addFiles( std::vector<std::string> files,std::function<void( emscripten::val )> cb )
    {
        // add file to callbacks so we can lookup the 
        emscripten::val watcher = emscripten::val::global( "window" )["CINDER_WATCHER"];

        for( std::string &path : files )
        {
            // add entry to global callbacks array so we can trigger different functions
            // depending on which file was changed. 
            watcher.call<void>( "addFile",emscripten::val( path ),ci::em::helpers::generateCallback( cb ) );
            // add file to chokidar(the filewatcher) 
            add_file_to_watcher( path.c_str() );
        }
    }
};