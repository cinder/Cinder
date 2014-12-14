#ifndef OPENGL_GEN_CORE_3_1_HPP
#define OPENGL_GEN_CORE_3_1_HPP

#include "_int_load_test.hpp"
namespace gl
{
	enum
	{
		INT_SAMPLER_2D_RECT              = 0x8DCD,
		INT_SAMPLER_BUFFER               = 0x8DD0,
		MAX_RECTANGLE_TEXTURE_SIZE       = 0x84F8,
		MAX_TEXTURE_BUFFER_SIZE          = 0x8C2B,
		PRIMITIVE_RESTART                = 0x8F9D,
		PRIMITIVE_RESTART_INDEX          = 0x8F9E,
		PROXY_TEXTURE_RECTANGLE          = 0x84F7,
		SAMPLER_2D_RECT                  = 0x8B63,
		SAMPLER_2D_RECT_SHADOW           = 0x8B64,
		SAMPLER_BUFFER                   = 0x8DC2,
		TEXTURE_BINDING_BUFFER           = 0x8C2C,
		TEXTURE_BINDING_RECTANGLE        = 0x84F6,
		TEXTURE_BUFFER_DATA_STORE_BINDING = 0x8C2D,
		UNSIGNED_INT_SAMPLER_2D_RECT     = 0x8DD5,
		UNSIGNED_INT_SAMPLER_BUFFER      = 0x8DD8,
	};
	
	namespace _detail
	{
		typedef void (CODEGEN_FUNCPTR * Proc_glCopyBufferSubData)(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
		typedef void (CODEGEN_FUNCPTR * Proc_glDrawArraysInstanced)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
		typedef void (CODEGEN_FUNCPTR * Proc_glDrawElementsInstanced)(GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLsizei instancecount);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetActiveUniformBlockName)(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei * length, GLchar * uniformBlockName);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetActiveUniformBlockiv)(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint * params);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetActiveUniformName)(GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei * length, GLchar * uniformName);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetActiveUniformsiv)(GLuint program, GLsizei uniformCount, const GLuint * uniformIndices, GLenum pname, GLint * params);
		typedef GLuint (CODEGEN_FUNCPTR * Proc_glGetUniformBlockIndex)(GLuint program, const GLchar * uniformBlockName);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetUniformIndices)(GLuint program, GLsizei uniformCount, const GLchar *const* uniformNames, GLuint * uniformIndices);
		typedef void (CODEGEN_FUNCPTR * Proc_glPrimitiveRestartIndex)(GLuint index);
		typedef void (CODEGEN_FUNCPTR * Proc_glTexBuffer)(GLenum target, GLenum internalformat, GLuint buffer);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniformBlockBinding)(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);
	}
	
	extern _detail::Proc_glCopyBufferSubData CopyBufferSubData;
	extern _detail::Proc_glDrawArraysInstanced DrawArraysInstanced;
	extern _detail::Proc_glDrawElementsInstanced DrawElementsInstanced;
	extern _detail::Proc_glGetActiveUniformBlockName GetActiveUniformBlockName;
	extern _detail::Proc_glGetActiveUniformBlockiv GetActiveUniformBlockiv;
	extern _detail::Proc_glGetActiveUniformName GetActiveUniformName;
	extern _detail::Proc_glGetActiveUniformsiv GetActiveUniformsiv;
	extern _detail::Proc_glGetUniformBlockIndex GetUniformBlockIndex;
	extern _detail::Proc_glGetUniformIndices GetUniformIndices;
	extern _detail::Proc_glPrimitiveRestartIndex PrimitiveRestartIndex;
	extern _detail::Proc_glTexBuffer TexBuffer;
	extern _detail::Proc_glUniformBlockBinding UniformBlockBinding;
	
}
#endif /*OPENGL_GEN_CORE_3_1_HPP*/
