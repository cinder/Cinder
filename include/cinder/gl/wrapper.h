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

CI_API class Context* context();
CI_API class Environment* env();

CI_API void enableVerticalSync( bool enable = true );
CI_API bool isVerticalSyncEnabled();

CI_API GLenum getError();
CI_API std::string getErrorString( GLenum err );
CI_API void checkError();

//! Returns whether OpenGL Extension \a extName is implemented on the hardware. For example, \c "GL_EXT_texture_swizzle". Case insensitive.
CI_API bool isExtensionAvailable( const std::string &extName );
//! Returns the OpenGL version number as a pair<major,minor>
CI_API std::pair<GLint,GLint>	getVersion();
//! Returns a string describing the active GL context.
CI_API std::string getString( GLenum name );
//! Returns a string representing the active GL context's version or release number.
CI_API std::string getVersionString();
//! Returns a string representing the company responsible for this GL implementation.
CI_API std::string getVendorString();


CI_API GlslProgRef& getStockShader( const class ShaderDef &shader );
CI_API void bindStockShader( const class ShaderDef &shader );
CI_API void setDefaultShaderVars();

CI_API void clear( const ColorA &color = ColorA::black(), bool clearDepthBuffer = true );
	
CI_API void clear( GLbitfield mask );
CI_API void clearColor( const ColorA &color );
CI_API void clearDepth( const double depth );
CI_API void clearDepth( const float depth );
CI_API void clearStencil( const int s );
	
CI_API void colorMask( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha );
CI_API void depthMask( GLboolean flag );
	
CI_API void stencilFunc( GLenum func, GLint ref, GLuint mask );
CI_API void stencilOp( GLenum fail, GLenum zfail, GLenum zpass );
CI_API void stencilMask( GLuint mask );

CI_API std::pair<ivec2, ivec2> getViewport();
CI_API void viewport( const std::pair<ivec2, ivec2> positionAndSize );
CI_API inline void viewport( int x, int y, int width, int height ) { viewport( std::pair<ivec2, ivec2>( ivec2( x, y ), ivec2( width, height ) ) ); }
CI_API inline void viewport( const ivec2 &position, const ivec2 &size ) { viewport( std::pair<ivec2, ivec2>( position, size ) ); }
CI_API inline void viewport( const ivec2 &size ) { viewport( ivec2(), size ); }
CI_API void pushViewport( const std::pair<ivec2, ivec2> positionAndSize );
CI_API inline void pushViewport() { pushViewport( getViewport() ); }
CI_API inline void pushViewport( int x, int y, int width, int height ) { pushViewport( std::pair<ivec2, ivec2>( ivec2( x, y ), ivec2( width, height ) ) ); }
CI_API inline void pushViewport( const ivec2 &position, const ivec2 &size ) { pushViewport( std::pair<ivec2, ivec2>( position, size ) ); }
CI_API inline void pushViewport( const ivec2 &size ) { pushViewport( ivec2(), size ); }
CI_API void popViewport();

CI_API std::pair<ivec2, ivec2> getScissor();
CI_API void scissor( const std::pair<ivec2, ivec2> positionAndSize );
CI_API inline void scissor( int x, int y, int width, int height ) { scissor( std::pair<ivec2, ivec2>( ivec2( x, y ), ivec2( width, height ) ) ); }
CI_API inline void scissor( const ivec2 &position, const ivec2 &size ) { scissor( std::pair<ivec2, ivec2>( position, size ) ); }
	
CI_API void enable( GLenum state, bool enable = true );
CI_API inline void disable( GLenum state ) { enable( state, false ); }

//! Enables or disables blending state as governed by \c GL_BLEND but does not modify blend function.
CI_API void enableBlending( bool enable = false );
//! Disables blending state via \c GL_BLEND, but does not modify blend function
CI_API inline void disableBlending() { enableBlending( false ); }
//! Enables blending via \c GL_BLEND and sets the blend function to unpremultiplied alpha blending when \p enable is \c true; otherwise disables blending without modifying the blend function.
CI_API void enableAlphaBlending( bool enable = true );
//! Enables blending via \c GL_BLEND and sets the blend function to premultiplied alpha blending
CI_API void enableAlphaBlendingPremult();
//! Disables blending state as governed by \c GL_BLEND but does not modify blend function.. Deprecated; prefer disableBlending()
CI_API inline void disableAlphaBlending() { disableBlending(); }
//! Enables \c GL_BLEND and sets the blend function to additive blending
CI_API void enableAdditiveBlending();

//! Specifies whether polygons are culled. Equivalent to calling enable( \c GL_CULL_FACE, \a enable ). Specify front or back faces with gl::cullFace().
CI_API void enableFaceCulling( bool enable = true );
//! Specifies whether front or back-facing polygons are culled (as specified by \a face) when polygon culling is enabled. Valid values are \c GL_BACK and \c GL_FRONT.
CI_API void cullFace( GLenum face );

#if ! defined( CINDER_GL_ES )
//! Specifies whether logic operations are enabled. Equivalent to calling enable( \c GL_COLOR_LOGIC_OP, \a enable ). Specify symbolic constants that selects the logical operation with gl::logicOp().
CI_API void enableLogicOp( bool enable = true );
//! Specifies a symbolic constant that selects a logical operation. Valid values are \c GL_CLEAR, \c GL_SET, \c GL_COPY, \c GL_COPY_INVERTED, \c GL_NOOP, \c GL_INVERT, \c GL_AND, \c GL_NAND, \c GL_OR, \c GL_NOR, \c GL_XOR, \c GL_EQUIV, \c GL_AND_REVERSE, \c GL_AND_INVERTED, \c GL_OR_REVERSE, or \c GL_OR_INVERTED.
CI_API void logicOp( GLenum mode );
#endif

//! Disables reading / testing from the depth buffer. Disables \c GL_DEPTH_TEST
CI_API void disableDepthRead();
//! Disables writing to depth buffer; analogous to calling glDepthMask( GL_FALSE );
CI_API void disableDepthWrite();
//! Enables or disables reading / testing from depth buffer; analogous to setting \c GL_DEPTH_TEST to \p enable
CI_API void enableDepthRead( bool enable = true );
//! Enables or disables writing to depth buffer; analogous to calling glDepthMask( \p enable ); Note that reading must also be enabled for writing to have any effect.
CI_API void enableDepthWrite( bool enable = true );
//! Enables or disables writing to and reading / testing from depth buffer
CI_API inline void enableDepth( bool enable = true ) { enableDepthRead( enable ); enableDepthWrite( enable ); }

//! Enables or disables the stencil test operation, which controls reading and writing to the stencil buffer. Analagous to `glEnable( GL_STENCIL_TEST, enable );`
CI_API void enableStencilTest( bool enable = true );
//! Disables the stencil test operation. Analagous to `glEnable( GL_STENCIL_TEST, false );`
CI_API void disableStencilTest();
 
//! Sets the View and Projection matrices based on a Camera
CI_API void setMatrices( const ci::Camera &cam );
CI_API void setModelMatrix( const ci::mat4 &m );
CI_API void setViewMatrix( const ci::mat4 &m );
CI_API void setProjectionMatrix( const ci::mat4 &m );
CI_API void pushModelMatrix();
CI_API void popModelMatrix();
CI_API void pushViewMatrix();
CI_API void popViewMatrix();
CI_API void pushProjectionMatrix();
CI_API void popProjectionMatrix();
//! Pushes Model and View matrices
CI_API void pushModelView();
//! Pops Model and View matrices
CI_API void popModelView();
//! Pushes Model, View and Projection matrices
CI_API void pushMatrices();
//! Pops Model, View and Projection matrices
CI_API void popMatrices();
CI_API void multModelMatrix( const ci::mat4 &mtx );
CI_API void multViewMatrix( const ci::mat4 &mtx );
CI_API void multProjectionMatrix( const ci::mat4 &mtx );

CI_API mat4 getModelMatrix();
CI_API mat4 getViewMatrix();
CI_API mat4 getProjectionMatrix();
CI_API mat4 getModelView();
CI_API mat4 getModelViewProjection();
CI_API mat4 calcViewMatrixInverse();
CI_API mat3 calcModelMatrixInverseTranspose();
CI_API mat3 calcNormalMatrix();
CI_API mat4 calcViewportMatrix();

CI_API void setMatricesWindowPersp( int screenWidth, int screenHeight, float fovDegrees = 60.0f, float nearPlane = 1.0f, float farPlane = 1000.0f, bool originUpperLeft = true );
CI_API void setMatricesWindowPersp( const ci::ivec2 &screenSize, float fovDegrees = 60.0f, float nearPlane = 1.0f, float farPlane = 1000.0f, bool originUpperLeft = true );
CI_API void setMatricesWindow( int screenWidth, int screenHeight, bool originUpperLeft = true );
CI_API void setMatricesWindow( const ci::ivec2 &screenSize, bool originUpperLeft = true );

CI_API void rotate( const quat &quat );
//! Rotates the Model matrix by \a angleRadians around the \a axis
CI_API void rotate( float angleRadians, const ci::vec3 &axis );
//! Rotates the Model matrix by \a angleRadians around the axis (\a x,\a y,\a z)
CI_API inline void rotate( float angleRadians, float xAxis, float yAxis, float zAxis ) { rotate( angleRadians, ci::vec3(xAxis, yAxis, zAxis) ); }
//! Rotates the Model matrix by \a zRadians around the z-axis
CI_API inline void rotate( float zRadians ) { rotate( zRadians, vec3( 0, 0, 1 ) ); }

//! Scales the Model matrix by \a v
CI_API void scale( const ci::vec3 &v );
//! Scales the Model matrix by (\a x,\a y, \a z)
CI_API inline void scale( float x, float y, float z ) { scale( vec3( x, y, z ) ); }
//! Scales the Model matrix by \a v
CI_API inline void scale( const ci::vec2 &v ) { scale( vec3( v.x, v.y, 1 ) ); }
//! Scales the Model matrix by (\a x,\a y, 1)
CI_API inline void scale( float x, float y ) { scale( vec3( x, y, 1 ) ); }

//! Translates the Model matrix by \a v
CI_API void translate( const ci::vec3 &v );
//! Translates the Model matrix by (\a x,\a y,\a z )
CI_API inline void translate( float x, float y, float z ) { translate( vec3( x, y, z ) ); }
//! Translates the Model matrix by \a v
CI_API inline void translate( const ci::vec2 &v ) { translate( vec3( v, 0 ) ); }
//! Translates the Model matrix by (\a x,\a y)
CI_API inline void translate( float x, float y ) { translate( vec3( x, y, 0 ) ); }

//! Returns the object space coordinate of the specified window \a coordinate, using the specified \a modelMatrix and the currently active view and projection matrices.
CI_API vec3 windowToObjectCoord( const mat4 &modelMatrix, const vec2 &coordinate, float z = 0.0f );
//! Returns the window coordinate of the specified world \a coordinate, using the specified \a modelMatrix and the currently active view and projection matrices.
CI_API vec2 objectToWindowCoord( const mat4 &modelMatrix, const vec3 &coordinate );
//! Returns the object space coordinate of the specified window \a coordinate, using the currently active model, view and projection matrices.
CI_API inline vec3 windowToObjectCoord( const vec2 &coordinate, float z = 0.0f ) { return windowToObjectCoord( gl::getModelMatrix(), coordinate, z ); }
//! Returns the window coordinate of the specified world \a coordinate, using the currently active model, view and projection matrices.
CI_API inline vec2 objectToWindowCoord( const vec3 &coordinate ) { return objectToWindowCoord( gl::getModelMatrix(), coordinate ); }
//! Returns the world space coordinate of the specified window \a coordinate, using the currently active view and projection matrices.
CI_API inline vec3 windowToWorldCoord( const vec2 &coordinate, float z = 0.0f ) { return windowToObjectCoord( mat4(), coordinate, z ); }
//! Returns the window coordinate of the specified world \a coordinate, using the currently active view and projection matrices.
CI_API inline vec2 worldToWindowCoord( const vec3 &coordinate ) { return objectToWindowCoord( mat4(), coordinate ); }

CI_API void begin( GLenum mode );
CI_API void end();

#if ! defined( CINDER_GL_ES_2 )
CI_API void bindBufferBase( GLenum target, int index, BufferObjRef buffer );
	
CI_API void beginTransformFeedback( GLenum primitiveMode );
CI_API void endTransformFeedback();
CI_API void resumeTransformFeedback();
CI_API void pauseTransformFeedback();

// Tesselation
//! Specifies the parameters that will be used for patch primitives. Analogous to glPatchParameteri().
CI_API void patchParameteri( GLenum pname, GLint value );
#if ! defined( CINDER_GL_ES )
//! Specifies the parameters that will be used for patch primitives. Analogous to glPatchParameterfv().
CI_API void patchParameterfv( GLenum pname, GLfloat *value );
#endif
#endif
	
CI_API void color( float r, float g, float b );
CI_API void color( float r, float g, float b, float a );
CI_API void color( const ci::Color &c );
CI_API void color( const ci::ColorA &c );
CI_API void color( const ci::Color8u &c );
CI_API void color( const ci::ColorA8u &c );

CI_API void texCoord( float s, float t );
CI_API void texCoord( float s, float t, float r );
CI_API void texCoord( float s, float t, float r, float q );
CI_API void texCoord( const ci::vec2 &v );
CI_API void texCoord( const ci::vec3 &v );
CI_API void texCoord( const ci::vec4 &v );

CI_API void vertex( float x, float y );
CI_API void vertex( float x, float y, float z );
CI_API void vertex( float x, float y, float z, float w );
CI_API void vertex( const ci::vec2 &v );
CI_API void vertex( const ci::vec3 &v );
CI_API void vertex( const ci::vec4 &v );

#if ! defined( CINDER_GL_ES )
//! Sets the current polygon rasterization mode. \a face must be \c GL_FRONT_AND_BACK. \c GL_POINT, \c GL_LINE & \c GL_FILL are legal values for \a mode.
CI_API void polygonMode( GLenum face, GLenum mode );
//! Enables wireframe drawing by setting the \c PolygonMode to \c GL_LINE.
CI_API void enableWireframe();
//! Disables wireframe drawing.
CI_API void disableWireframe();
//! Returns whether wirefrom drawing is enabled.
CI_API bool isWireframeEnabled();
//! Toggles wireframe drawing according to \a enable.
CI_API inline void setWireframeEnabled( bool enable = true )	{ if( enable ) enableWireframe(); else disableWireframe(); }
#endif

//! Sets the width of rasterized lines to \a width. The initial value is 1. Analogous to glLineWidth(). 
CI_API void	lineWidth( float width );
#if ! defined( CINDER_GL_ES )
//! Specifies the rasterized diameter of points. If point size mode is disabled (via gl::disable \c GL_PROGRAM_POINT_SIZE), this value will be used to rasterize points. Otherwise, the value written to the shading language built-in variable \c gl_PointSize will be used. Analogous to glPointSize().
CI_API void	pointSize( float size );
#endif

//! Converts a geom::Primitive to an OpenGL primitive mode( GL_TRIANGLES, GL_TRIANGLE_STRIP, etc )
CI_API GLenum toGl( geom::Primitive prim );
//! Converts an OpenGL primitive mode( GL_TRIANGLES, GL_TRIANGLE_STRIP, etc ) to a geom::Primitive
CI_API geom::Primitive toGeomPrimitive( GLenum prim );
//! Converts a UniformSemantic to its name
CI_API std::string uniformSemanticToString( UniformSemantic uniformSemantic );

// Vertex Attributes
//! Analogous to glVertexAttribPointer
CI_API void	vertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer );
#if ! defined( CINDER_GL_ES_2 )
//! Analogous to glVertexAttribIPointer
CI_API void	vertexAttribIPointer( GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer );
#endif // ! defined( CINDER_GL_ES )
//! Analogous to glEnableVertexAttribArray
CI_API void	enableVertexAttribArray( GLuint index );

CI_API void			vertexAttrib1f( GLuint index, float v0 );
CI_API inline void	vertexAttrib( GLuint index, float v0 ) { vertexAttrib1f( index, v0 ); }
CI_API void			vertexAttrib2f( GLuint index, float v0, float v1 );
CI_API inline void	vertexAttrib( GLuint index, float v0, float v1 ) { vertexAttrib2f( index, v0, v1 ); }
CI_API void			vertexAttrib3f( GLuint index, float v0, float v1, float v2 );
CI_API inline void	vertexAttrib( GLuint index, float v0, float v1, float v2 ) { vertexAttrib3f( index, v0, v1, v2 ); }
CI_API inline void	vertexAttrib( GLuint index, float v0, float v1 );
CI_API void			vertexAttrib4f( GLuint index, float v0, float v1, float v2, float v3 );
CI_API inline void	vertexAttrib( GLuint index, float v0, float v1, float v2, float v3 ) { vertexAttrib4f( index, v0, v1, v2, v3 ); }

// Buffers
CI_API void	bindBuffer( const BufferObjRef &buffer );
//! Binds a named buffer object \a buffer to \a target. Analogous to glBindBuffer().
CI_API void	bindBuffer( GLenum target, GLuint buffer );
#if ! defined( CINDER_GL_ES_2 )
//! Specifies a color buffer as the source for subsequent glReadPixels(), glCopyTexImage2D(), glCopyTexSubImage2D(), and glCopyTexSubImage3D() commands. Analogous to glReadBuffer().
CI_API void	readBuffer( GLenum src );
//! Specifies an array of buffers into which fragment color values or fragment data will be written for subsequent draw calls. Analogous to glDrawBuffers().
CI_API void	drawBuffers( GLsizei num, const GLenum *bufs );
//! Specifies a color buffer as the destination for subsequent draw calls. Analogous to glDrawBuffer(), and emulated on ES 3
CI_API void	drawBuffer( GLenum dst );
#endif

//! Reads a block of pixels from the framebuffer. Analogous to glReadPixels().
CI_API void	readPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *data );

// Compute
#if defined( CINDER_GL_HAS_COMPUTE_SHADER )
//! Launches one or more compute work groups. Analogous to glDispatchCompute(). 
CI_API inline void	dispatchCompute( GLuint numGroupsX, GLuint numGroupsY = 1, GLuint numGroupsZ = 1 ) { glDispatchCompute( numGroupsX, numGroupsY, numGroupsZ ); }
//! Defines a barrier ordering memory transactions. Analogous to glMemoryBarrier().
CI_API inline void	memoryBarrier( GLbitfield barriers ) { glMemoryBarrier( barriers ); }

//! Returns ivec3( GL_MAX_COMPUTE_WORK_GROUP_COUNT )
CI_API ivec3	getMaxComputeWorkGroupCount();
//! Returns ivec3( GL_MAX_COMPUTE_WORK_GROUP_SIZE )
CI_API ivec3	getMaxComputeWorkGroupSize();
#endif

// Debug
#if defined( CINDER_GL_HAS_KHR_DEBUG )
//! Pushes a named debug group into the command stream
CI_API void pushDebugGroup( const std::string &message );
//! Pushes a named debug group into the command stream with an id
CI_API void pushDebugGroup( GLuint id, const std::string &message );
//! Pushes a named debug group into the command stream with an id and a message source
CI_API void pushDebugGroup( GLenum source, GLuint id, const std::string &message );
//! Pops the active debug group
CI_API void popDebugGroup();
#endif

class CI_API Exception : public cinder::Exception {
  public:
	Exception()	{}
	Exception( const std::string &description ) : cinder::Exception( description )	{}
};

class CI_API ExceptionUnknownTarget : public Exception {
};

} } // namespace cinder::gl

