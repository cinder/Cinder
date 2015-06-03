#ifndef OPENGL_GEN_CORE_2_0_HPP
#define OPENGL_GEN_CORE_2_0_HPP

#include "_int_load_test.hpp"
namespace gl
{
	enum
	{
		ACTIVE_ATTRIBUTES                = 0x8B89,
		ACTIVE_ATTRIBUTE_MAX_LENGTH      = 0x8B8A,
		ACTIVE_UNIFORMS                  = 0x8B86,
		ACTIVE_UNIFORM_MAX_LENGTH        = 0x8B87,
		ATTACHED_SHADERS                 = 0x8B85,
		BLEND_EQUATION_ALPHA             = 0x883D,
		BLEND_EQUATION_RGB               = 0x8009,
		BOOL                             = 0x8B56,
		BOOL_VEC2                        = 0x8B57,
		BOOL_VEC3                        = 0x8B58,
		BOOL_VEC4                        = 0x8B59,
		COMPILE_STATUS                   = 0x8B81,
		CURRENT_PROGRAM                  = 0x8B8D,
		CURRENT_VERTEX_ATTRIB            = 0x8626,
		DELETE_STATUS                    = 0x8B80,
		DRAW_BUFFER0                     = 0x8825,
		DRAW_BUFFER1                     = 0x8826,
		DRAW_BUFFER10                    = 0x882F,
		DRAW_BUFFER11                    = 0x8830,
		DRAW_BUFFER12                    = 0x8831,
		DRAW_BUFFER13                    = 0x8832,
		DRAW_BUFFER14                    = 0x8833,
		DRAW_BUFFER15                    = 0x8834,
		DRAW_BUFFER2                     = 0x8827,
		DRAW_BUFFER3                     = 0x8828,
		DRAW_BUFFER4                     = 0x8829,
		DRAW_BUFFER5                     = 0x882A,
		DRAW_BUFFER6                     = 0x882B,
		DRAW_BUFFER7                     = 0x882C,
		DRAW_BUFFER8                     = 0x882D,
		DRAW_BUFFER9                     = 0x882E,
		FLOAT_MAT2                       = 0x8B5A,
		FLOAT_MAT3                       = 0x8B5B,
		FLOAT_MAT4                       = 0x8B5C,
		FLOAT_VEC2                       = 0x8B50,
		FLOAT_VEC3                       = 0x8B51,
		FLOAT_VEC4                       = 0x8B52,
		FRAGMENT_SHADER                  = 0x8B30,
		FRAGMENT_SHADER_DERIVATIVE_HINT  = 0x8B8B,
		INFO_LOG_LENGTH                  = 0x8B84,
		INT_VEC2                         = 0x8B53,
		INT_VEC3                         = 0x8B54,
		INT_VEC4                         = 0x8B55,
		LINK_STATUS                      = 0x8B82,
		LOWER_LEFT                       = 0x8CA1,
		MAX_COMBINED_TEXTURE_IMAGE_UNITS = 0x8B4D,
		MAX_DRAW_BUFFERS                 = 0x8824,
		MAX_FRAGMENT_UNIFORM_COMPONENTS  = 0x8B49,
		MAX_TEXTURE_IMAGE_UNITS          = 0x8872,
		MAX_VARYING_FLOATS               = 0x8B4B,
		MAX_VERTEX_ATTRIBS               = 0x8869,
		MAX_VERTEX_TEXTURE_IMAGE_UNITS   = 0x8B4C,
		MAX_VERTEX_UNIFORM_COMPONENTS    = 0x8B4A,
		POINT_SPRITE_COORD_ORIGIN        = 0x8CA0,
		SAMPLER_1D                       = 0x8B5D,
		SAMPLER_1D_SHADOW                = 0x8B61,
		SAMPLER_2D                       = 0x8B5E,
		SAMPLER_2D_SHADOW                = 0x8B62,
		SAMPLER_3D                       = 0x8B5F,
		SAMPLER_CUBE                     = 0x8B60,
		SHADER_SOURCE_LENGTH             = 0x8B88,
		SHADER_TYPE                      = 0x8B4F,
		SHADING_LANGUAGE_VERSION         = 0x8B8C,
		STENCIL_BACK_FAIL                = 0x8801,
		STENCIL_BACK_FUNC                = 0x8800,
		STENCIL_BACK_PASS_DEPTH_FAIL     = 0x8802,
		STENCIL_BACK_PASS_DEPTH_PASS     = 0x8803,
		STENCIL_BACK_REF                 = 0x8CA3,
		STENCIL_BACK_VALUE_MASK          = 0x8CA4,
		STENCIL_BACK_WRITEMASK           = 0x8CA5,
		UPPER_LEFT                       = 0x8CA2,
		VALIDATE_STATUS                  = 0x8B83,
		VERTEX_ATTRIB_ARRAY_ENABLED      = 0x8622,
		VERTEX_ATTRIB_ARRAY_NORMALIZED   = 0x886A,
		VERTEX_ATTRIB_ARRAY_POINTER      = 0x8645,
		VERTEX_ATTRIB_ARRAY_SIZE         = 0x8623,
		VERTEX_ATTRIB_ARRAY_STRIDE       = 0x8624,
		VERTEX_ATTRIB_ARRAY_TYPE         = 0x8625,
		VERTEX_PROGRAM_POINT_SIZE        = 0x8642,
		VERTEX_SHADER                    = 0x8B31,
	};
	
	namespace _detail
	{
		typedef void (CODEGEN_FUNCPTR * Proc_glAttachShader)(GLuint program, GLuint shader);
		typedef void (CODEGEN_FUNCPTR * Proc_glBindAttribLocation)(GLuint program, GLuint index, const GLchar * name);
		typedef void (CODEGEN_FUNCPTR * Proc_glBlendEquationSeparate)(GLenum modeRGB, GLenum modeAlpha);
		typedef void (CODEGEN_FUNCPTR * Proc_glCompileShader)(GLuint shader);
		typedef GLuint (CODEGEN_FUNCPTR * Proc_glCreateProgram)();
		typedef GLuint (CODEGEN_FUNCPTR * Proc_glCreateShader)(GLenum type);
		typedef void (CODEGEN_FUNCPTR * Proc_glDeleteProgram)(GLuint program);
		typedef void (CODEGEN_FUNCPTR * Proc_glDeleteShader)(GLuint shader);
		typedef void (CODEGEN_FUNCPTR * Proc_glDetachShader)(GLuint program, GLuint shader);
		typedef void (CODEGEN_FUNCPTR * Proc_glDisableVertexAttribArray)(GLuint index);
		typedef void (CODEGEN_FUNCPTR * Proc_glDrawBuffers)(GLsizei n, const GLenum * bufs);
		typedef void (CODEGEN_FUNCPTR * Proc_glEnableVertexAttribArray)(GLuint index);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetActiveAttrib)(GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLint * size, GLenum * type, GLchar * name);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetActiveUniform)(GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLint * size, GLenum * type, GLchar * name);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetAttachedShaders)(GLuint program, GLsizei maxCount, GLsizei * count, GLuint * shaders);
		typedef GLint (CODEGEN_FUNCPTR * Proc_glGetAttribLocation)(GLuint program, const GLchar * name);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetProgramInfoLog)(GLuint program, GLsizei bufSize, GLsizei * length, GLchar * infoLog);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetProgramiv)(GLuint program, GLenum pname, GLint * params);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetShaderInfoLog)(GLuint shader, GLsizei bufSize, GLsizei * length, GLchar * infoLog);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetShaderSource)(GLuint shader, GLsizei bufSize, GLsizei * length, GLchar * source);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetShaderiv)(GLuint shader, GLenum pname, GLint * params);
		typedef GLint (CODEGEN_FUNCPTR * Proc_glGetUniformLocation)(GLuint program, const GLchar * name);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetUniformfv)(GLuint program, GLint location, GLfloat * params);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetUniformiv)(GLuint program, GLint location, GLint * params);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetVertexAttribPointerv)(GLuint index, GLenum pname, GLvoid ** pointer);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetVertexAttribdv)(GLuint index, GLenum pname, GLdouble * params);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetVertexAttribfv)(GLuint index, GLenum pname, GLfloat * params);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetVertexAttribiv)(GLuint index, GLenum pname, GLint * params);
		typedef GLboolean (CODEGEN_FUNCPTR * Proc_glIsProgram)(GLuint program);
		typedef GLboolean (CODEGEN_FUNCPTR * Proc_glIsShader)(GLuint shader);
		typedef void (CODEGEN_FUNCPTR * Proc_glLinkProgram)(GLuint program);
		typedef void (CODEGEN_FUNCPTR * Proc_glShaderSource)(GLuint shader, GLsizei count, const GLchar *const* string, const GLint * length);
		typedef void (CODEGEN_FUNCPTR * Proc_glStencilFuncSeparate)(GLenum face, GLenum func, GLint ref, GLuint mask);
		typedef void (CODEGEN_FUNCPTR * Proc_glStencilMaskSeparate)(GLenum face, GLuint mask);
		typedef void (CODEGEN_FUNCPTR * Proc_glStencilOpSeparate)(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniform1f)(GLint location, GLfloat v0);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniform1fv)(GLint location, GLsizei count, const GLfloat * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniform1i)(GLint location, GLint v0);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniform1iv)(GLint location, GLsizei count, const GLint * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniform2f)(GLint location, GLfloat v0, GLfloat v1);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniform2fv)(GLint location, GLsizei count, const GLfloat * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniform2i)(GLint location, GLint v0, GLint v1);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniform2iv)(GLint location, GLsizei count, const GLint * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniform3f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniform3fv)(GLint location, GLsizei count, const GLfloat * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniform3i)(GLint location, GLint v0, GLint v1, GLint v2);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniform3iv)(GLint location, GLsizei count, const GLint * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniform4f)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniform4fv)(GLint location, GLsizei count, const GLfloat * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniform4i)(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniform4iv)(GLint location, GLsizei count, const GLint * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniformMatrix2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniformMatrix3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniformMatrix4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUseProgram)(GLuint program);
		typedef void (CODEGEN_FUNCPTR * Proc_glValidateProgram)(GLuint program);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib1d)(GLuint index, GLdouble x);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib1dv)(GLuint index, const GLdouble * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib1f)(GLuint index, GLfloat x);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib1fv)(GLuint index, const GLfloat * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib1s)(GLuint index, GLshort x);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib1sv)(GLuint index, const GLshort * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib2d)(GLuint index, GLdouble x, GLdouble y);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib2dv)(GLuint index, const GLdouble * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib2f)(GLuint index, GLfloat x, GLfloat y);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib2fv)(GLuint index, const GLfloat * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib2s)(GLuint index, GLshort x, GLshort y);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib2sv)(GLuint index, const GLshort * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib3d)(GLuint index, GLdouble x, GLdouble y, GLdouble z);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib3dv)(GLuint index, const GLdouble * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib3f)(GLuint index, GLfloat x, GLfloat y, GLfloat z);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib3fv)(GLuint index, const GLfloat * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib3s)(GLuint index, GLshort x, GLshort y, GLshort z);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib3sv)(GLuint index, const GLshort * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib4Nbv)(GLuint index, const GLbyte * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib4Niv)(GLuint index, const GLint * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib4Nsv)(GLuint index, const GLshort * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib4Nub)(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib4Nubv)(GLuint index, const GLubyte * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib4Nuiv)(GLuint index, const GLuint * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib4Nusv)(GLuint index, const GLushort * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib4bv)(GLuint index, const GLbyte * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib4d)(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib4dv)(GLuint index, const GLdouble * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib4f)(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib4fv)(GLuint index, const GLfloat * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib4iv)(GLuint index, const GLint * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib4s)(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib4sv)(GLuint index, const GLshort * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib4ubv)(GLuint index, const GLubyte * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib4uiv)(GLuint index, const GLuint * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttrib4usv)(GLuint index, const GLushort * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glVertexAttribPointer)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer);
	}
	
	extern _detail::Proc_glAttachShader AttachShader;
	extern _detail::Proc_glBindAttribLocation BindAttribLocation;
	extern _detail::Proc_glBlendEquationSeparate BlendEquationSeparate;
	extern _detail::Proc_glCompileShader CompileShader;
	extern _detail::Proc_glCreateProgram CreateProgram;
	extern _detail::Proc_glCreateShader CreateShader;
	extern _detail::Proc_glDeleteProgram DeleteProgram;
	extern _detail::Proc_glDeleteShader DeleteShader;
	extern _detail::Proc_glDetachShader DetachShader;
	extern _detail::Proc_glDisableVertexAttribArray DisableVertexAttribArray;
	extern _detail::Proc_glDrawBuffers DrawBuffers;
	extern _detail::Proc_glEnableVertexAttribArray EnableVertexAttribArray;
	extern _detail::Proc_glGetActiveAttrib GetActiveAttrib;
	extern _detail::Proc_glGetActiveUniform GetActiveUniform;
	extern _detail::Proc_glGetAttachedShaders GetAttachedShaders;
	extern _detail::Proc_glGetAttribLocation GetAttribLocation;
	extern _detail::Proc_glGetProgramInfoLog GetProgramInfoLog;
	extern _detail::Proc_glGetProgramiv GetProgramiv;
	extern _detail::Proc_glGetShaderInfoLog GetShaderInfoLog;
	extern _detail::Proc_glGetShaderSource GetShaderSource;
	extern _detail::Proc_glGetShaderiv GetShaderiv;
	extern _detail::Proc_glGetUniformLocation GetUniformLocation;
	extern _detail::Proc_glGetUniformfv GetUniformfv;
	extern _detail::Proc_glGetUniformiv GetUniformiv;
	extern _detail::Proc_glGetVertexAttribPointerv GetVertexAttribPointerv;
	extern _detail::Proc_glGetVertexAttribdv GetVertexAttribdv;
	extern _detail::Proc_glGetVertexAttribfv GetVertexAttribfv;
	extern _detail::Proc_glGetVertexAttribiv GetVertexAttribiv;
	extern _detail::Proc_glIsProgram IsProgram;
	extern _detail::Proc_glIsShader IsShader;
	extern _detail::Proc_glLinkProgram LinkProgram;
	extern _detail::Proc_glShaderSource ShaderSource;
	extern _detail::Proc_glStencilFuncSeparate StencilFuncSeparate;
	extern _detail::Proc_glStencilMaskSeparate StencilMaskSeparate;
	extern _detail::Proc_glStencilOpSeparate StencilOpSeparate;
	extern _detail::Proc_glUniform1f Uniform1f;
	extern _detail::Proc_glUniform1fv Uniform1fv;
	extern _detail::Proc_glUniform1i Uniform1i;
	extern _detail::Proc_glUniform1iv Uniform1iv;
	extern _detail::Proc_glUniform2f Uniform2f;
	extern _detail::Proc_glUniform2fv Uniform2fv;
	extern _detail::Proc_glUniform2i Uniform2i;
	extern _detail::Proc_glUniform2iv Uniform2iv;
	extern _detail::Proc_glUniform3f Uniform3f;
	extern _detail::Proc_glUniform3fv Uniform3fv;
	extern _detail::Proc_glUniform3i Uniform3i;
	extern _detail::Proc_glUniform3iv Uniform3iv;
	extern _detail::Proc_glUniform4f Uniform4f;
	extern _detail::Proc_glUniform4fv Uniform4fv;
	extern _detail::Proc_glUniform4i Uniform4i;
	extern _detail::Proc_glUniform4iv Uniform4iv;
	extern _detail::Proc_glUniformMatrix2fv UniformMatrix2fv;
	extern _detail::Proc_glUniformMatrix3fv UniformMatrix3fv;
	extern _detail::Proc_glUniformMatrix4fv UniformMatrix4fv;
	extern _detail::Proc_glUseProgram UseProgram;
	extern _detail::Proc_glValidateProgram ValidateProgram;
	extern _detail::Proc_glVertexAttrib1d VertexAttrib1d;
	extern _detail::Proc_glVertexAttrib1dv VertexAttrib1dv;
	extern _detail::Proc_glVertexAttrib1f VertexAttrib1f;
	extern _detail::Proc_glVertexAttrib1fv VertexAttrib1fv;
	extern _detail::Proc_glVertexAttrib1s VertexAttrib1s;
	extern _detail::Proc_glVertexAttrib1sv VertexAttrib1sv;
	extern _detail::Proc_glVertexAttrib2d VertexAttrib2d;
	extern _detail::Proc_glVertexAttrib2dv VertexAttrib2dv;
	extern _detail::Proc_glVertexAttrib2f VertexAttrib2f;
	extern _detail::Proc_glVertexAttrib2fv VertexAttrib2fv;
	extern _detail::Proc_glVertexAttrib2s VertexAttrib2s;
	extern _detail::Proc_glVertexAttrib2sv VertexAttrib2sv;
	extern _detail::Proc_glVertexAttrib3d VertexAttrib3d;
	extern _detail::Proc_glVertexAttrib3dv VertexAttrib3dv;
	extern _detail::Proc_glVertexAttrib3f VertexAttrib3f;
	extern _detail::Proc_glVertexAttrib3fv VertexAttrib3fv;
	extern _detail::Proc_glVertexAttrib3s VertexAttrib3s;
	extern _detail::Proc_glVertexAttrib3sv VertexAttrib3sv;
	extern _detail::Proc_glVertexAttrib4Nbv VertexAttrib4Nbv;
	extern _detail::Proc_glVertexAttrib4Niv VertexAttrib4Niv;
	extern _detail::Proc_glVertexAttrib4Nsv VertexAttrib4Nsv;
	extern _detail::Proc_glVertexAttrib4Nub VertexAttrib4Nub;
	extern _detail::Proc_glVertexAttrib4Nubv VertexAttrib4Nubv;
	extern _detail::Proc_glVertexAttrib4Nuiv VertexAttrib4Nuiv;
	extern _detail::Proc_glVertexAttrib4Nusv VertexAttrib4Nusv;
	extern _detail::Proc_glVertexAttrib4bv VertexAttrib4bv;
	extern _detail::Proc_glVertexAttrib4d VertexAttrib4d;
	extern _detail::Proc_glVertexAttrib4dv VertexAttrib4dv;
	extern _detail::Proc_glVertexAttrib4f VertexAttrib4f;
	extern _detail::Proc_glVertexAttrib4fv VertexAttrib4fv;
	extern _detail::Proc_glVertexAttrib4iv VertexAttrib4iv;
	extern _detail::Proc_glVertexAttrib4s VertexAttrib4s;
	extern _detail::Proc_glVertexAttrib4sv VertexAttrib4sv;
	extern _detail::Proc_glVertexAttrib4ubv VertexAttrib4ubv;
	extern _detail::Proc_glVertexAttrib4uiv VertexAttrib4uiv;
	extern _detail::Proc_glVertexAttrib4usv VertexAttrib4usv;
	extern _detail::Proc_glVertexAttribPointer VertexAttribPointer;
	
}
#endif /*OPENGL_GEN_CORE_2_0_HPP*/
