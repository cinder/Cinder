
//#include "cinder/audio/emscripten/ContextWebAudio.h"
#include "ContextWebAudio.h"
#include "cinder/audio/dsp/Dsp.h"

using namespace emscripten;
using namespace std;

namespace cinder { namespace audio { namespace em {

  // ============ CONTEXT ============== //
  ContextWebAudio::ContextWebAudio():
  mScriptNode( emscripten::val::undefined() ),
  mOscNode( emscripten::val::undefined() ),
  mBufferSize( 0 ),
  mCallbackFunction( emscripten::val::undefined() )
  {
    emscripten::val mAudioContext = getGlobalContext();
    mScriptNode = mAudioContext.call<emscripten::val>( "createScriptProcessor",0,1,mAudioContext["destination"]["channelCount"].as<int>() );

  }

  emscripten::val ContextWebAudio::getGlobalContext()
  {
    return val::global( "window" )[ "CINDER_AUDIO" ][ "globalContext" ];
  }

  emscripten::val ContextWebAudio::getRawContext()
  {
    return getGlobalContext();
  }

  void ContextWebAudio::setOutputBufferSize( int size )
  {
      emscripten::val mAudioContext = getGlobalContext();
      
      // if the bufferSize prop is wrong, disconnect, then re-connect with the correct size
      if(size != mScriptNode[ "bufferSize" ].as<int>())
      {

        mBufferSize = size;

        // disconnect ScriptProcessorNode from Oscillator.
        //mOscNode.call<void>("disconnect",mScriptNode);

        // disconnect script node to output
        mScriptNode.call<void>("disconnect",mAudioContext["destination"]);

        // rebuild ScriptProcessorNode with the correct buffer size.
        mScriptNode = mAudioContext.call<emscripten::val>( "createScriptProcessor",size,1,1 );

        // re-init all connections
        setRenderFunction();
      }
  }

  int ContextWebAudio::getChannelCount()
  {
    return getRawContext()[ "destination" ][ "channelCount" ].as<int>();
  }

  int ContextWebAudio::getBufferSize()
  {
    return mBufferSize;
  }

  void ContextWebAudio::connectNativeNode( emscripten::val input )
  {
    //getGlobalContext().call<void>("connect",input);
    input.call<void>( "connect",mScriptNode );
  }

  void ContextWebAudio::setRenderFunction()
  {
    emscripten::val mAudioContext = getGlobalContext();

    // set callback of script node
    mScriptNode.set( "onaudioprocess",mCallbackFunction );

    // create oscillator object to play back script node
    //mOscNode = mAudioContext.call<val>("createOscillator");

    // connect script node to oscillator
    //mOscNode.call<void>("connect",mScriptNode);

    // connect script node to output
    mScriptNode.call<void>( "connect",mAudioContext[ "destination" ] );

    // start script node
    //mOscNode.call<void>("start");
  }

  void ContextWebAudio::setRenderFunction( std::function<void(val e)> functor )
  {
    mCallbackFunction = ci::em::helpers::generateCallback( functor );
    setRenderFunction();
  }

  ContextWebAudio::~ContextWebAudio(){}

  OutputDeviceNodeRef ContextWebAudio::createOutputDeviceNode( const DeviceRef &device, const Node::Format &format )
  {
    auto thisRef = std::dynamic_pointer_cast<ContextWebAudio>( shared_from_this() );
    auto result = makeNode( new OutputDeviceNodeWebAudio( device, format, thisRef ) );
    mDeviceNodes.push_back( result );
    return result;
  }
  
  InputDeviceNodeRef ContextWebAudio::createInputDeviceNode( const DeviceRef &device, const Node::Format &format )
  {
    auto thisRef = std::dynamic_pointer_cast<ContextWebAudio>( shared_from_this() );
    auto result = makeNode( new InputDeviceNodeWebAudio( device, format, thisRef ) );
    mDeviceNodes.push_back( result );
    return result;
  }

   // ==============================

   // TODO this may technically not end up really being needed.
   struct OutputDeviceNodeWebAudioImpl 
   {

    OutputDeviceNodeWebAudioImpl( OutputDeviceNodeWebAudio* parent, const std::shared_ptr<ContextWebAudio>& context )
     :mParent( parent ), mCinderContext( context )
     {}

    void connectNativeNode( emscripten::val node )
    {
      mCinderContext->connectNativeNode( node );
    }

    void setOutputBufferSize( int size )
    {
      mCinderContext->setOutputBufferSize( size) ;
    }

    void setRenderFunction( std::function<void(val e)> functor )
    {
      mCinderContext->setRenderFunction( functor );
    }

     emscripten::val getRawContext()
     {
       return mCinderContext->getRawContext();
     }

     OutputDeviceNodeWebAudio * mParent = nullptr;
   	 std::shared_ptr<ContextWebAudio>	mCinderContext;
   };
   // ===============================

   InputDeviceNodeWebAudio::InputDeviceNodeWebAudio( const DeviceRef &device, const Format &format, const std::shared_ptr<ContextWebAudio> &context ):
   InputDeviceNode( device, format ),
   mStreamRef( emscripten::val::undefined() ),
   mInputStreamAquired( false ),
   mMediaStream( emscripten::val::undefined() ),
   mAudioContext( context ) 
   {

      // initialize stream
      auto utils = ci::em::helpers::getAudioHelpers();
      std::function<void(emscripten::val e)> cb = [this]( emscripten::val e )->void
      {
        // save stream reference
        mStreamRef = e;

        auto ctx = mAudioContext->getRawContext();

        // turn stream we aquired into something consumable by the output.
        mMediaStream = ctx.call<emscripten::val>( "createMediaStreamSource",mStreamRef );

        // signal that we have an input stream.
        mInputStreamAquired = true;
      };

      // get and check device id / name - if it matches default then pass in blank string.
      std::string name = device->getName();
      if( name == "WebAudioInputDevice" )
      {
        name = "";
      }

      // indicate that this is a native node and thus, needs slightly different processing.
      mIsNativeWebNode = true;

      // initialize stream - returns emscripten::val object
      mStreamRef = utils.call<emscripten::val>( "loadAudioInput",ci::em::helpers::generateCallback( cb ),val( name ) );
   }

   void InputDeviceNodeWebAudio::initialize()
   {
     //CI_LOG_I("InputDeviceNode :: initialize was triggered - note that you don't have to call 'enable'");
     // nothing to do here - we already initialize things when we create the media stream.
   }
   // ===============================
   OutputDeviceNodeWebAudio::OutputDeviceNodeWebAudio( const DeviceRef &device, const Format &format, const std::shared_ptr<ContextWebAudio> &context ):
   OutputDeviceNode( device, format )
    {
        mIsOutputDevice = true;
        mImpl = std::unique_ptr<OutputDeviceNodeWebAudioImpl>( new OutputDeviceNodeWebAudioImpl( this,context ) );
    }

  emscripten::val OutputDeviceNodeWebAudio::getRawContext()
  {
    return mImpl->getRawContext();
  }

  void OutputDeviceNodeWebAudio::connectNativeNodeToOutput( emscripten::val node )
  {
     mImpl->connectNativeNode(node);
  }

  void OutputDeviceNodeWebAudio::renderInputs( emscripten::val e )
  {
    auto outputBuffer = e["outputBuffer"];
    auto inputBuffer = e["inputBuffer"];

    auto ctx = getContext();
    if( ! ctx )
    {
       CI_LOG_I( "can't get context" );
       return;
    }

    // this technically doesn't do anything but leave it here for now.
    lock_guard<mutex> lock( ctx->getMutex() );

    ctx->preProcess();

      
    auto internalBuffer = getInternalBuffer();
    internalBuffer->zero();
    pullInputs( internalBuffer );

    if( checkNotClipping() )
    {
      internalBuffer->zero();
    }

    const size_t numFrames = internalBuffer->getNumFrames();
    const size_t numChannels = internalBuffer->getNumChannels();

    //dsp::interleave( internalBuffer->getData(), outputBuffer, numFrames, numChannels, numFrames );

    ctx->postProcess();

    // =========================================== //

    // make sure that the ScriptNodeProcessor's bufferSize prop is set properly.
    mImpl->setOutputBufferSize( numFrames );

    // output array is what gets directed towards speakers
    val output = outputBuffer.call<val>( "getChannelData", 0 );

    // input is only going to be populated when using native WebAudio nodes.
    val input = inputBuffer.call<val>( "getChannelData",0 );

    // check first 100 values to see if input buffer is filled or empty(aka the value is 0). If empty, we won't bother processing,
    bool mInputIsEmpty = true;
    for( int a = 0; a < 100; ++a )
    {
      if( input[a].as<float>() != 0.0 )
      {
        mInputIsEmpty = false;
      }
    }

    // get internal cinder data
    float * data = static_cast<float*>( internalBuffer->getData() );
    float * finalData;
    float * idata;


    if( !mInputIsEmpty ) 
    {

      // will hold input data
      std::vector<float> _inputData;

      // copy Float32Array(aka "input") into _inputData vector
      ci::em::helpers::copyToVector( input,_inputData );

      // convert to float* so we can pass information to dsp methods
      idata = &_inputData[0];

      // add input data to any cinder data
      dsp::add(idata,data,finalData,numFrames);

    }
    else{

      // if no input data just pass through
      finalData = data;
    }

    // loop through and set all info from finalData pointer onto the output buffer.
    for( int i = 0; i < numFrames; ++i )
    {
       output.set<float>( i,finalData[i] );
    }


      // release finalData pointer.
      // releasing these causes things to crash :(
      //free(finalData);
      //free(idata);
      
  }

  void OutputDeviceNodeWebAudio::initialize()
  {
    CI_LOG_I( "OutputDevicenode :: Initialize called" );

    const size_t sampleRate = getOutputSampleRate();
    const size_t framesPerBlock = getOutputFramesPerBlock();
    const size_t numChannels = getNumChannels();

    mInterleavedBuffer = BufferInterleaved( framesPerBlock, numChannels );

    auto functor = std::bind( &OutputDeviceNodeWebAudio::renderInputs,this,std::placeholders::_1 );
    mImpl->setRenderFunction( functor );

  }
}}}