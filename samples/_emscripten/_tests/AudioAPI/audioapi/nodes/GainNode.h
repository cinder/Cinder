#pragma once

#include "CustomNode.h"


namespace cinder { 



class CustomGainNode : public CustomNode {

    public:
    CustomGainNode(){}

    void process( emscripten::val jsData )
    {
       try {
           int v = jsData["channelCount"].as<int>();
       }catch(...){
           CI_LOG_E("Unable to get info");
       }
    }
};
}