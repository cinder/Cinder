/*
 Copyright (c) 2013, The Cinder Project, All rights reserved.
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

#include "cinder/Cinder.h"

#if defined( CINDER_GL_ANGLE )
	#define GL_GLEXT_PROTOTYPES
	#define CINDER_GL_ES
	// the default for ANGLE is GL ES 3, but can be overridden with CINDER_GL_ES_2
	#if defined( CINDER_GL_ES_2 )
		#include "GLES2/gl2.h"
		#include "GLES2/gl2ext.h"
	#else
		#include "GLES3/gl3.h"
		#include "GLES3/gl3ext.h"
		#include "GLES2/gl2ext.h"
		#define CINDER_GL_ES_3
	#endif
	#pragma comment( lib, "libEGL.lib" )
	#pragma comment( lib, "libGLESv2.lib" )
#elif ! defined( CINDER_COCOA_TOUCH ) // OS X
	#if defined( __clang__ )
		#pragma clang diagnostic push
		#pragma clang diagnostic ignored "-Wtypedef-redefinition"
	#endif
	#include "glload/gl_core.h"
	#if defined( __clang__ )
		#pragma clang diagnostic pop
	#endif
#else // iOS
	#define CINDER_GL_ES
	// the default for iOS is GL ES 2, but can be overridden with CINDER_GL_ES_3
	#if ! defined( CINDER_GL_ES_3 )
		#include <OpenGLES/ES2/gl.h>
		#include <OpenGLES/ES2/glext.h>
		#define CINDER_GL_ES_2
	#else
		#include <OpenGLES/ES3/gl.h>
		#include <OpenGLES/ES3/glext.h>
	#endif
#endif
#include <boost/noncopyable.hpp>

#include "cinder/Area.h"
#include "cinder/Rect.h"
#include "cinder/AxisAlignedBox.h"
#include "cinder/Exception.h"
#include "cinder/Color.h"
#include "cinder/Camera.h"
#include "cinder/Matrix44.h"
#include "cinder/GeomIo.h"
#include "cinder/PolyLine.h"
#include "cinder/Font.h"

#if ! defined( NDEBUG )
	#define CI_CHECK_GL()	cinder::gl::checkError()
#else
	#define CI_CHECK_GL()	((void)0)
#endif

// forward declarations
namespace cinder {
	class TriMesh;
	class Path2d;
	class Shape2d;
	class Font;
}

namespace cinder { namespace gl {

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

typedef std::shared_ptr<class Vbo>				VboRef;
typedef std::shared_ptr<class VboMesh>			VboMeshRef;
typedef std::shared_ptr<class TextureBase>		TextureBaseRef;
typedef std::shared_ptr<class Texture2d>		Texture2dRef;
typedef std::shared_ptr<class Texture3d>		Texture3dRef;
typedef std::shared_ptr<class TextureCubeMap>	TextureCubeMapRef;
typedef std::shared_ptr<class BufferObj>		BufferObjRef;
typedef std::shared_ptr<class GlslProg>			GlslProgRef;
typedef std::shared_ptr<class Vao>				VaoRef;
typedef std::shared_ptr<class Fbo>				FboRef;
typedef std::shared_ptr<class Renderbuffer>		RenderbufferRef;

class Context* context();
class Environment* env();

void enableVerticalSync( bool enable = true );
bool isVerticalSyncEnabled();
//! Returns whether OpenGL Extension \a extName is implemented on the hardware. For example, \c "GL_EXT_texture_swizzle". Case insensitive.
bool isExtensionAvailable( const std::string &extName );
//! Returns the OpenGL version number as a pair<major,minor>
std::pair<GLint,GLint>	getVersion();
std::string getVersionString();

GlslProgRef	getStockShader( const class ShaderDef &shader );
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
//! Specifies the rasterized diameter of points. If point size mode is disabled (via gl::disable \c GL_PROGRAM_POINT_SIZE), this value will be used to rasterize points. Otherwise, the value written to the shading language built-in variable \c gl_PointSize will be used. Analogous to glPointSize.
void	pointSize( float size );
#endif

//! Converts a geom::Primitive to an OpenGL primitive mode( GL_TRIANGLES, GL_TRIANGLE_STRIP, etc )
GLenum toGl( geom::Primitive prim );
//! Converts an OpenGL primitive mode( GL_TRIANGLES, GL_TRIANGLE_STRIP, etc ) to a geom::Primitive
geom::Primitive toGeomPrimitive( GLenum prim );
//! Converts a UniformSemantic to its name
std::string uniformSemanticToString( UniformSemantic uniformSemantic );

void draw( const VboMeshRef &mesh );
void draw( const Texture2dRef &texture, const Rectf &dstRect );
//! Draws a subregion \a srcArea of a Texture (expressed as upper-left origin pixels).
void draw( const Texture2dRef &texture, const Area &srcArea, const Rectf &dstRect );
void draw( const Texture2dRef &texture, const vec2 &dstOffset = vec2() );
void draw( const PolyLine2 &polyLine );
void draw( const PolyLine3 &polyLine );
//! Draws a Path2d \a pathd using approximation scale \a approximationScale. 1.0 corresponds to screenspace, 2.0 is double screen resolution, etc
void draw( const Path2d &path, float approximationScale = 1.0f );
//! Draws a Shaped2d \a shaped using approximation scale \a approximationScale. 1.0 corresponds to screenspace, 2.0 is double screen resolution, etc
void draw( const Shape2d &shape, float approximationScale = 1.0f );
//! Draws a TriMesh \a mesh at the origin. Currently only uses position and index information.
void draw( const TriMesh &mesh );
//! Draws a geom::Source \a source at the origin.
void draw( const geom::Source &source );

//! Draws a solid (filled) Path2d \a path using approximation scale \a approximationScale. 1.0 corresponds to screenspace, 2.0 is double screen resolution, etc. Performance warning: This routine tesselates the polygon into triangles. Consider using Triangulator directly.
void drawSolid( const Path2d &path2d, float approximationScale = 1.0f );
//! Draws a solid (filled) Shape2d \a shape using approximation scale \a approximationScale. 1.0 corresponds to screenspace, 2.0 is double screen resolution, etc. Performance warning: This routine tesselates the polygon into triangles. Consider using Triangulator directly.
void drawSolid( const Shape2d &shape, float approximationScale = 1.0f );
void drawSolid( const PolyLine2 &polyLine );

//! Renders a solid cube centered at \a center of size \a size. Normals and created texture coordinates are generated.
void drawCube( const vec3 &center, const vec3 &size );
//! Renders a solid cube centered at \a center of size \a size. Each face is assigned a unique color.
void drawColorCube( const vec3 &center, const vec3 &size );
//! Renders a stroked cube centered at \a center of size \a size.
void drawStrokedCube( const vec3 &center, const vec3 &size );
//! Renders a stroked cube using \a box as the guide for center and size.
inline void drawStrokedCube( const ci::AxisAlignedBox3f &box ) { drawStrokedCube( box.getCenter(), box.getSize() ); }
//! Renders a solid sphere at \a center of radius \a radius, subdivided on both longitude and latitude into \a subdivisions.
void drawSphere( const vec3 &center, float radius, int subdivisions );
//! Draws a textured quad of size \a scale that is aligned with the vectors \a bbRight and \a bbUp at \a pos, rotated by \a rotationRadians around the vector orthogonal to \a bbRight and \a bbUp.
void drawBillboard( const vec3 &pos, const vec2 &scale, float rotationRadians, const vec3 &bbRight, const vec3 &bbUp, const Rectf &texCoords = Rectf( 0, 0, 1, 1 ) );
//! Renders a stroked representation of \a cam
void drawFrustum( const Camera &cam );
void drawCoordinateFrame( float axisLength = 1.0f, float headLength = 0.2f, float headRadius = 0.05f );
//! Draws a vector starting at \a start and ending at \a end. An arrowhead is drawn at the end of radius \a headRadius and length \a headLength.
void drawVector( const vec3 &start, const vec3 &end, float headLength = 0.2f, float headRadius = 0.05f );

//! Draws a line between points a and b
void drawLine( const vec3 &a, const vec3 &b );
void drawLine( const vec2 &a, const vec2 &b );

//! Draws \a texture on the XY-plane
void drawSolidRect( const Rectf &r, const vec2 &upperLeftTexCoord = vec2( 0, 1 ), const vec2 &lowerRightTexCoord = vec2( 1, 0 ) );
void drawSolidCircle( const vec2 &center, float radius, int numSegments = -1 );

//! Draws a stroked rectangle with dimensions \a rect.
void drawStrokedRect( const Rectf &rect );
//! Draws a stroked rectangle centered around \a rect, with a line width of \a lineWidth
void drawStrokedRect( const Rectf &rect, float lineWidth );
//! Draws a stroked circles centered around \a center with a radius of \a radius
void drawStrokedCircle( const vec2 &center, float radius, int numSegments = -1 );

//! Draws a string \a str with its lower left corner located at \a pos. Optional \a font and \a color affect the style.
void drawString( const std::string &str, const vec2 &pos, const ColorA &color = ColorA( 1, 1, 1, 1 ), Font font = Font() );
//! Draws a string \a str with the horizontal center of its baseline located at \a pos. Optional \a font and \a color affect the style
void drawStringCentered( const std::string &str, const vec2 &pos, const ColorA &color = ColorA( 1, 1, 1, 1 ), Font font = Font() );
//! Draws a right-justified string \a str with the center of its  located at \a pos. Optional \a font and \a color affect the style
void drawStringRight( const std::string &str, const vec2 &pos, const ColorA &color = ColorA( 1, 1, 1, 1 ), Font font = Font() );

//! Renders a solid triangle.
void drawSolidTriangle( const vec2 &pt0, const vec2 &pt1, const vec2 &pt2 );
//! Renders a textured triangle.
void drawSolidTriangle( const vec2 &pt0, const vec2 &pt1, const vec2 &pt2, const vec2 &texPt0, const vec2 &texPt1, const vec2 &texPt2 );
//! Renders a textured triangle.
void drawSolidTriangle( const vec2 pts[3], const vec2 texCoord[3] = nullptr );

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
	
GLenum getError();
std::string getErrorString( GLenum err );
void checkError();


struct ScopedVao : public boost::noncopyable {
	ScopedVao( const VaoRef &vao );
	~ScopedVao();

  private:
	Context		*mCtx;
};

struct ScopedBuffer : public boost::noncopyable {
	ScopedBuffer( const BufferObjRef &bufferObj );
	ScopedBuffer( GLenum target, GLuint id );
	~ScopedBuffer();
	
  private:
	Context		*mCtx;
	GLenum		mTarget;
};

struct ScopedState : public boost::noncopyable {
	ScopedState( GLenum cap, GLboolean value );
	~ScopedState();

  private:
	Context		*mCtx;
	GLenum		mCap;
};

struct ScopedColor : public boost::noncopyable {
	ScopedColor();
	ScopedColor( const ColorAf &color );
	~ScopedColor();

  private:
	Context		*mCtx;
	ColorAf		mColor;
};

struct ScopedBlend : public boost::noncopyable
{
	ScopedBlend( GLboolean enable );
	//! Parallels glBlendFunc(), and implicitly enables blending
	ScopedBlend( GLenum sfactor, GLenum dfactor );
	//! Parallels glBlendFuncSeparate(), and implicitly enables blending
	ScopedBlend( GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha );
	~ScopedBlend();
	
  private:
	Context		*mCtx;
	bool		mSaveFactors; // whether we should also set the blend factors rather than just the blend state
};

struct ScopedAlphaBlend : public ScopedBlend
{
	ScopedAlphaBlend( bool premultipliedAlpha )
		: ScopedBlend( premultipliedAlpha ? GL_ONE : GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA )
	{}
};

struct ScopedAdditiveBlend : public ScopedBlend
{
	ScopedAdditiveBlend()
		: ScopedBlend( GL_SRC_ALPHA, GL_ONE )
	{}
};

struct ScopedGlslProg : public boost::noncopyable
{
	ScopedGlslProg( const GlslProgRef &prog );
	ScopedGlslProg( const std::shared_ptr<const GlslProg> &prog );
	~ScopedGlslProg();

  private:
	Context		*mCtx;
};

struct ScopedFramebuffer : public boost::noncopyable
{
	ScopedFramebuffer( const FboRef &fbo, GLenum target = GL_FRAMEBUFFER );
	//! Prefer the FboRef variant when possible. This does not allow gl::Fbo to mark itself as needing multisample resolution.
	ScopedFramebuffer( GLenum target, GLuint framebufferId );
	~ScopedFramebuffer();
	
  private:
	Context		*mCtx;
	GLenum		mTarget;
};

struct ScopedActiveTexture : public boost::noncopyable
{
	//! Sets the currently active texture through glActiveTexture. Expects values relative to \c 0, \em not GL_TEXTURE0
	ScopedActiveTexture( uint8_t textureUnit );
	~ScopedActiveTexture();
	
  private:
	Context		*mCtx;
};

struct ScopedTextureBind : public boost::noncopyable
{
	ScopedTextureBind( GLenum target, GLuint textureId );
	ScopedTextureBind( GLenum target, GLuint textureId, uint8_t textureUnit );
	ScopedTextureBind( const TextureBaseRef &texture );
	ScopedTextureBind( const TextureBaseRef &texture, uint8_t textureUnit );

	//! \cond
	// These overloads are to alleviate a VS2013 bug where it cannot deduce
	// the correct constructor when a TextureBaseRef subclass is passed in
	ScopedTextureBind( const Texture2dRef &texture, uint8_t textureUnit );
#if ! defined( CINDER_GL_ES_2 )
	ScopedTextureBind( const Texture3dRef &texture, uint8_t textureUnit );
#endif // ! defined( CINDER_GL_ES_2 )
	ScopedTextureBind( const TextureCubeMapRef &texture, uint8_t textureUnit );
	//! \endcond

	~ScopedTextureBind();
	
  private:
	Context		*mCtx;
	GLenum		mTarget;
	uint8_t		mTextureUnit;
};
	
struct ScopedScissor : public boost::noncopyable
{
	//! Implicitly enables scissor test
	ScopedScissor( const ivec2 &lowerLeftPosition, const ivec2 &dimension );
	//! Implicitly enables scissor test	
	ScopedScissor( int lowerLeftX, int lowerLeftY, int width, int height );
	~ScopedScissor();

  private:
	Context					*mCtx;
};

struct ScopedViewport : public boost::noncopyable
{
	ScopedViewport( const ivec2 &lowerLeftPosition, const ivec2 &dimension );
	ScopedViewport( int lowerLeftX, int lowerLeftY, int width, int height );
	~ScopedViewport();

  private:
	Context					*mCtx;
};

struct ScopedModelMatrix : public boost::noncopyable {
	ScopedModelMatrix()	{ gl::pushModelMatrix(); }
	~ScopedModelMatrix()	{ gl::popModelMatrix(); }
};

struct ScopedProjectionMatrix : public boost::noncopyable {
	ScopedProjectionMatrix()			{ gl::pushProjectionMatrix(); }
	~ScopedProjectionMatrix()	{ gl::popProjectionMatrix(); }
};

//! Preserves all
struct ScopedMatrices : public boost::noncopyable {
	ScopedMatrices()		{ gl::pushMatrices(); }
	~ScopedMatrices()	{ gl::popMatrices(); }
};

//! Scopes state of face culling.
struct ScopedFaceCulling : public boost::noncopyable
{
	//! Enables or disables polygon culling based on \a cull
	ScopedFaceCulling( bool cull );
	//! Enables or disables polygon culling based on \a cull and specifies a mode, either \c GL_BACK or GL_FRONT
	ScopedFaceCulling( bool cull, GLenum cullFace );
	~ScopedFaceCulling();
	
  private:
	Context		*mCtx;
	bool		mSaveFace;
};

//! Scopes state of Renderbuffer binding
struct ScopedRenderbuffer : public boost::noncopyable
{
	ScopedRenderbuffer( const RenderbufferRef &rb );
	ScopedRenderbuffer( GLenum target, GLuint id );
	~ScopedRenderbuffer();
	
  private:
	Context		*mCtx;
};

class Exception : public cinder::Exception {
  public:
	Exception()	{}
	Exception( const std::string &description ) : cinder::Exception( description )	{}
};

class ExceptionUnknownTarget : public Exception {
};

} }
