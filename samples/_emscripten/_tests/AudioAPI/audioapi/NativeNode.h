
#pragma once
#include <memory>
#include <emscripten/val.h>
#include "cinder/Log.h"
#include "AudioNode.h"
#include "cinder/emscripten/CinderEmscripten.h"



    enum WebAudioApiNode 
    {
        BiquadFilterNode,
        ConvolverNode,
        DelayNode,
        DynamicsCompressorNode,
        IIRFilterNode,
        PeriodicWaveNode,
        WaveShaperNode,
        ChannelMergerNode,
        ChannelSplitterNode,
        StereoPannerNode,
        GainNode,
        AnalyserNode,
        OscillatorNode
    };
    const static std::map<WebAudioApiNode,std::string> NativeWebTypes = 
    {
        { AnalyserNode,"createAnalyser" },
        { BiquadFilterNode,"createBiquadFilter" },
        { ChannelMergerNode, "createChannelMerger" },
        { ChannelSplitterNode,"createChannelSplitter" },
        { DelayNode,"createDelay" },
        { ConvolverNode,"createConvolver" },
        { GainNode,"createGain" },
        { PeriodicWaveNode,"createPeriodicWave" }, 
        { WaveShaperNode,"createWaveSharper" },
        { StereoPannerNode,"createStereoPanner" },
        { DynamicsCompressorNode,"createDynamicsCompressor" },
        { OscillatorNode , "createOscillator" }
    };


namespace cinder {


typedef std::shared_ptr<class NativeNode> NativeNodeRef;
class NativeNode : public AudioNode 
{
    public:
        NativeNode(WebAudioApiNode type=GainNode):mNode(emscripten::val::undefined()){
     

            auto it = NativeWebTypes.find(type);
            if(it != NativeWebTypes.end()){

                // get audio helpers
                emscripten::val ctx = ci::em::helpers::getAudioHelpers()["globalContext"];

                std::string command = it->second;

                // create the node. 
                mNode = ctx.call<emscripten::val>(command.c_str());
            }
            
        }
       
        std::string getName(){
            return "NativeNode";
        }
        //! Returns a property on the native audio node (usually a AudioParam object)
        emscripten::val getAudioParam(std::string name){
            return mNode[name];
        }

        //! Returns reference to JS element for native node
        emscripten::val getNode(){
            return mNode;
        }

        /**
         * 
         */ 
        template<typename T>
        void setAudioParam
        (std::string paramName,T value){
              mNode[paramName].set("value",emscripten::val(value));
        }

        template<typename T>
        void setParam(std::string paramName,T value){
            mNode.set(paramName,emscripten::val(value));
        }

    private:
        emscripten::val mNode;
        
       
};
}