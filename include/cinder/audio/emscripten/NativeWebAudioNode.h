#pragma once
#include "ContextWebAudio.h"
#include "cinder/audio/Node.h"
#include <emscripten/bind.h>
#include <vector>
#include <map>
#include <string>

/**
 * NOTE - CURRENTLY SHOULD BE CONSIDERED NON-FUNCTIONAL
 * This is more of a spec of what a native web audio node could look like on the c++ side of things.
 * 
 * The primary hurdle right now that's hindering completeness is extracting
 * the raw audio data from a WebAudio api node. It does not appear that it is possible
 * to  directly extract the data as it is not exposed in Javascript. You need to run things through either a 
 * ScriptProcessorNode or an AudioWorklet in order to extract the information.
 * 
 * A ScriptProcessorNode is set up here but I haven't thought of a good path to connect things to 
 * the rest of the graph yet. 
 * 
 */ 
namespace cinder { namespace audio {

    // List of all native Web Audio nodes along with it's respective command to
    // create in Javascript.
    const static std::map<std::string,std::string> NativeWebTypes = 
    {
      { "AnalyserNode","createAnalyser" },
      { "BiQuadFilter","createBiquadFilter" },
      { "ChannelMergerNode", "createChannelMerger" },
      { "ChannelSplitterNode","createChannelSplitter" },
      { "DelayNode","createDelay" },
      { "ConvolverNode","createConvolver" },
      { "GainNode","createGain" },
      { "PeriodicWaveNode","createPeriodicWave" },
      { "WaveSharperNode","createWaveSharper" },
      { "StereoPannerNode","createStereoPanner" },
      { "DynamicsCompressorNode","createDynamicsCompressor" },
    };
}}

/**
 * Defines a representation of a native WebAudio node.
 * At the moment, it's not intended to connect directly to a Cinder focused node, but rather to
 * another context or the audio destination
 */
namespace cinder { namespace audio { namespace em {
      typedef std::shared_ptr<class NativeWebAudioNode> NativeWebAudioNodeRef;

      class NativeWebAudioNode : public Node 
      {
      
      public:

        NativeWebAudioNode( const Format &format = Format() ):
        Node( format ),
        mRawNode( emscripten::val::undefined() ),
        mDataConverted( false )
        {
            mIsNativeWebNode = true;
        }

        //! Connects one web audio node to another.
        // TODO make sure the nodes don't delete themselves!
        void connectWebNode( emscripten::val node )
        {
          auto helpers = ci::em::helpers::getAudioHelpers();
          helpers.call<void>( "connectNodes",mRawNode,node );
        }

        // Connects this node directly to the global audio context
        void connectToContext()
        {
          auto helpers = ci::em::helpers::getAudioHelpers()[ "globalContext" ];
          mRawNode.call<void>( "connect",helpers[ "destination" ] );
        }

        //! Sets a parameter on the native web audio node.
        //! Pass in the name of the property you want to change and a value
        //! for the node.
        template<typename T>
        NativeWebAudioNode& setParameter( std::string name, T value )
        {
          mRawNode.set( name,val( value ) );
          return *this;
        }

        void connect( const NodeRef &output )
        {
          // cast to NativeWebAudioNode. 
          // We can figure out if it truly is a native web audio node or not by 
          // calling "isNativeWebNode" function which should return true if the 
          // constructor was called. If not, we assume it's a native cinder based node. 
          auto * node = dynamic_cast<NativeWebAudioNode*>( output.get() );

          // if the node is a native web node, connect to continue forming a chain
          if( node->isNativeWebNode() )
          {
            node->connectWebNode( mRawWebNode );
          }
          else if ( !node->isNativeWebNode() )
          {
            //check to see if it's an output node 
            auto outputNode = dynamic_cast<OutputDeviceNodeWebAudio*>( output.get() );

            // connect the node to the script processor of the output node, 
            // otherwise convert and pass data through to the node. 
            if( outputNode->isOutputDevice() )
            {
              outputNode->connectNativeNodeToOutput( mRawNode );
            }
            else {

              // convert to data suitable for internal buffer, place and then follow the connection path. 
              if( !mDataConverted )
              {
                convertToInternalBuffer();
              }

            }

          }
        }

        //! Convert the data the node is holding onto something that can be utilized down the line. 
        //! We take the data the node has collected so far and process it onto the internal buffer managed by the cinder side of things. 
        //! TODO make this private
        //! TODO decide if this needs to be continuously processing or not, seems we might see perf degradation eventually 
        void convertToInternalBuffer()
        {

          // get the audio context
          auto ctx = dynamic_cast<ContextWebAudio*>( getContext().get() );

          // get a reference to the raw WebAudio context 
          auto raw = ctx->getRawContext();

          auto helpers = ci::em::helpers::getAudioHelpers();

          // get reference to internalBuffer
          auto internalBuffer = getInternalBuffer();

          // build out a script processor 
          // TODO might need to keep an eye out on buffer size - SHOULD be the correct value at this point but not sure. 
          auto script = raw.call<emscripten::val>( "createScriptProcessor",ctx->getBufferSize(),1,raw[ "destination" ][ "channelCount" ].as<int>() );

          //connect this node to script processor
          mRawNode.call<void>( "connect",script );

          // connect script processor to destination to trigger processing. 
          script.call<void>( "connect",raw[ "destination" ] );

          // setup processing function
          std::function<void( emscripten::val e )> func = [=]( emscripten::val e )->void 
          {
            auto inputBuffer = e[ "inputBuffer" ];
   
            emscripten::val input = inputBuffer.call<val>( "getChannelData",0 );
          
            // get reference to internal buffer
            std::vector<float> idata;
            float * data = static_cast<float*>( internalBuffer->getData() );

            // copy data onto internal buffer
            for( int i = 0; i < internalBuffer->getNumFrames(); ++i )
            {
              idata.push_back( input[i].as<float>() );
            }

            data = &idata[0];
          };

          // setup callback
          script.set( "onaudioprocess",ci::em::helpers::generateCallback(func) );

          // setup helper to disconnect processor 
          // TODO should it keep processing? 
          helpers.call<void>( "disconnectScriptFromDestination",script,val( 1000 ) );
        }

        bool isNativeWebNode()
        {
          return mIsNativeWebNode;
        }

        //! Creates a new WebAudio node based on a string name.
        void createNode( std::string nodeName )
        {

          auto ctx = emscripten::val::global( "window" )[ "CINDER_AUDIO" ][ "globalContext" ];
          auto it = ci::audio::NativeWebTypes.find( nodeName );

          if  ( it != ci::audio::NativeWebTypes.end() )
          {
            mRawNode = ctx.call<emscripten::val>( it->second.c_str() );
          }
          else {
            CI_LOG_I( "Unable to create native WebAudio node - the name " << nodeName << " was not found" );
            return ;
          }
        }

        private:
            // A representation of a native web audio node on the Javascript side of things.
          emscripten::val mRawNode;
          bool mIsNativeWebNode;
          bool mDataConverted;
      };
}}}