var http = require('http')
var serveStatic = require('serve-static')
var finalhandler = require('finalhandler')


module.exports = function(port){

    var PORT = port !== undefined ? port : 3000;

    var serve = serveStatic(process.cwd(), {
        'index':['index.html','index.htm'],
        setHeaders:function(res,path,stat){
            if (path.search('.wasm') !== -1) {
                res.setHeader('Content-type','application/wasm')
            }
        }
    });

    var server = http.createServer(function(req,res){
        serve(req,res,finalhandler(req,res));
    })

    server.listen(PORT);
    console.log("Starting server on port " + PORT + ". Press ctrl/cmd - c to stop")
}