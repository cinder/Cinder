#pragma once 

#include <emscripten.h>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include "cinder/Log.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/gl.h"


namespace cinder { namespace em {

    typedef std::shared_ptr<class JSObject> JSObjRef;

    class JSObject {
        public:
            JSObject():obj( emscripten::val::undefined() ){ 
               
                auto Object = emscripten::val::global( "window" )[ "Object" ];
                obj = Object.new_();
            }
            ~JSObject(){}

            static JSObjRef create() {
                return JSObjRef( new JSObject() );
            }

            template<typename T>
            void set(std::string prop, T value ) {
                if( std::is_same< T, std::string >::value|| 
                    std::is_same< T, int>::value  ||
                    std::is_same< T, float >::value ||
                    std::is_same< T, emscripten::val>::value ) {
                    obj.set( prop, value );
                } else {
                    // otherwise we return because we can't handle values other than simple types. 
                    // you can technically add more handlers through the EMSCRIPTEN_BINDINGS macro.
                    return;
                }
            }

            // Returns a value on the JSObject in 
            int getIntValue( std::string prop ) {
                return obj[ prop ].as<int>();
            }

            float getFloatValue( std::string prop ) {
                return obj[ prop ].as<float>();
            }

            std::string getStringValue( std::string prop ) {
                 return obj[ prop ].as<std::string>();
            }

            //! Returns either the representation of the Javascript object or 
            // if the optional property name is passed, it will return the property value 
            // that is on the JSObject.
            emscripten::val get( std::string prop="" ) {
                if( prop != "" ) {
                    return obj[ prop ];
                }else {
                    return obj;
                }
            }

            void log(){
                auto helpers = emscripten::val::global( "window" )[ "CINDER_HELPERS" ];
                helpers.call<void>( "logNode", obj );
            }

            void log( std::string prop ) {
                 auto helpers = emscripten::val::global( "window" )[ "CINDER_HELPERS" ];
                 helpers.call<void>( "logNode",obj[ prop ]);
            }

            //! returns an emscripten::val object referencing the document global
            static emscripten::val document() {
                return emscripten::val::global( "document" );
            }

            //! returns an emscripten::val object referencing the window global
            static emscripten::val window() {
                return  emscripten::val::global( "window" );
            }

            static emscripten::val JSON(){
                return emscripten::val::global("JSON");
            }



        protected:
            emscripten::val obj;

    };

}}