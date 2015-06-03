#ifndef OPENGL_GEN_CORE_3_2_HPP
#define OPENGL_GEN_CORE_3_2_HPP

#include "_int_load_test.hpp"
namespace gl
{
	enum
	{
		CONTEXT_COMPATIBILITY_PROFILE_BIT = 0x00000002,
		CONTEXT_CORE_PROFILE_BIT         = 0x00000001,
		CONTEXT_PROFILE_MASK             = 0x9126,
		FRAMEBUFFER_ATTACHMENT_LAYERED   = 0x8DA7,
		FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS = 0x8DA8,
		GEOMETRY_INPUT_TYPE              = 0x8917,
		GEOMETRY_OUTPUT_TYPE             = 0x8918,
		GEOMETRY_SHADER                  = 0x8DD9,
		GEOMETRY_VERTICES_OUT            = 0x8916,
		LINES_ADJACENCY                  = 0x000A,
		LINE_STRIP_ADJACENCY             = 0x000B,
		MAX_FRAGMENT_INPUT_COMPONENTS    = 0x9125,
		MAX_GEOMETRY_INPUT_COMPONENTS    = 0x9123,
		MAX_GEOMETRY_OUTPUT_COMPONENTS   = 0x9124,
		MAX_GEOMETRY_OUTPUT_VERTICES     = 0x8DE0,
		MAX_GEOMETRY_TEXTURE_IMAGE_UNITS = 0x8C29,
		MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS = 0x8DE1,
		MAX_GEOMETRY_UNIFORM_COMPONENTS  = 0x8DDF,
		MAX_VERTEX_OUTPUT_COMPONENTS     = 0x9122,
		PROGRAM_POINT_SIZE               = 0x8642,
		TRIANGLES_ADJACENCY              = 0x000C,
		TRIANGLE_STRIP_ADJACENCY         = 0x000D,
	};
	
	namespace _detail
	{
		typedef GLenum (CODEGEN_FUNCPTR * Proc_glClientWaitSync)(GLsync sync, GLbitfield flags, GLuint64 timeout);
		typedef void (CODEGEN_FUNCPTR * Proc_glDeleteSync)(GLsync sync);
		typedef void (CODEGEN_FUNCPTR * Proc_glDrawElementsBaseVertex)(GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLint basevertex);
		typedef void (CODEGEN_FUNCPTR * Proc_glDrawElementsInstancedBaseVertex)(GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLsizei instancecount, GLint basevertex);
		typedef void (CODEGEN_FUNCPTR * Proc_glDrawRangeElementsBaseVertex)(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid * indices, GLint basevertex);
		typedef GLsync (CODEGEN_FUNCPTR * Proc_glFenceSync)(GLenum condition, GLbitfield flags);
		typedef void (CODEGEN_FUNCPTR * Proc_glFramebufferTexture)(GLenum target, GLenum attachment, GLuint texture, GLint level);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetBufferParameteri64v)(GLenum target, GLenum pname, GLint64 * params);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetInteger64i_v)(GLenum target, GLuint index, GLint64 * data);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetInteger64v)(GLenum pname, GLint64 * params);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetMultisamplefv)(GLenum pname, GLuint index, GLfloat * val);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetSynciv)(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei * length, GLint * values);
		typedef GLboolean (CODEGEN_FUNCPTR * Proc_glIsSync)(GLsync sync);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiDrawElementsBaseVertex)(GLenum mode, const GLsizei * count, GLenum type, const GLvoid *const* indices, GLsizei drawcount, const GLint * basevertex);
		typedef void (CODEGEN_FUNCPTR * Proc_glProvokingVertex)(GLenum mode);
		typedef void (CODEGEN_FUNCPTR * Proc_glSampleMaski)(GLuint index, GLbitfield mask);
		typedef void (CODEGEN_FUNCPTR * Proc_glTexImage2DMultisample)(GLenum target, GLsizei samples, GLint internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
		typedef void (CODEGEN_FUNCPTR * Proc_glTexImage3DMultisample)(GLenum target, GLsizei samples, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
		typedef void (CODEGEN_FUNCPTR * Proc_glWaitSync)(GLsync sync, GLbitfield flags, GLuint64 timeout);
	}
	
	extern _detail::Proc_glClientWaitSync ClientWaitSync;
	extern _detail::Proc_glDeleteSync DeleteSync;
	extern _detail::Proc_glDrawElementsBaseVertex DrawElementsBaseVertex;
	extern _detail::Proc_glDrawElementsInstancedBaseVertex DrawElementsInstancedBaseVertex;
	extern _detail::Proc_glDrawRangeElementsBaseVertex DrawRangeElementsBaseVertex;
	extern _detail::Proc_glFenceSync FenceSync;
	extern _detail::Proc_glFramebufferTexture FramebufferTexture;
	extern _detail::Proc_glGetBufferParameteri64v GetBufferParameteri64v;
	extern _detail::Proc_glGetInteger64i_v GetInteger64i_v;
	extern _detail::Proc_glGetInteger64v GetInteger64v;
	extern _detail::Proc_glGetMultisamplefv GetMultisamplefv;
	extern _detail::Proc_glGetSynciv GetSynciv;
	extern _detail::Proc_glIsSync IsSync;
	extern _detail::Proc_glMultiDrawElementsBaseVertex MultiDrawElementsBaseVertex;
	extern _detail::Proc_glProvokingVertex ProvokingVertex;
	extern _detail::Proc_glSampleMaski SampleMaski;
	extern _detail::Proc_glTexImage2DMultisample TexImage2DMultisample;
	extern _detail::Proc_glTexImage3DMultisample TexImage3DMultisample;
	extern _detail::Proc_glWaitSync WaitSync;
	
}
#endif /*OPENGL_GEN_CORE_3_2_HPP*/
