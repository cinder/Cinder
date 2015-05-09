#ifndef OPENGL_GEN_CORE_1_5_HPP
#define OPENGL_GEN_CORE_1_5_HPP

#include "_int_load_test.hpp"
namespace gl
{
	enum
	{
		ARRAY_BUFFER                     = 0x8892,
		ARRAY_BUFFER_BINDING             = 0x8894,
		BUFFER_ACCESS                    = 0x88BB,
		BUFFER_MAPPED                    = 0x88BC,
		BUFFER_MAP_POINTER               = 0x88BD,
		BUFFER_SIZE                      = 0x8764,
		BUFFER_USAGE                     = 0x8765,
		CURRENT_QUERY                    = 0x8865,
		DYNAMIC_COPY                     = 0x88EA,
		DYNAMIC_DRAW                     = 0x88E8,
		DYNAMIC_READ                     = 0x88E9,
		ELEMENT_ARRAY_BUFFER             = 0x8893,
		ELEMENT_ARRAY_BUFFER_BINDING     = 0x8895,
		QUERY_COUNTER_BITS               = 0x8864,
		QUERY_RESULT                     = 0x8866,
		QUERY_RESULT_AVAILABLE           = 0x8867,
		READ_ONLY                        = 0x88B8,
		SAMPLES_PASSED                   = 0x8914,
		STATIC_COPY                      = 0x88E6,
		STATIC_DRAW                      = 0x88E4,
		STATIC_READ                      = 0x88E5,
		STREAM_COPY                      = 0x88E2,
		STREAM_DRAW                      = 0x88E0,
		STREAM_READ                      = 0x88E1,
		VERTEX_ATTRIB_ARRAY_BUFFER_BINDING = 0x889F,
	};
	
	namespace _detail
	{
		typedef void (CODEGEN_FUNCPTR * Proc_glBeginQuery)(GLenum target, GLuint id);
		typedef void (CODEGEN_FUNCPTR * Proc_glBindBuffer)(GLenum target, GLuint buffer);
		typedef void (CODEGEN_FUNCPTR * Proc_glBufferData)(GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage);
		typedef void (CODEGEN_FUNCPTR * Proc_glBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data);
		typedef void (CODEGEN_FUNCPTR * Proc_glDeleteBuffers)(GLsizei n, const GLuint * buffers);
		typedef void (CODEGEN_FUNCPTR * Proc_glDeleteQueries)(GLsizei n, const GLuint * ids);
		typedef void (CODEGEN_FUNCPTR * Proc_glEndQuery)(GLenum target);
		typedef void (CODEGEN_FUNCPTR * Proc_glGenBuffers)(GLsizei n, GLuint * buffers);
		typedef void (CODEGEN_FUNCPTR * Proc_glGenQueries)(GLsizei n, GLuint * ids);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetBufferParameteriv)(GLenum target, GLenum pname, GLint * params);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetBufferPointerv)(GLenum target, GLenum pname, GLvoid ** params);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetBufferSubData)(GLenum target, GLintptr offset, GLsizeiptr size, GLvoid * data);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetQueryObjectiv)(GLuint id, GLenum pname, GLint * params);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetQueryObjectuiv)(GLuint id, GLenum pname, GLuint * params);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetQueryiv)(GLenum target, GLenum pname, GLint * params);
		typedef GLboolean (CODEGEN_FUNCPTR * Proc_glIsBuffer)(GLuint buffer);
		typedef GLboolean (CODEGEN_FUNCPTR * Proc_glIsQuery)(GLuint id);
		typedef void * (CODEGEN_FUNCPTR * Proc_glMapBuffer)(GLenum target, GLenum access);
		typedef GLboolean (CODEGEN_FUNCPTR * Proc_glUnmapBuffer)(GLenum target);
	}
	
	extern _detail::Proc_glBeginQuery BeginQuery;
	extern _detail::Proc_glBindBuffer BindBuffer;
	extern _detail::Proc_glBufferData BufferData;
	extern _detail::Proc_glBufferSubData BufferSubData;
	extern _detail::Proc_glDeleteBuffers DeleteBuffers;
	extern _detail::Proc_glDeleteQueries DeleteQueries;
	extern _detail::Proc_glEndQuery EndQuery;
	extern _detail::Proc_glGenBuffers GenBuffers;
	extern _detail::Proc_glGenQueries GenQueries;
	extern _detail::Proc_glGetBufferParameteriv GetBufferParameteriv;
	extern _detail::Proc_glGetBufferPointerv GetBufferPointerv;
	extern _detail::Proc_glGetBufferSubData GetBufferSubData;
	extern _detail::Proc_glGetQueryObjectiv GetQueryObjectiv;
	extern _detail::Proc_glGetQueryObjectuiv GetQueryObjectuiv;
	extern _detail::Proc_glGetQueryiv GetQueryiv;
	extern _detail::Proc_glIsBuffer IsBuffer;
	extern _detail::Proc_glIsQuery IsQuery;
	extern _detail::Proc_glMapBuffer MapBuffer;
	extern _detail::Proc_glUnmapBuffer UnmapBuffer;
	
}
#endif /*OPENGL_GEN_CORE_1_5_HPP*/
