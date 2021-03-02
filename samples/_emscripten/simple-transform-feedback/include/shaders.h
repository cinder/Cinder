//
// Created by sortofsleepy on 12/20/2018.
//

#ifndef EMSCRIPTEN_TF_SHADERS_H
#define EMSCRIPTEN_TF_SHADERS_H

#define STRINGIFY(A) #A
#include <string>
using namespace std;

const static string vertexTF = STRINGIFY(

        in vec3 vPosition;
        out vec3 oPosition;

        void main(){
            oPosition = vPosition;

            // not gonnna manipulate oPosition here - this is just a pass-thru shader
            // to ensure things work.
        }

        );

 const static string fragmentTF = STRINGIFY(
         precision highp float;
         out vec4 glFragColor;
         void main(){
             glFragColor = vec4(1.0,1.0,0.0,1.0);
         }
         );



const static string vertexR  = STRINGIFY(
        uniform mat4 ciModelViewProjection;
        in vec3 ciPosition;
        void main(){
            gl_PointSize = 1.0;
            gl_Position = ciModelViewProjection * vec4(ciPosition,1.);

        }
);
const static string fragmentR = STRINGIFY(
        precision highp float;
        out vec4 glFragColor;

        void main(){
            glFragColor = vec4(1.0,1.0,0.0,1.0);
        }
);

#endif //EMSCRIPTEN_TF_SHADERS_H
