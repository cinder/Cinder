// The copyright in this software is being made available under the BSD License, included below. 
// This software may be subject to other third party and contributor rights, including patent rights, 
// and no such rights are granted under this license.
//
// Copyright (c) 2013, Microsoft Open Technologies, Inc. 
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the following disclaimer.
// - Redistributions in binary form must reproduce the above copyright notice, 
//   this list of conditions and the following disclaimer in the documentation 
//   and/or other materials provided with the distribution.
// - Neither the name of Microsoft Open Technologies, Inc. nor the names of its contributors 
//   may be used to endorse or promote products derived from this software 
//   without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, 
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS 
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


#pragma once

#include "cinder/Cinder.h"

#include "cinder/gl/gl.h"

#include "cinder/Exception.h"
#include "cinder/Quaternion.h"
#include "cinder/Matrix.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Rect.h"
#include "cinder/Font.h"
#include "cinder/PolyLine.h"
#include "cinder/AxisAlignedBox.h"

#include <windows.h>
#undef min
#undef max

#if defined( CINDER_WINRT )
	#include "cinder/dx/gldx.h"
#else
	#include <gl/gl.h>
#endif

// forward declarations
namespace cinder {
	class Camera; class TriMesh2d; class TriMesh; class Sphere;
	namespace dx {
		class VboMesh;
		class Texture;
		typedef std::shared_ptr<Texture>	TextureRef;
		class Light;
	}
	namespace app {
		class AppImplMswRendererDx;
	}
} // namespace cinder

namespace cinder { namespace dx {

//#if defined( CINDER_MSW )
app::AppImplMswRendererDx *getDxRenderer();
//#endif

//! Returns whether a particular OpenGL extension is available. Caches results
bool isExtensionAvailable( const std::string &extName );

//! Clears the OpenGL color buffer using \a color and optionally clears the depth buffer when \a clearDepthBuffer
void clear( const ColorA &color = ColorA::black(), bool clearDepthBuffer = true );

//! Enables or disables wait for vertical sync
void enableVerticalSync( bool enable = true );
//! Disables wait for vertical sync
inline void disableVerticalSync() { enableVerticalSync( false ); }
//! Returns whether vertical sync is enabled for the current context
bool isVerticalSyncEnabled();

//! Sets the \c MODELVIEW and \c PROJECTION matrices to reflect the values of \a cam. Leaves the \c MatrixMode as \c MODELVIEW.
void setMatrices( const Camera &cam );
//! Sets the \c MODELVIEW matrix to reflect the values of \a cam. Leaves the \c MatrixMode as \c MODELVIEW.
void setModelView( const Camera &cam );
//! Sets the \c MODELVIEW matrix to reflect the values of \a m. Leaves the \c MatrixMode as \c MODELVIEW.
void setModelView( const mat4 &m );
//! Sets the \c PROJECTION matrix to reflect the values of \a cam. Leaves the \c MatrixMode as \c PROJECTION.
void setProjection( const Camera &cam );
//! Pushes the \c MODELVIEW matrix onto its stack, preserving the current values. Leaves the \c MatrixMode as \c MODELVIEW.
void pushModelView();
//! Pops the \c MODELVIEW matrix off of its stack, restoring the values saved with the previous push. Leaves the \c MatrixMode as \c MODELVIEW.
void popModelView();
//! Pushes the \c MODELVIEW matrix onto its stack, preserving the current values, and then sets the matrix to reflect \a cam. Leaves the \c MatrixMode as \c MODELVIEW.
void pushModelView( const Camera &cam );
//! Pushes the \c PROJECTION matrix onto its stack, preserving the current values, and then sets the matrix to reflect \a cam. Leaves the \c MatrixMode as \c PROJECTION.
void pushProjection( const Camera &cam );
//! Pushes the \c MODELVIEW and \c PROJECTION matrices onto their stacks, preserving the current values. Leaves the \c MatrixMode as \c MODELVIEW.
void pushMatrices();
//! Pops the \c MODELVIEW and \c PROJECTION matrices off their stacks, restoring the values saved with the previous push. Leaves the \c MatrixMode as \c MODELVIEW.
void popMatrices();
//! Multiplies the current \c MODELVIEW matrix with \a mtx. Leaves the \c MatrixMode as \c MODELVIEW.
void multModelView( const mat4 &mtx );
//! Multiplies the current \c PROJECTION matrix with \a mtx. Leaves the \c MatrixMode as \c MODELVIEW.
void multProjection( const mat4 &mtx );
//! Returns the value of the current \c MODELVIEW matrix as a mat4.
mat4 getModelView();
//! Returns the value of the current \c PROJECTION matrix as a mat4.
mat4 getProjection();

//! Sets the viepwort and \c MODELVIEW and \c PROJECTION matrices to be a perspective projection with the upper-left corner at \c [0,0] and the lower-right at \c [screenWidth,screenHeight], but flipped vertically if not \a originUpperLeft.
void setMatricesWindowPersp( int screenWidth, int screenHeight, float fovDegrees = 60.0f, float nearPlane = 1.0f, float farPlane = 1000.0f, bool originUpperLeft = true );
//! Sets the viewport and \c MODELVIEW and \c PROJECTION matrices to be a perspective projection with the upper-left corner at \c [0,0] and the lower-right at \c [screenWidth,screenHeight], but flipped vertically if not \a originUpperLeft.
inline void setMatricesWindowPersp( const ivec2 &screenSize, float fovDegrees = 60.0f, float nearPlane = 1.0f, float farPlane = 1000.0f, bool originUpperLeft = true )
	{ setMatricesWindowPersp( screenSize.x, screenSize.y, fovDegrees, nearPlane, farPlane ); }
//! Sets the viewport and \c MODELVIEW and \c PROJECTION matrices to orthographic with the upper-left corner at \c [0,0] and the lower-right at \c [screenWidth,screenHeight] if \a originUpperLeft is \c true. Otherwise the origin is in the lower right.
void setMatricesWindow( int screenWidth, int screenHeight, bool originUpperLeft = true );
//! Sets the viewport and the \c MODELVIEW and \c PROJECTION matrices to orthographic with the upper-left corner at \c [0,0] and the lower-right at \c [size.x,size.y] if \a originUpperLeft is \c true. Otherwise the origin is in the lower right.
inline void setMatricesWindow( const ivec2 &screenSize, bool originUpperLeft = true ) { setMatricesWindow( screenSize.x, screenSize.y, originUpperLeft ); }

//! Returns the current OpenGL Viewport as an Area
Area getViewport();
//! Sets the current OpenGL Viewport to \a area
void setViewport( const Area &area );

//! Produces a translation by \a pos in the current matrix.
void translate( const vec2 &pos );
//! Produces a translation by \a x and \a y in the current matrix.
inline void translate( float x, float y ) { translate( vec2( x, y ) ); }
//! Produces a translation by \a pos in the current matrix.
void translate( const vec3 &pos );
//! Produces a translation by \a x, \a y and \a z in the current matrix.
inline void translate( float x, float y, float z ) { translate( vec3( x, y, z ) ); }

//! Produces a scale by \a scale in the current matrix.
void scale( const vec3 &scl );
//! Produces a scale by \a scl in the current matrix.
inline void scale( const vec2 &scl ) { scale( vec3( scl.x, scl.y, 1 ) ); }
//! Produces a scale by \a x and \a y in the current matrix.
inline void scale( float x, float y ) { scale( vec3( x, y, 1 ) ); }
//! Produces a scale by \a x, \a y and \a z in the current matrix.
inline void scale( float x, float y, float z ) { scale( vec3( x, y, z ) ); }

//! Produces a rotation around the X-axis by \a xyz.x degrees, the Y-axis by \a xyz.y degrees and the Z-axis by \a xyz.z degrees in the current matrix. Processed in X-Y-Z order.
void rotate( const vec3 &xyz );
//! Produces a rotation by the quaternion \a quat in the current matrix.
void rotate( const quat &quat );
//! Produces a 2D rotation, the equivalent of a rotation around the Z axis by \a degrees.
inline void rotate( float degrees ) { rotate( vec3( 0, 0, degrees ) ); }

#if ! defined( CINDER_GLES )
//! Equivalent to glBegin() in immediate mode
void begin( GLenum mode );
//! Equivalent to glEnd() in immediate mode
void end();
//! Used between calls to gl::begin() and \c gl::end(), appends a vertex to the current primitive.
void vertex( const vec2 &v );
//! Used between calls to gl::begin() and \c gl::end(), appends a vertex to the current primitive.
void vertex( float x, float y );
//! Used between calls to gl::begin() and \c gl::end(), appends a vertex to the current primitive.
void vertex( const vec3 &v );
//! Used between calls to gl::begin() and \c gl::end(), appends a vertex to the current primitive.
void vertex( float x, float y, float z );
//! Used between calls to gl::begin() and gl::end(), sets the 2D texture coordinate for the next vertex.
void texCoord( float x, float y );
//! Used between calls to gl::begin() and gl::end(), sets the 2D texture coordinate for the next vertex.
void texCoord( const vec2 &v );
//! Used between calls to gl::begin() and gl::end(), sets the 3D texture coordinate for the next vertex.
void texCoord( float x, float y, float z );
//! Used between calls to gl::begin() and gl::end(), sets the 3D texture coordinate for the next vertex.
void texCoord( const vec3 &v );
#endif // ! defined( CINDER_GLES )
//! Sets the current color and the alpha value to 1.0
void color( float r, float g, float b );
//! Sets the current color and alpha value
void color( float r, float g, float b, float a );
//! Sets the current color, and the alpha value to 1.0
void color( const Color8u &c );
//! Sets the current color and alpha value
void color( const ColorA8u &c );
//! Sets the current color, and the alpha value to 1.0
void color( const Color &c );
//! Sets the current color and alpha value
void color( const ColorA &c );
//! Sets the current normal
void normal( float x, float y, float z );
//! Sets the current normal
void normal( const vec3 &v );

//! Enables the OpenGL State \a state. Equivalent to calling to glEnable( state );
inline void enable( GLenum state ) { throw (std::string(__FUNCTION__) + " not implemented yet").c_str(); /*glEnable( state );*/ }
//! Disables the OpenGL State \a state. Equivalent to calling to glDisable( state );
inline void disable( GLenum state ) { throw (std::string(__FUNCTION__) + " not implemented yet").c_str(); /*glDisable( state );*/ }

//! Enables alpha blending. Selects a \c BlendFunc that is appropriate for premultiplied-alpha when \a premultiplied
void enableAlphaBlending( bool premultiplied = false );
//! Disables alpha blending.
void disableAlphaBlending();
//! Enables alpha blending and selects a \c BlendFunc for additive blending
void enableAdditiveBlending();

/** \brief Enables alpha testing and sets the \c AlphaFunc to test for values which are \a func than \a value, which should be in the range [0, 1.0]. 
 *  Possible values for \a func include <tt>GL_NEVER, GL_LESS, GL_EQUAL, GL_LEQUAL, GL_GREATER, GL_NOTEQUAL, GL_GEQUAL and GL_ALWAYS</tt>. **/
void enableAlphaTest( float value = 0.5f, int func = GL_GREATER );
//! Disables alpha testing
void disableAlphaTest();

#if ! defined( CINDER_GLES )
//! Enables wireframe drawing by setting the \c PolygonMode to \c GL_LINE.
void enableWireframe();
//! Disables wireframe drawing.
void disableWireframe();
#endif // ! defined( CINDER_GLES )

//! Disables reading from the depth buffer, disabling z-testing.
void disableDepthRead();
//! Disables writing to the depth buffer.
void disableDepthWrite();
//! Enables reading from the depth buffer when \a enable, enabling z-testing.
void enableDepthRead( bool enable = true );
//! Enables writing to the depth buffer when \a enable.
void enableDepthWrite( bool enable = true );
//! Enables lighting
void enableLighting();
//! Disables lighting
void disableLighting();
//! Sets a light internally using the properties of \a light. Up to eight unique lights may exist in this model.
void setLight(const Light &light);

//! Draws a line from \a start to \a end
void drawLine( const vec2 &start, const vec2 &end );
//! Draws a line from \a start to \a end
void drawLine( const vec3 &start, const vec3 &end );
//! Renders a solid cube centered at \a center of size \a size. Normals and created texture coordinates are generated for \c GL_TEXTURE_2D, with each face in the range [0,0] - [1.0,1.0]
void drawCube( const vec3 &center, const vec3 &size );
//! Renders a solid cube centered at \a center of size \a size. Each face is assigned a unique color, and no normals or texture coordinates are generated.
void drawColorCube( const vec3 &center, const vec3 &size );
//! Renders a stroked cube centered at \a center of size \a size.
void drawStrokedCube( const vec3 &center, const vec3 &size );
//! Renders a stroked cube \a aab
inline void drawStrokedCube( const AxisAlignedBox3f &aab ) { drawStrokedCube( aab.getCenter(), aab.getSize() ); }
//! Renders a solid sphere centered at \a center of radius \a radius. \a segments defines how many segments the sphere is subdivided into. Normals and texture coordinates in the range [0,1] are generated.
void drawSphere( const vec3 &center, float radius, int segments = 12 );
//! Renders a solid sphere. \a segments defines how many segments the sphere is subdivided into. Normals and texture coordinates in the range [0,1] are generated.
void draw( const class Sphere &sphere, int segments = 12 );
//! Renders a solid circle using triangle fans. The default value of zero for \a numSegments automatically determines a number of segments based on the circle's circumference.
void drawSolidCircle( const vec2 &center, float radius, int numSegments = 0 );
//! Renders a stroked circle using a line loop. The default value of zero for \a numSegments automatically determines a number of segments based on the circle's circumference.
void drawStrokedCircle( const vec2 &center, float radius, int numSegments = 0 );
//! Renders a solid ellipse using triangle fans. The default value of zero for \a numSegments automatically determines a number of segments based on the ellipse's circumference.
void drawSolidEllipse( const vec2 &center, float radiusX, float radiusY, int numSegments = 0 );
//! Renders a stroked circle using a line loop. The default value of zero for \a numSegments automatically determines a number of segments based on the circle's circumference.
void drawStrokedEllipse( const vec2 &center, float radiusX, float radiusY, int numSegments = 0 );
//! Renders a solid rectangle. Texture coordinates in the range [0,1] are generated unless \a textureRectangle.
void drawSolidRect( const Rectf &rect, bool textureRectangle = false );
//! Renders a stroked rectangle.
void drawStrokedRect( const Rectf &rect );
void drawSolidRoundedRect( const Rectf &r, float cornerRadius, int numSegmentsPerCorner = 0 );
void drawStrokedRoundedRect( const Rectf &r, float cornerRadius, int numSegmentsPerCorner = 0 );
//! Renders a coordinate frame representation centered at the origin. Arrowheads are drawn at the end of each axis with radius \a headRadius and length \a headLength.
//! Renders a solid triangle.
void drawSolidTriangle( const vec2 &pt1, const vec2 &pt2, const vec2 &pt3 );
void drawSolidTriangle( const vec2 pts[3] );
//! Renders a textured triangle.
void drawSolidTriangle( const vec2 &pt1, const vec2 &pt2, const vec2 &pt3, const vec2 &texPt1, const vec2 &texPt2, const vec2 &texPt3 );
void drawSolidTriangle( const vec2 pts[3], const vec2 texCoord[3] );
//! Renders a stroked triangle.
void drawStrokedTriangle( const vec2 &pt1, const vec2 &pt2, const vec2 &pt3 );	
void drawStrokedTriangle( const vec2 pts[3] );
void drawCoordinateFrame( float axisLength = 1.0f, float headLength = 0.2f, float headRadius = 0.05f );
//! Draws a vector starting at \a start and ending at \a end. An arrowhead is drawn at the end of radius \a headRadius and length \a headLength.
void drawVector( const vec3 &start, const vec3 &end, float headLength = 0.2f, float headRadius = 0.05f );
//! Draws a wireframe representation of the frustum defined by \a cam.
void drawFrustum( const Camera &cam );
//! Draws a torus at the origin, with an outter radius \a outterRadius and an inner radius \a innerRadius, subdivided into \a longitudeSegments and \a latitudeSegments. Normals and texture coordinates in the range [0,1] are generated.
void drawTorus( float outterRadius, float innerRadius, int longitudeSegments = 12, int latitudeSegments = 12 );
//! Draws a open-ended cylinder, with base radius \a baseRadius and top radius \a topRadius, with height \a height, subdivided into \a slices and \a stacks. Normals and texture coordinates in the range [0,1] are generated.
void drawCylinder( float baseRadius, float topRadius, float height, int slices = 12, int stacks = 1 );
//! Draws a 2d PolyLine \a polyLine
void draw( const class PolyLine<vec2> &polyLine );
//! Draws a 3d PolyLine \a polyLine
void draw( const class PolyLine<vec3> &polyLine );
//! Draws a Path2d \a path2d using approximation scale \a approximationScale. 1.0 corresponds to screenspace, 2.0 is double screen resolution, etc
void draw( const class Path2d &path2d, float approximationScale = 1.0f );
//! Draws a Shape2d \a shape2d using approximation scale \a approximationScale. 1.0 corresponds to screenspace, 2.0 is double screen resolution, etc
void draw( const class Shape2d &shape2d, float approximationScale = 1.0f );

//! Draws a solid (filled) Path2d \a path2d using approximation scale \a approximationScale. 1.0 corresponds to screenspace, 2.0 is double screen resolution, etc. Performance warning: This routine tesselates the polygon into triangles. Consider using Triangulator directly.
void drawSolid( const class Path2d &path2d, float approximationScale = 1.0f );
//! Draws a solid (filled) Shape2d \a shape2d using approximation scale \a approximationScale. 1.0 corresponds to screenspace, 2.0 is double screen resolution, etc. Performance warning: This routine tesselates the polygon into triangles. Consider using Triangulator directly.
void drawSolid( const class Shape2d &shape2d, float approximationScale = 1.0f );
//! Draws a solid (filled) PolyLine2f \a polyLine. Performance warning: This routine tesselates the polygon into triangles. Consider using Triangulator directly.
void drawSolid( const PolyLine2f &polyLine );

void draw( const TriMesh &mesh );
//! Draws a range of triangles starting with triangle # \a startTriangle and a count of \a triangleCount from cinder::TriMesh \a mesh at the origin.
void drawRange( const TriMesh &mesh, size_t startTriangle, size_t triangleCount );
//! Draws a cinder::gl::VboMesh \a mesh at the origin.

void draw( const VboMesh &vbo );
//! Draws a range of vertices and elements of cinder::gl::VboMesh \a mesh at the origin. Default parameters for \a vertexStart and \a vertexEnd imply the VboMesh's full range of vertices.
void drawRange( const VboMesh &vbo, size_t startIndex, size_t indexCount, int vertexStart = -1, int vertexEnd = -1 );
//! Draws a range of elements from a cinder::gl::VboMesh \a vbo.
void drawArrays( const VboMesh &vbo, GLint first, GLsizei count );
//!	Draws a textured quad of size \a scale that is aligned with the vectors \a bbRight and \a bbUp at \a pos, rotated by \a rotationDegrees around the vector orthogonal to \a bbRight and \a bbUp.
	
void drawBillboard( const vec3 &pos, const vec2 &scale, float rotationDegrees, const vec3 &bbRight, const vec3 &bbUp );
//! Draws \a texture on the XY-plane
void draw( const TextureRef &texture );
//! Draws \a texture on the XY-plane at \a pos
void draw( const TextureRef &texture, const vec2 &pos );
//! Draws \a texture on the XY-plane in the rectangle defined by \a rect
void draw( const TextureRef &texture, const Rectf &rect );
//! Draws the pixels inside \a srcArea of \a texture on the XY-plane in the rectangle defined by \a destRect
void draw( const TextureRef &texture, const Area &srcArea, const Rectf &destRect );
//! Draws a bunch of textured triangles as specified by the function caller
void draw( const TextureRef &texture, const std::vector<float> &verts, const std::vector<float> &texCoords, const std::vector<ColorA8u> &vertColors, const std::vector<uint32_t> &indices );
//! Sets the render state to batch texture draw calls until the end for optimal performance
void batchTextureBegin();
//! Draws all the batched textures
void batchTextureEnd();

//! Draws a string \a str with its lower left corner located at \a pos. Optional \a font and \a color affect the style.
void drawString( const std::string &str, const vec2 &pos, const ColorA &color = ColorA( 1, 1, 1, 1 ), Font font = Font() );
//! Draws a string \a str with the horizontal center of its baseline located at \a pos. Optional \a font and \a color affect the style
void drawStringCentered( const std::string &str, const vec2 &pos, const ColorA &color = ColorA( 1, 1, 1, 1 ), Font font = Font() );
//! Draws a right-justified string \a str with the center of its  located at \a pos. Optional \a font and \a color affect the style
void drawStringRight( const std::string &str, const vec2 &pos, const ColorA &color = ColorA( 1, 1, 1, 1 ), Font font = Font() );

class Exception : public cinder::Exception {
};

class ExceptionUnknownTarget : public Exception {
};

class DrawingTooManyVerticesException : public Exception {
public:
	virtual const char *what() const { return "Drawing more vertices than the vertex buffer supports"; }	
};

} } // namespace cinder::dx