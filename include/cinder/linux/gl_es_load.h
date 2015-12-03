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

// -----------------------------------------------------------------------------
// iOS
// -----------------------------------------------------------------------------
#if defined( CINDER_COCOA_TOUCH )
	#if defined( CINDER_GL_ES_2 )
		// GL_OES_texture_half_float
		#define GL_HALF_FLOAT								GL_HALF_FLOAT_OES

		// GL_EXT_texture_rg
		#define GL_RED										GL_RED_EXT
		#define GL_RG										GL_RG_EXT
		#define GL_R8										GL_R8_EXT
		#define GL_RG8										GL_RG8_EXT

		// GL_OES_rgb8_rgba8
		#define GL_RGB8										GL_RGB8_OES
		#define GL_RGBA8									GL_RGBA8_OES

		// GL_EXT_color_buffer_half_float
		#define GL_RGBA16F									GL_RGBA16F_EXT
		#define GL_RGB16F									GL_RGB16F_EXT
		#define GL_RG16F									GL_RG16F_EXT
		#define GL_R16F										GL_R16F_EXT
		#define GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE	GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE_EXT
		#define GL_UNSIGNED_NORMALIZED						GL_UNSIGNED_NORMALIZED_EXT

		// GL_EXT_texture_storage
		#define GL_RGBA32F									GL_RGBA32F_EXT
		#define GL_RGB32F									GL_RGB32F_EXT
		#define GL_RG32F									GL_RG32F_EXT
		#define GL_R32F										GL_R32F_EXT

		// GL_APPLE_texture_packed_float
		#define GL_UNSIGNED_INT_10F_11F_11F_REV 			GL_UNSIGNED_INT_10F_11F_11F_REV_APPLE
		#define GL_UNSIGNED_INT_5_9_9_9_REV 				GL_UNSIGNED_INT_5_9_9_9_REV_APPLE
		#define GL_R11F_G11F_B10F           				GL_R11F_G11F_B10F_APPLE
		#define GL_RGB9_E5                  				GL_RGB9_E5_APPLE

		// GL_EXT_sRGB
		#define GL_SRGB										GL_SRGB_EXT
		#define GL_SRGB_ALPHA								GL_SRGB_ALPHA_EXT
		#define GL_SRGB8_ALPHA8								GL_SRGB8_ALPHA8_EXT
		#define GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING	GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING_EXT

		// GL_OES_depth24
		#define GL_DEPTH_COMPONENT24						GL_DEPTH_COMPONENT24_OES

		#define GL_WRITE_ONLY								GL_WRITE_ONLY_OES
		#define GL_BUFFER_ACCESS							GL_BUFFER_ACCESS_OES
		#define GL_BUFFER_MAPPED							GL_BUFFER_MAPPED_OES
		#define GL_BUFFER_MAP_POINTER						GL_BUFFER_MAP_POINTER_OES

		#define GL_MAP_READ_BIT								GL_MAP_READ_BIT_EXT
		#define GL_MAP_WRITE_BIT							GL_MAP_WRITE_BIT_EXT
		#define GL_MAP_INVALIDATE_RANGE_BIT					GL_MAP_INVALIDATE_RANGE_BIT_EXT
		#define GL_MAP_INVALIDATE_BUFFER_BIT				GL_MAP_INVALIDATE_BUFFER_BIT_EXT
		#define GL_MAP_FLUSH_EXPLICIT_BIT					GL_MAP_FLUSH_EXPLICIT_BIT_EXT
		#define GL_MAP_UNSYNCHRONIZED_BIT					GL_MAP_UNSYNCHRONIZED_BIT_EXT

		#define GL_READ_FRAMEBUFFER							GL_READ_FRAMEBUFFER_APPLE
		#define GL_DRAW_FRAMEBUFFER							GL_DRAW_FRAMEBUFFER_APPLE
		#define GL_READ_FRAMEBUFFER_BINDING					GL_READ_FRAMEBUFFER_BINDING_APPLE
		#define GL_DRAW_FRAMEBUFFER_BINDING					GL_DRAW_FRAMEBUFFER_BINDING_APPLE

		#define glBindVertexArray							glBindVertexArrayOES
		#define glDeleteVertexArrays						glDeleteVertexArraysOES
		#define glGenVertexArrays							glGenVertexArraysOES
		#define glIsVertexArray								glIsVertexArrayOES

		#define glMapBuffer									glMapBufferOES
		#define glUnmapBuffer								glUnmapBufferOES

		#define glMapBufferRange							glMapBufferRangeEXT

		#define glRenderbufferStorageMultisample			glRenderbufferStorageMultisampleAPPLE
		#define glResolveMultisampleFramebuffer				glResolveMultisampleFramebufferAPPLE

			#define glVertexAttribDivisor 						glVertexAttribDivisorEXT
	#endif

// -----------------------------------------------------------------------------
// Android and Linux
// -----------------------------------------------------------------------------
#elif ( defined( CINDER_ANDROID ) || defined( CINDER_LINUX ) )
	// ----------------------------------------------------------------------------
	// OpenGL ES 3.0
	// ----------------------------------------------------------------------------
	#if defined( CINDER_LINUX ) && ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3 )
		extern PFNGLREADBUFFERPROC fnptr_ci_glReadBuffer;
		extern PFNGLDRAWRANGEELEMENTSPROC fnptr_ci_glDrawRangeElements;
		extern PFNGLTEXIMAGE3DPROC fnptr_ci_glTexImage3D;
		extern PFNGLTEXSUBIMAGE3DPROC fnptr_ci_glTexSubImage3D;
		extern PFNGLCOPYTEXSUBIMAGE3DPROC fnptr_ci_glCopyTexSubImage3D;
		extern PFNGLCOMPRESSEDTEXIMAGE3DPROC fnptr_ci_glCompressedTexImage3D;
		extern PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC fnptr_ci_glCompressedTexSubImage3D;
		extern PFNGLGENQUERIESPROC fnptr_ci_glGenQueries;
		extern PFNGLDELETEQUERIESPROC fnptr_ci_glDeleteQueries;
		extern PFNGLISQUERYPROC fnptr_ci_glIsQuery;
		extern PFNGLBEGINQUERYPROC fnptr_ci_glBeginQuery;
		extern PFNGLENDQUERYPROC fnptr_ci_glEndQuery;
		extern PFNGLGETQUERYIVPROC fnptr_ci_glGetQueryiv;
		extern PFNGLGETQUERYOBJECTUIVPROC fnptr_ci_glGetQueryObjectuiv;
		extern PFNGLUNMAPBUFFERPROC fnptr_ci_glUnmapBuffer;
		extern PFNGLGETBUFFERPOINTERVPROC fnptr_ci_glGetBufferPointerv;
		extern PFNGLDRAWBUFFERSPROC fnptr_ci_glDrawBuffers;
		extern PFNGLUNIFORMMATRIX2X3FVPROC fnptr_ci_glUniformMatrix2x3fv;
		extern PFNGLUNIFORMMATRIX3X2FVPROC fnptr_ci_glUniformMatrix3x2fv;
		extern PFNGLUNIFORMMATRIX2X4FVPROC fnptr_ci_glUniformMatrix2x4fv;
		extern PFNGLUNIFORMMATRIX4X2FVPROC fnptr_ci_glUniformMatrix4x2fv;
		extern PFNGLUNIFORMMATRIX3X4FVPROC fnptr_ci_glUniformMatrix3x4fv;
		extern PFNGLUNIFORMMATRIX4X3FVPROC fnptr_ci_glUniformMatrix4x3fv;
		extern PFNGLBLITFRAMEBUFFERPROC fnptr_ci_glBlitFramebuffer;
		extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC fnptr_ci_glRenderbufferStorageMultisample;
		extern PFNGLFRAMEBUFFERTEXTURELAYERPROC fnptr_ci_glFramebufferTextureLayer;
		extern PFNGLMAPBUFFERRANGEPROC fnptr_ci_glMapBufferRange;
		extern PFNGLFLUSHMAPPEDBUFFERRANGEPROC fnptr_ci_glFlushMappedBufferRange;
		extern PFNGLBINDVERTEXARRAYPROC fnptr_ci_glBindVertexArray;
		extern PFNGLDELETEVERTEXARRAYSPROC fnptr_ci_glDeleteVertexArrays;
		extern PFNGLGENVERTEXARRAYSPROC fnptr_ci_glGenVertexArrays;
		extern PFNGLISVERTEXARRAYPROC fnptr_ci_glIsVertexArray;
		extern PFNGLGETINTEGERI_VPROC fnptr_ci_glGetIntegeri_v;
		extern PFNGLBEGINTRANSFORMFEEDBACKPROC fnptr_ci_glBeginTransformFeedback;
		extern PFNGLENDTRANSFORMFEEDBACKPROC fnptr_ci_glEndTransformFeedback;
		extern PFNGLBINDBUFFERRANGEPROC fnptr_ci_glBindBufferRange;
		extern PFNGLBINDBUFFERBASEPROC fnptr_ci_glBindBufferBase;
		extern PFNGLTRANSFORMFEEDBACKVARYINGSPROC fnptr_ci_glTransformFeedbackVaryings;
		extern PFNGLGETTRANSFORMFEEDBACKVARYINGPROC fnptr_ci_glGetTransformFeedbackVarying;
		extern PFNGLVERTEXATTRIBIPOINTERPROC fnptr_ci_glVertexAttribIPointer;
		extern PFNGLGETVERTEXATTRIBIIVPROC fnptr_ci_glGetVertexAttribIiv;
		extern PFNGLGETVERTEXATTRIBIUIVPROC fnptr_ci_glGetVertexAttribIuiv;
		extern PFNGLVERTEXATTRIBI4IPROC fnptr_ci_glVertexAttribI4i;
		extern PFNGLVERTEXATTRIBI4UIPROC fnptr_ci_glVertexAttribI4ui;
		extern PFNGLVERTEXATTRIBI4IVPROC fnptr_ci_glVertexAttribI4iv;
		extern PFNGLVERTEXATTRIBI4UIVPROC fnptr_ci_glVertexAttribI4uiv;
		extern PFNGLGETUNIFORMUIVPROC fnptr_ci_glGetUniformuiv;
		extern PFNGLGETFRAGDATALOCATIONPROC fnptr_ci_glGetFragDataLocation;
		extern PFNGLUNIFORM1UIPROC fnptr_ci_glUniform1ui;
		extern PFNGLUNIFORM2UIPROC fnptr_ci_glUniform2ui;
		extern PFNGLUNIFORM3UIPROC fnptr_ci_glUniform3ui;
		extern PFNGLUNIFORM4UIPROC fnptr_ci_glUniform4ui;
		extern PFNGLUNIFORM1UIVPROC fnptr_ci_glUniform1uiv;
		extern PFNGLUNIFORM2UIVPROC fnptr_ci_glUniform2uiv;
		extern PFNGLUNIFORM3UIVPROC fnptr_ci_glUniform3uiv;
		extern PFNGLUNIFORM4UIVPROC fnptr_ci_glUniform4uiv;
		extern PFNGLCLEARBUFFERIVPROC fnptr_ci_glClearBufferiv;
		extern PFNGLCLEARBUFFERUIVPROC fnptr_ci_glClearBufferuiv;
		extern PFNGLCLEARBUFFERFVPROC fnptr_ci_glClearBufferfv;
		extern PFNGLCLEARBUFFERFIPROC fnptr_ci_glClearBufferfi;
		extern PFNGLGETSTRINGIPROC fnptr_ci_glGetStringi;
		extern PFNGLCOPYBUFFERSUBDATAPROC fnptr_ci_glCopyBufferSubData;
		extern PFNGLGETUNIFORMINDICESPROC fnptr_ci_glGetUniformIndices;
		extern PFNGLGETACTIVEUNIFORMSIVPROC fnptr_ci_glGetActiveUniformsiv;
		extern PFNGLGETUNIFORMBLOCKINDEXPROC fnptr_ci_glGetUniformBlockIndex;
		extern PFNGLGETACTIVEUNIFORMBLOCKIVPROC fnptr_ci_glGetActiveUniformBlockiv;
		extern PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC fnptr_ci_glGetActiveUniformBlockName;
		extern PFNGLUNIFORMBLOCKBINDINGPROC fnptr_ci_glUniformBlockBinding;
		extern PFNGLDRAWARRAYSINSTANCEDPROC fnptr_ci_glDrawArraysInstanced;
		extern PFNGLDRAWELEMENTSINSTANCEDPROC fnptr_ci_glDrawElementsInstanced;
		extern PFNGLFENCESYNCPROC fnptr_ci_glFenceSync;
		extern PFNGLISSYNCPROC fnptr_ci_glIsSync;
		extern PFNGLDELETESYNCPROC fnptr_ci_glDeleteSync;
		extern PFNGLCLIENTWAITSYNCPROC fnptr_ci_glClientWaitSync;
		extern PFNGLWAITSYNCPROC fnptr_ci_glWaitSync;
		extern PFNGLGETINTEGER64VPROC fnptr_ci_glGetInteger64v;
		extern PFNGLGETSYNCIVPROC fnptr_ci_glGetSynciv;
		extern PFNGLGETINTEGER64I_VPROC fnptr_ci_glGetInteger64i_v;
		extern PFNGLGETBUFFERPARAMETERI64VPROC fnptr_ci_glGetBufferParameteri64v;
		extern PFNGLGENSAMPLERSPROC fnptr_ci_glGenSamplers;
		extern PFNGLDELETESAMPLERSPROC fnptr_ci_glDeleteSamplers;
		extern PFNGLISSAMPLERPROC fnptr_ci_glIsSampler;
		extern PFNGLBINDSAMPLERPROC fnptr_ci_glBindSampler;
		extern PFNGLSAMPLERPARAMETERIPROC fnptr_ci_glSamplerParameteri;
		extern PFNGLSAMPLERPARAMETERIVPROC fnptr_ci_glSamplerParameteriv;
		extern PFNGLSAMPLERPARAMETERFPROC fnptr_ci_glSamplerParameterf;
		extern PFNGLSAMPLERPARAMETERFVPROC fnptr_ci_glSamplerParameterfv;
		extern PFNGLGETSAMPLERPARAMETERIVPROC fnptr_ci_glGetSamplerParameteriv;
		extern PFNGLGETSAMPLERPARAMETERFVPROC fnptr_ci_glGetSamplerParameterfv;
		extern PFNGLVERTEXATTRIBDIVISORPROC fnptr_ci_glVertexAttribDivisor;
		extern PFNGLBINDTRANSFORMFEEDBACKPROC fnptr_ci_glBindTransformFeedback;
		extern PFNGLDELETETRANSFORMFEEDBACKSPROC fnptr_ci_glDeleteTransformFeedbacks;
		extern PFNGLGENTRANSFORMFEEDBACKSPROC fnptr_ci_glGenTransformFeedbacks;
		extern PFNGLISTRANSFORMFEEDBACKPROC fnptr_ci_glIsTransformFeedback;
		extern PFNGLPAUSETRANSFORMFEEDBACKPROC fnptr_ci_glPauseTransformFeedback;
		extern PFNGLRESUMETRANSFORMFEEDBACKPROC fnptr_ci_glResumeTransformFeedback;
		extern PFNGLGETPROGRAMBINARYPROC fnptr_ci_glGetProgramBinary;
		extern PFNGLPROGRAMBINARYPROC fnptr_ci_glProgramBinary;
		extern PFNGLPROGRAMPARAMETERIPROC fnptr_ci_glProgramParameteri;
		extern PFNGLINVALIDATEFRAMEBUFFERPROC fnptr_ci_glInvalidateFramebuffer;
		extern PFNGLINVALIDATESUBFRAMEBUFFERPROC fnptr_ci_glInvalidateSubFramebuffer;
		extern PFNGLTEXSTORAGE2DPROC fnptr_ci_glTexStorage2D;
		extern PFNGLTEXSTORAGE3DPROC fnptr_ci_glTexStorage3D;
		extern PFNGLGETINTERNALFORMATIVPROC fnptr_ci_glGetInternalformativ;

		#define glReadBuffer fnptr_ci_glReadBuffer
		#define glDrawRangeElements fnptr_ci_glDrawRangeElements
		#define glTexImage3D fnptr_ci_glTexImage3D
		#define glTexSubImage3D fnptr_ci_glTexSubImage3D
		#define glCopyTexSubImage3D fnptr_ci_glCopyTexSubImage3D
		#define glCompressedTexImage3D fnptr_ci_glCompressedTexImage3D
		#define glCompressedTexSubImage3D fnptr_ci_glCompressedTexSubImage3D
		#define glGenQueries fnptr_ci_glGenQueries
		#define glDeleteQueries fnptr_ci_glDeleteQueries
		#define glIsQuery fnptr_ci_glIsQuery
		#define glBeginQuery fnptr_ci_glBeginQuery
		#define glEndQuery fnptr_ci_glEndQuery
		#define glGetQueryiv fnptr_ci_glGetQueryiv
		#define glGetQueryObjectuiv fnptr_ci_glGetQueryObjectuiv
		#define glUnmapBuffer fnptr_ci_glUnmapBuffer
		#define glGetBufferPointerv fnptr_ci_glGetBufferPointerv
		#define glDrawBuffers fnptr_ci_glDrawBuffers
		#define glUniformMatrix2x3fv fnptr_ci_glUniformMatrix2x3fv
		#define glUniformMatrix3x2fv fnptr_ci_glUniformMatrix3x2fv
		#define glUniformMatrix2x4fv fnptr_ci_glUniformMatrix2x4fv
		#define glUniformMatrix4x2fv fnptr_ci_glUniformMatrix4x2fv
		#define glUniformMatrix3x4fv fnptr_ci_glUniformMatrix3x4fv
		#define glUniformMatrix4x3fv fnptr_ci_glUniformMatrix4x3fv
		#define glBlitFramebuffer fnptr_ci_glBlitFramebuffer
		#define glRenderbufferStorageMultisample fnptr_ci_glRenderbufferStorageMultisample
		#define glFramebufferTextureLayer fnptr_ci_glFramebufferTextureLayer
		#define glMapBufferRange fnptr_ci_glMapBufferRange
		#define glFlushMappedBufferRange fnptr_ci_glFlushMappedBufferRange
		#define glBindVertexArray fnptr_ci_glBindVertexArray
		#define glDeleteVertexArrays fnptr_ci_glDeleteVertexArrays
		#define glGenVertexArrays fnptr_ci_glGenVertexArrays
		#define glIsVertexArray fnptr_ci_glIsVertexArray
		#define glGetIntegeri_v fnptr_ci_glGetIntegeri_v
		#define glBeginTransformFeedback fnptr_ci_glBeginTransformFeedback
		#define glEndTransformFeedback fnptr_ci_glEndTransformFeedback
		#define glBindBufferRange fnptr_ci_glBindBufferRange
		#define glBindBufferBase fnptr_ci_glBindBufferBase
		#define glTransformFeedbackVaryings fnptr_ci_glTransformFeedbackVaryings
		#define glGetTransformFeedbackVarying fnptr_ci_glGetTransformFeedbackVarying
		#define glVertexAttribIPointer fnptr_ci_glVertexAttribIPointer
		#define glGetVertexAttribIiv fnptr_ci_glGetVertexAttribIiv
		#define glGetVertexAttribIuiv fnptr_ci_glGetVertexAttribIuiv
		#define glVertexAttribI4i fnptr_ci_glVertexAttribI4i
		#define glVertexAttribI4ui fnptr_ci_glVertexAttribI4ui
		#define glVertexAttribI4iv fnptr_ci_glVertexAttribI4iv
		#define glVertexAttribI4uiv fnptr_ci_glVertexAttribI4uiv
		#define glGetUniformuiv fnptr_ci_glGetUniformuiv
		#define glGetFragDataLocation fnptr_ci_glGetFragDataLocation
		#define glUniform1ui fnptr_ci_glUniform1ui
		#define glUniform2ui fnptr_ci_glUniform2ui
		#define glUniform3ui fnptr_ci_glUniform3ui
		#define glUniform4ui fnptr_ci_glUniform4ui
		#define glUniform1uiv fnptr_ci_glUniform1uiv
		#define glUniform2uiv fnptr_ci_glUniform2uiv
		#define glUniform3uiv fnptr_ci_glUniform3uiv
		#define glUniform4uiv fnptr_ci_glUniform4uiv
		#define glClearBufferiv fnptr_ci_glClearBufferiv
		#define glClearBufferuiv fnptr_ci_glClearBufferuiv
		#define glClearBufferfv fnptr_ci_glClearBufferfv
		#define glClearBufferfi fnptr_ci_glClearBufferfi
		#define glGetStringi fnptr_ci_glGetStringi
		#define glCopyBufferSubData fnptr_ci_glCopyBufferSubData
		#define glGetUniformIndices fnptr_ci_glGetUniformIndices
		#define glGetActiveUniformsiv fnptr_ci_glGetActiveUniformsiv
		#define glGetUniformBlockIndex fnptr_ci_glGetUniformBlockIndex
		#define glGetActiveUniformBlockiv fnptr_ci_glGetActiveUniformBlockiv
		#define glGetActiveUniformBlockName fnptr_ci_glGetActiveUniformBlockName
		#define glUniformBlockBinding fnptr_ci_glUniformBlockBinding
		#define glDrawArraysInstanced fnptr_ci_glDrawArraysInstanced
		#define glDrawElementsInstanced fnptr_ci_glDrawElementsInstanced
		#define glFenceSync fnptr_ci_glFenceSync
		#define glIsSync fnptr_ci_glIsSync
		#define glDeleteSync fnptr_ci_glDeleteSync
		#define glClientWaitSync fnptr_ci_glClientWaitSync
		#define glWaitSync fnptr_ci_glWaitSync
		#define glGetInteger64v fnptr_ci_glGetInteger64v
		#define glGetSynciv fnptr_ci_glGetSynciv
		#define glGetInteger64i_v fnptr_ci_glGetInteger64i_v
		#define glGetBufferParameteri64v fnptr_ci_glGetBufferParameteri64v
		#define glGenSamplers fnptr_ci_glGenSamplers
		#define glDeleteSamplers fnptr_ci_glDeleteSamplers
		#define glIsSampler fnptr_ci_glIsSampler
		#define glBindSampler fnptr_ci_glBindSampler
		#define glSamplerParameteri fnptr_ci_glSamplerParameteri
		#define glSamplerParameteriv fnptr_ci_glSamplerParameteriv
		#define glSamplerParameterf fnptr_ci_glSamplerParameterf
		#define glSamplerParameterfv fnptr_ci_glSamplerParameterfv
		#define glGetSamplerParameteriv fnptr_ci_glGetSamplerParameteriv
		#define glGetSamplerParameterfv fnptr_ci_glGetSamplerParameterfv
		#define glVertexAttribDivisor fnptr_ci_glVertexAttribDivisor
		#define glBindTransformFeedback fnptr_ci_glBindTransformFeedback
		#define glDeleteTransformFeedbacks fnptr_ci_glDeleteTransformFeedbacks
		#define glGenTransformFeedbacks fnptr_ci_glGenTransformFeedbacks
		#define glIsTransformFeedback fnptr_ci_glIsTransformFeedback
		#define glPauseTransformFeedback fnptr_ci_glPauseTransformFeedback
		#define glResumeTransformFeedback fnptr_ci_glResumeTransformFeedback
		#define glGetProgramBinary fnptr_ci_glGetProgramBinary
		#define glProgramBinary fnptr_ci_glProgramBinary
		#define glProgramParameteri fnptr_ci_glProgramParameteri
		#define glInvalidateFramebuffer fnptr_ci_glInvalidateFramebuffer
		#define glInvalidateSubFramebuffer fnptr_ci_glInvalidateSubFramebuffer
		#define glTexStorage2D fnptr_ci_glTexStorage2D
		#define glTexStorage3D fnptr_ci_glTexStorage3D
		#define glGetInternalformativ fnptr_ci_glGetInternalformativ
	#endif // defined( CINDER_LINUX ) && ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3 )

	// ----------------------------------------------------------------------------
	// OpenGL ES 3.1
	// ----------------------------------------------------------------------------
	#if  defined( CINDER_LINUX ) && ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3_1 )
		extern PFNGLDISPATCHCOMPUTEPROC fnptr_ci_glDispatchCompute; 
		extern PFNGLDISPATCHCOMPUTEINDIRECTPROC fnptr_ci_glDispatchComputeIndirect; 
		extern PFNGLDRAWARRAYSINDIRECTPROC fnptr_ci_glDrawArraysIndirect; 
		extern PFNGLDRAWELEMENTSINDIRECTPROC fnptr_ci_glDrawElementsIndirect; 
		extern PFNGLFRAMEBUFFERPARAMETERIPROC fnptr_ci_glFramebufferParameteri; 
		extern PFNGLGETFRAMEBUFFERPARAMETERIVPROC fnptr_ci_glGetFramebufferParameteriv; 
		extern PFNGLGETPROGRAMINTERFACEIVPROC fnptr_ci_glGetProgramInterfaceiv; 
		extern PFNGLGETPROGRAMRESOURCEINDEXPROC fnptr_ci_glGetProgramResourceIndex; 
		extern PFNGLGETPROGRAMRESOURCENAMEPROC fnptr_ci_glGetProgramResourceName; 
		extern PFNGLGETPROGRAMRESOURCEIVPROC fnptr_ci_glGetProgramResourceiv; 
		extern PFNGLGETPROGRAMRESOURCELOCATIONPROC fnptr_ci_glGetProgramResourceLocation; 
		extern PFNGLUSEPROGRAMSTAGESPROC fnptr_ci_glUseProgramStages; 
		extern PFNGLACTIVESHADERPROGRAMPROC fnptr_ci_glActiveShaderProgram; 
		extern PFNGLCREATESHADERPROGRAMVPROC fnptr_ci_glCreateShaderProgramv; 
		extern PFNGLBINDPROGRAMPIPELINEPROC fnptr_ci_glBindProgramPipeline; 
		extern PFNGLDELETEPROGRAMPIPELINESPROC fnptr_ci_glDeleteProgramPipelines; 
		extern PFNGLGENPROGRAMPIPELINESPROC fnptr_ci_glGenProgramPipelines; 
		extern PFNGLISPROGRAMPIPELINEPROC fnptr_ci_glIsProgramPipeline; 
		extern PFNGLGETPROGRAMPIPELINEIVPROC fnptr_ci_glGetProgramPipelineiv; 
		extern PFNGLPROGRAMUNIFORM1IPROC fnptr_ci_glProgramUniform1i; 
		extern PFNGLPROGRAMUNIFORM2IPROC fnptr_ci_glProgramUniform2i; 
		extern PFNGLPROGRAMUNIFORM3IPROC fnptr_ci_glProgramUniform3i; 
		extern PFNGLPROGRAMUNIFORM4IPROC fnptr_ci_glProgramUniform4i; 
		extern PFNGLPROGRAMUNIFORM1UIPROC fnptr_ci_glProgramUniform1ui; 
		extern PFNGLPROGRAMUNIFORM2UIPROC fnptr_ci_glProgramUniform2ui; 
		extern PFNGLPROGRAMUNIFORM3UIPROC fnptr_ci_glProgramUniform3ui; 
		extern PFNGLPROGRAMUNIFORM4UIPROC fnptr_ci_glProgramUniform4ui; 
		extern PFNGLPROGRAMUNIFORM1FPROC fnptr_ci_glProgramUniform1f; 
		extern PFNGLPROGRAMUNIFORM2FPROC fnptr_ci_glProgramUniform2f; 
		extern PFNGLPROGRAMUNIFORM3FPROC fnptr_ci_glProgramUniform3f; 
		extern PFNGLPROGRAMUNIFORM4FPROC fnptr_ci_glProgramUniform4f; 
		extern PFNGLPROGRAMUNIFORM1IVPROC fnptr_ci_glProgramUniform1iv; 
		extern PFNGLPROGRAMUNIFORM2IVPROC fnptr_ci_glProgramUniform2iv; 
		extern PFNGLPROGRAMUNIFORM3IVPROC fnptr_ci_glProgramUniform3iv; 
		extern PFNGLPROGRAMUNIFORM4IVPROC fnptr_ci_glProgramUniform4iv; 
		extern PFNGLPROGRAMUNIFORM1UIVPROC fnptr_ci_glProgramUniform1uiv; 
		extern PFNGLPROGRAMUNIFORM2UIVPROC fnptr_ci_glProgramUniform2uiv; 
		extern PFNGLPROGRAMUNIFORM3UIVPROC fnptr_ci_glProgramUniform3uiv; 
		extern PFNGLPROGRAMUNIFORM4UIVPROC fnptr_ci_glProgramUniform4uiv; 
		extern PFNGLPROGRAMUNIFORM1FVPROC fnptr_ci_glProgramUniform1fv; 
		extern PFNGLPROGRAMUNIFORM2FVPROC fnptr_ci_glProgramUniform2fv; 
		extern PFNGLPROGRAMUNIFORM3FVPROC fnptr_ci_glProgramUniform3fv; 
		extern PFNGLPROGRAMUNIFORM4FVPROC fnptr_ci_glProgramUniform4fv; 
		extern PFNGLPROGRAMUNIFORMMATRIX2FVPROC fnptr_ci_glProgramUniformMatrix2fv; 
		extern PFNGLPROGRAMUNIFORMMATRIX3FVPROC fnptr_ci_glProgramUniformMatrix3fv; 
		extern PFNGLPROGRAMUNIFORMMATRIX4FVPROC fnptr_ci_glProgramUniformMatrix4fv; 
		extern PFNGLPROGRAMUNIFORMMATRIX2X3FVPROC fnptr_ci_glProgramUniformMatrix2x3fv; 
		extern PFNGLPROGRAMUNIFORMMATRIX3X2FVPROC fnptr_ci_glProgramUniformMatrix3x2fv; 
		extern PFNGLPROGRAMUNIFORMMATRIX2X4FVPROC fnptr_ci_glProgramUniformMatrix2x4fv; 
		extern PFNGLPROGRAMUNIFORMMATRIX4X2FVPROC fnptr_ci_glProgramUniformMatrix4x2fv; 
		extern PFNGLPROGRAMUNIFORMMATRIX3X4FVPROC fnptr_ci_glProgramUniformMatrix3x4fv; 
		extern PFNGLPROGRAMUNIFORMMATRIX4X3FVPROC fnptr_ci_glProgramUniformMatrix4x3fv; 
		extern PFNGLVALIDATEPROGRAMPIPELINEPROC fnptr_ci_glValidateProgramPipeline; 
		extern PFNGLGETPROGRAMPIPELINEINFOLOGPROC fnptr_ci_glGetProgramPipelineInfoLog; 
		extern PFNGLBINDIMAGETEXTUREPROC fnptr_ci_glBindImageTexture; 
		extern PFNGLGETBOOLEANI_VPROC fnptr_ci_glGetBooleani_v; 
		extern PFNGLMEMORYBARRIERPROC fnptr_ci_glMemoryBarrier; 
		extern PFNGLMEMORYBARRIERBYREGIONPROC fnptr_ci_glMemoryBarrierByRegion; 
		extern PFNGLTEXSTORAGE2DMULTISAMPLEPROC fnptr_ci_glTexStorage2DMultisample; 
		extern PFNGLGETMULTISAMPLEFVPROC fnptr_ci_glGetMultisamplefv; 
		extern PFNGLSAMPLEMASKIPROC fnptr_ci_glSampleMaski; 
		extern PFNGLGETTEXLEVELPARAMETERIVPROC fnptr_ci_glGetTexLevelParameteriv; 
		extern PFNGLGETTEXLEVELPARAMETERFVPROC fnptr_ci_glGetTexLevelParameterfv; 
		extern PFNGLBINDVERTEXBUFFERPROC fnptr_ci_glBindVertexBuffer; 
		extern PFNGLVERTEXATTRIBFORMATPROC fnptr_ci_glVertexAttribFormat; 
		extern PFNGLVERTEXATTRIBIFORMATPROC fnptr_ci_glVertexAttribIFormat; 
		extern PFNGLVERTEXATTRIBBINDINGPROC fnptr_ci_glVertexAttribBinding; 
		extern PFNGLVERTEXBINDINGDIVISORPROC fnptr_ci_glVertexBindingDivisor; 

		#define glDispatchCompute fnptr_ci_glDispatchCompute 
		#define glDispatchComputeIndirect fnptr_ci_glDispatchComputeIndirect 
		#define glDrawArraysIndirect fnptr_ci_glDrawArraysIndirect 
		#define glDrawElementsIndirect fnptr_ci_glDrawElementsIndirect 
		#define glFramebufferParameteri fnptr_ci_glFramebufferParameteri 
		#define glGetFramebufferParameteriv fnptr_ci_glGetFramebufferParameteriv 
		#define glGetProgramInterfaceiv fnptr_ci_glGetProgramInterfaceiv 
		#define glGetProgramResourceIndex fnptr_ci_glGetProgramResourceIndex 
		#define glGetProgramResourceName fnptr_ci_glGetProgramResourceName 
		#define glGetProgramResourceiv fnptr_ci_glGetProgramResourceiv 
		#define glGetProgramResourceLocation fnptr_ci_glGetProgramResourceLocation 
		#define glUseProgramStages fnptr_ci_glUseProgramStages 
		#define glActiveShaderProgram fnptr_ci_glActiveShaderProgram 
		#define glCreateShaderProgramv fnptr_ci_glCreateShaderProgramv 
		#define glBindProgramPipeline fnptr_ci_glBindProgramPipeline 
		#define glDeleteProgramPipelines fnptr_ci_glDeleteProgramPipelines 
		#define glGenProgramPipelines fnptr_ci_glGenProgramPipelines 
		#define glIsProgramPipeline fnptr_ci_glIsProgramPipeline 
		#define glGetProgramPipelineiv fnptr_ci_glGetProgramPipelineiv 
		#define glProgramUniform1i fnptr_ci_glProgramUniform1i 
		#define glProgramUniform2i fnptr_ci_glProgramUniform2i 
		#define glProgramUniform3i fnptr_ci_glProgramUniform3i 
		#define glProgramUniform4i fnptr_ci_glProgramUniform4i 
		#define glProgramUniform1ui fnptr_ci_glProgramUniform1ui 
		#define glProgramUniform2ui fnptr_ci_glProgramUniform2ui 
		#define glProgramUniform3ui fnptr_ci_glProgramUniform3ui 
		#define glProgramUniform4ui fnptr_ci_glProgramUniform4ui 
		#define glProgramUniform1f fnptr_ci_glProgramUniform1f 
		#define glProgramUniform2f fnptr_ci_glProgramUniform2f 
		#define glProgramUniform3f fnptr_ci_glProgramUniform3f 
		#define glProgramUniform4f fnptr_ci_glProgramUniform4f 
		#define glProgramUniform1iv fnptr_ci_glProgramUniform1iv 
		#define glProgramUniform2iv fnptr_ci_glProgramUniform2iv 
		#define glProgramUniform3iv fnptr_ci_glProgramUniform3iv 
		#define glProgramUniform4iv fnptr_ci_glProgramUniform4iv 
		#define glProgramUniform1uiv fnptr_ci_glProgramUniform1uiv 
		#define glProgramUniform2uiv fnptr_ci_glProgramUniform2uiv 
		#define glProgramUniform3uiv fnptr_ci_glProgramUniform3uiv 
		#define glProgramUniform4uiv fnptr_ci_glProgramUniform4uiv 
		#define glProgramUniform1fv fnptr_ci_glProgramUniform1fv 
		#define glProgramUniform2fv fnptr_ci_glProgramUniform2fv 
		#define glProgramUniform3fv fnptr_ci_glProgramUniform3fv 
		#define glProgramUniform4fv fnptr_ci_glProgramUniform4fv 
		#define glProgramUniformMatrix2fv fnptr_ci_glProgramUniformMatrix2fv 
		#define glProgramUniformMatrix3fv fnptr_ci_glProgramUniformMatrix3fv 
		#define glProgramUniformMatrix4fv fnptr_ci_glProgramUniformMatrix4fv 
		#define glProgramUniformMatrix2x3fv fnptr_ci_glProgramUniformMatrix2x3fv 
		#define glProgramUniformMatrix3x2fv fnptr_ci_glProgramUniformMatrix3x2fv 
		#define glProgramUniformMatrix2x4fv fnptr_ci_glProgramUniformMatrix2x4fv 
		#define glProgramUniformMatrix4x2fv fnptr_ci_glProgramUniformMatrix4x2fv 
		#define glProgramUniformMatrix3x4fv fnptr_ci_glProgramUniformMatrix3x4fv 
		#define glProgramUniformMatrix4x3fv fnptr_ci_glProgramUniformMatrix4x3fv 
		#define glValidateProgramPipeline fnptr_ci_glValidateProgramPipeline 
		#define glGetProgramPipelineInfoLog fnptr_ci_glGetProgramPipelineInfoLog 
		#define glBindImageTexture fnptr_ci_glBindImageTexture 
		#define glGetBooleani_v fnptr_ci_glGetBooleani_v 
		#define glMemoryBarrier fnptr_ci_glMemoryBarrier 
		#define glMemoryBarrierByRegion fnptr_ci_glMemoryBarrierByRegion 
		#define glTexStorage2DMultisample fnptr_ci_glTexStorage2DMultisample 
		#define glGetMultisamplefv fnptr_ci_glGetMultisamplefv 
		#define glSampleMaski fnptr_ci_glSampleMaski 
		#define glGetTexLevelParameteriv fnptr_ci_glGetTexLevelParameteriv 
		#define glGetTexLevelParameterfv fnptr_ci_glGetTexLevelParameterfv 
		#define glBindVertexBuffer fnptr_ci_glBindVertexBuffer 
		#define glVertexAttribFormat fnptr_ci_glVertexAttribFormat 
		#define glVertexAttribIFormat fnptr_ci_glVertexAttribIFormat 
		#define glVertexAttribBinding fnptr_ci_glVertexAttribBinding 
		#define glVertexBindingDivisor fnptr_ci_glVertexBindingDivisor
	#endif // defined( CINDER_LINUX ) && ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3_1 )

	// ----------------------------------------------------------------------------
	// OpenGL ES 3.2
	// ----------------------------------------------------------------------------
	#if  defined( CINDER_LINUX ) && ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3_2 )
		extern PFNGLBLENDBARRIERPROC fnptr_ci_glBlendBarrier; 
		extern PFNGLCOPYIMAGESUBDATAPROC fnptr_ci_glCopyImageSubData; 
		extern PFNGLDEBUGMESSAGECONTROLPROC fnptr_ci_glDebugMessageControl; 
		extern PFNGLDEBUGMESSAGEINSERTPROC fnptr_ci_glDebugMessageInsert; 
		extern PFNGLDEBUGMESSAGECALLBACKPROC fnptr_ci_glDebugMessageCallback; 
		extern PFNGLGETDEBUGMESSAGELOGPROC fnptr_ci_glGetDebugMessageLog; 
		extern PFNGLPUSHDEBUGGROUPPROC fnptr_ci_glPushDebugGroup; 
		extern PFNGLPOPDEBUGGROUPPROC fnptr_ci_glPopDebugGroup; 
		extern PFNGLOBJECTLABELPROC fnptr_ci_glObjectLabel; 
		extern PFNGLGETOBJECTLABELPROC fnptr_ci_glGetObjectLabel; 
		extern PFNGLOBJECTPTRLABELPROC fnptr_ci_glObjectPtrLabel; 
		extern PFNGLGETOBJECTPTRLABELPROC fnptr_ci_glGetObjectPtrLabel; 
		extern PFNGLGETPOINTERVPROC fnptr_ci_glGetPointerv; 
		extern PFNGLENABLEIPROC fnptr_ci_glEnablei; 
		extern PFNGLDISABLEIPROC fnptr_ci_glDisablei; 
		extern PFNGLBLENDEQUATIONIPROC fnptr_ci_glBlendEquationi; 
		extern PFNGLBLENDEQUATIONSEPARATEIPROC fnptr_ci_glBlendEquationSeparatei; 
		extern PFNGLBLENDFUNCIPROC fnptr_ci_glBlendFunci; 
		extern PFNGLBLENDFUNCSEPARATEIPROC fnptr_ci_glBlendFuncSeparatei; 
		extern PFNGLCOLORMASKIPROC fnptr_ci_glColorMaski; 
		extern PFNGLISENABLEDIPROC fnptr_ci_glIsEnabledi; 
		extern PFNGLDRAWELEMENTSBASEVERTEXPROC fnptr_ci_glDrawElementsBaseVertex; 
		extern PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC fnptr_ci_glDrawRangeElementsBaseVertex; 
		extern PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC fnptr_ci_glDrawElementsInstancedBaseVertex; 
		extern PFNGLFRAMEBUFFERTEXTUREPROC fnptr_ci_glFramebufferTexture; 
		extern PFNGLPRIMITIVEBOUNDINGBOXPROC fnptr_ci_glPrimitiveBoundingBox; 
		extern PFNGLGETGRAPHICSRESETSTATUSPROC fnptr_ci_glGetGraphicsResetStatus; 
		extern PFNGLREADNPIXELSPROC fnptr_ci_glReadnPixels; 
		extern PFNGLGETNUNIFORMFVPROC fnptr_ci_glGetnUniformfv; 
		extern PFNGLGETNUNIFORMIVPROC fnptr_ci_glGetnUniformiv; 
		extern PFNGLGETNUNIFORMUIVPROC fnptr_ci_glGetnUniformuiv; 
		extern PFNGLMINSAMPLESHADINGPROC fnptr_ci_glMinSampleShading; 
		extern PFNGLPATCHPARAMETERIPROC fnptr_ci_glPatchParameteri; 
		extern PFNGLTEXPARAMETERIIVPROC fnptr_ci_glTexParameterIiv; 
		extern PFNGLTEXPARAMETERIUIVPROC fnptr_ci_glTexParameterIuiv; 
		extern PFNGLGETTEXPARAMETERIIVPROC fnptr_ci_glGetTexParameterIiv; 
		extern PFNGLGETTEXPARAMETERIUIVPROC fnptr_ci_glGetTexParameterIuiv; 
		extern PFNGLSAMPLERPARAMETERIIVPROC fnptr_ci_glSamplerParameterIiv; 
		extern PFNGLSAMPLERPARAMETERIUIVPROC fnptr_ci_glSamplerParameterIuiv; 
		extern PFNGLGETSAMPLERPARAMETERIIVPROC fnptr_ci_glGetSamplerParameterIiv; 
		extern PFNGLGETSAMPLERPARAMETERIUIVPROC fnptr_ci_glGetSamplerParameterIuiv; 
		extern PFNGLTEXBUFFERPROC fnptr_ci_glTexBuffer; 
		extern PFNGLTEXBUFFERRANGEPROC fnptr_ci_glTexBufferRange; 
		extern PFNGLTEXSTORAGE3DMULTISAMPLEPROC fnptr_ci_glTexStorage3DMultisample; 

		#define glBlendBarrier fnptr_ci_glBlendBarrier 
		#define glCopyImageSubData fnptr_ci_glCopyImageSubData 
		#define glDebugMessageControl fnptr_ci_glDebugMessageControl 
		#define glDebugMessageInsert fnptr_ci_glDebugMessageInsert 
		#define glDebugMessageCallback fnptr_ci_glDebugMessageCallback 
		#define glGetDebugMessageLog fnptr_ci_glGetDebugMessageLog 
		#define glPushDebugGroup fnptr_ci_glPushDebugGroup 
		#define glPopDebugGroup fnptr_ci_glPopDebugGroup 
		#define glObjectLabel fnptr_ci_glObjectLabel 
		#define glGetObjectLabel fnptr_ci_glGetObjectLabel 
		#define glObjectPtrLabel fnptr_ci_glObjectPtrLabel 
		#define glGetObjectPtrLabel fnptr_ci_glGetObjectPtrLabel 
		#define glGetPointerv fnptr_ci_glGetPointerv 
		#define glEnablei fnptr_ci_glEnablei 
		#define glDisablei fnptr_ci_glDisablei 
		#define glBlendEquationi fnptr_ci_glBlendEquationi 
		#define glBlendEquationSeparatei fnptr_ci_glBlendEquationSeparatei 
		#define glBlendFunci fnptr_ci_glBlendFunci 
		#define glBlendFuncSeparatei fnptr_ci_glBlendFuncSeparatei 
		#define glColorMaski fnptr_ci_glColorMaski 
		#define glIsEnabledi fnptr_ci_glIsEnabledi 
		#define glDrawElementsBaseVertex fnptr_ci_glDrawElementsBaseVertex 
		#define glDrawRangeElementsBaseVertex fnptr_ci_glDrawRangeElementsBaseVertex 
		#define glDrawElementsInstancedBaseVertex fnptr_ci_glDrawElementsInstancedBaseVertex 
		#define glFramebufferTexture fnptr_ci_glFramebufferTexture 
		#define glPrimitiveBoundingBox fnptr_ci_glPrimitiveBoundingBox 
		#define glGetGraphicsResetStatus fnptr_ci_glGetGraphicsResetStatus 
		#define glReadnPixels fnptr_ci_glReadnPixels 
		#define glGetnUniformfv fnptr_ci_glGetnUniformfv 
		#define glGetnUniformiv fnptr_ci_glGetnUniformiv 
		#define glGetnUniformuiv fnptr_ci_glGetnUniformuiv 
		#define glMinSampleShading fnptr_ci_glMinSampleShading 
		#define glPatchParameteri fnptr_ci_glPatchParameteri 
		#define glTexParameterIiv fnptr_ci_glTexParameterIiv 
		#define glTexParameterIuiv fnptr_ci_glTexParameterIuiv 
		#define glGetTexParameterIiv fnptr_ci_glGetTexParameterIiv 
		#define glGetTexParameterIuiv fnptr_ci_glGetTexParameterIuiv 
		#define glSamplerParameterIiv fnptr_ci_glSamplerParameterIiv 
		#define glSamplerParameterIuiv fnptr_ci_glSamplerParameterIuiv 
		#define glGetSamplerParameterIiv fnptr_ci_glGetSamplerParameterIiv 
		#define glGetSamplerParameterIuiv fnptr_ci_glGetSamplerParameterIuiv 
		#define glTexBuffer fnptr_ci_glTexBuffer 
		#define glTexBufferRange fnptr_ci_glTexBufferRange 
		#define glTexStorage3DMultisample fnptr_ci_glTexStorage3DMultisample 
	#endif // defined( CINDER_LINUX ) && ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3_2 )

	// -------------------------------------------------------------------------
	// OpenGL ES 2.0 Extensions
	// -------------------------------------------------------------------------
	#if ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_2 )
		extern PFNGLBLENDBARRIERKHRPROC fnptr_ci_glBlendBarrierKHR; 
		extern PFNGLDEBUGMESSAGECONTROLKHRPROC fnptr_ci_glDebugMessageControlKHR; 
		extern PFNGLDEBUGMESSAGEINSERTKHRPROC fnptr_ci_glDebugMessageInsertKHR; 
		extern PFNGLDEBUGMESSAGECALLBACKKHRPROC fnptr_ci_glDebugMessageCallbackKHR; 
		extern PFNGLGETDEBUGMESSAGELOGKHRPROC fnptr_ci_glGetDebugMessageLogKHR; 
		extern PFNGLPUSHDEBUGGROUPKHRPROC fnptr_ci_glPushDebugGroupKHR; 
		extern PFNGLPOPDEBUGGROUPKHRPROC fnptr_ci_glPopDebugGroupKHR; 
		extern PFNGLOBJECTLABELKHRPROC fnptr_ci_glObjectLabelKHR; 
		extern PFNGLGETOBJECTLABELKHRPROC fnptr_ci_glGetObjectLabelKHR; 
		extern PFNGLOBJECTPTRLABELKHRPROC fnptr_ci_glObjectPtrLabelKHR; 
		extern PFNGLGETOBJECTPTRLABELKHRPROC fnptr_ci_glGetObjectPtrLabelKHR; 
		extern PFNGLGETPOINTERVKHRPROC fnptr_ci_glGetPointervKHR; 
		extern PFNGLGETGRAPHICSRESETSTATUSKHRPROC fnptr_ci_glGetGraphicsResetStatusKHR; 
		extern PFNGLREADNPIXELSKHRPROC fnptr_ci_glReadnPixelsKHR; 
		extern PFNGLGETNUNIFORMFVKHRPROC fnptr_ci_glGetnUniformfvKHR; 
		extern PFNGLGETNUNIFORMIVKHRPROC fnptr_ci_glGetnUniformivKHR; 
		extern PFNGLGETNUNIFORMUIVKHRPROC fnptr_ci_glGetnUniformuivKHR; 
		extern PFNGLEGLIMAGETARGETTEXTURE2DOESPROC fnptr_ci_glEGLImageTargetTexture2DOES; 
		extern PFNGLEGLIMAGETARGETRENDERBUFFERSTORAGEOESPROC fnptr_ci_glEGLImageTargetRenderbufferStorageOES; 
		extern PFNGLCOPYIMAGESUBDATAOESPROC fnptr_ci_glCopyImageSubDataOES; 
		extern PFNGLENABLEIOESPROC fnptr_ci_glEnableiOES; 
		extern PFNGLDISABLEIOESPROC fnptr_ci_glDisableiOES; 
		extern PFNGLBLENDEQUATIONIOESPROC fnptr_ci_glBlendEquationiOES; 
		extern PFNGLBLENDEQUATIONSEPARATEIOESPROC fnptr_ci_glBlendEquationSeparateiOES; 
		extern PFNGLBLENDFUNCIOESPROC fnptr_ci_glBlendFunciOES; 
		extern PFNGLBLENDFUNCSEPARATEIOESPROC fnptr_ci_glBlendFuncSeparateiOES; 
		extern PFNGLCOLORMASKIOESPROC fnptr_ci_glColorMaskiOES; 
		extern PFNGLISENABLEDIOESPROC fnptr_ci_glIsEnablediOES; 
		extern PFNGLDRAWELEMENTSBASEVERTEXOESPROC fnptr_ci_glDrawElementsBaseVertexOES; 
		extern PFNGLDRAWRANGEELEMENTSBASEVERTEXOESPROC fnptr_ci_glDrawRangeElementsBaseVertexOES; 
		extern PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXOESPROC fnptr_ci_glDrawElementsInstancedBaseVertexOES; 
		extern PFNGLMULTIDRAWELEMENTSBASEVERTEXOESPROC fnptr_ci_glMultiDrawElementsBaseVertexOES; 
		extern PFNGLFRAMEBUFFERTEXTUREOESPROC fnptr_ci_glFramebufferTextureOES; 
		extern PFNGLGETPROGRAMBINARYOESPROC fnptr_ci_glGetProgramBinaryOES; 
		extern PFNGLPROGRAMBINARYOESPROC fnptr_ci_glProgramBinaryOES; 
		extern PFNGLGETBUFFERPOINTERVOESPROC fnptr_ci_glGetBufferPointervOES; 
		extern PFNGLPRIMITIVEBOUNDINGBOXOESPROC fnptr_ci_glPrimitiveBoundingBoxOES; 
		extern PFNGLMINSAMPLESHADINGOESPROC fnptr_ci_glMinSampleShadingOES; 
		extern PFNGLTEXIMAGE3DOESPROC fnptr_ci_glTexImage3DOES; 
		extern PFNGLTEXSUBIMAGE3DOESPROC fnptr_ci_glTexSubImage3DOES; 
		extern PFNGLCOPYTEXSUBIMAGE3DOESPROC fnptr_ci_glCopyTexSubImage3DOES; 
		extern PFNGLCOMPRESSEDTEXIMAGE3DOESPROC fnptr_ci_glCompressedTexImage3DOES; 
		extern PFNGLCOMPRESSEDTEXSUBIMAGE3DOESPROC fnptr_ci_glCompressedTexSubImage3DOES; 
		extern PFNGLFRAMEBUFFERTEXTURE3DOESPROC fnptr_ci_glFramebufferTexture3DOES; 
		extern PFNGLTEXPARAMETERIIVOESPROC fnptr_ci_glTexParameterIivOES; 
		extern PFNGLTEXPARAMETERIUIVOESPROC fnptr_ci_glTexParameterIuivOES; 
		extern PFNGLGETTEXPARAMETERIIVOESPROC fnptr_ci_glGetTexParameterIivOES; 
		extern PFNGLGETTEXPARAMETERIUIVOESPROC fnptr_ci_glGetTexParameterIuivOES; 
		extern PFNGLSAMPLERPARAMETERIIVOESPROC fnptr_ci_glSamplerParameterIivOES; 
		extern PFNGLSAMPLERPARAMETERIUIVOESPROC fnptr_ci_glSamplerParameterIuivOES; 
		extern PFNGLGETSAMPLERPARAMETERIIVOESPROC fnptr_ci_glGetSamplerParameterIivOES; 
		extern PFNGLGETSAMPLERPARAMETERIUIVOESPROC fnptr_ci_glGetSamplerParameterIuivOES; 
		extern PFNGLTEXBUFFEROESPROC fnptr_ci_glTexBufferOES; 
		extern PFNGLTEXBUFFERRANGEOESPROC fnptr_ci_glTexBufferRangeOES; 
		extern PFNGLTEXSTORAGE3DMULTISAMPLEOESPROC fnptr_ci_glTexStorage3DMultisampleOES; 
		extern PFNGLTEXTUREVIEWOESPROC fnptr_ci_glTextureViewOES; 
		extern PFNGLGETPERFMONITORGROUPSAMDPROC fnptr_ci_glGetPerfMonitorGroupsAMD; 
		extern PFNGLGETPERFMONITORCOUNTERSAMDPROC fnptr_ci_glGetPerfMonitorCountersAMD; 
		extern PFNGLGETPERFMONITORGROUPSTRINGAMDPROC fnptr_ci_glGetPerfMonitorGroupStringAMD; 
		extern PFNGLGETPERFMONITORCOUNTERSTRINGAMDPROC fnptr_ci_glGetPerfMonitorCounterStringAMD; 
		extern PFNGLGETPERFMONITORCOUNTERINFOAMDPROC fnptr_ci_glGetPerfMonitorCounterInfoAMD; 
		extern PFNGLGENPERFMONITORSAMDPROC fnptr_ci_glGenPerfMonitorsAMD; 
		extern PFNGLDELETEPERFMONITORSAMDPROC fnptr_ci_glDeletePerfMonitorsAMD; 
		extern PFNGLSELECTPERFMONITORCOUNTERSAMDPROC fnptr_ci_glSelectPerfMonitorCountersAMD; 
		extern PFNGLBEGINPERFMONITORAMDPROC fnptr_ci_glBeginPerfMonitorAMD; 
		extern PFNGLENDPERFMONITORAMDPROC fnptr_ci_glEndPerfMonitorAMD; 
		extern PFNGLGETPERFMONITORCOUNTERDATAAMDPROC fnptr_ci_glGetPerfMonitorCounterDataAMD; 
		extern PFNGLBLITFRAMEBUFFERANGLEPROC fnptr_ci_glBlitFramebufferANGLE; 
		extern PFNGLGETTRANSLATEDSHADERSOURCEANGLEPROC fnptr_ci_glGetTranslatedShaderSourceANGLE; 
		extern PFNGLCOPYTEXTURELEVELSAPPLEPROC fnptr_ci_glCopyTextureLevelsAPPLE; 
		extern PFNGLFENCESYNCAPPLEPROC fnptr_ci_glFenceSyncAPPLE; 
		extern PFNGLISSYNCAPPLEPROC fnptr_ci_glIsSyncAPPLE; 
		extern PFNGLDELETESYNCAPPLEPROC fnptr_ci_glDeleteSyncAPPLE; 
		extern PFNGLCLIENTWAITSYNCAPPLEPROC fnptr_ci_glClientWaitSyncAPPLE; 
		extern PFNGLWAITSYNCAPPLEPROC fnptr_ci_glWaitSyncAPPLE; 
		extern PFNGLGETINTEGER64VAPPLEPROC fnptr_ci_glGetInteger64vAPPLE; 
		extern PFNGLGETSYNCIVAPPLEPROC fnptr_ci_glGetSyncivAPPLE; 
		extern PFNGLDRAWARRAYSINSTANCEDBASEINSTANCEEXTPROC fnptr_ci_glDrawArraysInstancedBaseInstanceEXT; 
		extern PFNGLDRAWELEMENTSINSTANCEDBASEINSTANCEEXTPROC fnptr_ci_glDrawElementsInstancedBaseInstanceEXT; 
		extern PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXBASEINSTANCEEXTPROC fnptr_ci_glDrawElementsInstancedBaseVertexBaseInstanceEXT; 
		extern PFNGLBINDFRAGDATALOCATIONINDEXEDEXTPROC fnptr_ci_glBindFragDataLocationIndexedEXT; 
		extern PFNGLBINDFRAGDATALOCATIONEXTPROC fnptr_ci_glBindFragDataLocationEXT; 
		extern PFNGLGETPROGRAMRESOURCELOCATIONINDEXEXTPROC fnptr_ci_glGetProgramResourceLocationIndexEXT; 
		extern PFNGLGETFRAGDATAINDEXEXTPROC fnptr_ci_glGetFragDataIndexEXT; 
		extern PFNGLCOPYIMAGESUBDATAEXTPROC fnptr_ci_glCopyImageSubDataEXT; 
		extern PFNGLLABELOBJECTEXTPROC fnptr_ci_glLabelObjectEXT; 
		extern PFNGLGETOBJECTLABELEXTPROC fnptr_ci_glGetObjectLabelEXT; 
		extern PFNGLINSERTEVENTMARKEREXTPROC fnptr_ci_glInsertEventMarkerEXT; 
		extern PFNGLPUSHGROUPMARKEREXTPROC fnptr_ci_glPushGroupMarkerEXT; 
		extern PFNGLPOPGROUPMARKEREXTPROC fnptr_ci_glPopGroupMarkerEXT; 
		extern PFNGLDISCARDFRAMEBUFFEREXTPROC fnptr_ci_glDiscardFramebufferEXT; 
		extern PFNGLGENQUERIESEXTPROC fnptr_ci_glGenQueriesEXT; 
		extern PFNGLDELETEQUERIESEXTPROC fnptr_ci_glDeleteQueriesEXT; 
		extern PFNGLISQUERYEXTPROC fnptr_ci_glIsQueryEXT; 
		extern PFNGLBEGINQUERYEXTPROC fnptr_ci_glBeginQueryEXT; 
		extern PFNGLENDQUERYEXTPROC fnptr_ci_glEndQueryEXT; 
		extern PFNGLQUERYCOUNTEREXTPROC fnptr_ci_glQueryCounterEXT; 
		extern PFNGLGETQUERYIVEXTPROC fnptr_ci_glGetQueryivEXT; 
		extern PFNGLGETQUERYOBJECTIVEXTPROC fnptr_ci_glGetQueryObjectivEXT; 
		extern PFNGLGETQUERYOBJECTUIVEXTPROC fnptr_ci_glGetQueryObjectuivEXT; 
		extern PFNGLGETQUERYOBJECTI64VEXTPROC fnptr_ci_glGetQueryObjecti64vEXT; 
		extern PFNGLGETQUERYOBJECTUI64VEXTPROC fnptr_ci_glGetQueryObjectui64vEXT; 
		extern PFNGLDRAWBUFFERSEXTPROC fnptr_ci_glDrawBuffersEXT; 
		extern PFNGLENABLEIEXTPROC fnptr_ci_glEnableiEXT; 
		extern PFNGLDISABLEIEXTPROC fnptr_ci_glDisableiEXT; 
		extern PFNGLBLENDEQUATIONIEXTPROC fnptr_ci_glBlendEquationiEXT; 
		extern PFNGLBLENDEQUATIONSEPARATEIEXTPROC fnptr_ci_glBlendEquationSeparateiEXT; 
		extern PFNGLBLENDFUNCIEXTPROC fnptr_ci_glBlendFunciEXT; 
		extern PFNGLBLENDFUNCSEPARATEIEXTPROC fnptr_ci_glBlendFuncSeparateiEXT; 
		extern PFNGLCOLORMASKIEXTPROC fnptr_ci_glColorMaskiEXT; 
		extern PFNGLISENABLEDIEXTPROC fnptr_ci_glIsEnablediEXT; 
		extern PFNGLDRAWELEMENTSBASEVERTEXEXTPROC fnptr_ci_glDrawElementsBaseVertexEXT; 
		extern PFNGLDRAWRANGEELEMENTSBASEVERTEXEXTPROC fnptr_ci_glDrawRangeElementsBaseVertexEXT; 
		extern PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXEXTPROC fnptr_ci_glDrawElementsInstancedBaseVertexEXT; 
		extern PFNGLMULTIDRAWELEMENTSBASEVERTEXEXTPROC fnptr_ci_glMultiDrawElementsBaseVertexEXT; 
		extern PFNGLMULTIDRAWARRAYSEXTPROC fnptr_ci_glMultiDrawArraysEXT; 
		extern PFNGLMULTIDRAWELEMENTSEXTPROC fnptr_ci_glMultiDrawElementsEXT; 
		extern PFNGLMULTIDRAWARRAYSINDIRECTEXTPROC fnptr_ci_glMultiDrawArraysIndirectEXT; 
		extern PFNGLMULTIDRAWELEMENTSINDIRECTEXTPROC fnptr_ci_glMultiDrawElementsIndirectEXT; 
		extern PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEEXTPROC fnptr_ci_glFramebufferTexture2DMultisampleEXT; 
		extern PFNGLREADBUFFERINDEXEDEXTPROC fnptr_ci_glReadBufferIndexedEXT; 
		extern PFNGLDRAWBUFFERSINDEXEDEXTPROC fnptr_ci_glDrawBuffersIndexedEXT; 
		extern PFNGLGETINTEGERI_VEXTPROC fnptr_ci_glGetIntegeri_vEXT; 
		extern PFNGLPRIMITIVEBOUNDINGBOXEXTPROC fnptr_ci_glPrimitiveBoundingBoxEXT; 
		extern PFNGLRASTERSAMPLESEXTPROC fnptr_ci_glRasterSamplesEXT; 
		extern PFNGLGETGRAPHICSRESETSTATUSEXTPROC fnptr_ci_glGetGraphicsResetStatusEXT; 
		extern PFNGLREADNPIXELSEXTPROC fnptr_ci_glReadnPixelsEXT; 
		extern PFNGLGETNUNIFORMFVEXTPROC fnptr_ci_glGetnUniformfvEXT; 
		extern PFNGLGETNUNIFORMIVEXTPROC fnptr_ci_glGetnUniformivEXT; 
		extern PFNGLACTIVESHADERPROGRAMEXTPROC fnptr_ci_glActiveShaderProgramEXT; 
		extern PFNGLBINDPROGRAMPIPELINEEXTPROC fnptr_ci_glBindProgramPipelineEXT; 
		extern PFNGLCREATESHADERPROGRAMVEXTPROC fnptr_ci_glCreateShaderProgramvEXT; 
		extern PFNGLDELETEPROGRAMPIPELINESEXTPROC fnptr_ci_glDeleteProgramPipelinesEXT; 
		extern PFNGLGENPROGRAMPIPELINESEXTPROC fnptr_ci_glGenProgramPipelinesEXT; 
		extern PFNGLGETPROGRAMPIPELINEINFOLOGEXTPROC fnptr_ci_glGetProgramPipelineInfoLogEXT; 
		extern PFNGLGETPROGRAMPIPELINEIVEXTPROC fnptr_ci_glGetProgramPipelineivEXT; 
		extern PFNGLISPROGRAMPIPELINEEXTPROC fnptr_ci_glIsProgramPipelineEXT; 
		extern PFNGLPROGRAMPARAMETERIEXTPROC fnptr_ci_glProgramParameteriEXT; 
		extern PFNGLPROGRAMUNIFORM1FEXTPROC fnptr_ci_glProgramUniform1fEXT; 
		extern PFNGLPROGRAMUNIFORM1FVEXTPROC fnptr_ci_glProgramUniform1fvEXT; 
		extern PFNGLPROGRAMUNIFORM1IEXTPROC fnptr_ci_glProgramUniform1iEXT; 
		extern PFNGLPROGRAMUNIFORM1IVEXTPROC fnptr_ci_glProgramUniform1ivEXT; 
		extern PFNGLPROGRAMUNIFORM2FEXTPROC fnptr_ci_glProgramUniform2fEXT; 
		extern PFNGLPROGRAMUNIFORM2FVEXTPROC fnptr_ci_glProgramUniform2fvEXT; 
		extern PFNGLPROGRAMUNIFORM2IEXTPROC fnptr_ci_glProgramUniform2iEXT; 
		extern PFNGLPROGRAMUNIFORM2IVEXTPROC fnptr_ci_glProgramUniform2ivEXT; 
		extern PFNGLPROGRAMUNIFORM3FEXTPROC fnptr_ci_glProgramUniform3fEXT; 
		extern PFNGLPROGRAMUNIFORM3FVEXTPROC fnptr_ci_glProgramUniform3fvEXT; 
		extern PFNGLPROGRAMUNIFORM3IEXTPROC fnptr_ci_glProgramUniform3iEXT; 
		extern PFNGLPROGRAMUNIFORM3IVEXTPROC fnptr_ci_glProgramUniform3ivEXT; 
		extern PFNGLPROGRAMUNIFORM4FEXTPROC fnptr_ci_glProgramUniform4fEXT; 
		extern PFNGLPROGRAMUNIFORM4FVEXTPROC fnptr_ci_glProgramUniform4fvEXT; 
		extern PFNGLPROGRAMUNIFORM4IEXTPROC fnptr_ci_glProgramUniform4iEXT; 
		extern PFNGLPROGRAMUNIFORM4IVEXTPROC fnptr_ci_glProgramUniform4ivEXT; 
		extern PFNGLPROGRAMUNIFORMMATRIX2FVEXTPROC fnptr_ci_glProgramUniformMatrix2fvEXT; 
		extern PFNGLPROGRAMUNIFORMMATRIX3FVEXTPROC fnptr_ci_glProgramUniformMatrix3fvEXT; 
		extern PFNGLPROGRAMUNIFORMMATRIX4FVEXTPROC fnptr_ci_glProgramUniformMatrix4fvEXT; 
		extern PFNGLUSEPROGRAMSTAGESEXTPROC fnptr_ci_glUseProgramStagesEXT; 
		extern PFNGLVALIDATEPROGRAMPIPELINEEXTPROC fnptr_ci_glValidateProgramPipelineEXT; 
		extern PFNGLPROGRAMUNIFORM1UIEXTPROC fnptr_ci_glProgramUniform1uiEXT; 
		extern PFNGLPROGRAMUNIFORM2UIEXTPROC fnptr_ci_glProgramUniform2uiEXT; 
		extern PFNGLPROGRAMUNIFORM3UIEXTPROC fnptr_ci_glProgramUniform3uiEXT; 
		extern PFNGLPROGRAMUNIFORM4UIEXTPROC fnptr_ci_glProgramUniform4uiEXT; 
		extern PFNGLPROGRAMUNIFORM1UIVEXTPROC fnptr_ci_glProgramUniform1uivEXT; 
		extern PFNGLPROGRAMUNIFORM2UIVEXTPROC fnptr_ci_glProgramUniform2uivEXT; 
		extern PFNGLPROGRAMUNIFORM3UIVEXTPROC fnptr_ci_glProgramUniform3uivEXT; 
		extern PFNGLPROGRAMUNIFORM4UIVEXTPROC fnptr_ci_glProgramUniform4uivEXT; 
		extern PFNGLPROGRAMUNIFORMMATRIX2X3FVEXTPROC fnptr_ci_glProgramUniformMatrix2x3fvEXT; 
		extern PFNGLPROGRAMUNIFORMMATRIX3X2FVEXTPROC fnptr_ci_glProgramUniformMatrix3x2fvEXT; 
		extern PFNGLPROGRAMUNIFORMMATRIX2X4FVEXTPROC fnptr_ci_glProgramUniformMatrix2x4fvEXT; 
		extern PFNGLPROGRAMUNIFORMMATRIX4X2FVEXTPROC fnptr_ci_glProgramUniformMatrix4x2fvEXT; 
		extern PFNGLPROGRAMUNIFORMMATRIX3X4FVEXTPROC fnptr_ci_glProgramUniformMatrix3x4fvEXT; 
		extern PFNGLPROGRAMUNIFORMMATRIX4X3FVEXTPROC fnptr_ci_glProgramUniformMatrix4x3fvEXT; 
		extern PFNGLTEXPAGECOMMITMENTEXTPROC fnptr_ci_glTexPageCommitmentEXT; 
		extern PFNGLTEXPARAMETERIIVEXTPROC fnptr_ci_glTexParameterIivEXT; 
		extern PFNGLTEXPARAMETERIUIVEXTPROC fnptr_ci_glTexParameterIuivEXT; 
		extern PFNGLGETTEXPARAMETERIIVEXTPROC fnptr_ci_glGetTexParameterIivEXT; 
		extern PFNGLGETTEXPARAMETERIUIVEXTPROC fnptr_ci_glGetTexParameterIuivEXT; 
		extern PFNGLSAMPLERPARAMETERIIVEXTPROC fnptr_ci_glSamplerParameterIivEXT; 
		extern PFNGLSAMPLERPARAMETERIUIVEXTPROC fnptr_ci_glSamplerParameterIuivEXT; 
		extern PFNGLGETSAMPLERPARAMETERIIVEXTPROC fnptr_ci_glGetSamplerParameterIivEXT; 
		extern PFNGLGETSAMPLERPARAMETERIUIVEXTPROC fnptr_ci_glGetSamplerParameterIuivEXT; 
		extern PFNGLTEXBUFFEREXTPROC fnptr_ci_glTexBufferEXT; 
		extern PFNGLTEXBUFFERRANGEEXTPROC fnptr_ci_glTexBufferRangeEXT; 
		extern PFNGLTEXSTORAGE1DEXTPROC fnptr_ci_glTexStorage1DEXT; 
		extern PFNGLTEXSTORAGE2DEXTPROC fnptr_ci_glTexStorage2DEXT; 
		extern PFNGLTEXSTORAGE3DEXTPROC fnptr_ci_glTexStorage3DEXT; 
		extern PFNGLTEXTURESTORAGE1DEXTPROC fnptr_ci_glTextureStorage1DEXT; 
		extern PFNGLTEXTURESTORAGE2DEXTPROC fnptr_ci_glTextureStorage2DEXT; 
		extern PFNGLTEXTURESTORAGE3DEXTPROC fnptr_ci_glTextureStorage3DEXT; 
		extern PFNGLTEXTUREVIEWEXTPROC fnptr_ci_glTextureViewEXT; 
		extern PFNGLFRAMEBUFFERTEXTURE2DMULTISAMPLEIMGPROC fnptr_ci_glFramebufferTexture2DMultisampleIMG; 
		extern PFNGLAPPLYFRAMEBUFFERATTACHMENTCMAAINTELPROC fnptr_ci_glApplyFramebufferAttachmentCMAAINTEL; 
		extern PFNGLBEGINPERFQUERYINTELPROC fnptr_ci_glBeginPerfQueryINTEL; 
		extern PFNGLCREATEPERFQUERYINTELPROC fnptr_ci_glCreatePerfQueryINTEL; 
		extern PFNGLDELETEPERFQUERYINTELPROC fnptr_ci_glDeletePerfQueryINTEL; 
		extern PFNGLENDPERFQUERYINTELPROC fnptr_ci_glEndPerfQueryINTEL; 
		extern PFNGLGETFIRSTPERFQUERYIDINTELPROC fnptr_ci_glGetFirstPerfQueryIdINTEL; 
		extern PFNGLGETNEXTPERFQUERYIDINTELPROC fnptr_ci_glGetNextPerfQueryIdINTEL; 
		extern PFNGLGETPERFCOUNTERINFOINTELPROC fnptr_ci_glGetPerfCounterInfoINTEL; 
		extern PFNGLGETPERFQUERYDATAINTELPROC fnptr_ci_glGetPerfQueryDataINTEL; 
		extern PFNGLGETPERFQUERYIDBYNAMEINTELPROC fnptr_ci_glGetPerfQueryIdByNameINTEL; 
		extern PFNGLGETPERFQUERYINFOINTELPROC fnptr_ci_glGetPerfQueryInfoINTEL; 
		extern PFNGLGETTEXTUREHANDLENVPROC fnptr_ci_glGetTextureHandleNV; 
		extern PFNGLGETTEXTURESAMPLERHANDLENVPROC fnptr_ci_glGetTextureSamplerHandleNV; 
		extern PFNGLMAKETEXTUREHANDLERESIDENTNVPROC fnptr_ci_glMakeTextureHandleResidentNV; 
		extern PFNGLMAKETEXTUREHANDLENONRESIDENTNVPROC fnptr_ci_glMakeTextureHandleNonResidentNV; 
		extern PFNGLGETIMAGEHANDLENVPROC fnptr_ci_glGetImageHandleNV; 
		extern PFNGLMAKEIMAGEHANDLERESIDENTNVPROC fnptr_ci_glMakeImageHandleResidentNV; 
		extern PFNGLMAKEIMAGEHANDLENONRESIDENTNVPROC fnptr_ci_glMakeImageHandleNonResidentNV; 
		extern PFNGLUNIFORMHANDLEUI64NVPROC fnptr_ci_glUniformHandleui64NV; 
		extern PFNGLUNIFORMHANDLEUI64VNVPROC fnptr_ci_glUniformHandleui64vNV; 
		extern PFNGLPROGRAMUNIFORMHANDLEUI64NVPROC fnptr_ci_glProgramUniformHandleui64NV; 
		extern PFNGLPROGRAMUNIFORMHANDLEUI64VNVPROC fnptr_ci_glProgramUniformHandleui64vNV; 
		extern PFNGLISTEXTUREHANDLERESIDENTNVPROC fnptr_ci_glIsTextureHandleResidentNV; 
		extern PFNGLISIMAGEHANDLERESIDENTNVPROC fnptr_ci_glIsImageHandleResidentNV; 
		extern PFNGLBLENDPARAMETERINVPROC fnptr_ci_glBlendParameteriNV; 
		extern PFNGLBLENDBARRIERNVPROC fnptr_ci_glBlendBarrierNV; 
		extern PFNGLBEGINCONDITIONALRENDERNVPROC fnptr_ci_glBeginConditionalRenderNV; 
		extern PFNGLENDCONDITIONALRENDERNVPROC fnptr_ci_glEndConditionalRenderNV; 
		extern PFNGLSUBPIXELPRECISIONBIASNVPROC fnptr_ci_glSubpixelPrecisionBiasNV; 
		extern PFNGLCOPYBUFFERSUBDATANVPROC fnptr_ci_glCopyBufferSubDataNV; 
		extern PFNGLCOVERAGEMASKNVPROC fnptr_ci_glCoverageMaskNV; 
		extern PFNGLCOVERAGEOPERATIONNVPROC fnptr_ci_glCoverageOperationNV; 
		extern PFNGLDRAWBUFFERSNVPROC fnptr_ci_glDrawBuffersNV; 
		extern PFNGLDELETEFENCESNVPROC fnptr_ci_glDeleteFencesNV; 
		extern PFNGLGENFENCESNVPROC fnptr_ci_glGenFencesNV; 
		extern PFNGLISFENCENVPROC fnptr_ci_glIsFenceNV; 
		extern PFNGLTESTFENCENVPROC fnptr_ci_glTestFenceNV; 
		extern PFNGLGETFENCEIVNVPROC fnptr_ci_glGetFenceivNV; 
		extern PFNGLFINISHFENCENVPROC fnptr_ci_glFinishFenceNV; 
		extern PFNGLSETFENCENVPROC fnptr_ci_glSetFenceNV; 
		extern PFNGLFRAGMENTCOVERAGECOLORNVPROC fnptr_ci_glFragmentCoverageColorNV; 
		extern PFNGLBLITFRAMEBUFFERNVPROC fnptr_ci_glBlitFramebufferNV; 
		extern PFNGLCOVERAGEMODULATIONTABLENVPROC fnptr_ci_glCoverageModulationTableNV; 
		extern PFNGLGETCOVERAGEMODULATIONTABLENVPROC fnptr_ci_glGetCoverageModulationTableNV; 
		extern PFNGLCOVERAGEMODULATIONNVPROC fnptr_ci_glCoverageModulationNV; 
		extern PFNGLGETINTERNALFORMATSAMPLEIVNVPROC fnptr_ci_glGetInternalformatSampleivNV; 
		extern PFNGLUNIFORMMATRIX2X3FVNVPROC fnptr_ci_glUniformMatrix2x3fvNV; 
		extern PFNGLUNIFORMMATRIX3X2FVNVPROC fnptr_ci_glUniformMatrix3x2fvNV; 
		extern PFNGLUNIFORMMATRIX2X4FVNVPROC fnptr_ci_glUniformMatrix2x4fvNV; 
		extern PFNGLUNIFORMMATRIX4X2FVNVPROC fnptr_ci_glUniformMatrix4x2fvNV; 
		extern PFNGLUNIFORMMATRIX3X4FVNVPROC fnptr_ci_glUniformMatrix3x4fvNV; 
		extern PFNGLUNIFORMMATRIX4X3FVNVPROC fnptr_ci_glUniformMatrix4x3fvNV; 
		extern PFNGLGENPATHSNVPROC fnptr_ci_glGenPathsNV; 
		extern PFNGLDELETEPATHSNVPROC fnptr_ci_glDeletePathsNV; 
		extern PFNGLISPATHNVPROC fnptr_ci_glIsPathNV; 
		extern PFNGLPATHCOMMANDSNVPROC fnptr_ci_glPathCommandsNV; 
		extern PFNGLPATHCOORDSNVPROC fnptr_ci_glPathCoordsNV; 
		extern PFNGLPATHSUBCOMMANDSNVPROC fnptr_ci_glPathSubCommandsNV; 
		extern PFNGLPATHSUBCOORDSNVPROC fnptr_ci_glPathSubCoordsNV; 
		extern PFNGLPATHSTRINGNVPROC fnptr_ci_glPathStringNV; 
		extern PFNGLPATHGLYPHSNVPROC fnptr_ci_glPathGlyphsNV; 
		extern PFNGLPATHGLYPHRANGENVPROC fnptr_ci_glPathGlyphRangeNV; 
		extern PFNGLWEIGHTPATHSNVPROC fnptr_ci_glWeightPathsNV; 
		extern PFNGLCOPYPATHNVPROC fnptr_ci_glCopyPathNV; 
		extern PFNGLINTERPOLATEPATHSNVPROC fnptr_ci_glInterpolatePathsNV; 
		extern PFNGLTRANSFORMPATHNVPROC fnptr_ci_glTransformPathNV; 
		extern PFNGLPATHPARAMETERIVNVPROC fnptr_ci_glPathParameterivNV; 
		extern PFNGLPATHPARAMETERINVPROC fnptr_ci_glPathParameteriNV; 
		extern PFNGLPATHPARAMETERFVNVPROC fnptr_ci_glPathParameterfvNV; 
		extern PFNGLPATHPARAMETERFNVPROC fnptr_ci_glPathParameterfNV; 
		extern PFNGLPATHDASHARRAYNVPROC fnptr_ci_glPathDashArrayNV; 
		extern PFNGLPATHSTENCILFUNCNVPROC fnptr_ci_glPathStencilFuncNV; 
		extern PFNGLPATHSTENCILDEPTHOFFSETNVPROC fnptr_ci_glPathStencilDepthOffsetNV; 
		extern PFNGLSTENCILFILLPATHNVPROC fnptr_ci_glStencilFillPathNV; 
		extern PFNGLSTENCILSTROKEPATHNVPROC fnptr_ci_glStencilStrokePathNV; 
		extern PFNGLSTENCILFILLPATHINSTANCEDNVPROC fnptr_ci_glStencilFillPathInstancedNV; 
		extern PFNGLSTENCILSTROKEPATHINSTANCEDNVPROC fnptr_ci_glStencilStrokePathInstancedNV; 
		extern PFNGLPATHCOVERDEPTHFUNCNVPROC fnptr_ci_glPathCoverDepthFuncNV; 
		extern PFNGLCOVERFILLPATHNVPROC fnptr_ci_glCoverFillPathNV; 
		extern PFNGLCOVERSTROKEPATHNVPROC fnptr_ci_glCoverStrokePathNV; 
		extern PFNGLCOVERFILLPATHINSTANCEDNVPROC fnptr_ci_glCoverFillPathInstancedNV; 
		extern PFNGLCOVERSTROKEPATHINSTANCEDNVPROC fnptr_ci_glCoverStrokePathInstancedNV; 
		extern PFNGLGETPATHPARAMETERIVNVPROC fnptr_ci_glGetPathParameterivNV; 
		extern PFNGLGETPATHPARAMETERFVNVPROC fnptr_ci_glGetPathParameterfvNV; 
		extern PFNGLGETPATHCOMMANDSNVPROC fnptr_ci_glGetPathCommandsNV; 
		extern PFNGLGETPATHCOORDSNVPROC fnptr_ci_glGetPathCoordsNV; 
		extern PFNGLGETPATHDASHARRAYNVPROC fnptr_ci_glGetPathDashArrayNV; 
		extern PFNGLGETPATHMETRICSNVPROC fnptr_ci_glGetPathMetricsNV; 
		extern PFNGLGETPATHMETRICRANGENVPROC fnptr_ci_glGetPathMetricRangeNV; 
		extern PFNGLGETPATHSPACINGNVPROC fnptr_ci_glGetPathSpacingNV; 
		extern PFNGLISPOINTINFILLPATHNVPROC fnptr_ci_glIsPointInFillPathNV; 
		extern PFNGLISPOINTINSTROKEPATHNVPROC fnptr_ci_glIsPointInStrokePathNV; 
		extern PFNGLGETPATHLENGTHNVPROC fnptr_ci_glGetPathLengthNV; 
		extern PFNGLPOINTALONGPATHNVPROC fnptr_ci_glPointAlongPathNV; 
		extern PFNGLMATRIXLOAD3X2FNVPROC fnptr_ci_glMatrixLoad3x2fNV; 
		extern PFNGLMATRIXLOAD3X3FNVPROC fnptr_ci_glMatrixLoad3x3fNV; 
		extern PFNGLMATRIXLOADTRANSPOSE3X3FNVPROC fnptr_ci_glMatrixLoadTranspose3x3fNV; 
		extern PFNGLMATRIXMULT3X2FNVPROC fnptr_ci_glMatrixMult3x2fNV; 
		extern PFNGLMATRIXMULT3X3FNVPROC fnptr_ci_glMatrixMult3x3fNV; 
		extern PFNGLMATRIXMULTTRANSPOSE3X3FNVPROC fnptr_ci_glMatrixMultTranspose3x3fNV; 
		extern PFNGLSTENCILTHENCOVERFILLPATHNVPROC fnptr_ci_glStencilThenCoverFillPathNV; 
		extern PFNGLSTENCILTHENCOVERSTROKEPATHNVPROC fnptr_ci_glStencilThenCoverStrokePathNV; 
		extern PFNGLSTENCILTHENCOVERFILLPATHINSTANCEDNVPROC fnptr_ci_glStencilThenCoverFillPathInstancedNV; 
		extern PFNGLSTENCILTHENCOVERSTROKEPATHINSTANCEDNVPROC fnptr_ci_glStencilThenCoverStrokePathInstancedNV; 
		extern PFNGLPATHGLYPHINDEXRANGENVPROC fnptr_ci_glPathGlyphIndexRangeNV; 
		extern PFNGLPATHGLYPHINDEXARRAYNVPROC fnptr_ci_glPathGlyphIndexArrayNV; 
		extern PFNGLPATHMEMORYGLYPHINDEXARRAYNVPROC fnptr_ci_glPathMemoryGlyphIndexArrayNV; 
		extern PFNGLPROGRAMPATHFRAGMENTINPUTGENNVPROC fnptr_ci_glProgramPathFragmentInputGenNV; 
		extern PFNGLGETPROGRAMRESOURCEFVNVPROC fnptr_ci_glGetProgramResourcefvNV; 
		extern PFNGLPOLYGONMODENVPROC fnptr_ci_glPolygonModeNV; 
		extern PFNGLREADBUFFERNVPROC fnptr_ci_glReadBufferNV; 
		extern PFNGLFRAMEBUFFERSAMPLELOCATIONSFVNVPROC fnptr_ci_glFramebufferSampleLocationsfvNV; 
		extern PFNGLNAMEDFRAMEBUFFERSAMPLELOCATIONSFVNVPROC fnptr_ci_glNamedFramebufferSampleLocationsfvNV; 
		extern PFNGLRESOLVEDEPTHVALUESNVPROC fnptr_ci_glResolveDepthValuesNV; 
		extern PFNGLVIEWPORTARRAYVNVPROC fnptr_ci_glViewportArrayvNV; 
		extern PFNGLVIEWPORTINDEXEDFNVPROC fnptr_ci_glViewportIndexedfNV; 
		extern PFNGLVIEWPORTINDEXEDFVNVPROC fnptr_ci_glViewportIndexedfvNV; 
		extern PFNGLSCISSORARRAYVNVPROC fnptr_ci_glScissorArrayvNV; 
		extern PFNGLSCISSORINDEXEDNVPROC fnptr_ci_glScissorIndexedNV; 
		extern PFNGLSCISSORINDEXEDVNVPROC fnptr_ci_glScissorIndexedvNV; 
		extern PFNGLDEPTHRANGEARRAYFVNVPROC fnptr_ci_glDepthRangeArrayfvNV; 
		extern PFNGLDEPTHRANGEINDEXEDFNVPROC fnptr_ci_glDepthRangeIndexedfNV; 
		extern PFNGLGETFLOATI_VNVPROC fnptr_ci_glGetFloati_vNV; 
		extern PFNGLENABLEINVPROC fnptr_ci_glEnableiNV; 
		extern PFNGLDISABLEINVPROC fnptr_ci_glDisableiNV; 
		extern PFNGLISENABLEDINVPROC fnptr_ci_glIsEnablediNV; 
		extern PFNGLFRAMEBUFFERTEXTUREMULTIVIEWOVRPROC fnptr_ci_glFramebufferTextureMultiviewOVR; 
		extern PFNGLALPHAFUNCQCOMPROC fnptr_ci_glAlphaFuncQCOM; 
		extern PFNGLGETDRIVERCONTROLSQCOMPROC fnptr_ci_glGetDriverControlsQCOM; 
		extern PFNGLGETDRIVERCONTROLSTRINGQCOMPROC fnptr_ci_glGetDriverControlStringQCOM; 
		extern PFNGLENABLEDRIVERCONTROLQCOMPROC fnptr_ci_glEnableDriverControlQCOM; 
		extern PFNGLDISABLEDRIVERCONTROLQCOMPROC fnptr_ci_glDisableDriverControlQCOM; 
		extern PFNGLEXTGETTEXTURESQCOMPROC fnptr_ci_glExtGetTexturesQCOM; 
		extern PFNGLEXTGETBUFFERSQCOMPROC fnptr_ci_glExtGetBuffersQCOM; 
		extern PFNGLEXTGETRENDERBUFFERSQCOMPROC fnptr_ci_glExtGetRenderbuffersQCOM; 
		extern PFNGLEXTGETFRAMEBUFFERSQCOMPROC fnptr_ci_glExtGetFramebuffersQCOM; 
		extern PFNGLEXTGETTEXLEVELPARAMETERIVQCOMPROC fnptr_ci_glExtGetTexLevelParameterivQCOM; 
		extern PFNGLEXTTEXOBJECTSTATEOVERRIDEIQCOMPROC fnptr_ci_glExtTexObjectStateOverrideiQCOM; 
		extern PFNGLEXTGETTEXSUBIMAGEQCOMPROC fnptr_ci_glExtGetTexSubImageQCOM; 
		extern PFNGLEXTGETBUFFERPOINTERVQCOMPROC fnptr_ci_glExtGetBufferPointervQCOM; 
		extern PFNGLEXTGETSHADERSQCOMPROC fnptr_ci_glExtGetShadersQCOM; 
		extern PFNGLEXTGETPROGRAMSQCOMPROC fnptr_ci_glExtGetProgramsQCOM; 
		extern PFNGLEXTISPROGRAMBINARYQCOMPROC fnptr_ci_glExtIsProgramBinaryQCOM; 
		extern PFNGLEXTGETPROGRAMBINARYSOURCEQCOMPROC fnptr_ci_glExtGetProgramBinarySourceQCOM; 
		extern PFNGLSTARTTILINGQCOMPROC fnptr_ci_glStartTilingQCOM; 
		extern PFNGLENDTILINGQCOMPROC fnptr_ci_glEndTilingQCOM; 

		#define glBlendBarrierKHR fnptr_ci_glBlendBarrierKHR 
		#define glDebugMessageControlKHR fnptr_ci_glDebugMessageControlKHR 
		#define glDebugMessageInsertKHR fnptr_ci_glDebugMessageInsertKHR 
		#define glDebugMessageCallbackKHR fnptr_ci_glDebugMessageCallbackKHR 
		#define glGetDebugMessageLogKHR fnptr_ci_glGetDebugMessageLogKHR 
		#define glPushDebugGroupKHR fnptr_ci_glPushDebugGroupKHR 
		#define glPopDebugGroupKHR fnptr_ci_glPopDebugGroupKHR 
		#define glObjectLabelKHR fnptr_ci_glObjectLabelKHR 
		#define glGetObjectLabelKHR fnptr_ci_glGetObjectLabelKHR 
		#define glObjectPtrLabelKHR fnptr_ci_glObjectPtrLabelKHR 
		#define glGetObjectPtrLabelKHR fnptr_ci_glGetObjectPtrLabelKHR 
		#define glGetPointervKHR fnptr_ci_glGetPointervKHR 
		#define glGetGraphicsResetStatusKHR fnptr_ci_glGetGraphicsResetStatusKHR 
		#define glReadnPixelsKHR fnptr_ci_glReadnPixelsKHR 
		#define glGetnUniformfvKHR fnptr_ci_glGetnUniformfvKHR 
		#define glGetnUniformivKHR fnptr_ci_glGetnUniformivKHR 
		#define glGetnUniformuivKHR fnptr_ci_glGetnUniformuivKHR 
		#define glEGLImageTargetTexture2DOES fnptr_ci_glEGLImageTargetTexture2DOES 
		#define glEGLImageTargetRenderbufferStorageOES fnptr_ci_glEGLImageTargetRenderbufferStorageOES 
		#define glCopyImageSubDataOES fnptr_ci_glCopyImageSubDataOES 
		#define glEnableiOES fnptr_ci_glEnableiOES 
		#define glDisableiOES fnptr_ci_glDisableiOES 
		#define glBlendEquationiOES fnptr_ci_glBlendEquationiOES 
		#define glBlendEquationSeparateiOES fnptr_ci_glBlendEquationSeparateiOES 
		#define glBlendFunciOES fnptr_ci_glBlendFunciOES 
		#define glBlendFuncSeparateiOES fnptr_ci_glBlendFuncSeparateiOES 
		#define glColorMaskiOES fnptr_ci_glColorMaskiOES 
		#define glIsEnablediOES fnptr_ci_glIsEnablediOES 
		#define glDrawElementsBaseVertexOES fnptr_ci_glDrawElementsBaseVertexOES 
		#define glDrawRangeElementsBaseVertexOES fnptr_ci_glDrawRangeElementsBaseVertexOES 
		#define glDrawElementsInstancedBaseVertexOES fnptr_ci_glDrawElementsInstancedBaseVertexOES 
		#define glMultiDrawElementsBaseVertexOES fnptr_ci_glMultiDrawElementsBaseVertexOES 
		#define glFramebufferTextureOES fnptr_ci_glFramebufferTextureOES 
		#define glGetProgramBinaryOES fnptr_ci_glGetProgramBinaryOES 
		#define glProgramBinaryOES fnptr_ci_glProgramBinaryOES 
		#define glGetBufferPointervOES fnptr_ci_glGetBufferPointervOES 
		#define glPrimitiveBoundingBoxOES fnptr_ci_glPrimitiveBoundingBoxOES 
		#define glMinSampleShadingOES fnptr_ci_glMinSampleShadingOES
		#define glTexImage3DOES fnptr_ci_glTexImage3DOES 
		#define glTexSubImage3DOES fnptr_ci_glTexSubImage3DOES 
		#define glCopyTexSubImage3DOES fnptr_ci_glCopyTexSubImage3DOES 
		#define glCompressedTexImage3DOES fnptr_ci_glCompressedTexImage3DOES 
		#define glCompressedTexSubImage3DOES fnptr_ci_glCompressedTexSubImage3DOES 
		#define glFramebufferTexture3DOES fnptr_ci_glFramebufferTexture3DOES 
		#define glTexParameterIivOES fnptr_ci_glTexParameterIivOES 
		#define glTexParameterIuivOES fnptr_ci_glTexParameterIuivOES 
		#define glGetTexParameterIivOES fnptr_ci_glGetTexParameterIivOES 
		#define glGetTexParameterIuivOES fnptr_ci_glGetTexParameterIuivOES 
		#define glSamplerParameterIivOES fnptr_ci_glSamplerParameterIivOES 
		#define glSamplerParameterIuivOES fnptr_ci_glSamplerParameterIuivOES 
		#define glGetSamplerParameterIivOES fnptr_ci_glGetSamplerParameterIivOES 
		#define glGetSamplerParameterIuivOES fnptr_ci_glGetSamplerParameterIuivOES 
		#define glTexBufferOES fnptr_ci_glTexBufferOES 
		#define glTexBufferRangeOES fnptr_ci_glTexBufferRangeOES 
		#define glTexStorage3DMultisampleOES fnptr_ci_glTexStorage3DMultisampleOES 
		#define glTextureViewOES fnptr_ci_glTextureViewOES 
		#define glGetPerfMonitorGroupsAMD fnptr_ci_glGetPerfMonitorGroupsAMD 
		#define glGetPerfMonitorCountersAMD fnptr_ci_glGetPerfMonitorCountersAMD 
		#define glGetPerfMonitorGroupStringAMD fnptr_ci_glGetPerfMonitorGroupStringAMD 
		#define glGetPerfMonitorCounterStringAMD fnptr_ci_glGetPerfMonitorCounterStringAMD 
		#define glGetPerfMonitorCounterInfoAMD fnptr_ci_glGetPerfMonitorCounterInfoAMD 
		#define glGenPerfMonitorsAMD fnptr_ci_glGenPerfMonitorsAMD 
		#define glDeletePerfMonitorsAMD fnptr_ci_glDeletePerfMonitorsAMD 
		#define glSelectPerfMonitorCountersAMD fnptr_ci_glSelectPerfMonitorCountersAMD 
		#define glBeginPerfMonitorAMD fnptr_ci_glBeginPerfMonitorAMD 
		#define glEndPerfMonitorAMD fnptr_ci_glEndPerfMonitorAMD 
		#define glGetPerfMonitorCounterDataAMD fnptr_ci_glGetPerfMonitorCounterDataAMD 
		#define glBlitFramebufferANGLE fnptr_ci_glBlitFramebufferANGLE 
		#define glDrawArraysInstancedANGLE fnptr_ci_glDrawArraysInstancedANGLE 
		#define glDrawElementsInstancedANGLE fnptr_ci_glDrawElementsInstancedANGLE 
		#define glGetTranslatedShaderSourceANGLE fnptr_ci_glGetTranslatedShaderSourceANGLE 
		#define glCopyTextureLevelsAPPLE fnptr_ci_glCopyTextureLevelsAPPLE 
		#define glResolveMultisampleFramebufferAPPLE fnptr_ci_glResolveMultisampleFramebufferAPPLE 
		#define glFenceSyncAPPLE fnptr_ci_glFenceSyncAPPLE 
		#define glIsSyncAPPLE fnptr_ci_glIsSyncAPPLE 
		#define glDeleteSyncAPPLE fnptr_ci_glDeleteSyncAPPLE 
		#define glClientWaitSyncAPPLE fnptr_ci_glClientWaitSyncAPPLE 
		#define glWaitSyncAPPLE fnptr_ci_glWaitSyncAPPLE 
		#define glGetInteger64vAPPLE fnptr_ci_glGetInteger64vAPPLE 
		#define glGetSyncivAPPLE fnptr_ci_glGetSyncivAPPLE 
		#define glDrawArraysInstancedBaseInstanceEXT fnptr_ci_glDrawArraysInstancedBaseInstanceEXT 
		#define glDrawElementsInstancedBaseInstanceEXT fnptr_ci_glDrawElementsInstancedBaseInstanceEXT 
		#define glDrawElementsInstancedBaseVertexBaseInstanceEXT fnptr_ci_glDrawElementsInstancedBaseVertexBaseInstanceEXT 
		#define glBindFragDataLocationIndexedEXT fnptr_ci_glBindFragDataLocationIndexedEXT 
		#define glBindFragDataLocationEXT fnptr_ci_glBindFragDataLocationEXT 
		#define glGetProgramResourceLocationIndexEXT fnptr_ci_glGetProgramResourceLocationIndexEXT 
		#define glGetFragDataIndexEXT fnptr_ci_glGetFragDataIndexEXT 
		#define glCopyImageSubDataEXT fnptr_ci_glCopyImageSubDataEXT 
		#define glLabelObjectEXT fnptr_ci_glLabelObjectEXT 
		#define glGetObjectLabelEXT fnptr_ci_glGetObjectLabelEXT 
		#define glInsertEventMarkerEXT fnptr_ci_glInsertEventMarkerEXT 
		#define glPushGroupMarkerEXT fnptr_ci_glPushGroupMarkerEXT 
		#define glPopGroupMarkerEXT fnptr_ci_glPopGroupMarkerEXT 
		#define glDiscardFramebufferEXT fnptr_ci_glDiscardFramebufferEXT 
		#define glGenQueriesEXT fnptr_ci_glGenQueriesEXT 
		#define glDeleteQueriesEXT fnptr_ci_glDeleteQueriesEXT 
		#define glIsQueryEXT fnptr_ci_glIsQueryEXT 
		#define glBeginQueryEXT fnptr_ci_glBeginQueryEXT 
		#define glEndQueryEXT fnptr_ci_glEndQueryEXT 
		#define glQueryCounterEXT fnptr_ci_glQueryCounterEXT 
		#define glGetQueryivEXT fnptr_ci_glGetQueryivEXT 
		#define glGetQueryObjectivEXT fnptr_ci_glGetQueryObjectivEXT 
		#define glGetQueryObjectuivEXT fnptr_ci_glGetQueryObjectuivEXT 
		#define glGetQueryObjecti64vEXT fnptr_ci_glGetQueryObjecti64vEXT 
		#define glGetQueryObjectui64vEXT fnptr_ci_glGetQueryObjectui64vEXT 
		#define glEnableiEXT fnptr_ci_glEnableiEXT 
		#define glDisableiEXT fnptr_ci_glDisableiEXT 
		#define glBlendEquationiEXT fnptr_ci_glBlendEquationiEXT 
		#define glBlendEquationSeparateiEXT fnptr_ci_glBlendEquationSeparateiEXT 
		#define glBlendFunciEXT fnptr_ci_glBlendFunciEXT 
		#define glBlendFuncSeparateiEXT fnptr_ci_glBlendFuncSeparateiEXT 
		#define glColorMaskiEXT fnptr_ci_glColorMaskiEXT 
		#define glIsEnablediEXT fnptr_ci_glIsEnablediEXT 
		#define glDrawElementsBaseVertexEXT fnptr_ci_glDrawElementsBaseVertexEXT 
		#define glDrawRangeElementsBaseVertexEXT fnptr_ci_glDrawRangeElementsBaseVertexEXT 
		#define glDrawElementsInstancedBaseVertexEXT fnptr_ci_glDrawElementsInstancedBaseVertexEXT 
		#define glMultiDrawElementsBaseVertexEXT fnptr_ci_glMultiDrawElementsBaseVertexEXT 
		#define glDrawArraysInstancedEXT fnptr_ci_glDrawArraysInstancedEXT 
		#define glDrawElementsInstancedEXT fnptr_ci_glDrawElementsInstancedEXT  
		#define glMapBufferRangeEXT fnptr_ci_glMapBufferRangeEXT 
		#define glMultiDrawArraysEXT fnptr_ci_glMultiDrawArraysEXT 
		#define glMultiDrawElementsEXT fnptr_ci_glMultiDrawElementsEXT 
		#define glMultiDrawArraysIndirectEXT fnptr_ci_glMultiDrawArraysIndirectEXT 
		#define glMultiDrawElementsIndirectEXT fnptr_ci_glMultiDrawElementsIndirectEXT 
		#define glFramebufferTexture2DMultisampleEXT fnptr_ci_glFramebufferTexture2DMultisampleEXT 
		#define glReadBufferIndexedEXT fnptr_ci_glReadBufferIndexedEXT 
		#define glDrawBuffersIndexedEXT fnptr_ci_glDrawBuffersIndexedEXT 
		#define glGetIntegeri_vEXT fnptr_ci_glGetIntegeri_vEXT 
		#define glPrimitiveBoundingBoxEXT fnptr_ci_glPrimitiveBoundingBoxEXT 
		#define glRasterSamplesEXT fnptr_ci_glRasterSamplesEXT 
		#define glGetGraphicsResetStatusEXT fnptr_ci_glGetGraphicsResetStatusEXT 
		#define glReadnPixelsEXT fnptr_ci_glReadnPixelsEXT 
		#define glGetnUniformfvEXT fnptr_ci_glGetnUniformfvEXT 
		#define glGetnUniformivEXT fnptr_ci_glGetnUniformivEXT 
		#define glActiveShaderProgramEXT fnptr_ci_glActiveShaderProgramEXT 
		#define glBindProgramPipelineEXT fnptr_ci_glBindProgramPipelineEXT 
		#define glCreateShaderProgramvEXT fnptr_ci_glCreateShaderProgramvEXT 
		#define glDeleteProgramPipelinesEXT fnptr_ci_glDeleteProgramPipelinesEXT 
		#define glGenProgramPipelinesEXT fnptr_ci_glGenProgramPipelinesEXT 
		#define glGetProgramPipelineInfoLogEXT fnptr_ci_glGetProgramPipelineInfoLogEXT 
		#define glGetProgramPipelineivEXT fnptr_ci_glGetProgramPipelineivEXT 
		#define glIsProgramPipelineEXT fnptr_ci_glIsProgramPipelineEXT 
		#define glProgramParameteriEXT fnptr_ci_glProgramParameteriEXT 
		#define glProgramUniform1fEXT fnptr_ci_glProgramUniform1fEXT 
		#define glProgramUniform1fvEXT fnptr_ci_glProgramUniform1fvEXT 
		#define glProgramUniform1iEXT fnptr_ci_glProgramUniform1iEXT 
		#define glProgramUniform1ivEXT fnptr_ci_glProgramUniform1ivEXT 
		#define glProgramUniform2fEXT fnptr_ci_glProgramUniform2fEXT 
		#define glProgramUniform2fvEXT fnptr_ci_glProgramUniform2fvEXT 
		#define glProgramUniform2iEXT fnptr_ci_glProgramUniform2iEXT 
		#define glProgramUniform2ivEXT fnptr_ci_glProgramUniform2ivEXT 
		#define glProgramUniform3fEXT fnptr_ci_glProgramUniform3fEXT 
		#define glProgramUniform3fvEXT fnptr_ci_glProgramUniform3fvEXT 
		#define glProgramUniform3iEXT fnptr_ci_glProgramUniform3iEXT 
		#define glProgramUniform3ivEXT fnptr_ci_glProgramUniform3ivEXT 
		#define glProgramUniform4fEXT fnptr_ci_glProgramUniform4fEXT 
		#define glProgramUniform4fvEXT fnptr_ci_glProgramUniform4fvEXT 
		#define glProgramUniform4iEXT fnptr_ci_glProgramUniform4iEXT 
		#define glProgramUniform4ivEXT fnptr_ci_glProgramUniform4ivEXT 
		#define glProgramUniformMatrix2fvEXT fnptr_ci_glProgramUniformMatrix2fvEXT 
		#define glProgramUniformMatrix3fvEXT fnptr_ci_glProgramUniformMatrix3fvEXT 
		#define glProgramUniformMatrix4fvEXT fnptr_ci_glProgramUniformMatrix4fvEXT 
		#define glUseProgramStagesEXT fnptr_ci_glUseProgramStagesEXT 
		#define glValidateProgramPipelineEXT fnptr_ci_glValidateProgramPipelineEXT 
		#define glProgramUniform1uiEXT fnptr_ci_glProgramUniform1uiEXT 
		#define glProgramUniform2uiEXT fnptr_ci_glProgramUniform2uiEXT 
		#define glProgramUniform3uiEXT fnptr_ci_glProgramUniform3uiEXT 
		#define glProgramUniform4uiEXT fnptr_ci_glProgramUniform4uiEXT 
		#define glProgramUniform1uivEXT fnptr_ci_glProgramUniform1uivEXT 
		#define glProgramUniform2uivEXT fnptr_ci_glProgramUniform2uivEXT 
		#define glProgramUniform3uivEXT fnptr_ci_glProgramUniform3uivEXT 
		#define glProgramUniform4uivEXT fnptr_ci_glProgramUniform4uivEXT 
		#define glProgramUniformMatrix2x3fvEXT fnptr_ci_glProgramUniformMatrix2x3fvEXT 
		#define glProgramUniformMatrix3x2fvEXT fnptr_ci_glProgramUniformMatrix3x2fvEXT 
		#define glProgramUniformMatrix2x4fvEXT fnptr_ci_glProgramUniformMatrix2x4fvEXT 
		#define glProgramUniformMatrix4x2fvEXT fnptr_ci_glProgramUniformMatrix4x2fvEXT 
		#define glProgramUniformMatrix3x4fvEXT fnptr_ci_glProgramUniformMatrix3x4fvEXT 
		#define glProgramUniformMatrix4x3fvEXT fnptr_ci_glProgramUniformMatrix4x3fvEXT 
		#define glTexPageCommitmentEXT fnptr_ci_glTexPageCommitmentEXT 
		#define glTexParameterIivEXT fnptr_ci_glTexParameterIivEXT 
		#define glTexParameterIuivEXT fnptr_ci_glTexParameterIuivEXT 
		#define glGetTexParameterIivEXT fnptr_ci_glGetTexParameterIivEXT 
		#define glGetTexParameterIuivEXT fnptr_ci_glGetTexParameterIuivEXT 
		#define glSamplerParameterIivEXT fnptr_ci_glSamplerParameterIivEXT 
		#define glSamplerParameterIuivEXT fnptr_ci_glSamplerParameterIuivEXT 
		#define glGetSamplerParameterIivEXT fnptr_ci_glGetSamplerParameterIivEXT 
		#define glGetSamplerParameterIuivEXT fnptr_ci_glGetSamplerParameterIuivEXT 
		#define glTexBufferEXT fnptr_ci_glTexBufferEXT 
		#define glTexBufferRangeEXT fnptr_ci_glTexBufferRangeEXT 
		#define glTexStorage1DEXT fnptr_ci_glTexStorage1DEXT 
		#define glTexStorage2DEXT fnptr_ci_glTexStorage2DEXT 
		#define glTexStorage3DEXT fnptr_ci_glTexStorage3DEXT 
		#define glTextureStorage1DEXT fnptr_ci_glTextureStorage1DEXT 
		#define glTextureStorage2DEXT fnptr_ci_glTextureStorage2DEXT 
		#define glTextureStorage3DEXT fnptr_ci_glTextureStorage3DEXT 
		#define glTextureViewEXT fnptr_ci_glTextureViewEXT 
		#define glFramebufferTexture2DMultisampleIMG fnptr_ci_glFramebufferTexture2DMultisampleIMG 
		#define glApplyFramebufferAttachmentCMAAINTEL fnptr_ci_glApplyFramebufferAttachmentCMAAINTEL 
		#define glBeginPerfQueryINTEL fnptr_ci_glBeginPerfQueryINTEL 
		#define glCreatePerfQueryINTEL fnptr_ci_glCreatePerfQueryINTEL 
		#define glDeletePerfQueryINTEL fnptr_ci_glDeletePerfQueryINTEL 
		#define glEndPerfQueryINTEL fnptr_ci_glEndPerfQueryINTEL 
		#define glGetFirstPerfQueryIdINTEL fnptr_ci_glGetFirstPerfQueryIdINTEL 
		#define glGetNextPerfQueryIdINTEL fnptr_ci_glGetNextPerfQueryIdINTEL 
		#define glGetPerfCounterInfoINTEL fnptr_ci_glGetPerfCounterInfoINTEL 
		#define glGetPerfQueryDataINTEL fnptr_ci_glGetPerfQueryDataINTEL 
		#define glGetPerfQueryIdByNameINTEL fnptr_ci_glGetPerfQueryIdByNameINTEL 
		#define glGetPerfQueryInfoINTEL fnptr_ci_glGetPerfQueryInfoINTEL 
		#define glGetTextureHandleNV fnptr_ci_glGetTextureHandleNV 
		#define glGetTextureSamplerHandleNV fnptr_ci_glGetTextureSamplerHandleNV 
		#define glMakeTextureHandleResidentNV fnptr_ci_glMakeTextureHandleResidentNV 
		#define glMakeTextureHandleNonResidentNV fnptr_ci_glMakeTextureHandleNonResidentNV 
		#define glGetImageHandleNV fnptr_ci_glGetImageHandleNV 
		#define glMakeImageHandleResidentNV fnptr_ci_glMakeImageHandleResidentNV 
		#define glMakeImageHandleNonResidentNV fnptr_ci_glMakeImageHandleNonResidentNV 
		#define glUniformHandleui64NV fnptr_ci_glUniformHandleui64NV 
		#define glUniformHandleui64vNV fnptr_ci_glUniformHandleui64vNV 
		#define glProgramUniformHandleui64NV fnptr_ci_glProgramUniformHandleui64NV 
		#define glProgramUniformHandleui64vNV fnptr_ci_glProgramUniformHandleui64vNV 
		#define glIsTextureHandleResidentNV fnptr_ci_glIsTextureHandleResidentNV 
		#define glIsImageHandleResidentNV fnptr_ci_glIsImageHandleResidentNV 
		#define glBlendParameteriNV fnptr_ci_glBlendParameteriNV 
		#define glBlendBarrierNV fnptr_ci_glBlendBarrierNV 
		#define glBeginConditionalRenderNV fnptr_ci_glBeginConditionalRenderNV 
		#define glEndConditionalRenderNV fnptr_ci_glEndConditionalRenderNV 
		#define glSubpixelPrecisionBiasNV fnptr_ci_glSubpixelPrecisionBiasNV 
		#define glCopyBufferSubDataNV fnptr_ci_glCopyBufferSubDataNV 
		#define glCoverageMaskNV fnptr_ci_glCoverageMaskNV 
		#define glCoverageOperationNV fnptr_ci_glCoverageOperationNV 
		#define glDrawBuffersNV fnptr_ci_glDrawBuffersNV 
		#define glDrawArraysInstancedNV fnptr_ci_glDrawArraysInstancedNV 
		#define glDrawElementsInstancedNV fnptr_ci_glDrawElementsInstancedNV 
		#define glDeleteFencesNV fnptr_ci_glDeleteFencesNV 
		#define glGenFencesNV fnptr_ci_glGenFencesNV 
		#define glIsFenceNV fnptr_ci_glIsFenceNV 
		#define glTestFenceNV fnptr_ci_glTestFenceNV 
		#define glGetFenceivNV fnptr_ci_glGetFenceivNV 
		#define glFinishFenceNV fnptr_ci_glFinishFenceNV 
		#define glSetFenceNV fnptr_ci_glSetFenceNV 
		#define glFragmentCoverageColorNV fnptr_ci_glFragmentCoverageColorNV 
		#define glBlitFramebufferNV fnptr_ci_glBlitFramebufferNV 
		#define glCoverageModulationTableNV fnptr_ci_glCoverageModulationTableNV 
		#define glGetCoverageModulationTableNV fnptr_ci_glGetCoverageModulationTableNV 
		#define glCoverageModulationNV fnptr_ci_glCoverageModulationNV 
		#define glGetInternalformatSampleivNV fnptr_ci_glGetInternalformatSampleivNV 
		#define glUniformMatrix2x3fvNV fnptr_ci_glUniformMatrix2x3fvNV 
		#define glUniformMatrix3x2fvNV fnptr_ci_glUniformMatrix3x2fvNV 
		#define glUniformMatrix2x4fvNV fnptr_ci_glUniformMatrix2x4fvNV 
		#define glUniformMatrix4x2fvNV fnptr_ci_glUniformMatrix4x2fvNV 
		#define glUniformMatrix3x4fvNV fnptr_ci_glUniformMatrix3x4fvNV 
		#define glUniformMatrix4x3fvNV fnptr_ci_glUniformMatrix4x3fvNV 
		#define glGenPathsNV fnptr_ci_glGenPathsNV 
		#define glDeletePathsNV fnptr_ci_glDeletePathsNV 
		#define glIsPathNV fnptr_ci_glIsPathNV 
		#define glPathCommandsNV fnptr_ci_glPathCommandsNV 
		#define glPathCoordsNV fnptr_ci_glPathCoordsNV 
		#define glPathSubCommandsNV fnptr_ci_glPathSubCommandsNV 
		#define glPathSubCoordsNV fnptr_ci_glPathSubCoordsNV 
		#define glPathStringNV fnptr_ci_glPathStringNV 
		#define glPathGlyphsNV fnptr_ci_glPathGlyphsNV 
		#define glPathGlyphRangeNV fnptr_ci_glPathGlyphRangeNV 
		#define glWeightPathsNV fnptr_ci_glWeightPathsNV 
		#define glCopyPathNV fnptr_ci_glCopyPathNV 
		#define glInterpolatePathsNV fnptr_ci_glInterpolatePathsNV 
		#define glTransformPathNV fnptr_ci_glTransformPathNV 
		#define glPathParameterivNV fnptr_ci_glPathParameterivNV 
		#define glPathParameteriNV fnptr_ci_glPathParameteriNV 
		#define glPathParameterfvNV fnptr_ci_glPathParameterfvNV 
		#define glPathParameterfNV fnptr_ci_glPathParameterfNV 
		#define glPathDashArrayNV fnptr_ci_glPathDashArrayNV 
		#define glPathStencilFuncNV fnptr_ci_glPathStencilFuncNV 
		#define glPathStencilDepthOffsetNV fnptr_ci_glPathStencilDepthOffsetNV 
		#define glStencilFillPathNV fnptr_ci_glStencilFillPathNV 
		#define glStencilStrokePathNV fnptr_ci_glStencilStrokePathNV 
		#define glStencilFillPathInstancedNV fnptr_ci_glStencilFillPathInstancedNV 
		#define glStencilStrokePathInstancedNV fnptr_ci_glStencilStrokePathInstancedNV 
		#define glPathCoverDepthFuncNV fnptr_ci_glPathCoverDepthFuncNV 
		#define glCoverFillPathNV fnptr_ci_glCoverFillPathNV 
		#define glCoverStrokePathNV fnptr_ci_glCoverStrokePathNV 
		#define glCoverFillPathInstancedNV fnptr_ci_glCoverFillPathInstancedNV 
		#define glCoverStrokePathInstancedNV fnptr_ci_glCoverStrokePathInstancedNV 
		#define glGetPathParameterivNV fnptr_ci_glGetPathParameterivNV 
		#define glGetPathParameterfvNV fnptr_ci_glGetPathParameterfvNV 
		#define glGetPathCommandsNV fnptr_ci_glGetPathCommandsNV 
		#define glGetPathCoordsNV fnptr_ci_glGetPathCoordsNV 
		#define glGetPathDashArrayNV fnptr_ci_glGetPathDashArrayNV 
		#define glGetPathMetricsNV fnptr_ci_glGetPathMetricsNV 
		#define glGetPathMetricRangeNV fnptr_ci_glGetPathMetricRangeNV 
		#define glGetPathSpacingNV fnptr_ci_glGetPathSpacingNV 
		#define glIsPointInFillPathNV fnptr_ci_glIsPointInFillPathNV 
		#define glIsPointInStrokePathNV fnptr_ci_glIsPointInStrokePathNV 
		#define glGetPathLengthNV fnptr_ci_glGetPathLengthNV 
		#define glPointAlongPathNV fnptr_ci_glPointAlongPathNV 
		#define glMatrixLoad3x2fNV fnptr_ci_glMatrixLoad3x2fNV 
		#define glMatrixLoad3x3fNV fnptr_ci_glMatrixLoad3x3fNV 
		#define glMatrixLoadTranspose3x3fNV fnptr_ci_glMatrixLoadTranspose3x3fNV 
		#define glMatrixMult3x2fNV fnptr_ci_glMatrixMult3x2fNV 
		#define glMatrixMult3x3fNV fnptr_ci_glMatrixMult3x3fNV 
		#define glMatrixMultTranspose3x3fNV fnptr_ci_glMatrixMultTranspose3x3fNV 
		#define glStencilThenCoverFillPathNV fnptr_ci_glStencilThenCoverFillPathNV 
		#define glStencilThenCoverStrokePathNV fnptr_ci_glStencilThenCoverStrokePathNV 
		#define glStencilThenCoverFillPathInstancedNV fnptr_ci_glStencilThenCoverFillPathInstancedNV 
		#define glStencilThenCoverStrokePathInstancedNV fnptr_ci_glStencilThenCoverStrokePathInstancedNV 
		#define glPathGlyphIndexRangeNV fnptr_ci_glPathGlyphIndexRangeNV 
		#define glPathGlyphIndexArrayNV fnptr_ci_glPathGlyphIndexArrayNV 
		#define glPathMemoryGlyphIndexArrayNV fnptr_ci_glPathMemoryGlyphIndexArrayNV 
		#define glProgramPathFragmentInputGenNV fnptr_ci_glProgramPathFragmentInputGenNV 
		#define glGetProgramResourcefvNV fnptr_ci_glGetProgramResourcefvNV 
		#define glPolygonModeNV fnptr_ci_glPolygonModeNV 
		#define glFramebufferSampleLocationsfvNV fnptr_ci_glFramebufferSampleLocationsfvNV 
		#define glNamedFramebufferSampleLocationsfvNV fnptr_ci_glNamedFramebufferSampleLocationsfvNV 
		#define glResolveDepthValuesNV fnptr_ci_glResolveDepthValuesNV 
		#define glViewportArrayvNV fnptr_ci_glViewportArrayvNV 
		#define glViewportIndexedfNV fnptr_ci_glViewportIndexedfNV 
		#define glViewportIndexedfvNV fnptr_ci_glViewportIndexedfvNV 
		#define glScissorArrayvNV fnptr_ci_glScissorArrayvNV 
		#define glScissorIndexedNV fnptr_ci_glScissorIndexedNV 
		#define glScissorIndexedvNV fnptr_ci_glScissorIndexedvNV 
		#define glDepthRangeArrayfvNV fnptr_ci_glDepthRangeArrayfvNV 
		#define glDepthRangeIndexedfNV fnptr_ci_glDepthRangeIndexedfNV 
		#define glGetFloati_vNV fnptr_ci_glGetFloati_vNV 
		#define glEnableiNV fnptr_ci_glEnableiNV 
		#define glDisableiNV fnptr_ci_glDisableiNV 
		#define glIsEnablediNV fnptr_ci_glIsEnablediNV 
		#define glFramebufferTextureMultiviewOVR fnptr_ci_glFramebufferTextureMultiviewOVR 
		#define glAlphaFuncQCOM fnptr_ci_glAlphaFuncQCOM 
		#define glGetDriverControlsQCOM fnptr_ci_glGetDriverControlsQCOM 
		#define glGetDriverControlStringQCOM fnptr_ci_glGetDriverControlStringQCOM 
		#define glEnableDriverControlQCOM fnptr_ci_glEnableDriverControlQCOM 
		#define glDisableDriverControlQCOM fnptr_ci_glDisableDriverControlQCOM 
		#define glExtGetTexturesQCOM fnptr_ci_glExtGetTexturesQCOM 
		#define glExtGetBuffersQCOM fnptr_ci_glExtGetBuffersQCOM 
		#define glExtGetRenderbuffersQCOM fnptr_ci_glExtGetRenderbuffersQCOM 
		#define glExtGetFramebuffersQCOM fnptr_ci_glExtGetFramebuffersQCOM 
		#define glExtGetTexLevelParameterivQCOM fnptr_ci_glExtGetTexLevelParameterivQCOM 
		#define glExtTexObjectStateOverrideiQCOM fnptr_ci_glExtTexObjectStateOverrideiQCOM 
		#define glExtGetTexSubImageQCOM fnptr_ci_glExtGetTexSubImageQCOM 
		#define glExtGetBufferPointervQCOM fnptr_ci_glExtGetBufferPointervQCOM 
		#define glExtGetShadersQCOM fnptr_ci_glExtGetShadersQCOM 
		#define glExtGetProgramsQCOM fnptr_ci_glExtGetProgramsQCOM 
		#define glExtIsProgramBinaryQCOM fnptr_ci_glExtIsProgramBinaryQCOM 
		#define glExtGetProgramBinarySourceQCOM fnptr_ci_glExtGetProgramBinarySourceQCOM 
		#define glStartTilingQCOM fnptr_ci_glStartTilingQCOM 
		#define glEndTilingQCOM fnptr_ci_glEndTilingQCOM 

		#if ( CINDER_GL_ES_VERSION == CINDER_GL_ES_VERSION_2 )
	        // GL_OES_required_internalformat
	        #define GL_ALPHA8                                   GL_ALPHA8_OES
	        //#define GL_DEPTH_COMPONENT16                        GL_DEPTH_COMPONENT16_OES
	        #define GL_LUMINANCE4_ALPHA4                        GL_LUMINANCE4_ALPHA4_OES
	        #define GL_LUMINANCE8_ALPHA8                        GL_LUMINANCE8_ALPHA8_OES
	        #define GL_LUMINANCE8                               GL_LUMINANCE8_OES
	        //#define GL_RGBA4                                    GL_RGBA4_OES
	        //#define GL_RGB5_A1                                  GL_RGB5_A1_OES
	        //#define GL_RGB565                                   GL_RGB565_OES
	        #define GL_RGB8                                     GL_RGB8_OES
	        #define GL_RGBA8                                    GL_RGBA8_OES
	        #define GL_RGB10                                    GL_RGB10_EXT
			#define GL_RGB10_A2                  				GL_RGB10_A2_EXT

	        // GL_EXT_texture_rg
	        #define GL_RED                                      GL_RED_EXT
	        #define GL_RG                                       GL_RG_EXT
	        #define GL_R8                                       GL_R8_EXT
	        #define GL_RG8                                      GL_RG8_EXT

	        // GL_OES_texture_half_float
	        #define GL_HALF_FLOAT                               GL_HALF_FLOAT_OES

			// GL_EXT_color_buffer_half_float
			#define GL_RGBA16F									GL_RGBA16F_EXT
			#define GL_RGB16F									GL_RGB16F_EXT
			#define GL_RG16F									GL_RG16F_EXT
			#define GL_R16F										GL_R16F_EXT
			#define GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE	GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE_EXT
			#define GL_UNSIGNED_NORMALIZED						GL_UNSIGNED_NORMALIZED_EXT

			// GL_EXT_texture_storage
			#define GL_RGBA32F									GL_RGBA32F_EXT
			#define GL_RGB32F									GL_RGB32F_EXT
			#define GL_RG32F									GL_RG32F_EXT
			#define GL_R32F										GL_R32F_EXT

			// GL_APPLE_texture_packed_float
			#define GL_UNSIGNED_INT_10F_11F_11F_REV 			GL_UNSIGNED_INT_10F_11F_11F_REV_APPLE
			#define GL_UNSIGNED_INT_5_9_9_9_REV 				GL_UNSIGNED_INT_5_9_9_9_REV_APPLE
			#define GL_R11F_G11F_B10F           				GL_R11F_G11F_B10F_APPLE
			#define GL_RGB9_E5                  				GL_RGB9_E5_APPLE

	        // GL_OES_depth24
	        #define GL_DEPTH_COMPONENT24 						GL_DEPTH_COMPONENT24_OES

	        // GL_OES_mapbuffer
	        #define GL_WRITE_ONLY 								GL_WRITE_ONLY_OES
	    	extern PFNGLMAPBUFFEROESPROC 						fnptr_ci_glMapBufferOES;
			extern PFNGLUNMAPBUFFEROESPROC 						fnptr_ci_glUnmapBufferOES; 
	    	#define glMapBuffer 								fnptr_ci_glMapBufferOES
	        #define glUnmapBuffer 								fnptr_ci_glUnmapBufferOES 

			// GL_OES_vertex_array_object
			extern PFNGLBINDVERTEXARRAYOESPROC 					fnptr_ci_glBindVertexArrayOES; 
			extern PFNGLDELETEVERTEXARRAYSOESPROC 				fnptr_ci_glDeleteVertexArraysOES; 
			extern PFNGLGENVERTEXARRAYSOESPROC 					fnptr_ci_glGenVertexArraysOES; 
			extern PFNGLISVERTEXARRAYOESPROC 					fnptr_ci_glIsVertexArrayOES; 
			#define glBindVertexArray 							fnptr_ci_glBindVertexArrayOES 
			#define glDeleteVertexArrays 						fnptr_ci_glDeleteVertexArraysOES 
			#define glGenVertexArrays 							fnptr_ci_glGenVertexArraysOES 
			#define glIsVertexArray 							fnptr_ci_glIsVertexArrayOES 

			// GL_EXT_map_buffer_range
			#if ! defined( GL_MAP_READ_BIT )
				#define GL_MAP_READ_BIT 						GL_MAP_READ_BIT_EXT
				#define GL_MAP_WRITE_BIT 						GL_MAP_WRITE_BIT_EXT
			#endif
			#define GL_MAP_INVALIDATE_RANGE_BIT					GL_MAP_INVALIDATE_RANGE_BIT_EXT
			#define GL_MAP_INVALIDATE_BUFFER_BIT				GL_MAP_INVALIDATE_BUFFER_BIT_EXT
			#define GL_MAP_FLUSH_EXPLICIT_BIT					GL_MAP_FLUSH_EXPLICIT_BIT_EXT
			#define GL_MAP_UNSYNCHRONIZED_BIT					GL_MAP_UNSYNCHRONIZED_BIT_EXT
			extern PFNGLMAPBUFFERRANGEEXTPROC 					fnptr_ci_glMapBufferRangeEXT; 
			extern PFNGLFLUSHMAPPEDBUFFERRANGEEXTPROC 			fnptr_ci_glFlushMappedBufferRangeEXT; 
			#define glMapBufferRange 							fnptr_ci_glMapBufferRangeEXT 
			#define glFlushMappedBufferRangeEXT 				fnptr_ci_glFlushMappedBufferRangeEXT 

			// GL_EXT_multisampled_render_to_texture and the like
			//
			// fnptr_ci_glRenderbufferStorageMultisample maps to one of the following:
			//    glRenderbufferStorageMultisampleANGLE
			//    glRenderbufferStorageMultisampleAPPLE
			//    glRenderbufferStorageMultisampleEXT
			//    glRenderbufferStorageMultisampleIMG
			//    glRenderbufferStorageMultisampleNV
			//
			#define GL_READ_FRAMEBUFFER         				0x8CA8
			#define GL_DRAW_FRAMEBUFFER         				0x8CA9	
			#define GL_DRAW_FRAMEBUFFER_BINDING 				0x8CA6
			#define GL_READ_FRAMEBUFFER_BINDING 				0x8CAA
			typedef void (GL_APIENTRYP PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC) (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
			extern PFNGLRENDERBUFFERSTORAGEMULTISAMPLEPROC 		fnptr_ci_glRenderbufferStorageMultisample;
			#define glRenderbufferStorageMultisample 			fnptr_ci_glRenderbufferStorageMultisample 

			// GL_NV_framebuffer_blit, GL_ANGLE_framebuffer_blit
			typedef void (GL_APIENTRYP PFNGLBLITFRAMEBUFFERPROC) (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter);
			extern PFNGLBLITFRAMEBUFFERPROC 					fnptr_ci_glBlitFramebuffer;
			#define glBlitFramebuffer 							fnptr_ci_glBlitFramebuffer

			// GL_EXT_draw_buffers
			#define glDrawBuffers 								fnptr_ci_glDrawBuffersEXT

			// GL_NV_read_buffer
			#define glReadBuffer 								fnptr_ci_glReadBufferNV

			// GL_ANGLE_instanced_arrays, GL_EXT_draw_instanced, GL_NV_draw_instanced
			typedef void (GL_APIENTRYP PFNGLDRAWARRAYSINSTANCEDPROC) (GLenum mode, GLint first, GLsizei count, GLsizei primcount);
			typedef void (GL_APIENTRYP PFNGLDRAWELEMENTSINSTANCEDPROC) (GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei primcount);
			extern PFNGLDRAWARRAYSINSTANCEDPROC 				fnptr_ci_glDrawArraysInstanced; 
			extern PFNGLDRAWELEMENTSINSTANCEDPROC 				fnptr_ci_glDrawElementsInstanced; 
			#define glDrawArraysInstanced						fnptr_ci_glDrawArraysInstanced
			#define glDrawElementsInstanced 					fnptr_ci_glDrawElementsInstanced

			// GL_ANGLE_instanced_arrays, GL_EXT_instanced_arrays, GL_NV_instanced_arrays
			typedef void (GL_APIENTRYP PFNGLVERTEXATTRIBDIVISORPROC) (GLuint index, GLuint divisor);
			extern PFNGLVERTEXATTRIBDIVISORPROC 				fnptr_ci_glVertexAttribDivisor;
			#define glVertexAttribDivisor 						fnptr_ci_glVertexAttribDivisor

			// GL_EXT_shadow_samplers
			#define GL_TEXTURE_COMPARE_MODE						0x884C
			#define GL_TEXTURE_COMPARE_FUNC						0x884D
			#define GL_COMPARE_REF_TO_TEXTURE					0x884E
			#define GL_SAMPLER_2D_SHADOW						0x8B62
		#endif	
	#endif // ( CINDER_GL_ES_VERSION == CINDER_GL_ES_VERSION_2 )

	#if ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_2 )
		// GL_EXT_render_snorm, GL_EXT_texture_norm16 (GL_RGB16_SNORM_EXT)
		#define GL_R16_SNORM 									GL_R16_SNORM_EXT
		#define GL_RG16_SNORM 									GL_RG16_SNORM_EXT
		#define GL_RGB16_SNORM 									GL_RGB16_SNORM_EXT
		#define GL_RGBA16_SNORM 								GL_RGBA16_SNORM_EXT

		// GL_EXT_texture_norm16
		#define GL_R16 											GL_R16_EXT
		#define GL_RG16 										GL_RG16_EXT
		#define GL_RGB16 										GL_RGB16_EXT	
		#define GL_RGBA16 										GL_RGBA16_EXT

		// GL_EXT_buffer_storage
		typedef void (GL_APIENTRYP PFNGLBUFFERSTORAGEPROC) (GLenum target, GLsizeiptr size, const void *data, GLbitfield flags);
		extern PFNGLBUFFERSTORAGEPROC 							fnptr_ci_glBufferStorage;
		#define glBufferStorage									fnptr_ci_glBufferStorage
	#endif

	#if ( CINDER_GL_ES_VERSION >= CINDER_GL_ES_VERSION_3 )
		// GL_OES_required_internalformat
	   	#define GL_RGB10                                    	GL_RGB10_EXT
	#endif

	// Android Extension Pack
	#if ( CINDER_GL_ES_VERSION == CINDER_GL_ES_VERSION_3_1 )
	    #define GL_GEOMETRY_SHADER GL_GEOMETRY_SHADER_EXT            
	    #define GL_GEOMETRY_SHADER_BIT GL_GEOMETRY_SHADER_BIT_EXT        
	    #define GL_GEOMETRY_LINKED_VERTICES_OUT GL_GEOMETRY_LINKED_VERTICES_OUT_EXT 
	    #define GL_GEOMETRY_LINKED_INPUT_TYPE GL_GEOMETRY_LINKED_INPUT_TYPE_EXT 
	    #define GL_GEOMETRY_LINKED_OUTPUT_TYPE GL_GEOMETRY_LINKED_OUTPUT_TYPE_EXT 
	    #define GL_GEOMETRY_SHADER_INVOCATIONS GL_GEOMETRY_SHADER_INVOCATIONS_EXT 
	    #define GL_LAYER_PROVOKING_VERTEX GL_LAYER_PROVOKING_VERTEX_EXT     
	    #define GL_LINES_ADJACENCY GL_LINES_ADJACENCY_EXT            
	    #define GL_LINE_STRIP_ADJACENCY GL_LINE_STRIP_ADJACENCY_EXT       
	    #define GL_TRIANGLES_ADJACENCY GL_TRIANGLES_ADJACENCY_EXT        
	    #define GL_TRIANGLE_STRIP_ADJACENCY GL_TRIANGLE_STRIP_ADJACENCY_EXT   
	    #define GL_MAX_GEOMETRY_UNIFORM_COMPONENTS GL_MAX_GEOMETRY_UNIFORM_COMPONENTS_EXT 
	    #define GL_MAX_GEOMETRY_UNIFORM_BLOCKS GL_MAX_GEOMETRY_UNIFORM_BLOCKS_EXT 
	    #define GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS_EXT 
	    #define GL_MAX_GEOMETRY_INPUT_COMPONENTS GL_MAX_GEOMETRY_INPUT_COMPONENTS_EXT 
	    #define GL_MAX_GEOMETRY_OUTPUT_COMPONENTS GL_MAX_GEOMETRY_OUTPUT_COMPONENTS_EXT 
	    #define GL_MAX_GEOMETRY_OUTPUT_VERTICES GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT 
	    #define GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS_EXT 
	    #define GL_MAX_GEOMETRY_SHADER_INVOCATIONS GL_MAX_GEOMETRY_SHADER_INVOCATIONS_EXT 
	    #define GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS_EXT 
	    #define GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS GL_MAX_GEOMETRY_ATOMIC_COUNTER_BUFFERS_EXT 
	    #define GL_MAX_GEOMETRY_ATOMIC_COUNTERS GL_MAX_GEOMETRY_ATOMIC_COUNTERS_EXT 
	    #define GL_MAX_GEOMETRY_IMAGE_UNIFORMS GL_MAX_GEOMETRY_IMAGE_UNIFORMS_EXT 
	    #define GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS_EXT 
	    #define GL_FIRST_VERTEX_CONVENTION GL_FIRST_VERTEX_CONVENTION_EXT    
	    #define GL_LAST_VERTEX_CONVENTION GL_LAST_VERTEX_CONVENTION_EXT     
	    #define GL_UNDEFINED_VERTEX GL_UNDEFINED_VERTEX_EXT           
	    #define GL_PRIMITIVES_GENERATED GL_PRIMITIVES_GENERATED_EXT       
	    #define GL_FRAMEBUFFER_DEFAULT_LAYERS GL_FRAMEBUFFER_DEFAULT_LAYERS_EXT 
	    #define GL_MAX_FRAMEBUFFER_LAYERS GL_MAX_FRAMEBUFFER_LAYERS_EXT     
	    #define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS_EXT 
	    #define GL_FRAMEBUFFER_ATTACHMENT_LAYERED GL_FRAMEBUFFER_ATTACHMENT_LAYERED_EXT 
	    #define GL_REFERENCED_BY_GEOMETRY_SHADER GL_REFERENCED_BY_GEOMETRY_SHADER_EXT 

	    #define GL_PATCHES GL_PATCHES_EXT                    
	    #define GL_PATCH_VERTICES GL_PATCH_VERTICES_EXT             
	    #define GL_TESS_CONTROL_OUTPUT_VERTICES GL_TESS_CONTROL_OUTPUT_VERTICES_EXT 
	    #define GL_TESS_GEN_MODE GL_TESS_GEN_MODE_EXT              
	    #define GL_TESS_GEN_SPACING GL_TESS_GEN_SPACING_EXT           
	    #define GL_TESS_GEN_VERTEX_ORDER GL_TESS_GEN_VERTEX_ORDER_EXT      
	    #define GL_TESS_GEN_POINT_MODE GL_TESS_GEN_POINT_MODE_EXT        
	    #define GL_ISOLINES GL_ISOLINES_EXT                   
	    #define GL_QUADS GL_QUADS_EXT                      
	    #define GL_FRACTIONAL_ODD GL_FRACTIONAL_ODD_EXT             
	    #define GL_FRACTIONAL_EVEN GL_FRACTIONAL_EVEN_EXT            
	    #define GL_MAX_PATCH_VERTICES GL_MAX_PATCH_VERTICES_EXT         
	    #define GL_MAX_TESS_GEN_LEVEL GL_MAX_TESS_GEN_LEVEL_EXT         
	    #define GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS_EXT 
	    #define GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS_EXT 
	    #define GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS_EXT 
	    #define GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS_EXT 
	    #define GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS GL_MAX_TESS_CONTROL_OUTPUT_COMPONENTS_EXT 
	    #define GL_MAX_TESS_PATCH_COMPONENTS GL_MAX_TESS_PATCH_COMPONENTS_EXT  
	    #define GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS GL_MAX_TESS_CONTROL_TOTAL_OUTPUT_COMPONENTS_EXT 
	    #define GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS GL_MAX_TESS_EVALUATION_OUTPUT_COMPONENTS_EXT 
	    #define GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS GL_MAX_TESS_CONTROL_UNIFORM_BLOCKS_EXT 
	    #define GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS GL_MAX_TESS_EVALUATION_UNIFORM_BLOCKS_EXT 
	    #define GL_MAX_TESS_CONTROL_INPUT_COMPONENTS GL_MAX_TESS_CONTROL_INPUT_COMPONENTS_EXT 
	    #define GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS GL_MAX_TESS_EVALUATION_INPUT_COMPONENTS_EXT 
	    #define GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS GL_MAX_COMBINED_TESS_CONTROL_UNIFORM_COMPONENTS_EXT 
	    #define GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS GL_MAX_COMBINED_TESS_EVALUATION_UNIFORM_COMPONENTS_EXT 
	    #define GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS GL_MAX_TESS_CONTROL_ATOMIC_COUNTER_BUFFERS_EXT 
	    #define GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS GL_MAX_TESS_EVALUATION_ATOMIC_COUNTER_BUFFERS_EXT 
	    #define GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS_EXT 
	    #define GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS_EXT 
	    #define GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS GL_MAX_TESS_CONTROL_IMAGE_UNIFORMS_EXT 
	    #define GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS GL_MAX_TESS_EVALUATION_IMAGE_UNIFORMS_EXT 
	    #define GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS_EXT 
	    #define GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS_EXT 
	    #define GL_IS_PER_PATCH GL_IS_PER_PATCH_EXT               
	    #define GL_REFERENCED_BY_TESS_CONTROL_SHADER GL_REFERENCED_BY_TESS_CONTROL_SHADER_EXT 
	    #define GL_REFERENCED_BY_TESS_EVALUATION_SHADER GL_REFERENCED_BY_TESS_EVALUATION_SHADER_EXT 
	    #define GL_TESS_CONTROL_SHADER GL_TESS_CONTROL_SHADER_EXT        
	    #define GL_TESS_EVALUATION_SHADER GL_TESS_EVALUATION_SHADER_EXT     
	    #define GL_TESS_CONTROL_SHADER_BIT GL_TESS_CONTROL_SHADER_BIT_EXT    
	    #define GL_TESS_EVALUATION_SHADER_BIT GL_TESS_EVALUATION_SHADER_BIT_EXT 

	    typedef void (GL_APIENTRYP PFNGLFRAMEBUFFERTEXTUREPROC) (GLenum target, GLenum attachment, GLuint texture, GLint level);
	    typedef void (GL_APIENTRYP PFNGLPATCHPARAMETERIPROC) (GLenum pname, GLint value);
	    extern PFNGLFRAMEBUFFERTEXTUREPROC fnptr_ci_glFramebufferTexture;
	    extern PFNGLPATCHPARAMETERIPROC fnptr_ci_glPatchParameteri;
	    #define glFramebufferTexture fnptr_ci_glFramebufferTexture
	    #define glPatchParameteri fnptr_ci_glPatchParameteri
	#endif
#endif    

// ----------------------------------------------------------------------------
// gl_es_load
// ----------------------------------------------------------------------------
#if defined( CINDER_ANDROID ) || defined( CINDER_LINUX )	    
void gl_es_load();
#endif

