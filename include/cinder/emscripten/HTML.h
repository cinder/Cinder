#include <emscripten.h>
#include <emscripten/val.h>
#include <string>

/**
 * Some potentially helpful functions if you wish to interact with the DOM
 */

namespace cinder { namespace em { namespace html {


    //! creates an HTML element. Pass in the tag name as the parameter. 
    //! Note that you could technically pass in anything so there will be no error 
    //! if you pass in something that isn't an actual tag as according to the curren t
    //! HTML spec. 
    static emscripten::val createElement( std::string el )
    {
        return emscripten::val::global( "document" ).call<emscripten::val>( "createElement", emscripten::val(el) );
    }

    //! Helper to return the document object.
    static emscripten::val getDocument()
    {
        return emscripten::val::global( "document" );
    }

    //! helper to return the window object
    static emscripten::val getWindow()
    {
        return emscripten::val::global( "window" );
    }

    static emscripten::val querySelector( std::string selector )
    {
        emscripten::val document = getDocument();
        return document.call<emscripten::val>( "querySelector",emscripten::val( selector ) );
    }

    //! appends one element to another. Pass in the selector (can be id or className) of the parent element 
    //! and the emscripten::val element you want to append to the parent. 
    static void appendTo( std::string selector,emscripten::val el )
    {
        // note - will throw error in console if element doesn't exist. 
        emscripten::val node = getDocument().call<emscripten::val>( "querySelector",emscripten::val( selector ) );
        node.call<void>( "appendChild",el );
    }

    //! same as above but parent is assumed to have already been found 
    static void appendTo( emscripten::val parent ,emscripten::val el )
    {
       
         parent.call<void>( "appendChild",el );
    }

    /**
     * Small wrapper around some of the workings of the emscripten::val object to 
     * allow you to easily add HTML elements.
     */
    class HTMLElement 
    {

 
        public:
        HTMLElement( std::string el="" ):mElement( emscripten::val::undefined() )
        {
            // TODO maybe do querySelector if we know the el parameter is an id or classname?
            if( el != "" )
            {
                mElement = createElement( el) ;
            }
        }

        //! sets classname property on the element
        void setClassName( std::string id )
        {
            mElement.set( "className" ,emscripten::val( id ) );
        }
        
        //! sets id property on the element
        void setId( std::string id )
        {
            mElement.set( "id" ,emscripten::val( id ) );
        }

        //! sets the style property on the element
        void css( std::string styles )
        {
            mElement.set( "style", emscripten::val( styles ) );
        }

        //! sets an attribute on the element. 
        //! note that you should only pass in types that are appropriate or you may 
        //! get un-expected behavior. 
        template<typename T>
        void setAttribute( std::string name, T value )
        {
            mElement.set( name, emscripten::val( value ) );
        }

        //! appends a child element to this element. 
        void appendChild( emscripten::val el )
        {
            mElement.call<void>( "appendChild",el );
        }

        //! appends this element to another element
        void appendToEl( std::string el )
        {
            appendTo(el,mElement);
        }

        private:
        emscripten::val mElement;

    };
}}}