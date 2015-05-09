#ifndef OPENGL_GEN_CORE_4_4_HPP
#define OPENGL_GEN_CORE_4_4_HPP

#include "_int_load_test.hpp"
namespace gl
{
	enum
	{
		MAX_VERTEX_ATTRIB_STRIDE         = 0x82E5,
	};
	
	namespace _detail
	{
		typedef void (CODEGEN_FUNCPTR * Proc_glBindBuffersBase)(GLenum target, GLuint first, GLsizei count, const GLuint * buffers);
		typedef void (CODEGEN_FUNCPTR * Proc_glBindBuffersRange)(GLenum target, GLuint first, GLsizei count, const GLuint * buffers, const GLintptr * offsets, const GLsizeiptr * sizes);
		typedef void (CODEGEN_FUNCPTR * Proc_glBindImageTextures)(GLuint first, GLsizei count, const GLuint * textures);
		typedef void (CODEGEN_FUNCPTR * Proc_glBindSamplers)(GLuint first, GLsizei count, const GLuint * samplers);
		typedef void (CODEGEN_FUNCPTR * Proc_glBindTextures)(GLuint first, GLsizei count, const GLuint * textures);
		typedef void (CODEGEN_FUNCPTR * Proc_glBindVertexBuffers)(GLuint first, GLsizei count, const GLuint * buffers, const GLintptr * offsets, const GLsizei * strides);
		typedef void (CODEGEN_FUNCPTR * Proc_glBufferStorage)(GLenum target, GLsizeiptr size, const void * data, GLbitfield flags);
		typedef void (CODEGEN_FUNCPTR * Proc_glClearTexImage)(GLuint texture, GLint level, GLenum format, GLenum type, const void * data);
		typedef void (CODEGEN_FUNCPTR * Proc_glClearTexSubImage)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void * data);
	}
	
	extern _detail::Proc_glBindBuffersBase BindBuffersBase;
	extern _detail::Proc_glBindBuffersRange BindBuffersRange;
	extern _detail::Proc_glBindImageTextures BindImageTextures;
	extern _detail::Proc_glBindSamplers BindSamplers;
	extern _detail::Proc_glBindTextures BindTextures;
	extern _detail::Proc_glBindVertexBuffers BindVertexBuffers;
	extern _detail::Proc_glBufferStorage BufferStorage;
	extern _detail::Proc_glClearTexImage ClearTexImage;
	extern _detail::Proc_glClearTexSubImage ClearTexSubImage;
	
}
#endif /*OPENGL_GEN_CORE_4_4_HPP*/
