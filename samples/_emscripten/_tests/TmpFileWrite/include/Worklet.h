#include <emscripten.h>
#include <emscripten/val.h>

class Worklet {

    emscripten::val mWorklet;
    std::string mWorkletSource;
public:
    Worklet( std::string workletSource ):
    mWorkletSource(workletSource),
    mWorklet( emscripten::val::undefined() )
    {}

    void loadWorklet(std::string source){
        
    }

};