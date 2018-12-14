#include <emscripten.h>

/**
 * note - websocket packet should look like 
 * {
 *  type:<message type>
 *  data:<data for the message>
 * }
 */

namespace cinder { namespace em { namespace watcher {

     // server connection function. Call to connect to the server. 
    // note that it assumes the server is already running. 
    EM_JS( void,connect_to_server,( const char * serverurl ), {

        // note that we can only have one client at a time any new instances will just overwrite the 
        // current socket client. 
        window.CINDER_WATCHER.client = new WebSocket( UTF8ToString( serverurl ) );

        // messages should be passed back and forth as a type / data pair. something like 
        // {type:"this is my message type" , data:"this is my payload"}
        window.CINDER_WATCHER.client.onmessage = function( msg ){
            msg = JSON.parse( msg.data );

            switch( msg.type ){
                case "FILE_CHANGED":
                window.CINDER_WATCHER.processServerResponse( msg.data );
                break;
            }
        }
    });

    // add file function - tells server to add a file to the watch list. 
    EM_JS( void,add_file_to_watcher,( const char * filepath ), {
        let file = UTF8ToString( filepath) ;
        let client = window.CINDER_WATCHER.client;
        
        let timer = setInterval(() => {
            if( client.readyState === 1 )
            {
                client.send( JSON.stringify({
                    type:"ADD_FILE",
                    data:file
                }));
                
                clearInterval( timer );
            }

        });
    });
}}}