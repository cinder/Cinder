

#include <string>
#include <emscripten/val.h>
#include "cinder/emscripten/CinderEmscripten.h"
#include <memory>

class DragDrop {


    public:
    
    DragDrop( std::string elId="#canvas" );

    //! set the function used when something is dropped over the target area. 
    void setOnDrop( std::function<void( emscripten::val )> onDrop );

    //! Returns the current file being handled by the instance of this object.
    emscripten::val getFile() { return mFile; }

    private:

    //! The function called when something is dropped on the target area. 
    std::function<void( emscripten::val )> onDrop;

    //! The function called just before drop event, called to prevent default browser behavior
    std::function<void( emscripten::val )> onDragOver;

    //! the element to use as the drop zone
    emscripten::val mEl;

    //! By default we assume we're only dealing with one file. This is a the refernce to that file.
    emscripten::val mFile;

    //! Sets things up 
    void setup();
};

