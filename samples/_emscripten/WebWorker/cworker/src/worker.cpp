#include <emscripten.h>
#include <iostream>
#include <stdio.h>
#include <vector>
#include "cinder/Log.h"
#include "cinder/Vector.h"
#include "cinder/Rand.h"



/**
 *  A very simple worker. It's only goal is to update the vec3 we pass in
 * and shift the values along a sphere.
 */


extern "C" {

    float theta = 0.0f;
    float phi = 0.0f;
    float thetaSpeed = ci::randFloat( -0.01,0.01 );
    float phiSpeed = ci::randFloat( -0.01,0.01 );
    float radius = 50.0f;

  void onmessage( char* data, int size) 
  {

    ci::vec3 * pt = ( ci::vec3* )data;
    ci::vec3 v = *pt;

    v.x = cos( theta ) * sin( phi ) * radius;
    v.y = sin( theta ) * sin( phi ) * radius;
    v.z = cos( phi ) * radius;

    theta += thetaSpeed;
    phi += phiSpeed;

    // send back to callback passing the data through.
    emscripten_worker_respond( ( char* )glm::value_ptr( v ),size );
  }
}
