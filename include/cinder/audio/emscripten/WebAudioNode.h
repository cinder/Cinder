#pragma once
#include "cinder/audio/Node.h"
#include <emscripten/bind.h>

namespace cinder { namespace audio { namespace em {

  typedef std::shared_ptr<class WebAudioNode> WebAudioNodeRef;

  // a list of built in nodes that can be created.
  // Note that we're leaving out nodes that are already made in the CinderAPI
  // See "Interfaces" side menu on https://developer.mozilla.org/en-US/docs/Web/API/Web_Audio_API
  enum WebAudioApiNode 
  {
    BiquadFilterNode,
    ConvolverNode,
    DelayNode,
    DynamicsCompressorNode,
    IIRFilterNode,
    PeriodicWave,
    WaveShaperNode,
    StereoPannerNode
  };

  class CI_API WebAudioNode: public Node 
  {
    emscripten::val mRawNode;

    // refers to whether or not a native node was set. If so,
    // we need to extract the native data to pass along / add to any inputs.
    bool mNativeNodeSet;

    public:
    	WebAudioNode( const Format &format = Format() ):
      Node( format ),
      mNativeNodeSet(false),
      mRawNode(emscripten::val::undefined()){
        mNativeNode = true;
      }

      //! sets a property on the native WebAudio node.
      template<typename T>
      void setParameter(std::string name,T value){
        mRawNode.set(name,val(value));
      }

      // runs when something connects to this node.
      void connect( const NodeRef &output )
      {

        // TODO convert any native data and add it to Cinder data.
        // if this is a native web audio node
        if( output->mNativeNode )
        {
        
        }
        else {
          // get the buffer of the output
          auto oBuffer = output->getInternalBuffer();
        }
      }

      // runs when this node gets connected to something.
      void connectInput( const NodeRef &output )
      {
        CI_LOG_I("connect input called");

        if( output->mNativeNode )
        {
        }
        else {
             //getInputs().insert(output);
        }

      }

      //! builds a new instance of an native WebAudio API node.
      void makeNativeNode( WebAudioApiNode nodeName ) 
      {
          // get a reference to the underlying ContextWebAudio's refernce to the actual WebAudioAPI context
          auto ctx = (ContextWebAudio*)getContext().get();
          auto audioCtx = ctx->getRawContext();

          // figure out which node to create
          switch(nodeName){
            case BiquadFilterNode:
              mRawNode = audioCtx.call<emscripten::val>("createBiquadFilter");
             break;

            case ConvolverNode:
              mRawNode = audioCtx.call<emscripten::val>("createConvolver");
             break;

            case DelayNode:
              mRawNode = audioCtx.call<emscripten::val>("createDelay");
             break;

            case DynamicsCompressorNode:
              mRawNode = audioCtx.call<emscripten::val>("createDynamicsCompressor");
             break;

            case IIRFilterNode:
              mRawNode = audioCtx.call<emscripten::val>("createIIRFilter");
             break;

            case PeriodicWave:
              mRawNode = audioCtx.call<emscripten::val>("createPeriodicWave");
             break;

            case WaveShaperNode:
              mRawNode = audioCtx.call<emscripten::val>("createWaveSharper");
             break;

            case StereoPannerNode:
              mRawNode = audioCtx.call<emscripten::val>("createStereoPanner");
             break;
           }
        }
    };
}}}
