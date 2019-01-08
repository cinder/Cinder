#include "dragdrop.h"
#include "cinder/Log.h"
using namespace ci;
using namespace emscripten;
using namespace std;

DragDrop::DragDrop( std::string elId ):
mEl(emscripten::val::undefined()),
mFile(emscripten::val::undefined())
{

    // query element
    mEl = em::helpers::querySelector(elId);    
    
    // if element is null, log and bail out. 
    if( mEl == val::null() )
    {
        CI_LOG_I( "Cannot set up drag and drop - the element does not exist" );
        return;
    }

    // setup default handler for when things are dropped onto canvas. 
    // This assumes that only one item is being dropped at a time, if you need to handle more
    // than one file, add your own drop handler.                 
    onDrop = []( emscripten::val e )-> void {
        e.call<void>( "preventDefault" );

        // if there are items being dropped
        if(e["dataTransfer"]["items"].as<bool>()){

            // loop through and process whatever the last file was that was dropped. 
            auto len = e["dataTransfer"]["items"]["length"].as<int>();
            for(int i = 0; i < len; ++i){
                auto itm = e["dataTransfer"]["items"][i];

                // if the itm dropped is classified as file, set that to be the current item.
                if(itm["kind"].as<string>() == "file"){
                    mFile = itm;
                }
            }
        }
    };
                                   
    // need to set this up to stop default browser functionality. 
    onDragOver = []( emscripten::val e )-> void {
        e.call<void>( "preventDefault" );
    };

    setup();
}

void DragDrop::setOnDrop(std::function<void( emscripten::val )> onDrop)
{
    this->onDrop = onDrop;
    setup();
}


// ========================= //

void DragDrop::setup(){
    mEl.set( "ondrop" , em::helpers::generateCallback( onDrop ) );
    mEl.set( "ondragover" , em::helpers::generateCallback( onDragOver ) );
}