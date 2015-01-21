#include "cinder/gl/gl.h"
#include "cinder/gl/Context.h"
#include "cinder/gl/VboMesh.h"
#include "cinder/gl/Vao.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/Texture.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Environment.h"
#include "cinder/gl/Fbo.h"

#include "cinder/Utilities.h"
#include "cinder/Text.h"
#include "cinder/Triangulate.h"
#include "cinder/TriMesh.h"
#include "cinder/Path2d.h"
#include "cinder/Shape2d.h"
#include "cinder/Log.h"
#include <vector>

#if defined( CINDER_MSW )
	#include "glload/wgl_all.h"
#elif defined( CINDER_MAC )
	#include <OpenGL/OpenGL.h>
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

GlslProgRef	getStockShader( const class ShaderDef &shader )
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
		depthMask( GL_TRUE );
		clear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	} else {
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

void stencilMask( GLboolean mask )
{
	glStencilMask( mask );
}

void stencilFunc( GLenum func, GLint ref, GLuint mask )
{
    glStencilFunc( func, ref, mask );
}

void stencilOp( GLenum fail, GLenum zfail, GLenum zpass )
{
    glStencilOp( fail, zfail, zpass );
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

void enableAlphaBlending( bool premultiplied )
{
	auto ctx = gl::context();
	ctx->enable( GL_BLEND );
	if( ! premultiplied ) {
		ctx->blendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	}
	else {
		ctx->blendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
	}
}

void disableAlphaBlending()
{
	gl::disable( GL_BLEND );
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
		ScopedGlslProg ScopedGlslProg( ctx->getStockShader( ShaderDef().color() ) );
		ctx->immediate().draw();
		ctx->immediate().clear();
	}
}

#if ! defined( CINDER_GL_ES_2 )
void bindBufferBase( GLenum target, int index, BufferObjRef buffer )
{
	auto ctx = gl::context();
	ctx->bindBufferBase( target, index, buffer );
}

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
#endif // ! defined( CINDER_GL_ES_2 )

#if ! defined( CINDER_GL_ES )
void patchParameteri( GLenum pname, GLint value )
{
	glPatchParameteri( pname, value );
}

void patchParameterfv( GLenum pname, GLfloat *value )
{
	glPatchParameterfv( pname, value );
}
#endif // ! defined( CINDER_GL_ES )

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
	glLineWidth( width );
}

#if ! defined( CINDER_GL_ES )

void pointSize( float size )
{
	glPointSize( size );
}

#endif // ! defined( CINDER_GL_ES )

void draw( const VboMeshRef& mesh )
{
	auto ctx = gl::context();
	auto curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();
	mesh->buildVao( curGlslProg );
	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	mesh->drawImpl();
	ctx->popVao();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Vertex Attributes
void vertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer )
{
	context()->vertexAttribPointer( index, size, type, normalized, stride, pointer );
}

#if ! defined( CINDER_GL_ES )
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

#if ! defined( CINDER_GL_ES_2 )
void readBuffer( GLenum src )
{
	glReadBuffer( src );
}

void drawBuffers( GLsizei num, const GLenum *bufs )
{
	glDrawBuffers( num, bufs );
}
#endif

#if ! defined( CINDER_GL_ES )
void drawBuffer( GLenum dst )
{
	glDrawBuffer( dst );
}
#endif

void readPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *data )
{
	glReadPixels( x, y, width, height, format, type, data );
}

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
		case UNIFORM_NORMAL_MATRIX: return "UNIFORM_NORMAL_MATRIX";
		case UNIFORM_VIEWPORT_MATRIX: return "UNIFORM_VIEWPORT_MATRIX";
		case UNIFORM_WINDOW_SIZE: return "UNIFORM_WINDOW_SIZE";
		case UNIFORM_ELAPSED_SECONDS: return "UNIFORM_ELAPSED_SECONDS";
		default: return "";
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// Draw*
void drawArrays( GLenum mode, GLint first, GLsizei count )
{
	context()->drawArrays( mode, first, count );
}

void drawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices )
{
	context()->drawElements( mode, count, type, indices );
}

namespace {
std::array<vec3, 8> getCubePoints( const vec3 &c, const vec3 &size )
{
	vec3 s = size * 0.5f;																			// origin points
	std::array<vec3, 8> points = { vec3(  c.x + 1.0f * s.x, c.y + 1.0f * s.y, c.z + 1.0f * s.z ),	// upper front right
								   vec3(  c.x + 1.0f * s.x, c.y - 1.0f * s.y, c.z + 1.0f * s.z ),	// lower front right
								   vec3(  c.x + 1.0f * s.x, c.y - 1.0f * s.y, c.z - 1.0f * s.z ),	// lower back right
								   vec3(  c.x + 1.0f * s.x, c.y + 1.0f * s.y, c.z - 1.0f * s.z ),	// upper back right
								   vec3(  c.x - 1.0f * s.x, c.y + 1.0f * s.y, c.z + 1.0f * s.z ),	// upper front left
								   vec3(  c.x - 1.0f * s.x, c.y - 1.0f * s.y, c.z + 1.0f * s.z ),	// lower front left
								   vec3(  c.x - 1.0f * s.x, c.y - 1.0f * s.y, c.z - 1.0f * s.z ),	// lower back left
								   vec3(  c.x - 1.0f * s.x, c.y + 1.0f * s.y, c.z - 1.0f * s.z ) };	// upper back left
	return points;
}
	
void drawCubeImpl( const vec3 &c, const vec3 &size, bool faceColors )
{
	GLfloat sx = size.x * 0.5f;
	GLfloat sy = size.y * 0.5f;
	GLfloat sz = size.z * 0.5f;
	GLfloat vertices[24*3]={c.x+1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,		// +X
							c.x+1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,		// +Y
							c.x+1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,		// +Z
							c.x+-1.0f*sx,c.y+1.0f*sy,c.z+1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	// -X
							c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+1.0f*sz,	// -Y
							c.x+1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+-1.0f*sy,c.z+-1.0f*sz,	c.x+-1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz,	c.x+1.0f*sx,c.y+1.0f*sy,c.z+-1.0f*sz};	// -Z

	static GLfloat normals[24*3]={ 1,0,0,	1,0,0,	1,0,0,	1,0,0,
								  0,1,0,	0,1,0,	0,1,0,	0,1,0,
									0,0,1,	0,0,1,	0,0,1,	0,0,1,
								  -1,0,0,	-1,0,0,	-1,0,0,	-1,0,0,
								  0,-1,0,	0,-1,0,  0,-1,0,0,-1,0,
								  0,0,-1,	0,0,-1,	0,0,-1,	0,0,-1};

	static GLubyte colors[24*4]={	255,0,0,255,	255,0,0,255,	255,0,0,255,	255,0,0,255,	// +X = red
									0,255,0,255,	0,255,0,255,	0,255,0,255,	0,255,0,255,	// +Y = green
									0,0,255,255,	0,0,255,255,	0,0,255,255,	0,0,255,255,	// +Z = blue
									0,255,255,255,	0,255,255,255,	0,255,255,255,	0,255,255,255,	// -X = cyan
									255,0,255,255,	255,0,255,255,	255,0,255,255,	255,0,255,255,	// -Y = purple
									255,255,0,255,	255,255,0,255,	255,255,0,255,	255,255,0,255 };// -Z = yellow

	static GLfloat texs[24*2]={	0,1,	1,1,	1,0,	0,0,
								1,1,	1,0,	0,0,	0,1,
								0,1,	1,1,	1,0,	0,0,
								1,1,	1,0,	0,0,	0,1,
								1,0,	0,0,	0,1,	1,1,
								1,0,	0,0,	0,1,	1,1 };

	static GLubyte elements[6*6] ={	0, 1, 2, 0, 2, 3,
									4, 5, 6, 4, 6, 7,
									8, 9,10, 8, 10,11,
									12,13,14,12,14,15,
									16,17,18,16,18,19,
									20,21,22,20,22,23 };

	Context *ctx = gl::context();
	auto curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}

	bool hasPositions = curGlslProg->hasAttribSemantic( geom::Attrib::POSITION );
	bool hasNormals = curGlslProg->hasAttribSemantic( geom::Attrib::NORMAL );
	bool hasTextureCoords = curGlslProg->hasAttribSemantic( geom::Attrib::TEX_COORD_0 );
	bool hasColors = faceColors && curGlslProg->hasAttribSemantic( geom::Attrib::COLOR );

	size_t totalArrayBufferSize = 0;
	if( hasPositions )
		totalArrayBufferSize += sizeof(float)*24*3;
	if( hasNormals )
		totalArrayBufferSize += sizeof(float)*24*3;
	if( hasTextureCoords )
		totalArrayBufferSize += sizeof(float)*24*2;
	if( hasColors )
		totalArrayBufferSize += 24*4;

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();

	VboRef defaultArrayVbo = ctx->getDefaultArrayVbo( totalArrayBufferSize );
	ScopedBuffer vboScp( defaultArrayVbo );
	VboRef elementVbo = ctx->getDefaultElementVbo( 6*6 );
	// we seem to need to orphan the existing element vbo on AMD on OS X
	elementVbo->bufferData( 36, nullptr, GL_STREAM_DRAW );

	elementVbo->bind();
	size_t curBufferOffset = 0;
	if( hasPositions ) {
		int loc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
		enableVertexAttribArray( loc );
		vertexAttribPointer( loc, 3, GL_FLOAT, GL_FALSE, 0, (void*)curBufferOffset );
		defaultArrayVbo->bufferSubData( curBufferOffset, sizeof(float)*24*3, vertices );
		curBufferOffset += sizeof(float)*24*3;
	}
	if( hasNormals ) {
		int loc = curGlslProg->getAttribSemanticLocation( geom::Attrib::NORMAL );
		enableVertexAttribArray( loc );
		vertexAttribPointer( loc, 3, GL_FLOAT, GL_FALSE, 0, (void*)curBufferOffset );
		defaultArrayVbo->bufferSubData( curBufferOffset, sizeof(float)*24*3, normals );
		curBufferOffset += sizeof(float)*24*3;
	}
	if( hasTextureCoords ) {
		int loc = curGlslProg->getAttribSemanticLocation( geom::Attrib::TEX_COORD_0 );
		enableVertexAttribArray( loc );
		vertexAttribPointer( loc, 2, GL_FLOAT, GL_FALSE, 0, (void*)curBufferOffset );
		defaultArrayVbo->bufferSubData( curBufferOffset, sizeof(float)*24*2, texs );
		curBufferOffset += sizeof(float)*24*2;
	}
	if( hasColors ) {
		int loc = curGlslProg->getAttribSemanticLocation( geom::Attrib::COLOR );
		enableVertexAttribArray( loc );
		vertexAttribPointer( loc, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, (void*)curBufferOffset );
		defaultArrayVbo->bufferSubData( curBufferOffset, 24*4, colors );
		curBufferOffset += 24*4;
	}

	elementVbo->bufferSubData( 0, 36, elements );
	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	ctx->drawElements( GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0 );
	ctx->popVao();
}
}

void drawCube( const vec3 &center, const vec3 &size )
{
	drawCubeImpl( center, size, false );
}

void drawColorCube( const vec3 &center, const vec3 &size )
{
	drawCubeImpl( center, size, true );
}
	
void drawStrokedCube( const vec3 &center, const vec3 &size )
{
	auto vertices = getCubePoints( center, size );
	
	static std::array<GLubyte, 24> indices = { 0, 1, 1, 2, 2, 3, 3, 0,		// right side connection
												4, 5, 5, 6, 6, 7, 7, 4,		// left side connection
												0, 4, 1, 5, 2, 6, 3, 7 };	// right to left connections
	
	auto ctx = ci::gl::context();
	gl::GlslProgRef curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}
	
	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();
	gl::VboRef defaultVbo = ctx->getDefaultArrayVbo( sizeof(vec3) * 8 );
	gl::VboRef elementVbo = ctx->getDefaultElementVbo( 24 );
	gl::ScopedBuffer bufferBindScp( defaultVbo );
	defaultVbo->bufferSubData( 0, sizeof(vec3) * 8, vertices.data() );
	
	elementVbo->bind();
	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		gl::enableVertexAttribArray( posLoc );
		gl::vertexAttribPointer( posLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
	}
	
	elementVbo->bufferSubData( 0, 24, indices.data() );
	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	ctx->drawElements( GL_LINES, 24, GL_UNSIGNED_BYTE, 0 );
	ctx->popVao();
}

void draw( const TextureRef &texture, const Area &srcArea, const Rectf &dstRect )
{
	auto ctx = context();

	Rectf texRect = texture->getAreaTexCoords( srcArea );

	ScopedVao vaoScp( ctx->getDrawTextureVao() );
	ScopedBuffer vboScp( ctx->getDrawTextureVbo() );
	ScopedTextureBind texBindScope( texture );

	auto glsl = getStockShader( ShaderDef().uniformBasedPosAndTexCoord().color().texture( texture ) );
	ScopedGlslProg glslScp( glsl );
	glsl->uniform( "uTex0", 0 );
	glsl->uniform( "uPositionOffset", dstRect.getUpperLeft() );
	glsl->uniform( "uPositionScale", dstRect.getSize() );
	glsl->uniform( "uTexCoordOffset", texRect.getUpperLeft() );
	glsl->uniform( "uTexCoordScale", texRect.getSize() );

	ctx->setDefaultShaderVars();
	ctx->drawArrays( GL_TRIANGLE_STRIP, 0, 4 );
}

void draw( const TextureRef &texture, const Rectf &dstRect )
{
	draw( texture, texture->getBounds(), dstRect );
}


void draw( const TextureRef &texture, const vec2 &dstOffset )
{
	if( ! texture )
		return;
	draw( texture, texture->getBounds(), Rectf( texture->getBounds() ) + dstOffset );
}

void draw( const Path2d &path, float approximationScale )
{
	if( path.getNumSegments() == 0 )
		return;

	auto ctx = context();
	GlslProgRef curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}

	vector<vec2> points = path.subdivide( approximationScale );
	VboRef arrayVbo = ctx->getDefaultArrayVbo( sizeof(vec2) * points.size() );
	arrayVbo->bufferSubData( 0, sizeof(vec2) * points.size(), points.data() );

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();
	ScopedBuffer bufferBindScp( arrayVbo );
	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		enableVertexAttribArray( posLoc );
		vertexAttribPointer( posLoc, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)nullptr );
	}

	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	ctx->drawArrays( GL_LINE_STRIP, 0, (GLsizei)points.size() );
	ctx->popVao();
}

void draw( const Shape2d &shape, float approximationScale )
{
	for( const auto &path : shape.getContours() )
		gl::draw( path );
}

void draw( const PolyLine2 &polyLine )
{
	auto ctx = context();
	GlslProgRef curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}

	const vector<vec2> &points = polyLine.getPoints();
	VboRef arrayVbo = ctx->getDefaultArrayVbo( sizeof(vec2) * points.size() );
	arrayVbo->bufferSubData( 0, sizeof(vec2) * points.size(), points.data() );

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();
	ScopedBuffer bufferBindScp( arrayVbo );
	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		enableVertexAttribArray( posLoc );
		vertexAttribPointer( posLoc, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)nullptr );
	}

	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	ctx->drawArrays( GL_LINE_STRIP, 0, (GLsizei)points.size() );
	ctx->popVao();
}

void draw( const PolyLine3 &polyLine )
{
	auto ctx = context();
	GlslProgRef curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}
	
	const vector<vec3> &points = polyLine.getPoints();
	VboRef arrayVbo = ctx->getDefaultArrayVbo( sizeof(vec3) * points.size() );
	arrayVbo->bufferSubData( 0, sizeof(vec3) * points.size(), points.data() );

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();
	ScopedBuffer bufferBindScp( arrayVbo );
	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		enableVertexAttribArray( posLoc );
		vertexAttribPointer( posLoc, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)nullptr );
	}

	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	ctx->drawArrays( GL_LINE_STRIP, 0, (GLsizei)points.size() );
	ctx->popVao();
}

void drawLine( const vec3 &a, const vec3 &b )
{
	const int dims = 3;
	const int size = sizeof( vec3 ) * 2;
	array<vec3, 2> points = { a, b };

	auto ctx = context();
	GlslProgRef curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();

	VboRef arrayVbo = ctx->getDefaultArrayVbo( size );
	ScopedBuffer bufferBindScp( arrayVbo );

	arrayVbo->bufferSubData( 0, size, points.data() );
	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		enableVertexAttribArray( posLoc );
		vertexAttribPointer( posLoc, dims, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)nullptr );
	}
	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	ctx->drawArrays( GL_LINES, 0, points.size() );
	ctx->popVao();
}

void drawLine( const vec2 &a, const vec2 &b )
{
	const int dims = 2;
	const int size = sizeof( vec2 ) * 2;
	array<vec2, 2> points = { a, b };
	auto ctx = context();
	GlslProgRef curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();

	VboRef arrayVbo = ctx->getDefaultArrayVbo( size );
	ScopedBuffer bufferBindScp( arrayVbo );

	arrayVbo->bufferSubData( 0, size, points.data() );
	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		enableVertexAttribArray( posLoc );
		vertexAttribPointer( posLoc, dims, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)nullptr );
	}
	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	ctx->drawArrays( GL_LINES, 0, points.size() );
	ctx->popVao();
}

void draw( const TriMesh &mesh )
{
	if( mesh.getNumVertices() <= 0 )
		return;

	draw( VboMesh::create( mesh ) );
}
	
void draw( const geom::Source &source )
{
	if( source.getNumVertices() <= 0 )
		return;
	
	draw( VboMesh::create( source ) );
}

void drawSolid( const Path2d &path, float approximationScale )
{
	draw( Triangulator( path ).calcMesh() );
}

void drawSolid( const Shape2d &shape, float approximationScale )
{
	draw( Triangulator( shape ).calcMesh() );
}

void drawSolid( const PolyLine2 &polyLine )
{
	draw( Triangulator( polyLine ).calcMesh() );
}

void drawSolidRect( const Rectf &r, const vec2 &upperLeftTexCoord, const vec2 &lowerRightTexCoord )
{
	auto ctx = context();
	GlslProgRef curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}

	GLfloat data[8+8]; // both verts and texCoords
	GLfloat *verts = data, *texs = data + 8;

	verts[0*2+0] = r.getX2(); texs[0*2+0] = lowerRightTexCoord.x;
	verts[0*2+1] = r.getY1(); texs[0*2+1] = upperLeftTexCoord.y;
	verts[1*2+0] = r.getX1(); texs[1*2+0] = upperLeftTexCoord.x;
	verts[1*2+1] = r.getY1(); texs[1*2+1] = upperLeftTexCoord.y;
	verts[2*2+0] = r.getX2(); texs[2*2+0] = lowerRightTexCoord.x;
	verts[2*2+1] = r.getY2(); texs[2*2+1] = lowerRightTexCoord.y;
	verts[3*2+0] = r.getX1(); texs[3*2+0] = upperLeftTexCoord.x;
	verts[3*2+1] = r.getY2(); texs[3*2+1] = lowerRightTexCoord.y;

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();
	VboRef defaultVbo = ctx->getDefaultArrayVbo( sizeof(float)*16 );
	ScopedBuffer bufferBindScp( defaultVbo );
	defaultVbo->bufferSubData( 0, sizeof(float)*16, data );

	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		enableVertexAttribArray( posLoc );
		vertexAttribPointer( posLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );
	}
	int texLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::TEX_COORD_0 );
	if( texLoc >= 0 ) {
		enableVertexAttribArray( texLoc );
		vertexAttribPointer( texLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float)*8) );
	}
	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	ctx->drawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	ctx->popVao();
}

void drawStrokedRect( const Rectf &rect )
{
	GLfloat verts[8];
	verts[0] = rect.x1;	verts[1] = rect.y1;
	verts[2] = rect.x2;	verts[3] = rect.y1;
	verts[4] = rect.x2;	verts[5] = rect.y2;
	verts[6] = rect.x1;	verts[7] = rect.y2;

	auto ctx = context();
	GlslProgRef curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();

	VboRef defaultVbo = ctx->getDefaultArrayVbo( 8 * sizeof( float ) );
	ScopedBuffer bufferBindScp( defaultVbo );
	defaultVbo->bufferSubData( 0, 8 * sizeof( float ), verts );

	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		enableVertexAttribArray( posLoc );
		vertexAttribPointer( posLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );
	}

	ctx->setDefaultShaderVars();
	ctx->getDefaultVao()->replacementBindEnd();
	ctx->drawArrays( GL_LINE_LOOP, 0, 4 );
	ctx->popVao();
}

void drawStrokedRect( const Rectf &rect, float lineWidth )
{
	const float halfWidth = lineWidth / 2;
	GLfloat verts[32];
	verts[0] = rect.x1 + halfWidth;		verts[1] = rect.y2 - halfWidth;		// left bar
	verts[2] = rect.x1 + halfWidth;		verts[3] = rect.y1 + halfWidth;
	verts[4] = rect.x1 - halfWidth;		verts[5] = rect.y2 + halfWidth;
	verts[6] = rect.x1 - halfWidth;		verts[7] = rect.y1 - halfWidth;
	verts[8] = rect.x1 + halfWidth;		verts[9] = rect.y1 + halfWidth;
	verts[10] = rect.x2 - halfWidth;	verts[11] = rect.y1 + halfWidth;	// upper bar
	verts[12] = rect.x1 - halfWidth;	verts[13] = rect.y1 - halfWidth;
	verts[14] = rect.x2 + halfWidth;	verts[15] = rect.y1 - halfWidth;
	verts[16] = rect.x2 - halfWidth;	verts[17] = rect.y1 + halfWidth;	// right bar
	verts[18] = rect.x2 - halfWidth;	verts[19] = rect.y2 - halfWidth;
	verts[20] = rect.x2 + halfWidth;	verts[21] = rect.y1 - halfWidth;
	verts[22] = rect.x2 + halfWidth;	verts[23] = rect.y2 + halfWidth;
	verts[24] = rect.x2 - halfWidth;	verts[25] = rect.y2 - halfWidth;	// bottom bar
	verts[26] = rect.x1 + halfWidth;	verts[27] = rect.y2 - halfWidth;
	verts[28] = rect.x2 + halfWidth;	verts[29] = rect.y2 + halfWidth;
	verts[30] = rect.x1 - halfWidth;	verts[31] = rect.y2 + halfWidth;

	auto ctx = context();
	GlslProgRef curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();

	VboRef defaultVbo = ctx->getDefaultArrayVbo( 32 * sizeof( float ) );
	ScopedBuffer bufferBindScp( defaultVbo );
	defaultVbo->bufferSubData( 0, 32 * sizeof( float ), verts );

	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		enableVertexAttribArray( posLoc );
		vertexAttribPointer( posLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );
	}

	ctx->setDefaultShaderVars();
	ctx->getDefaultVao()->replacementBindEnd();	
	ctx->drawArrays( GL_TRIANGLE_STRIP, 0, 16 );
	ctx->popVao();
}

void drawStrokedCircle( const vec2 &center, float radius, int numSegments )
{
	auto ctx = context();
	GlslProgRef curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}
	
	if( numSegments <= 0 ) {
		numSegments = static_cast<int>(math<double>::floor( radius * M_PI * 2 ) );
	}
	if( numSegments < 3 ) {
		numSegments = 3;
	}
	// construct circle
	const size_t numVertices = numSegments;
	vector<vec2> positions;
	positions.assign( numVertices, center );	// all vertices start at center
	const float tDelta = 2.0f * static_cast<float>(M_PI) * (1.0f / numVertices);
	float t = 0;
	for( auto &pos : positions ) {
		const vec2 unit( math<float>::cos( t ), math<float>::sin( t ) );
		pos += unit * radius;	// push out from center
		t += tDelta;
	}
	// copy data to GPU
	const size_t size = positions.size() * sizeof( vec2 );
	auto arrayVbo = ctx->getDefaultArrayVbo( size );
	arrayVbo->bufferSubData( 0, size, (GLvoid*)positions.data() );
	// set attributes
	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();
	ScopedBuffer bufferBindScp( arrayVbo );

	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		enableVertexAttribArray( posLoc );
		vertexAttribPointer( posLoc, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)nullptr );
	}
	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	// draw
	drawArrays( GL_LINE_LOOP, 0, (GLsizei)positions.size() );
	ctx->popVao();
}

void drawSolidCircle( const vec2 &center, float radius, int numSegments )
{
	auto ctx = context();
	GlslProgRef curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();

	if( numSegments <= 0 ) {
		numSegments = (int)math<double>::floor( radius * M_PI * 2 );
	}
	if( numSegments < 3 ) numSegments = 3;
	size_t numVertices = numSegments + 2;

	size_t worstCaseSize = numVertices * sizeof(float) * ( 2 + 2 + 3 );
	VboRef defaultVbo = ctx->getDefaultArrayVbo( worstCaseSize );
	ScopedBuffer vboScp( defaultVbo );

	size_t dataSizeBytes = 0;

	size_t vertsOffset, texCoordsOffset, normalsOffset;
	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		enableVertexAttribArray( posLoc );
		vertexAttribPointer( posLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)dataSizeBytes );
		vertsOffset = dataSizeBytes;
		dataSizeBytes += numVertices * 2 * sizeof(float);
	}
	int texLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::TEX_COORD_0 );
	if( texLoc >= 0 ) {
		enableVertexAttribArray( texLoc );
		vertexAttribPointer( texLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)dataSizeBytes );
		texCoordsOffset = dataSizeBytes;
		dataSizeBytes += numVertices * 2 * sizeof(float);
	}
	int normalLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::NORMAL );
	if( normalLoc >= 0 ) {
		enableVertexAttribArray( normalLoc );
		vertexAttribPointer( normalLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)(dataSizeBytes) );
		normalsOffset = dataSizeBytes;
		dataSizeBytes += numVertices * 3 * sizeof(float);
	}

	unique_ptr<uint8_t[]> data( new uint8_t[dataSizeBytes] );
	vec2 *verts = ( posLoc >= 0 ) ? reinterpret_cast<vec2*>( data.get() + vertsOffset ) : nullptr;
	vec2 *texCoords = ( texLoc >= 0 ) ? reinterpret_cast<vec2*>( data.get() + texCoordsOffset ) : nullptr;
	vec3 *normals = ( normalLoc >= 0 ) ? reinterpret_cast<vec3*>( data.get() + normalsOffset ) : nullptr;

	if( verts )
		verts[0] = center;
	if( texCoords )
		texCoords[0] = vec2( 0.5f, 0.5f );
	if( normals )
		normals[0] = vec3( 0, 0, 1 );
	const float tDelta = 1.0f / numSegments * 2 * (float)M_PI;
	float t = 0;
	for( int s = 0; s <= numSegments; s++ ) {
		const vec2 unit( math<float>::cos( t ), math<float>::sin( t ) );
		if( verts )
			verts[s+1] = center + unit * radius;
		if( texCoords )
			texCoords[s+1] = unit * 0.5f + vec2( 0.5f, 0.5f );
		if( normals )
			normals[s+1] = vec3( 0, 0, 1 );
		t += tDelta;
	}

	defaultVbo->bufferSubData( 0, dataSizeBytes, data.get() );
	ctx->getDefaultVao()->replacementBindEnd();

	ctx->setDefaultShaderVars();
	ctx->drawArrays( GL_TRIANGLE_FAN, 0, numSegments + 2 );
	ctx->popVao();
}

void drawSolidTriangle( const vec2 pts[3] )
{
	drawSolidTriangle( pts, nullptr );
}

void drawSolidTriangle( const vec2 &pt0, const vec2 &pt1, const vec2 &pt2 )
{
	vec2 pts[3] = { pt0, pt1, pt2 };
	drawSolidTriangle( pts, nullptr );
}

//! Renders a textured triangle.
void drawSolidTriangle( const vec2 &pt0, const vec2 &pt1, const vec2 &pt2, const vec2 &texCoord0, const vec2 &texCoord1, const vec2 &texCoord2 )
{
	vec2 pts[3] = { pt0, pt1, pt2 };
	vec2 texs[3] = { texCoord0, texCoord1, texCoord2 };
	
	drawSolidTriangle( pts, texs );
}

void drawStrokedTriangle( const vec2 &pt0, const vec2 &pt1, const vec2 &pt2 )
{
	vec2 pts[3] = { pt0, pt1, pt2 };
	drawSolidTriangle( pts, nullptr );
}

void drawSolidTriangle( const vec2 pts[3], const vec2 texCoord[3] )
{
	auto ctx = context();
	GlslProgRef curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}

	GLfloat data[3*2+3*2]; // both verts and texCoords
	memcpy( data, pts, sizeof(float) * 3 * 2 );
	if( texCoord )
		memcpy( data + 3 * 2, texCoord, sizeof(float) * 3 * 2 );

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();
	VboRef defaultVbo = ctx->getDefaultArrayVbo( sizeof(float)*12 );
	ScopedBuffer bufferBindScp( defaultVbo );
	defaultVbo->bufferSubData( 0, sizeof(float) * ( texCoord ? 12 : 6 ), data );

	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		enableVertexAttribArray( posLoc );
		vertexAttribPointer( posLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );
	}
	if( texCoord ) {
		int texLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::TEX_COORD_0 );
		if( texLoc >= 0 ) {
			enableVertexAttribArray( texLoc );
			vertexAttribPointer( texLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float)*6) );
		}
	}
	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	ctx->drawArrays( GL_TRIANGLES, 0, 3 );
	ctx->popVao();
}

void drawSphere( const vec3 &center, float radius, int subdivisions )
{
	auto ctx = gl::context();
	GlslProgRef curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}
	//auto batch = gl::Batch::create( geom::Sphere().center( center ).radius( radius ).segments( segments ).normals().texCoords(), glslProg );
	//batch->draw();

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();
	gl::VboMeshRef mesh = gl::VboMesh::create( geom::Sphere().center( center ).radius( radius ).subdivisions( subdivisions ),
			{ { VboMesh::Layout().attrib( geom::POSITION, 3 ).attrib( geom::NORMAL, 3 ).attrib( geom::TEX_COORD_0, 2 ), ctx->getDefaultArrayVbo() } }, ctx->getDefaultElementVbo() );
	mesh->buildVao( curGlslProg );
	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	mesh->drawImpl();
	ctx->popVao();
}

void drawBillboard( const vec3 &pos, const vec2 &scale, float rotationRadians, const vec3 &bbRight, const vec3 &bbUp, const Rectf &texCoords )
{
	auto ctx = context();
	GlslProgRef curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}

	GLfloat data[12+8]; // both verts and texCoords
	vec3 *verts = (vec3*)data;
	float *texCoordsOut = data + 12;

	float sinA = math<float>::sin( rotationRadians );
	float cosA = math<float>::cos( rotationRadians );

	verts[0] = pos + bbRight * ( -0.5f * scale.x * cosA - 0.5f * sinA * scale.y ) + bbUp * ( -0.5f * scale.x * sinA + 0.5f * cosA * scale.y );
	texCoordsOut[0*2+0] = texCoords.getX1(); texCoordsOut[0*2+1] = texCoords.getY1();
	verts[1] = pos + bbRight * ( -0.5f * scale.x * cosA - -0.5f * sinA * scale.y ) + bbUp * ( -0.5f * scale.x * sinA + -0.5f * cosA * scale.y );
	texCoordsOut[1*2+0] = texCoords.getX1(); texCoordsOut[1*2+1] = texCoords.getY2();
	verts[2] = pos + bbRight * ( 0.5f * scale.x * cosA - 0.5f * sinA * scale.y ) + bbUp * ( 0.5f * scale.x * sinA + 0.5f * cosA * scale.y );
	texCoordsOut[2*2+0] = texCoords.getX2(); texCoordsOut[2*2+1] = texCoords.getY1();
	verts[3] = pos + bbRight * ( 0.5f * scale.x * cosA - -0.5f * sinA * scale.y ) + bbUp * ( 0.5f * scale.x * sinA + -0.5f * cosA * scale.y );
	texCoordsOut[3*2+0] = texCoords.getX2(); texCoordsOut[3*2+1] = texCoords.getY2();

	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();
	VboRef defaultVbo = ctx->getDefaultArrayVbo( sizeof(float)*20 );
	ScopedBuffer bufferBindScp( defaultVbo );
	defaultVbo->bufferSubData( 0, sizeof(float)*20, data );

	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		enableVertexAttribArray( posLoc );
		vertexAttribPointer( posLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
	}
	int texLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::TEX_COORD_0 );
	if( texLoc >= 0 ) {
		enableVertexAttribArray( texLoc );
		vertexAttribPointer( texLoc, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float)*12) );
	}

	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	ctx->drawArrays( GL_TRIANGLE_STRIP, 0, 4 );
	ctx->popVao();
}
	
void drawFrustum( const Camera &cam )
{
	vec3 nearTopLeft, nearTopRight, nearBottomLeft, nearBottomRight;
	cam.getNearClipCoordinates( &nearTopLeft, &nearTopRight, &nearBottomLeft, &nearBottomRight );
	
	vec3 farTopLeft, farTopRight, farBottomLeft, farBottomRight;
	cam.getFarClipCoordinates( &farTopLeft, &farTopRight, &farBottomLeft, &farBottomRight );

	// extract camera position from modelview matrix, so that it will work with any camera	
	//  see: http://www.gamedev.net/topic/397751-how-to-get-camera-position/page__p__3638207#entry3638207
	mat4 view = cam.getViewMatrix();
	vec3 eye;
	eye.x = -( view[0][0] * view[3][0] + view[0][1] * view[3][1] + view[0][2] * view[3][2] );
	eye.y = -( view[1][0] * view[3][0] + view[1][1] * view[3][1] + view[1][2] * view[3][2] );
	eye.z = -( view[2][0] * view[3][0] + view[2][1] * view[3][1] + view[2][2] * view[3][2] );

														// indices
	std::array<vec3, 9> vertices = { eye,				// 0
									nearTopLeft,		// 1
									nearTopRight,		// 2
									nearBottomLeft,		// 3
									nearBottomRight,	// 4
									farTopLeft,			// 5
									farTopRight,		// 6
									farBottomLeft,		// 7
									farBottomRight };	// 8		// indice usage
	std::array<GLubyte, 32> indices = { 0, 1, 0, 2, 0, 4, 0, 3,		// draws from eye to near plane
										5, 1, 6, 2, 7, 3, 8, 4,		// draws from far to near corners
										1, 2, 2, 4, 4, 3, 3, 1,		// draws near rect
										5, 6, 6, 8, 8, 7, 7, 5};	// draws far rect
	
	auto ctx = ci::gl::context();
	gl::GlslProgRef curGlslProg = ctx->getGlslProg();
	if( ! curGlslProg ) {
		CI_LOG_E( "No GLSL program bound" );
		return;
	}
	
	ctx->pushVao();
	ctx->getDefaultVao()->replacementBindBegin();
	gl::VboRef defaultVbo = ctx->getDefaultArrayVbo( sizeof(vec3)*9 );
	gl::VboRef elementVbo = ctx->getDefaultElementVbo( 32 );
	gl::ScopedBuffer bufferBindScp( defaultVbo );
	defaultVbo->bufferSubData( 0, sizeof(vec3)*9, vertices.data() );
	
	elementVbo->bind();
	int posLoc = curGlslProg->getAttribSemanticLocation( geom::Attrib::POSITION );
	if( posLoc >= 0 ) {
		gl::enableVertexAttribArray( posLoc );
		gl::vertexAttribPointer( posLoc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
	}
	
	elementVbo->bufferSubData( 0, 32, indices.data() );
	ctx->getDefaultVao()->replacementBindEnd();
	ctx->setDefaultShaderVars();
	ctx->drawElements( GL_LINES, 32, GL_UNSIGNED_BYTE, 0 );
	ctx->popVao();
}
	
void drawCoordinateFrame( float axisLength, float headLength, float headRadius )
{
	gl::ScopedColor color( ColorA( 1.0f, 0.0f, 0.0f, 1.0f ) );
	drawVector( vec3( 0.0f ), vec3( 1.0f, 0.0f, 0.0f ) * axisLength, headLength, headRadius );
	gl::color( 0.0f, 1.0f, 0.0f, 1.0f );
	drawVector( vec3( 0.0f ), vec3( 0.0f, 1.0f, 0.0f ) * axisLength, headLength, headRadius );
	gl::color( 0.0f, 0.0f, 1.0f, 1.0f );
	drawVector( vec3( 0.0f ), vec3( 0.0f, 0.0f, 1.0f ) * axisLength, headLength, headRadius );
}
	
void drawVector( const vec3& start, const vec3& end, float headLength, float headRadius )
{
	gl::drawLine( start, end );
	vec3 dir = end - start;
	vec3 ori = end - normalize( dir ) * headLength;
	gl::draw( geom::Cone().base( headRadius ).height( headLength ).origin( ori ).direction( dir ) );
}
	
namespace {
void drawStringHelper( const std::string &str, const vec2 &pos, const ColorA &color, Font font, int justification )
{
	if( str.empty() )
		return;

	// justification: { left = -1, center = 0, right = 1 }
	ScopedColor colorScp( Color::white() );

	static Font defaultFont = Font::getDefault();
	if( ! font )
		font = defaultFont;

	float baselineOffset;
#if defined( CINDER_COCOA_TOUCH )
	ivec2 actualSize;
	Surface8u pow2Surface( renderStringPow2( str, font, color, &actualSize, &baselineOffset ) );
	gl::TextureRef tex = gl::Texture::create( pow2Surface );
	tex->setCleanSize( actualSize.x, actualSize.y );
	baselineOffset += pow2Surface.getHeight();
#else
	gl::TextureRef tex = gl::Texture::create( renderString( str, font, color, &baselineOffset ) );
#endif

	if( justification == -1 ) // left
		draw( tex, pos - vec2( 0, baselineOffset ) );
	else if( justification == 0 ) // center
		draw( tex, pos - vec2( tex->getWidth() * 0.5f, baselineOffset ) );
	else // right
		draw( tex, pos - vec2( (float)tex->getWidth(), baselineOffset ) );
}
} // anonymous namespace

void drawString( const std::string &str, const vec2 &pos, const ColorA &color, Font font )
{
	drawStringHelper( str, pos, color, font, -1 );
}

void drawStringCentered( const std::string &str, const vec2 &pos, const ColorA &color, Font font )
{
	drawStringHelper( str, pos, color, font, 0 );
}

void drawStringRight( const std::string &str, const vec2 &pos, const ColorA &color, Font font )
{
	drawStringHelper( str, pos, color, font, 1 );
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

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedVao
ScopedVao::ScopedVao( const VaoRef &vao )
	: mCtx( gl::context() )
{
	mCtx->pushVao( vao );
}

ScopedVao::~ScopedVao()
{
	mCtx->popVao();
}

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedBuffer
ScopedBuffer::ScopedBuffer( const BufferObjRef &bufferObj )
	: mCtx( gl::context() ), mTarget( bufferObj->getTarget() )
{
	mCtx->pushBufferBinding( mTarget, bufferObj->getId() );
}

ScopedBuffer::ScopedBuffer( GLenum target, GLuint id )
		: mCtx( gl::context() ), mTarget( target )
{
	mCtx->pushBufferBinding( target, id );
}

ScopedBuffer::~ScopedBuffer()
{
	mCtx->popBufferBinding( mTarget );
}

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedState
ScopedState::ScopedState( GLenum cap, GLboolean value )
	: mCtx( gl::context() ), mCap( cap )
{
	mCtx->pushBoolState( cap, value );
}

ScopedState::~ScopedState() {
	mCtx->popBoolState( mCap );
}

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedColor
ScopedColor::ScopedColor()
	: mCtx( gl::context() )
{
	mColor = mCtx->getCurrentColor();
}

ScopedColor::ScopedColor( const ColorAf &color )
	: mCtx( gl::context() )
{
	mColor = mCtx->getCurrentColor();
	mCtx->setCurrentColor( color );
}

ScopedColor::~ScopedColor()
{
	mCtx->setCurrentColor( mColor );
}

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedBlend
ScopedBlend::ScopedBlend( GLboolean enable )
	: mCtx( gl::context() ), mSaveFactors( false )
{
	mCtx->pushBoolState( GL_BLEND, enable );
}

//! Parallels glBlendFunc(), implicitly enables blending
ScopedBlend::ScopedBlend( GLenum sfactor, GLenum dfactor )
	: mCtx( gl::context() ), mSaveFactors( true )
{
	mCtx->pushBoolState( GL_BLEND, GL_TRUE );
	mCtx->pushBlendFuncSeparate( sfactor, dfactor, sfactor, dfactor );
}

//! Parallels glBlendFuncSeparate(), implicitly enables blending
ScopedBlend::ScopedBlend( GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha )
	: mCtx( gl::context() ), mSaveFactors( true )
{
	mCtx->pushBoolState( GL_BLEND, GL_TRUE );
	mCtx->pushBlendFuncSeparate( srcRGB, dstRGB, srcAlpha, dstAlpha );
}

ScopedBlend::~ScopedBlend()
{
	mCtx->popBoolState( GL_BLEND );
	if( mSaveFactors )
		mCtx->popBlendFuncSeparate();
}

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedGlslProg
ScopedGlslProg::ScopedGlslProg( const GlslProgRef &prog )
	: mCtx( gl::context() )
{
	mCtx->pushGlslProg( prog );
}

ScopedGlslProg::ScopedGlslProg( const std::shared_ptr<const GlslProg> &prog )
	: mCtx( gl::context() )
{
	mCtx->pushGlslProg( std::const_pointer_cast<GlslProg>( prog ) );
}

ScopedGlslProg::~ScopedGlslProg()
{
	mCtx->popGlslProg();
}

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedFramebuffer
ScopedFramebuffer::ScopedFramebuffer( const FboRef &fbo, GLenum target )
	: mCtx( gl::context() ), mTarget( target )
{
	mCtx->pushFramebuffer( fbo, target );
}

ScopedFramebuffer::ScopedFramebuffer( GLenum target, GLuint framebufferId )
	: mCtx( gl::context() ), mTarget( target )
{
	mCtx->pushFramebuffer( target, framebufferId );
}

ScopedFramebuffer::~ScopedFramebuffer()
{	
#if ! defined( SUPPORTS_FBO_MULTISAMPLING )
	mCtx->popFramebuffer( GL_FRAMEBUFFER );
#else
	if( mTarget == GL_FRAMEBUFFER || mTarget == GL_READ_FRAMEBUFFER )
		mCtx->popFramebuffer( GL_READ_FRAMEBUFFER );
	if( mTarget == GL_FRAMEBUFFER || mTarget == GL_DRAW_FRAMEBUFFER )
		mCtx->popFramebuffer( GL_DRAW_FRAMEBUFFER );
#endif
}

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedActiveTexture
ScopedActiveTexture::ScopedActiveTexture( uint8_t textureUnit )
	: mCtx( gl::context() )
{
	mCtx->pushActiveTexture( textureUnit );
}
	
ScopedActiveTexture::~ScopedActiveTexture()
{
	mCtx->popActiveTexture();
}

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedTextureBind
ScopedTextureBind::ScopedTextureBind( GLenum target, GLuint textureId )
	: mCtx( gl::context() ), mTarget( target )
{
	mTextureUnit = mCtx->getActiveTexture();
	mCtx->pushTextureBinding( mTarget, textureId, mTextureUnit );
}

ScopedTextureBind::ScopedTextureBind( GLenum target, GLuint textureId, uint8_t textureUnit )
	: mCtx( gl::context() ), mTarget( target ), mTextureUnit( textureUnit )
{
	mCtx->pushTextureBinding( mTarget, textureId, mTextureUnit );
}

ScopedTextureBind::ScopedTextureBind( const TextureBaseRef &texture )
	: mCtx( gl::context() ), mTarget( texture->getTarget() )
{
	mTextureUnit = mCtx->getActiveTexture();
	mCtx->pushTextureBinding( mTarget, texture->getId(), mTextureUnit );
}

ScopedTextureBind::ScopedTextureBind( const TextureBaseRef &texture, uint8_t textureUnit )
	: mCtx( gl::context() ), mTarget( texture->getTarget() ), mTextureUnit( textureUnit )
{
	mCtx->pushTextureBinding( mTarget, texture->getId(), mTextureUnit );
}

// ----------------------------------------------------------------
// These overloads are to alleviate a VS2013 bug where it cannot deduce
// the correct constructor when a TextureBaseRef subclass is passed in
ScopedTextureBind::ScopedTextureBind( const Texture2dRef &texture, uint8_t textureUnit )
	: mCtx( gl::context() ), mTarget( texture->getTarget() ), mTextureUnit( textureUnit )
{
	mCtx->pushTextureBinding( mTarget, texture->getId(), mTextureUnit );
}

#if ! defined( CINDER_GL_ES_2 )
ScopedTextureBind::ScopedTextureBind( const Texture3dRef &texture, uint8_t textureUnit )
	: mCtx( gl::context() ), mTarget( texture->getTarget() ), mTextureUnit( textureUnit )
{
	mCtx->pushTextureBinding( mTarget, texture->getId(), mTextureUnit );
}
#endif // ! defined( CINDER_GL_ES_2 )

ScopedTextureBind::ScopedTextureBind( const TextureCubeMapRef &texture, uint8_t textureUnit )
	: mCtx( gl::context() ), mTarget( texture->getTarget() ), mTextureUnit( textureUnit )
{
	mCtx->pushTextureBinding( mTarget, texture->getId(), mTextureUnit );
}
// ----------------------------------------------------------------

ScopedTextureBind::~ScopedTextureBind()
{
	mCtx->popTextureBinding( mTarget, mTextureUnit );
}

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedScissor
ScopedScissor::ScopedScissor( const ivec2 &lowerLeftPosition, const ivec2 &dimension )
	: mCtx( gl::context() )
{
	mCtx->pushBoolState( GL_SCISSOR_TEST, GL_TRUE );
	mCtx->pushScissor( std::pair<ivec2, ivec2>( lowerLeftPosition, dimension ) );
}

ScopedScissor::ScopedScissor( int lowerLeftX, int lowerLeftY, int width, int height )
	: mCtx( gl::context() )
{
	mCtx->pushBoolState( GL_SCISSOR_TEST, GL_TRUE );
	mCtx->pushScissor( std::pair<ivec2, ivec2>( ivec2( lowerLeftX, lowerLeftY ), ivec2( width, height ) ) );		
}
	
ScopedScissor::~ScopedScissor()
{
	mCtx->popBoolState( GL_SCISSOR_TEST );
	mCtx->popScissor();
}

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedFaceCulling
ScopedFaceCulling::ScopedFaceCulling( bool cull )
	: mCtx( gl::context() ), mSaveFace( false )
{
	mCtx->pushBoolState( GL_CULL_FACE, cull );
}

ScopedFaceCulling::ScopedFaceCulling( bool cull, GLenum face )
	: mCtx( gl::context() ), mSaveFace( true )
{
	mCtx->pushBoolState( GL_CULL_FACE, cull );
	mCtx->pushCullFace( face );
}

ScopedFaceCulling::~ScopedFaceCulling()
{
	mCtx->popBoolState( GL_CULL_FACE );
	if( mSaveFace )
		mCtx->popCullFace();
}

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedRenderbuffer
ScopedRenderbuffer::ScopedRenderbuffer( const RenderbufferRef &rb )
	: mCtx( gl::context() )
{
	mCtx->pushRenderbufferBinding( GL_RENDERBUFFER, rb->getId() );
}

ScopedRenderbuffer::ScopedRenderbuffer( GLenum target, GLuint id )
	: mCtx( gl::context() )
{
	// this is the only legal value currently
	CI_ASSERT( target == GL_RENDERBUFFER );
	mCtx->pushRenderbufferBinding( target, id );
}

ScopedRenderbuffer::~ScopedRenderbuffer()
{
	mCtx->popRenderbufferBinding( GL_RENDERBUFFER );
}

///////////////////////////////////////////////////////////////////////////////////////////
// ScopedViewport
ScopedViewport::ScopedViewport( const ivec2 &lowerLeftPosition, const ivec2 &dimension )
	: mCtx( gl::context() )
{
	mCtx->pushViewport( std::pair<ivec2, ivec2>( lowerLeftPosition, dimension ) );
}

ScopedViewport::ScopedViewport( int lowerLeftX, int lowerLeftY, int width, int height )
	: mCtx( gl::context() )
{
	mCtx->pushViewport( std::pair<ivec2, ivec2>( ivec2( lowerLeftX, lowerLeftY ), ivec2( width, height ) ) );		
}
	
ScopedViewport::~ScopedViewport()
{
	mCtx->popViewport();
}

} } // namespace cinder::gl
