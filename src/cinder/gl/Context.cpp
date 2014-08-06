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

#include "cinder/gl/Context.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Environment.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Shader.h"
#include "cinder/gl/Vao.h"
#include "cinder/gl/Vbo.h"
#include "cinder/gl/TransformFeedbackObj.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/Batch.h"
#include "cinder/gl/ConstantStrings.h"
#include "cinder/Log.h"
#include "cinder/Utilities.h"

#include "cinder/app/App.h"

using namespace std;

// ES 2 Multisampling is available on iOS via an extension
#if ! defined( CINDER_GL_ES ) || ( defined( CINDER_COCOA_TOUCH ) )
	#define SUPPORTS_FBO_MULTISAMPLING
	#if defined( CINDER_COCOA_TOUCH )
		#define GL_READ_FRAMEBUFFER					GL_READ_FRAMEBUFFER_APPLE
		#define GL_DRAW_FRAMEBUFFER					GL_DRAW_FRAMEBUFFER_APPLE
		#define GL_READ_FRAMEBUFFER_BINDING			GL_READ_FRAMEBUFFER_BINDING_APPLE
		#define GL_DRAW_FRAMEBUFFER_BINDING			GL_DRAW_FRAMEBUFFER_BINDING_APPLE
	#endif
#endif

namespace cinder { namespace gl {

#if defined( CINDER_COCOA )
	static pthread_key_t sThreadSpecificCurrentContextKey;
	static bool sThreadSpecificCurrentContextInitialized = false;
#elif defined( _MSC_VER )
	__declspec(thread) Context *sThreadSpecificCurrentContext = NULL;
#else
	thread_local Context *sThreadSpecificCurrentContext = NULL;
#endif

Context::Context( const std::shared_ptr<PlatformData> &platformData )
	: mPlatformData( platformData ),
	mColor( ColorAf::white() )
#if ! defined( CINDER_GL_ES )
	,mCachedFrontPolygonMode( GL_FILL ), mCachedBackPolygonMode( GL_FILL ),
	mCachedTransformFeedbackObj( nullptr )
#endif
	,mObjectTrackingEnabled( platformData->mObjectTracking )
{
	// set thread's active Context to 'this' in case anything calls gl::context() (like the GlslProg constructor)
	Context::reflectCurrent( this );

	// setup default VAO
#if ! defined( CINDER_GL_ES )
	mDefaultVao = Vao::create();
	mVaoStack.push_back( mDefaultVao );
	mDefaultVao->setContext( this );
	mDefaultVao->bindImpl( NULL );
	
	mBufferBindingStack[GL_ARRAY_BUFFER] = vector<int>();
	mBufferBindingStack[GL_ARRAY_BUFFER].push_back( 0 );
	mBufferBindingStack[GL_ELEMENT_ARRAY_BUFFER] = vector<int>();
	mBufferBindingStack[GL_ELEMENT_ARRAY_BUFFER].push_back( 0 );
	 
	mReadFramebufferStack.push_back( 0 );
	mDrawFramebufferStack.push_back( 0 );	
#elif defined( CINDER_GL_ANGLE )
	mFramebufferStack.push_back( 0 );
#endif
	mDefaultArrayVboIdx = 0;

	// initial state for depth mask is enabled
	mBoolStateStack[GL_DEPTH_WRITEMASK] = vector<GLboolean>();
	mBoolStateStack[GL_DEPTH_WRITEMASK].push_back( GL_TRUE );
	mActiveTextureStack.push_back( 0 );

	mImmediateMode = gl::VertBatch::create();
	
	GLint params[4];
	glGetIntegerv( GL_VIEWPORT, params );
	mViewportStack.push_back( std::pair<Vec2i, Vec2i>( Vec2i( params[ 0 ], params[ 1 ] ), Vec2i( params[ 2 ], params[ 3 ] ) ) );
    
	glGetIntegerv( GL_SCISSOR_BOX, params );
	mScissorStack.push_back( std::pair<Vec2i, Vec2i>( Vec2i( params[ 0 ], params[ 1 ] ), Vec2i( params[ 2 ], params[ 3 ] ) ) );

	GLint queriedInt;
	glGetIntegerv( GL_BLEND_SRC_RGB, &queriedInt );
	mBlendSrcRgbStack.push_back( queriedInt );
	glGetIntegerv( GL_BLEND_DST_RGB, &queriedInt );
	mBlendDstRgbStack.push_back( queriedInt );
	glGetIntegerv( GL_BLEND_SRC_ALPHA, &queriedInt );
	mBlendSrcAlphaStack.push_back( queriedInt );
	glGetIntegerv( GL_BLEND_DST_ALPHA, &queriedInt );
	mBlendDstAlphaStack.push_back( queriedInt );
    
    mModelMatrixStack.push_back( Matrix44f() );
	mModelMatrixStack.back().setToIdentity();
    mViewMatrixStack.push_back( Matrix44f() );
	mViewMatrixStack.back().setToIdentity();
	mProjectionMatrixStack.push_back( Matrix44f() );
	mProjectionMatrixStack.back().setToIdentity();
	mGlslProgStack.push_back( GlslProgRef() );

	// set default shader
	pushGlslProg( getStockShader( ShaderDef().color() ) );
	
	// debug context
#if ! defined( CINDER_GL_ES ) && defined( CINDER_MSW )
	if( mPlatformData->mDebug ) {
		mDebugLogSeverity = mPlatformData->mDebugLogSeverity;
		mDebugBreakSeverity = mPlatformData->mDebugBreakSeverity;
		if( (mDebugLogSeverity > 0) || (mDebugBreakSeverity > 0) ) {
			//setBoolState( GL_DEBUG_OUTPUT_SYNCHRONOUS, GL_TRUE );
			glEnable( GL_DEBUG_OUTPUT_SYNCHRONOUS );
			glDebugMessageCallback( (GLDEBUGPROC)debugMessageCallback, this );
		}
	}
#endif
}

Context::~Context()
{
	if( getCurrent() == this ) {
		env()->makeContextCurrent( nullptr );

	#if defined( CINDER_COCOA )
		pthread_setspecific( sThreadSpecificCurrentContextKey, NULL );
	#else
		sThreadSpecificCurrentContext = (Context*)( nullptr );
	#endif
	}
}

ContextRef Context::create( const Context *sharedContext )
{
	return env()->createSharedContext( sharedContext );

}

ContextRef Context::createFromExisting( const std::shared_ptr<PlatformData> &platformData )
{
	env()->initializeFunctionPointers();
	ContextRef result( std::shared_ptr<Context>( new Context( platformData ) ) );

	return result;
}

void Context::makeCurrent() const
{
#if defined( CINDER_COCOA )
	if( ! sThreadSpecificCurrentContextInitialized ) {
		pthread_key_create( &sThreadSpecificCurrentContextKey, NULL );
		sThreadSpecificCurrentContextInitialized = true;
	}
	if( pthread_getspecific( sThreadSpecificCurrentContextKey ) != this ) {
	pthread_setspecific( sThreadSpecificCurrentContextKey, this );
		env()->makeContextCurrent( this );
	}
#else
	if( sThreadSpecificCurrentContext != this ) {
	sThreadSpecificCurrentContext = const_cast<Context*>( this );
		env()->makeContextCurrent( this );
	}
#endif
}

Context* Context::getCurrent()
{
#if defined( CINDER_COCOA )
	if( ! sThreadSpecificCurrentContextInitialized ) {
		return nullptr;
	}
	return reinterpret_cast<Context*>( pthread_getspecific( sThreadSpecificCurrentContextKey ) );
#else
	return sThreadSpecificCurrentContext;
#endif
}

void Context::reflectCurrent( Context *context )
{
#if defined( CINDER_COCOA )
	if( ! sThreadSpecificCurrentContextInitialized ) {
		pthread_key_create( &sThreadSpecificCurrentContextKey, NULL );
		sThreadSpecificCurrentContextInitialized = true;
	}
	pthread_setspecific( sThreadSpecificCurrentContextKey, context );
#else
	sThreadSpecificCurrentContext = context;
#endif	
}

//////////////////////////////////////////////////////////////////
// VAO
void Context::bindVao( const VaoRef &vao )
{
	VaoRef prevVao = getVao();
	if( setStackState( mVaoStack, vao ) ) {
		if( prevVao )
			prevVao->unbindImpl( this );
		if( vao )
			vao->bindImpl( this );	
	}
}

void Context::pushVao( const VaoRef &vao )
{
	VaoRef prevVao = getVao();
	if( pushStackState( mVaoStack, vao ) ) {
		if( prevVao )
			prevVao->unbindImpl( this );
		if( vao )
			vao->bindImpl( this );
	}
}

void Context::pushVao()
{
	mVaoStack.push_back( getVao() );
}

void Context::popVao()
{
	VaoRef prevVao = getVao();

	if( ! mVaoStack.empty() ) {
		mVaoStack.pop_back();
		if( ! mVaoStack.empty() ) {
			if( prevVao != mVaoStack.back() ) {
				if( prevVao )
					prevVao->unbindImpl( this );
				if( mVaoStack.back() )
					mVaoStack.back()->bindImpl( this );
			}
		}
		else {
			if( prevVao )
				prevVao->unbindImpl( this );
		}
	}
}

VaoRef Context::getVao()
{
	if( ! mVaoStack.empty() )
		return mVaoStack.back();
	else
		return VaoRef();
}

void Context::restoreInvalidatedVao()
{
	if( ! mVaoStack.empty() )
		mVaoStack.back()->bindImpl( this );
}

void Context::vaoCreated( const Vao *vao )
{
	if( mObjectTrackingEnabled )
		mLiveVaos.insert( vao );
}

void Context::vaoDeleted( const Vao *vao )
{
	// remove from object tracking
	if( mObjectTrackingEnabled )
		mLiveVaos.erase( vao );
		
	// if this was the currently bound VAO, mark the top of the stack as null
	if( ! mVaoStack.empty() && mVaoStack.back()->getId() == vao->getId() )
		mVaoStack.back() = nullptr;
}

//////////////////////////////////////////////////////////////////
// Viewport
void Context::viewport( const std::pair<Vec2i, Vec2i> &viewport )
{
	if( setStackState( mViewportStack, viewport ) )
		glViewport( viewport.first.x, viewport.first.y, viewport.second.x, viewport.second.y );
}

void Context::pushViewport( const std::pair<Vec2i, Vec2i> &viewport )
{
	if( pushStackState( mViewportStack, viewport ) )
		glViewport( viewport.first.x, viewport.first.y, viewport.second.x, viewport.second.y );
}

void Context::pushViewport()
{
	mViewportStack.push_back( getViewport() );
}

void Context::popViewport()
{
	if( mViewportStack.empty() || popStackState( mViewportStack ) ) {
		auto viewport = getViewport();
		glViewport( viewport.first.x, viewport.first.y, viewport.second.x, viewport.second.y );
	}
}

std::pair<Vec2i, Vec2i> Context::getViewport()
{
	if( mViewportStack.empty() ) {
		GLint params[4];
		glGetIntegerv( GL_VIEWPORT, params );
		// push twice in anticipation of later pop
		mViewportStack.push_back( std::pair<Vec2i, Vec2i>( Vec2i( params[0], params[1] ), Vec2i( params[2], params[3] ) ) );
		mViewportStack.push_back( std::pair<Vec2i, Vec2i>( Vec2i( params[0], params[1] ), Vec2i( params[2], params[3] ) ) );
	}

	return mViewportStack.back();
}
    
//////////////////////////////////////////////////////////////////
// Scissor Test
void Context::setScissor( const std::pair<Vec2i, Vec2i> &scissor )
{
	if( setStackState( mScissorStack, scissor ) )
		glScissor( scissor.first.x, scissor.first.y, scissor.second.x, scissor.second.y );
}

void Context::pushScissor( const std::pair<Vec2i, Vec2i> &scissor )
{
	if( pushStackState( mScissorStack, scissor ) )
		glScissor( scissor.first.x, scissor.first.y, scissor.second.x, scissor.second.y );
}

void Context::pushScissor()
{
	mScissorStack.push_back( getScissor() );
}

//! Sets the active texture unit; expects values relative to \c 0, \em not GL_TEXTURE0
void Context::popScissor()
{
	if( mScissorStack.empty() || popStackState( mScissorStack ) ) {
		auto scissor = getScissor();
		glScissor( scissor.first.x, scissor.first.y, scissor.second.x, scissor.second.y );
	}
}

std::pair<Vec2i, Vec2i> Context::getScissor()
{
	if( mScissorStack.empty() ) {
		GLint params[4];
		glGetIntegerv( GL_SCISSOR_BOX, params ); 
		// push twice in anticipation of later pop
		mScissorStack.push_back( std::pair<Vec2i, Vec2i>( Vec2i( params[ 0 ], params[ 1 ] ), Vec2i( params[ 2 ], params[ 3 ] ) ) );
		mScissorStack.push_back( std::pair<Vec2i, Vec2i>( Vec2i( params[ 0 ], params[ 1 ] ), Vec2i( params[ 2 ], params[ 3 ] ) ) );
	}

	return mScissorStack.back();
}

//////////////////////////////////////////////////////////////////
// Buffer
void Context::bindBuffer( GLenum target, GLuint id )
{
	GLuint prevValue = getBufferBinding( target );
	if( prevValue != id ) {
		mBufferBindingStack[target].back() = id;
		if( target == GL_ARRAY_BUFFER || target == GL_ELEMENT_ARRAY_BUFFER ) {
			VaoRef vao = getVao();
			if( vao )
				vao->reflectBindBufferImpl( target, id );
			else
				glBindBuffer( target, id );
		}
		else
			glBindBuffer( target, id );
	}
}

void Context::pushBufferBinding( GLenum target, GLuint id )
{
	pushBufferBinding( target );
	bindBuffer( target, id );
}

void Context::pushBufferBinding( GLenum target )
{
	GLuint curValue = getBufferBinding( target );
	mBufferBindingStack[target].push_back( curValue );
}

void Context::popBufferBinding( GLenum target )
{
	GLuint prevValue = getBufferBinding( target );
	auto cachedIt = mBufferBindingStack.find( target );
	cachedIt->second.pop_back();
	if( ! cachedIt->second.empty() && cachedIt->second.back() != prevValue ) {
		if( target == GL_ARRAY_BUFFER || target == GL_ELEMENT_ARRAY_BUFFER ) {
			VaoRef vao = getVao();
			if( vao )
				vao->reflectBindBufferImpl( target, cachedIt->second.back() );
			else
				glBindBuffer( target, cachedIt->second.back() );
		}
		else
			glBindBuffer( target, cachedIt->second.back() );
	}
}

GLuint Context::getBufferBinding( GLenum target )
{
	auto cachedIt = mBufferBindingStack.find( target );
	if( (cachedIt == mBufferBindingStack.end()) || ( cachedIt->second.empty() ) || ( cachedIt->second.back() == -1 ) ) {
		GLint queriedInt = 0;
		GLenum targetBinding = BufferObj::getBindingConstantForTarget( target );
		if( targetBinding > 0 ) {
			glGetIntegerv( targetBinding, &queriedInt );
		}
		else
			return 0; // warning?
		
		if( mBufferBindingStack[target].empty() ) { // bad - empty stack; push twice to allow for the pop later and not lead to an empty stack
			mBufferBindingStack[target] = vector<GLint>();
			mBufferBindingStack[target].push_back( queriedInt );
			mBufferBindingStack[target].push_back( queriedInt );			
		}
		else
			mBufferBindingStack[target].back() = queriedInt;
		return (GLuint)queriedInt;
	}
	else
		return (GLuint)cachedIt->second.back();
}

void Context::reflectBufferBinding( GLenum target, GLuint id )
{
	// first time we've met this target; start an empty stack
	if( mBufferBindingStack.find(target) == mBufferBindingStack.end() ) {
		mBufferBindingStack[target] = vector<int>();
		mBufferBindingStack[target].push_back( id );
	}
	else if( ! mBufferBindingStack[target].empty() )
		mBufferBindingStack[target].back() = id;
}

void Context::bufferCreated( const BufferObj *buffer )
{
	if( mObjectTrackingEnabled )
		mLiveBuffers.insert( buffer );
}

void Context::bufferDeleted( const BufferObj *buffer )
{
	// remove from object tracking
	if( mObjectTrackingEnabled )
		mLiveBuffers.erase( buffer );

	auto target = buffer->getTarget();

	// if 'id' was bound to 'target', mark 'target's binding as 0
	auto existingIt = mBufferBindingStack.find( target );
	if( existingIt != mBufferBindingStack.end() ) {
		if( mBufferBindingStack[target].back() == buffer->getId() )
			mBufferBindingStack[target].back() = 0;
			// alert the currently bound VAO
			if( target == GL_ARRAY_BUFFER || target == GL_ELEMENT_ARRAY_BUFFER ) {
				VaoRef vao = getVao();
				if( vao )
					vao->reflectBindBufferImpl( target, 0 );
	}
	}
	else
		mBufferBindingStack[target].push_back( 0 );
}

void Context::invalidateBufferBindingCache( GLenum target )
{
	if( mBufferBindingStack.find(target) == mBufferBindingStack.end() ) {
		mBufferBindingStack[target] = vector<int>();
		mBufferBindingStack[target].push_back( -1 );
	}
	else if( ! mBufferBindingStack[target].empty() )
		mBufferBindingStack[target].back() = -1;
}
	
void Context::restoreInvalidatedBufferBinding( GLenum target )
{
	if( mBufferBindingStack.find(target) != mBufferBindingStack.end() ) {
		if( ! mBufferBindingStack[target].empty() )
			glBindBuffer( target, mBufferBindingStack[target].back() );
	}
}

#if ! defined( CINDER_GL_ES )
void Context::bindBufferBase( GLenum target, int index, const BufferObjRef &buffer )
{
	switch (target) {
		case GL_TRANSFORM_FEEDBACK_BUFFER: {
			if( mCachedTransformFeedbackObj ) {
				mCachedTransformFeedbackObj->setIndex( index, buffer );
			}
			else {
				glBindBufferBase( target, index, buffer->getId() );
			}
		}
		break;
		case GL_UNIFORM_BUFFER: {
			// Soon to implement
		}
		break;
		default:
		break;
	}
}
	
//////////////////////////////////////////////////////////////////
// TransformFeedbackObj
void Context::bindTransformFeedbackObj( const TransformFeedbackObjRef &feedbackObj )
{
	if( feedbackObj != mCachedTransformFeedbackObj ) {
		if( mCachedTransformFeedbackObj )
			mCachedTransformFeedbackObj->unbindImpl( this );
		if( feedbackObj )
			feedbackObj->bindImpl( this );
		
		mCachedTransformFeedbackObj = feedbackObj;
	}
}

TransformFeedbackObjRef Context::transformFeedbackObjGet()
{
	return mCachedTransformFeedbackObj;
}
	
void Context::beginTransformFeedback( GLenum primitiveMode )
{
	if( mCachedTransformFeedbackObj ) {
		mCachedTransformFeedbackObj->begin( primitiveMode );
	}
	else {
		glBeginTransformFeedback( primitiveMode );
	}
}

void Context::pauseTransformFeedback()
{
	if( mCachedTransformFeedbackObj ) {
		mCachedTransformFeedbackObj->pause();
	}
	else {
		glPauseTransformFeedback();
	}
}

void Context::resumeTransformFeedback()
{
	if( mCachedTransformFeedbackObj ) {
		mCachedTransformFeedbackObj->resume();
	}
	else {
		glResumeTransformFeedback();
	}
}

void Context::endTransformFeedback()
{
	if( mCachedTransformFeedbackObj ) {
		mCachedTransformFeedbackObj->end();
	}
	else {
		glEndTransformFeedback();
	}
}
#endif

//////////////////////////////////////////////////////////////////
// Shader
void Context::pushGlslProg( const GlslProgRef &prog )
{
	GlslProgRef prevGlsl = getGlslProg();

	mGlslProgStack.push_back( prog );
	if( prog != prevGlsl ) {
		if( prog )
			prog->bindImpl();
		else
			glUseProgram( 0 );
	}
}

void Context::pushGlslProg()
{
	mGlslProgStack.push_back( getGlslProg() );
}

void Context::popGlslProg()
{
	GlslProgRef prevGlsl = getGlslProg();

	if( ! mGlslProgStack.empty() ) {
		mGlslProgStack.pop_back();
		if( ! mGlslProgStack.empty() ) {
			if( prevGlsl != mGlslProgStack.back() ) {
				if( mGlslProgStack.back() )
					mGlslProgStack.back()->bindImpl();
				else
					glUseProgram( 0 );
			}
		}
	}
}

void Context::bindGlslProg( const GlslProgRef &prog )
{
	if( mGlslProgStack.empty() || (mGlslProgStack.back() != prog) ) {
		if( ! mGlslProgStack.empty() )
			mGlslProgStack.back() = prog;
		if( prog )
			prog->bindImpl();
		else
			glUseProgram( 0 );
	}
}

GlslProgRef Context::getGlslProg()
{
	if( mGlslProgStack.empty() )
		mGlslProgStack.push_back( GlslProgRef() );
	
	return mGlslProgStack.back();
}

void Context::glslProgCreated( const GlslProg *glslProg )
{
	if( mObjectTrackingEnabled )
		mLiveGlslProgs.insert( glslProg );
}

void Context::glslProgDeleted( const GlslProg *glslProg )
{
	if( mObjectTrackingEnabled )
		mLiveGlslProgs.erase( glslProg );
}

//////////////////////////////////////////////////////////////////
// TextureBinding
void Context::bindTexture( GLenum target, GLuint textureId )
{
	bindTexture( target, textureId, getActiveTexture() );
}

void Context::bindTexture( GLenum target, GLuint textureId, uint8_t textureUnit )
{
	if( mTextureBindingStack.find( textureUnit ) == mTextureBindingStack.end() )
		mTextureBindingStack[textureUnit] = std::map<GLenum,std::vector<GLint>>();

	GLuint prevValue = getTextureBinding( target, textureUnit );
	if( prevValue != textureId ) {
		mTextureBindingStack[textureUnit][target].back() = textureId;
		ScopedActiveTexture actScp( textureUnit );
		glBindTexture( target, textureId );
	}
}

void Context::pushTextureBinding( GLenum target, uint8_t textureUnit )
{
	if( mTextureBindingStack.find( textureUnit ) == mTextureBindingStack.end() ) {
		mTextureBindingStack[textureUnit] = std::map<GLenum,std::vector<GLint>>();
		GLenum targetBinding = Texture::getBindingConstantForTarget( target );
		GLint queriedInt = -1;
		if( targetBinding > 0 ) {
			ScopedActiveTexture actScp( textureUnit );
			glGetIntegerv( targetBinding, &queriedInt );
		}
		mTextureBindingStack[textureUnit][target].push_back( queriedInt );
	}
	else if( mTextureBindingStack[textureUnit].find( target ) == mTextureBindingStack[textureUnit].end() ) {
		mTextureBindingStack[textureUnit][target] = std::vector<GLint>();
		GLenum targetBinding = Texture::getBindingConstantForTarget( target );
		GLint queriedInt = -1;
		if( targetBinding > 0 ) {
			ScopedActiveTexture actScp( textureUnit );
			glGetIntegerv( targetBinding, &queriedInt );
		}
		mTextureBindingStack[textureUnit][target].push_back( queriedInt );
	}
	
	mTextureBindingStack[textureUnit][target].push_back( mTextureBindingStack[textureUnit][target].back() );
}

void Context::pushTextureBinding( GLenum target, GLuint textureId, uint8_t textureUnit )
{
	pushTextureBinding( target, textureUnit );
	bindTexture( target, textureId, textureUnit );
}

void Context::popTextureBinding( GLenum target, uint8_t textureUnit )
{
	if( mTextureBindingStack.find( textureUnit ) == mTextureBindingStack.end() )
		mTextureBindingStack[textureUnit] = std::map<GLenum,std::vector<GLint>>();

	auto cached = mTextureBindingStack[textureUnit].find( target );
	if( ( cached != mTextureBindingStack[textureUnit].end() ) && ( ! cached->second.empty() ) ) {
		GLint prevValue = cached->second.back();
		cached->second.pop_back();
		if( ! cached->second.empty() ) {
			if( cached->second.back() != prevValue ) {
				ScopedActiveTexture actScp( textureUnit );
				glBindTexture( target, cached->second.back() );
		}
	}
	}
}

GLuint Context::getTextureBinding( GLenum target, uint8_t textureUnit )
{
	if( mTextureBindingStack.find( textureUnit ) == mTextureBindingStack.end() )
		mTextureBindingStack[textureUnit] = std::map<GLenum,std::vector<GLint>>();

	auto cachedIt = mTextureBindingStack[textureUnit].find( target );
	if( (cachedIt == mTextureBindingStack[textureUnit].end()) || ( cachedIt->second.empty() ) || ( cachedIt->second.back() == -1 ) ) {
		GLint queriedInt = 0;
		GLenum targetBinding = Texture::getBindingConstantForTarget( target );
		if( targetBinding > 0 ) {
			ScopedActiveTexture actScp( textureUnit );
			glGetIntegerv( targetBinding, &queriedInt );
		}
		else
			return 0; // warning?
		
		if( mTextureBindingStack[textureUnit][target].empty() ) { // bad - empty stack; push twice to allow for the pop later and not lead to an empty stack
			mTextureBindingStack[textureUnit][target] = vector<GLint>();
			mTextureBindingStack[textureUnit][target].push_back( queriedInt );
			mTextureBindingStack[textureUnit][target].push_back( queriedInt );			
		}
		else
			mTextureBindingStack[textureUnit][target].back() = queriedInt;
		return (GLuint)queriedInt;
	}
	else
		return (GLuint)cachedIt->second.back();
}

void Context::textureCreated( const TextureBase *texture )
{
	if( mObjectTrackingEnabled )
		mLiveTextures.insert( texture );
}

void Context::textureDeleted( const TextureBase *texture )
{
	GLenum target = texture->getTarget();
	GLuint textureId = texture->getId();

	// remove from object tracking
	if( mObjectTrackingEnabled )
		mLiveTextures.erase( texture );

	for( auto &unit : mTextureBindingStack ) {
		auto cachedIt = unit.second.find( target );
		if( cachedIt != unit.second.end() && ( ! cachedIt->second.empty() ) && ( cachedIt->second.back() != textureId ) ) {
		// GL will have set the binding to 0 for target, so let's do the same
			unit.second[target].back() = 0;
	}
	}
}

//////////////////////////////////////////////////////////////////
// ActiveTexture
void Context::setActiveTexture( uint8_t textureUnit )
{
	if( setStackState<uint8_t>( mActiveTextureStack, textureUnit ) )
		glActiveTexture( GL_TEXTURE0 + textureUnit );
}

void Context::pushActiveTexture( uint8_t textureUnit )
{
	if( pushStackState<uint8_t>( mActiveTextureStack, textureUnit ) )
		glActiveTexture( GL_TEXTURE0 + textureUnit );
}

void Context::pushActiveTexture()
{
	pushStackState<uint8_t>( mActiveTextureStack, getActiveTexture() );
}

//! Sets the active texture unit; expects values relative to \c 0, \em not GL_TEXTURE0
void Context::popActiveTexture()
{
	if( mActiveTextureStack.empty() || popStackState<uint8_t>( mActiveTextureStack ) )
		glActiveTexture( GL_TEXTURE0 + getActiveTexture() );
}

uint8_t Context::getActiveTexture()
{
	if( mActiveTextureStack.empty() ) {
		GLint queriedInt;
		glGetIntegerv( GL_ACTIVE_TEXTURE, &queriedInt );
		// push twice to account for inevitable pop to follow
		mActiveTextureStack.push_back( queriedInt - GL_TEXTURE0 );
		mActiveTextureStack.push_back( queriedInt - GL_TEXTURE0 );
	}

	return mActiveTextureStack.back();
}

//////////////////////////////////////////////////////////////////
// Framebuffers
void Context::bindFramebuffer( GLenum target, GLuint framebuffer )
{
#if ! defined( SUPPORTS_FBO_MULTISAMPLING )
	if( target == GL_FRAMEBUFFER ) {
		if( setStackState<GLint>( mFramebufferStack, framebuffer ) )
			glBindFramebuffer( target, framebuffer );
	}
	else {
		//throw gl::Exception( "Illegal target for Context::bindFramebuffer" );	
	}
#else
	if( target == GL_FRAMEBUFFER ) {
		bool readRequiresBind = setStackState<GLint>( mReadFramebufferStack, framebuffer );
		bool drawRequiresBind = setStackState<GLint>( mDrawFramebufferStack, framebuffer );
		if( readRequiresBind || drawRequiresBind )
			glBindFramebuffer( GL_FRAMEBUFFER, framebuffer );
	}
	else if( target == GL_READ_FRAMEBUFFER ) {
		if( setStackState<GLint>( mReadFramebufferStack, framebuffer ) )
			glBindFramebuffer( target, framebuffer );
	}
	else if( target == GL_DRAW_FRAMEBUFFER ) {
		if( setStackState<GLint>( mDrawFramebufferStack, framebuffer ) )
			glBindFramebuffer( target, framebuffer );		
	}
	else {
		//throw gl::Exception( "Illegal target for Context::bindFramebuffer" );	
	}
#endif
}

void Context::bindFramebuffer( const FboRef &fbo, GLenum target )
{
	bindFramebuffer( target, fbo->getId() );
	fbo->markAsDirty();
}

void Context::unbindFramebuffer()
{
	bindFramebuffer( GL_FRAMEBUFFER, 0 );
}

void Context::pushFramebuffer( const FboRef &fbo, GLenum target )
{
	pushFramebuffer( target, fbo->getId() );
	fbo->markAsDirty();	
}

void Context::pushFramebuffer( GLenum target, GLuint framebuffer )
{
#if ! defined( SUPPORTS_FBO_MULTISAMPLING )
	if( pushStackState<GLint>( mFramebufferStack, framebuffer ) )
		glBindFramebuffer( target, framebuffer );
#else
	if( target == GL_FRAMEBUFFER || target == GL_READ_FRAMEBUFFER ) {
		if( pushStackState<GLint>( mReadFramebufferStack, framebuffer ) )
			glBindFramebuffer( GL_READ_FRAMEBUFFER, framebuffer );
	}
	if( target == GL_FRAMEBUFFER || target == GL_DRAW_FRAMEBUFFER ) {
		if( pushStackState<GLint>( mDrawFramebufferStack, framebuffer ) )
			glBindFramebuffer( GL_DRAW_FRAMEBUFFER, framebuffer );	
	}
#endif
}

void Context::pushFramebuffer( GLenum target )
{
#if ! defined( SUPPORTS_FBO_MULTISAMPLING )
	pushStackState<GLint>( mFramebufferStack, getFramebuffer( target ) );
#else
	if( target == GL_FRAMEBUFFER || target == GL_READ_FRAMEBUFFER ) {
		pushStackState<GLint>( mReadFramebufferStack, getFramebuffer( GL_READ_FRAMEBUFFER ) );
	}
	if( target == GL_FRAMEBUFFER || target == GL_DRAW_FRAMEBUFFER ) {
		pushStackState<GLint>( mDrawFramebufferStack, getFramebuffer( GL_DRAW_FRAMEBUFFER ) );
	}
#endif
}

void Context::popFramebuffer( GLenum target )
{
#if ! defined( SUPPORTS_FBO_MULTISAMPLING )
	if( popStackState<GLint>( mFramebufferStack ) )
		if( ! mFramebufferStack.empty() )
			glBindFramebuffer( target, mFramebufferStack.back() );
#else
	if( target == GL_FRAMEBUFFER || target == GL_READ_FRAMEBUFFER ) {
		if( popStackState<GLint>( mReadFramebufferStack ) )
			if( ! mReadFramebufferStack.empty() )
				glBindFramebuffer( target, mReadFramebufferStack.back() );
	}
	if( target == GL_FRAMEBUFFER || target == GL_DRAW_FRAMEBUFFER ) {
		if( popStackState<GLint>( mDrawFramebufferStack ) )
			if( ! mDrawFramebufferStack.empty() )
				glBindFramebuffer( target, mDrawFramebufferStack.back() );
	}
#endif
}

GLuint Context::getFramebuffer( GLenum target )
{
#if ! defined( SUPPORTS_FBO_MULTISAMPLING )
	if( mFramebufferStack.empty() ) {
		GLint queriedInt;
		glGetIntegerv( GL_FRAMEBUFFER_BINDING, &queriedInt );
		mFramebufferStack.push_back( queriedInt );
		mFramebufferStack.push_back( queriedInt );		
	}
	
	return mFramebufferStack.back();
#else
	if( target == GL_READ_FRAMEBUFFER ) {
		if( mReadFramebufferStack.empty() ) {
			GLint queriedInt;
			glGetIntegerv( GL_READ_FRAMEBUFFER_BINDING, &queriedInt );
			// push twice for pop to follow
			mReadFramebufferStack.push_back( queriedInt );
			mReadFramebufferStack.push_back( queriedInt );
		}
		return (GLuint)mReadFramebufferStack.back();
	}
	else if( target == GL_DRAW_FRAMEBUFFER || target == GL_FRAMEBUFFER ) {
		if( mDrawFramebufferStack.empty() ) {
			GLint queriedInt;
			glGetIntegerv( GL_DRAW_FRAMEBUFFER_BINDING, &queriedInt );
			mDrawFramebufferStack.push_back( queriedInt );
			mDrawFramebufferStack.push_back( queriedInt );
		}
		return (GLuint)mDrawFramebufferStack.back();
	}
	else {
		//throw gl::Exception( "Illegal target for getFramebufferBinding" );
		return 0; // 
	}
#endif
}

void Context::framebufferCreated( const Fbo *fbo )
{
	if( mObjectTrackingEnabled )
		mLiveFbos.insert( fbo );
}

void Context::framebufferDeleted( const Fbo *fbo )
{
	// remove from object tracking
	if( mObjectTrackingEnabled )
		mLiveFbos.erase( fbo );
}

//////////////////////////////////////////////////////////////////
// States
void Context::setBoolState( GLenum cap, GLboolean value )
{
	bool needsToBeSet = true;
	auto cached = mBoolStateStack.find( cap );
	if( ( cached != mBoolStateStack.end() ) && ( ! cached->second.empty() ) && ( cached->second.back() == value ) )
		needsToBeSet = false;
	if( cached == mBoolStateStack.end() ) {
		mBoolStateStack[cap] = vector<GLboolean>();
		mBoolStateStack[cap].push_back( value );
	}
	else
		mBoolStateStack[cap].back() = value;
	if( needsToBeSet ) {
		if( value )
			glEnable( cap );
		else
			glDisable( cap );
	}	
}

void Context::setBoolState( GLenum cap, GLboolean value, const std::function<void(GLboolean)> &setter )
{
	bool needsToBeSet = true;
	auto cached = mBoolStateStack.find( cap );
	if( ( cached != mBoolStateStack.end() ) && ( ! cached->second.empty() ) && ( cached->second.back() == value ) )
		needsToBeSet = false;
	if( cached == mBoolStateStack.end() ) {
		mBoolStateStack[cap] = vector<GLboolean>();
		mBoolStateStack[cap].push_back( value );
	}
	else
		mBoolStateStack[cap].back() = value;
	if( needsToBeSet )
		setter( value );
}

void Context::pushBoolState( GLenum cap, GLboolean value )
{
	bool needsToBeSet = true;
	auto cached = mBoolStateStack.find( cap );
	if( ( cached != mBoolStateStack.end() ) && ( ! cached->second.empty() ) && ( cached->second.back() == value ) )
		needsToBeSet = false;
	else if( cached == mBoolStateStack.end() ) {
		mBoolStateStack[cap] = vector<GLboolean>();
		mBoolStateStack[cap].push_back( glIsEnabled( cap ) );
	}
	mBoolStateStack[cap].push_back( value );
	if( needsToBeSet ) {
		if( value )
			glEnable( cap );
		else
			glDisable( cap );
	}	
}

void Context::pushBoolState( GLenum cap )
{
	GLboolean existingVal = getBoolState( cap );
	mBoolStateStack[cap].push_back( existingVal );
}

void Context::popBoolState( GLenum cap )
{
	auto cached = mBoolStateStack.find( cap );
	if( ( cached != mBoolStateStack.end() ) && ( ! cached->second.empty() ) ) {
		GLboolean prevValue = cached->second.back();
		cached->second.pop_back();
		if( ! cached->second.empty() ) {
			if( cached->second.back() != prevValue ) {
				if( cached->second.back() )
					glEnable( cap );
				else
					glDisable( cap );
			}
		}
	}
}

void Context::enable( GLenum cap, GLboolean value )
{
	setBoolState( cap, value );
}

GLboolean Context::getBoolState( GLenum cap )
{
	auto cached = mBoolStateStack.find( cap );
	if( ( cached == mBoolStateStack.end() ) || cached->second.empty() ) {
		GLboolean result = glIsEnabled( cap );
		if( cached == mBoolStateStack.end() )
			mBoolStateStack[cap] = vector<GLboolean>();
		// push twice to accommodate later pop
		mBoolStateStack[cap].push_back( result );
		mBoolStateStack[cap].push_back( result );
		return result;
	}
	else
		return cached->second.back();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// BlendFunc
void Context::blendFunc( GLenum sfactor, GLenum dfactor )
{
	blendFuncSeparate( sfactor, dfactor, sfactor, dfactor );
}

void Context::blendFuncSeparate( GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha )
{
	bool needsChange = setStackState<GLint>( mBlendSrcRgbStack, srcRGB );
	needsChange = setStackState<GLint>( mBlendDstRgbStack, dstRGB ) || needsChange;
	needsChange = setStackState<GLint>( mBlendSrcAlphaStack, srcAlpha ) || needsChange;
	needsChange = setStackState<GLint>( mBlendDstAlphaStack, dstAlpha ) || needsChange;
	if( needsChange )
		glBlendFuncSeparate( srcRGB, dstRGB, srcAlpha, dstAlpha );
}

void Context::pushBlendFuncSeparate( GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha )
{
	bool needsChange = pushStackState<GLint>( mBlendSrcRgbStack, srcRGB );
	needsChange = pushStackState<GLint>( mBlendDstRgbStack, dstRGB ) || needsChange;
	needsChange = pushStackState<GLint>( mBlendSrcAlphaStack, srcAlpha ) || needsChange;
	needsChange = pushStackState<GLint>( mBlendDstAlphaStack, dstAlpha ) || needsChange;
	if( needsChange )
		glBlendFuncSeparate( srcRGB, dstRGB, srcAlpha, dstAlpha );
}

void Context::pushBlendFuncSeparate()
{
	GLenum resultSrcRGB, resultDstRGB, resultSrcAlpha, resultDstAlpha;
	getBlendFuncSeparate( &resultSrcRGB, &resultDstRGB, &resultSrcAlpha, &resultDstAlpha );

	mBlendSrcRgbStack.push_back( resultSrcRGB );
	mBlendDstRgbStack.push_back( resultDstRGB );
	mBlendSrcAlphaStack.push_back( resultSrcAlpha );
	mBlendDstAlphaStack.push_back( resultDstAlpha );
}

void Context::popBlendFuncSeparate()
{
	bool needsChange = popStackState<GLint>( mBlendSrcRgbStack );
	needsChange = popStackState<GLint>( mBlendDstRgbStack ) || needsChange;
	needsChange = popStackState<GLint>( mBlendSrcAlphaStack ) || needsChange;
	needsChange = popStackState<GLint>( mBlendDstAlphaStack ) || needsChange;
	if( needsChange && ( ! mBlendSrcRgbStack.empty() ) && ( ! mBlendSrcAlphaStack.empty() ) && ( ! mBlendDstRgbStack.empty() ) && ( ! mBlendDstAlphaStack.empty() ) )
		glBlendFuncSeparate( mBlendSrcRgbStack.back(), mBlendDstRgbStack.back(), mBlendSrcAlphaStack.back(), mBlendDstAlphaStack.back() );
}

void Context::getBlendFuncSeparate( GLenum *resultSrcRGB, GLenum *resultDstRGB, GLenum *resultSrcAlpha, GLenum *resultDstAlpha )
{
	// push twice on empty to accommodate inevitable push later
	GLint queriedInt;
	if( mBlendSrcRgbStack.empty() ) {
		glGetIntegerv( GL_BLEND_SRC_RGB, &queriedInt );
		mBlendSrcRgbStack.push_back( queriedInt ); mBlendSrcRgbStack.push_back( queriedInt );
	}
	if( mBlendDstRgbStack.empty() ) {
		glGetIntegerv( GL_BLEND_DST_RGB, &queriedInt );
		mBlendDstRgbStack.push_back( queriedInt ); mBlendDstRgbStack.push_back( queriedInt );
	}
	if( mBlendSrcAlphaStack.empty() ) {
		glGetIntegerv( GL_BLEND_SRC_ALPHA, &queriedInt );
		mBlendSrcAlphaStack.push_back( queriedInt ); mBlendSrcAlphaStack.push_back( queriedInt );
	}
	if( mBlendDstAlphaStack.empty() ) {
		glGetIntegerv( GL_BLEND_DST_ALPHA, &queriedInt );
		mBlendDstAlphaStack.push_back( queriedInt ); mBlendDstAlphaStack.push_back( queriedInt );
	}
	
	*resultSrcRGB = mBlendSrcRgbStack.back();
	*resultDstRGB = mBlendDstRgbStack.back();
	*resultSrcAlpha = mBlendSrcAlphaStack.back();
	*resultDstAlpha = mBlendDstAlphaStack.back();
}

///////////////////////////////////////////////////////////////////////////////////////////
// DepthMask
void Context::depthMask( GLboolean enable )
{
	setBoolState( GL_DEPTH_WRITEMASK, enable, glDepthMask );
}

///////////////////////////////////////////////////////////////////////////////////////////
// PolygonMode
#if ! defined( CINDER_GL_ES )
void Context::polygonMode( GLenum face, GLenum mode )
{
	if( face == GL_FRONT_AND_BACK ) {
		if( mCachedFrontPolygonMode != mode || mCachedBackPolygonMode != mode ) {
			mCachedFrontPolygonMode = mCachedBackPolygonMode = mode;
			glPolygonMode( GL_FRONT_AND_BACK, mode );
		}
	}
	else if( face == GL_FRONT ) {
		if( mCachedFrontPolygonMode != mode ) {
			mCachedFrontPolygonMode = mode;
			glPolygonMode( GL_FRONT, mode );
		}
	}
	else if( face == GL_BACK ) {
		if( mCachedBackPolygonMode != mode ) {
			mCachedBackPolygonMode = mode;
			glPolygonMode( GL_BACK, mode );
		}		
	}
}

GLenum Context::getPolygonMode( GLenum face ) const
{
	return face == GL_FRONT ? mCachedFrontPolygonMode : mCachedBackPolygonMode;
}
#endif // ! defined( CINDER_GL_ES )

//////////////////////////////////////////////////////////////////////////////////////////
// Templated stack management routines
template<typename T>
bool Context::pushStackState( std::vector<T> &stack, T value )
{
	bool needsToBeSet = true;
	if( ( ! stack.empty() ) && ( stack.back() == value ) )
		needsToBeSet = false;
	stack.push_back( value );
	return needsToBeSet;
}

template<typename T>
bool Context::popStackState( std::vector<T> &stack )
{
	if( ! stack.empty() ) {
		T prevValue = stack.back();
		stack.pop_back();
		if( ! stack.empty() )
			return stack.back() != prevValue;
		else
			return true;
	}
	else
		return true;
}

template<typename T>
bool Context::setStackState( std::vector<T> &stack, T value )
{
	bool needsToBeSet = true;
	if( ( ! stack.empty() ) && ( stack.back() == value ) )
		needsToBeSet = false;
	else if( stack.empty() )
		stack.push_back( value );
	else
		stack.back() = value;
	return needsToBeSet;
}

template<typename T>
bool Context::getStackState( std::vector<T> &stack, T *result )
{
	if( stack.empty() )
		return false;
	else {
		*result = stack.back();
		return true;
	}
}

/////////////////////////////////////////////////////////////////////////////////////
// This routine confirms that the ci::gl::Context's understanding of the world matches OpenGL's
void Context::sanityCheck()
{
	// assert cached (VAO) GL_VERTEX_ARRAY_BINDING is correct
	GLint trueVaoBinding;
#if defined( CINDER_GL_ES )
	glGetIntegerv( GL_VERTEX_ARRAY_BINDING_OES, &trueVaoBinding );
#else
	glGetIntegerv( GL_VERTEX_ARRAY_BINDING, &trueVaoBinding );
#endif

	VaoRef boundVao = getVao();
	if( boundVao ) {
		CI_ASSERT( trueVaoBinding == boundVao->mId );
		CI_ASSERT( getBufferBinding( GL_ARRAY_BUFFER ) == boundVao->getLayout().mCachedArrayBufferBinding );
		CI_ASSERT( getBufferBinding( GL_ELEMENT_ARRAY_BUFFER ) == boundVao->getLayout().mElementArrayBufferBinding );		
	}
	else
		assert( trueVaoBinding == 0 );

	// assert cached GL_ARRAY_BUFFER is correct
	GLint cachedArrayBufferBinding = getBufferBinding( GL_ARRAY_BUFFER );
	GLint trueArrayBufferBinding;
	glGetIntegerv( GL_ARRAY_BUFFER_BINDING, &trueArrayBufferBinding );
	CI_ASSERT( ( cachedArrayBufferBinding == -1 ) || ( trueArrayBufferBinding == cachedArrayBufferBinding ) );

	// assert cached GL_ELEMENT_ARRAY_BUFFER is correct
	GLint cachedElementArrayBufferBinding = getBufferBinding( GL_ELEMENT_ARRAY_BUFFER );
	GLint trueElementArrayBufferBinding;
	glGetIntegerv( GL_ELEMENT_ARRAY_BUFFER_BINDING, &trueElementArrayBufferBinding );
	assert( ( cachedElementArrayBufferBinding == -1 ) || ( trueElementArrayBufferBinding == cachedElementArrayBufferBinding ) );

#if defined( CINDER_MSW ) && ! defined( CINDER_GL_ANGLE )
	auto platformDataMsw = dynamic_pointer_cast<PlatformDataMsw>( context()->getPlatformData() );
	assert( platformDataMsw->mGlrc == wglGetCurrentContext() );
	assert( platformDataMsw->mDc == wglGetCurrentDC() );
#endif

	// assert that the (first 8) vertex attribute params are the same
	auto attribs = boundVao->getLayout().getVertexAttribs();
	for( int idx = 0; idx < std::min<int>( 8, boundVao->getLayout().getVertexAttribs().size() ); ++idx ) {
		GLint enabled;
		glGetVertexAttribiv( idx, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &enabled );
		int matchingIdx = -1;
		for( auto ciVaoAttribIt = attribs.begin(); ciVaoAttribIt != attribs.end(); ++ciVaoAttribIt ) {
			if( ciVaoAttribIt->first == idx ) {
				matchingIdx = ciVaoAttribIt - attribs.begin();
			}
		}
		if( matchingIdx == -1 ) {
			assert( enabled == 0 );
			continue;
		}
		assert( (enabled != 0) == attribs[idx].second.mEnabled );
		if( enabled ) {
			GLint arrayBufferBinding;
			glGetVertexAttribiv( idx, GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING, &arrayBufferBinding );
			assert( arrayBufferBinding == attribs[matchingIdx].second.mArrayBufferBinding );
			GLint arraySize;
			glGetVertexAttribiv( idx, GL_VERTEX_ATTRIB_ARRAY_SIZE, &arraySize );
			assert( arraySize == attribs[matchingIdx].second.mSize );
			GLint arrayType;
			glGetVertexAttribiv( idx, GL_VERTEX_ATTRIB_ARRAY_TYPE, &arrayType );
			assert( arrayType == attribs[matchingIdx].second.mType );
		}
	}

	// assert the various texture bindings are correct
/*	for( auto& cachedTextureBinding : mTextureBindingStack ) {
		GLenum target = cachedTextureBinding.first;
		glGetIntegerv( Texture::getBindingConstantForTarget( target ), &queriedInt );
		GLenum cachedTextureId = cachedTextureBinding.second.back();
		assert( queriedInt == cachedTextureId );
	}*/
}

void Context::printState( std::ostream &os ) const
{
	GLint queriedInt;
	
	glGetIntegerv( GL_ARRAY_BUFFER_BINDING, &queriedInt );	
	os << "{ARRAY_BUFFER:" << queriedInt << ", ";

	glGetIntegerv( GL_ELEMENT_ARRAY_BUFFER_BINDING, &queriedInt );
	os << "GL_ELEMENT_ARRAY_BUFFER:" << queriedInt << ", ";

#if defined( CINDER_GL_ES )
	glGetIntegerv( GL_VERTEX_ARRAY_BINDING_OES, &queriedInt );
#else
	glGetIntegerv( GL_VERTEX_ARRAY_BINDING, &queriedInt );
#endif
	os << "GL_VERTEX_ARRAY_BINDING:" << queriedInt << "}" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Vertex Attributes
void Context::enableVertexAttribArray( GLuint index )
{
	VaoRef vao = getVao();
	if( vao )
		vao->enableVertexAttribArrayImpl( index );
}

void Context::disableVertexAttribArray( GLuint index )
{
	VaoRef vao = getVao();
	if( vao )
		vao->disableVertexAttribArrayImpl( index );
}

void Context::vertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer )
{
	VaoRef vao = getVao();
	if( vao )
		vao->vertexAttribPointerImpl( index, size, type, normalized, stride, pointer );
}

#if ! defined( CINDER_GL_ES )
void Context::vertexAttribIPointer( GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer )
{
	VaoRef vao = getVao();
	if( vao )
		vao->vertexAttribIPointerImpl( index, size, type, stride, pointer );
}
#endif // ! defined( CINDER_GL_ES )

void Context::vertexAttribDivisor( GLuint index, GLuint divisor )
{
	VaoRef vao = getVao();
	if( vao )
		vao->vertexAttribDivisorImpl( index, divisor );
}

void Context::vertexAttrib1f( GLuint index, float v0 )
{
	glVertexAttrib1f( index, v0 );
}

void Context::vertexAttrib2f( GLuint index, float v0, float v1 )
{
	glVertexAttrib2f( index, v0, v1 );
}

void Context::vertexAttrib3f( GLuint index, float v0, float v1, float v2 )
{
	glVertexAttrib3f( index, v0, v1, v2 );
}

void Context::vertexAttrib4f( GLuint index, float v0, float v1, float v2, float v3 )
{
	glVertexAttrib4f( index, v0, v1, v2, v3 );
}

///////////////////////////////////////////////////////////////////////////////////////////
// draw*
void Context::drawArrays( GLenum mode, GLint first, GLsizei count )
{
	glDrawArrays( mode, first, count );
}

void Context::drawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices )
{
	glDrawElements( mode, count, type, indices );
}

#if ! defined( CINDER_GL_ES )
void Context::drawArraysInstanced( GLenum mode, GLint first, GLsizei count, GLsizei primcount )
{
	glDrawArraysInstanced( mode, first, count, primcount );
}

void Context::drawElementsInstanced( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei primcount )
{
	glDrawElementsInstanced( mode, count, type, indices, primcount );
}
#endif // ! defined( CINDER_GL_ES )

///////////////////////////////////////////////////////////////////////////////////////////
// Shaders
GlslProgRef	Context::getStockShader( const ShaderDef &shaderDef )
{
	auto existing = mStockShaders.find( shaderDef );
	if( existing == mStockShaders.end() ) {
		auto result = gl::env()->buildShader( shaderDef );
		mStockShaders[shaderDef] = result;
		return result;
	}
	else
		return existing->second;
}

void Context::setDefaultShaderVars()
{
	const auto &ctx = gl::context();
	const auto &glslProg = ctx->getGlslProg();
	if( glslProg ) {
		const auto &uniforms = glslProg->getUniformSemantics();
		for( const auto &unifIt : uniforms ) {
			switch( unifIt.second ) {
				case UNIFORM_MODEL_MATRIX:
					glslProg->uniform( unifIt.first, gl::getModelMatrix() ); break;
				case UNIFORM_MODEL_MATRIX_INVERSE:
					glslProg->uniform( unifIt.first, gl::getModelMatrix().inverted() ); break;
				case UNIFORM_MODEL_MATRIX_INVERSE_TRANSPOSE:
					glslProg->uniform( unifIt.first, gl::calcModelMatrixInverseTranspose() ); break;
				case UNIFORM_VIEW_MATRIX:
					glslProg->uniform( unifIt.first, gl::getViewMatrix() ); break;
				case UNIFORM_VIEW_MATRIX_INVERSE:
					glslProg->uniform( unifIt.first, gl::calcViewMatrixInverse() ); break;
				case UNIFORM_MODEL_VIEW:
					glslProg->uniform( unifIt.first, gl::getModelView() ); break;
				case UNIFORM_MODEL_VIEW_INVERSE:
					glslProg->uniform( unifIt.first, gl::getModelView().inverted() ); break;
				case UNIFORM_MODEL_VIEW_INVERSE_TRANSPOSE:
					glslProg->uniform( unifIt.first, gl::calcNormalMatrix() ); break;
				case UNIFORM_MODEL_VIEW_PROJECTION:
					glslProg->uniform( unifIt.first, gl::getModelViewProjection() ); break;
				case UNIFORM_MODEL_VIEW_PROJECTION_INVERSE:
					glslProg->uniform( unifIt.first, gl::getModelViewProjection().inverted() ); break;
				case UNIFORM_PROJECTION_MATRIX:
					glslProg->uniform( unifIt.first, gl::getProjectionMatrix() ); break;
				case UNIFORM_PROJECTION_MATRIX_INVERSE:
					glslProg->uniform( unifIt.first, gl::getProjectionMatrix().inverted() ); break;
				case UNIFORM_NORMAL_MATRIX:
					glslProg->uniform( unifIt.first, gl::calcNormalMatrix() ); break;
				case UNIFORM_VIEWPORT_MATRIX:
					glslProg->uniform( unifIt.first, gl::calcViewportMatrix() ); break;
				case UNIFORM_WINDOW_SIZE:
					glslProg->uniform( unifIt.first, app::getWindowSize() ); break;
				case UNIFORM_ELAPSED_SECONDS:
					glslProg->uniform( unifIt.first, float( app::getElapsedSeconds() ) ); break;
			}
		}

		const auto &attribs = glslProg->getAttribSemantics();
		for( const auto &attribIt : attribs ) {
			switch( attribIt.second ) {
				case geom::Attrib::COLOR: {
					int loc = glslProg->getAttribLocation( attribIt.first );
					ColorA c = ctx->getCurrentColor();
					gl::vertexAttrib4f( loc, c.r, c.g, c.b, c.a );
				}
				break;
				default:
					;
			}
		}
	}
}

VaoRef Context::getDefaultVao()
{
	if( ! mDefaultVao ) {
		mDefaultVao = Vao::create();
	}

	return mDefaultVao;
}

VboRef Context::getDrawTextureVbo()
{
	if( ! mDrawTextureVbo ) {
		allocateDrawTextureVboAndVao();
	}

	return mDrawTextureVbo;
}

VaoRef Context::getDrawTextureVao()
{
	if( ! mDrawTextureVao ) {
		allocateDrawTextureVboAndVao();
	}

	return mDrawTextureVao;
}

void Context::allocateDrawTextureVboAndVao()
{
	GLfloat data[8+8]; // both verts and texCoords
	GLfloat *verts = data, *texCoords = data + 8;

	verts[0*2+0] = 1.0f; texCoords[0*2+0] = 1.0f;
	verts[0*2+1] = 0.0f; texCoords[0*2+1] = 0.0f;
	verts[1*2+0] = 0.0f; texCoords[1*2+0] = 0.0f;
	verts[1*2+1] = 0.0f; texCoords[1*2+1] = 0.0f;
	verts[2*2+0] = 1.0f; texCoords[2*2+0] = 1.0f;
	verts[2*2+1] = 1.0f; texCoords[2*2+1] = 1.0f;
	verts[3*2+0] = 0.0f; texCoords[3*2+0] = 0.0f;
	verts[3*2+1] = 1.0f; texCoords[3*2+1] = 1.0f;

	mDrawTextureVao = Vao::create();
	ScopedVao scpVao( mDrawTextureVao );
	mDrawTextureVbo = gl::Vbo::create( GL_ARRAY_BUFFER, sizeof(float)*16, data, GL_STATIC_DRAW );
	ScopedBuffer bufferBindScp( mDrawTextureVbo );

	// position
	enableVertexAttribArray( 0 );
	vertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0 );

	// tex coord 0
	enableVertexAttribArray( 1 );
	vertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(float)*8) );
}

VboRef Context::getDefaultArrayVbo( size_t requiredSize )
{
	mDefaultArrayVboIdx = ( mDefaultArrayVboIdx + 1 ) % 4;

	if( ! mDefaultArrayVbo[mDefaultArrayVboIdx] ) {
		mDefaultArrayVbo[mDefaultArrayVboIdx] = Vbo::create( GL_ARRAY_BUFFER, std::max<size_t>( 1, requiredSize ), NULL, GL_STREAM_DRAW );
	}
	else if( requiredSize > mDefaultArrayVbo[mDefaultArrayVboIdx]->getSize() ) {
		mDefaultArrayVbo[mDefaultArrayVboIdx]->ensureMinimumSize( std::max<size_t>( 1, requiredSize ) );
	}

	return mDefaultArrayVbo[mDefaultArrayVboIdx];
}

VboRef Context::getDefaultElementVbo( size_t requiredSize )
{
	if( ! mDefaultElementVbo ) {
		mDefaultElementVbo = Vbo::create( GL_ELEMENT_ARRAY_BUFFER, requiredSize, NULL, GL_STREAM_DRAW );
	}
	if( requiredSize > mDefaultElementVbo->getSize() ) {
		mDefaultElementVbo = Vbo::create( GL_ELEMENT_ARRAY_BUFFER, requiredSize, NULL, GL_STREAM_DRAW );
	}
	
	return mDefaultElementVbo;
}
///////////////////////////////////////////////////////////////////////////////////////////
#if defined( CINDER_MSW ) && ! defined( CINDER_GL_ANGLE )
namespace {
// because the constants aren't in sequential (or ascending) order, we need to convert it
int debugSeverityToOrd( GLenum severity )
{
	switch( severity ) {
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			return 1;
		break;
		case GL_DEBUG_SEVERITY_LOW:
			return 2;
		break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			return 3;
		break;
		default:/*GL_DEBUG_SEVERITY_HIGH:*/
			return 4;
		break;
	}
}
} // anonymous namespace

void __stdcall Context::debugMessageCallback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, void *userParam )
{
	Context *ctx = reinterpret_cast<Context*>( userParam );
	if( ctx->mDebugLogSeverity && (debugSeverityToOrd(severity) >= debugSeverityToOrd(ctx->mDebugLogSeverity)) ) {
		switch( severity ) {
			case GL_DEBUG_SEVERITY_HIGH:
				CI_LOG_E( message );
			break;
			case GL_DEBUG_SEVERITY_MEDIUM:
				CI_LOG_W( message );
			break;
			case GL_DEBUG_SEVERITY_LOW:
			case GL_DEBUG_SEVERITY_NOTIFICATION:
				CI_LOG_I( message );
			break;
		}
	}

	if( ctx->mDebugBreakSeverity && (debugSeverityToOrd(severity) >= debugSeverityToOrd(ctx->mDebugBreakSeverity)) ) {
		__debugbreak();	
	}
}
#endif // defined( CINDER_MSW )

} } // namespace cinder::gl
