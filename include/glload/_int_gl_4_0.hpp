#ifndef OPENGL_GEN_CORE_4_0_HPP
#define OPENGL_GEN_CORE_4_0_HPP

#include "_int_load_test.hpp"
namespace gl
{
	enum
	{
		INT_SAMPLER_CUBE_MAP_ARRAY       = 0x900E,
		MAX_PROGRAM_TEXTURE_GATHER_OFFSET = 0x8E5F,
		MIN_PROGRAM_TEXTURE_GATHER_OFFSET = 0x8E5E,
		MIN_SAMPLE_SHADING_VALUE         = 0x8C37,
		PROXY_TEXTURE_CUBE_MAP_ARRAY     = 0x900B,
		SAMPLER_CUBE_MAP_ARRAY           = 0x900C,
		SAMPLER_CUBE_MAP_ARRAY_SHADOW    = 0x900D,
		SAMPLE_SHADING                   = 0x8C36,
		TEXTURE_BINDING_CUBE_MAP_ARRAY   = 0x900A,
		UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY = 0x900F,
	};
	
	namespace _detail
	{
		typedef void (CODEGEN_FUNCPTR * Proc_glBeginQueryIndexed)(GLenum target, GLuint index, GLuint id);
		typedef void (CODEGEN_FUNCPTR * Proc_glBindTransformFeedback)(GLenum target, GLuint id);
		typedef void (CODEGEN_FUNCPTR * Proc_glBlendEquationSeparatei)(GLuint buf, GLenum modeRGB, GLenum modeAlpha);
		typedef void (CODEGEN_FUNCPTR * Proc_glBlendEquationi)(GLuint buf, GLenum mode);
		typedef void (CODEGEN_FUNCPTR * Proc_glBlendFuncSeparatei)(GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
		typedef void (CODEGEN_FUNCPTR * Proc_glBlendFunci)(GLuint buf, GLenum src, GLenum dst);
		typedef void (CODEGEN_FUNCPTR * Proc_glDeleteTransformFeedbacks)(GLsizei n, const GLuint * ids);
		typedef void (CODEGEN_FUNCPTR * Proc_glDrawArraysIndirect)(GLenum mode, const GLvoid * indirect);
		typedef void (CODEGEN_FUNCPTR * Proc_glDrawElementsIndirect)(GLenum mode, GLenum type, const GLvoid * indirect);
		typedef void (CODEGEN_FUNCPTR * Proc_glDrawTransformFeedback)(GLenum mode, GLuint id);
		typedef void (CODEGEN_FUNCPTR * Proc_glDrawTransformFeedbackStream)(GLenum mode, GLuint id, GLuint stream);
		typedef void (CODEGEN_FUNCPTR * Proc_glEndQueryIndexed)(GLenum target, GLuint index);
		typedef void (CODEGEN_FUNCPTR * Proc_glGenTransformFeedbacks)(GLsizei n, GLuint * ids);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetActiveSubroutineName)(GLuint program, GLenum shadertype, GLuint index, GLsizei bufsize, GLsizei * length, GLchar * name);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetActiveSubroutineUniformName)(GLuint program, GLenum shadertype, GLuint index, GLsizei bufsize, GLsizei * length, GLchar * name);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetActiveSubroutineUniformiv)(GLuint program, GLenum shadertype, GLuint index, GLenum pname, GLint * values);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetProgramStageiv)(GLuint program, GLenum shadertype, GLenum pname, GLint * values);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetQueryIndexediv)(GLenum target, GLuint index, GLenum pname, GLint * params);
		typedef GLuint (CODEGEN_FUNCPTR * Proc_glGetSubroutineIndex)(GLuint program, GLenum shadertype, const GLchar * name);
		typedef GLint (CODEGEN_FUNCPTR * Proc_glGetSubroutineUniformLocation)(GLuint program, GLenum shadertype, const GLchar * name);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetUniformSubroutineuiv)(GLenum shadertype, GLint location, GLuint * params);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetUniformdv)(GLuint program, GLint location, GLdouble * params);
		typedef GLboolean (CODEGEN_FUNCPTR * Proc_glIsTransformFeedback)(GLuint id);
		typedef void (CODEGEN_FUNCPTR * Proc_glMinSampleShading)(GLfloat value);
		typedef void (CODEGEN_FUNCPTR * Proc_glPatchParameterfv)(GLenum pname, const GLfloat * values);
		typedef void (CODEGEN_FUNCPTR * Proc_glPatchParameteri)(GLenum pname, GLint value);
		typedef void (CODEGEN_FUNCPTR * Proc_glPauseTransformFeedback)();
		typedef void (CODEGEN_FUNCPTR * Proc_glResumeTransformFeedback)();
		typedef void (CODEGEN_FUNCPTR * Proc_glUniform1d)(GLint location, GLdouble x);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniform1dv)(GLint location, GLsizei count, const GLdouble * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniform2d)(GLint location, GLdouble x, GLdouble y);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniform2dv)(GLint location, GLsizei count, const GLdouble * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniform3d)(GLint location, GLdouble x, GLdouble y, GLdouble z);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniform3dv)(GLint location, GLsizei count, const GLdouble * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniform4d)(GLint location, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniform4dv)(GLint location, GLsizei count, const GLdouble * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniformMatrix2dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniformMatrix2x3dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniformMatrix2x4dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniformMatrix3dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniformMatrix3x2dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniformMatrix3x4dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniformMatrix4dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniformMatrix4x2dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniformMatrix4x3dv)(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniformSubroutinesuiv)(GLenum shadertype, GLsizei count, const GLuint * indices);
	}
	
	extern _detail::Proc_glBeginQueryIndexed BeginQueryIndexed;
	extern _detail::Proc_glBindTransformFeedback BindTransformFeedback;
	extern _detail::Proc_glBlendEquationSeparatei BlendEquationSeparatei;
	extern _detail::Proc_glBlendEquationi BlendEquationi;
	extern _detail::Proc_glBlendFuncSeparatei BlendFuncSeparatei;
	extern _detail::Proc_glBlendFunci BlendFunci;
	extern _detail::Proc_glDeleteTransformFeedbacks DeleteTransformFeedbacks;
	extern _detail::Proc_glDrawArraysIndirect DrawArraysIndirect;
	extern _detail::Proc_glDrawElementsIndirect DrawElementsIndirect;
	extern _detail::Proc_glDrawTransformFeedback DrawTransformFeedback;
	extern _detail::Proc_glDrawTransformFeedbackStream DrawTransformFeedbackStream;
	extern _detail::Proc_glEndQueryIndexed EndQueryIndexed;
	extern _detail::Proc_glGenTransformFeedbacks GenTransformFeedbacks;
	extern _detail::Proc_glGetActiveSubroutineName GetActiveSubroutineName;
	extern _detail::Proc_glGetActiveSubroutineUniformName GetActiveSubroutineUniformName;
	extern _detail::Proc_glGetActiveSubroutineUniformiv GetActiveSubroutineUniformiv;
	extern _detail::Proc_glGetProgramStageiv GetProgramStageiv;
	extern _detail::Proc_glGetQueryIndexediv GetQueryIndexediv;
	extern _detail::Proc_glGetSubroutineIndex GetSubroutineIndex;
	extern _detail::Proc_glGetSubroutineUniformLocation GetSubroutineUniformLocation;
	extern _detail::Proc_glGetUniformSubroutineuiv GetUniformSubroutineuiv;
	extern _detail::Proc_glGetUniformdv GetUniformdv;
	extern _detail::Proc_glIsTransformFeedback IsTransformFeedback;
	extern _detail::Proc_glMinSampleShading MinSampleShading;
	extern _detail::Proc_glPatchParameterfv PatchParameterfv;
	extern _detail::Proc_glPatchParameteri PatchParameteri;
	extern _detail::Proc_glPauseTransformFeedback PauseTransformFeedback;
	extern _detail::Proc_glResumeTransformFeedback ResumeTransformFeedback;
	extern _detail::Proc_glUniform1d Uniform1d;
	extern _detail::Proc_glUniform1dv Uniform1dv;
	extern _detail::Proc_glUniform2d Uniform2d;
	extern _detail::Proc_glUniform2dv Uniform2dv;
	extern _detail::Proc_glUniform3d Uniform3d;
	extern _detail::Proc_glUniform3dv Uniform3dv;
	extern _detail::Proc_glUniform4d Uniform4d;
	extern _detail::Proc_glUniform4dv Uniform4dv;
	extern _detail::Proc_glUniformMatrix2dv UniformMatrix2dv;
	extern _detail::Proc_glUniformMatrix2x3dv UniformMatrix2x3dv;
	extern _detail::Proc_glUniformMatrix2x4dv UniformMatrix2x4dv;
	extern _detail::Proc_glUniformMatrix3dv UniformMatrix3dv;
	extern _detail::Proc_glUniformMatrix3x2dv UniformMatrix3x2dv;
	extern _detail::Proc_glUniformMatrix3x4dv UniformMatrix3x4dv;
	extern _detail::Proc_glUniformMatrix4dv UniformMatrix4dv;
	extern _detail::Proc_glUniformMatrix4x2dv UniformMatrix4x2dv;
	extern _detail::Proc_glUniformMatrix4x3dv UniformMatrix4x3dv;
	extern _detail::Proc_glUniformSubroutinesuiv UniformSubroutinesuiv;
	
}
#endif /*OPENGL_GEN_CORE_4_0_HPP*/
