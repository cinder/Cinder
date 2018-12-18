/**
 Defines some additional javascript to make some video related things easier.
 */
window.CINDER_ASYNC = {
  
  /**
   * An async loading function to use in place of ci::app::loadAsset
   * Pass in a string for the url and a callback function. 
   * @param {String} url the url of the asset you want to load
   * @param {Function} cb the callback function to call once the asset is done loading. 
   */
  loadAssetAsync:function( url,cb )
  {

    // =========== DETERMINE FILE TYPE ================ //
    
    // we can't decode video into raw info on the web without a lot of potentially time 
    // consuming manuvers so direct users to use Emscriptenvideo object instead.
    var videoExtensions = [ ".mp4", ".mov",".webm" ]
    videoExtensions.forEach( function( itm )
    {
      if( url.search( itm ) !== -1 )
      {
        console.error( "loadAsync was called seemingly with the intention of loading video. Please use EmscriptenVideo object instead. \n The path was " + url );
        return;
      }
    });

    // if audio file, load with existing helpers.
    // TODO not sure if things work similarly to WebGL where contexts are tied to specific contexts as in this case we'd make a new AudioContext.
    var audioExtensions = [ ".mp3",".ogg",".wav" ];
    var isAudio = false;
    audioExtensions.forEach( function( itm ) {
      if( url.search( itm ) !== -1)
      {
        CINDER_AUDIO.loadAudioFile( url,undefined,cb );
        isAudio = true;
      }
    });


    if( isAudio )
    {
      return;
    }

    
    var isImage = false;
    var imgExtensions = [ ".png",".jpg",".gif",".webp",".jpeg" ];
    imgExtensions.forEach( function( itm ) 
    {
      if( url.search( itm ) !== -1 )
      {
        isImage = true;
      }
    });

    // if image, use XMLHttpRequest to load binary data, otherwise use
    // fetch to load textual data.
    if( isImage )
    {
      var req = new XMLHttpRequest();
      req.open( 'GET', url );
      req.responseType = "arraybuffer"
      req.onload = function( e ) 
      {
        var byteArray = req.response;
  
        byteArray = new Uint8Array( byteArray );
        if( byteArray )
        {
          cb( byteArray );
        }
      }

      req.onerror = function()
      {
        console.error( "Unable to load file at ",url );
      }

      req.send();
    }
    else { 

      fetch( url ).then( function( res )
      {
        return res.text();
      }).then( function( data )
      {
        cb( data );
      }).catch( function()
      {
        console.error( "Issue loading ", url );
      })
    }
  }
};

window.CINDER_HELPERS = {

    /**
     * Logs the node to the console if you need to examine any attributes / values
     * @param {emscripten::val} e 
     */
    logNode:function(e){
      console.log(e);
    },
    // returns the browser you're using. Looking for Firefox / Chrome since those
    // present the best experiences. If it's not Firefox / Chrome, return full user agent string
    getBrowser:function()
    {
      //TODO
      if( navigator.userAgent.indexOf( 'FireFox' ) !== -1 )
      {
        return "Firefox";
      }
      else if( navigator.userAgent.indexOf( 'Chrome' ) !== -1 )
      {
        return "Chrome";
      }
      else {
        return navigator.userAgent;
      }
   },

  /**
   * Helper to quickly load data binary. Should be used when constructing 
   * your own html shell and you require a custom configuration that 
   * can't be met by just including {{SCRIPT}} - a similar function is included already when using 
   * default emscripten shell. 
   * @param {String} filename filename to load 
   * @param {String} srcFile path to js that will initiate asm.js/wasm load
   */
  loadEmterpreterBinary:function( filename,srcFile ) 
  {

    var emterpretURL = filename;
    var emterpretXHR = new XMLHttpRequest();
    emterpretXHR.open( 'GET', emterpretURL, true );
    emterpretXHR.responseType = 'arraybuffer';
    
    emterpretXHR.onload = function() 
    {
      if ( emterpretXHR.status === 200 || emterpretXHR.status === 0 )
      {
        Module.emterpreterFile = emterpretXHR.response;
      } 
      else {
        var emterpretURLBytes = tryParseAsDataURI( emterpretURL );
        
        if ( emterpretURLBytes )  
        {
          Module.emterpreterFile = emterpretURLBytes.buffer;
        }
      }
 
      var script = document.createElement( 'script' );
      script.src = srcFile;
      document.body.appendChild( script );
    };
 
    emterpretXHR.send( null );
 
  },
     
   /**
    * Returns a new emscripten::val value representing a TypedArray buffer.
    * @param {Number} size - the size for the array.
    */
   getFloatArray:function( size )
   {
       return new Float32Array( size );
   }
 };

// all audio related functionality
window.CINDER_AUDIO = {

    //! all of the nodes 
    workletNodes:[],

    // creates an AudioContext - does so to take into account of situations where
    // things deviate from the norm, ie safari requiring "webkitAudioContext" instead of just "AudioContext"
    createContext:function()
    {
        return new ( window.AudioContext || window.webkitAudioContext )();
    },

    /**
     * This is a helper function used by NativeWebAudioNodes to disconnect a script processor 
     * from the context when it's done converting it's data to something c++ compatible
     * @param {ScriptProcessorNode} processor an emscripten::val reference to the node's ScriptProcessorNode
     * @param {Number} delay the number of seconds to delay prior to disconnection to ensure all the data is processed. 
     */
    disconnectScriptFromDestination:function( processor,delay )
    {
      delay = delay !== undefined ? delay : 0;
      setTimeout(function()
      {
        processor.onaudioprocess = null;
        processor.disconnect( window.CINDER_AUDIO.globalContext[ "destination" ] );
      },delay);
    },

    /**
     * Connects 2 nodes to each other .
     * TODO probably should check to make sure they are both audio nodes.
     */
     connectNodes:function( node1,node2 )
     {
         if( node1 instanceof AudioNode && node2 instanceof AudioNode )
         {
             node1.connect( node2 );
         }
     },

    /**
     * Obtains an input audio stream to a microphone.
     * @param {Function} callback - a callback value that is used to receive the stream when available
     * @param {string} deviceId - an optional device id that can be used to select a particular device.
    */
    loadAudioInput:function( callback,deviceId )
    {
        if( navigator.mediaDevices.getUserMedia ) 
        {

          // get a stream to the specified device, if one isn't stated,
          // get stream to default input device.
          navigator.mediaDevices.getUserMedia({
            audio: deviceId !== undefined || deviceId !== "" ? {deviceId:deviceId} : true,
            video:false
          }).then( function( stream )
          {
            // note that "stream" will be received as an emscripten::val object.
            callback( stream );
          }).catch( function( err )
          {
         
            if( err )
            {
              console.error( "There was an issue obtaining a media stream ",err );
              throw err;
            }
          });
     }
     else{
       alert( "Your browser does not support getUserMedia :( so unfortunately we're unable to obtain audio input " );
     }
   },

   /**
    * Returns the list of available input devices
    * @param {Function} callback - the function to use to receive the list of available callback devices.
    */
   getAvailableDevices:function( callback )
   {
     navigator.mediaDevices.enumerateDevices().then(function( devices )
     {
       callback( devices );
     })
   },

   // the global context for all audio processing
   // This is done due to the issue of the audio context reference being
   // deleted when attempted to use outside of the class.
   globalContext:new AudioContext(),

   /**
    * Loads an audio file and parses it's audio information.
    * If for some reason something goes wrong, will log error to the dev tools console.
    * @param {string} path - path to the audio file.
    * @param {AudioContext} context - an emscripten::val representing a WebAudio context(aka AudioContext object)
    * @param {Function} callback - an emscripten::val representing a callback function that will receive the decoded audio information.
    */
   loadAudioFile:function( path,context,callback )
   {
    // use a new audio context if user did not pass one in. 
    context = context !== undefined ? context : new AudioContext();

    fetch( path ).then(function( res )
    {
       //perhaps one of the great failings of the fetch api - need to
       // return audio as an ArrayBuffer first.
       return res.arrayBuffer();
     }).then(function( buffer )
     {
       // decode the audio data
       context.decodeAudioData( buffer,function( data )
       {
         callback(data);
       },function( err )
       {
         console.error( "error decoding audio information - ",err );
       });

     }).catch(function( err )
     {
       console.log("\n");
       console.error("There was an error fetching the audio file at " + path);
       console.error("The error message was ",err);
       console.log("\n");
     })
   },

   // alias for the loadAudioFile
   loadAudio:function( path,context,callback )
   {
     window.CINDER_AUDIO.loadAudioFile( path,context,callback );
   }
 }

 // for all video related functionality.
window.CINDER_VIDEO = {
  // some css to style warnings
  warnCSS:[
    "font-size:15px",
    "color:red",
    "background:yellow"
  ],

  /**
   * Creates an HTML5 Video element
   * @return {<video>} an HTML5 Video element.
   */
  createVideo:function()
  {

    // =========== BUILD OUT OUR SPECIAL VIDEO ELEMENT ================= //

    // normally bad practice but for convinience sake, gonna augment a video object with
    // some extra methods and helpers.

    var video = document.createElement( "video" );

    // get the width of the texture
    video.getWidth = function()
    {
      return video.videoWidth;
    }

    // get the height of the texture
    video.getHeight = function()
    {
      return video.videoHeight;
    };

    // sets the texture onto the video object for easy access.
    // Note that GL.textures is populated by Emscripten.
    // All ci::Texture::create calls will make textures that get populated into
    // that array.
    video.setTexture = function( id )
    {
      this.texture = GL.textures[ id ];
    }

    // Call to update the video element's texture with new information.
    video.updateTexture = function()
    {
      var texture = this.texture;
      var gl = GLctx;

      gl.bindTexture( gl.TEXTURE_2D,texture );
      gl.texImage2D( gl.TEXTURE_2D,0,gl.RGBA,this.videoWidth,this.videoHeight,0,gl.RGBA,gl.UNSIGNED_BYTE,this );
      gl.bindTexture (gl.TEXTURE_2D,null );
    }

    // allow cross-origin videos to be able to play.
    // Note that you will have to set up external servers to handle cross-origin requests. 
    video.crossOrigin = "anonymous";

    return video;
  }
}

/**
 * Deals with . 
 */
window.CINDER_FILEIO = {

  /**
   * Converts a source string into a Blob object that's suitable for 
   * passing into new WebWorker and AudioWorklet instances. 
   * @param {String} source the source string that you want to make into a blob. 
   */
  sourceToBlob:function(source){
    var blob = new Blob([source],{
      type:'application/javascript'
    });

    return URL.createObjectURL(blob);
    
  },

  /**
   * Loads a WebWorker. 
   * @param {String} source Pass in the source for that web worker.  
   * @returns {WebWorker} returns an emscripten::val instance of the WebWorker object.
   */
  loadWorker:function(source){
    return new Worker(this.sourceToBlob(source));
  },

  /**
   * Loads an AudioWorklet processor script
   * @param {String} source The source for the script
   * @param {Function} cb a function to run when the script has been instantiated and registered.
   */
  loadProcessor:function(source,cb){
    var context = window.CINDER_AUDIO.globalContext;

    context.audioWorklet.addModule(this.sourceToBlob(source)).then(cb)
      .catch( function() {
        console.error("There was an error loading the processor");
      });
  }
}


// =================== TESTING =======================//


window.CINDER_WORKLETS = {

  // an array holding the chain of nodes to string together. 
  // will start out reversed, but eventually re-order itself properly
  graph:[],

  /**
   * Creates a AudioWorkletNode that is used to handle custom processing. 
   * @param {Function} processFunc 
   */
  createWorkletNode(processFunc){
   
   this.graph.push(processFunc);

  },

  /**
   * Removes all items in the graph;
   */
  clearGraph:function(){
    this.graph = [];
  },


  /**
   * Goes through the graph and executes commands where necessary
   * to finish initializing everything. Afterwards things get connected. 
   */
  sortGraph:function()
  {
    // first reverse graph cause things are gonna get inserted in the wrong order for 
    // ease of use
    let graph = this.graph.reverse();

    // gotta remove the first item for some reason - probably cause we aren't setting AudioContext as "enabled"
    graph.shift();

    // check graph to see if any items are just functions, if they are we they are custom nodes and we need to instantiate
    // a custom AudioWorketNode
    graph = graph.map(function(itm) {
      if(itm !== undefined){
        if(typeof itm === "function"){
          return new CustomNode(itm);
         
        }else{
          return itm;
        }
      }
    });

    // connect all the nodes together. 
    for(let i = 0; i < graph.length-1; ++i){
      if(i !== graph.length - 1){
        let start = graph[i];
        let next = graph[i + 1];
        start.connect(next);
      }
    }
  }

};