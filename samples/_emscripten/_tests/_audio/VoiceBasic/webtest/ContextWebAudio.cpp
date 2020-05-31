
//#include "cinder/audio/emscripten/ContextWebAudio.h"
#include "ContextWebAudio.h"
using namespace emscripten;
using namespace std;
namespace cinder { namespace audio { namespace em {
  // ============ CONTEXT ============== //
  ContextWebAudio::ContextWebAudio():mContext(emscripten::val::undefined()),
  mScriptNode(emscripten::val::undefined()),
  mOscNode(emscripten::val::undefined()),
  mCallbackFunction(emscripten::val::undefined()){

    mContext = val::global("window")["CINDER_AUDIO"].call<val>("createContext");
    mScriptNode = mContext.call<emscripten::val>("createScriptProcessor",0,1,1);

  }
  emscripten::val ContextWebAudio::getRawContext(){

    return mContext;
  }
  void ContextWebAudio::setOutputBufferSize(int size){
      // if the bufferSize prop is wrong, disconnect, then re-connect with the correct size
      if(size != mScriptNode["bufferSize"].as<int>()){

        // disconnect ScriptProcessorNode from Oscillator.
        mOscNode.call<void>("disconnect",mScriptNode);

        // disconnect script node to output
        mScriptNode.call<void>("disconnect",mContext["destination"]);

        // rebuild ScriptProcessorNode with the correct buffer size.
        mScriptNode = mContext.call<emscripten::val>("createScriptProcessor",size,1,1);

        // re-init all connections
        setRenderFunction();
      }
  }

  void ContextWebAudio::setRenderFunction(){

    // set callback of script node
    mScriptNode.set("onaudioprocess",mCallbackFunction);

    // create oscillator object to play back script node
    mOscNode = mContext.call<val>("createOscillator");

    // connect script node to oscillator
    mOscNode.call<void>("connect",mScriptNode);

    // connect script node to output
    mScriptNode.call<void>("connect",mContext["destination"]);

    // start script node
    mOscNode.call<void>("start");
  }
  void ContextWebAudio::setRenderFunction(std::function<void(val e)> functor){

    mCallbackFunction = ci::em::helpers::generateCallback(functor);
    setRenderFunction();
  }

  ContextWebAudio::~ContextWebAudio(){}

  OutputDeviceNodeRef ContextWebAudio::createOutputDeviceNode(const DeviceRef &device, const Node::Format &format){
    auto thisRef = std::dynamic_pointer_cast<ContextWebAudio>( shared_from_this() );
    auto result = makeNode( new OutputDeviceNodeWebAudio( device, format, thisRef ) );
    mDeviceNodes.push_back( result );
    return result;
  }

   // ==============================
   struct OutputDeviceNodeWebAudioImpl {

     OutputDeviceNodeWebAudioImpl( OutputDeviceNodeWebAudio* parent, const std::shared_ptr<ContextWebAudio>& context )
     :mParent( parent ), mCinderContext( context ){
         //CI_LOG_I("OutputDeviceNodeImpl requests");

     }

     void setOutputBufferSize(int size){
       mCinderContext->setOutputBufferSize(size);
     }

     void setRenderFunction(std::function<void(val e)> functor){
        mCinderContext->setRenderFunction(functor);
     }

     emscripten::val getRawContext(){
       CI_LOG_I("obtained raw context");
       return mCinderContext->getRawContext();
     }
     OutputDeviceNodeWebAudio * mParent = nullptr;
   	 std::shared_ptr<ContextWebAudio>	mCinderContext;
   };
   // ===============================
   	OutputDeviceNodeWebAudio::OutputDeviceNodeWebAudio(const DeviceRef &device, const Format &format, const std::shared_ptr<ContextWebAudio> &context ):
        OutputDeviceNode( device, format ){
        mImpl = std::unique_ptr<OutputDeviceNodeWebAudioImpl>(new OutputDeviceNodeWebAudioImpl(this,context));
    }

    emscripten::val OutputDeviceNodeWebAudio::getRawContext(){
      return mImpl->getRawContext();
    }

    void OutputDeviceNodeWebAudio::renderInputs(emscripten::val e){
      auto outputBuffer = e["outputBuffer"];
      auto inputBuffer = e["inputBuffer"];

      val output = outputBuffer.call<val>("getChannelData", 0);
      val input = inputBuffer.call<val>("getChannelData",0);

      auto ctx = getContext();
      if( ! ctx ){
        CI_LOG_I("can't get context");
        return;
      }

      // this technically doesn't do anything but leave it here for now.
      lock_guard<mutex> lock( ctx->getMutex() );




      ctx->preProcess();

      auto internalBuffer = getInternalBuffer();
      internalBuffer->zero();
      pullInputs( internalBuffer );

      if( checkNotClipping() ){
        internalBuffer->zero();
      }


      const size_t numFrames = internalBuffer->getNumFrames();
      const size_t numChannels = internalBuffer->getNumChannels();
      //dsp::interleave( internalBuffer->getData(), outputBuffer, numFrames, numChannels, numFrames );

      ctx->postProcess();

      // make sure that the ScriptNodeProcessor's bufferSize prop is set properly.
      mImpl->setOutputBufferSize(numFrames);

      float * data = static_cast<float*>(internalBuffer->getData());
      for(int i = 0; i < 512; ++i){


        auto _input = input[i].as<float>();
        output.set<float>(i, data[i]);
      }


    }

    void OutputDeviceNodeWebAudio::initialize()
    {
      //CI_LOG_I("Initialize called");
    	const size_t sampleRate = getOutputSampleRate();
    	const size_t framesPerBlock = getOutputFramesPerBlock();
    	const size_t numChannels = getNumChannels();

    	mInterleavedBuffer = BufferInterleaved( framesPerBlock, numChannels );

      auto functor = std::bind(&OutputDeviceNodeWebAudio::renderInputs,this,std::placeholders::_1);
      mImpl->setRenderFunction(functor);


      /*
      // recursively re-build graph so we know what to tie into
      // the WebAudio api.
      auto inputs = getInputs();

      std::set<NodeRef>::iterator it = inputs.begin();
      for(; it != inputs.end(); ++it){
        gatherInputs(*it);
      }
      mImpl->playAudio()

      */

      // take all the values in the graph


    }


}}}
