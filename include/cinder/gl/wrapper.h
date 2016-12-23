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
	UNIFORM_VIEW_PROJECTION,
	UNIFORM_NORMAL_MATRIX,
	UNIFORM_VIEWPORT_MATRIX,
	UNIFORM_WINDOW_SIZE,
	UNIFORM_ELAPSED_SECONDS,
	UNIFORM_USER_DEFINED
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
//! Returns a string describing the active GL context.
std::string getString( GLenum name );
//! Returns a string representing the active GL context's version or release number.
std::string getVersionString();
//! Returns a string representing the company responsible for this GL implementation.
std::string getVendorString();


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
	
void stencilFunc( GLenum func, GLint ref, GLuint mask );
void stencilOp( GLenum fail, GLenum zfail, GLenum zpass );
void stencilMask( GLuint mask );

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

//! Enables or disables blending state as governed by \c GL_BLEND but does not modify blend function.
void enableBlending( bool enable = false );
//! Disables blending state via \c GL_BLEND, but does not modify blend function
inline void disableBlending() { enableBlending( false ); }
//! Enables blending via \c GL_BLEND and sets the blend function to unpremultiplied alpha blending when \p enable is \c true; otherwise disables blending without modifying the blend function.
void enableAlphaBlending( bool enable = true );
//! Enables blending via \c GL_BLEND and sets the blend function to premultiplied alpha blending
void enableAlphaBlendingPremult();
//! Disables blending state as governed by \c GL_BLEND but does not modify blend function.. Deprecated; prefer disableBlending()
inline void disableAlphaBlending() { disableBlending(); }
//! Enables \c GL_BLEND and sets the blend function to additive blending
void enableAdditiveBlending();

//! Specifies whether polygons are culled. Equivalent to calling enable( \c GL_CULL_FACE, \a enable ). Specify front or back faces with gl::cullFace().
void enableFaceCulling( bool enable = true );
//! Specifies whether front or back-facing polygons are culled (as specified by \a face) when polygon culling is enabled. Valid values are \c GL_BACK and \c GL_FRONT.
void cullFace( GLenum face );

#if ! defined( CINDER_GL_ES )
//! Specifies whether logic operations are enabled. Equivalent to calling enable( \c GL_COLOR_LOGIC_OP, \a enable ). Specify symbolic constants that selects the logical operation with gl::logicOp().
void enableLogicOp( bool enable = true );
//! Specifies a symbolic constant that selects a logical operation. Valid values are \c GL_CLEAR, \c GL_SET, \c GL_COPY, \c GL_COPY_INVERTED, \c GL_NOOP, \c GL_INVERT, \c GL_AND, \c GL_NAND, \c GL_OR, \c GL_NOR, \c GL_XOR, \c GL_EQUIV, \c GL_AND_REVERSE, \c GL_AND_INVERTED, \c GL_OR_REVERSE, or \c GL_OR_INVERTED.
void logicOp( GLenum mode );
#endif

//! Disables reading / testing from the depth buffer. Disables \c GL_DEPTH_TEST
void disableDepthRead();
//! Disables writing to depth buffer; analogous to calling glDepthMask( GL_FALSE );
void disableDepthWrite();
//! Enables or disables reading / testing from depth buffer; analogous to setting \c GL_DEPTH_TEST to \p enable
void enableDepthRead( bool enable = true );
//! Enables or disables writing to depth buffer; analogous to calling glDepthMask( \p enable ); Note that reading must also be enabled for writing to have any effect.
void enableDepthWrite( bool enable = true );
//! Enables or disables writing to and reading / testing from depth buffer
inline void enableDepth( bool enable = true ) { enableDepthRead( enable ); enableDepthWrite( enable ); }

//! Enables or disables the stencil test operation, which controls reading and writing to the stencil buffer. Analagous to `glEnable( GL_STENCIL_TEST, enable );`
void enableStencilTest( bool enable = true );
//! Disables the stencil test operation. Analagous to `glEnable( GL_STENCIL_TEST, false );`
void disableStencilTest();
 
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
#if ! defined( CINDER_GL_ES )
//! Specifies the parameters that will be used for patch primitives. Analogous to glPatchParameterfv().
void patchParameterfv( GLenum pname, GLfloat *value );
#endif
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
#if ! defined( CINDER_GL_ES_2 )
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
//! Binds a named buffer object \a buffer to \a target. Analogous to glBindBuffer().
void	bindBuffer( GLenum target, GLuint buffer );
#if ! defined( CINDER_GL_ES_2 )
//! Specifies a color buffer as the source for subsequent glReadPixels(), glCopyTexImage2D(), glCopyTexSubImage2D(), and glCopyTexSubImage3D() commands. Analogous to glReadBuffer().
void	readBuffer( GLenum src );
//! Specifies an array of buffers into which fragment color values or fragment data will be written for subsequent draw calls. Analogous to glDrawBuffers().
void	drawBuffers( GLsizei num, const GLenum *bufs );
//! Specifies a color buffer as the destination for subsequent draw calls. Analogous to glDrawBuffer(), and emulated on ES 3
void	drawBuffer( GLenum dst );
#endif

//! Reads a block of pixels from the framebuffer. Analogous to glReadPixels().
void	readPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *data );

// Compute
#if defined( CINDER_GL_HAS_COMPUTE_SHADER )
//! Launches one or more compute work groups. Analogous to glDispatchCompute(). 
inline void	dispatchCompute( GLuint numGroupsX, GLuint numGroupsY = 1, GLuint numGroupsZ = 1 ) { glDispatchCompute( numGroupsX, numGroupsY, numGroupsZ ); }
//! Defines a barrier ordering memory transactions. Analogous to glMemoryBarrier().
inline void	memoryBarrier( GLbitfield barriers ) { glMemoryBarrier( barriers ); }

//! Returns ivec3( GL_MAX_COMPUTE_WORK_GROUP_COUNT )
ivec3	getMaxComputeWorkGroupCount();
//! Returns ivec3( GL_MAX_COMPUTE_WORK_GROUP_SIZE )
ivec3	getMaxComputeWorkGroupSize();
#endif

class Exception : public cinder::Exception {
  public:
	Exception()	{}
	Exception( const std::string &description ) : cinder::Exception( description )	{}
};

class ExceptionUnknownTarget : public Exception {
};

} } // namespace cinder::gl

