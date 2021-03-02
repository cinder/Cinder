
#pragma once
#include <memory>
#include <emscripten/bind.h>
#include <emscripten/val.h>

namespace cinder {

typedef std::shared_ptr<class AudioNode> AudioNodeRef;

class AudioNode {
    public:
    AudioNode():mConnectionEnabled(false){}

    virtual void connect(const AudioNodeRef input){
        mConnectedTo = input;
    }

    //! Logs node to the console. 
    void logNode(){

    }

    AudioNodeRef getConnectedNode(){
        return mConnectedTo;
    }

    void setEnabled(){
        mConnectionEnabled = true;
    }

    bool isEnabled(){
        return mConnectionEnabled;
    }

    protected:
    AudioNodeRef mConnectedTo;

    //! Used to serve as a marker of when this node is connected during the AudioContext's enable() call
    bool mConnectionEnabled;
    
};

inline const AudioNodeRef& operator>>( const AudioNodeRef &input, const AudioNodeRef &output )
{
	output->connect(input);
	return output;
}
}