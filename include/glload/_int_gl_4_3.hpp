#ifndef OPENGL_GEN_CORE_4_3_HPP
#define OPENGL_GEN_CORE_4_3_HPP

#include "_int_load_test.hpp"
namespace gl
{
	enum
	{
		NUM_SHADING_LANGUAGE_VERSIONS    = 0x82E9,
		VERTEX_ATTRIB_ARRAY_LONG         = 0x874E,
	};
	
	namespace _detail
	{
		typedef void (CODEGEN_FUNCPTR * Proc_glBindVertexBuffer)(GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
		typedef void (CODEGEN_FUNCPTR * Proc_glClearBufferData)(GLenum target, GLenum internalformat, GLenum format, GLenum type, const void * data);
		typedef void (CODEGEN_FUNCPTR * Proc_glClearBufferSubData)(GLenum target, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void * data);
		typedef void (CODEGEN_FUNCPTR * Proc_glCopyImageSubData)(GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth);
		typedef void (CODEGEN_FUNCPTR * Proc_glDebugMessageCallback)(GLDEBUGPROC callback, const void * userParam);
		typedef void (CODEGEN_FUNCPTR * Proc_glDebugMessageControl)(GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint * ids, GLboolean enabled);
		typedef void (CODEGEN_FUNCPTR * Proc_glDebugMessageInsert)(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * buf);
		typedef void (CODEGEN_FUNCPTR * Proc_glDispatchCompute)(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z);
		typedef void (CODEGEN_FUNCPTR * Proc_glDispatchComputeIndirect)(GLintptr indirect);
		typedef void (CODEGEN_FUNCPTR * Proc_glFramebufferParameteri)(GLenum target, GLenum pname, GLint param);
		typedef GLuint (CODEGEN_FUNCPTR * Proc_glGetDebugMessageLog)(GLuint count, GLsizei bufsize, GLenum * sources, GLenum * types, GLuint * ids, GLenum * severities, GLsizei * lengths, GLchar * messageLog);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetFramebufferParameteriv)(GLenum target, GLenum pname, GLint * params);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetObjectLabel)(GLenum identifier, GLuint name, GLsizei bufSize, GLsizei * length, GLchar * label);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetObjectPtrLabel)(const void * ptr, GLsizei bufSize, GLsizei * length, GLchar * label);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetProgramInterfaceiv)(GLuint program, GLenum programInterface, GLenum pname, GLint * params);
		typedef GLuint (CODEGEN_FUNCPTR * Proc_glGetProgramResourceIndex)(GLuint program, GLenum programInterface, const GLchar * name);
		typedef GLint (CODEGEN_FUNCPTR * Proc_glGetProgramResourceLocation)(GLuint program, GLenum programInterface, const GLchar * name);
		typedef GLint (CODEGEN_FUNCPTR * Proc_glGetProgramResourceLocationIndex)(GLuint program, GLenum programInterface, const GLchar * name);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetProgramResourceName)(GLuint program, GLenum programInterface, GLuint index, GLsizei bufSize, GLsizei * length, GLchar * name);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetProgramResourceiv)(GLuint program, GLenum programInterface, GLuint index, GLsizei propCount, const GLenum * props, GLsizei bufSize, GLsizei * length, GLint * params);
		typedef void (CODEGEN_FUNCPTR * Proc_glInvalidateBufferData)(GLuint buffer);
		typedef void (CODEGEN_FUNCPTR * Proc_glInvalidateBufferSubData)(GLuint buffer, GLintptr offset, GLsizeiptr length);
		typedef void (CODEGEN_FUNCPTR * Proc_glInvalidateFramebuffer)(GLenum target, GLsizei numAttachments, const GLenum * attachments);
		typedef void (CODEGEN_FUNCPTR * Proc_glInvalidateSubFramebuffer)(GLenum target, GLsizei numAttachments, const GLenum * attachments, GLint x, GLint y, GLsizei width, GLsizei height);
		typedef void (CODEGEN_FUNCPTR * Proc_glInvalidateTexImage)(GLuint texture, GLint level);
		typedef void (CODEGEN_FUNCPTR * Proc_glInvalidateTexSubImage)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiDrawArraysIndirect)(GLenum mode, const void * indirect, GLsizei drawcount, GLsizei stride);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiDrawElementsIndirect)(GLenum mode, GLenum type, const void * indirect, GLsizei drawcount, GLsizei stride);
		typedef void (CODEGEN_FUNCPTR * Proc_glObjectLabel)(GLenum identifier, GLuint name, GLsizei length, const GLchar * label);
		typedef void (CODEGEN_FUNCPTR * Proc_glObjectPtrLabel)(const void * ptr, GLsizei length, const GLchar * label);
		typedef void (CODEGEN_FUNCPTR * Proc_glPopDebugGroup)();
		typedef void (CODEGEN_FUNCPTR * Proc_glPushDebugGroup)(GLenum source, GLuint id, GLsizei length, const GLchar * message);
		typedef void (CODEGEN_FUNCPTR * Proc_glShaderStorageBlockBinding)(GLuint program, GLuint storageBlockIndex, GLuint storageBlockBinding);
		typedef void (CODEGEN_FUNCPTR * Proc_glTexBufferRange)(GLenum target, GLenum internalformat, GLuint buffer, GLintptr offset, GLsizeiptr size);
		typedef void (CODEGEN_FUNCPTR * Proc_glTexStorage2DMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
		typedef void (CODEGEN_FUNCPTR * Proc_glTexStorage3DMultisample)(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
		typedef void (CODEGEN_FUNCPTR * Proc_glTextureView)(GLuint texture, GLenum target, GLuint origtexture, GLenum internalformat, GLuint minlevel, GLuint numlevels, GLuint minlayer, GLuint numlayers);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttribBinding)(GLuint attribindex, GLuint bindingindex);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttribFormat)(GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttribIFormat)(GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttribLFormat)(GLuint attribindex, GLint size, GLenum type, GLuint relativeoffset);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexBindingDivisor)(GLuint bindingindex, GLuint divisor);
	}
	
	extern _detail::Proc_glBindVertexBuffer BindVertexBuffer;
	extern _detail::Proc_glClearBufferData ClearBufferData;
	extern _detail::Proc_glClearBufferSubData ClearBufferSubData;
	extern _detail::Proc_glCopyImageSubData CopyImageSubData;
	extern _detail::Proc_glDebugMessageCallback DebugMessageCallback;
	extern _detail::Proc_glDebugMessageControl DebugMessageControl;
	extern _detail::Proc_glDebugMessageInsert DebugMessageInsert;
	extern _detail::Proc_glDispatchCompute DispatchCompute;
	extern _detail::Proc_glDispatchComputeIndirect DispatchComputeIndirect;
	extern _detail::Proc_glFramebufferParameteri FramebufferParameteri;
	extern _detail::Proc_glGetDebugMessageLog GetDebugMessageLog;
	extern _detail::Proc_glGetFramebufferParameteriv GetFramebufferParameteriv;
	extern _detail::Proc_glGetObjectLabel GetObjectLabel;
	extern _detail::Proc_glGetObjectPtrLabel GetObjectPtrLabel;
	extern _detail::Proc_glGetProgramInterfaceiv GetProgramInterfaceiv;
	extern _detail::Proc_glGetProgramResourceIndex GetProgramResourceIndex;
	extern _detail::Proc_glGetProgramResourceLocation GetProgramResourceLocation;
	extern _detail::Proc_glGetProgramResourceLocationIndex GetProgramResourceLocationIndex;
	extern _detail::Proc_glGetProgramResourceName GetProgramResourceName;
	extern _detail::Proc_glGetProgramResourceiv GetProgramResourceiv;
	extern _detail::Proc_glInvalidateBufferData InvalidateBufferData;
	extern _detail::Proc_glInvalidateBufferSubData InvalidateBufferSubData;
	extern _detail::Proc_glInvalidateFramebuffer InvalidateFramebuffer;
	extern _detail::Proc_glInvalidateSubFramebuffer InvalidateSubFramebuffer;
	extern _detail::Proc_glInvalidateTexImage InvalidateTexImage;
	extern _detail::Proc_glInvalidateTexSubImage InvalidateTexSubImage;
	extern _detail::Proc_glMultiDrawArraysIndirect MultiDrawArraysIndirect;
	extern _detail::Proc_glMultiDrawElementsIndirect MultiDrawElementsIndirect;
	extern _detail::Proc_glObjectLabel ObjectLabel;
	extern _detail::Proc_glObjectPtrLabel ObjectPtrLabel;
	extern _detail::Proc_glPopDebugGroup PopDebugGroup;
	extern _detail::Proc_glPushDebugGroup PushDebugGroup;
	extern _detail::Proc_glShaderStorageBlockBinding ShaderStorageBlockBinding;
	extern _detail::Proc_glTexBufferRange TexBufferRange;
	extern _detail::Proc_glTexStorage2DMultisample TexStorage2DMultisample;
	extern _detail::Proc_glTexStorage3DMultisample TexStorage3DMultisample;
	extern _detail::Proc_glTextureView TextureView;
	extern _detail::Proc_glVertexAttribBinding VertexAttribBinding;
	extern _detail::Proc_glVertexAttribFormat VertexAttribFormat;
	extern _detail::Proc_glVertexAttribIFormat VertexAttribIFormat;
	extern _detail::Proc_glVertexAttribLFormat VertexAttribLFormat;
	extern _detail::Proc_glVertexBindingDivisor VertexBindingDivisor;
	
}
#endif /*OPENGL_GEN_CORE_4_3_HPP*/
