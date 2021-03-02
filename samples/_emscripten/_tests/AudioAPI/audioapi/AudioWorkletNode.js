/**
 * A custom AudioWorklet node. When the main AudioWorkletProcessor receives information, 
 * the _handleMessage function will get called which in turn will call the process() function
 * of your CustomNode instance. 
 */
class CustomNode extends AudioWorkletNode {
    constructor(processingFunction){
      super(CINDER_AUDIO.globalContext,"CinderAudioProcessor");
      this.port.onmessage = this._handleMessage.bind(this);
      this.processingFunction = processingFunction;
    }
      /**
     * When a message is received from the procesor - send the inputs and outputs
     * to our C++ function which should have been passed in as the "processingFunction"
     * in the constructor
     */ 
    _handleMessage( e )
    { 
      
        this.processingFunction( e.data );
    }
  }