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
#include "cinder/gl/platform.h"
#include "cinder/linux/gl_es_load.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>

#if defined( CINDER_ANDROID )
	#include <android/log.h>
#endif

// -----------------------------------------------------------------------------
// iOS
// -----------------------------------------------------------------------------
#if defined( CINDER_COCOA_TOUCH )

// Nothing for now

// -----------------------------------------------------------------------------
// Android and Linux
// -----------------------------------------------------------------------------
#elif defined( CINDER_ANDROID ) || defined( CINDER_LINUX )

static bool 	hasExtension( const std::string& extName );
static void* 	loadEglProc( const std::string& procName ) ;
static void 	clearLoadCount();
static uint32_t	getLoadCount();

#define _GL_ES_LOAD_DEBUG_ 

static void _dbg_out( const std::string& s )
{
#if defined( _GL_ES_LOAD_DEBUG_ )
  #if defined( CINDER_ANDROID )
	__android_log_print( ANDROID_LOG_INFO, "cinder:gl_es_load", s.c_str() );
  #elif defined( CINDER_LINUX )
	std::cout << s << std::endl; 
  #endif
#endif	
}

#define DEBUG( STRM ) 				\
	{								\
		std::stringstream tmpss;	\
		tmpss << STRM;				\
		_dbg_out( tmpss.str() );	\
	}

#define EXT_QUOTE_STR( STR ) \
	#STR

// ----------------------------------------------------------------------------
// OpenGL ES 3.0
// ----------------------------------------------------------------------------
#if defined( CINDER_LINUX ) && ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3 )
PFNGLREADBUFFERPROC fnptr_ci_glReadBuffer = nullptr;
PFNGLDRAWRANGEELEMENTSPROC fnptr_ci_glDrawRangeElements = nullptr;
PFNGLTEXIMAGE3DPROC fnptr_ci_glTexImage3D = nullptr;
PFNGLTEXSUBIMAGE3DPROC fnptr_ci_glTexSubImage3D = nullptr;
PFNGLCOPYTEXSUBIMAGE3DPROC fnptr_ci_glCopyTexSubImage3D = nullptr;
PFNGLCOMPRESSEDTEXIMAGE3DPROC fnptr_ci_glCompressedTexImage3D = nullptr;
PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC fnptr_ci_glCompressedTexSubImage3D = nullptr;
PFNGLGENQUERIESPROC fnptr_ci_glGenQueries = nullptr;
PFNGLDELETEQUERIESPROC fnptr_ci_glDeleteQueries = nullptr;
PFNGLISQUERYPROC fnptr_ci_glIsQuery = nullptr;
PFNGLBEGINQUERYPROC fnptr_ci_glBeginQuery = nullptr;
PFNGLENDQUERYPROC fnptr_ci_glEndQuery = nullptr;
PFNGLGETQUERYIVPROC fnptr_ci_glGetQueryiv = nullptr;
PFNGLGETQUERYOBJECTUIVPROC fnptr_ci_glGetQueryObjectuiv = nullptr;
PFNGLUNMAPBUFFERPROC fnptr_ci_glUnmapBuffer = nullptr;
PFNGLGETBUFFERPOINTERVPROC fnptr_ci_glGetBufferPointerv = nullptr;
PFNGLDRAWBUFFERSPROC fnptr_ci_glDrawBuffers = nullptr;
PFNGLUNIFORMMATRIX2X3FVPROC fnptr_ci_glUniformMatrix2x3fv = nullptr;
PFNGLUNIFORMMATRIX3X2FVPROC fnptr_ci_glUniformMatrix3x2fv = nullptr;
PFNGLUNIFORMMATRIX2X4FVPROC fnptr_ci_glUniformMatrix2x4fv = nullptr;
PFNGLUNIFORMMATRIX4X2FVPROC fnptr_ci_glUniformMatrix4x2fv = nullptr;
PFNGLUNIFORMMATRIX3X4FVPROC fnptr_ci_glUniformMatrix3x4fv = nullptr;
PFNGLUNIFORMMATRIX4X3FVPROC fnptr_ci_glUniformMatrix4x3fv = nullptr;
PFNGLBLITFRAMEBUFFERPROC fnptr_ci_glBlitFramebuffer = nullptr;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC fnptr_ci_glRenderbufferStorageMultisample = nullptr;
PFNGLFRAMEBUFFERTEXTURELAYERPROC fnptr_ci_glFramebufferTextureLayer = nullptr;
PFNGLMAPBUFFERRANGEPROC fnptr_ci_glMapBufferRange = nullptr;
PFNGLFLUSHMAPPEDBUFFERRANGEPROC fnptr_ci_glFlushMappedBufferRange = nullptr;
PFNGLBINDVERTEXARRAYPROC fnptr_ci_glBindVertexArray = nullptr;
PFNGLDELETEVERTEXARRAYSPROC fnptr_ci_glDeleteVertexArrays = nullptr;
PFNGLGENVERTEXARRAYSPROC fnptr_ci_glGenVertexArrays = nullptr;
PFNGLISVERTEXARRAYPROC fnptr_ci_glIsVertexArray = nullptr;
PFNGLGETINTEGERI_VPROC fnptr_ci_glGetIntegeri_v = nullptr;
PFNGLBEGINTRANSFORMFEEDBACKPROC fnptr_ci_glBeginTransformFeedback = nullptr;
PFNGLENDTRANSFORMFEEDBACKPROC fnptr_ci_glEndTransformFeedback = nullptr;
PFNGLBINDBUFFERRANGEPROC fnptr_ci_glBindBufferRange = nullptr;
PFNGLBINDBUFFERBASEPROC fnptr_ci_glBindBufferBase = nullptr;
PFNGLTRANSFORMFEEDBACKVARYINGSPROC fnptr_ci_glTransformFeedbackVaryings = nullptr;
PFNGLGETTRANSFORMFEEDBACKVARYINGPROC fnptr_ci_glGetTransformFeedbackVarying = nullptr;
PFNGLVERTEXATTRIBIPOINTERPROC fnptr_ci_glVertexAttribIPointer = nullptr;
PFNGLGETVERTEXATTRIBIIVPROC fnptr_ci_glGetVertexAttribIiv = nullptr;
PFNGLGETVERTEXATTRIBIUIVPROC fnptr_ci_glGetVertexAttribIuiv = nullptr;
PFNGLVERTEXATTRIBI4IPROC fnptr_ci_glVertexAttribI4i = nullptr;
PFNGLVERTEXATTRIBI4UIPROC fnptr_ci_glVertexAttribI4ui = nullptr;
PFNGLVERTEXATTRIBI4IVPROC fnptr_ci_glVertexAttribI4iv = nullptr;
PFNGLVERTEXATTRIBI4UIVPROC fnptr_ci_glVertexAttribI4uiv = nullptr;
PFNGLGETUNIFORMUIVPROC fnptr_ci_glGetUniformuiv = nullptr;
PFNGLGETFRAGDATALOCATIONPROC fnptr_ci_glGetFragDataLocation = nullptr;
PFNGLUNIFORM1UIPROC fnptr_ci_glUniform1ui = nullptr;
PFNGLUNIFORM2UIPROC fnptr_ci_glUniform2ui = nullptr;
PFNGLUNIFORM3UIPROC fnptr_ci_glUniform3ui = nullptr;
PFNGLUNIFORM4UIPROC fnptr_ci_glUniform4ui = nullptr;
PFNGLUNIFORM1UIVPROC fnptr_ci_glUniform1uiv = nullptr;
PFNGLUNIFORM2UIVPROC fnptr_ci_glUniform2uiv = nullptr;
PFNGLUNIFORM3UIVPROC fnptr_ci_glUniform3uiv = nullptr;
PFNGLUNIFORM4UIVPROC fnptr_ci_glUniform4uiv = nullptr;
PFNGLCLEARBUFFERIVPROC fnptr_ci_glClearBufferiv = nullptr;
PFNGLCLEARBUFFERUIVPROC fnptr_ci_glClearBufferuiv = nullptr;
PFNGLCLEARBUFFERFVPROC fnptr_ci_glClearBufferfv = nullptr;
PFNGLCLEARBUFFERFIPROC fnptr_ci_glClearBufferfi = nullptr;
PFNGLGETSTRINGIPROC fnptr_ci_glGetStringi = nullptr;
PFNGLCOPYBUFFERSUBDATAPROC fnptr_ci_glCopyBufferSubData = nullptr;
PFNGLGETUNIFORMINDICESPROC fnptr_ci_glGetUniformIndices = nullptr;
PFNGLGETACTIVEUNIFORMSIVPROC fnptr_ci_glGetActiveUniformsiv = nullptr;
PFNGLGETUNIFORMBLOCKINDEXPROC fnptr_ci_glGetUniformBlockIndex = nullptr;
PFNGLGETACTIVEUNIFORMBLOCKIVPROC fnptr_ci_glGetActiveUniformBlockiv = nullptr;
PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC fnptr_ci_glGetActiveUniformBlockName = nullptr;
PFNGLUNIFORMBLOCKBINDINGPROC fnptr_ci_glUniformBlockBinding = nullptr;
PFNGLDRAWARRAYSINSTANCEDPROC fnptr_ci_glDrawArraysInstanced = nullptr;
PFNGLDRAWELEMENTSINSTANCEDPROC fnptr_ci_glDrawElementsInstanced = nullptr;
PFNGLFENCESYNCPROC fnptr_ci_glFenceSync = nullptr;
PFNGLISSYNCPROC fnptr_ci_glIsSync = nullptr;
PFNGLDELETESYNCPROC fnptr_ci_glDeleteSync = nullptr;
PFNGLCLIENTWAITSYNCPROC fnptr_ci_glClientWaitSync = nullptr;
PFNGLWAITSYNCPROC fnptr_ci_glWaitSync = nullptr;
PFNGLGETINTEGER64VPROC fnptr_ci_glGetInteger64v = nullptr;
PFNGLGETSYNCIVPROC fnptr_ci_glGetSynciv = nullptr;
PFNGLGETINTEGER64I_VPROC fnptr_ci_glGetInteger64i_v = nullptr;
PFNGLGETBUFFERPARAMETERI64VPROC fnptr_ci_glGetBufferParameteri64v = nullptr;
PFNGLGENSAMPLERSPROC fnptr_ci_glGenSamplers = nullptr;
PFNGLDELETESAMPLERSPROC fnptr_ci_glDeleteSamplers = nullptr;
PFNGLISSAMPLERPROC fnptr_ci_glIsSampler = nullptr;
PFNGLBINDSAMPLERPROC fnptr_ci_glBindSampler = nullptr;
PFNGLSAMPLERPARAMETERIPROC fnptr_ci_glSamplerParameteri = nullptr;
PFNGLSAMPLERPARAMETERIVPROC fnptr_ci_glSamplerParameteriv = nullptr;
PFNGLSAMPLERPARAMETERFPROC fnptr_ci_glSamplerParameterf = nullptr;
PFNGLSAMPLERPARAMETERFVPROC fnptr_ci_glSamplerParameterfv = nullptr;
PFNGLGETSAMPLERPARAMETERIVPROC fnptr_ci_glGetSamplerParameteriv = nullptr;
PFNGLGETSAMPLERPARAMETERFVPROC fnptr_ci_glGetSamplerParameterfv = nullptr;
PFNGLVERTEXATTRIBDIVISORPROC fnptr_ci_glVertexAttribDivisor = nullptr;
PFNGLBINDTRANSFORMFEEDBACKPROC fnptr_ci_glBindTransformFeedback = nullptr;
PFNGLDELETETRANSFORMFEEDBACKSPROC fnptr_ci_glDeleteTransformFeedbacks = nullptr;
PFNGLGENTRANSFORMFEEDBACKSPROC fnptr_ci_glGenTransformFeedbacks = nullptr;
PFNGLISTRANSFORMFEEDBACKPROC fnptr_ci_glIsTransformFeedback = nullptr;
PFNGLPAUSETRANSFORMFEEDBACKPROC fnptr_ci_glPauseTransformFeedback = nullptr;
PFNGLRESUMETRANSFORMFEEDBACKPROC fnptr_ci_glResumeTransformFeedback = nullptr;
PFNGLGETPROGRAMBINARYPROC fnptr_ci_glGetProgramBinary = nullptr;
PFNGLPROGRAMBINARYPROC fnptr_ci_glProgramBinary = nullptr;
PFNGLPROGRAMPARAMETERIPROC fnptr_ci_glProgramParameteri = nullptr;
PFNGLINVALIDATEFRAMEBUFFERPROC fnptr_ci_glInvalidateFramebuffer = nullptr;
PFNGLINVALIDATESUBFRAMEBUFFERPROC fnptr_ci_glInvalidateSubFramebuffer = nullptr;
PFNGLTEXSTORAGE2DPROC fnptr_ci_glTexStorage2D = nullptr;
PFNGLTEXSTORAGE3DPROC fnptr_ci_glTexStorage3D = nullptr;
PFNGLGETINTERNALFORMATIVPROC fnptr_ci_glGetInternalformativ = nullptr;

void gl_es_3_0_load() 
{
	//DEBUG( "gl_es_3_0_load entered..." );

	clearLoadCount();
	fnptr_ci_glReadBuffer = (PFNGLREADBUFFERPROC)loadEglProc("glReadBuffer");
	fnptr_ci_glDrawRangeElements = (PFNGLDRAWRANGEELEMENTSPROC)loadEglProc("glDrawRangeElements");
	fnptr_ci_glTexImage3D = (PFNGLTEXIMAGE3DPROC)loadEglProc("glTexImage3D");
	fnptr_ci_glTexSubImage3D = (PFNGLTEXSUBIMAGE3DPROC)loadEglProc("glTexSubImage3D");
	fnptr_ci_glCopyTexSubImage3D = (PFNGLCOPYTEXSUBIMAGE3DPROC)loadEglProc("glCopyTexSubImage3D");
	fnptr_ci_glCompressedTexImage3D = (PFNGLCOMPRESSEDTEXIMAGE3DPROC)loadEglProc("glCompressedTexImage3D");
	fnptr_ci_glCompressedTexSubImage3D = (PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC)loadEglProc("glCompressedTexSubImage3D");
	fnptr_ci_glGenQueries = (PFNGLGENQUERIESPROC)loadEglProc("glGenQueries");
	fnptr_ci_glDeleteQueries = (PFNGLDELETEQUERIESPROC)loadEglProc("glDeleteQueries");
	fnptr_ci_glIsQuery = (PFNGLISQUERYPROC)loadEglProc("glIsQuery");
	fnptr_ci_glBeginQuery = (PFNGLBEGINQUERYPROC)loadEglProc("glBeginQuery");
	fnptr_ci_glEndQuery = (PFNGLENDQUERYPROC)loadEglProc("glEndQuery");
	fnptr_ci_glGetQueryiv = (PFNGLGETQUERYIVPROC)loadEglProc("glGetQueryiv");
	fnptr_ci_glGetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIVPROC)loadEglProc("glGetQueryObjectuiv");
	fnptr_ci_glUnmapBuffer = (PFNGLUNMAPBUFFERPROC)loadEglProc("glUnmapBuffer");
	fnptr_ci_glGetBufferPointerv = (PFNGLGETBUFFERPOINTERVPROC)loadEglProc("glGetBufferPointerv");
	fnptr_ci_glDrawBuffers = (PFNGLDRAWBUFFERSPROC)loadEglProc("glDrawBuffers");
	fnptr_ci_glUniformMatrix2x3fv = (PFNGLUNIFORMMATRIX2X3FVPROC)loadEglProc("glUniformMatrix2x3fv");
	fnptr_ci_glUniformMatrix3x2fv = (PFNGLUNIFORMMATRIX3X2FVPROC)loadEglProc("glUniformMatrix3x2fv");
	fnptr_ci_glUniformMatrix2x4fv = (PFNGLUNIFORMMATRIX2X4FVPROC)loadEglProc("glUniformMatrix2x4fv");
	fnptr_ci_glUniformMatrix4x2fv = (PFNGLUNIFORMMATRIX4X2FVPROC)loadEglProc("glUniformMatrix4x2fv");
	fnptr_ci_glUniformMatrix3x4fv = (PFNGLUNIFORMMATRIX3X4FVPROC)loadEglProc("glUniformMatrix3x4fv");
	fnptr_ci_glUniformMatrix4x3fv = (PFNGLUNIFORMMATRIX4X3FVPROC)loadEglProc("glUniformMatrix4x3fv");
	fnptr_ci_glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)loadEglProc("glBlitFramebuffer");
	fnptr_ci_glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)loadEglProc("glRenderbufferStorageMultisample");
	fnptr_ci_glFramebufferTextureLayer = (PFNGLFRAMEBUFFERTEXTURELAYERPROC)loadEglProc("glFramebufferTextureLayer");
	fnptr_ci_glMapBufferRange = (PFNGLMAPBUFFERRANGEPROC)loadEglProc("glMapBufferRange");
	fnptr_ci_glFlushMappedBufferRange = (PFNGLFLUSHMAPPEDBUFFERRANGEPROC)loadEglProc("glFlushMappedBufferRange");
	fnptr_ci_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)loadEglProc("glBindVertexArray");
	fnptr_ci_glDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)loadEglProc("glDeleteVertexArrays");
	fnptr_ci_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)loadEglProc("glGenVertexArrays");
	fnptr_ci_glIsVertexArray = (PFNGLISVERTEXARRAYPROC)loadEglProc("glIsVertexArray");
	fnptr_ci_glGetIntegeri_v = (PFNGLGETINTEGERI_VPROC)loadEglProc("glGetIntegeri_v");
	fnptr_ci_glBeginTransformFeedback = (PFNGLBEGINTRANSFORMFEEDBACKPROC)loadEglProc("glBeginTransformFeedback");
	fnptr_ci_glEndTransformFeedback = (PFNGLENDTRANSFORMFEEDBACKPROC)loadEglProc("glEndTransformFeedback");
	fnptr_ci_glBindBufferRange = (PFNGLBINDBUFFERRANGEPROC)loadEglProc("glBindBufferRange");
	fnptr_ci_glBindBufferBase = (PFNGLBINDBUFFERBASEPROC)loadEglProc("glBindBufferBase");
	fnptr_ci_glTransformFeedbackVaryings = (PFNGLTRANSFORMFEEDBACKVARYINGSPROC)loadEglProc("glTransformFeedbackVaryings");
	fnptr_ci_glGetTransformFeedbackVarying = (PFNGLGETTRANSFORMFEEDBACKVARYINGPROC)loadEglProc("glGetTransformFeedbackVarying");
	fnptr_ci_glVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTERPROC)loadEglProc("glVertexAttribIPointer");
	fnptr_ci_glGetVertexAttribIiv = (PFNGLGETVERTEXATTRIBIIVPROC)loadEglProc("glGetVertexAttribIiv");
	fnptr_ci_glGetVertexAttribIuiv = (PFNGLGETVERTEXATTRIBIUIVPROC)loadEglProc("glGetVertexAttribIuiv");
	fnptr_ci_glVertexAttribI4i = (PFNGLVERTEXATTRIBI4IPROC)loadEglProc("glVertexAttribI4i");
	fnptr_ci_glVertexAttribI4ui = (PFNGLVERTEXATTRIBI4UIPROC)loadEglProc("glVertexAttribI4ui");
	fnptr_ci_glVertexAttribI4iv = (PFNGLVERTEXATTRIBI4IVPROC)loadEglProc("glVertexAttribI4iv");
	fnptr_ci_glVertexAttribI4uiv = (PFNGLVERTEXATTRIBI4UIVPROC)loadEglProc("glVertexAttribI4uiv");
	fnptr_ci_glGetUniformuiv = (PFNGLGETUNIFORMUIVPROC)loadEglProc("glGetUniformuiv");
	fnptr_ci_glGetFragDataLocation = (PFNGLGETFRAGDATALOCATIONPROC)loadEglProc("glGetFragDataLocation");
	fnptr_ci_glUniform1ui = (PFNGLUNIFORM1UIPROC)loadEglProc("glUniform1ui");
	fnptr_ci_glUniform2ui = (PFNGLUNIFORM2UIPROC)loadEglProc("glUniform2ui");
	fnptr_ci_glUniform3ui = (PFNGLUNIFORM3UIPROC)loadEglProc("glUniform3ui");
	fnptr_ci_glUniform4ui = (PFNGLUNIFORM4UIPROC)loadEglProc("glUniform4ui");
	fnptr_ci_glUniform1uiv = (PFNGLUNIFORM1UIVPROC)loadEglProc("glUniform1uiv");
	fnptr_ci_glUniform2uiv = (PFNGLUNIFORM2UIVPROC)loadEglProc("glUniform2uiv");
	fnptr_ci_glUniform3uiv = (PFNGLUNIFORM3UIVPROC)loadEglProc("glUniform3uiv");
	fnptr_ci_glUniform4uiv = (PFNGLUNIFORM4UIVPROC)loadEglProc("glUniform4uiv");
	fnptr_ci_glClearBufferiv = (PFNGLCLEARBUFFERIVPROC)loadEglProc("glClearBufferiv");
	fnptr_ci_glClearBufferuiv = (PFNGLCLEARBUFFERUIVPROC)loadEglProc("glClearBufferuiv");
	fnptr_ci_glClearBufferfv = (PFNGLCLEARBUFFERFVPROC)loadEglProc("glClearBufferfv");
	fnptr_ci_glClearBufferfi = (PFNGLCLEARBUFFERFIPROC)loadEglProc("glClearBufferfi");
	fnptr_ci_glGetStringi = (PFNGLGETSTRINGIPROC)loadEglProc("glGetStringi");
	fnptr_ci_glCopyBufferSubData = (PFNGLCOPYBUFFERSUBDATAPROC)loadEglProc("glCopyBufferSubData");
	fnptr_ci_glGetUniformIndices = (PFNGLGETUNIFORMINDICESPROC)loadEglProc("glGetUniformIndices");
	fnptr_ci_glGetActiveUniformsiv = (PFNGLGETACTIVEUNIFORMSIVPROC)loadEglProc("glGetActiveUniformsiv");
	fnptr_ci_glGetUniformBlockIndex = (PFNGLGETUNIFORMBLOCKINDEXPROC)loadEglProc("glGetUniformBlockIndex");
	fnptr_ci_glGetActiveUniformBlockiv = (PFNGLGETACTIVEUNIFORMBLOCKIVPROC)loadEglProc("glGetActiveUniformBlockiv");
	fnptr_ci_glGetActiveUniformBlockName = (PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC)loadEglProc("glGetActiveUniformBlockName");
	fnptr_ci_glUniformBlockBinding = (PFNGLUNIFORMBLOCKBINDINGPROC)loadEglProc("glUniformBlockBinding");
	fnptr_ci_glDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDPROC)loadEglProc("glDrawArraysInstanced");
	fnptr_ci_glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDPROC)loadEglProc("glDrawElementsInstanced");
	fnptr_ci_glFenceSync = (PFNGLFENCESYNCPROC)loadEglProc("glFenceSync");
	fnptr_ci_glIsSync = (PFNGLISSYNCPROC)loadEglProc("glIsSync");
	fnptr_ci_glDeleteSync = (PFNGLDELETESYNCPROC)loadEglProc("glDeleteSync");
	fnptr_ci_glClientWaitSync = (PFNGLCLIENTWAITSYNCPROC)loadEglProc("glClientWaitSync");
	fnptr_ci_glWaitSync = (PFNGLWAITSYNCPROC)loadEglProc("glWaitSync");
	fnptr_ci_glGetInteger64v = (PFNGLGETINTEGER64VPROC)loadEglProc("glGetInteger64v");
	fnptr_ci_glGetSynciv = (PFNGLGETSYNCIVPROC)loadEglProc("glGetSynciv");
	fnptr_ci_glGetInteger64i_v = (PFNGLGETINTEGER64I_VPROC)loadEglProc("glGetInteger64i_v");
	fnptr_ci_glGetBufferParameteri64v = (PFNGLGETBUFFERPARAMETERI64VPROC)loadEglProc("glGetBufferParameteri64v");
	fnptr_ci_glGenSamplers = (PFNGLGENSAMPLERSPROC)loadEglProc("glGenSamplers");
	fnptr_ci_glDeleteSamplers = (PFNGLDELETESAMPLERSPROC)loadEglProc("glDeleteSamplers");
	fnptr_ci_glIsSampler = (PFNGLISSAMPLERPROC)loadEglProc("glIsSampler");
	fnptr_ci_glBindSampler = (PFNGLBINDSAMPLERPROC)loadEglProc("glBindSampler");
	fnptr_ci_glSamplerParameteri = (PFNGLSAMPLERPARAMETERIPROC)loadEglProc("glSamplerParameteri");
	fnptr_ci_glSamplerParameteriv = (PFNGLSAMPLERPARAMETERIVPROC)loadEglProc("glSamplerParameteriv");
	fnptr_ci_glSamplerParameterf = (PFNGLSAMPLERPARAMETERFPROC)loadEglProc("glSamplerParameterf");
	fnptr_ci_glSamplerParameterfv = (PFNGLSAMPLERPARAMETERFVPROC)loadEglProc("glSamplerParameterfv");
	fnptr_ci_glGetSamplerParameteriv = (PFNGLGETSAMPLERPARAMETERIVPROC)loadEglProc("glGetSamplerParameteriv");
	fnptr_ci_glGetSamplerParameterfv = (PFNGLGETSAMPLERPARAMETERFVPROC)loadEglProc("glGetSamplerParameterfv");
	fnptr_ci_glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC)loadEglProc("glVertexAttribDivisor");
	fnptr_ci_glBindTransformFeedback = (PFNGLBINDTRANSFORMFEEDBACKPROC)loadEglProc("glBindTransformFeedback");
	fnptr_ci_glDeleteTransformFeedbacks = (PFNGLDELETETRANSFORMFEEDBACKSPROC)loadEglProc("glDeleteTransformFeedbacks");
	fnptr_ci_glGenTransformFeedbacks = (PFNGLGENTRANSFORMFEEDBACKSPROC)loadEglProc("glGenTransformFeedbacks");
	fnptr_ci_glIsTransformFeedback = (PFNGLISTRANSFORMFEEDBACKPROC)loadEglProc("glIsTransformFeedback");
	fnptr_ci_glPauseTransformFeedback = (PFNGLPAUSETRANSFORMFEEDBACKPROC)loadEglProc("glPauseTransformFeedback");
	fnptr_ci_glResumeTransformFeedback = (PFNGLRESUMETRANSFORMFEEDBACKPROC)loadEglProc("glResumeTransformFeedback");
	fnptr_ci_glGetProgramBinary = (PFNGLGETPROGRAMBINARYPROC)loadEglProc("glGetProgramBinary");
	fnptr_ci_glProgramBinary = (PFNGLPROGRAMBINARYPROC)loadEglProc("glProgramBinary");
	fnptr_ci_glProgramParameteri = (PFNGLPROGRAMPARAMETERIPROC)loadEglProc("glProgramParameteri");
	fnptr_ci_glInvalidateFramebuffer = (PFNGLINVALIDATEFRAMEBUFFERPROC)loadEglProc("glInvalidateFramebuffer");
	fnptr_ci_glInvalidateSubFramebuffer = (PFNGLINVALIDATESUBFRAMEBUFFERPROC)loadEglProc("glInvalidateSubFramebuffer");
	fnptr_ci_glTexStorage2D = (PFNGLTEXSTORAGE2DPROC)loadEglProc("glTexStorage2D");
	fnptr_ci_glTexStorage3D = (PFNGLTEXSTORAGE3DPROC)loadEglProc("glTexStorage3D");
	fnptr_ci_glGetInternalformativ = (PFNGLGETINTERNALFORMATIVPROC)loadEglProc("glGetInternalformativ");

	DEBUG( "gl_es_3_0_load: SUCCESSFUL! | " << getLoadCount() << " procs loaded");
}
#endif // defined( CINDER_LINUX ) && ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3 )

// ----------------------------------------------------------------------------
// OpenGL ES 3.1
// ----------------------------------------------------------------------------
#if defined( CINDER_LINUX ) && ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3_1 )
PFNGLDISPATCHCOMPUTEPROC fnptr_ci_glDispatchCompute = nullptr; 
PFNGLDISPATCHCOMPUTEINDIRECTPROC fnptr_ci_glDispatchComputeIndirect = nullptr; 
PFNGLDRAWARRAYSINDIRECTPROC fnptr_ci_glDrawArraysIndirect = nullptr; 
PFNGLDRAWELEMENTSINDIRECTPROC fnptr_ci_glDrawElementsIndirect = nullptr; 
PFNGLFRAMEBUFFERPARAMETERIPROC fnptr_ci_glFramebufferParameteri = nullptr; 
PFNGLGETFRAMEBUFFERPARAMETERIVPROC fnptr_ci_glGetFramebufferParameteriv = nullptr; 
PFNGLGETPROGRAMINTERFACEIVPROC fnptr_ci_glGetProgramInterfaceiv = nullptr; 
PFNGLGETPROGRAMRESOURCEINDEXPROC fnptr_ci_glGetProgramResourceIndex = nullptr; 
PFNGLGETPROGRAMRESOURCENAMEPROC fnptr_ci_glGetProgramResourceName = nullptr; 
PFNGLGETPROGRAMRESOURCEIVPROC fnptr_ci_glGetProgramResourceiv = nullptr; 
PFNGLGETPROGRAMRESOURCELOCATIONPROC fnptr_ci_glGetProgramResourceLocation = nullptr; 
PFNGLUSEPROGRAMSTAGESPROC fnptr_ci_glUseProgramStages = nullptr; 
PFNGLACTIVESHADERPROGRAMPROC fnptr_ci_glActiveShaderProgram = nullptr; 
PFNGLCREATESHADERPROGRAMVPROC fnptr_ci_glCreateShaderProgramv = nullptr; 
PFNGLBINDPROGRAMPIPELINEPROC fnptr_ci_glBindProgramPipeline = nullptr; 
PFNGLDELETEPROGRAMPIPELINESPROC fnptr_ci_glDeleteProgramPipelines = nullptr; 
PFNGLGENPROGRAMPIPELINESPROC fnptr_ci_glGenProgramPipelines = nullptr; 
PFNGLISPROGRAMPIPELINEPROC fnptr_ci_glIsProgramPipeline = nullptr; 
PFNGLGETPROGRAMPIPELINEIVPROC fnptr_ci_glGetProgramPipelineiv = nullptr; 
PFNGLPROGRAMUNIFORM1IPROC fnptr_ci_glProgramUniform1i = nullptr; 
PFNGLPROGRAMUNIFORM2IPROC fnptr_ci_glProgramUniform2i = nullptr; 
PFNGLPROGRAMUNIFORM3IPROC fnptr_ci_glProgramUniform3i = nullptr; 
PFNGLPROGRAMUNIFORM4IPROC fnptr_ci_glProgramUniform4i = nullptr; 
PFNGLPROGRAMUNIFORM1UIPROC fnptr_ci_glProgramUniform1ui = nullptr; 
PFNGLPROGRAMUNIFORM2UIPROC fnptr_ci_glProgramUniform2ui = nullptr; 
PFNGLPROGRAMUNIFORM3UIPROC fnptr_ci_glProgramUniform3ui = nullptr; 
PFNGLPROGRAMUNIFORM4UIPROC fnptr_ci_glProgramUniform4ui = nullptr; 
PFNGLPROGRAMUNIFORM1FPROC fnptr_ci_glProgramUniform1f = nullptr; 
PFNGLPROGRAMUNIFORM2FPROC fnptr_ci_glProgramUniform2f = nullptr; 
PFNGLPROGRAMUNIFORM3FPROC fnptr_ci_glProgramUniform3f = nullptr; 
PFNGLPROGRAMUNIFORM4FPROC fnptr_ci_glProgramUniform4f = nullptr; 
PFNGLPROGRAMUNIFORM1IVPROC fnptr_ci_glProgramUniform1iv = nullptr; 
PFNGLPROGRAMUNIFORM2IVPROC fnptr_ci_glProgramUniform2iv = nullptr; 
PFNGLPROGRAMUNIFORM3IVPROC fnptr_ci_glProgramUniform3iv = nullptr; 
PFNGLPROGRAMUNIFORM4IVPROC fnptr_ci_glProgramUniform4iv = nullptr; 
PFNGLPROGRAMUNIFORM1UIVPROC fnptr_ci_glProgramUniform1uiv = nullptr; 
PFNGLPROGRAMUNIFORM2UIVPROC fnptr_ci_glProgramUniform2uiv = nullptr; 
PFNGLPROGRAMUNIFORM3UIVPROC fnptr_ci_glProgramUniform3uiv = nullptr; 
PFNGLPROGRAMUNIFORM4UIVPROC fnptr_ci_glProgramUniform4uiv = nullptr; 
PFNGLPROGRAMUNIFORM1FVPROC fnptr_ci_glProgramUniform1fv = nullptr; 
PFNGLPROGRAMUNIFORM2FVPROC fnptr_ci_glProgramUniform2fv = nullptr; 
PFNGLPROGRAMUNIFORM3FVPROC fnptr_ci_glProgramUniform3fv = nullptr; 
PFNGLPROGRAMUNIFORM4FVPROC fnptr_ci_glProgramUniform4fv = nullptr; 
PFNGLPROGRAMUNIFORMMATRIX2FVPROC fnptr_ci_glProgramUniformMatrix2fv = nullptr; 
PFNGLPROGRAMUNIFORMMATRIX3FVPROC fnptr_ci_glProgramUniformMatrix3fv = nullptr; 
PFNGLPROGRAMUNIFORMMATRIX4FVPROC fnptr_ci_glProgramUniformMatrix4fv = nullptr; 
PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC fnptr_ci_glProgramUniformMatrix2x3fv = nullptr; 
PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC fnptr_ci_glProgramUniformMatrix3x2fv = nullptr; 
PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC fnptr_ci_glProgramUniformMatrix2x4fv = nullptr; 
PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC fnptr_ci_glProgramUniformMatrix4x2fv = nullptr; 
PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC fnptr_ci_glProgramUniformMatrix3x4fv = nullptr; 
PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC fnptr_ci_glProgramUniformMatrix4x3fv = nullptr; 
PFNGLVALIDATEPROGRAMPIPELINEPROC fnptr_ci_glValidateProgramPipeline = nullptr; 
PFNGLGETPROGRAMPIPELINEINFOLOGPROC fnptr_ci_glGetProgramPipelineInfoLog = nullptr; 
PFNGLBINDIMAGETEXTUREPROC fnptr_ci_glBindImageTexture = nullptr; 
PFNGLGETBOOLEANI_VPROC fnptr_ci_glGetBooleani_v = nullptr; 
PFNGLMEMORYBARRIERPROC fnptr_ci_glMemoryBarrier = nullptr; 
PFNGLMEMORYBARRIERBYREGIONPROC fnptr_ci_glMemoryBarrierByRegion = nullptr; 
PFNGLTEXSTORAGE2DMULTISAMPLEPROC fnptr_ci_glTexStorage2DMultisample = nullptr; 
PFNGLGETMULTISAMPLEFVPROC fnptr_ci_glGetMultisamplefv = nullptr; 
PFNGLSAMPLEMASKIPROC fnptr_ci_glSampleMaski = nullptr; 
PFNGLGETTEXLEVELPARAMETERIVPROC fnptr_ci_glGetTexLevelParameteriv = nullptr; 
PFNGLGETTEXLEVELPARAMETERFVPROC fnptr_ci_glGetTexLevelParameterfv = nullptr; 
PFNGLBINDVERTEXBUFFERPROC fnptr_ci_glBindVertexBuffer = nullptr; 
PFNGLVERTEXATTRIBFORMATPROC fnptr_ci_glVertexAttribFormat = nullptr; 
PFNGLVERTEXATTRIBIFORMATPROC fnptr_ci_glVertexAttribIFormat = nullptr; 
PFNGLVERTEXATTRIBBINDINGPROC fnptr_ci_glVertexAttribBinding = nullptr; 
PFNGLVERTEXBINDINGDIVISORPROC fnptr_ci_glVertexBindingDivisor = nullptr; 

void gl_es_3_1_load()
{
	//DEBUG( "gl_es_3_1_load entered..." );

	clearLoadCount();
	fnptr_ci_glDispatchCompute = (PFNGLDISPATCHCOMPUTEPROC)loadEglProc("glDispatchCompute"); 
	fnptr_ci_glDispatchComputeIndirect = (PFNGLDISPATCHCOMPUTEINDIRECTPROC)loadEglProc("glDispatchComputeIndirect"); 
	fnptr_ci_glDrawArraysIndirect = (PFNGLDRAWARRAYSINDIRECTPROC)loadEglProc("glDrawArraysIndirect"); 
	fnptr_ci_glDrawElementsIndirect = (PFNGLDRAWELEMENTSINDIRECTPROC)loadEglProc("glDrawElementsIndirect"); 
	fnptr_ci_glFramebufferParameteri = (PFNGLFRAMEBUFFERPARAMETERIPROC)loadEglProc("glFramebufferParameteri"); 
	fnptr_ci_glGetFramebufferParameteriv = (PFNGLGETFRAMEBUFFERPARAMETERIVPROC)loadEglProc("glGetFramebufferParameteriv"); 
	fnptr_ci_glGetProgramInterfaceiv = (PFNGLGETPROGRAMINTERFACEIVPROC)loadEglProc("glGetProgramInterfaceiv"); 
	fnptr_ci_glGetProgramResourceIndex = (PFNGLGETPROGRAMRESOURCEINDEXPROC)loadEglProc("glGetProgramResourceIndex"); 
	fnptr_ci_glGetProgramResourceName = (PFNGLGETPROGRAMRESOURCENAMEPROC)loadEglProc("glGetProgramResourceName"); 
	fnptr_ci_glGetProgramResourceiv = (PFNGLGETPROGRAMRESOURCEIVPROC)loadEglProc("glGetProgramResourceiv"); 
	fnptr_ci_glGetProgramResourceLocation = (PFNGLGETPROGRAMRESOURCELOCATIONPROC)loadEglProc("glGetProgramResourceLocation"); 
	fnptr_ci_glUseProgramStages = (PFNGLUSEPROGRAMSTAGESPROC)loadEglProc("glUseProgramStages"); 
	fnptr_ci_glActiveShaderProgram = (PFNGLACTIVESHADERPROGRAMPROC)loadEglProc("glActiveShaderProgram"); 
	fnptr_ci_glCreateShaderProgramv = (PFNGLCREATESHADERPROGRAMVPROC)loadEglProc("glCreateShaderProgramv"); 
	fnptr_ci_glBindProgramPipeline = (PFNGLBINDPROGRAMPIPELINEPROC)loadEglProc("glBindProgramPipeline"); 
	fnptr_ci_glDeleteProgramPipelines = (PFNGLDELETEPROGRAMPIPELINESPROC)loadEglProc("glDeleteProgramPipelines"); 
	fnptr_ci_glGenProgramPipelines = (PFNGLGENPROGRAMPIPELINESPROC)loadEglProc("glGenProgramPipelines"); 
	fnptr_ci_glIsProgramPipeline = (PFNGLISPROGRAMPIPELINEPROC)loadEglProc("glIsProgramPipeline"); 
	fnptr_ci_glGetProgramPipelineiv = (PFNGLGETPROGRAMPIPELINEIVPROC)loadEglProc("glGetProgramPipelineiv"); 
	fnptr_ci_glProgramUniform1i = (PFNGLPROGRAMUNIFORM1IPROC)loadEglProc("glProgramUniform1i"); 
	fnptr_ci_glProgramUniform2i = (PFNGLPROGRAMUNIFORM2IPROC)loadEglProc("glProgramUniform2i"); 
	fnptr_ci_glProgramUniform3i = (PFNGLPROGRAMUNIFORM3IPROC)loadEglProc("glProgramUniform3i"); 
	fnptr_ci_glProgramUniform4i = (PFNGLPROGRAMUNIFORM4IPROC)loadEglProc("glProgramUniform4i"); 
	fnptr_ci_glProgramUniform1ui = (PFNGLPROGRAMUNIFORM1UIPROC)loadEglProc("glProgramUniform1ui"); 
	fnptr_ci_glProgramUniform2ui = (PFNGLPROGRAMUNIFORM2UIPROC)loadEglProc("glProgramUniform2ui"); 
	fnptr_ci_glProgramUniform3ui = (PFNGLPROGRAMUNIFORM3UIPROC)loadEglProc("glProgramUniform3ui"); 
	fnptr_ci_glProgramUniform4ui = (PFNGLPROGRAMUNIFORM4UIPROC)loadEglProc("glProgramUniform4ui"); 
	fnptr_ci_glProgramUniform1f = (PFNGLPROGRAMUNIFORM1FPROC)loadEglProc("glProgramUniform1f"); 
	fnptr_ci_glProgramUniform2f = (PFNGLPROGRAMUNIFORM2FPROC)loadEglProc("glProgramUniform2f"); 
	fnptr_ci_glProgramUniform3f = (PFNGLPROGRAMUNIFORM3FPROC)loadEglProc("glProgramUniform3f"); 
	fnptr_ci_glProgramUniform4f = (PFNGLPROGRAMUNIFORM4FPROC)loadEglProc("glProgramUniform4f"); 
	fnptr_ci_glProgramUniform1iv = (PFNGLPROGRAMUNIFORM1IVPROC)loadEglProc("glProgramUniform1iv"); 
	fnptr_ci_glProgramUniform2iv = (PFNGLPROGRAMUNIFORM2IVPROC)loadEglProc("glProgramUniform2iv"); 
	fnptr_ci_glProgramUniform3iv = (PFNGLPROGRAMUNIFORM3IVPROC)loadEglProc("glProgramUniform3iv"); 
	fnptr_ci_glProgramUniform4iv = (PFNGLPROGRAMUNIFORM4IVPROC)loadEglProc("glProgramUniform4iv"); 
	fnptr_ci_glProgramUniform1uiv = (PFNGLPROGRAMUNIFORM1UIVPROC)loadEglProc("glProgramUniform1uiv"); 
	fnptr_ci_glProgramUniform2uiv = (PFNGLPROGRAMUNIFORM2UIVPROC)loadEglProc("glProgramUniform2uiv"); 
	fnptr_ci_glProgramUniform3uiv = (PFNGLPROGRAMUNIFORM3UIVPROC)loadEglProc("glProgramUniform3uiv"); 
	fnptr_ci_glProgramUniform4uiv = (PFNGLPROGRAMUNIFORM4UIVPROC)loadEglProc("glProgramUniform4uiv"); 
	fnptr_ci_glProgramUniform1fv = (PFNGLPROGRAMUNIFORM1FVPROC)loadEglProc("glProgramUniform1fv"); 
	fnptr_ci_glProgramUniform2fv = (PFNGLPROGRAMUNIFORM2FVPROC)loadEglProc("glProgramUniform2fv"); 
	fnptr_ci_glProgramUniform3fv = (PFNGLPROGRAMUNIFORM3FVPROC)loadEglProc("glProgramUniform3fv"); 
	fnptr_ci_glProgramUniform4fv = (PFNGLPROGRAMUNIFORM4FVPROC)loadEglProc("glProgramUniform4fv"); 
	fnptr_ci_glProgramUniformMatrix2fv = (PFNGLPROGRAMUNIFORMMATRIX2FVPROC)loadEglProc("glProgramUniformMatrix2fv"); 
	fnptr_ci_glProgramUniformMatrix3fv = (PFNGLPROGRAMUNIFORMMATRIX3FVPROC)loadEglProc("glProgramUniformMatrix3fv"); 
	fnptr_ci_glProgramUniformMatrix4fv = (PFNGLPROGRAMUNIFORMMATRIX4FVPROC)loadEglProc("glProgramUniformMatrix4fv"); 
	fnptr_ci_glProgramUniformMatrix2x3fv = (PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC)loadEglProc("glProgramUniformMatrix2x3fv"); 
	fnptr_ci_glProgramUniformMatrix3x2fv = (PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC)loadEglProc("glProgramUniformMatrix3x2fv"); 
	fnptr_ci_glProgramUniformMatrix2x4fv = (PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC)loadEglProc("glProgramUniformMatrix2x4fv"); 
	fnptr_ci_glProgramUniformMatrix4x2fv = (PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC)loadEglProc("glProgramUniformMatrix4x2fv"); 
	fnptr_ci_glProgramUniformMatrix3x4fv = (PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC)loadEglProc("glProgramUniformMatrix3x4fv"); 
	fnptr_ci_glProgramUniformMatrix4x3fv = (PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC)loadEglProc("glProgramUniformMatrix4x3fv"); 
	fnptr_ci_glValidateProgramPipeline = (PFNGLVALIDATEPROGRAMPIPELINEPROC)loadEglProc("glValidateProgramPipeline"); 
	fnptr_ci_glGetProgramPipelineInfoLog = (PFNGLGETPROGRAMPIPELINEINFOLOGPROC)loadEglProc("glGetProgramPipelineInfoLog"); 
	fnptr_ci_glBindImageTexture = (PFNGLBINDIMAGETEXTUREPROC)loadEglProc("glBindImageTexture"); 
	fnptr_ci_glGetBooleani_v = (PFNGLGETBOOLEANI_VPROC)loadEglProc("glGetBooleani_v"); 
	fnptr_ci_glMemoryBarrier = (PFNGLMEMORYBARRIERPROC)loadEglProc("glMemoryBarrier"); 
	fnptr_ci_glMemoryBarrierByRegion = (PFNGLMEMORYBARRIERBYREGIONPROC)loadEglProc("glMemoryBarrierByRegion"); 
	fnptr_ci_glTexStorage2DMultisample = (PFNGLTEXSTORAGE2DMULTISAMPLEPROC)loadEglProc("glTexStorage2DMultisample"); 
	fnptr_ci_glGetMultisamplefv = (PFNGLGETMULTISAMPLEFVPROC)loadEglProc("glGetMultisamplefv"); 
	fnptr_ci_glSampleMaski = (PFNGLSAMPLEMASKIPROC)loadEglProc("glSampleMaski"); 
	fnptr_ci_glGetTexLevelParameteriv = (PFNGLGETTEXLEVELPARAMETERIVPROC)loadEglProc("glGetTexLevelParameteriv"); 
	fnptr_ci_glGetTexLevelParameterfv = (PFNGLGETTEXLEVELPARAMETERFVPROC)loadEglProc("glGetTexLevelParameterfv"); 
	fnptr_ci_glBindVertexBuffer = (PFNGLBINDVERTEXBUFFERPROC)loadEglProc("glBindVertexBuffer"); 
	fnptr_ci_glVertexAttribFormat = (PFNGLVERTEXATTRIBFORMATPROC)loadEglProc("glVertexAttribFormat"); 
	fnptr_ci_glVertexAttribIFormat = (PFNGLVERTEXATTRIBIFORMATPROC)loadEglProc("glVertexAttribIFormat"); 
	fnptr_ci_glVertexAttribBinding = (PFNGLVERTEXATTRIBBINDINGPROC)loadEglProc("glVertexAttribBinding"); 
	fnptr_ci_glVertexBindingDivisor = (PFNGLVERTEXBINDINGDIVISORPROC)loadEglProc("glVertexBindingDivisor"); 

	DEBUG( "gl_es_3_1_load: SUCCESSFUL! | " << getLoadCount() << " procs loaded");
}
#endif // defined( CINDER_LINUX ) && ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3_1 )

// ----------------------------------------------------------------------------
// OpenGL ES 3.2
// ----------------------------------------------------------------------------
#if defined( CINDER_LINUX ) && ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3_2 )
PFNGLBLENDBARRIERPROC fnptr_ci_glBlendBarrier = nullptr; 
PFNGLCOPYIMAGESUBDATAPROC fnptr_ci_glCopyImageSubData = nullptr; 
PFNGLDEBUGMESSAGECONTROLPROC fnptr_ci_glDebugMessageControl = nullptr; 
PFNGLDEBUGMESSAGEINSERTPROC fnptr_ci_glDebugMessageInsert = nullptr; 
PFNGLDEBUGMESSAGECALLBACKPROC fnptr_ci_glDebugMessageCallback = nullptr; 
PFNGLGETDEBUGMESSAGELOGPROC fnptr_ci_glGetDebugMessageLog = nullptr; 
PFNGLPUSHDEBUGGROUPPROC fnptr_ci_glPushDebugGroup = nullptr; 
PFNGLPOPDEBUGGROUPPROC fnptr_ci_glPopDebugGroup = nullptr; 
PFNGLOBJECTLABELPROC fnptr_ci_glObjectLabel = nullptr; 
PFNGLGETOBJECTLABELPROC fnptr_ci_glGetObjectLabel = nullptr; 
PFNGLOBJECTPTRLABELPROC fnptr_ci_glObjectPtrLabel = nullptr; 
PFNGLGETOBJECTPTRLABELPROC fnptr_ci_glGetObjectPtrLabel = nullptr; 
PFNGLGETPOINTERVPROC fnptr_ci_glGetPointerv = nullptr; 
PFNGLENABLEIPROC fnptr_ci_glEnablei = nullptr; 
PFNGLDISABLEIPROC fnptr_ci_glDisablei = nullptr; 
PFNGLBLENDEQUATIONIPROC fnptr_ci_glBlendEquationi = nullptr; 
PFNGLBLENDEQUATIONSEPARATEIPROC fnptr_ci_glBlendEquationSeparatei = nullptr; 
PFNGLBLENDFUNCIPROC fnptr_ci_glBlendFunci = nullptr; 
PFNGLBLENDFUNCSEPARATEIPROC fnptr_ci_glBlendFuncSeparatei = nullptr; 
PFNGLCOLORMASKIPROC fnptr_ci_glColorMaski = nullptr; 
PFNGLISENABLEDIPROC fnptr_ci_glIsEnabledi = nullptr; 
PFNGLDRAWELEMENTSBASEVERTEXPROC fnptr_ci_glDrawElementsBaseVertex = nullptr; 
PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC fnptr_ci_glDrawRangeElementsBaseVertex = nullptr; 
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC fnptr_ci_glDrawElementsInstancedBaseVertex = nullptr; 
PFNGLFRAMEBUFFERTEXTUREPROC fnptr_ci_glFramebufferTexture = nullptr; 
PFNGLPRIMITIVEBOUNDINGBOXPROC fnptr_ci_glPrimitiveBoundingBox = nullptr; 
PFNGLGETGRAPHICSRESETSTATUSPROC fnptr_ci_glGetGraphicsResetStatus = nullptr; 
PFNGLREADNPIXELSPROC fnptr_ci_glReadnPixels = nullptr; 
PFNGLGETNUNIFORMFVPROC fnptr_ci_glGetnUniformfv = nullptr; 
PFNGLGETNUNIFORMIVPROC fnptr_ci_glGetnUniformiv = nullptr; 
PFNGLGETNUNIFORMUIVPROC fnptr_ci_glGetnUniformuiv = nullptr; 
PFNGLMINSAMPLESHADINGPROC fnptr_ci_glMinSampleShading = nullptr; 
PFNGLPATCHPARAMETERIPROC fnptr_ci_glPatchParameteri = nullptr; 
PFNGLTEXPARAMETERIIVPROC fnptr_ci_glTexParameterIiv = nullptr; 
PFNGLTEXPARAMETERIUIVPROC fnptr_ci_glTexParameterIuiv = nullptr; 
PFNGLGETTEXPARAMETERIIVPROC fnptr_ci_glGetTexParameterIiv = nullptr; 
PFNGLGETTEXPARAMETERIUIVPROC fnptr_ci_glGetTexParameterIuiv = nullptr; 
PFNGLSAMPLERPARAMETERIIVPROC fnptr_ci_glSamplerParameterIiv = nullptr; 
PFNGLSAMPLERPARAMETERIUIVPROC fnptr_ci_glSamplerParameterIuiv = nullptr; 
PFNGLGETSAMPLERPARAMETERIIVPROC fnptr_ci_glGetSamplerParameterIiv = nullptr; 
PFNGLGETSAMPLERPARAMETERIUIVPROC fnptr_ci_glGetSamplerParameterIuiv = nullptr; 
PFNGLTEXBUFFERPROC fnptr_ci_glTexBuffer = nullptr; 
PFNGLTEXBUFFERRANGEPROC fnptr_ci_glTexBufferRange = nullptr; 
PFNGLTEXSTORAGE3DMULTISAMPLEPROC fnptr_ci_glTexStorage3DMultisample = nullptr; 

void gl_es_3_2_load()
{
	//DEBUG( "gl_es_3_2_load entered..." );

	clearLoadCount();
	fnptr_ci_glBlendBarrier = (PFNGLBLENDBARRIERPROC)loadEglProc("glBlendBarrier"); 
	fnptr_ci_glCopyImageSubData = (PFNGLCOPYIMAGESUBDATAPROC)loadEglProc("glCopyImageSubData"); 
	fnptr_ci_glDebugMessageControl = (PFNGLDEBUGMESSAGECONTROLPROC)loadEglProc("glDebugMessageControl"); 
	fnptr_ci_glDebugMessageInsert = (PFNGLDEBUGMESSAGEINSERTPROC)loadEglProc("glDebugMessageInsert"); 
	fnptr_ci_glDebugMessageCallback = (PFNGLDEBUGMESSAGECALLBACKPROC)loadEglProc("glDebugMessageCallback"); 
	fnptr_ci_glGetDebugMessageLog = (PFNGLGETDEBUGMESSAGELOGPROC)loadEglProc("glGetDebugMessageLog"); 
	fnptr_ci_glPushDebugGroup = (PFNGLPUSHDEBUGGROUPPROC)loadEglProc("glPushDebugGroup"); 
	fnptr_ci_glPopDebugGroup = (PFNGLPOPDEBUGGROUPPROC)loadEglProc("glPopDebugGroup"); 
	fnptr_ci_glObjectLabel = (PFNGLOBJECTLABELPROC)loadEglProc("glObjectLabel"); 
	fnptr_ci_glGetObjectLabel = (PFNGLGETOBJECTLABELPROC)loadEglProc("glGetObjectLabel"); 
	fnptr_ci_glObjectPtrLabel = (PFNGLOBJECTPTRLABELPROC)loadEglProc("glObjectPtrLabel"); 
	fnptr_ci_glGetObjectPtrLabel = (PFNGLGETOBJECTPTRLABELPROC)loadEglProc("glGetObjectPtrLabel"); 
	fnptr_ci_glGetPointerv = (PFNGLGETPOINTERVPROC)loadEglProc("glGetPointerv"); 
	fnptr_ci_glEnablei = (PFNGLENABLEIPROC)loadEglProc("glEnablei"); 
	fnptr_ci_glDisablei = (PFNGLDISABLEIPROC)loadEglProc("glDisablei"); 
	fnptr_ci_glBlendEquationi = (PFNGLBLENDEQUATIONIPROC)loadEglProc("glBlendEquationi"); 
	fnptr_ci_glBlendEquationSeparatei = (PFNGLBLENDEQUATIONSEPARATEIPROC)loadEglProc("glBlendEquationSeparatei"); 
	fnptr_ci_glBlendFunci = (PFNGLBLENDFUNCIPROC)loadEglProc("glBlendFunci"); 
	fnptr_ci_glBlendFuncSeparatei = (PFNGLBLENDFUNCSEPARATEIPROC)loadEglProc("glBlendFuncSeparatei"); 
	fnptr_ci_glColorMaski = (PFNGLCOLORMASKIPROC)loadEglProc("glColorMaski"); 
	fnptr_ci_glIsEnabledi = (PFNGLISENABLEDIPROC)loadEglProc("glIsEnabledi"); 
	fnptr_ci_glDrawElementsBaseVertex = (PFNGLDRAWELEMENTSBASEVERTEXPROC)loadEglProc("glDrawElementsBaseVertex"); 
	fnptr_ci_glDrawRangeElementsBaseVertex = (PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC)loadEglProc("glDrawRangeElementsBaseVertex"); 
	fnptr_ci_glDrawElementsInstancedBaseVertex = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC)loadEglProc("glDrawElementsInstancedBaseVertex"); 
	fnptr_ci_glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC)loadEglProc("glFramebufferTexture"); 
	fnptr_ci_glPrimitiveBoundingBox = (PFNGLPRIMITIVEBOUNDINGBOXPROC)loadEglProc("glPrimitiveBoundingBox"); 
	fnptr_ci_glGetGraphicsResetStatus = (PFNGLGETGRAPHICSRESETSTATUSPROC)loadEglProc("glGetGraphicsResetStatus"); 
	fnptr_ci_glReadnPixels = (PFNGLREADNPIXELSPROC)loadEglProc("glReadnPixels"); 
	fnptr_ci_glGetnUniformfv = (PFNGLGETNUNIFORMFVPROC)loadEglProc("glGetnUniformfv"); 
	fnptr_ci_glGetnUniformiv = (PFNGLGETNUNIFORMIVPROC)loadEglProc("glGetnUniformiv"); 
	fnptr_ci_glGetnUniformuiv = (PFNGLGETNUNIFORMUIVPROC)loadEglProc("glGetnUniformuiv"); 
	fnptr_ci_glMinSampleShading = (PFNGLMINSAMPLESHADINGPROC)loadEglProc("glMinSampleShading"); 
	fnptr_ci_glPatchParameteri = (PFNGLPATCHPARAMETERIPROC)loadEglProc("glPatchParameteri"); 
	fnptr_ci_glTexParameterIiv = (PFNGLTEXPARAMETERIIVPROC)loadEglProc("glTexParameterIiv"); 
	fnptr_ci_glTexParameterIuiv = (PFNGLTEXPARAMETERIUIVPROC)loadEglProc("glTexParameterIuiv"); 
	fnptr_ci_glGetTexParameterIiv = (PFNGLGETTEXPARAMETERIIVPROC)loadEglProc("glGetTexParameterIiv"); 
	fnptr_ci_glGetTexParameterIuiv = (PFNGLGETTEXPARAMETERIUIVPROC)loadEglProc("glGetTexParameterIuiv"); 
	fnptr_ci_glSamplerParameterIiv = (PFNGLSAMPLERPARAMETERIIVPROC)loadEglProc("glSamplerParameterIiv"); 
	fnptr_ci_glSamplerParameterIuiv = (PFNGLSAMPLERPARAMETERIUIVPROC)loadEglProc("glSamplerParameterIuiv"); 
	fnptr_ci_glGetSamplerParameterIiv = (PFNGLGETSAMPLERPARAMETERIIVPROC)loadEglProc("glGetSamplerParameterIiv"); 
	fnptr_ci_glGetSamplerParameterIuiv = (PFNGLGETSAMPLERPARAMETERIUIVPROC)loadEglProc("glGetSamplerParameterIuiv"); 
	fnptr_ci_glTexBuffer = (PFNGLTEXBUFFERPROC)loadEglProc("glTexBuffer"); 
	fnptr_ci_glTexBufferRange = (PFNGLTEXBUFFERRANGEPROC)loadEglProc("glTexBufferRange"); 
	fnptr_ci_glTexStorage3DMultisample = (PFNGLTEXSTORAGE3DMULTISAMPLEPROC)loadEglProc("glTexStorage3DMultisample"); 

	DEBUG( "gl_es_3_2_load: SUCCESSFUL! | " << getLoadCount() << " procs loaded");
}
#endif // defined( CINDER_LINUX ) && ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3_2 )

// ----------------------------------------------------------------------------
// OpenGL ES 2.0 Extensions
// ----------------------------------------------------------------------------
#if ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_2 )

#define extstr_GL_NV_read_buffer 			EXT_QUOTE_STR(GL_NV_read_buffer)
#define extstr_GL_OES_vertex_array_object	EXT_QUOTE_STR(GL_OES_vertex_array_object)
#define extstr_GL_ARB_vertex_array_object	EXT_QUOTE_STR(GL_ARB_vertex_array_object)

PFNGLBLENDBARRIERKHRPROC fnptr_ci_glBlendBarrierKHR = nullptr; 
PFNGLDEBUGMESSAGECONTROLKHRPROC fnptr_ci_glDebugMessageControlKHR = nullptr; 
PFNGLDEBUGMESSAGEINSERTKHRPROC fnptr_ci_glDebugMessageInsertKHR = nullptr; 
PFNGLDEBUGMESSAGECALLBACKKHRPROC fnptr_ci_glDebugMessageCallbackKHR = nullptr; 
PFNGLGETDEBUGMESSAGELOGKHRPROC fnptr_ci_glGetDebugMessageLogKHR = nullptr; 
PFNGLPUSHDEBUGGROUPKHRPROC fnptr_ci_glPushDebugGroupKHR = nullptr; 
PFNGLPOPDEBUGGROUPKHRPROC fnptr_ci_glPopDebugGroupKHR = nullptr; 
PFNGLOBJECTLABELKHRPROC fnptr_ci_glObjectLabelKHR = nullptr; 
PFNGLGETOBJECTLABELKHRPROC fnptr_ci_glGetObjectLabelKHR = nullptr; 
PFNGLOBJECTPTRLABELKHRPROC fnptr_ci_glObjectPtrLabelKHR = nullptr; 
PFNGLGETOBJECTPTRLABELKHRPROC fnptr_ci_glGetObjectPtrLabelKHR = nullptr; 
PFNGLGETPOINTERVKHRPROC fnptr_ci_glGetPointervKHR = nullptr; 
PFNGLGETGRAPHICSRESETSTATUSKHRPROC fnptr_ci_glGetGraphicsResetStatusKHR = nullptr; 
PFNGLREADNPIXELSKHRPROC fnptr_ci_glReadnPixelsKHR = nullptr; 
PFNGLGETNUNIFORMFVKHRPROC fnptr_ci_glGetnUniformfvKHR = nullptr; 
PFNGLGETNUNIFORMIVKHRPROC fnptr_ci_glGetnUniformivKHR = nullptr; 
PFNGLGETNUNIFORMUIVKHRPROC fnptr_ci_glGetnUniformuivKHR = nullptr; 
PFNGLEGLIMAGETARGETTEXTURE2DOESPROC fnptr_ci_glEGLImageTargetTexture2DOES = nullptr; 
PFNGLEGLIMAGETARGETRENDERBUFFERSTORAGEOESPROC fnptr_ci_glEGLImageTargetRenderbufferStorageOES = nullptr; 
PFNGLCOPYIMAGESUBDATAOESPROC fnptr_ci_glCopyImageSubDataOES = nullptr; 
PFNGLENABLEIOESPROC fnptr_ci_glEnableiOES = nullptr; 
PFNGLDISABLEIOESPROC fnptr_ci_glDisableiOES = nullptr; 
PFNGLBLENDEQUATIONIOESPROC fnptr_ci_glBlendEquationiOES = nullptr; 
PFNGLBLENDEQUATIONSEPARATEIOESPROC fnptr_ci_glBlendEquationSeparateiOES = nullptr; 
PFNGLBLENDFUNCIOESPROC fnptr_ci_glBlendFunciOES = nullptr; 
PFNGLBLENDFUNCSEPARATEIOESPROC fnptr_ci_glBlendFuncSeparateiOES = nullptr; 
PFNGLCOLORMASKIOESPROC fnptr_ci_glColorMaskiOES = nullptr; 
PFNGLISENABLEDIOESPROC fnptr_ci_glIsEnablediOES = nullptr; 
PFNGLDRAWELEMENTSBASEVERTEXOESPROC fnptr_ci_glDrawElementsBaseVertexOES = nullptr; 
PFNGLDRAWRANGEELEMENTSBASEVERTEXOESPROC fnptr_ci_glDrawRangeElementsBaseVertexOES = nullptr; 
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXOESPROC fnptr_ci_glDrawElementsInstancedBaseVertexOES = nullptr; 
PFNGLMULTIDRAWELEMENTSBASEVERTEXOESPROC fnptr_ci_glMultiDrawElementsBaseVertexOES = nullptr; 
PFNGLFRAMEBUFFERTEXTUREOESPROC fnptr_ci_glFramebufferTextureOES = nullptr; 
PFNGLGETPROGRAMBINARYOESPROC fnptr_ci_glGetProgramBinaryOES = nullptr; 
PFNGLPROGRAMBINARYOESPROC fnptr_ci_glProgramBinaryOES = nullptr; 
PFNGLGETBUFFERPOINTERVOESPROC fnptr_ci_glGetBufferPointervOES = nullptr; 
PFNGLPRIMITIVEBOUNDINGBOXOESPROC fnptr_ci_glPrimitiveBoundingBoxOES = nullptr; 
PFNGLMINSAMPLESHADINGOESPROC fnptr_ci_glMinSampleShadingOES = nullptr;
PFNGLTEXIMAGE3DOESPROC fnptr_ci_glTexImage3DOES = nullptr; 
PFNGLTEXSUBIMAGE3DOESPROC fnptr_ci_glTexSubImage3DOES = nullptr; 
PFNGLCOPYTEXSUBIMAGE3DOESPROC fnptr_ci_glCopyTexSubImage3DOES = nullptr; 
PFNGLCOMPRESSEDTEXIMAGE3DOESPROC fnptr_ci_glCompressedTexImage3DOES = nullptr; 
PFNGLCOMPRESSEDTEXSUBIMAGE3DOESPROC fnptr_ci_glCompressedTexSubImage3DOES = nullptr; 
PFNGLFRAMEBUFFERTEXTURE3DOESPROC fnptr_ci_glFramebufferTexture3DOES = nullptr; 
PFNGLTEXPARAMETERIIVOESPROC fnptr_ci_glTexParameterIivOES = nullptr; 
PFNGLTEXPARAMETERIUIVOESPROC fnptr_ci_glTexParameterIuivOES = nullptr; 
PFNGLGETTEXPARAMETERIIVOESPROC fnptr_ci_glGetTexParameterIivOES = nullptr; 
PFNGLGETTEXPARAMETERIUIVOESPROC fnptr_ci_glGetTexParameterIuivOES = nullptr; 
PFNGLSAMPLERPARAMETERIIVOESPROC fnptr_ci_glSamplerParameterIivOES = nullptr; 
PFNGLSAMPLERPARAMETERIUIVOESPROC fnptr_ci_glSamplerParameterIuivOES = nullptr; 
PFNGLGETSAMPLERPARAMETERIIVOESPROC fnptr_ci_glGetSamplerParameterIivOES = nullptr; 
PFNGLGETSAMPLERPARAMETERIUIVOESPROC fnptr_ci_glGetSamplerParameterIuivOES = nullptr; 
PFNGLTEXBUFFEROESPROC fnptr_ci_glTexBufferOES = nullptr; 
PFNGLTEXBUFFERRANGEOESPROC fnptr_ci_glTexBufferRangeOES = nullptr; 
PFNGLTEXSTORAGE3DMULTISAMPLEOESPROC fnptr_ci_glTexStorage3DMultisampleOES = nullptr; 
PFNGLTEXTUREVIEWOESPROC fnptr_ci_glTextureViewOES = nullptr; 
PFNGLGETPERFMONITORGROUPSAMDPROC fnptr_ci_glGetPerfMonitorGroupsAMD = nullptr; 
PFNGLGETPERFMONITORCOUNTERSAMDPROC fnptr_ci_glGetPerfMonitorCountersAMD = nullptr; 
PFNGLGETPERFMONITORGROUPSTRINGAMDPROC fnptr_ci_glGetPerfMonitorGroupStringAMD = nullptr; 
PFNGLGETPERFMONITORCOUNTERSTRINGAMDPROC fnptr_ci_glGetPerfMonitorCounterStringAMD = nullptr; 
PFNGLGETPERFMONITORCOUNTERINFOAMDPROC fnptr_ci_glGetPerfMonitorCounterInfoAMD = nullptr; 
PFNGLGENPERFMONITORSAMDPROC fnptr_ci_glGenPerfMonitorsAMD = nullptr; 
PFNGLDELETEPERFMONITORSAMDPROC fnptr_ci_glDeletePerfMonitorsAMD = nullptr; 
PFNGLSELECTPERFMONITORCOUNTERSAMDPROC fnptr_ci_glSelectPerfMonitorCountersAMD = nullptr; 
PFNGLBEGINPERFMONITORAMDPROC fnptr_ci_glBeginPerfMonitorAMD = nullptr; 
PFNGLENDPERFMONITORAMDPROC fnptr_ci_glEndPerfMonitorAMD = nullptr; 
PFNGLGETPERFMONITORCOUNTERDATAAMDPROC fnptr_ci_glGetPerfMonitorCounterDataAMD = nullptr; 
PFNGLGETTRANSLATEDSHADERSOURCEANGLEPROC fnptr_ci_glGetTranslatedShaderSourceANGLE = nullptr; 
PFNGLCOPYTEXTURELEVELSAPPLEPROC fnptr_ci_glCopyTextureLevelsAPPLE = nullptr; 
PFNGLRESOLVEMULTISAMPLEFRAMEBUFFERAPPLEPROC fnptr_ci_glResolveMultisampleFramebufferAPPLE = nullptr; 
PFNGLFENCESYNCAPPLEPROC fnptr_ci_glFenceSyncAPPLE = nullptr; 
PFNGLISSYNCAPPLEPROC fnptr_ci_glIsSyncAPPLE = nullptr; 
PFNGLDELETESYNCAPPLEPROC fnptr_ci_glDeleteSyncAPPLE = nullptr; 
PFNGLCLIENTWAITSYNCAPPLEPROC fnptr_ci_glClientWaitSyncAPPLE = nullptr; 
PFNGLWAITSYNCAPPLEPROC fnptr_ci_glWaitSyncAPPLE = nullptr; 
PFNGLGETINTEGER64VAPPLEPROC fnptr_ci_glGetInteger64vAPPLE = nullptr; 
PFNGLGETSYNCIVAPPLEPROC fnptr_ci_glGetSyncivAPPLE = nullptr; 
PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEEXTPROC fnptr_ci_glDrawArraysInstancedBaseInstanceEXT = nullptr; 
PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEEXTPROC fnptr_ci_glDrawElementsInstancedBaseInstanceEXT = nullptr; 
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEEXTPROC fnptr_ci_glDrawElementsInstancedBaseVertexBaseInstanceEXT = nullptr; 
PFNGLBINDFRAGDATALOCATIONINDEXEDEXTPROC fnptr_ci_glBindFragDataLocationIndexedEXT = nullptr; 
PFNGLBINDFRAGDATALOCATIONEXTPROC fnptr_ci_glBindFragDataLocationEXT = nullptr; 
PFNGLGETPROGRAMRESOURCELOCATIONINDEXEXTPROC fnptr_ci_glGetProgramResourceLocationIndexEXT = nullptr; 
PFNGLGETFRAGDATAINDEXEXTPROC fnptr_ci_glGetFragDataIndexEXT = nullptr; 
PFNGLCOPYIMAGESUBDATAEXTPROC fnptr_ci_glCopyImageSubDataEXT = nullptr; 
PFNGLLABELOBJECTEXTPROC fnptr_ci_glLabelObjectEXT = nullptr; 
PFNGLGETOBJECTLABELEXTPROC fnptr_ci_glGetObjectLabelEXT = nullptr; 
PFNGLINSERTEVENTMARKEREXTPROC fnptr_ci_glInsertEventMarkerEXT = nullptr; 
PFNGLPUSHGROUPMARKEREXTPROC fnptr_ci_glPushGroupMarkerEXT = nullptr; 
PFNGLPOPGROUPMARKEREXTPROC fnptr_ci_glPopGroupMarkerEXT = nullptr; 
PFNGLDISCARDFRAMEBUFFEREXTPROC fnptr_ci_glDiscardFramebufferEXT = nullptr; 
PFNGLGENQUERIESEXTPROC fnptr_ci_glGenQueriesEXT = nullptr; 
PFNGLDELETEQUERIESEXTPROC fnptr_ci_glDeleteQueriesEXT = nullptr; 
PFNGLISQUERYEXTPROC fnptr_ci_glIsQueryEXT = nullptr; 
PFNGLBEGINQUERYEXTPROC fnptr_ci_glBeginQueryEXT = nullptr; 
PFNGLENDQUERYEXTPROC fnptr_ci_glEndQueryEXT = nullptr; 
PFNGLQUERYCOUNTEREXTPROC fnptr_ci_glQueryCounterEXT = nullptr; 
PFNGLGETQUERYIVEXTPROC fnptr_ci_glGetQueryivEXT = nullptr; 
PFNGLGETQUERYOBJECTIVEXTPROC fnptr_ci_glGetQueryObjectivEXT = nullptr; 
PFNGLGETQUERYOBJECTUIVEXTPROC fnptr_ci_glGetQueryObjectuivEXT = nullptr; 
PFNGLGETQUERYOBJECTI64VEXTPROC fnptr_ci_glGetQueryObjecti64vEXT = nullptr; 
PFNGLGETQUERYOBJECTUI64VEXTPROC fnptr_ci_glGetQueryObjectui64vEXT = nullptr; 
PFNGLDRAWBUFFERSEXTPROC fnptr_ci_glDrawBuffersEXT = nullptr; 
PFNGLENABLEIEXTPROC fnptr_ci_glEnableiEXT = nullptr; 
PFNGLDISABLEIEXTPROC fnptr_ci_glDisableiEXT = nullptr; 
PFNGLBLENDEQUATIONIEXTPROC fnptr_ci_glBlendEquationiEXT = nullptr; 
PFNGLBLENDEQUATIONSEPARATEIEXTPROC fnptr_ci_glBlendEquationSeparateiEXT = nullptr; 
PFNGLBLENDFUNCIEXTPROC fnptr_ci_glBlendFunciEXT = nullptr; 
PFNGLBLENDFUNCSEPARATEIEXTPROC fnptr_ci_glBlendFuncSeparateiEXT = nullptr; 
PFNGLCOLORMASKIEXTPROC fnptr_ci_glColorMaskiEXT = nullptr; 
PFNGLISENABLEDIEXTPROC fnptr_ci_glIsEnablediEXT = nullptr; 
PFNGLDRAWELEMENTSBASEVERTEXEXTPROC fnptr_ci_glDrawElementsBaseVertexEXT = nullptr; 
PFNGLDRAWRANGEELEMENTSBASEVERTEXEXTPROC fnptr_ci_glDrawRangeElementsBaseVertexEXT = nullptr; 
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXEXTPROC fnptr_ci_glDrawElementsInstancedBaseVertexEXT = nullptr; 
PFNGLMULTIDRAWELEMENTSBASEVERTEXEXTPROC fnptr_ci_glMultiDrawElementsBaseVertexEXT = nullptr; 
PFNGLMULTIDRAWARRAYSEXTPROC fnptr_ci_glMultiDrawArraysEXT = nullptr; 
PFNGLMULTIDRAWELEMENTSEXTPROC fnptr_ci_glMultiDrawElementsEXT = nullptr; 
PFNGLMULTIDRAWARRAYSINDIRECTEXTPROC fnptr_ci_glMultiDrawArraysIndirectEXT = nullptr; 
PFNGLMULTIDRAWELEMENTSINDIRECTEXTPROC fnptr_ci_glMultiDrawElementsIndirectEXT = nullptr; 
PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC fnptr_ci_glFramebufferTexture2DMultisampleEXT = nullptr; 
PFNGLREADBUFFERINDEXEDEXTPROC fnptr_ci_glReadBufferIndexedEXT = nullptr; 
PFNGLDRAWBUFFERSINDEXEDEXTPROC fnptr_ci_glDrawBuffersIndexedEXT = nullptr; 
PFNGLGETINTEGERI_VEXTPROC fnptr_ci_glGetIntegeri_vEXT = nullptr; 
PFNGLPRIMITIVEBOUNDINGBOXEXTPROC fnptr_ci_glPrimitiveBoundingBoxEXT = nullptr; 
PFNGLRASTERSAMPLESEXTPROC fnptr_ci_glRasterSamplesEXT = nullptr; 
PFNGLGETGRAPHICSRESETSTATUSEXTPROC fnptr_ci_glGetGraphicsResetStatusEXT = nullptr; 
PFNGLREADNPIXELSEXTPROC fnptr_ci_glReadnPixelsEXT = nullptr; 
PFNGLGETNUNIFORMFVEXTPROC fnptr_ci_glGetnUniformfvEXT = nullptr; 
PFNGLGETNUNIFORMIVEXTPROC fnptr_ci_glGetnUniformivEXT = nullptr; 
PFNGLACTIVESHADERPROGRAMEXTPROC fnptr_ci_glActiveShaderProgramEXT = nullptr; 
PFNGLBINDPROGRAMPIPELINEEXTPROC fnptr_ci_glBindProgramPipelineEXT = nullptr; 
PFNGLCREATESHADERPROGRAMVEXTPROC fnptr_ci_glCreateShaderProgramvEXT = nullptr; 
PFNGLDELETEPROGRAMPIPELINESEXTPROC fnptr_ci_glDeleteProgramPipelinesEXT = nullptr; 
PFNGLGENPROGRAMPIPELINESEXTPROC fnptr_ci_glGenProgramPipelinesEXT = nullptr; 
PFNGLGETPROGRAMPIPELINEINFOLOGEXTPROC fnptr_ci_glGetProgramPipelineInfoLogEXT = nullptr; 
PFNGLGETPROGRAMPIPELINEIVEXTPROC fnptr_ci_glGetProgramPipelineivEXT = nullptr; 
PFNGLISPROGRAMPIPELINEEXTPROC fnptr_ci_glIsProgramPipelineEXT = nullptr; 
PFNGLPROGRAMPARAMETERIEXTPROC fnptr_ci_glProgramParameteriEXT = nullptr; 
PFNGLPROGRAMUNIFORM1FEXTPROC fnptr_ci_glProgramUniform1fEXT = nullptr; 
PFNGLPROGRAMUNIFORM1FVEXTPROC fnptr_ci_glProgramUniform1fvEXT = nullptr; 
PFNGLPROGRAMUNIFORM1IEXTPROC fnptr_ci_glProgramUniform1iEXT = nullptr; 
PFNGLPROGRAMUNIFORM1IVEXTPROC fnptr_ci_glProgramUniform1ivEXT = nullptr; 
PFNGLPROGRAMUNIFORM2FEXTPROC fnptr_ci_glProgramUniform2fEXT = nullptr; 
PFNGLPROGRAMUNIFORM2FVEXTPROC fnptr_ci_glProgramUniform2fvEXT = nullptr; 
PFNGLPROGRAMUNIFORM2IEXTPROC fnptr_ci_glProgramUniform2iEXT = nullptr; 
PFNGLPROGRAMUNIFORM2IVEXTPROC fnptr_ci_glProgramUniform2ivEXT = nullptr; 
PFNGLPROGRAMUNIFORM3FEXTPROC fnptr_ci_glProgramUniform3fEXT = nullptr; 
PFNGLPROGRAMUNIFORM3FVEXTPROC fnptr_ci_glProgramUniform3fvEXT = nullptr; 
PFNGLPROGRAMUNIFORM3IEXTPROC fnptr_ci_glProgramUniform3iEXT = nullptr; 
PFNGLPROGRAMUNIFORM3IVEXTPROC fnptr_ci_glProgramUniform3ivEXT = nullptr; 
PFNGLPROGRAMUNIFORM4FEXTPROC fnptr_ci_glProgramUniform4fEXT = nullptr; 
PFNGLPROGRAMUNIFORM4FVEXTPROC fnptr_ci_glProgramUniform4fvEXT = nullptr; 
PFNGLPROGRAMUNIFORM4IEXTPROC fnptr_ci_glProgramUniform4iEXT = nullptr; 
PFNGLPROGRAMUNIFORM4IVEXTPROC fnptr_ci_glProgramUniform4ivEXT = nullptr; 
PFNGLPROGRAMUNIFORMMATRIX2FVEXTPROC fnptr_ci_glProgramUniformMatrix2fvEXT = nullptr; 
PFNGLPROGRAMUNIFORMMATRIX3FVEXTPROC fnptr_ci_glProgramUniformMatrix3fvEXT = nullptr; 
PFNGLPROGRAMUNIFORMMATRIX4FVEXTPROC fnptr_ci_glProgramUniformMatrix4fvEXT = nullptr; 
PFNGLUSEPROGRAMSTAGESEXTPROC fnptr_ci_glUseProgramStagesEXT = nullptr; 
PFNGLVALIDATEPROGRAMPIPELINEEXTPROC fnptr_ci_glValidateProgramPipelineEXT = nullptr; 
PFNGLPROGRAMUNIFORM1UIEXTPROC fnptr_ci_glProgramUniform1uiEXT = nullptr; 
PFNGLPROGRAMUNIFORM2UIEXTPROC fnptr_ci_glProgramUniform2uiEXT = nullptr; 
PFNGLPROGRAMUNIFORM3UIEXTPROC fnptr_ci_glProgramUniform3uiEXT = nullptr; 
PFNGLPROGRAMUNIFORM4UIEXTPROC fnptr_ci_glProgramUniform4uiEXT = nullptr; 
PFNGLPROGRAMUNIFORM1UIVEXTPROC fnptr_ci_glProgramUniform1uivEXT = nullptr; 
PFNGLPROGRAMUNIFORM2UIVEXTPROC fnptr_ci_glProgramUniform2uivEXT = nullptr; 
PFNGLPROGRAMUNIFORM3UIVEXTPROC fnptr_ci_glProgramUniform3uivEXT = nullptr; 
PFNGLPROGRAMUNIFORM4UIVEXTPROC fnptr_ci_glProgramUniform4uivEXT = nullptr; 
PFNGLPROGRAMUNIFORMMATRIX2X3FVEXTPROC fnptr_ci_glProgramUniformMatrix2x3fvEXT = nullptr; 
PFNGLPROGRAMUNIFORMMATRIX3X2FVEXTPROC fnptr_ci_glProgramUniformMatrix3x2fvEXT = nullptr; 
PFNGLPROGRAMUNIFORMMATRIX2X4FVEXTPROC fnptr_ci_glProgramUniformMatrix2x4fvEXT = nullptr; 
PFNGLPROGRAMUNIFORMMATRIX4X2FVEXTPROC fnptr_ci_glProgramUniformMatrix4x2fvEXT = nullptr; 
PFNGLPROGRAMUNIFORMMATRIX3X4FVEXTPROC fnptr_ci_glProgramUniformMatrix3x4fvEXT = nullptr; 
PFNGLPROGRAMUNIFORMMATRIX4X3FVEXTPROC fnptr_ci_glProgramUniformMatrix4x3fvEXT = nullptr; 
PFNGLTEXPAGECOMMITMENTEXTPROC fnptr_ci_glTexPageCommitmentEXT = nullptr;
PFNGLTEXPARAMETERIIVEXTPROC fnptr_ci_glTexParameterIivEXT = nullptr; 
PFNGLTEXPARAMETERIUIVEXTPROC fnptr_ci_glTexParameterIuivEXT = nullptr; 
PFNGLGETTEXPARAMETERIIVEXTPROC fnptr_ci_glGetTexParameterIivEXT = nullptr; 
PFNGLGETTEXPARAMETERIUIVEXTPROC fnptr_ci_glGetTexParameterIuivEXT = nullptr; 
PFNGLSAMPLERPARAMETERIIVEXTPROC fnptr_ci_glSamplerParameterIivEXT = nullptr; 
PFNGLSAMPLERPARAMETERIUIVEXTPROC fnptr_ci_glSamplerParameterIuivEXT = nullptr; 
PFNGLGETSAMPLERPARAMETERIIVEXTPROC fnptr_ci_glGetSamplerParameterIivEXT = nullptr; 
PFNGLGETSAMPLERPARAMETERIUIVEXTPROC fnptr_ci_glGetSamplerParameterIuivEXT = nullptr; 
PFNGLTEXBUFFEREXTPROC fnptr_ci_glTexBufferEXT = nullptr; 
PFNGLTEXBUFFERRANGEEXTPROC fnptr_ci_glTexBufferRangeEXT = nullptr; 
PFNGLTEXSTORAGE1DEXTPROC fnptr_ci_glTexStorage1DEXT = nullptr; 
PFNGLTEXSTORAGE2DEXTPROC fnptr_ci_glTexStorage2DEXT = nullptr; 
PFNGLTEXSTORAGE3DEXTPROC fnptr_ci_glTexStorage3DEXT = nullptr; 
PFNGLTEXTURESTORAGE1DEXTPROC fnptr_ci_glTextureStorage1DEXT = nullptr; 
PFNGLTEXTURESTORAGE2DEXTPROC fnptr_ci_glTextureStorage2DEXT = nullptr; 
PFNGLTEXTURESTORAGE3DEXTPROC fnptr_ci_glTextureStorage3DEXT = nullptr; 
PFNGLTEXTUREVIEWEXTPROC fnptr_ci_glTextureViewEXT = nullptr; 
PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMGPROC fnptr_ci_glFramebufferTexture2DMultisampleIMG = nullptr; 
PFNGLAPPLYFRAMEBUFFERATTACHMENTCMAAINTELPROC fnptr_ci_glApplyFramebufferAttachmentCMAAINTEL = nullptr; 
PFNGLBEGINPERFQUERYINTELPROC fnptr_ci_glBeginPerfQueryINTEL = nullptr; 
PFNGLCREATEPERFQUERYINTELPROC fnptr_ci_glCreatePerfQueryINTEL = nullptr; 
PFNGLDELETEPERFQUERYINTELPROC fnptr_ci_glDeletePerfQueryINTEL = nullptr; 
PFNGLENDPERFQUERYINTELPROC fnptr_ci_glEndPerfQueryINTEL = nullptr; 
PFNGLGETFIRSTPERFQUERYIDINTELPROC fnptr_ci_glGetFirstPerfQueryIdINTEL = nullptr; 
PFNGLGETNEXTPERFQUERYIDINTELPROC fnptr_ci_glGetNextPerfQueryIdINTEL = nullptr; 
PFNGLGETPERFCOUNTERINFOINTELPROC fnptr_ci_glGetPerfCounterInfoINTEL = nullptr; 
PFNGLGETPERFQUERYDATAINTELPROC fnptr_ci_glGetPerfQueryDataINTEL = nullptr; 
PFNGLGETPERFQUERYIDBYNAMEINTELPROC fnptr_ci_glGetPerfQueryIdByNameINTEL = nullptr; 
PFNGLGETPERFQUERYINFOINTELPROC fnptr_ci_glGetPerfQueryInfoINTEL = nullptr; 
PFNGLGETTEXTUREHANDLENVPROC fnptr_ci_glGetTextureHandleNV = nullptr; 
PFNGLGETTEXTURESAMPLERHANDLENVPROC fnptr_ci_glGetTextureSamplerHandleNV = nullptr; 
PFNGLMAKETEXTUREHANDLERESIDENTNVPROC fnptr_ci_glMakeTextureHandleResidentNV = nullptr; 
PFNGLMAKETEXTUREHANDLENONRESIDENTNVPROC fnptr_ci_glMakeTextureHandleNonResidentNV = nullptr; 
PFNGLGETIMAGEHANDLENVPROC fnptr_ci_glGetImageHandleNV = nullptr; 
PFNGLMAKEIMAGEHANDLERESIDENTNVPROC fnptr_ci_glMakeImageHandleResidentNV = nullptr; 
PFNGLMAKEIMAGEHANDLENONRESIDENTNVPROC fnptr_ci_glMakeImageHandleNonResidentNV = nullptr; 
PFNGLUNIFORMHANDLEUI64NVPROC fnptr_ci_glUniformHandleui64NV = nullptr; 
PFNGLUNIFORMHANDLEUI64VNVPROC fnptr_ci_glUniformHandleui64vNV = nullptr; 
PFNGLPROGRAMUNIFORMHANDLEUI64NVPROC fnptr_ci_glProgramUniformHandleui64NV = nullptr; 
PFNGLPROGRAMUNIFORMHANDLEUI64VNVPROC fnptr_ci_glProgramUniformHandleui64vNV = nullptr; 
PFNGLISTEXTUREHANDLERESIDENTNVPROC fnptr_ci_glIsTextureHandleResidentNV = nullptr; 
PFNGLISIMAGEHANDLERESIDENTNVPROC fnptr_ci_glIsImageHandleResidentNV = nullptr; 
PFNGLBLENDPARAMETERINVPROC fnptr_ci_glBlendParameteriNV = nullptr; 
PFNGLBLENDBARRIERNVPROC fnptr_ci_glBlendBarrierNV = nullptr; 
PFNGLBEGINCONDITIONALRENDERNVPROC fnptr_ci_glBeginConditionalRenderNV = nullptr; 
PFNGLENDCONDITIONALRENDERNVPROC fnptr_ci_glEndConditionalRenderNV = nullptr; 
PFNGLSUBPIXELPRECISIONBIASNVPROC fnptr_ci_glSubpixelPrecisionBiasNV = nullptr; 
PFNGLCOPYBUFFERSUBDATANVPROC fnptr_ci_glCopyBufferSubDataNV = nullptr; 
PFNGLCOVERAGEMASKNVPROC fnptr_ci_glCoverageMaskNV = nullptr; 
PFNGLCOVERAGEOPERATIONNVPROC fnptr_ci_glCoverageOperationNV = nullptr; 
PFNGLDRAWBUFFERSNVPROC fnptr_ci_glDrawBuffersNV = nullptr; 
PFNGLDELETEFENCESNVPROC fnptr_ci_glDeleteFencesNV = nullptr; 
PFNGLGENFENCESNVPROC fnptr_ci_glGenFencesNV = nullptr; 
PFNGLISFENCENVPROC fnptr_ci_glIsFenceNV = nullptr; 
PFNGLTESTFENCENVPROC fnptr_ci_glTestFenceNV = nullptr; 
PFNGLGETFENCEIVNVPROC fnptr_ci_glGetFenceivNV = nullptr; 
PFNGLFINISHFENCENVPROC fnptr_ci_glFinishFenceNV = nullptr; 
PFNGLSETFENCENVPROC fnptr_ci_glSetFenceNV = nullptr; 
PFNGLFRAGMENTCOVERAGECOLORNVPROC fnptr_ci_glFragmentCoverageColorNV = nullptr; 
PFNGLCOVERAGEMODULATIONTABLENVPROC fnptr_ci_glCoverageModulationTableNV = nullptr; 
PFNGLGETCOVERAGEMODULATIONTABLENVPROC fnptr_ci_glGetCoverageModulationTableNV = nullptr; 
PFNGLCOVERAGEMODULATIONNVPROC fnptr_ci_glCoverageModulationNV = nullptr; 
PFNGLGETINTERNALFORMATSAMPLEIVNVPROC fnptr_ci_glGetInternalformatSampleivNV = nullptr; 
PFNGLUNIFORMMATRIX2X3FVNVPROC fnptr_ci_glUniformMatrix2x3fvNV = nullptr; 
PFNGLUNIFORMMATRIX3X2FVNVPROC fnptr_ci_glUniformMatrix3x2fvNV = nullptr; 
PFNGLUNIFORMMATRIX2X4FVNVPROC fnptr_ci_glUniformMatrix2x4fvNV = nullptr; 
PFNGLUNIFORMMATRIX4X2FVNVPROC fnptr_ci_glUniformMatrix4x2fvNV = nullptr; 
PFNGLUNIFORMMATRIX3X4FVNVPROC fnptr_ci_glUniformMatrix3x4fvNV = nullptr; 
PFNGLUNIFORMMATRIX4X3FVNVPROC fnptr_ci_glUniformMatrix4x3fvNV = nullptr; 
PFNGLGENPATHSNVPROC fnptr_ci_glGenPathsNV = nullptr; 
PFNGLDELETEPATHSNVPROC fnptr_ci_glDeletePathsNV = nullptr; 
PFNGLISPATHNVPROC fnptr_ci_glIsPathNV = nullptr; 
PFNGLPATHCOMMANDSNVPROC fnptr_ci_glPathCommandsNV = nullptr; 
PFNGLPATHCOORDSNVPROC fnptr_ci_glPathCoordsNV = nullptr; 
PFNGLPATHSUBCOMMANDSNVPROC fnptr_ci_glPathSubCommandsNV = nullptr; 
PFNGLPATHSUBCOORDSNVPROC fnptr_ci_glPathSubCoordsNV = nullptr; 
PFNGLPATHSTRINGNVPROC fnptr_ci_glPathStringNV = nullptr; 
PFNGLPATHGLYPHSNVPROC fnptr_ci_glPathGlyphsNV = nullptr; 
PFNGLPATHGLYPHRANGENVPROC fnptr_ci_glPathGlyphRangeNV = nullptr; 
PFNGLWEIGHTPATHSNVPROC fnptr_ci_glWeightPathsNV = nullptr; 
PFNGLCOPYPATHNVPROC fnptr_ci_glCopyPathNV = nullptr; 
PFNGLINTERPOLATEPATHSNVPROC fnptr_ci_glInterpolatePathsNV = nullptr; 
PFNGLTRANSFORMPATHNVPROC fnptr_ci_glTransformPathNV = nullptr; 
PFNGLPATHPARAMETERIVNVPROC fnptr_ci_glPathParameterivNV = nullptr; 
PFNGLPATHPARAMETERINVPROC fnptr_ci_glPathParameteriNV = nullptr; 
PFNGLPATHPARAMETERFVNVPROC fnptr_ci_glPathParameterfvNV = nullptr; 
PFNGLPATHPARAMETERFNVPROC fnptr_ci_glPathParameterfNV = nullptr; 
PFNGLPATHDASHARRAYNVPROC fnptr_ci_glPathDashArrayNV = nullptr; 
PFNGLPATHSTENCILFUNCNVPROC fnptr_ci_glPathStencilFuncNV = nullptr; 
PFNGLPATHSTENCILDEPTHOFFSETNVPROC fnptr_ci_glPathStencilDepthOffsetNV = nullptr; 
PFNGLSTENCILFILLPATHNVPROC fnptr_ci_glStencilFillPathNV = nullptr; 
PFNGLSTENCILSTROKEPATHNVPROC fnptr_ci_glStencilStrokePathNV = nullptr; 
PFNGLSTENCILFILLPATHINSTANCEDNVPROC fnptr_ci_glStencilFillPathInstancedNV = nullptr; 
PFNGLSTENCILSTROKEPATHINSTANCEDNVPROC fnptr_ci_glStencilStrokePathInstancedNV = nullptr; 
PFNGLPATHCOVERDEPTHFUNCNVPROC fnptr_ci_glPathCoverDepthFuncNV = nullptr; 
PFNGLCOVERFILLPATHNVPROC fnptr_ci_glCoverFillPathNV = nullptr; 
PFNGLCOVERSTROKEPATHNVPROC fnptr_ci_glCoverStrokePathNV = nullptr; 
PFNGLCOVERFILLPATHINSTANCEDNVPROC fnptr_ci_glCoverFillPathInstancedNV = nullptr; 
PFNGLCOVERSTROKEPATHINSTANCEDNVPROC fnptr_ci_glCoverStrokePathInstancedNV = nullptr; 
PFNGLGETPATHPARAMETERIVNVPROC fnptr_ci_glGetPathParameterivNV = nullptr; 
PFNGLGETPATHPARAMETERFVNVPROC fnptr_ci_glGetPathParameterfvNV = nullptr; 
PFNGLGETPATHCOMMANDSNVPROC fnptr_ci_glGetPathCommandsNV = nullptr; 
PFNGLGETPATHCOORDSNVPROC fnptr_ci_glGetPathCoordsNV = nullptr; 
PFNGLGETPATHDASHARRAYNVPROC fnptr_ci_glGetPathDashArrayNV = nullptr; 
PFNGLGETPATHMETRICSNVPROC fnptr_ci_glGetPathMetricsNV = nullptr; 
PFNGLGETPATHMETRICRANGENVPROC fnptr_ci_glGetPathMetricRangeNV = nullptr; 
PFNGLGETPATHSPACINGNVPROC fnptr_ci_glGetPathSpacingNV = nullptr; 
PFNGLISPOINTINFILLPATHNVPROC fnptr_ci_glIsPointInFillPathNV = nullptr; 
PFNGLISPOINTINSTROKEPATHNVPROC fnptr_ci_glIsPointInStrokePathNV = nullptr; 
PFNGLGETPATHLENGTHNVPROC fnptr_ci_glGetPathLengthNV = nullptr; 
PFNGLPOINTALONGPATHNVPROC fnptr_ci_glPointAlongPathNV = nullptr; 
PFNGLMATRIXLOAD3X2FNVPROC fnptr_ci_glMatrixLoad3x2fNV = nullptr; 
PFNGLMATRIXLOAD3X3FNVPROC fnptr_ci_glMatrixLoad3x3fNV = nullptr; 
PFNGLMATRIXLOADTRANSPOSE3X3FNVPROC fnptr_ci_glMatrixLoadTranspose3x3fNV = nullptr; 
PFNGLMATRIXMULT3X2FNVPROC fnptr_ci_glMatrixMult3x2fNV = nullptr; 
PFNGLMATRIXMULT3X3FNVPROC fnptr_ci_glMatrixMult3x3fNV = nullptr; 
PFNGLMATRIXMULTTRANSPOSE3X3FNVPROC fnptr_ci_glMatrixMultTranspose3x3fNV = nullptr; 
PFNGLSTENCILTHENCOVERFILLPATHNVPROC fnptr_ci_glStencilThenCoverFillPathNV = nullptr; 
PFNGLSTENCILTHENCOVERSTROKEPATHNVPROC fnptr_ci_glStencilThenCoverStrokePathNV = nullptr; 
PFNGLSTENCILTHENCOVERFILLPATHINSTANCEDNVPROC fnptr_ci_glStencilThenCoverFillPathInstancedNV = nullptr; 
PFNGLSTENCILTHENCOVERSTROKEPATHINSTANCEDNVPROC fnptr_ci_glStencilThenCoverStrokePathInstancedNV = nullptr; 
PFNGLPATHGLYPHINDEXRANGENVPROC fnptr_ci_glPathGlyphIndexRangeNV = nullptr; 
PFNGLPATHGLYPHINDEXARRAYNVPROC fnptr_ci_glPathGlyphIndexArrayNV = nullptr; 
PFNGLPATHMEMORYGLYPHINDEXARRAYNVPROC fnptr_ci_glPathMemoryGlyphIndexArrayNV = nullptr; 
PFNGLPROGRAMPATHFRAGMENTINPUTGENNVPROC fnptr_ci_glProgramPathFragmentInputGenNV = nullptr; 
PFNGLGETPROGRAMRESOURCEFVNVPROC fnptr_ci_glGetProgramResourcefvNV = nullptr; 
PFNGLPOLYGONMODENVPROC fnptr_ci_glPolygonModeNV = nullptr; 
PFNGLREADBUFFERNVPROC fnptr_ci_glReadBufferNV = nullptr; 
PFNGLFRAMEBUFFERSAMPLELOCATIONSFVNVPROC fnptr_ci_glFramebufferSampleLocationsfvNV = nullptr; 
PFNGLNAMEDFRAMEBUFFERSAMPLELOCATIONSFVNVPROC fnptr_ci_glNamedFramebufferSampleLocationsfvNV = nullptr; 
PFNGLRESOLVEDEPTHVALUESNVPROC fnptr_ci_glResolveDepthValuesNV = nullptr; 
PFNGLVIEWPORTARRAYVNVPROC fnptr_ci_glViewportArrayvNV = nullptr; 
PFNGLVIEWPORTINDEXEDFNVPROC fnptr_ci_glViewportIndexedfNV = nullptr; 
PFNGLVIEWPORTINDEXEDFVNVPROC fnptr_ci_glViewportIndexedfvNV = nullptr; 
PFNGLSCISSORARRAYVNVPROC fnptr_ci_glScissorArrayvNV = nullptr; 
PFNGLSCISSORINDEXEDNVPROC fnptr_ci_glScissorIndexedNV = nullptr; 
PFNGLSCISSORINDEXEDVNVPROC fnptr_ci_glScissorIndexedvNV = nullptr; 
PFNGLDEPTHRANGEARRAYFVNVPROC fnptr_ci_glDepthRangeArrayfvNV = nullptr; 
PFNGLDEPTHRANGEINDEXEDFNVPROC fnptr_ci_glDepthRangeIndexedfNV = nullptr; 
PFNGLGETFLOATI_VNVPROC fnptr_ci_glGetFloati_vNV = nullptr; 
PFNGLENABLEINVPROC fnptr_ci_glEnableiNV = nullptr; 
PFNGLDISABLEINVPROC fnptr_ci_glDisableiNV = nullptr; 
PFNGLISENABLEDINVPROC fnptr_ci_glIsEnablediNV = nullptr; 
PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVRPROC fnptr_ci_glFramebufferTextureMultiviewOVR = nullptr; 
PFNGLALPHAFUNCQCOMPROC fnptr_ci_glAlphaFuncQCOM = nullptr; 
PFNGLGETDRIVERCONTROLSQCOMPROC fnptr_ci_glGetDriverControlsQCOM = nullptr; 
PFNGLGETDRIVERCONTROLSTRINGQCOMPROC fnptr_ci_glGetDriverControlStringQCOM = nullptr; 
PFNGLENABLEDRIVERCONTROLQCOMPROC fnptr_ci_glEnableDriverControlQCOM = nullptr; 
PFNGLDISABLEDRIVERCONTROLQCOMPROC fnptr_ci_glDisableDriverControlQCOM = nullptr; 
PFNGLEXTGETTEXTURESQCOMPROC fnptr_ci_glExtGetTexturesQCOM = nullptr; 
PFNGLEXTGETBUFFERSQCOMPROC fnptr_ci_glExtGetBuffersQCOM = nullptr; 
PFNGLEXTGETRENDERBUFFERSQCOMPROC fnptr_ci_glExtGetRenderbuffersQCOM = nullptr; 
PFNGLEXTGETFRAMEBUFFERSQCOMPROC fnptr_ci_glExtGetFramebuffersQCOM = nullptr; 
PFNGLEXTGETTEXLEVELPARAMETERIVQCOMPROC fnptr_ci_glExtGetTexLevelParameterivQCOM = nullptr; 
PFNGLEXTTEXOBJECTSTATEOVERRIDEIQCOMPROC fnptr_ci_glExtTexObjectStateOverrideiQCOM = nullptr; 
PFNGLEXTGETTEXSUBIMAGEQCOMPROC fnptr_ci_glExtGetTexSubImageQCOM = nullptr; 
PFNGLEXTGETBUFFERPOINTERVQCOMPROC fnptr_ci_glExtGetBufferPointervQCOM = nullptr; 
PFNGLEXTGETSHADERSQCOMPROC fnptr_ci_glExtGetShadersQCOM = nullptr; 
PFNGLEXTGETPROGRAMSQCOMPROC fnptr_ci_glExtGetProgramsQCOM = nullptr; 
PFNGLEXTISPROGRAMBINARYQCOMPROC fnptr_ci_glExtIsProgramBinaryQCOM = nullptr; 
PFNGLEXTGETPROGRAMBINARYSOURCEQCOMPROC fnptr_ci_glExtGetProgramBinarySourceQCOM = nullptr; 
PFNGLSTARTTILINGQCOMPROC fnptr_ci_glStartTilingQCOM = nullptr; 
PFNGLENDTILINGQCOMPROC fnptr_ci_glEndTilingQCOM = nullptr; 


#if ( CINDER_GL_ES_VERSION == CINDER_GL_ES_VERSION_2 )
	// GL_OES_vertex_array_object
	PFNGLBINDVERTEXARRAYOESPROC fnptr_ci_glBindVertexArrayOES = nullptr; 
	PFNGLDELETEVERTEXARRAYSOESPROC fnptr_ci_glDeleteVertexArraysOES = nullptr; 
	PFNGLGENVERTEXARRAYSOESPROC fnptr_ci_glGenVertexArraysOES = nullptr; 
	PFNGLISVERTEXARRAYOESPROC fnptr_ci_glIsVertexArrayOES = nullptr; 

	// GL_OES_mapbuffer
    PFNGLMAPBUFFEROESPROC fnptr_ci_glMapBufferOES = nullptr; 
	PFNGLUNMAPBUFFEROESPROC fnptr_ci_glUnmapBufferOES = nullptr; 

	// GL_EXT_map_buffer_range
	PFNGLMAPBUFFERRANGEEXTPROC fnptr_ci_glMapBufferRangeEXT = nullptr; 
	PFNGLFLUSHMAPPEDBUFFERRANGEEXTPROC fnptr_ci_glFlushMappedBufferRangeEXT = nullptr; 

	// GL_EXT_multisampled_render_to_texture and the like
	PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC fnptr_ci_glRenderbufferStorageMultisample = nullptr; 

	// GL_NV_framebuffer_blit, GL_ANGLE_framebuffer_blit
	PFNGLBLITFRAMEBUFFERANGLEPROC fnptr_ci_glBlitFramebuffer = nullptr; 

	// GL_ANGLE_instanced_arrays, GL_EXT_draw_instanced, GL_NV_draw_instanced
	PFNGLDRAWARRAYSINSTANCEDPROC fnptr_ci_glDrawArraysInstanced = nullptr; 
	PFNGLDRAWELEMENTSINSTANCEDPROC fnptr_ci_glDrawElementsInstanced = nullptr; 

	// GL_ANGLE_instanced_arrays, GL_EXT_instanced_arrays, GL_NV_instanced_arrays
	PFNGLVERTEXATTRIBDIVISORPROC fnptr_ci_glVertexAttribDivisor = nullptr; 
#endif


#if ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_2 )
	PFNGLBUFFERSTORAGEPROC fnptr_ci_glBufferStorage = nullptr; 
#endif	

void gl_es_2_0_ext_load() 
{
	//DEBUG( "gl_es_2_0_ext_load entered..." );

	clearLoadCount();
	fnptr_ci_glBlendBarrierKHR = (PFNGLBLENDBARRIERKHRPROC)loadEglProc("glBlendBarrierKHR"); 
	fnptr_ci_glDebugMessageControlKHR = (PFNGLDEBUGMESSAGECONTROLKHRPROC)loadEglProc("glDebugMessageControlKHR"); 
	fnptr_ci_glDebugMessageInsertKHR = (PFNGLDEBUGMESSAGEINSERTKHRPROC)loadEglProc("glDebugMessageInsertKHR"); 
	fnptr_ci_glDebugMessageCallbackKHR = (PFNGLDEBUGMESSAGECALLBACKKHRPROC)loadEglProc("glDebugMessageCallbackKHR"); 
	fnptr_ci_glGetDebugMessageLogKHR = (PFNGLGETDEBUGMESSAGELOGKHRPROC)loadEglProc("glGetDebugMessageLogKHR"); 
	fnptr_ci_glPushDebugGroupKHR = (PFNGLPUSHDEBUGGROUPKHRPROC)loadEglProc("glPushDebugGroupKHR"); 
	fnptr_ci_glPopDebugGroupKHR = (PFNGLPOPDEBUGGROUPKHRPROC)loadEglProc("glPopDebugGroupKHR"); 
	fnptr_ci_glObjectLabelKHR = (PFNGLOBJECTLABELKHRPROC)loadEglProc("glObjectLabelKHR"); 
	fnptr_ci_glGetObjectLabelKHR = (PFNGLGETOBJECTLABELKHRPROC)loadEglProc("glGetObjectLabelKHR"); 
	fnptr_ci_glObjectPtrLabelKHR = (PFNGLOBJECTPTRLABELKHRPROC)loadEglProc("glObjectPtrLabelKHR"); 
	fnptr_ci_glGetObjectPtrLabelKHR = (PFNGLGETOBJECTPTRLABELKHRPROC)loadEglProc("glGetObjectPtrLabelKHR"); 
	fnptr_ci_glGetPointervKHR = (PFNGLGETPOINTERVKHRPROC)loadEglProc("glGetPointervKHR"); 
	fnptr_ci_glGetGraphicsResetStatusKHR = (PFNGLGETGRAPHICSRESETSTATUSKHRPROC)loadEglProc("glGetGraphicsResetStatusKHR"); 
	fnptr_ci_glReadnPixelsKHR = (PFNGLREADNPIXELSKHRPROC)loadEglProc("glReadnPixelsKHR"); 
	fnptr_ci_glGetnUniformfvKHR = (PFNGLGETNUNIFORMFVKHRPROC)loadEglProc("glGetnUniformfvKHR"); 
	fnptr_ci_glGetnUniformivKHR = (PFNGLGETNUNIFORMIVKHRPROC)loadEglProc("glGetnUniformivKHR"); 
	fnptr_ci_glGetnUniformuivKHR = (PFNGLGETNUNIFORMUIVKHRPROC)loadEglProc("glGetnUniformuivKHR"); 
	fnptr_ci_glEGLImageTargetTexture2DOES = (PFNGLEGLIMAGETARGETTEXTURE2DOESPROC)loadEglProc("glEGLImageTargetTexture2DOES"); 
	fnptr_ci_glEGLImageTargetRenderbufferStorageOES = (PFNGLEGLIMAGETARGETRENDERBUFFERSTORAGEOESPROC)loadEglProc("glEGLImageTargetRenderbufferStorageOES"); 
	fnptr_ci_glCopyImageSubDataOES = (PFNGLCOPYIMAGESUBDATAOESPROC)loadEglProc("glCopyImageSubDataOES"); 
	fnptr_ci_glEnableiOES = (PFNGLENABLEIOESPROC)loadEglProc("glEnableiOES"); 
	fnptr_ci_glDisableiOES = (PFNGLDISABLEIOESPROC)loadEglProc("glDisableiOES"); 
	fnptr_ci_glBlendEquationiOES = (PFNGLBLENDEQUATIONIOESPROC)loadEglProc("glBlendEquationiOES"); 
	fnptr_ci_glBlendEquationSeparateiOES = (PFNGLBLENDEQUATIONSEPARATEIOESPROC)loadEglProc("glBlendEquationSeparateiOES"); 
	fnptr_ci_glBlendFunciOES = (PFNGLBLENDFUNCIOESPROC)loadEglProc("glBlendFunciOES"); 
	fnptr_ci_glBlendFuncSeparateiOES = (PFNGLBLENDFUNCSEPARATEIOESPROC)loadEglProc("glBlendFuncSeparateiOES"); 
	fnptr_ci_glColorMaskiOES = (PFNGLCOLORMASKIOESPROC)loadEglProc("glColorMaskiOES"); 
	fnptr_ci_glIsEnablediOES = (PFNGLISENABLEDIOESPROC)loadEglProc("glIsEnablediOES"); 
	fnptr_ci_glDrawElementsBaseVertexOES = (PFNGLDRAWELEMENTSBASEVERTEXOESPROC)loadEglProc("glDrawElementsBaseVertexOES"); 
	fnptr_ci_glDrawRangeElementsBaseVertexOES = (PFNGLDRAWRANGEELEMENTSBASEVERTEXOESPROC)loadEglProc("glDrawRangeElementsBaseVertexOES"); 
	fnptr_ci_glDrawElementsInstancedBaseVertexOES = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXOESPROC)loadEglProc("glDrawElementsInstancedBaseVertexOES"); 
	fnptr_ci_glMultiDrawElementsBaseVertexOES = (PFNGLMULTIDRAWELEMENTSBASEVERTEXOESPROC)loadEglProc("glMultiDrawElementsBaseVertexOES"); 
	fnptr_ci_glFramebufferTextureOES = (PFNGLFRAMEBUFFERTEXTUREOESPROC)loadEglProc("glFramebufferTextureOES"); 
	fnptr_ci_glGetProgramBinaryOES = (PFNGLGETPROGRAMBINARYOESPROC)loadEglProc("glGetProgramBinaryOES"); 
	fnptr_ci_glProgramBinaryOES = (PFNGLPROGRAMBINARYOESPROC)loadEglProc("glProgramBinaryOES"); 
	fnptr_ci_glGetBufferPointervOES = (PFNGLGETBUFFERPOINTERVOESPROC)loadEglProc("glGetBufferPointervOES"); 
	fnptr_ci_glPrimitiveBoundingBoxOES = (PFNGLPRIMITIVEBOUNDINGBOXOESPROC)loadEglProc("glPrimitiveBoundingBoxOES"); 
	fnptr_ci_glMinSampleShadingOES = (PFNGLMINSAMPLESHADINGOESPROC)loadEglProc("glMinSampleShadingOES"); 
	fnptr_ci_glTexImage3DOES = (PFNGLTEXIMAGE3DOESPROC)loadEglProc("glTexImage3DOES"); 
	fnptr_ci_glTexSubImage3DOES = (PFNGLTEXSUBIMAGE3DOESPROC)loadEglProc("glTexSubImage3DOES"); 
	fnptr_ci_glCopyTexSubImage3DOES = (PFNGLCOPYTEXSUBIMAGE3DOESPROC)loadEglProc("glCopyTexSubImage3DOES"); 
	fnptr_ci_glCompressedTexImage3DOES = (PFNGLCOMPRESSEDTEXIMAGE3DOESPROC)loadEglProc("glCompressedTexImage3DOES"); 
	fnptr_ci_glCompressedTexSubImage3DOES = (PFNGLCOMPRESSEDTEXSUBIMAGE3DOESPROC)loadEglProc("glCompressedTexSubImage3DOES"); 
	fnptr_ci_glFramebufferTexture3DOES = (PFNGLFRAMEBUFFERTEXTURE3DOESPROC)loadEglProc("glFramebufferTexture3DOES"); 
	fnptr_ci_glTexParameterIivOES = (PFNGLTEXPARAMETERIIVOESPROC)loadEglProc("glTexParameterIivOES"); 
	fnptr_ci_glTexParameterIuivOES = (PFNGLTEXPARAMETERIUIVOESPROC)loadEglProc("glTexParameterIuivOES"); 
	fnptr_ci_glGetTexParameterIivOES = (PFNGLGETTEXPARAMETERIIVOESPROC)loadEglProc("glGetTexParameterIivOES"); 
	fnptr_ci_glGetTexParameterIuivOES = (PFNGLGETTEXPARAMETERIUIVOESPROC)loadEglProc("glGetTexParameterIuivOES"); 
	fnptr_ci_glSamplerParameterIivOES = (PFNGLSAMPLERPARAMETERIIVOESPROC)loadEglProc("glSamplerParameterIivOES"); 
	fnptr_ci_glSamplerParameterIuivOES = (PFNGLSAMPLERPARAMETERIUIVOESPROC)loadEglProc("glSamplerParameterIuivOES"); 
	fnptr_ci_glGetSamplerParameterIivOES = (PFNGLGETSAMPLERPARAMETERIIVOESPROC)loadEglProc("glGetSamplerParameterIivOES"); 
	fnptr_ci_glGetSamplerParameterIuivOES = (PFNGLGETSAMPLERPARAMETERIUIVOESPROC)loadEglProc("glGetSamplerParameterIuivOES"); 
	fnptr_ci_glTexBufferOES = (PFNGLTEXBUFFEROESPROC)loadEglProc("glTexBufferOES"); 
	fnptr_ci_glTexBufferRangeOES = (PFNGLTEXBUFFERRANGEOESPROC)loadEglProc("glTexBufferRangeOES"); 
	fnptr_ci_glTexStorage3DMultisampleOES = (PFNGLTEXSTORAGE3DMULTISAMPLEOESPROC)loadEglProc("glTexStorage3DMultisampleOES"); 
	fnptr_ci_glTextureViewOES = (PFNGLTEXTUREVIEWOESPROC)loadEglProc("glTextureViewOES"); 
	fnptr_ci_glGetPerfMonitorGroupsAMD = (PFNGLGETPERFMONITORGROUPSAMDPROC)loadEglProc("glGetPerfMonitorGroupsAMD"); 
	fnptr_ci_glGetPerfMonitorCountersAMD = (PFNGLGETPERFMONITORCOUNTERSAMDPROC)loadEglProc("glGetPerfMonitorCountersAMD"); 
	fnptr_ci_glGetPerfMonitorGroupStringAMD = (PFNGLGETPERFMONITORGROUPSTRINGAMDPROC)loadEglProc("glGetPerfMonitorGroupStringAMD"); 
	fnptr_ci_glGetPerfMonitorCounterStringAMD = (PFNGLGETPERFMONITORCOUNTERSTRINGAMDPROC)loadEglProc("glGetPerfMonitorCounterStringAMD"); 
	fnptr_ci_glGetPerfMonitorCounterInfoAMD = (PFNGLGETPERFMONITORCOUNTERINFOAMDPROC)loadEglProc("glGetPerfMonitorCounterInfoAMD"); 
	fnptr_ci_glGenPerfMonitorsAMD = (PFNGLGENPERFMONITORSAMDPROC)loadEglProc("glGenPerfMonitorsAMD"); 
	fnptr_ci_glDeletePerfMonitorsAMD = (PFNGLDELETEPERFMONITORSAMDPROC)loadEglProc("glDeletePerfMonitorsAMD"); 
	fnptr_ci_glSelectPerfMonitorCountersAMD = (PFNGLSELECTPERFMONITORCOUNTERSAMDPROC)loadEglProc("glSelectPerfMonitorCountersAMD"); 
	fnptr_ci_glBeginPerfMonitorAMD = (PFNGLBEGINPERFMONITORAMDPROC)loadEglProc("glBeginPerfMonitorAMD"); 
	fnptr_ci_glEndPerfMonitorAMD = (PFNGLENDPERFMONITORAMDPROC)loadEglProc("glEndPerfMonitorAMD"); 
	fnptr_ci_glGetPerfMonitorCounterDataAMD = (PFNGLGETPERFMONITORCOUNTERDATAAMDPROC)loadEglProc("glGetPerfMonitorCounterDataAMD"); 
	fnptr_ci_glGetTranslatedShaderSourceANGLE = (PFNGLGETTRANSLATEDSHADERSOURCEANGLEPROC)loadEglProc("glGetTranslatedShaderSourceANGLE"); 
	fnptr_ci_glCopyTextureLevelsAPPLE = (PFNGLCOPYTEXTURELEVELSAPPLEPROC)loadEglProc("glCopyTextureLevelsAPPLE"); 
	fnptr_ci_glResolveMultisampleFramebufferAPPLE = (PFNGLRESOLVEMULTISAMPLEFRAMEBUFFERAPPLEPROC)loadEglProc("glResolveMultisampleFramebufferAPPLE"); 
	fnptr_ci_glFenceSyncAPPLE = (PFNGLFENCESYNCAPPLEPROC)loadEglProc("glFenceSyncAPPLE"); 
	fnptr_ci_glIsSyncAPPLE = (PFNGLISSYNCAPPLEPROC)loadEglProc("glIsSyncAPPLE"); 
	fnptr_ci_glDeleteSyncAPPLE = (PFNGLDELETESYNCAPPLEPROC)loadEglProc("glDeleteSyncAPPLE"); 
	fnptr_ci_glClientWaitSyncAPPLE = (PFNGLCLIENTWAITSYNCAPPLEPROC)loadEglProc("glClientWaitSyncAPPLE"); 
	fnptr_ci_glWaitSyncAPPLE = (PFNGLWAITSYNCAPPLEPROC)loadEglProc("glWaitSyncAPPLE"); 
	fnptr_ci_glGetInteger64vAPPLE = (PFNGLGETINTEGER64VAPPLEPROC)loadEglProc("glGetInteger64vAPPLE"); 
	fnptr_ci_glGetSyncivAPPLE = (PFNGLGETSYNCIVAPPLEPROC)loadEglProc("glGetSyncivAPPLE"); 
	fnptr_ci_glDrawArraysInstancedBaseInstanceEXT = (PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEEXTPROC)loadEglProc("glDrawArraysInstancedBaseInstanceEXT"); 
	fnptr_ci_glDrawElementsInstancedBaseInstanceEXT = (PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEEXTPROC)loadEglProc("glDrawElementsInstancedBaseInstanceEXT"); 
	fnptr_ci_glDrawElementsInstancedBaseVertexBaseInstanceEXT = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEEXTPROC)loadEglProc("glDrawElementsInstancedBaseVertexBaseInstanceEXT"); 
	fnptr_ci_glBindFragDataLocationIndexedEXT = (PFNGLBINDFRAGDATALOCATIONINDEXEDEXTPROC)loadEglProc("glBindFragDataLocationIndexedEXT"); 
	fnptr_ci_glBindFragDataLocationEXT = (PFNGLBINDFRAGDATALOCATIONEXTPROC)loadEglProc("glBindFragDataLocationEXT"); 
	fnptr_ci_glGetProgramResourceLocationIndexEXT = (PFNGLGETPROGRAMRESOURCELOCATIONINDEXEXTPROC)loadEglProc("glGetProgramResourceLocationIndexEXT"); 
	fnptr_ci_glGetFragDataIndexEXT = (PFNGLGETFRAGDATAINDEXEXTPROC)loadEglProc("glGetFragDataIndexEXT"); 
	fnptr_ci_glCopyImageSubDataEXT = (PFNGLCOPYIMAGESUBDATAEXTPROC)loadEglProc("glCopyImageSubDataEXT"); 
	fnptr_ci_glLabelObjectEXT = (PFNGLLABELOBJECTEXTPROC)loadEglProc("glLabelObjectEXT"); 
	fnptr_ci_glGetObjectLabelEXT = (PFNGLGETOBJECTLABELEXTPROC)loadEglProc("glGetObjectLabelEXT"); 
	fnptr_ci_glInsertEventMarkerEXT = (PFNGLINSERTEVENTMARKEREXTPROC)loadEglProc("glInsertEventMarkerEXT"); 
	fnptr_ci_glPushGroupMarkerEXT = (PFNGLPUSHGROUPMARKEREXTPROC)loadEglProc("glPushGroupMarkerEXT"); 
	fnptr_ci_glPopGroupMarkerEXT = (PFNGLPOPGROUPMARKEREXTPROC)loadEglProc("glPopGroupMarkerEXT"); 
	fnptr_ci_glDiscardFramebufferEXT = (PFNGLDISCARDFRAMEBUFFEREXTPROC)loadEglProc("glDiscardFramebufferEXT"); 
	fnptr_ci_glGenQueriesEXT = (PFNGLGENQUERIESEXTPROC)loadEglProc("glGenQueriesEXT"); 
	fnptr_ci_glDeleteQueriesEXT = (PFNGLDELETEQUERIESEXTPROC)loadEglProc("glDeleteQueriesEXT"); 
	fnptr_ci_glIsQueryEXT = (PFNGLISQUERYEXTPROC)loadEglProc("glIsQueryEXT"); 
	fnptr_ci_glBeginQueryEXT = (PFNGLBEGINQUERYEXTPROC)loadEglProc("glBeginQueryEXT"); 
	fnptr_ci_glEndQueryEXT = (PFNGLENDQUERYEXTPROC)loadEglProc("glEndQueryEXT"); 
	fnptr_ci_glQueryCounterEXT = (PFNGLQUERYCOUNTEREXTPROC)loadEglProc("glQueryCounterEXT"); 
	fnptr_ci_glGetQueryivEXT = (PFNGLGETQUERYIVEXTPROC)loadEglProc("glGetQueryivEXT"); 
	fnptr_ci_glGetQueryObjectivEXT = (PFNGLGETQUERYOBJECTIVEXTPROC)loadEglProc("glGetQueryObjectivEXT"); 
	fnptr_ci_glGetQueryObjectuivEXT = (PFNGLGETQUERYOBJECTUIVEXTPROC)loadEglProc("glGetQueryObjectuivEXT"); 
	fnptr_ci_glGetQueryObjecti64vEXT = (PFNGLGETQUERYOBJECTI64VEXTPROC)loadEglProc("glGetQueryObjecti64vEXT"); 
	fnptr_ci_glGetQueryObjectui64vEXT = (PFNGLGETQUERYOBJECTUI64VEXTPROC)loadEglProc("glGetQueryObjectui64vEXT"); 
	fnptr_ci_glDrawBuffersEXT = (PFNGLDRAWBUFFERSEXTPROC)loadEglProc("glDrawBuffersEXT"); 
	fnptr_ci_glEnableiEXT = (PFNGLENABLEIEXTPROC)loadEglProc("glEnableiEXT"); 
	fnptr_ci_glDisableiEXT = (PFNGLDISABLEIEXTPROC)loadEglProc("glDisableiEXT"); 
	fnptr_ci_glBlendEquationiEXT = (PFNGLBLENDEQUATIONIEXTPROC)loadEglProc("glBlendEquationiEXT"); 
	fnptr_ci_glBlendEquationSeparateiEXT = (PFNGLBLENDEQUATIONSEPARATEIEXTPROC)loadEglProc("glBlendEquationSeparateiEXT"); 
	fnptr_ci_glBlendFunciEXT = (PFNGLBLENDFUNCIEXTPROC)loadEglProc("glBlendFunciEXT"); 
	fnptr_ci_glBlendFuncSeparateiEXT = (PFNGLBLENDFUNCSEPARATEIEXTPROC)loadEglProc("glBlendFuncSeparateiEXT"); 
	fnptr_ci_glColorMaskiEXT = (PFNGLCOLORMASKIEXTPROC)loadEglProc("glColorMaskiEXT"); 
	fnptr_ci_glIsEnablediEXT = (PFNGLISENABLEDIEXTPROC)loadEglProc("glIsEnablediEXT"); 
	fnptr_ci_glDrawElementsBaseVertexEXT = (PFNGLDRAWELEMENTSBASEVERTEXEXTPROC)loadEglProc("glDrawElementsBaseVertexEXT"); 
	fnptr_ci_glDrawRangeElementsBaseVertexEXT = (PFNGLDRAWRANGEELEMENTSBASEVERTEXEXTPROC)loadEglProc("glDrawRangeElementsBaseVertexEXT"); 
	fnptr_ci_glDrawElementsInstancedBaseVertexEXT = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXEXTPROC)loadEglProc("glDrawElementsInstancedBaseVertexEXT"); 
	fnptr_ci_glMultiDrawElementsBaseVertexEXT = (PFNGLMULTIDRAWELEMENTSBASEVERTEXEXTPROC)loadEglProc("glMultiDrawElementsBaseVertexEXT"); 
	fnptr_ci_glMultiDrawArraysEXT = (PFNGLMULTIDRAWARRAYSEXTPROC)loadEglProc("glMultiDrawArraysEXT"); 
	fnptr_ci_glMultiDrawElementsEXT = (PFNGLMULTIDRAWELEMENTSEXTPROC)loadEglProc("glMultiDrawElementsEXT"); 
	fnptr_ci_glMultiDrawArraysIndirectEXT = (PFNGLMULTIDRAWARRAYSINDIRECTEXTPROC)loadEglProc("glMultiDrawArraysIndirectEXT"); 
	fnptr_ci_glMultiDrawElementsIndirectEXT = (PFNGLMULTIDRAWELEMENTSINDIRECTEXTPROC)loadEglProc("glMultiDrawElementsIndirectEXT"); 
	fnptr_ci_glFramebufferTexture2DMultisampleEXT = (PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC)loadEglProc("glFramebufferTexture2DMultisampleEXT"); 
	fnptr_ci_glReadBufferIndexedEXT = (PFNGLREADBUFFERINDEXEDEXTPROC)loadEglProc("glReadBufferIndexedEXT"); 
	fnptr_ci_glDrawBuffersIndexedEXT = (PFNGLDRAWBUFFERSINDEXEDEXTPROC)loadEglProc("glDrawBuffersIndexedEXT"); 
	fnptr_ci_glGetIntegeri_vEXT = (PFNGLGETINTEGERI_VEXTPROC)loadEglProc("glGetIntegeri_vEXT"); 
	fnptr_ci_glPrimitiveBoundingBoxEXT = (PFNGLPRIMITIVEBOUNDINGBOXEXTPROC)loadEglProc("glPrimitiveBoundingBoxEXT"); 
	fnptr_ci_glRasterSamplesEXT = (PFNGLRASTERSAMPLESEXTPROC)loadEglProc("glRasterSamplesEXT"); 
	fnptr_ci_glGetGraphicsResetStatusEXT = (PFNGLGETGRAPHICSRESETSTATUSEXTPROC)loadEglProc("glGetGraphicsResetStatusEXT"); 
	fnptr_ci_glReadnPixelsEXT = (PFNGLREADNPIXELSEXTPROC)loadEglProc("glReadnPixelsEXT"); 
	fnptr_ci_glGetnUniformfvEXT = (PFNGLGETNUNIFORMFVEXTPROC)loadEglProc("glGetnUniformfvEXT"); 
	fnptr_ci_glGetnUniformivEXT = (PFNGLGETNUNIFORMIVEXTPROC)loadEglProc("glGetnUniformivEXT"); 
	fnptr_ci_glActiveShaderProgramEXT = (PFNGLACTIVESHADERPROGRAMEXTPROC)loadEglProc("glActiveShaderProgramEXT"); 
	fnptr_ci_glBindProgramPipelineEXT = (PFNGLBINDPROGRAMPIPELINEEXTPROC)loadEglProc("glBindProgramPipelineEXT"); 
	fnptr_ci_glCreateShaderProgramvEXT = (PFNGLCREATESHADERPROGRAMVEXTPROC)loadEglProc("glCreateShaderProgramvEXT"); 
	fnptr_ci_glDeleteProgramPipelinesEXT = (PFNGLDELETEPROGRAMPIPELINESEXTPROC)loadEglProc("glDeleteProgramPipelinesEXT"); 
	fnptr_ci_glGenProgramPipelinesEXT = (PFNGLGENPROGRAMPIPELINESEXTPROC)loadEglProc("glGenProgramPipelinesEXT"); 
	fnptr_ci_glGetProgramPipelineInfoLogEXT = (PFNGLGETPROGRAMPIPELINEINFOLOGEXTPROC)loadEglProc("glGetProgramPipelineInfoLogEXT"); 
	fnptr_ci_glGetProgramPipelineivEXT = (PFNGLGETPROGRAMPIPELINEIVEXTPROC)loadEglProc("glGetProgramPipelineivEXT"); 
	fnptr_ci_glIsProgramPipelineEXT = (PFNGLISPROGRAMPIPELINEEXTPROC)loadEglProc("glIsProgramPipelineEXT"); 
	fnptr_ci_glProgramParameteriEXT = (PFNGLPROGRAMPARAMETERIEXTPROC)loadEglProc("glProgramParameteriEXT"); 
	fnptr_ci_glProgramUniform1fEXT = (PFNGLPROGRAMUNIFORM1FEXTPROC)loadEglProc("glProgramUniform1fEXT"); 
	fnptr_ci_glProgramUniform1fvEXT = (PFNGLPROGRAMUNIFORM1FVEXTPROC)loadEglProc("glProgramUniform1fvEXT"); 
	fnptr_ci_glProgramUniform1iEXT = (PFNGLPROGRAMUNIFORM1IEXTPROC)loadEglProc("glProgramUniform1iEXT"); 
	fnptr_ci_glProgramUniform1ivEXT = (PFNGLPROGRAMUNIFORM1IVEXTPROC)loadEglProc("glProgramUniform1ivEXT"); 
	fnptr_ci_glProgramUniform2fEXT = (PFNGLPROGRAMUNIFORM2FEXTPROC)loadEglProc("glProgramUniform2fEXT"); 
	fnptr_ci_glProgramUniform2fvEXT = (PFNGLPROGRAMUNIFORM2FVEXTPROC)loadEglProc("glProgramUniform2fvEXT"); 
	fnptr_ci_glProgramUniform2iEXT = (PFNGLPROGRAMUNIFORM2IEXTPROC)loadEglProc("glProgramUniform2iEXT"); 
	fnptr_ci_glProgramUniform2ivEXT = (PFNGLPROGRAMUNIFORM2IVEXTPROC)loadEglProc("glProgramUniform2ivEXT"); 
	fnptr_ci_glProgramUniform3fEXT = (PFNGLPROGRAMUNIFORM3FEXTPROC)loadEglProc("glProgramUniform3fEXT"); 
	fnptr_ci_glProgramUniform3fvEXT = (PFNGLPROGRAMUNIFORM3FVEXTPROC)loadEglProc("glProgramUniform3fvEXT"); 
	fnptr_ci_glProgramUniform3iEXT = (PFNGLPROGRAMUNIFORM3IEXTPROC)loadEglProc("glProgramUniform3iEXT"); 
	fnptr_ci_glProgramUniform3ivEXT = (PFNGLPROGRAMUNIFORM3IVEXTPROC)loadEglProc("glProgramUniform3ivEXT"); 
	fnptr_ci_glProgramUniform4fEXT = (PFNGLPROGRAMUNIFORM4FEXTPROC)loadEglProc("glProgramUniform4fEXT"); 
	fnptr_ci_glProgramUniform4fvEXT = (PFNGLPROGRAMUNIFORM4FVEXTPROC)loadEglProc("glProgramUniform4fvEXT"); 
	fnptr_ci_glProgramUniform4iEXT = (PFNGLPROGRAMUNIFORM4IEXTPROC)loadEglProc("glProgramUniform4iEXT"); 
	fnptr_ci_glProgramUniform4ivEXT = (PFNGLPROGRAMUNIFORM4IVEXTPROC)loadEglProc("glProgramUniform4ivEXT"); 
	fnptr_ci_glProgramUniformMatrix2fvEXT = (PFNGLPROGRAMUNIFORMMATRIX2FVEXTPROC)loadEglProc("glProgramUniformMatrix2fvEXT"); 
	fnptr_ci_glProgramUniformMatrix3fvEXT = (PFNGLPROGRAMUNIFORMMATRIX3FVEXTPROC)loadEglProc("glProgramUniformMatrix3fvEXT"); 
	fnptr_ci_glProgramUniformMatrix4fvEXT = (PFNGLPROGRAMUNIFORMMATRIX4FVEXTPROC)loadEglProc("glProgramUniformMatrix4fvEXT"); 
	fnptr_ci_glUseProgramStagesEXT = (PFNGLUSEPROGRAMSTAGESEXTPROC)loadEglProc("glUseProgramStagesEXT"); 
	fnptr_ci_glValidateProgramPipelineEXT = (PFNGLVALIDATEPROGRAMPIPELINEEXTPROC)loadEglProc("glValidateProgramPipelineEXT"); 
	fnptr_ci_glProgramUniform1uiEXT = (PFNGLPROGRAMUNIFORM1UIEXTPROC)loadEglProc("glProgramUniform1uiEXT"); 
	fnptr_ci_glProgramUniform2uiEXT = (PFNGLPROGRAMUNIFORM2UIEXTPROC)loadEglProc("glProgramUniform2uiEXT"); 
	fnptr_ci_glProgramUniform3uiEXT = (PFNGLPROGRAMUNIFORM3UIEXTPROC)loadEglProc("glProgramUniform3uiEXT"); 
	fnptr_ci_glProgramUniform4uiEXT = (PFNGLPROGRAMUNIFORM4UIEXTPROC)loadEglProc("glProgramUniform4uiEXT"); 
	fnptr_ci_glProgramUniform1uivEXT = (PFNGLPROGRAMUNIFORM1UIVEXTPROC)loadEglProc("glProgramUniform1uivEXT"); 
	fnptr_ci_glProgramUniform2uivEXT = (PFNGLPROGRAMUNIFORM2UIVEXTPROC)loadEglProc("glProgramUniform2uivEXT"); 
	fnptr_ci_glProgramUniform3uivEXT = (PFNGLPROGRAMUNIFORM3UIVEXTPROC)loadEglProc("glProgramUniform3uivEXT"); 
	fnptr_ci_glProgramUniform4uivEXT = (PFNGLPROGRAMUNIFORM4UIVEXTPROC)loadEglProc("glProgramUniform4uivEXT"); 
	fnptr_ci_glProgramUniformMatrix2x3fvEXT = (PFNGLPROGRAMUNIFORMMATRIX2X3FVEXTPROC)loadEglProc("glProgramUniformMatrix2x3fvEXT"); 
	fnptr_ci_glProgramUniformMatrix3x2fvEXT = (PFNGLPROGRAMUNIFORMMATRIX3X2FVEXTPROC)loadEglProc("glProgramUniformMatrix3x2fvEXT"); 
	fnptr_ci_glProgramUniformMatrix2x4fvEXT = (PFNGLPROGRAMUNIFORMMATRIX2X4FVEXTPROC)loadEglProc("glProgramUniformMatrix2x4fvEXT"); 
	fnptr_ci_glProgramUniformMatrix4x2fvEXT = (PFNGLPROGRAMUNIFORMMATRIX4X2FVEXTPROC)loadEglProc("glProgramUniformMatrix4x2fvEXT"); 
	fnptr_ci_glProgramUniformMatrix3x4fvEXT = (PFNGLPROGRAMUNIFORMMATRIX3X4FVEXTPROC)loadEglProc("glProgramUniformMatrix3x4fvEXT"); 
	fnptr_ci_glProgramUniformMatrix4x3fvEXT = (PFNGLPROGRAMUNIFORMMATRIX4X3FVEXTPROC)loadEglProc("glProgramUniformMatrix4x3fvEXT"); 
	fnptr_ci_glTexPageCommitmentEXT = (PFNGLTEXPAGECOMMITMENTEXTPROC)loadEglProc("glTexPageCommitmentEXT"); 
	fnptr_ci_glTexParameterIivEXT = (PFNGLTEXPARAMETERIIVEXTPROC)loadEglProc("glTexParameterIivEXT"); 
	fnptr_ci_glTexParameterIuivEXT = (PFNGLTEXPARAMETERIUIVEXTPROC)loadEglProc("glTexParameterIuivEXT"); 
	fnptr_ci_glGetTexParameterIivEXT = (PFNGLGETTEXPARAMETERIIVEXTPROC)loadEglProc("glGetTexParameterIivEXT"); 
	fnptr_ci_glGetTexParameterIuivEXT = (PFNGLGETTEXPARAMETERIUIVEXTPROC)loadEglProc("glGetTexParameterIuivEXT"); 
	fnptr_ci_glSamplerParameterIivEXT = (PFNGLSAMPLERPARAMETERIIVEXTPROC)loadEglProc("glSamplerParameterIivEXT"); 
	fnptr_ci_glSamplerParameterIuivEXT = (PFNGLSAMPLERPARAMETERIUIVEXTPROC)loadEglProc("glSamplerParameterIuivEXT"); 
	fnptr_ci_glGetSamplerParameterIivEXT = (PFNGLGETSAMPLERPARAMETERIIVEXTPROC)loadEglProc("glGetSamplerParameterIivEXT"); 
	fnptr_ci_glGetSamplerParameterIuivEXT = (PFNGLGETSAMPLERPARAMETERIUIVEXTPROC)loadEglProc("glGetSamplerParameterIuivEXT"); 
	fnptr_ci_glTexBufferEXT = (PFNGLTEXBUFFEREXTPROC)loadEglProc("glTexBufferEXT"); 
	fnptr_ci_glTexBufferRangeEXT = (PFNGLTEXBUFFERRANGEEXTPROC)loadEglProc("glTexBufferRangeEXT"); 
	fnptr_ci_glTexStorage1DEXT = (PFNGLTEXSTORAGE1DEXTPROC)loadEglProc("glTexStorage1DEXT"); 
	fnptr_ci_glTexStorage2DEXT = (PFNGLTEXSTORAGE2DEXTPROC)loadEglProc("glTexStorage2DEXT"); 
	fnptr_ci_glTexStorage3DEXT = (PFNGLTEXSTORAGE3DEXTPROC)loadEglProc("glTexStorage3DEXT"); 
	fnptr_ci_glTextureStorage1DEXT = (PFNGLTEXTURESTORAGE1DEXTPROC)loadEglProc("glTextureStorage1DEXT"); 
	fnptr_ci_glTextureStorage2DEXT = (PFNGLTEXTURESTORAGE2DEXTPROC)loadEglProc("glTextureStorage2DEXT"); 
	fnptr_ci_glTextureStorage3DEXT = (PFNGLTEXTURESTORAGE3DEXTPROC)loadEglProc("glTextureStorage3DEXT"); 
	fnptr_ci_glTextureViewEXT = (PFNGLTEXTUREVIEWEXTPROC)loadEglProc("glTextureViewEXT"); 
	fnptr_ci_glFramebufferTexture2DMultisampleIMG = (PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMGPROC)loadEglProc("glFramebufferTexture2DMultisampleIMG"); 
	fnptr_ci_glApplyFramebufferAttachmentCMAAINTEL = (PFNGLAPPLYFRAMEBUFFERATTACHMENTCMAAINTELPROC)loadEglProc("glApplyFramebufferAttachmentCMAAINTEL"); 
	fnptr_ci_glBeginPerfQueryINTEL = (PFNGLBEGINPERFQUERYINTELPROC)loadEglProc("glBeginPerfQueryINTEL"); 
	fnptr_ci_glCreatePerfQueryINTEL = (PFNGLCREATEPERFQUERYINTELPROC)loadEglProc("glCreatePerfQueryINTEL"); 
	fnptr_ci_glDeletePerfQueryINTEL = (PFNGLDELETEPERFQUERYINTELPROC)loadEglProc("glDeletePerfQueryINTEL"); 
	fnptr_ci_glEndPerfQueryINTEL = (PFNGLENDPERFQUERYINTELPROC)loadEglProc("glEndPerfQueryINTEL"); 
	fnptr_ci_glGetFirstPerfQueryIdINTEL = (PFNGLGETFIRSTPERFQUERYIDINTELPROC)loadEglProc("glGetFirstPerfQueryIdINTEL"); 
	fnptr_ci_glGetNextPerfQueryIdINTEL = (PFNGLGETNEXTPERFQUERYIDINTELPROC)loadEglProc("glGetNextPerfQueryIdINTEL"); 
	fnptr_ci_glGetPerfCounterInfoINTEL = (PFNGLGETPERFCOUNTERINFOINTELPROC)loadEglProc("glGetPerfCounterInfoINTEL"); 
	fnptr_ci_glGetPerfQueryDataINTEL = (PFNGLGETPERFQUERYDATAINTELPROC)loadEglProc("glGetPerfQueryDataINTEL"); 
	fnptr_ci_glGetPerfQueryIdByNameINTEL = (PFNGLGETPERFQUERYIDBYNAMEINTELPROC)loadEglProc("glGetPerfQueryIdByNameINTEL"); 
	fnptr_ci_glGetPerfQueryInfoINTEL = (PFNGLGETPERFQUERYINFOINTELPROC)loadEglProc("glGetPerfQueryInfoINTEL"); 
	fnptr_ci_glGetTextureHandleNV = (PFNGLGETTEXTUREHANDLENVPROC)loadEglProc("glGetTextureHandleNV"); 
	fnptr_ci_glGetTextureSamplerHandleNV = (PFNGLGETTEXTURESAMPLERHANDLENVPROC)loadEglProc("glGetTextureSamplerHandleNV"); 
	fnptr_ci_glMakeTextureHandleResidentNV = (PFNGLMAKETEXTUREHANDLERESIDENTNVPROC)loadEglProc("glMakeTextureHandleResidentNV"); 
	fnptr_ci_glMakeTextureHandleNonResidentNV = (PFNGLMAKETEXTUREHANDLENONRESIDENTNVPROC)loadEglProc("glMakeTextureHandleNonResidentNV"); 
	fnptr_ci_glGetImageHandleNV = (PFNGLGETIMAGEHANDLENVPROC)loadEglProc("glGetImageHandleNV"); 
	fnptr_ci_glMakeImageHandleResidentNV = (PFNGLMAKEIMAGEHANDLERESIDENTNVPROC)loadEglProc("glMakeImageHandleResidentNV"); 
	fnptr_ci_glMakeImageHandleNonResidentNV = (PFNGLMAKEIMAGEHANDLENONRESIDENTNVPROC)loadEglProc("glMakeImageHandleNonResidentNV"); 
	fnptr_ci_glUniformHandleui64NV = (PFNGLUNIFORMHANDLEUI64NVPROC)loadEglProc("glUniformHandleui64NV"); 
	fnptr_ci_glUniformHandleui64vNV = (PFNGLUNIFORMHANDLEUI64VNVPROC)loadEglProc("glUniformHandleui64vNV"); 
	fnptr_ci_glProgramUniformHandleui64NV = (PFNGLPROGRAMUNIFORMHANDLEUI64NVPROC)loadEglProc("glProgramUniformHandleui64NV"); 
	fnptr_ci_glProgramUniformHandleui64vNV = (PFNGLPROGRAMUNIFORMHANDLEUI64VNVPROC)loadEglProc("glProgramUniformHandleui64vNV"); 
	fnptr_ci_glIsTextureHandleResidentNV = (PFNGLISTEXTUREHANDLERESIDENTNVPROC)loadEglProc("glIsTextureHandleResidentNV"); 
	fnptr_ci_glIsImageHandleResidentNV = (PFNGLISIMAGEHANDLERESIDENTNVPROC)loadEglProc("glIsImageHandleResidentNV"); 
	fnptr_ci_glBlendParameteriNV = (PFNGLBLENDPARAMETERINVPROC)loadEglProc("glBlendParameteriNV"); 
	fnptr_ci_glBlendBarrierNV = (PFNGLBLENDBARRIERNVPROC)loadEglProc("glBlendBarrierNV"); 
	fnptr_ci_glBeginConditionalRenderNV = (PFNGLBEGINCONDITIONALRENDERNVPROC)loadEglProc("glBeginConditionalRenderNV"); 
	fnptr_ci_glEndConditionalRenderNV = (PFNGLENDCONDITIONALRENDERNVPROC)loadEglProc("glEndConditionalRenderNV"); 
	fnptr_ci_glSubpixelPrecisionBiasNV = (PFNGLSUBPIXELPRECISIONBIASNVPROC)loadEglProc("glSubpixelPrecisionBiasNV"); 
	fnptr_ci_glCopyBufferSubDataNV = (PFNGLCOPYBUFFERSUBDATANVPROC)loadEglProc("glCopyBufferSubDataNV"); 
	fnptr_ci_glCoverageMaskNV = (PFNGLCOVERAGEMASKNVPROC)loadEglProc("glCoverageMaskNV"); 
	fnptr_ci_glCoverageOperationNV = (PFNGLCOVERAGEOPERATIONNVPROC)loadEglProc("glCoverageOperationNV"); 
	fnptr_ci_glDrawBuffersNV = (PFNGLDRAWBUFFERSNVPROC)loadEglProc("glDrawBuffersNV"); 
	fnptr_ci_glDeleteFencesNV = (PFNGLDELETEFENCESNVPROC)loadEglProc("glDeleteFencesNV"); 
	fnptr_ci_glGenFencesNV = (PFNGLGENFENCESNVPROC)loadEglProc("glGenFencesNV"); 
	fnptr_ci_glIsFenceNV = (PFNGLISFENCENVPROC)loadEglProc("glIsFenceNV"); 
	fnptr_ci_glTestFenceNV = (PFNGLTESTFENCENVPROC)loadEglProc("glTestFenceNV"); 
	fnptr_ci_glGetFenceivNV = (PFNGLGETFENCEIVNVPROC)loadEglProc("glGetFenceivNV"); 
	fnptr_ci_glFinishFenceNV = (PFNGLFINISHFENCENVPROC)loadEglProc("glFinishFenceNV"); 
	fnptr_ci_glSetFenceNV = (PFNGLSETFENCENVPROC)loadEglProc("glSetFenceNV"); 
	fnptr_ci_glFragmentCoverageColorNV = (PFNGLFRAGMENTCOVERAGECOLORNVPROC)loadEglProc("glFragmentCoverageColorNV"); 
	fnptr_ci_glCoverageModulationTableNV = (PFNGLCOVERAGEMODULATIONTABLENVPROC)loadEglProc("glCoverageModulationTableNV"); 
	fnptr_ci_glGetCoverageModulationTableNV = (PFNGLGETCOVERAGEMODULATIONTABLENVPROC)loadEglProc("glGetCoverageModulationTableNV"); 
	fnptr_ci_glCoverageModulationNV = (PFNGLCOVERAGEMODULATIONNVPROC)loadEglProc("glCoverageModulationNV"); 
	fnptr_ci_glGetInternalformatSampleivNV = (PFNGLGETINTERNALFORMATSAMPLEIVNVPROC)loadEglProc("glGetInternalformatSampleivNV"); 
	fnptr_ci_glUniformMatrix2x3fvNV = (PFNGLUNIFORMMATRIX2X3FVNVPROC)loadEglProc("glUniformMatrix2x3fvNV"); 
	fnptr_ci_glUniformMatrix3x2fvNV = (PFNGLUNIFORMMATRIX3X2FVNVPROC)loadEglProc("glUniformMatrix3x2fvNV"); 
	fnptr_ci_glUniformMatrix2x4fvNV = (PFNGLUNIFORMMATRIX2X4FVNVPROC)loadEglProc("glUniformMatrix2x4fvNV"); 
	fnptr_ci_glUniformMatrix4x2fvNV = (PFNGLUNIFORMMATRIX4X2FVNVPROC)loadEglProc("glUniformMatrix4x2fvNV"); 
	fnptr_ci_glUniformMatrix3x4fvNV = (PFNGLUNIFORMMATRIX3X4FVNVPROC)loadEglProc("glUniformMatrix3x4fvNV"); 
	fnptr_ci_glUniformMatrix4x3fvNV = (PFNGLUNIFORMMATRIX4X3FVNVPROC)loadEglProc("glUniformMatrix4x3fvNV"); 
	fnptr_ci_glGenPathsNV = (PFNGLGENPATHSNVPROC)loadEglProc("glGenPathsNV"); 
	fnptr_ci_glDeletePathsNV = (PFNGLDELETEPATHSNVPROC)loadEglProc("glDeletePathsNV"); 
	fnptr_ci_glIsPathNV = (PFNGLISPATHNVPROC)loadEglProc("glIsPathNV"); 
	fnptr_ci_glPathCommandsNV = (PFNGLPATHCOMMANDSNVPROC)loadEglProc("glPathCommandsNV"); 
	fnptr_ci_glPathCoordsNV = (PFNGLPATHCOORDSNVPROC)loadEglProc("glPathCoordsNV"); 
	fnptr_ci_glPathSubCommandsNV = (PFNGLPATHSUBCOMMANDSNVPROC)loadEglProc("glPathSubCommandsNV"); 
	fnptr_ci_glPathSubCoordsNV = (PFNGLPATHSUBCOORDSNVPROC)loadEglProc("glPathSubCoordsNV"); 
	fnptr_ci_glPathStringNV = (PFNGLPATHSTRINGNVPROC)loadEglProc("glPathStringNV"); 
	fnptr_ci_glPathGlyphsNV = (PFNGLPATHGLYPHSNVPROC)loadEglProc("glPathGlyphsNV"); 
	fnptr_ci_glPathGlyphRangeNV = (PFNGLPATHGLYPHRANGENVPROC)loadEglProc("glPathGlyphRangeNV"); 
	fnptr_ci_glWeightPathsNV = (PFNGLWEIGHTPATHSNVPROC)loadEglProc("glWeightPathsNV"); 
	fnptr_ci_glCopyPathNV = (PFNGLCOPYPATHNVPROC)loadEglProc("glCopyPathNV"); 
	fnptr_ci_glInterpolatePathsNV = (PFNGLINTERPOLATEPATHSNVPROC)loadEglProc("glInterpolatePathsNV"); 
	fnptr_ci_glTransformPathNV = (PFNGLTRANSFORMPATHNVPROC)loadEglProc("glTransformPathNV"); 
	fnptr_ci_glPathParameterivNV = (PFNGLPATHPARAMETERIVNVPROC)loadEglProc("glPathParameterivNV"); 
	fnptr_ci_glPathParameteriNV = (PFNGLPATHPARAMETERINVPROC)loadEglProc("glPathParameteriNV"); 
	fnptr_ci_glPathParameterfvNV = (PFNGLPATHPARAMETERFVNVPROC)loadEglProc("glPathParameterfvNV"); 
	fnptr_ci_glPathParameterfNV = (PFNGLPATHPARAMETERFNVPROC)loadEglProc("glPathParameterfNV"); 
	fnptr_ci_glPathDashArrayNV = (PFNGLPATHDASHARRAYNVPROC)loadEglProc("glPathDashArrayNV"); 
	fnptr_ci_glPathStencilFuncNV = (PFNGLPATHSTENCILFUNCNVPROC)loadEglProc("glPathStencilFuncNV"); 
	fnptr_ci_glPathStencilDepthOffsetNV = (PFNGLPATHSTENCILDEPTHOFFSETNVPROC)loadEglProc("glPathStencilDepthOffsetNV"); 
	fnptr_ci_glStencilFillPathNV = (PFNGLSTENCILFILLPATHNVPROC)loadEglProc("glStencilFillPathNV"); 
	fnptr_ci_glStencilStrokePathNV = (PFNGLSTENCILSTROKEPATHNVPROC)loadEglProc("glStencilStrokePathNV"); 
	fnptr_ci_glStencilFillPathInstancedNV = (PFNGLSTENCILFILLPATHINSTANCEDNVPROC)loadEglProc("glStencilFillPathInstancedNV"); 
	fnptr_ci_glStencilStrokePathInstancedNV = (PFNGLSTENCILSTROKEPATHINSTANCEDNVPROC)loadEglProc("glStencilStrokePathInstancedNV"); 
	fnptr_ci_glPathCoverDepthFuncNV = (PFNGLPATHCOVERDEPTHFUNCNVPROC)loadEglProc("glPathCoverDepthFuncNV"); 
	fnptr_ci_glCoverFillPathNV = (PFNGLCOVERFILLPATHNVPROC)loadEglProc("glCoverFillPathNV"); 
	fnptr_ci_glCoverStrokePathNV = (PFNGLCOVERSTROKEPATHNVPROC)loadEglProc("glCoverStrokePathNV"); 
	fnptr_ci_glCoverFillPathInstancedNV = (PFNGLCOVERFILLPATHINSTANCEDNVPROC)loadEglProc("glCoverFillPathInstancedNV"); 
	fnptr_ci_glCoverStrokePathInstancedNV = (PFNGLCOVERSTROKEPATHINSTANCEDNVPROC)loadEglProc("glCoverStrokePathInstancedNV"); 
	fnptr_ci_glGetPathParameterivNV = (PFNGLGETPATHPARAMETERIVNVPROC)loadEglProc("glGetPathParameterivNV"); 
	fnptr_ci_glGetPathParameterfvNV = (PFNGLGETPATHPARAMETERFVNVPROC)loadEglProc("glGetPathParameterfvNV"); 
	fnptr_ci_glGetPathCommandsNV = (PFNGLGETPATHCOMMANDSNVPROC)loadEglProc("glGetPathCommandsNV"); 
	fnptr_ci_glGetPathCoordsNV = (PFNGLGETPATHCOORDSNVPROC)loadEglProc("glGetPathCoordsNV"); 
	fnptr_ci_glGetPathDashArrayNV = (PFNGLGETPATHDASHARRAYNVPROC)loadEglProc("glGetPathDashArrayNV"); 
	fnptr_ci_glGetPathMetricsNV = (PFNGLGETPATHMETRICSNVPROC)loadEglProc("glGetPathMetricsNV"); 
	fnptr_ci_glGetPathMetricRangeNV = (PFNGLGETPATHMETRICRANGENVPROC)loadEglProc("glGetPathMetricRangeNV"); 
	fnptr_ci_glGetPathSpacingNV = (PFNGLGETPATHSPACINGNVPROC)loadEglProc("glGetPathSpacingNV"); 
	fnptr_ci_glIsPointInFillPathNV = (PFNGLISPOINTINFILLPATHNVPROC)loadEglProc("glIsPointInFillPathNV"); 
	fnptr_ci_glIsPointInStrokePathNV = (PFNGLISPOINTINSTROKEPATHNVPROC)loadEglProc("glIsPointInStrokePathNV"); 
	fnptr_ci_glGetPathLengthNV = (PFNGLGETPATHLENGTHNVPROC)loadEglProc("glGetPathLengthNV"); 
	fnptr_ci_glPointAlongPathNV = (PFNGLPOINTALONGPATHNVPROC)loadEglProc("glPointAlongPathNV"); 
	fnptr_ci_glMatrixLoad3x2fNV = (PFNGLMATRIXLOAD3X2FNVPROC)loadEglProc("glMatrixLoad3x2fNV"); 
	fnptr_ci_glMatrixLoad3x3fNV = (PFNGLMATRIXLOAD3X3FNVPROC)loadEglProc("glMatrixLoad3x3fNV"); 
	fnptr_ci_glMatrixLoadTranspose3x3fNV = (PFNGLMATRIXLOADTRANSPOSE3X3FNVPROC)loadEglProc("glMatrixLoadTranspose3x3fNV"); 
	fnptr_ci_glMatrixMult3x2fNV = (PFNGLMATRIXMULT3X2FNVPROC)loadEglProc("glMatrixMult3x2fNV"); 
	fnptr_ci_glMatrixMult3x3fNV = (PFNGLMATRIXMULT3X3FNVPROC)loadEglProc("glMatrixMult3x3fNV"); 
	fnptr_ci_glMatrixMultTranspose3x3fNV = (PFNGLMATRIXMULTTRANSPOSE3X3FNVPROC)loadEglProc("glMatrixMultTranspose3x3fNV"); 
	fnptr_ci_glStencilThenCoverFillPathNV = (PFNGLSTENCILTHENCOVERFILLPATHNVPROC)loadEglProc("glStencilThenCoverFillPathNV"); 
	fnptr_ci_glStencilThenCoverStrokePathNV = (PFNGLSTENCILTHENCOVERSTROKEPATHNVPROC)loadEglProc("glStencilThenCoverStrokePathNV"); 
	fnptr_ci_glStencilThenCoverFillPathInstancedNV = (PFNGLSTENCILTHENCOVERFILLPATHINSTANCEDNVPROC)loadEglProc("glStencilThenCoverFillPathInstancedNV"); 
	fnptr_ci_glStencilThenCoverStrokePathInstancedNV = (PFNGLSTENCILTHENCOVERSTROKEPATHINSTANCEDNVPROC)loadEglProc("glStencilThenCoverStrokePathInstancedNV"); 
	fnptr_ci_glPathGlyphIndexRangeNV = (PFNGLPATHGLYPHINDEXRANGENVPROC)loadEglProc("glPathGlyphIndexRangeNV"); 
	fnptr_ci_glPathGlyphIndexArrayNV = (PFNGLPATHGLYPHINDEXARRAYNVPROC)loadEglProc("glPathGlyphIndexArrayNV"); 
	fnptr_ci_glPathMemoryGlyphIndexArrayNV = (PFNGLPATHMEMORYGLYPHINDEXARRAYNVPROC)loadEglProc("glPathMemoryGlyphIndexArrayNV"); 
	fnptr_ci_glProgramPathFragmentInputGenNV = (PFNGLPROGRAMPATHFRAGMENTINPUTGENNVPROC)loadEglProc("glProgramPathFragmentInputGenNV"); 
	fnptr_ci_glGetProgramResourcefvNV = (PFNGLGETPROGRAMRESOURCEFVNVPROC)loadEglProc("glGetProgramResourcefvNV"); 
	fnptr_ci_glPolygonModeNV = (PFNGLPOLYGONMODENVPROC)loadEglProc("glPolygonModeNV"); 

	if( hasExtension( extstr_GL_NV_read_buffer ) ) {
		fnptr_ci_glReadBufferNV = (PFNGLREADBUFFERNVPROC)loadEglProc("glReadBufferNV"); 
	}

	fnptr_ci_glFramebufferSampleLocationsfvNV = (PFNGLFRAMEBUFFERSAMPLELOCATIONSFVNVPROC)loadEglProc("glFramebufferSampleLocationsfvNV"); 
	fnptr_ci_glNamedFramebufferSampleLocationsfvNV = (PFNGLNAMEDFRAMEBUFFERSAMPLELOCATIONSFVNVPROC)loadEglProc("glNamedFramebufferSampleLocationsfvNV"); 
	fnptr_ci_glResolveDepthValuesNV = (PFNGLRESOLVEDEPTHVALUESNVPROC)loadEglProc("glResolveDepthValuesNV"); 
	fnptr_ci_glViewportArrayvNV = (PFNGLVIEWPORTARRAYVNVPROC)loadEglProc("glViewportArrayvNV"); 
	fnptr_ci_glViewportIndexedfNV = (PFNGLVIEWPORTINDEXEDFNVPROC)loadEglProc("glViewportIndexedfNV"); 
	fnptr_ci_glViewportIndexedfvNV = (PFNGLVIEWPORTINDEXEDFVNVPROC)loadEglProc("glViewportIndexedfvNV"); 
	fnptr_ci_glScissorArrayvNV = (PFNGLSCISSORARRAYVNVPROC)loadEglProc("glScissorArrayvNV"); 
	fnptr_ci_glScissorIndexedNV = (PFNGLSCISSORINDEXEDNVPROC)loadEglProc("glScissorIndexedNV"); 
	fnptr_ci_glScissorIndexedvNV = (PFNGLSCISSORINDEXEDVNVPROC)loadEglProc("glScissorIndexedvNV"); 
	fnptr_ci_glDepthRangeArrayfvNV = (PFNGLDEPTHRANGEARRAYFVNVPROC)loadEglProc("glDepthRangeArrayfvNV"); 
	fnptr_ci_glDepthRangeIndexedfNV = (PFNGLDEPTHRANGEINDEXEDFNVPROC)loadEglProc("glDepthRangeIndexedfNV"); 
	fnptr_ci_glGetFloati_vNV = (PFNGLGETFLOATI_VNVPROC)loadEglProc("glGetFloati_vNV"); 
	fnptr_ci_glEnableiNV = (PFNGLENABLEINVPROC)loadEglProc("glEnableiNV"); 
	fnptr_ci_glDisableiNV = (PFNGLDISABLEINVPROC)loadEglProc("glDisableiNV"); 
	fnptr_ci_glIsEnablediNV = (PFNGLISENABLEDINVPROC)loadEglProc("glIsEnablediNV"); 
	fnptr_ci_glFramebufferTextureMultiviewOVR = (PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVRPROC)loadEglProc("glFramebufferTextureMultiviewOVR"); 
	fnptr_ci_glAlphaFuncQCOM = (PFNGLALPHAFUNCQCOMPROC)loadEglProc("glAlphaFuncQCOM"); 
	fnptr_ci_glGetDriverControlsQCOM = (PFNGLGETDRIVERCONTROLSQCOMPROC)loadEglProc("glGetDriverControlsQCOM"); 
	fnptr_ci_glGetDriverControlStringQCOM = (PFNGLGETDRIVERCONTROLSTRINGQCOMPROC)loadEglProc("glGetDriverControlStringQCOM"); 
	fnptr_ci_glEnableDriverControlQCOM = (PFNGLENABLEDRIVERCONTROLQCOMPROC)loadEglProc("glEnableDriverControlQCOM"); 
	fnptr_ci_glDisableDriverControlQCOM = (PFNGLDISABLEDRIVERCONTROLQCOMPROC)loadEglProc("glDisableDriverControlQCOM"); 
	fnptr_ci_glExtGetTexturesQCOM = (PFNGLEXTGETTEXTURESQCOMPROC)loadEglProc("glExtGetTexturesQCOM"); 
	fnptr_ci_glExtGetBuffersQCOM = (PFNGLEXTGETBUFFERSQCOMPROC)loadEglProc("glExtGetBuffersQCOM"); 
	fnptr_ci_glExtGetRenderbuffersQCOM = (PFNGLEXTGETRENDERBUFFERSQCOMPROC)loadEglProc("glExtGetRenderbuffersQCOM"); 
	fnptr_ci_glExtGetFramebuffersQCOM = (PFNGLEXTGETFRAMEBUFFERSQCOMPROC)loadEglProc("glExtGetFramebuffersQCOM"); 
	fnptr_ci_glExtGetTexLevelParameterivQCOM = (PFNGLEXTGETTEXLEVELPARAMETERIVQCOMPROC)loadEglProc("glExtGetTexLevelParameterivQCOM"); 
	fnptr_ci_glExtTexObjectStateOverrideiQCOM = (PFNGLEXTTEXOBJECTSTATEOVERRIDEIQCOMPROC)loadEglProc("glExtTexObjectStateOverrideiQCOM"); 
	fnptr_ci_glExtGetTexSubImageQCOM = (PFNGLEXTGETTEXSUBIMAGEQCOMPROC)loadEglProc("glExtGetTexSubImageQCOM"); 
	fnptr_ci_glExtGetBufferPointervQCOM = (PFNGLEXTGETBUFFERPOINTERVQCOMPROC)loadEglProc("glExtGetBufferPointervQCOM"); 
	fnptr_ci_glExtGetShadersQCOM = (PFNGLEXTGETSHADERSQCOMPROC)loadEglProc("glExtGetShadersQCOM"); 
	fnptr_ci_glExtGetProgramsQCOM = (PFNGLEXTGETPROGRAMSQCOMPROC)loadEglProc("glExtGetProgramsQCOM"); 
	fnptr_ci_glExtIsProgramBinaryQCOM = (PFNGLEXTISPROGRAMBINARYQCOMPROC)loadEglProc("glExtIsProgramBinaryQCOM"); 
	fnptr_ci_glExtGetProgramBinarySourceQCOM = (PFNGLEXTGETPROGRAMBINARYSOURCEQCOMPROC)loadEglProc("glExtGetProgramBinarySourceQCOM"); 
	fnptr_ci_glStartTilingQCOM = (PFNGLSTARTTILINGQCOMPROC)loadEglProc("glStartTilingQCOM"); 
	fnptr_ci_glEndTilingQCOM = (PFNGLENDTILINGQCOMPROC)loadEglProc("glEndTilingQCOM"); 	


#if ( CINDER_GL_ES_VERSION == CINDER_GL_ES_VERSION_2 )
	// GL_OES_vertex_array_object
	if( hasExtension( extstr_GL_OES_vertex_array_object ) || hasExtension( "GL_ARB_vertex_array_object" ) ) {
		fnptr_ci_glBindVertexArrayOES = (PFNGLBINDVERTEXARRAYOESPROC)loadEglProc("glBindVertexArrayOES"); 
		fnptr_ci_glDeleteVertexArraysOES = (PFNGLDELETEVERTEXARRAYSOESPROC)loadEglProc("glDeleteVertexArraysOES"); 
		fnptr_ci_glGenVertexArraysOES = (PFNGLGENVERTEXARRAYSOESPROC)loadEglProc("glGenVertexArraysOES"); 
		fnptr_ci_glIsVertexArrayOES = (PFNGLISVERTEXARRAYOESPROC)loadEglProc("glIsVertexArrayOES"); 
	}

	// GL_OES_mapbuffer
	if( hasExtension( "GL_OES_mapbuffer" ) ) {
		fnptr_ci_glMapBufferOES = (PFNGLMAPBUFFEROESPROC)loadEglProc("glMapBufferOES"); 	
		fnptr_ci_glUnmapBufferOES = (PFNGLUNMAPBUFFEROESPROC)loadEglProc("glUnmapBufferOES");
	}

	// GL_EXT_map_buffer_range
	if( hasExtension( "GL_EXT_map_buffer_range" ) ) {
		fnptr_ci_glMapBufferRangeEXT = (PFNGLMAPBUFFERRANGEEXTPROC)loadEglProc("glMapBufferRangeEXT"); 
		fnptr_ci_glFlushMappedBufferRangeEXT = (PFNGLFLUSHMAPPEDBUFFERRANGEEXTPROC)loadEglProc("glFlushMappedBufferRangeEXT"); 
	}

	// GL_EXT_multisampled_render_to_texture and the like
	if( hasExtension( "GL_ANGLE_framebuffer_multisample" ) ) {
		fnptr_ci_glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)loadEglProc("glRenderbufferStorageMultisampleANGLE");
	}
	else if( hasExtension( "GL_APPLE_framebuffer_multisample" ) ) {
		fnptr_ci_glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)loadEglProc("glRenderbufferStorageMultisampleAPPLE");
	}
	else if( hasExtension( "GL_EXT_multisampled_render_to_texture" ) ) {
		fnptr_ci_glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)loadEglProc("glRenderbufferStorageMultisampleEXT");
	}
	else if( hasExtension( "GL_IMG_multisampled_render_to_texture" ) ) {
		fnptr_ci_glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)loadEglProc("glRenderbufferStorageMultisampleIMG");
	}
	else if( hasExtension( "GL_NV_framebuffer_multisample" ) ) {
		fnptr_ci_glRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC)loadEglProc("glRenderbufferStorageMultisampleNV");
	}

	// // GL_NV_framebuffer_blit, GL_ANGLE_framebuffer_blit
	if( hasExtension( "GL_NV_framebuffer_blit" ) ) {
		fnptr_ci_glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)loadEglProc("glBlitFramebufferNV"); 
	}
	else if( hasExtension( "GL_ANGLE_framebuffer_blit" ) ) {
		fnptr_ci_glBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)loadEglProc("glBlitFramebufferANGLE"); 
	}

	// GL_ANGLE_instanced_arrays, GL_EXT_draw_instanced, GL_NV_draw_instanced
	if( hasExtension( "GL_ANGLE_instanced_arrays" ) ) {
		fnptr_ci_glDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDPROC)loadEglProc("glDrawArraysInstancedANGLE"); 
		fnptr_ci_glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDPROC)loadEglProc("glDrawElementsInstancedANGLE"); 
	}
	else if( hasExtension( "GL_EXT_draw_instanced" ) ) {
		fnptr_ci_glDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDPROC)loadEglProc("glDrawArraysInstancedEXT"); 
		fnptr_ci_glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDPROC)loadEglProc("glDrawElementsInstancedEXT");
	}
	else if( hasExtension( "GL_NV_draw_instanced" ) ) {
		fnptr_ci_glDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCEDPROC)loadEglProc("glDrawArraysInstancedNV"); 
		fnptr_ci_glDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCEDPROC)loadEglProc("glDrawElementsInstancedNV"); 
	}

	// GL_ANGLE_instanced_arrays, GL_EXT_instanced_arrays, GL_NV_instanced_arrays
	if( hasExtension( "GL_ANGLE_instanced_arrays" ) ) {
		fnptr_ci_glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC)loadEglProc("glVertexAttribDivisorANGLE"); 
	}
	else if( hasExtension( "GL_EXT_instanced_arrays" ) ) {
		fnptr_ci_glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC)loadEglProc("glVertexAttribDivisorEXT"); 
	}
	else if( hasExtension( "GL_NV_instanced_arrays" ) ) {
		fnptr_ci_glVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISORPROC)loadEglProc("glVertexAttribDivisorNV"); 
	}
#endif // ( CINDER_GL_ES_VERSION == CINDER_GL_ES_VERSION_2 )

#if ( CINDER_GL_ES_VERSION == CINDER_GL_ES_VERSION_2 )
	fnptr_ci_glBufferStorage = (PFNGLBUFFERSTORAGEPROC)loadEglProc("glBufferStorageEXT"); 
#endif // ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_2 )

	DEBUG( "gl_es_2_0_ext_load: SUCCESSFUL! | " << getLoadCount() << " procs loaded");	
}
#endif // ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_2 )


// ----------------------------------------------------------------------------
// Android Extension Pack
// ----------------------------------------------------------------------------
#if ( CINDER_GL_ES_VERSION == CINDER_GL_ES_VERSION_3_1 )
PFNGLFRAMEBUFFERTEXTUREPROC fnptr_ci_glFramebufferTexture = nullptr;
PFNGLPATCHPARAMETERIPROC fnptr_ci_glPatchParameteri = nullptr;

void gl_es_aep_load() 
{
	//DEBUG( "gl_es_aep_load entered..." );

	if( hasExtension( "GL_EXT_geometry_shader" ) ) {
		fnptr_ci_glFramebufferTexture = (PFNGLFRAMEBUFFERTEXTUREPROC)loadEglProc("glFramebufferTextureEXT");
	}

	// GL_EXT_tessellation_shader, GL_OES_tessellation_shader
	if( hasExtension( "GL_EXT_tessellation_shader" ) ) {
		fnptr_ci_glPatchParameteri = (PFNGLPATCHPARAMETERIOESPROC)loadEglProc("glPatchParameteriEXT");
	}
	else if( hasExtension( "GL_OES_tessellation_shader" ) ) {
		fnptr_ci_glPatchParameteri = (PFNGLPATCHPARAMETERIOESPROC)loadEglProc("glPatchParameteriOES");
	}	

	DEBUG( "gl_es_2_0_ext_load: SUCCESSFUL! | " << getLoadCount() << " procs loaded");	
}
#endif

#endif // defined( CINDER_ANDROID ) || defined( CINDER_LINUX )	

// ----------------------------------------------------------------------------
// gl_es_load
// ----------------------------------------------------------------------------
#if defined( CINDER_ANDROID ) || defined( CINDER_LINUX )	    
static bool 	sInitialized = false;
static uint32_t	sLoadCount = 0;

void gl_es_load()
{
	if( sInitialized ) {
		return;
	}

	DEBUG( "gl_es_load entered..." );

#if ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_2 )
	gl_es_2_0_ext_load();
#endif

#if defined( CINDER_LINUX ) && ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3 )
	gl_es_3_0_load();
#endif

#if defined( CINDER_LINUX ) && ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3_1 )
	gl_es_3_1_load();
#endif

#if defined( CINDER_LINUX ) && ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3_2 )
	gl_es_3_2_load();
#endif

#if ( CINDER_GL_ES_VERSION == CINDER_GL_ES_VERSION_3_1 )
    gl_es_aep_load();
#endif

	sInitialized = true;

	DEBUG( "gl_es_load: SUCCESSFUL" );
}

static bool hasExtension( const std::string& extName )
{
	static std::string sExtensions;
	if( sExtensions.empty() ) {
		const char *buf = reinterpret_cast<const char*>( glGetString( GL_EXTENSIONS ) );
		if( nullptr != buf ) {
			sExtensions = std::string( reinterpret_cast<const char*>( buf ) );
			std::transform( sExtensions.begin(), sExtensions.end(), sExtensions.begin(), ::tolower );

			std::string extsStr = std::string( buf );
			size_t startPos = 0;
			size_t endPos = extsStr.find( ' ' );
			bool done = ( std::string::npos == endPos );
			while( ! done ) {
				size_t len = endPos - startPos;
				std::string s = extsStr.substr( startPos, len );
				DEBUG( "Extension: " << s );

				startPos = endPos + 1;
				endPos = extsStr.find( ' ', startPos );
				if( std::string::npos == endPos ) {
					endPos = extsStr.length();
				}

				done = ( startPos >= endPos );
			}			
		}
	}

	std::string extension = extName;
	std::transform( extension.begin(), extension.end(), extension.begin(), ::tolower );
	return ( std::string::npos != sExtensions.find( extension ) );
}

static void* loadEglProc( const std::string& procName ) 
{
	uint8_t* result = (uint8_t *)eglGetProcAddress( procName.c_str() );
	if( nullptr != result ) {
		++sLoadCount;
		//DEBUG( "   " << "loaded EGL proc: " << procName );
	}
	return reinterpret_cast<void*>( result );
}

static void clearLoadCount()
{
	sLoadCount = 0;
}

static uint32_t	getLoadCount()
{
	return sLoadCount;
}

#endif // defined( CINDER_ANDROID ) || defined( CINDER_LINUX )	
