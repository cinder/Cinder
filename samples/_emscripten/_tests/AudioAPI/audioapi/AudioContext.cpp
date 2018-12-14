#include "AudioContext.h"

using namespace ci;
using namespace std;

namespace cinder { namespace audio {

    void AudioContext::enable( AudioNodeRef node = nullptr ){
        // just go through and push into js graph 
        emscripten::val helpers = ci::em::helpers::getWorkletHelpers();
        emscripten::val graph = helpers["graph"];
        
        // if the graph has no items, we start by injecting the output
        if( graph[ "length" ].as<int>() == 0 )
        {

            graph.call<void>( "push",emscripten::val::global("window")["CINDER_AUDIO"]["globalContext"]["destination"] );   

            // push the connected node and recurse the function until we have no nodes left that have connections. 
            enable( this->mConnectedTo );
        }
        else {
            // there are only 2 possible derivations from the base class of AudioNode - if NativeNode cast fails, 
            // then it must be an instance of a CustomNode
            NativeNode * n = dynamic_cast<NativeNode*>(node.get());
            if( n != nullptr )
            {
                // push the native node onto the stack 
                graph.call<void>( "push",n->getNode() );
                n->setEnabled();

                if( !n->getConnectedNode()->isEnabled() )
                {
                    // push the connected node and recurse the function until we have no nodes left that have connections. 
                    enable(n->getConnectedNode());                       
                }
            }
            else {
                CustomNode * n = dynamic_cast<CustomNode*>( node.get() );
                graph.call<void>( "push",n->getProcessFunction() );
                n->setEnabled();

                if( n->getConnectedNode() != nullptr )
                {
                   
                    if(!n->getConnectedNode()->isEnabled()){
                         // push the connected node and recurse the function until we have no nodes left that have connections. 
                        enable(n->getConnectedNode());                       
                    }
            
                }   
            }

            if( !mProcessorLoaded ){
                load_processor(processor.c_str());
                mProcessorLoaded = true;
            }
        }
    }
}}
