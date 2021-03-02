
#include <emscripten.h>
#include "AudioJSFunctions.h"
#include "AudioNode.h"
#include "cinder/emscripten/CinderEmscripten.h"
#include "NativeNode.h"
#include "CustomNode.h"
#define STRINGIFY(A) #A
#define quote(x) #x

namespace cinder {

    typedef std::shared_ptr<class AudioContext>AudioContextRef;


    /**
    * A class representing an audio worklet. 
    * Analagous to an audio context in this case
    */
    class AudioContext : public AudioNode   
    {
        public:
    
	    static AudioContextRef Instance()
        {
		    static AudioContextRef instance( new AudioContext );
		    return instance;
    	}

        template<typename T>    
        std::shared_ptr<T> makeNode( T *node )
        {
    	    static_assert( std::is_base_of<AudioNode, T>::value, "AudioNode must inherit from audio::Node" );

	        std::shared_ptr<T> result( node );
	        return result;
        }

        //! builds and chains the graph together according to 
        //! the order in which nodes were added. 
        void enable( AudioNodeRef node=nullptr ) 
        {
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
            
            }   
        
            if( !mProcessorLoaded )
            {
                // load audio processor 
                load_processor(processor.c_str());
                mProcessorLoaded = true;
            }
        }

    AudioContext(AudioContext const&) = delete;
	void operator=(AudioContext const&) = delete;


    private:
	AudioContext(){}

    bool mProcessorLoaded = false;

    //! The AudioContextProcessor - receives inputs, sends changes via outputs. 
    std::string processor = STRINGIFY(
        class CinderAudioProcessor extends AudioWorkletProcessor {
            constructor() {
                super();
            }
        
            process(inputs, outputs, parameters) {

                // this should send a message to all connected 
                // AudioContextNodes with this data.                   
                this.port.postMessage({
                  inputs:inputs,
                    outputs:outputs,
                    channelCount:inputs.length
                });


                // return true to keep this AudioWorkletProcessor running. 
                return true;
            }
        }

        registerProcessor("CinderAudioProcessor",CinderAudioProcessor);
    );

};


}