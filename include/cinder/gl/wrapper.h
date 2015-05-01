/*
 Copyright (c) 2015, The Cinder Project, All rights reserved.

 This code is intended for use with the Cinder C++ library: http://libcinder.org

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

#include "cinder/gl/platform.h"
#include "cinder/Camera.h"
#include "cinder/Color.h"
#include "cinder/GeomIo.h"
#include "cinder/Matrix44.h"

#if ! defined( NDEBUG )
	#define CI_CHECK_GL()	cinder::gl::checkError()
#else
	#define CI_CHECK_GL()	((void)0)
#endif

namespace cinder { namespace gl {

typedef std::shared_ptr<class GlslProg>			GlslProgRef;
typedef std::shared_ptr<class BufferObj>		BufferObjRef;

// Remember to add a matching case to uniformSemanticToString
enum UniformSemantic {
	UNIFORM_MODEL_MATRIX,
	UNIFORM_MODEL_MATRIX_INVERSE,
	UNIFORM_MODEL_MATRIX_INVERSE_TRANSPOSE,
	UNIFORM_VIEW_MATRIX,
	UNIFORM_VIEW_MATRIX_INVERSE,
	UNIFORM_MODEL_VIEW,
	UNIFORM_MODEL_VIEW_INVERSE,
	UNIFORM_MODEL_VIEW_INVERSE_TRANSPOSE,
	UNIFORM_MODEL_VIEW_PROJECTION,
	UNIFORM_MODEL_VIEW_PROJECTION_INVERSE,
	UNIFORM_PROJECTION_MATRIX,
	UNIFORM_PROJECTION_MATRIX_INVERSE,
	UNIFORM_NORMAL_MATRIX,
	UNIFORM_VIEWPORT_MATRIX,
	UNIFORM_WINDOW_SIZE,
	UNIFORM_ELAPSED_SECONDS
};

class Context* context();
class Environment* env();

void enableVerticalSync( bool enable = true );
bool isVerticalSyncEnabled();

GLenum getError();
std::string getErrorString( GLenum err );
void checkError();

//! Returns whether OpenGL Extension \a extName is implemented on the hardware. For example, \c "GL_EXT_texture_swizzle". Case insensitive.
bool isExtensionAvailable( const std::string &extName );
//! Returns the OpenGL version number as a pair<major,minor>
std::pair<GLint,GLint>	getVersion();
std::string getVersionString();

GlslProgRef& getStockShader( const class ShaderDef &shader );
void bindStockShader( const class ShaderDef &shader );
void setDefaultShaderVars();

void clear( const ColorA &color = ColorA::black(), bool clearDepthBuffer = true );
	
void clear( GLbitfield mask );
void clearColor( const ColorA &color );
void clearDepth( const double depth );
void clearDepth( const float depth );
void clearStencil( const int s );
	
void colorMask( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha );
void depthMask( GLboolean flag );
void stencilMask( GLboolean mask );
	
void stencilFunc( GLenum func, GLint ref, GLuint mask );
void stencilOp( GLenum fail, GLenum zfail, GLenum zpass );

std::pair<ivec2, ivec2> getViewport();
void viewport( const std::pair<ivec2, ivec2> positionAndSize );
inline void viewport( int x, int y, int width, int height ) { viewport( std::pair<ivec2, ivec2>( ivec2( x, y ), ivec2( width, height ) ) ); }
inline void viewport( const ivec2 &position, const ivec2 &size ) { viewport( std::pair<ivec2, ivec2>( position, size ) ); }
inline void viewport( const ivec2 &size ) { viewport( ivec2(), size ); }
void pushViewport( const std::pair<ivec2, ivec2> positionAndSize );
inline void pushViewport() { pushViewport( getViewport() ); }
inline void pushViewport( int x, int y, int width, int height ) { pushViewport( std::pair<ivec2, ivec2>( ivec2( x, y ), ivec2( width, height ) ) ); }
inline void pushViewport( const ivec2 &position, const ivec2 &size ) { pushViewport( std::pair<ivec2, ivec2>( position, size ) ); }
inline void pushViewport( const ivec2 &size ) { pushViewport( ivec2(), size ); }
void popViewport();

std::pair<ivec2, ivec2> getScissor();
void scissor( const std::pair<ivec2, ivec2> positionAndSize );
inline void scissor( int x, int y, int width, int height ) { scissor( std::pair<ivec2, ivec2>( ivec2( x, y ), ivec2( width, height ) ) ); }
inline void scissor( const ivec2 &position, const ivec2 &size ) { scissor( std::pair<ivec2, ivec2>( position, size ) ); }
	
void enable( GLenum state, bool enable = true );
inline void disable( GLenum state ) { enable( state, false ); }

void enableAlphaBlending( bool premultiplied = false );
void disableAlphaBlending();
void enableAdditiveBlending();

//! Specifies whether polygons are culled. Equivalent to calling enable( \c GL_CULL_FACE, \a enable ). Specify front or back faces with gl::cullFace().
void enableFaceCulling( bool enable = true );
//! Specifies whether front or back-facing polygons are culled (as specified by \a face) when polygon culling is enabled. Valid values are \c GL_BACK and \c GL_FRONT.
void cullFace( GLenum face );

void disableDepthRead();
void disableDepthWrite();
void enableDepthRead( bool enable = true );
void enableDepthWrite( bool enable = true );

void enableStencilRead( bool enable = true );
void disableStencilRead();
void enableStencilWrite( bool enable = true );
void disableStencilWrite();

//! Sets the View and Projection matrices based on a Camera
void setMatrices( const ci::Camera &cam );
void setModelMatrix( const ci::mat4 &m );
void setViewMatrix( const ci::mat4 &m );
void setProjectionMatrix( const ci::mat4 &m );
void pushModelMatrix();
void popModelMatrix();
void pushViewMatrix();
void popViewMatrix();
void pushProjectionMatrix();
void popProjectionMatrix();
//! Pushes Model and View matrices
void pushModelView();
//! Pops Model and View matrices
void popModelView();
//! Pushes Model, View and Projection matrices
void pushMatrices();
//! Pops Model, View and Projection matrices
void popMatrices();
void multModelMatrix( const ci::mat4 &mtx );
void multViewMatrix( const ci::mat4 &mtx );
void multProjectionMatrix( const ci::mat4 &mtx );

mat4 getModelMatrix();
mat4 getViewMatrix();
mat4 getProjectionMatrix();
mat4 getModelView();
mat4 getModelViewProjection();
mat4 calcViewMatrixInverse();
mat3 calcModelMatrixInverseTranspose();
mat3 calcNormalMatrix();
mat4 calcViewportMatrix();

void setMatricesWindowPersp( int screenWidth, int screenHeight, float fovDegrees = 60.0f, float nearPlane = 1.0f, float farPlane = 1000.0f, bool originUpperLeft = true );
void setMatricesWindowPersp( const ci::ivec2 &screenSize, float fovDegrees = 60.0f, float nearPlane = 1.0f, float farPlane = 1000.0f, bool originUpperLeft = true );
void setMatricesWindow( int screenWidth, int screenHeight, bool originUpperLeft = true );
void setMatricesWindow( const ci::ivec2 &screenSize, bool originUpperLeft = true );

void rotate( const quat &quat );
//! Rotates the Model matrix by \a angleRadians around the \a axis
void rotate( float angleRadians, const ci::vec3 &axis );
//! Rotates the Model matrix by \a angleRadians around the axis (\a x,\a y,\a z)
inline void rotate( float angleRadians, float xAxis, float yAxis, float zAxis ) { rotate( angleRadians, ci::vec3(xAxis, yAxis, zAxis) ); }
//! Rotates the Model matrix by \a zRadians around the z-axis
inline void rotate( float zRadians ) { rotate( zRadians, vec3( 0, 0, 1 ) ); }

//! Scales the Model matrix by \a v
void scale( const ci::vec3 &v );
//! Scales the Model matrix by (\a x,\a y, \a z)
inline void scale( float x, float y, float z ) { scale( vec3( x, y, z ) ); }
//! Scales the Model matrix by \a v
inline void scale( const ci::vec2 &v ) { scale( vec3( v.x, v.y, 1 ) ); }
//! Scales the Model matrix by (\a x,\a y, 1)
inline void scale( float x, float y ) { scale( vec3( x, y, 1 ) ); }

//! Translates the Model matrix by \a v
void translate( const ci::vec3 &v );
//! Translates the Model matrix by (\a x,\a y,\a z )
inline void translate( float x, float y, float z ) { translate( vec3( x, y, z ) ); }
//! Translates the Model matrix by \a v
inline void translate( const ci::vec2 &v ) { translate( vec3( v, 0 ) ); }
//! Translates the Model matrix by (\a x,\a y)
inline void translate( float x, float y ) { translate( vec3( x, y, 0 ) ); }

//! Returns the object space coordinate of the specified window \a coordinate, using the specified \a modelMatrix and the currently active view and projection matrices.
vec3 windowToObjectCoord( const mat4 &modelMatrix, const vec2 &coordinate, float z = 0.0f );
//! Returns the window coordinate of the specified world \a coordinate, using the specified \a modelMatrix and the currently active view and projection matrices.
vec2 objectToWindowCoord( const mat4 &modelMatrix, const vec3 &coordinate );
//! Returns the object space coordinate of the specified window \a coordinate, using the currently active model, view and projection matrices.
inline vec3 windowToObjectCoord( const vec2 &coordinate, float z = 0.0f ) { return windowToObjectCoord( gl::getModelMatrix(), coordinate, z ); }
//! Returns the window coordinate of the specified world \a coordinate, using the currently active model, view and projection matrices.
inline vec2 objectToWindowCoord( const vec3 &coordinate ) { return objectToWindowCoord( gl::getModelMatrix(), coordinate ); }
//! Returns the world space coordinate of the specified window \a coordinate, using the currently active view and projection matrices.
inline vec3 windowToWorldCoord( const vec2 &coordinate, float z = 0.0f ) { return windowToObjectCoord( mat4(), coordinate, z ); }
//! Returns the window coordinate of the specified world \a coordinate, using the currently active view and projection matrices.
inline vec2 worldToWindowCoord( const vec3 &coordinate ) { return objectToWindowCoord( mat4(), coordinate ); }

void begin( GLenum mode );
void end();

#if ! defined( CINDER_GL_ES_2 )
void bindBufferBase( GLenum target, int index, BufferObjRef buffer );
	
void beginTransformFeedback( GLenum primitiveMode );
void endTransformFeedback();
void resumeTransformFeedback();
void pauseTransformFeedback();

// Tesselation
//! Specifies the parameters that will be used for patch primitives. Analogous to glPatchParameteri().
void patchParameteri( GLenum pname, GLint value );
//! Specifies the parameters that will be used for patch primitives. Analogous to glPatchParameterfv().
void patchParameterfv( GLenum pname, GLfloat *value );
#endif
	
void color( float r, float g, float b );
void color( float r, float g, float b, float a );
void color( const ci::Color &c );
void color( const ci::ColorA &c );
void color( const ci::Color8u &c );
void color( const ci::ColorA8u &c );

void texCoord( float s, float t );
void texCoord( float s, float t, float r );
void texCoord( float s, float t, float r, float q );
void texCoord( const ci::vec2 &v );
void texCoord( const ci::vec3 &v );
void texCoord( const ci::vec4 &v );

void vertex( float x, float y );
void vertex( float x, float y, float z );
void vertex( float x, float y, float z, float w );
void vertex( const ci::vec2 &v );
void vertex( const ci::vec3 &v );
void vertex( const ci::vec4 &v );

#if ! defined( CINDER_GL_ES )
//! Sets the current polygon rasterization mode. \a face must be \c GL_FRONT_AND_BACK. \c GL_POINT, \c GL_LINE & \c GL_FILL are legal values for \a mode.
void polygonMode( GLenum face, GLenum mode );
//! Enables wireframe drawing by setting the \c PolygonMode to \c GL_LINE.
void enableWireframe();
//! Disables wireframe drawing.
void disableWireframe();
//! Returns whether wirefrom drawing is enabled.
bool isWireframeEnabled();
//! Toggles wireframe drawing according to \a enable.
inline void setWireframeEnabled( bool enable = true )	{ if( enable ) enableWireframe(); else disableWireframe(); }
#endif

//! Sets the width of rasterized lines to \a width. The initial value is 1. Analogous to glLineWidth(). 
void	lineWidth( float width );
#if ! defined( CINDER_GL_ES )
//! Specifies the rasterized diameter of points. If point size mode is disabled (via gl::disable \c GL_PROGRAM_POINT_SIZE), this value will be used to rasterize points. Otherwise, the value written to the shading language built-in variable \c gl_PointSize will be used. Analogous to glPointSize().
void	pointSize( float size );
#endif

//! Converts a geom::Primitive to an OpenGL primitive mode( GL_TRIANGLES, GL_TRIANGLE_STRIP, etc )
GLenum toGl( geom::Primitive prim );
//! Converts an OpenGL primitive mode( GL_TRIANGLES, GL_TRIANGLE_STRIP, etc ) to a geom::Primitive
geom::Primitive toGeomPrimitive( GLenum prim );
//! Converts a UniformSemantic to its name
std::string uniformSemanticToString( UniformSemantic uniformSemantic );

// Vertex Attributes
//! Analogous to glVertexAttribPointer
void	vertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer );
#if ! defined( CINDER_GL_ES )
//! Analogous to glVertexAttribIPointer
void	vertexAttribIPointer( GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer );
#endif // ! defined( CINDER_GL_ES )
//! Analogous to glEnableVertexAttribArray
void	enableVertexAttribArray( GLuint index );

void		vertexAttrib1f( GLuint index, float v0 );
inline void	vertexAttrib( GLuint index, float v0 ) { vertexAttrib1f( index, v0 ); }
void		vertexAttrib2f( GLuint index, float v0, float v1 );
inline void	vertexAttrib( GLuint index, float v0, float v1 ) { vertexAttrib2f( index, v0, v1 ); }
void		vertexAttrib3f( GLuint index, float v0, float v1, float v2 );
inline void	vertexAttrib( GLuint index, float v0, float v1, float v2 ) { vertexAttrib3f( index, v0, v1, v2 ); }
inline void	vertexAttrib( GLuint index, float v0, float v1 );
void		vertexAttrib4f( GLuint index, float v0, float v1, float v2, float v3 );
inline void	vertexAttrib( GLuint index, float v0, float v1, float v2, float v3 ) { vertexAttrib4f( index, v0, v1, v2, v3 ); }

// Buffers
void	bindBuffer( const BufferObjRef &buffer );
#if ! defined( CINDER_GL_ES_2 )
//! Specifies a color buffer as the source for subsequent glReadPixels(), glCopyTexImage2D(), glCopyTexSubImage2D(), and glCopyTexSubImage3D() commands. Analogous to glReadBuffer().
void	readBuffer( GLenum src );
//! Specifies an array of buffers into which fragment color values or fragment data will be written for subsequent draw calls. Analogous to glDrawBuffers().
void	drawBuffers( GLsizei num, const GLenum *bufs );
#endif
#if ! defined( CINDER_GL_ES )
//! Specifies a color buffer as the destination for subsequent draw calls. Analogous to glDrawBuffer().
void	drawBuffer( GLenum dst );
#endif

//! Reads a block of pixels from the framebuffer. Analogous to glReadPixels().
void	readPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *data );

void	drawArrays( GLenum mode, GLint first, GLsizei count );
void	drawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices );


class Exception : public cinder::Exception {
  public:
	Exception()	{}
	Exception( const std::string &description ) : cinder::Exception( description )	{}
};

class ExceptionUnknownTarget : public Exception {
};

} } // namespace cinder::gl

