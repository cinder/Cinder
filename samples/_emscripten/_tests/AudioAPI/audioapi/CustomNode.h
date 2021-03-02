#pragma once
#include "AudioNode.h"

#include "cinder/emscripten/CinderEmscripten.h"
#include <emscripten/val.h>
#include <memory>
#include "cinder/Log.h"


namespace cinder {


/**
 * A type of node that allows for custom processing of the audio stream. 
 * The idea with this to keep things compatible with C++, is that users will 
 * override this node and it's process() function. 
 * 
 * When the final audio graph is built - a custom AudioWorkletNode class is created and 
 * the process function is passed to that AudioWorkletNode instance and called whenever the 
 * main AudioWorkletProcessor triggers an update. 
 */ 
typedef std::shared_ptr<class CustomNode>CustomNodeRef;

class CustomNode : public AudioNode 
{
    public:
    CustomNode():mProcessFunction( emscripten::val::undefined() )
    {
        
        
        // generate a JS compatible callback function that will get passed to auto-generated instances of 
        // AudioWorkletNode on the JS side. 
        std::function<void( emscripten::val )> func = std::bind( &CustomNode::process,this,std::placeholders::_1 );
        mProcessFunction = em::helpers::generateCallback( func );
    }

    //! returns the processing function for this CustomNode
    emscripten::val getProcessFunction(){
        return mProcessFunction;
    }
    
  
    //! A function to run to process the input and output buffers from the AudioWorkletProcessor. 
    //! JS side will send data in the following manner. 
    //! {inputs : [Float32Array], outputs:[Float32Array], channelCount:int }
    //! Note the [] brackets denote a potential array 
    virtual void process( emscripten::val jsdata )
    {
        int channel_count = jsdata["channelCount"].as<int>();

        for( int i = 0; i < channel_count; ++i )
        { 
            emscripten::val inputs = jsdata["inputs"];
            emscripten::val outputs = jsdata["outputs"];

            int size = inputs["length"].as<int>();

            // loop through arrays of each channel
            for( int a = 0; a < size; ++a )
            {
                float inValue = inputs[a].as<float>();
                
                // just pass through here - users should override CustomNode object. 
                outputs[a].set(i,inValue);

            }
        }
    }

    protected:

    unsigned mRenderQuantumFrames = 128;
    unsigned mKBytedPerChannel = mRenderQuantumFrames * sizeof(float);
    emscripten::val mProcessFunction;

};
}