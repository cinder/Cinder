const argv = require('minimist')(process.argv.slice(2));
const server = require('../index')

let files = [];
if(argv.files){
    // look for any files we know we want to watch right away. 
    // files parameter should take a string seperated by commas 
    files = argv.files.split(",");
}

// set the port on which to operate the server
let port = argv.port  !== undefined ? argv.port : 3400;

// the root path to where your files ought to be. if not specified, 
// the root defaults to wherever your script is being run from. 
let root = argv.root !== undefined ? argv.root : "./"

// start the server. 
server({
    files:files,
    port:port
});