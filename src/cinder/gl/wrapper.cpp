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

#include "cinder/gl/wrapper.h"
#include "cinder/gl/Environment.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/scoped.h"
#include "cinder/Log.h"

#if defined( CINDER_MSW )
	#include "glload/wgl_all.h"
#elif defined( CINDER_MAC )
	#include <OpenGL/OpenGL.h>
#elif defined( CINDER_LINUX )
	#if ! defined( CINDER_LINUX_EGL_ONLY )
		#include "glfw/glfw3.h"
	#endif
#endif

using namespace std;

namespace cinder { namespace gl {

Context* context()
{
	return Context::getCurrent();
}

void enableVerticalSync( bool enable )
{
#if defined( CINDER_MAC )
	GLint sync = ( enable ) ? 1 : 0;
	::CGLSetParameter( ::CGLGetCurrentContext(), kCGLCPSwapInterval, &sync );
#elif defined( CINDER_MSW ) && ! defined( CINDER_GL_ANGLE )
	GLint sync = ( enable ) ? 1 : 0;
	if( wglext_EXT_swap_control )
		::wglSwapIntervalEXT( sync );
#elif defined( CINDER_LINUX )
	#if ! defined( CINDER_LINUX_EGL_ONLY )
		GLint sync = ( enable ) ? 1 : 0;
		glfwSwapInterval( sync );
	#endif
#endif
}

bool isVerticalSyncEnabled()
{
#if defined( CINDER_MAC )
	GLint enabled;
	::CGLGetParameter( ::CGLGetCurrentContext(), kCGLCPSwapInterval, &enabled );
	return enabled > 0;
#elif defined( CINDER_MSW ) && ! defined( CINDER_GL_ANGLE )
	if( wglext_EXT_swap_control )
		return ::wglGetSwapIntervalEXT() > 0;
	else
		return true;
#else
	return true;
#endif
}

GLenum getError()
{
	return glGetError();
}

std::string getErrorString( GLenum err )
{
	switch( err ) {
		case GL_NO_ERROR:
			return "GL_NO_ERROR";
		case GL_INVALID_ENUM:
			return "GL_INVALID_ENUM";
		case GL_INVALID_VALUE:
			return "GL_INVALID_VALUE";
		case GL_INVALID_OPERATION:
			return "GL_INVALID_OPERATION";
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			return "GL_INVALID_FRAMEBUFFER_OPERATION";
		case GL_OUT_OF_MEMORY:
			return "GL_OUT_OF_MEMORY";
		default:
			return "";
	}
}

void checkError()
{
	GLenum errorFlag = getError();
	if( errorFlag != GL_NO_ERROR ) {
		CI_LOG_E( "glGetError flag set: " << getErrorString( errorFlag ) );
		CI_ASSERT( 0 );
	}
}

bool isExtensionAvailable( const std::string &extName )
{
	return env()->isExtensionAvailable( extName );
}

std::pair<GLint,GLint> getVersion()
{
	//hard-coded for now
#if defined( CINDER_GL_ES_2 )
	return std::make_pair( (GLint)2, (GLint)0 );
#elif defined( CINDER_GL_ES_3 )
	return std::make_pair( (GLint)3, (GLint)0 );
#else
	static bool	sInitialized = false;
	static pair<GLint,GLint> sVersion;
	if( ! sInitialized ) {
		// adapted from LoadOGL
		const char *strVersion = reinterpret_cast<const char*>( glGetString( GL_VERSION ) );
		GLint major = 0, minor = 0;
		const char *strDotPos = NULL;
		int iLength = 0;
		char strWorkBuff[10];

		strDotPos = strchr( strVersion, '.' );
		if( ! strDotPos )
			return std::make_pair( 0, 0 );

		iLength = (int)((ptrdiff_t)strDotPos - (ptrdiff_t)strVersion);
		strncpy(strWorkBuff, strVersion, iLength);
		strWorkBuff[iLength] = '\0';

		major = atoi(strWorkBuff);
		strDotPos = strchr( strVersion + iLength + 1, ' ' );
		if( ! strDotPos ) { // No extra data. Take the whole rest of the string.
			strcpy( strWorkBuff, strVersion + iLength + 1 );
		}
		else {
			// Copy only up until the space.
			int iLengthMinor = (int)((ptrdiff_t)strDotPos - (ptrdiff_t)strVersion);
			iLengthMinor = iLengthMinor - (iLength + 1);
			strncpy( strWorkBuff, strVersion + iLength + 1, iLengthMinor );
			strWorkBuff[iLengthMinor] = '\0';
		}

		minor = atoi( strWorkBuff );
		sVersion = std::make_pair( major, minor );
		sInitialized = true;
	}

	return sVersion;
#endif
}

std::string getVersionString()
{
	const GLubyte* s = glGetString( GL_VERSION );

	return std::string( reinterpret_cast<const char*>( s ) );
}

GlslProgRef& getStockShader( const class ShaderDef &shader )
{
	return context()->getStockShader( shader );
}

void bindStockShader( const class ShaderDef &shaderDef )
{
	auto ctx = gl::context();
	auto shader = ctx->getStockShader( shaderDef );
	ctx->bindGlslProg( shader );
}

void setDefaultShaderVars()
{
	auto ctx = gl::context();
	ctx->setDefaultShaderVars();
}

void clear( const ColorA& color, bool clearDepthBuffer )
{
	clearColor( color );
	if ( clearDepthBuffer ) {
		ScopedDepthWrite depthWriteScp( GL_TRUE );
		clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	}
	else {
		clear( GL_COLOR_BUFFER_BIT );
	}
}

void clear( GLbitfield mask )
{
    glClear( mask );
}

void clearColor( const ColorA &color )
{
    glClearColor( color.r, color.g, color.b, color.a );
}

void clearDepth( const double depth )
{
#if ! defined( CINDER_GL_ES )
    glClearDepth( depth );
#else
	glClearDepthf( depth );
#endif
}

void clearDepth( const float depth )
{
    glClearDepthf( depth );
}

void clearStencil( const int s )
{
    glClearStencil( s );
}

void colorMask( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha )
{
    glColorMask( red, green, blue, alpha );
}

void depthMask( GLboolean flag )
{
    auto ctx = gl::context();
	ctx->depthMask( flag );
}

void stencilFunc( GLenum func, GLint ref, GLuint mask )
{
    glStencilFunc( func, ref, mask );
}

void stencilOp( GLenum fail, GLenum zfail, GLenum zpass )
{
    glStencilOp( fail, zfail, zpass );
}

void stencilMask( GLuint mask )
{
	glStencilMask( mask );
}

std::pair<ivec2, ivec2> getViewport()
{
	auto ctx = gl::context();
	auto view = ctx->getViewport();
	return view;
}

void viewport( const std::pair<ivec2, ivec2> positionAndSize )
{
	auto ctx = gl::context();
	ctx->viewport( positionAndSize );
}

void pushViewport( const std::pair<ivec2, ivec2> positionAndSize )
{
	auto ctx = gl::context();
	ctx->pushViewport( positionAndSize );
}

void popViewport()
{
	auto ctx = gl::context();
	ctx->popViewport();
}

std::pair<ivec2, ivec2> getScissor()
{
	auto ctx = gl::context();
	auto scissor = ctx->getScissor();
	return scissor;
}

void scissor( const std::pair<ivec2, ivec2> positionAndSize )
{
	auto ctx = gl::context();
	ctx->setScissor( positionAndSize );
}

void enable( GLenum state, bool enable )
{
	auto ctx = gl::context();
	ctx->enable( state, enable );
}

void enableBlending( bool enable )
{
	auto ctx = gl::context();
	ctx->enable( GL_BLEND, enable );
}

void enableAlphaBlending( bool enable )
{
	auto ctx = gl::context();
	ctx->enable( GL_BLEND );
	if( enable )
		ctx->blendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}

void enableAlphaBlendingPremult()
{
	auto ctx = gl::context();
	ctx->enable( GL_BLEND );
	ctx->blendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
}

void enableAdditiveBlending()
{
	auto ctx = gl::context();
	ctx->enable( GL_BLEND );
	ctx->blendFunc( GL_SRC_ALPHA, GL_ONE );
}

void enableFaceCulling( bool enable )
{
	gl::context()->setBoolState( GL_CULL_FACE, enable );
}

void cullFace( GLenum face )
{
	gl::context()->cullFace( face );
}

#if ! defined( CINDER_GL_ES )
void enableLogicOp( bool enable )
{
	gl::context()->setBoolState( GL_COLOR_LOGIC_OP, enable );
}

void logicOp( GLenum mode )
{
	gl::context()->logicOp( mode );
}
#endif

void disableDepthRead()
{
	gl::disable( GL_DEPTH_TEST );
}

void enableDepthRead( bool enable )
{
	gl::enable( GL_DEPTH_TEST, enable );
}

void enableDepthWrite( bool enable )
{
	auto ctx = gl::context();
	ctx->depthMask( enable ? GL_TRUE : GL_FALSE );
}

void disableDepthWrite()
{
	auto ctx = gl::context();
	ctx->depthMask( GL_FALSE );
}

void enableStencilTest( bool enable )
{
    gl::enable( GL_STENCIL_TEST, enable );
}

void disableStencilTest()
{
    gl::disable( GL_STENCIL_TEST );
}

void setMatrices( const ci::Camera& cam )
{
	auto ctx = context();
	ctx->getViewMatrixStack().back() = cam.getViewMatrix();
	ctx->getProjectionMatrixStack().back() = cam.getProjectionMatrix();
	ctx->getModelMatrixStack().back() = mat4();
}

void setModelMatrix( const ci::mat4 &m )
{
	auto ctx = context();
	ctx->getModelMatrixStack().back() = m;
}

void setViewMatrix( const ci::mat4 &m )
{
	auto ctx = context();
	ctx->getViewMatrixStack().back() = m;
}

void setProjectionMatrix( const ci::mat4 &m )
{
	auto ctx = context();
	ctx->getProjectionMatrixStack().back() = m;
}

void pushModelMatrix()
{
	auto ctx = context();
	ctx->getModelMatrixStack().push_back( ctx->getModelMatrixStack().back() );
}

void popModelMatrix()
{
	auto ctx = context();
	ctx->getModelMatrixStack().pop_back();
}

void pushViewMatrix()
{
	auto ctx = context();
	ctx->getViewMatrixStack().push_back( ctx->getViewMatrixStack().back() );
}

void popViewMatrix()
{
	auto ctx = context();
	ctx->getViewMatrixStack().pop_back();
}

void pushProjectionMatrix()
{
	auto ctx = context();
	ctx->getProjectionMatrixStack().push_back( ctx->getProjectionMatrixStack().back() );
}

void popProjectionMatrix()
{
	auto ctx = context();
	ctx->getProjectionMatrixStack().pop_back();
}

void pushModelView()
{
	auto ctx = context();
	ctx->getModelMatrixStack().push_back( ctx->getModelMatrixStack().back() );
	ctx->getViewMatrixStack().push_back( ctx->getViewMatrixStack().back() );
}

void popModelView()
{
	auto ctx = context();
	ctx->getModelMatrixStack().pop_back();
	ctx->getViewMatrixStack().pop_back();
}

void pushMatrices()
{
	auto ctx = context();
	ctx->getModelMatrixStack().push_back( ctx->getModelMatrixStack().back() );
	ctx->getViewMatrixStack().push_back( ctx->getViewMatrixStack().back() );
	ctx->getProjectionMatrixStack().push_back( ctx->getProjectionMatrixStack().back() );
}

void popMatrices()
{
	auto ctx = context();
	ctx->getModelMatrixStack().pop_back();
	ctx->getViewMatrixStack().pop_back();
	ctx->getProjectionMatrixStack().pop_back();
}

void multModelMatrix( const ci::mat4& mtx )
{
	auto ctx = gl::context();
	ctx->getModelMatrixStack().back() *= mtx;
}

void multViewMatrix( const ci::mat4& mtx )
{
	auto ctx = gl::context();
	ctx->getViewMatrixStack().back() *= mtx;
}

void multProjectionMatrix( const ci::mat4& mtx )
{
	auto ctx = gl::context();
	ctx->getProjectionMatrixStack().back() *= mtx;
}

mat4 getModelMatrix()
{
	auto ctx = gl::context();
	return ctx->getModelMatrixStack().back();
}

mat4 getViewMatrix()
{
	auto ctx = gl::context();
	return ctx->getViewMatrixStack().back();
}

mat4 getProjectionMatrix()
{
	auto ctx = gl::context();
	return ctx->getProjectionMatrixStack().back();
}

mat4 getModelView()
{
	auto ctx = context();
	return ctx->getViewMatrixStack().back() * ctx->getModelMatrixStack().back();
}

mat4 getModelViewProjection()
{
	auto ctx = context();
	return ctx->getProjectionMatrixStack().back() * ctx->getViewMatrixStack().back() * ctx->getModelMatrixStack().back();
}

mat4 calcViewMatrixInverse()
{
	return glm::inverse( getViewMatrix() );
}

mat3 calcNormalMatrix()
{
	return glm::inverseTranspose( glm::mat3( getModelView() ) );
}
	
mat3 calcModelMatrixInverseTranspose()
{
	auto m = glm::inverseTranspose( getModelMatrix() );
	return mat3( m );
}
	
mat4 calcViewportMatrix()
{
	auto curViewport = getViewport();
	
	const float a = ( curViewport.second.x - curViewport.first.x ) / 2.0f;
	const float b = ( curViewport.second.y - curViewport.first.y ) / 2.0f;
	const float c = 1.0f / 2.0f;
	
	const float tx = ( curViewport.second.x + curViewport.first.x ) / 2.0f;
	const float ty = ( curViewport.second.y + curViewport.second.y ) / 2.0f;
	const float tz = 1.0f / 2.0f;
	
	return mat4(
		a, 0, 0, 0,
		0, b, 0, 0,
		0, 0, c, 0,
		tx, ty, tz, 1
	);
}

void setMatricesWindowPersp( int screenWidth, int screenHeight, float fovDegrees, float nearPlane, float farPlane, bool originUpperLeft )
{
	auto ctx = gl::context();

	CameraPersp cam( screenWidth, screenHeight, fovDegrees, nearPlane, farPlane );
	ctx->getModelMatrixStack().back() = mat4();
	ctx->getProjectionMatrixStack().back() = cam.getProjectionMatrix();
	ctx->getViewMatrixStack().back() = cam.getViewMatrix();
	if( originUpperLeft ) {
		ctx->getViewMatrixStack().back() *= glm::scale( vec3( 1, -1, 1 ) );								// invert Y axis so increasing Y goes down.
		ctx->getViewMatrixStack().back() *= glm::translate( vec3( 0, (float) - screenHeight, 0 ) );		// shift origin up to upper-left corner.
	}
}

void setMatricesWindowPersp( const ci::ivec2& screenSize, float fovDegrees, float nearPlane, float farPlane, bool originUpperLeft )
{
	setMatricesWindowPersp( screenSize.x, screenSize.y, fovDegrees, nearPlane, farPlane, originUpperLeft );
}

void setMatricesWindow( int screenWidth, int screenHeight, bool originUpperLeft )
{
	auto ctx = gl::context();
	ctx->getModelMatrixStack().back() = mat4();
	ctx->getViewMatrixStack().back() = mat4();

	float sx = 2.0f / (float)screenWidth;
	float sy = 2.0f / (float)screenHeight;
	float ty = -1;

	if( originUpperLeft ) {
		sy *= -1;
		ty *= -1;
	}

	mat4 &m = ctx->getProjectionMatrixStack().back();
	m = mat4( sx, 0,  0, 0,
			  0, sy,  0, 0,
			  0,  0, -1, 0,
			 -1, ty,  0, 1 );
}

void setMatricesWindow( const ci::ivec2& screenSize, bool originUpperLeft )
{
	setMatricesWindow( screenSize.x, screenSize.y, originUpperLeft );
}

void rotate( const quat &quat )
{
	auto ctx = gl::context();
	ctx->getModelMatrixStack().back() *= toMat4( quat );
}

void rotate( float angleRadians, const vec3 &axis )
{
	if( math<float>::abs( angleRadians ) > EPSILON_VALUE ) {
		auto ctx = gl::context();
		ctx->getModelMatrixStack().back() *= glm::rotate( angleRadians, axis );
	}
}

void scale( const ci::vec3& v )
{
	auto ctx = gl::context();
	ctx->getModelMatrixStack().back() *= glm::scale( v );
}

void translate( const ci::vec3& v )
{
	auto ctx = gl::context();
	ctx->getModelMatrixStack().back() *= glm::translate( v );
}

vec3 windowToObjectCoord( const mat4 &modelMatrix, const ci::vec2 &coordinate, float z )
{
	// Build the viewport (x, y, width, height).
	vec2 offset = gl::getViewport().first;
	vec2 size = gl::getViewport().second;
	vec4 viewport = vec4( offset.x, offset.y, size.x, size.y );

	// Calculate the view-projection matrix.
	mat4 viewProjectionMatrix = gl::getProjectionMatrix() * gl::getViewMatrix();

	// Calculate the intersection of the mouse ray with the near (z=0) and far (z=1) planes.
	vec3 nearPlane = glm::unProject( vec3( coordinate.x, size.y - coordinate.y, 0 ), modelMatrix, viewProjectionMatrix, viewport );
	vec3 farPlane = glm::unProject( vec3( coordinate.x, size.y - coordinate.y, 1 ), modelMatrix, viewProjectionMatrix, viewport );

	// Calculate world position.
	return ci::lerp( nearPlane, farPlane, ( z - nearPlane.z ) / ( farPlane.z - nearPlane.z ) );
}

vec2 objectToWindowCoord( const mat4 &modelMatrix, const ci::vec3 &coordinate )
{
	// Build the viewport (x, y, width, height).
	vec2 offset = gl::getViewport().first;
	vec2 size = gl::getViewport().second;
	vec4 viewport = vec4( offset.x, offset.y, size.x, size.y );

	// Calculate the view-projection matrix.
	mat4 viewProjectionMatrix = gl::getProjectionMatrix() * gl::getViewMatrix();

	vec2 p = vec2( glm::project( coordinate, modelMatrix, viewProjectionMatrix, viewport ) );

	return p;
}

void begin( GLenum mode )
{
	auto ctx = gl::context();
	ctx->immediate().begin( mode );
}

void end()
{
	auto ctx = gl::context();

	if( ctx->immediate().empty() )
		return;
	else {
		const GlslProg* curGlslProg = ctx->getGlslProg();
		if( ! curGlslProg )
			ctx->pushGlslProg( ctx->getStockShader( ShaderDef().color() ).get() );

		ctx->immediate().draw();
		ctx->immediate().clear();

		if( ! curGlslProg )
			ctx->popGlslProg();
	}
}

#if ! defined( CINDER_GL_ES_2 )
void bindBufferBase( GLenum target, int index, BufferObjRef buffer )
{
	auto ctx = gl::context();
	ctx->bindBufferBase( target, index, buffer );
}
#endif // ! defined( CINDER_GL_ES_2 )

#if defined( CINDER_GL_HAS_TRANSFORM_FEEDBACK )
void beginTransformFeedback( GLenum primitiveMode )
{
	auto ctx = gl::context();
	ctx->beginTransformFeedback( primitiveMode );
}

void pauseTransformFeedback()
{
	auto ctx = gl::context();
	ctx->pauseTransformFeedback();
}

void resumeTransformFeedback()
{
	auto ctx = gl::context();
	ctx->resumeTransformFeedback();
}

void endTransformFeedback()
{
	auto ctx = gl::context();
	ctx->endTransformFeedback();
}
#endif // defined( CINDER_GL_HAS_TRANSFORM_FEEDBACK )

#if defined( CINDER_GL_HAS_TESS_SHADER )
void patchParameteri( GLenum pname, GLint value )
{
	glPatchParameteri( pname, value );
}

#if ! defined( CINDER_GL_ES )
void patchParameterfv( GLenum pname, GLfloat *value )
{
	glPatchParameterfv( pname, value );
}
#endif 
#endif // defined( CINDER_GL_HAS_TESS_SHADER )

void color( float r, float g, float b )
{
	auto ctx = gl::context();
	ctx->setCurrentColor( ColorAf( r, g, b, 1.0f ) );
}

void color( float r, float g, float b, float a )
{
	auto ctx = gl::context();
	ctx->setCurrentColor( ColorAf( r, g, b, a ) );
}

void color( const ci::Color &c )
{
	auto ctx = gl::context();
	ctx->setCurrentColor( c );
}

void color( const ci::ColorA &c )
{
	auto ctx = gl::context();
	ctx->setCurrentColor( c );
}

void color( const ci::Color8u &c )
{
	auto ctx = gl::context();
	ctx->setCurrentColor( c );
}

void color( const ci::ColorA8u &c )
{
	auto ctx = gl::context();
	ctx->setCurrentColor( c );
}

void texCoord( float s, float t )
{
	auto ctx = gl::context();
	ctx->immediate().texCoord( s, t );
}

void texCoord( float s, float t, float r )
{
	auto ctx = gl::context();
	ctx->immediate().texCoord( s, t, r );
}

void texCoord( float s, float t, float r, float q )
{
	auto ctx = gl::context();
	ctx->immediate().texCoord( s, t, r, q );
}

void texCoord( const ci::vec2 &v )
{
	auto ctx = gl::context();
	ctx->immediate().texCoord( v.x, v.y );
}

void texCoord( const ci::vec3 &v )
{
	auto ctx = gl::context();
	ctx->immediate().texCoord( v );
}

void texCoord( const ci::vec4 &v )
{
	auto ctx = gl::context();
	ctx->immediate().texCoord( v );
}

void vertex( float x, float y )
{
	auto ctx = gl::context();
	ctx->immediate().vertex( vec4( x, y, 0, 1 ), ctx->getCurrentColor() );
}

void vertex( float x, float y, float z )
{
	auto ctx = gl::context();
	ctx->immediate().vertex( vec4( x, y, z, 1 ), ctx->getCurrentColor() );
}

void vertex( float x, float y, float z, float w )
{
	auto ctx = gl::context();
	ctx->immediate().vertex( vec4( x, y, z, w ), ctx->getCurrentColor() );
}

void vertex( const ci::vec2 &v )
{
	auto ctx = gl::context();
	ctx->immediate().vertex( vec4( v.x, v.y, 0, 1 ), ctx->getCurrentColor() );
}

void vertex( const ci::vec3 &v )
{
	auto ctx = gl::context();
	ctx->immediate().vertex( vec4( v.x, v.y, v.z, 1 ), ctx->getCurrentColor() );
}

void vertex( const ci::vec4 &v )
{
	auto ctx = gl::context();
	ctx->immediate().vertex( v, ctx->getCurrentColor() );
}

#if ! defined( CINDER_GL_ES )
void polygonMode( GLenum face, GLenum mode )
{
	auto ctx = gl::context();
	ctx->polygonMode( face, mode );
}

void enableWireframe()
{
	gl::polygonMode( GL_FRONT_AND_BACK, GL_LINE );
}

void disableWireframe()
{
	gl::polygonMode( GL_FRONT_AND_BACK, GL_FILL );
}

bool isWireframeEnabled()
{
	auto ctx = gl::context();
	return ctx->getPolygonMode( GL_FRONT_AND_BACK ) == GL_LINE;
}

#endif // ! defined( CINDER_GL_ES )

void lineWidth( float width )
{
	gl::context()->lineWidth( width );
}

#if ! defined( CINDER_GL_ES )

void pointSize( float size )
{
	glPointSize( size );
}

#endif // ! defined( CINDER_GL_ES )

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Vertex Attributes
void vertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer )
{
	context()->vertexAttribPointer( index, size, type, normalized, stride, pointer );
}

#if ! defined( CINDER_GL_ES_2 )
void vertexAttribIPointer( GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer )
{
	context()->vertexAttribIPointer( index, size, type, stride, pointer );
}
#endif

void enableVertexAttribArray( GLuint index )
{
	context()->enableVertexAttribArray( index );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Buffers
void vertexAttrib1f( GLuint index, float v0 )
{
	context()->vertexAttrib1f( index, v0 );
}

void vertexAttrib2f( GLuint index, float v0, float v1 )
{
	context()->vertexAttrib2f( index, v0, v1 );
}

void vertexAttrib3f( GLuint index, float v0, float v1, float v2 )
{
	context()->vertexAttrib3f( index, v0, v1, v2 );
}

void vertexAttrib4f( GLuint index, float v0, float v1, float v2, float v3 )
{
	context()->vertexAttrib4f( index, v0, v1, v2, v3 );
}

void bindBuffer( const BufferObjRef &buffer )
{
	context()->bindBuffer( buffer->getTarget(), buffer->getId() );
}

void bindBuffer( GLenum target, GLuint buffer )
{
	context()->bindBuffer( target, buffer );
}

#if ! defined( CINDER_GL_ES_2 )
void readBuffer( GLenum src )
{
	glReadBuffer( src );
}

void drawBuffers( GLsizei num, const GLenum *bufs )
{
	glDrawBuffers( num, bufs );
}

void drawBuffer( GLenum dst )
{
#if ! defined( CINDER_GL_ES )
	glDrawBuffer( dst );
#else
	const GLenum bufs[] = { dst };
	glDrawBuffers( 1, bufs );
#endif
}
#endif // ! defined( CINDER_GL_ES_2 )

void readPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *data )
{
	glReadPixels( x, y, width, height, format, type, data );
}

// Compute
#if defined( CINDER_GL_HAS_COMPUTE_SHADER )
ivec3 getMaxComputeWorkGroupCount()
{
	ivec3 count;
	glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &count.x );
	glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &count.y );
	glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &count.z );
	return count;
}

ivec3 getMaxComputeWorkGroupSize()
{
	ivec3 size;
	glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &size.x );
	glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &size.y );
	glGetIntegeri_v( GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &size.z );
	return size;
}
#endif // defined( CINDER_GL_HAS_COMPUTE_SHADER )


//////////////////////////////////////////////////////////////////////////////////////////////////////////
// toGL conversion functions
GLenum toGl( geom::Primitive prim )
{
	switch( prim ) {
		case geom::Primitive::LINES:
			return GL_LINES;
		break;
		case geom::Primitive::LINE_STRIP:
			return GL_LINE_STRIP;
		break;
		case geom::Primitive::TRIANGLES:
			return GL_TRIANGLES;
		break;
		case geom::Primitive::TRIANGLE_STRIP:
			return GL_TRIANGLE_STRIP;
		break;
		case geom::Primitive::TRIANGLE_FAN:
			return GL_TRIANGLE_FAN;
		default:
			return 0; // no clear right choice here
	}
}

geom::Primitive toGeomPrimitive( GLenum prim )
{
	switch( prim ) {
		case GL_LINES:
			return geom::Primitive::LINES;
		break;
		case GL_LINE_STRIP:
			return geom::Primitive::LINE_STRIP;
		break;
		case GL_TRIANGLES:
			return geom::Primitive::TRIANGLES;
		break;
		case GL_TRIANGLE_STRIP:
			return geom::Primitive::TRIANGLE_STRIP;
		break;
		case GL_TRIANGLE_FAN:
			return geom::Primitive::TRIANGLE_FAN;
		default:
			return geom::Primitive( 65535 ); // no clear right choice here
	}
}

std::string uniformSemanticToString( UniformSemantic uniformSemantic )
{
	switch( uniformSemantic ) {
		case UNIFORM_MODEL_MATRIX: return "UNIFORM_MODEL_MATRIX";
		case UNIFORM_MODEL_MATRIX_INVERSE: return "UNIFORM_MODEL_MATRIX_INVERSE";
		case UNIFORM_MODEL_MATRIX_INVERSE_TRANSPOSE: return "UNIFORM_MODEL_MATRIX_INVERSE_TRANSPOSE";
		case UNIFORM_VIEW_MATRIX: return "UNIFORM_VIEW_MATRIX";
		case UNIFORM_VIEW_MATRIX_INVERSE: return "UNIFORM_VIEW_MATRIX_INVERSE";
		case UNIFORM_MODEL_VIEW: return "UNIFORM_MODEL_VIEW";
		case UNIFORM_MODEL_VIEW_INVERSE: return "UNIFORM_MODEL_VIEW_INVERSE";
		case UNIFORM_MODEL_VIEW_INVERSE_TRANSPOSE: return "UNIFORM_MODEL_VIEW_INVERSE_TRANSPOSE";
		case UNIFORM_MODEL_VIEW_PROJECTION: return "UNIFORM_MODEL_VIEW_PROJECTION";
		case UNIFORM_MODEL_VIEW_PROJECTION_INVERSE: return "UNIFORM_MODEL_VIEW_PROJECTION_INVERSE";
		case UNIFORM_PROJECTION_MATRIX: return "UNIFORM_PROJECTION_MATRIX";
		case UNIFORM_PROJECTION_MATRIX_INVERSE: return "UNIFORM_PROJECTION_MATRIX_INVERSE";
		case UNIFORM_VIEW_PROJECTION: return "UNIFORM_VIEW_PROJECTION";
		case UNIFORM_NORMAL_MATRIX: return "UNIFORM_NORMAL_MATRIX";
		case UNIFORM_VIEWPORT_MATRIX: return "UNIFORM_VIEWPORT_MATRIX";
		case UNIFORM_WINDOW_SIZE: return "UNIFORM_WINDOW_SIZE";
		case UNIFORM_ELAPSED_SECONDS: return "UNIFORM_ELAPSED_SECONDS";
		case UNIFORM_USER_DEFINED: return "UNIFORM_USER_DEFINED";
		default: return "";
	}
}

} } // namespace cinder::gl
