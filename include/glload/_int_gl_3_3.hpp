#ifndef OPENGL_GEN_CORE_3_3_HPP
#define OPENGL_GEN_CORE_3_3_HPP

#include "_int_load_test.hpp"
namespace gl
{
	enum
	{
		VERTEX_ATTRIB_ARRAY_DIVISOR      = 0x88FE,
	};
	
	namespace _detail
	{
		typedef void (CODEGEN_FUNCPTR * Proc_glBindFragDataLocationIndexed)(GLuint program, GLuint colorNumber, GLuint index, const GLchar * name);
		typedef void (CODEGEN_FUNCPTR * Proc_glBindSampler)(GLuint unit, GLuint sampler);
		typedef void (CODEGEN_FUNCPTR * Proc_glDeleteSamplers)(GLsizei count, const GLuint * samplers);
		typedef void (CODEGEN_FUNCPTR * Proc_glGenSamplers)(GLsizei count, GLuint * samplers);
		typedef GLint (CODEGEN_FUNCPTR * Proc_glGetFragDataIndex)(GLuint program, const GLchar * name);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetQueryObjecti64v)(GLuint id, GLenum pname, GLint64 * params);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetQueryObjectui64v)(GLuint id, GLenum pname, GLuint64 * params);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetSamplerParameterIiv)(GLuint sampler, GLenum pname, GLint * params);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetSamplerParameterIuiv)(GLuint sampler, GLenum pname, GLuint * params);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetSamplerParameterfv)(GLuint sampler, GLenum pname, GLfloat * params);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetSamplerParameteriv)(GLuint sampler, GLenum pname, GLint * params);
		typedef GLboolean (CODEGEN_FUNCPTR * Proc_glIsSampler)(GLuint sampler);
		typedef void (CODEGEN_FUNCPTR * Proc_glQueryCounter)(GLuint id, GLenum target);
		typedef void (CODEGEN_FUNCPTR * Proc_glSamplerParameterIiv)(GLuint sampler, GLenum pname, const GLint * param);
		typedef void (CODEGEN_FUNCPTR * Proc_glSamplerParameterIuiv)(GLuint sampler, GLenum pname, const GLuint * param);
		typedef void (CODEGEN_FUNCPTR * Proc_glSamplerParameterf)(GLuint sampler, GLenum pname, GLfloat param);
		typedef void (CODEGEN_FUNCPTR * Proc_glSamplerParameterfv)(GLuint sampler, GLenum pname, const GLfloat * param);
		typedef void (CODEGEN_FUNCPTR * Proc_glSamplerParameteri)(GLuint sampler, GLenum pname, GLint param);
		typedef void (CODEGEN_FUNCPTR * Proc_glSamplerParameteriv)(GLuint sampler, GLenum pname, const GLint * param);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttribDivisor)(GLuint index, GLuint divisor);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttribP1ui)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttribP1uiv)(GLuint index, GLenum type, GLboolean normalized, const GLuint * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttribP2ui)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttribP2uiv)(GLuint index, GLenum type, GLboolean normalized, const GLuint * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttribP3ui)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttribP3uiv)(GLuint index, GLenum type, GLboolean normalized, const GLuint * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttribP4ui)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttribP4uiv)(GLuint index, GLenum type, GLboolean normalized, const GLuint * value);
	}
	
	extern _detail::Proc_glBindFragDataLocationIndexed BindFragDataLocationIndexed;
	extern _detail::Proc_glBindSampler BindSampler;
	extern _detail::Proc_glDeleteSamplers DeleteSamplers;
	extern _detail::Proc_glGenSamplers GenSamplers;
	extern _detail::Proc_glGetFragDataIndex GetFragDataIndex;
	extern _detail::Proc_glGetQueryObjecti64v GetQueryObjecti64v;
	extern _detail::Proc_glGetQueryObjectui64v GetQueryObjectui64v;
	extern _detail::Proc_glGetSamplerParameterIiv GetSamplerParameterIiv;
	extern _detail::Proc_glGetSamplerParameterIuiv GetSamplerParameterIuiv;
	extern _detail::Proc_glGetSamplerParameterfv GetSamplerParameterfv;
	extern _detail::Proc_glGetSamplerParameteriv GetSamplerParameteriv;
	extern _detail::Proc_glIsSampler IsSampler;
	extern _detail::Proc_glQueryCounter QueryCounter;
	extern _detail::Proc_glSamplerParameterIiv SamplerParameterIiv;
	extern _detail::Proc_glSamplerParameterIuiv SamplerParameterIuiv;
	extern _detail::Proc_glSamplerParameterf SamplerParameterf;
	extern _detail::Proc_glSamplerParameterfv SamplerParameterfv;
	extern _detail::Proc_glSamplerParameteri SamplerParameteri;
	extern _detail::Proc_glSamplerParameteriv SamplerParameteriv;
	extern _detail::Proc_glVertexAttribDivisor VertexAttribDivisor;
	extern _detail::Proc_glVertexAttribP1ui VertexAttribP1ui;
	extern _detail::Proc_glVertexAttribP1uiv VertexAttribP1uiv;
	extern _detail::Proc_glVertexAttribP2ui VertexAttribP2ui;
	extern _detail::Proc_glVertexAttribP2uiv VertexAttribP2uiv;
	extern _detail::Proc_glVertexAttribP3ui VertexAttribP3ui;
	extern _detail::Proc_glVertexAttribP3uiv VertexAttribP3uiv;
	extern _detail::Proc_glVertexAttribP4ui VertexAttribP4ui;
	extern _detail::Proc_glVertexAttribP4uiv VertexAttribP4uiv;
	
}
#endif /*OPENGL_GEN_CORE_3_3_HPP*/
