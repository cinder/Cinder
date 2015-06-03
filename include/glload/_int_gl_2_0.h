#ifndef OPENGL_GEN_CORE_2_0_H
#define OPENGL_GEN_CORE_2_0_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
#define GL_ACTIVE_ATTRIBUTES 0x8B89
#define GL_ACTIVE_ATTRIBUTE_MAX_LENGTH 0x8B8A
#define GL_ACTIVE_UNIFORMS 0x8B86
#define GL_ACTIVE_UNIFORM_MAX_LENGTH 0x8B87
#define GL_ATTACHED_SHADERS 0x8B85
#define GL_BLEND_EQUATION_ALPHA 0x883D
#define GL_BLEND_EQUATION_RGB 0x8009
#define GL_BOOL 0x8B56
#define GL_BOOL_VEC2 0x8B57
#define GL_BOOL_VEC3 0x8B58
#define GL_BOOL_VEC4 0x8B59
#define GL_COMPILE_STATUS 0x8B81
#define GL_CURRENT_PROGRAM 0x8B8D
#define GL_CURRENT_VERTEX_ATTRIB 0x8626
#define GL_DELETE_STATUS 0x8B80
#define GL_DRAW_BUFFER0 0x8825
#define GL_DRAW_BUFFER1 0x8826
#define GL_DRAW_BUFFER10 0x882F
#define GL_DRAW_BUFFER11 0x8830
#define GL_DRAW_BUFFER12 0x8831
#define GL_DRAW_BUFFER13 0x8832
#define GL_DRAW_BUFFER14 0x8833
#define GL_DRAW_BUFFER15 0x8834
#define GL_DRAW_BUFFER2 0x8827
#define GL_DRAW_BUFFER3 0x8828
#define GL_DRAW_BUFFER4 0x8829
#define GL_DRAW_BUFFER5 0x882A
#define GL_DRAW_BUFFER6 0x882B
#define GL_DRAW_BUFFER7 0x882C
#define GL_DRAW_BUFFER8 0x882D
#define GL_DRAW_BUFFER9 0x882E
#define GL_FLOAT_MAT2 0x8B5A
#define GL_FLOAT_MAT3 0x8B5B
#define GL_FLOAT_MAT4 0x8B5C
#define GL_FLOAT_VEC2 0x8B50
#define GL_FLOAT_VEC3 0x8B51
#define GL_FLOAT_VEC4 0x8B52
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_FRAGMENT_SHADER_DERIVATIVE_HINT 0x8B8B
#define GL_INFO_LOG_LENGTH 0x8B84
#define GL_INT_VEC2 0x8B53
#define GL_INT_VEC3 0x8B54
#define GL_INT_VEC4 0x8B55
#define GL_LINK_STATUS 0x8B82
#define GL_LOWER_LEFT 0x8CA1
#define GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS 0x8B4D
#define GL_MAX_DRAW_BUFFERS 0x8824
#define GL_MAX_FRAGMENT_UNIFORM_COMPONENTS 0x8B49
#define GL_MAX_TEXTURE_IMAGE_UNITS 0x8872
#define GL_MAX_VARYING_FLOATS 0x8B4B
#define GL_MAX_VERTEX_ATTRIBS 0x8869
#define GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS 0x8B4C
#define GL_MAX_VERTEX_UNIFORM_COMPONENTS 0x8B4A
#define GL_POINT_SPRITE_COORD_ORIGIN 0x8CA0
#define GL_SAMPLER_1D 0x8B5D
#define GL_SAMPLER_1D_SHADOW 0x8B61
#define GL_SAMPLER_2D 0x8B5E
#define GL_SAMPLER_2D_SHADOW 0x8B62
#define GL_SAMPLER_3D 0x8B5F
#define GL_SAMPLER_CUBE 0x8B60
#define GL_SHADER_SOURCE_LENGTH 0x8B88
#define GL_SHADER_TYPE 0x8B4F
#define GL_SHADING_LANGUAGE_VERSION 0x8B8C
#define GL_STENCIL_BACK_FAIL 0x8801
#define GL_STENCIL_BACK_FUNC 0x8800
#define GL_STENCIL_BACK_PASS_DEPTH_FAIL 0x8802
#define GL_STENCIL_BACK_PASS_DEPTH_PASS 0x8803
#define GL_STENCIL_BACK_REF 0x8CA3
#define GL_STENCIL_BACK_VALUE_MASK 0x8CA4
#define GL_STENCIL_BACK_WRITEMASK 0x8CA5
#define GL_UPPER_LEFT 0x8CA2
#define GL_VALIDATE_STATUS 0x8B83
#define GL_VERTEX_ATTRIB_ARRAY_ENABLED 0x8622
#define GL_VERTEX_ATTRIB_ARRAY_NORMALIZED 0x886A
#define GL_VERTEX_ATTRIB_ARRAY_POINTER 0x8645
#define GL_VERTEX_ATTRIB_ARRAY_SIZE 0x8623
#define GL_VERTEX_ATTRIB_ARRAY_STRIDE 0x8624
#define GL_VERTEX_ATTRIB_ARRAY_TYPE 0x8625
#define GL_VERTEX_PROGRAM_POINT_SIZE 0x8642
#define GL_VERTEX_SHADER 0x8B31

typedef void (CODEGEN_FUNCPTR * PFNGLATTACHSHADERPROC)(GLuint program, GLuint shader);
typedef void (CODEGEN_FUNCPTR * PFNGLBINDATTRIBLOCATIONPROC)(GLuint program, GLuint index, const GLchar * name);
typedef void (CODEGEN_FUNCPTR * PFNGLBLENDEQUATIONSEPARATEPROC)(GLenum modeRGB, GLenum modeAlpha);
typedef void (CODEGEN_FUNCPTR * PFNGLCOMPILESHADERPROC)(GLuint shader);
typedef GLuint (CODEGEN_FUNCPTR * PFNGLCREATEPROGRAMPROC)();
typedef GLuint (CODEGEN_FUNCPTR * PFNGLCREATESHADERPROC)(GLenum type);
typedef void (CODEGEN_FUNCPTR * PFNGLDELETEPROGRAMPROC)(GLuint program);
typedef void (CODEGEN_FUNCPTR * PFNGLDELETESHADERPROC)(GLuint shader);
typedef void (CODEGEN_FUNCPTR * PFNGLDETACHSHADERPROC)(GLuint program, GLuint shader);
typedef void (CODEGEN_FUNCPTR * PFNGLDISABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void (CODEGEN_FUNCPTR * PFNGLDRAWBUFFERSPROC)(GLsizei n, const GLenum * bufs);
typedef void (CODEGEN_FUNCPTR * PFNGLENABLEVERTEXATTRIBARRAYPROC)(GLuint index);
typedef void (CODEGEN_FUNCPTR * PFNGLGETACTIVEATTRIBPROC)(GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLint * size, GLenum * type, GLchar * name);
typedef void (CODEGEN_FUNCPTR * PFNGLGETACTIVEUNIFORMPROC)(GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLint * size, GLenum * type, GLchar * name);
typedef void (CODEGEN_FUNCPTR * PFNGLGETATTACHEDSHADERSPROC)(GLuint program, GLsizei maxCount, GLsizei * count, GLuint * shaders);
typedef GLint (CODEGEN_FUNCPTR * PFNGLGETATTRIBLOCATIONPROC)(GLuint program, const GLchar * name);
typedef void (CODEGEN_FUNCPTR * PFNGLGETPROGRAMINFOLOGPROC)(GLuint program, GLsizei bufSize, GLsizei * length, GLchar * infoLog);
typedef void (CODEGEN_FUNCPTR * PFNGLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei bufSize, GLsizei * length, GLchar * infoLog);
typedef void (CODEGEN_FUNCPTR * PFNGLGETSHADERSOURCEPROC)(GLuint shader, GLsizei bufSize, GLsizei * length, GLchar * source);
typedef void (CODEGEN_FUNCPTR * PFNGLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint * params);
typedef GLint (CODEGEN_FUNCPTR * PFNGLGETUNIFORMLOCATIONPROC)(GLuint program, const GLchar * name);
typedef void (CODEGEN_FUNCPTR * PFNGLGETUNIFORMFVPROC)(GLuint program, GLint location, GLfloat * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETUNIFORMIVPROC)(GLuint program, GLint location, GLint * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETVERTEXATTRIBPOINTERVPROC)(GLuint index, GLenum pname, GLvoid ** pointer);
typedef void (CODEGEN_FUNCPTR * PFNGLGETVERTEXATTRIBDVPROC)(GLuint index, GLenum pname, GLdouble * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETVERTEXATTRIBFVPROC)(GLuint index, GLenum pname, GLfloat * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETVERTEXATTRIBIVPROC)(GLuint index, GLenum pname, GLint * params);
typedef GLboolean (CODEGEN_FUNCPTR * PFNGLISPROGRAMPROC)(GLuint program);
typedef GLboolean (CODEGEN_FUNCPTR * PFNGLISSHADERPROC)(GLuint shader);
typedef void (CODEGEN_FUNCPTR * PFNGLLINKPROGRAMPROC)(GLuint program);
typedef void (CODEGEN_FUNCPTR * PFNGLSHADERSOURCEPROC)(GLuint shader, GLsizei count, const GLchar *const* string, const GLint * length);
typedef void (CODEGEN_FUNCPTR * PFNGLSTENCILFUNCSEPARATEPROC)(GLenum face, GLenum func, GLint ref, GLuint mask);
typedef void (CODEGEN_FUNCPTR * PFNGLSTENCILMASKSEPARATEPROC)(GLenum face, GLuint mask);
typedef void (CODEGEN_FUNCPTR * PFNGLSTENCILOPSEPARATEPROC)(GLenum face, GLenum sfail, GLenum dpfail, GLenum dppass);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORM1FPROC)(GLint location, GLfloat v0);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORM1FVPROC)(GLint location, GLsizei count, const GLfloat * value);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORM1IPROC)(GLint location, GLint v0);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORM1IVPROC)(GLint location, GLsizei count, const GLint * value);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORM2FPROC)(GLint location, GLfloat v0, GLfloat v1);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORM2FVPROC)(GLint location, GLsizei count, const GLfloat * value);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORM2IPROC)(GLint location, GLint v0, GLint v1);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORM2IVPROC)(GLint location, GLsizei count, const GLint * value);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORM3FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORM3FVPROC)(GLint location, GLsizei count, const GLfloat * value);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORM3IPROC)(GLint location, GLint v0, GLint v1, GLint v2);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORM3IVPROC)(GLint location, GLsizei count, const GLint * value);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORM4FPROC)(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORM4FVPROC)(GLint location, GLsizei count, const GLfloat * value);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORM4IPROC)(GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORM4IVPROC)(GLint location, GLsizei count, const GLint * value);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORMMATRIX2FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORMMATRIX3FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORMMATRIX4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
typedef void (CODEGEN_FUNCPTR * PFNGLUSEPROGRAMPROC)(GLuint program);
typedef void (CODEGEN_FUNCPTR * PFNGLVALIDATEPROGRAMPROC)(GLuint program);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB1DPROC)(GLuint index, GLdouble x);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB1DVPROC)(GLuint index, const GLdouble * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB1FPROC)(GLuint index, GLfloat x);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB1FVPROC)(GLuint index, const GLfloat * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB1SPROC)(GLuint index, GLshort x);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB1SVPROC)(GLuint index, const GLshort * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB2DPROC)(GLuint index, GLdouble x, GLdouble y);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB2DVPROC)(GLuint index, const GLdouble * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB2FPROC)(GLuint index, GLfloat x, GLfloat y);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB2FVPROC)(GLuint index, const GLfloat * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB2SPROC)(GLuint index, GLshort x, GLshort y);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB2SVPROC)(GLuint index, const GLshort * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB3DPROC)(GLuint index, GLdouble x, GLdouble y, GLdouble z);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB3DVPROC)(GLuint index, const GLdouble * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB3FPROC)(GLuint index, GLfloat x, GLfloat y, GLfloat z);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB3FVPROC)(GLuint index, const GLfloat * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB3SPROC)(GLuint index, GLshort x, GLshort y, GLshort z);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB3SVPROC)(GLuint index, const GLshort * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB4NBVPROC)(GLuint index, const GLbyte * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB4NIVPROC)(GLuint index, const GLint * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB4NSVPROC)(GLuint index, const GLshort * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB4NUBPROC)(GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB4NUBVPROC)(GLuint index, const GLubyte * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB4NUIVPROC)(GLuint index, const GLuint * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB4NUSVPROC)(GLuint index, const GLushort * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB4BVPROC)(GLuint index, const GLbyte * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB4DPROC)(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB4DVPROC)(GLuint index, const GLdouble * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB4FPROC)(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB4FVPROC)(GLuint index, const GLfloat * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB4IVPROC)(GLuint index, const GLint * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB4SPROC)(GLuint index, GLshort x, GLshort y, GLshort z, GLshort w);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB4SVPROC)(GLuint index, const GLshort * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB4UBVPROC)(GLuint index, const GLubyte * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB4UIVPROC)(GLuint index, const GLuint * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIB4USVPROC)(GLuint index, const GLushort * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer);

extern PFNGLATTACHSHADERPROC _funcptr_glAttachShader;
#define glAttachShader _funcptr_glAttachShader
extern PFNGLBINDATTRIBLOCATIONPROC _funcptr_glBindAttribLocation;
#define glBindAttribLocation _funcptr_glBindAttribLocation
extern PFNGLBLENDEQUATIONSEPARATEPROC _funcptr_glBlendEquationSeparate;
#define glBlendEquationSeparate _funcptr_glBlendEquationSeparate
extern PFNGLCOMPILESHADERPROC _funcptr_glCompileShader;
#define glCompileShader _funcptr_glCompileShader
extern PFNGLCREATEPROGRAMPROC _funcptr_glCreateProgram;
#define glCreateProgram _funcptr_glCreateProgram
extern PFNGLCREATESHADERPROC _funcptr_glCreateShader;
#define glCreateShader _funcptr_glCreateShader
extern PFNGLDELETEPROGRAMPROC _funcptr_glDeleteProgram;
#define glDeleteProgram _funcptr_glDeleteProgram
extern PFNGLDELETESHADERPROC _funcptr_glDeleteShader;
#define glDeleteShader _funcptr_glDeleteShader
extern PFNGLDETACHSHADERPROC _funcptr_glDetachShader;
#define glDetachShader _funcptr_glDetachShader
extern PFNGLDISABLEVERTEXATTRIBARRAYPROC _funcptr_glDisableVertexAttribArray;
#define glDisableVertexAttribArray _funcptr_glDisableVertexAttribArray
extern PFNGLDRAWBUFFERSPROC _funcptr_glDrawBuffers;
#define glDrawBuffers _funcptr_glDrawBuffers
extern PFNGLENABLEVERTEXATTRIBARRAYPROC _funcptr_glEnableVertexAttribArray;
#define glEnableVertexAttribArray _funcptr_glEnableVertexAttribArray
extern PFNGLGETACTIVEATTRIBPROC _funcptr_glGetActiveAttrib;
#define glGetActiveAttrib _funcptr_glGetActiveAttrib
extern PFNGLGETACTIVEUNIFORMPROC _funcptr_glGetActiveUniform;
#define glGetActiveUniform _funcptr_glGetActiveUniform
extern PFNGLGETATTACHEDSHADERSPROC _funcptr_glGetAttachedShaders;
#define glGetAttachedShaders _funcptr_glGetAttachedShaders
extern PFNGLGETATTRIBLOCATIONPROC _funcptr_glGetAttribLocation;
#define glGetAttribLocation _funcptr_glGetAttribLocation
extern PFNGLGETPROGRAMINFOLOGPROC _funcptr_glGetProgramInfoLog;
#define glGetProgramInfoLog _funcptr_glGetProgramInfoLog
extern PFNGLGETPROGRAMIVPROC _funcptr_glGetProgramiv;
#define glGetProgramiv _funcptr_glGetProgramiv
extern PFNGLGETSHADERINFOLOGPROC _funcptr_glGetShaderInfoLog;
#define glGetShaderInfoLog _funcptr_glGetShaderInfoLog
extern PFNGLGETSHADERSOURCEPROC _funcptr_glGetShaderSource;
#define glGetShaderSource _funcptr_glGetShaderSource
extern PFNGLGETSHADERIVPROC _funcptr_glGetShaderiv;
#define glGetShaderiv _funcptr_glGetShaderiv
extern PFNGLGETUNIFORMLOCATIONPROC _funcptr_glGetUniformLocation;
#define glGetUniformLocation _funcptr_glGetUniformLocation
extern PFNGLGETUNIFORMFVPROC _funcptr_glGetUniformfv;
#define glGetUniformfv _funcptr_glGetUniformfv
extern PFNGLGETUNIFORMIVPROC _funcptr_glGetUniformiv;
#define glGetUniformiv _funcptr_glGetUniformiv
extern PFNGLGETVERTEXATTRIBPOINTERVPROC _funcptr_glGetVertexAttribPointerv;
#define glGetVertexAttribPointerv _funcptr_glGetVertexAttribPointerv
extern PFNGLGETVERTEXATTRIBDVPROC _funcptr_glGetVertexAttribdv;
#define glGetVertexAttribdv _funcptr_glGetVertexAttribdv
extern PFNGLGETVERTEXATTRIBFVPROC _funcptr_glGetVertexAttribfv;
#define glGetVertexAttribfv _funcptr_glGetVertexAttribfv
extern PFNGLGETVERTEXATTRIBIVPROC _funcptr_glGetVertexAttribiv;
#define glGetVertexAttribiv _funcptr_glGetVertexAttribiv
extern PFNGLISPROGRAMPROC _funcptr_glIsProgram;
#define glIsProgram _funcptr_glIsProgram
extern PFNGLISSHADERPROC _funcptr_glIsShader;
#define glIsShader _funcptr_glIsShader
extern PFNGLLINKPROGRAMPROC _funcptr_glLinkProgram;
#define glLinkProgram _funcptr_glLinkProgram
extern PFNGLSHADERSOURCEPROC _funcptr_glShaderSource;
#define glShaderSource _funcptr_glShaderSource
extern PFNGLSTENCILFUNCSEPARATEPROC _funcptr_glStencilFuncSeparate;
#define glStencilFuncSeparate _funcptr_glStencilFuncSeparate
extern PFNGLSTENCILMASKSEPARATEPROC _funcptr_glStencilMaskSeparate;
#define glStencilMaskSeparate _funcptr_glStencilMaskSeparate
extern PFNGLSTENCILOPSEPARATEPROC _funcptr_glStencilOpSeparate;
#define glStencilOpSeparate _funcptr_glStencilOpSeparate
extern PFNGLUNIFORM1FPROC _funcptr_glUniform1f;
#define glUniform1f _funcptr_glUniform1f
extern PFNGLUNIFORM1FVPROC _funcptr_glUniform1fv;
#define glUniform1fv _funcptr_glUniform1fv
extern PFNGLUNIFORM1IPROC _funcptr_glUniform1i;
#define glUniform1i _funcptr_glUniform1i
extern PFNGLUNIFORM1IVPROC _funcptr_glUniform1iv;
#define glUniform1iv _funcptr_glUniform1iv
extern PFNGLUNIFORM2FPROC _funcptr_glUniform2f;
#define glUniform2f _funcptr_glUniform2f
extern PFNGLUNIFORM2FVPROC _funcptr_glUniform2fv;
#define glUniform2fv _funcptr_glUniform2fv
extern PFNGLUNIFORM2IPROC _funcptr_glUniform2i;
#define glUniform2i _funcptr_glUniform2i
extern PFNGLUNIFORM2IVPROC _funcptr_glUniform2iv;
#define glUniform2iv _funcptr_glUniform2iv
extern PFNGLUNIFORM3FPROC _funcptr_glUniform3f;
#define glUniform3f _funcptr_glUniform3f
extern PFNGLUNIFORM3FVPROC _funcptr_glUniform3fv;
#define glUniform3fv _funcptr_glUniform3fv
extern PFNGLUNIFORM3IPROC _funcptr_glUniform3i;
#define glUniform3i _funcptr_glUniform3i
extern PFNGLUNIFORM3IVPROC _funcptr_glUniform3iv;
#define glUniform3iv _funcptr_glUniform3iv
extern PFNGLUNIFORM4FPROC _funcptr_glUniform4f;
#define glUniform4f _funcptr_glUniform4f
extern PFNGLUNIFORM4FVPROC _funcptr_glUniform4fv;
#define glUniform4fv _funcptr_glUniform4fv
extern PFNGLUNIFORM4IPROC _funcptr_glUniform4i;
#define glUniform4i _funcptr_glUniform4i
extern PFNGLUNIFORM4IVPROC _funcptr_glUniform4iv;
#define glUniform4iv _funcptr_glUniform4iv
extern PFNGLUNIFORMMATRIX2FVPROC _funcptr_glUniformMatrix2fv;
#define glUniformMatrix2fv _funcptr_glUniformMatrix2fv
extern PFNGLUNIFORMMATRIX3FVPROC _funcptr_glUniformMatrix3fv;
#define glUniformMatrix3fv _funcptr_glUniformMatrix3fv
extern PFNGLUNIFORMMATRIX4FVPROC _funcptr_glUniformMatrix4fv;
#define glUniformMatrix4fv _funcptr_glUniformMatrix4fv
extern PFNGLUSEPROGRAMPROC _funcptr_glUseProgram;
#define glUseProgram _funcptr_glUseProgram
extern PFNGLVALIDATEPROGRAMPROC _funcptr_glValidateProgram;
#define glValidateProgram _funcptr_glValidateProgram
extern PFNGLVERTEXATTRIB1DPROC _funcptr_glVertexAttrib1d;
#define glVertexAttrib1d _funcptr_glVertexAttrib1d
extern PFNGLVERTEXATTRIB1DVPROC _funcptr_glVertexAttrib1dv;
#define glVertexAttrib1dv _funcptr_glVertexAttrib1dv
extern PFNGLVERTEXATTRIB1FPROC _funcptr_glVertexAttrib1f;
#define glVertexAttrib1f _funcptr_glVertexAttrib1f
extern PFNGLVERTEXATTRIB1FVPROC _funcptr_glVertexAttrib1fv;
#define glVertexAttrib1fv _funcptr_glVertexAttrib1fv
extern PFNGLVERTEXATTRIB1SPROC _funcptr_glVertexAttrib1s;
#define glVertexAttrib1s _funcptr_glVertexAttrib1s
extern PFNGLVERTEXATTRIB1SVPROC _funcptr_glVertexAttrib1sv;
#define glVertexAttrib1sv _funcptr_glVertexAttrib1sv
extern PFNGLVERTEXATTRIB2DPROC _funcptr_glVertexAttrib2d;
#define glVertexAttrib2d _funcptr_glVertexAttrib2d
extern PFNGLVERTEXATTRIB2DVPROC _funcptr_glVertexAttrib2dv;
#define glVertexAttrib2dv _funcptr_glVertexAttrib2dv
extern PFNGLVERTEXATTRIB2FPROC _funcptr_glVertexAttrib2f;
#define glVertexAttrib2f _funcptr_glVertexAttrib2f
extern PFNGLVERTEXATTRIB2FVPROC _funcptr_glVertexAttrib2fv;
#define glVertexAttrib2fv _funcptr_glVertexAttrib2fv
extern PFNGLVERTEXATTRIB2SPROC _funcptr_glVertexAttrib2s;
#define glVertexAttrib2s _funcptr_glVertexAttrib2s
extern PFNGLVERTEXATTRIB2SVPROC _funcptr_glVertexAttrib2sv;
#define glVertexAttrib2sv _funcptr_glVertexAttrib2sv
extern PFNGLVERTEXATTRIB3DPROC _funcptr_glVertexAttrib3d;
#define glVertexAttrib3d _funcptr_glVertexAttrib3d
extern PFNGLVERTEXATTRIB3DVPROC _funcptr_glVertexAttrib3dv;
#define glVertexAttrib3dv _funcptr_glVertexAttrib3dv
extern PFNGLVERTEXATTRIB3FPROC _funcptr_glVertexAttrib3f;
#define glVertexAttrib3f _funcptr_glVertexAttrib3f
extern PFNGLVERTEXATTRIB3FVPROC _funcptr_glVertexAttrib3fv;
#define glVertexAttrib3fv _funcptr_glVertexAttrib3fv
extern PFNGLVERTEXATTRIB3SPROC _funcptr_glVertexAttrib3s;
#define glVertexAttrib3s _funcptr_glVertexAttrib3s
extern PFNGLVERTEXATTRIB3SVPROC _funcptr_glVertexAttrib3sv;
#define glVertexAttrib3sv _funcptr_glVertexAttrib3sv
extern PFNGLVERTEXATTRIB4NBVPROC _funcptr_glVertexAttrib4Nbv;
#define glVertexAttrib4Nbv _funcptr_glVertexAttrib4Nbv
extern PFNGLVERTEXATTRIB4NIVPROC _funcptr_glVertexAttrib4Niv;
#define glVertexAttrib4Niv _funcptr_glVertexAttrib4Niv
extern PFNGLVERTEXATTRIB4NSVPROC _funcptr_glVertexAttrib4Nsv;
#define glVertexAttrib4Nsv _funcptr_glVertexAttrib4Nsv
extern PFNGLVERTEXATTRIB4NUBPROC _funcptr_glVertexAttrib4Nub;
#define glVertexAttrib4Nub _funcptr_glVertexAttrib4Nub
extern PFNGLVERTEXATTRIB4NUBVPROC _funcptr_glVertexAttrib4Nubv;
#define glVertexAttrib4Nubv _funcptr_glVertexAttrib4Nubv
extern PFNGLVERTEXATTRIB4NUIVPROC _funcptr_glVertexAttrib4Nuiv;
#define glVertexAttrib4Nuiv _funcptr_glVertexAttrib4Nuiv
extern PFNGLVERTEXATTRIB4NUSVPROC _funcptr_glVertexAttrib4Nusv;
#define glVertexAttrib4Nusv _funcptr_glVertexAttrib4Nusv
extern PFNGLVERTEXATTRIB4BVPROC _funcptr_glVertexAttrib4bv;
#define glVertexAttrib4bv _funcptr_glVertexAttrib4bv
extern PFNGLVERTEXATTRIB4DPROC _funcptr_glVertexAttrib4d;
#define glVertexAttrib4d _funcptr_glVertexAttrib4d
extern PFNGLVERTEXATTRIB4DVPROC _funcptr_glVertexAttrib4dv;
#define glVertexAttrib4dv _funcptr_glVertexAttrib4dv
extern PFNGLVERTEXATTRIB4FPROC _funcptr_glVertexAttrib4f;
#define glVertexAttrib4f _funcptr_glVertexAttrib4f
extern PFNGLVERTEXATTRIB4FVPROC _funcptr_glVertexAttrib4fv;
#define glVertexAttrib4fv _funcptr_glVertexAttrib4fv
extern PFNGLVERTEXATTRIB4IVPROC _funcptr_glVertexAttrib4iv;
#define glVertexAttrib4iv _funcptr_glVertexAttrib4iv
extern PFNGLVERTEXATTRIB4SPROC _funcptr_glVertexAttrib4s;
#define glVertexAttrib4s _funcptr_glVertexAttrib4s
extern PFNGLVERTEXATTRIB4SVPROC _funcptr_glVertexAttrib4sv;
#define glVertexAttrib4sv _funcptr_glVertexAttrib4sv
extern PFNGLVERTEXATTRIB4UBVPROC _funcptr_glVertexAttrib4ubv;
#define glVertexAttrib4ubv _funcptr_glVertexAttrib4ubv
extern PFNGLVERTEXATTRIB4UIVPROC _funcptr_glVertexAttrib4uiv;
#define glVertexAttrib4uiv _funcptr_glVertexAttrib4uiv
extern PFNGLVERTEXATTRIB4USVPROC _funcptr_glVertexAttrib4usv;
#define glVertexAttrib4usv _funcptr_glVertexAttrib4usv
extern PFNGLVERTEXATTRIBPOINTERPROC _funcptr_glVertexAttribPointer;
#define glVertexAttribPointer _funcptr_glVertexAttribPointer

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif /*OPENGL_GEN_CORE_2_0_H*/
