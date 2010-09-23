/*
 Copyright (c) 2010, The Barbarian Group
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification, are permitted provided that
 the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and
	the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
	the following disclaimer in the documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include "cinder/Cinder.h"

#if defined( CINDER_MAC )
	#include <OpenGL/gl.h>
	#include <OpenGL/glext.h>
#elif defined( CINDER_MSW )
	#include "cinder/gl/GLee.h"
#else
	#define CINDER_GLES
	#define CINDER_GLES1
#endif

#include "cinder/Quaternion.h"
#include "cinder/Matrix.h"
#include "cinder/Vector.h"
#include "cinder/Color.h"
#include "cinder/Rect.h"
#include "cinder/Font.h"
#include "cinder/PolyLine.h"
#include "cinder/AxisAlignedBox.h"

#if defined( CINDER_MSW )
	#include <windows.h>
	#undef min
	#undef max
	#include <gl/gl.h>
#elif defined( CINDER_COCOA_TOUCH )
	#include <OpenGLES/ES1/gl.h>
	#include <OpenGLES/ES1/glext.h>
#elif defined( CINDER_MAC )
	#include <OpenGL/gl.h>
#endif

// forward declarations
namespace cinder {
	class Camera; class TriMesh; class Sphere;
	namespace gl {
		 class VboMesh; class Texture;
	}
} // namespace cinder

namespace cinder { namespace gl {

//! Returns whether a particular OpenGL extension is available. Caches results
bool isExtensionAvailable( const std::string &extName );

//! Clears the OpenGL color buffer using \a color and optionally clears the depth buffer when \a clearDepthBuffer
void clear( const ColorA &color = ColorA::black(), bool clearDepthBuffer = true );

//! Sets the \c MODELVIEW and \c PROJECTION matrices to reflect the values of \a cam. Leaves the \c MatrixMode as \c MODELVIEW.
void setMatrices( const Camera &cam );
//! Sets the \c MODELVIEW matrix to reflect the values of \a cam. Leaves the \c MatrixMode as \c MODELVIEW.
void setModelView( const Camera &cam );
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
void multModelView( const Matrix44f &mtx );
//! Multiplies the current \c PROJECTION matrix with \a mtx. Leaves the \c MatrixMode as \c MODELVIEW.
void multProjection( const Matrix44f &mtx );
//! Returns the value of the current \c MODELVIEW matrix as a Matrix44f.
Matrix44f getModelView();
//! Returns the value of the current \c PROJECTION matrix as a Matrix44f.
Matrix44f getProjection();

//! Sets the viepwort and \c MODELVIEW and \c PROJECTION matrices to be a perspective projection with the upper-left corner at \c [0,0] and the lower-right at \c [screenWidth,screenHeight], but flipped vertically if not \a originUpperLeft.
void setMatricesWindowPersp( int screenWidth, int screenHeight, float fovDegrees = 60.0f, float nearPlane = 1.0f, float farPlane = 1000.0f, bool originUpperLeft = true );
//! Sets the viewport and \c MODELVIEW and \c PROJECTION matrices to be a perspective projection with the upper-left corner at \c [0,0] and the lower-right at \c [screenWidth,screenHeight], but flipped vertically if not \a originUpperLeft.
inline void setMatricesWindowPersp( const Vec2i &screenSize, float fovDegrees = 60.0f, float nearPlane = 1.0f, float farPlane = 1000.0f, bool originUpperLeft = true )
	{ setMatricesWindowPersp( screenSize.x, screenSize.y, fovDegrees, nearPlane, farPlane ); }
//! Sets the viewport and \c MODELVIEW and \c PROJECTION matrices to orthographic with the upper-left corner at \c [0,0] and the lower-right at \c [screenWidth,screenHeight] if \a originUpperLeft is \c true. Otherwise the origin is in the lower right.
void setMatricesWindow( int screenWidth, int screenHeight, bool originUpperLeft = true );
//! Sets the viewport and the \c MODELVIEW and \c PROJECTION matrices to orthographic with the upper-left corner at \c [0,0] and the lower-right at \c [size.x,size.y] if \a originUpperLeft is \c true. Otherwise the origin is in the lower right.
inline void setMatricesWindow( const Vec2i &screenSize, bool originUpperLeft = true ) { setMatricesWindow( screenSize.x, screenSize.y, originUpperLeft ); }

//! Returns the current OpenGL Viewport as an Area
Area getViewport();
//! Sets the current OpenGL Viewport to \a area
void setViewport( const Area &area );

//! Produces a translation by \a pos in the current matrix.
void translate( const Vec2f &pos );
//! Produces a translation by \a pos in the current matrix.
void translate( const Vec3f &pos );
//! Produces a scale by \a pos in the current matrix.
void scale( const Vec3f &scale );
//! Produces a rotation around the X-axis by \a xyz.x degrees, the Y-axis by \a xyz.y degrees and the Z-axis by \a xyz.z degrees in the current matrix. Processed in X-Y-Z order.
void rotate( const Vec3f &xyz );
//! Produces a rotation by the quaternion \a quat in the current matrix.
void rotate( const Quatf &quat );
//! Produces a 2D rotation, the equivalent of a rotation around the Z axis by \a degrees.
inline void rotate( float degrees ) { rotate( Vec3f( 0, 0, degrees ) ); }

#if ! defined( CINDER_GLES )
//! Used between calls to \c glBegin and \c glEnd, appends a vertex to the current primitive.
inline void vertex( const Vec2f &v ) { glVertex2fv( &v.x ); }
//! Used between calls to \c glBegin and \c glEnd, appends a vertex to the current primitive.
inline void vertex( const Vec3f &v ) { glVertex3fv( &v.x ); }
#endif // ! defined( CINDER_GLES )
//! Sets the current color, and the alpha value to 1.0
inline void color( const Color8u &c ) { glColor4ub( c.r, c.g, c.b, 255 ); }
//! Sets the current color and alpha value
inline void color( const ColorA8u &c ) { glColor4ub( c.r, c.g, c.b, c.a ); }
//! Sets the current color, and the alpha value to 1.0
inline void color( const Color &c ) { glColor4f( c.r, c.g, c.b, 1.0f ); }
//! Sets the current color and alpha value
inline void color( const ColorA &c ) { glColor4f( c.r, c.g, c.b, c.a ); }

//! Enables the OpenGL State \a state. Equivalent to calling to glEnable( state );
inline void enable( GLenum state ) { glEnable( state ); }
//! Disables the OpenGL State \a state. Equivalent to calling to glDisable( state );
inline void disable( GLenum state ) { glDisable( state ); }

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

//! Draws a line from \a start to \a end
void drawLine( const Vec2f &start, const Vec2f &end );
//! Draws a line from \a start to \a end
void drawLine( const Vec3f &start, const Vec3f &end );
//! Renders a solid cube centered at \a center of size \a size. Normals and created texture coordinates are generated for \c GL_TEXTURE_2D, with each face in the range [0,0] - [1.0,1.0]
void drawCube( const Vec3f &center, const Vec3f &size );
//! Renders a solid cube centered at \a center of size \a size. Each face is assigned a unique color, and no normals or texture coordinates are generated.
void drawColorCube( const Vec3f &center, const Vec3f &size );
//! Renders a stroked cube centered at \a center of size \a size.
void drawStrokedCube( const Vec3f &center, const Vec3f &size );
//! Renders a stroked cube \a aab
inline void drawStrokedCube( const AxisAlignedBox3f &aab ) { drawStrokedCube( aab.getCenter(), aab.getSize() ); }
//! Renders a solid sphere centered at \a center of radius \a radius. \a segments defines how many segments the sphere is subdivided into. Normals and texture coordinates in the range [0,1] are generated.
void drawSphere( const Vec3f &center, float radius, int segments = 12 );
//! Renders a solid sphere. \a segments defines how many segments the sphere is subdivided into. Normals and texture coordinates in the range [0,1] are generated.
void draw( const class Sphere &sphere, int segments = 12 );
//! Renders a solid circle using triangle fans. The default value of zero for \a numSegments automatically determines a number of segments based on the circle's circumference.
void drawSolidCircle( const Vec2f &center, float radius, int numSegments = 0 );
//! Renders a stroked circle using a line loop. The default value of zero for \a numSegments automatically determines a number of segments based on the circle's circumference.
void drawStrokedCircle( const Vec2f &center, float radius, int numSegments = 0 );
//! Renders a solid rectangle. Texture coordinates in the range [0,1] are generated unless \a textureRectangle.
void drawSolidRect( const Rectf &rect, bool textureRectangle = false );
//! Renders a coordinate frame representation centered at the origin. Arrowheads are drawn at the end of each axis with radius \a headRadius and length \a headLength.
void drawCoordinateFrame( float axisLength = 1.0f, float headLength = 0.2f, float headRadius = 0.05f );
//! Draws a vector starting at \a start and ending at \a end. An arrowhead is drawn at the end of radius \a headRadius and length \a headLength.
void drawVector( const Vec3f &start, const Vec3f &end, float headLength = 0.2f, float headRadius = 0.05f );
//! Draws a wireframe representation of the frustum defined by \a cam.
void drawFrustum( const Camera &cam );
//! Draws a torus at the origin, with an outter radius \a outterRadius and an inner radius \a innerRadius, subdivided into \a longitudeSegments and \a latitudeSegments. Normals and texture coordinates in the range [0,1] are generated.
void drawTorus( float outterRadius, float innerRadius, int longitudeSegments = 12, int latitudeSegments = 12 );
//! Draws a PolyLine \a polyLine
void draw( const class PolyLine<Vec2f> &polyLine );

#if ! defined( CINDER_GLES )
//! Draws a cinder::TriMesh \a mesh at the origin.
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
#endif // ! defined( CINDER_GLES )

void drawBillboard( const Vec3f &pos, const Vec2f &scale, float rotationDegrees, const Vec3f &bbRight, const Vec3f &bbUp );
//! Draws \a texture on the XY-plane
void draw( const Texture &texture );
//! Draws \a texture on the XY-plane at \a pos
void draw( const Texture &texture, const Vec2f &pos );
//! Draws \a texture on the XY-plane in the rectangle defined by \a rect
void draw( const Texture &texture, const Rectf &rect );
//! Draws the pixels inside \a srcArea of \a texture on the XY-plane in the rectangle defined by \a destRect
void draw( const Texture &texture, const Area &srcArea, const Rectf &destRect );

//! Draws a string \a str with its lower left corner located at \a pos. Optional \a font and \a color affect the style.
void drawString( const std::string &str, const Vec2f &pos, const ColorA &color = ColorA( 1, 1, 1, 1 ), Font font = Font() );
//! Draws a string \a str with the horizontal center of its baseline located at \a pos. Optional \a font and \a color affect the style
void drawStringCentered( const std::string &str, const Vec2f &pos, const ColorA &color = ColorA( 1, 1, 1, 1 ), Font font = Font() );
//! Draws a right-justified string \a str with the center of its  located at \a pos. Optional \a font and \a color affect the style
void drawStringRight( const std::string &str, const Vec2f &pos, const ColorA &color = ColorA( 1, 1, 1, 1 ), Font font = Font() );


//! Convenience class designed to push and pop the currently bound texture for a given texture unit
struct SaveTextureBindState {
	SaveTextureBindState( GLint target );
	~SaveTextureBindState();
  private:
	GLint	mTarget;
	GLint	mOldID;
};

//! Convenience class designed to push and pop the enabled/disabled state of a given texture unit
struct SaveTextureEnabledState {
	SaveTextureEnabledState( GLint target );
	~SaveTextureEnabledState();
  private:
	GLint		mTarget;
	GLboolean	mOldValue;
};

//! Convenience class designed to push and pop the current color
struct SaveColorState {
	SaveColorState();
	~SaveColorState();
  private:
	GLfloat		mOldValues[4];
};

//! Convenience class which pushes and pops the currently bound framebuffer
struct SaveFramebufferBinding {
	SaveFramebufferBinding();
	~SaveFramebufferBinding();
  private:
	GLint		mOldValue;
};

#if defined( CINDER_MSW )
//! Initializes the GLee library. This is generally called automatically by the application and is only necessary if you need to use GLee before your app's setup() method is called.
void initializeGlee();
#endif

} } // namespace cinder::gl 

//@{
//! Global overloads for OpenGL free functions to allow the use of Cinder types natively
#if ! defined( CINDER_GLES )
inline void glVertex2f( const cinder::Vec2f &v ) { glVertex2f( v.x, v.y ); }
inline void glVertex3f( const cinder::Vec3f &v ) { glVertex3f( v.x, v.y, v.z ); }
inline void glVertex4f( const cinder::Vec4f &v ) { glVertex4f( v.x, v.y, v.z, v.w ); }
inline void glNormal3f( const cinder::Vec3f &v ) { glNormal3f( v.x, v.y, v.z ); }
inline void glColor3f( const cinder::Color &c ) { glColor3f( c.r, c.g, c.b ); }
inline void glColor4f( const cinder::ColorA &c ) { glColor4f( c.r, c.g, c.b, c.a ); }
inline void glTexCoord2f( const cinder::Vec2f &v ) { glTexCoord2f( v.x, v.y ); }
inline void glTexCoord3f( const cinder::Vec3f &v ) { glTexCoord3f( v.x, v.y, v.z ); }
inline void glTexCoord4f( const cinder::Vec4f &v ) { glTexCoord4f( v.x, v.y, v.z, v.w ); }
// This style of definition conflicts with GLee
//inline void glMultiTexCoord2f( GLenum target, const cinder::Vec2f &v ) { glMultiTexCoord2f( target, v.x, v.y ); }
//inline void glMultiTexCoord3f( GLenum target, const cinder::Vec3f &v ) { glMultiTexCoord3f( target, v.x, v.y, v.z ); }
//inline void glMultiTexCoord4f( GLenum target, const cinder::Vec4f &v ) { glMultiTexCoord4f( target, v.x, v.y, v.z, v.w ); }
#endif // ! defined( CINDER_GLES )
inline void glTranslatef( const cinder::Vec3f &v ) { glTranslatef( v.x, v.y, v.z ); }
inline void glScalef( const cinder::Vec3f &v ) { glScalef( v.x, v.y, v.z ); }
inline void glRotatef( float angle, const cinder::Vec3f &v ) { glRotatef( angle, v.x, v.y, v.z ); }
inline void glRotatef( const cinder::Quatf &quat ) { cinder::Vec3f axis; float angle; quat.getAxisAngle( &axis, &angle ); glRotatef( cinder::toDegrees( angle ), axis.x, axis.y, axis.z ); }
inline void glMultMatrixf( const cinder::Matrix44f &m ) { glMultMatrixf( m.m ); }
inline void glLoadMatrixf( const cinder::Matrix44f &m ) { glLoadMatrixf( m.m ); }
//@}