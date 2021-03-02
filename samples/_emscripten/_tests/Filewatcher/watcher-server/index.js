const chokidar = require( "chokidar" );
const ws = require( "ws" );

module.exports = function( options )
{
    let files = options.files;
    let port = options.port;
    let root = options.root;
    let watcher = null;

    // start watching known files
    if( files.length > 0 )
    {
        //start watcher.
        watcher = chokidar.watch( files );
    }
    else { 
        // if no initial dataset, pass in blank array and start watcher. 
        watcher = chokidar.watch( [] );
    }

    // start socket server 
    console.log( "starting server on port ", port )

    // start Socket server
    let wss = new ws.Server({
        port:port
    })    

    
    wss.on('connection', socket => {

        // when we get a message from client, process. 
        socket.on('message', msg => {
            msg = JSON.parse( msg );
            
            switch(msg.type)
            {
                case "ADD_FILE":
                    // add path to watcher
                    watcher.add(msg.data);
                break;
            }
        });

        // when the file changes ... 
        watcher.on('change', path => {
            let id = path.split("/");
            id = id[id.length - 1];

            console.log("file changed", path);

            // send a message back saying which file changed. 
            // so we can lookup the appropriate callback. 
            socket.send(JSON.stringify({
                type:"FILE_CHANGED",
                data:id
            }))
        });
        console.log("A connection was made");
    })
}