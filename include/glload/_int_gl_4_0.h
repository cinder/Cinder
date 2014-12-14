#ifndef OPENGL_GEN_CORE_4_0_H
#define OPENGL_GEN_CORE_4_0_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
#define GL_INT_SAMPLER_CUBE_MAP_ARRAY 0x900E
#define GL_MAX_PROGRAM_TEXTURE_GATHER_OFFSET 0x8E5F
#define GL_MIN_PROGRAM_TEXTURE_GATHER_OFFSET 0x8E5E
#define GL_MIN_SAMPLE_SHADING_VALUE 0x8C37
#define GL_PROXY_TEXTURE_CUBE_MAP_ARRAY 0x900B
#define GL_SAMPLER_CUBE_MAP_ARRAY 0x900C
#define GL_SAMPLER_CUBE_MAP_ARRAY_SHADOW 0x900D
#define GL_SAMPLE_SHADING 0x8C36
#define GL_TEXTURE_BINDING_CUBE_MAP_ARRAY 0x900A
#define GL_UNSIGNED_INT_SAMPLER_CUBE_MAP_ARRAY 0x900F

typedef void (CODEGEN_FUNCPTR * PFNGLBEGINQUERYINDEXEDPROC)(GLenum target, GLuint index, GLuint id);
typedef void (CODEGEN_FUNCPTR * PFNGLBINDTRANSFORMFEEDBACKPROC)(GLenum target, GLuint id);
typedef void (CODEGEN_FUNCPTR * PFNGLBLENDEQUATIONSEPARATEIPROC)(GLuint buf, GLenum modeRGB, GLenum modeAlpha);
typedef void (CODEGEN_FUNCPTR * PFNGLBLENDEQUATIONIPROC)(GLuint buf, GLenum mode);
typedef void (CODEGEN_FUNCPTR * PFNGLBLENDFUNCSEPARATEIPROC)(GLuint buf, GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);
typedef void (CODEGEN_FUNCPTR * PFNGLBLENDFUNCIPROC)(GLuint buf, GLenum src, GLenum dst);
typedef void (CODEGEN_FUNCPTR * PFNGLDELETETRANSFORMFEEDBACKSPROC)(GLsizei n, const GLuint * ids);
typedef void (CODEGEN_FUNCPTR * PFNGLDRAWARRAYSINDIRECTPROC)(GLenum mode, const GLvoid * indirect);
typedef void (CODEGEN_FUNCPTR * PFNGLDRAWELEMENTSINDIRECTPROC)(GLenum mode, GLenum type, const GLvoid * indirect);
typedef void (CODEGEN_FUNCPTR * PFNGLDRAWTRANSFORMFEEDBACKPROC)(GLenum mode, GLuint id);
typedef void (CODEGEN_FUNCPTR * PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC)(GLenum mode, GLuint id, GLuint stream);
typedef void (CODEGEN_FUNCPTR * PFNGLENDQUERYINDEXEDPROC)(GLenum target, GLuint index);
typedef void (CODEGEN_FUNCPTR * PFNGLGENTRANSFORMFEEDBACKSPROC)(GLsizei n, GLuint * ids);
typedef void (CODEGEN_FUNCPTR * PFNGLGETACTIVESUBROUTINENAMEPROC)(GLuint program, GLenum shadertype, GLuint index, GLsizei bufsize, GLsizei * length, GLchar * name);
typedef void (CODEGEN_FUNCPTR * PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC)(GLuint program, GLenum shadertype, GLuint index, GLsizei bufsize, GLsizei * length, GLchar * name);
typedef void (CODEGEN_FUNCPTR * PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC)(GLuint program, GLenum shadertype, GLuint index, GLenum pname, GLint * values);
typedef void (CODEGEN_FUNCPTR * PFNGLGETPROGRAMSTAGEIVPROC)(GLuint program, GLenum shadertype, GLenum pname, GLint * values);
typedef void (CODEGEN_FUNCPTR * PFNGLGETQUERYINDEXEDIVPROC)(GLenum target, GLuint index, GLenum pname, GLint * params);
typedef GLuint (CODEGEN_FUNCPTR * PFNGLGETSUBROUTINEINDEXPROC)(GLuint program, GLenum shadertype, const GLchar * name);
typedef GLint (CODEGEN_FUNCPTR * PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC)(GLuint program, GLenum shadertype, const GLchar * name);
typedef void (CODEGEN_FUNCPTR * PFNGLGETUNIFORMSUBROUTINEUIVPROC)(GLenum shadertype, GLint location, GLuint * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETUNIFORMDVPROC)(GLuint program, GLint location, GLdouble * params);
typedef GLboolean (CODEGEN_FUNCPTR * PFNGLISTRANSFORMFEEDBACKPROC)(GLuint id);
typedef void (CODEGEN_FUNCPTR * PFNGLMINSAMPLESHADINGPROC)(GLfloat value);
typedef void (CODEGEN_FUNCPTR * PFNGLPATCHPARAMETERFVPROC)(GLenum pname, const GLfloat * values);
typedef void (CODEGEN_FUNCPTR * PFNGLPATCHPARAMETERIPROC)(GLenum pname, GLint value);
typedef void (CODEGEN_FUNCPTR * PFNGLPAUSETRANSFORMFEEDBACKPROC)();
typedef void (CODEGEN_FUNCPTR * PFNGLRESUMETRANSFORMFEEDBACKPROC)();
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORM1DPROC)(GLint location, GLdouble x);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORM1DVPROC)(GLint location, GLsizei count, const GLdouble * value);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORM2DPROC)(GLint location, GLdouble x, GLdouble y);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORM2DVPROC)(GLint location, GLsizei count, const GLdouble * value);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORM3DPROC)(GLint location, GLdouble x, GLdouble y, GLdouble z);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORM3DVPROC)(GLint location, GLsizei count, const GLdouble * value);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORM4DPROC)(GLint location, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORM4DVPROC)(GLint location, GLsizei count, const GLdouble * value);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORMMATRIX2DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORMMATRIX2X3DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORMMATRIX2X4DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORMMATRIX3DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORMMATRIX3X2DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORMMATRIX3X4DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORMMATRIX4DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORMMATRIX4X2DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORMMATRIX4X3DVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLdouble * value);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORMSUBROUTINESUIVPROC)(GLenum shadertype, GLsizei count, const GLuint * indices);

extern PFNGLBEGINQUERYINDEXEDPROC _funcptr_glBeginQueryIndexed;
#define glBeginQueryIndexed _funcptr_glBeginQueryIndexed
extern PFNGLBINDTRANSFORMFEEDBACKPROC _funcptr_glBindTransformFeedback;
#define glBindTransformFeedback _funcptr_glBindTransformFeedback
extern PFNGLBLENDEQUATIONSEPARATEIPROC _funcptr_glBlendEquationSeparatei;
#define glBlendEquationSeparatei _funcptr_glBlendEquationSeparatei
extern PFNGLBLENDEQUATIONIPROC _funcptr_glBlendEquationi;
#define glBlendEquationi _funcptr_glBlendEquationi
extern PFNGLBLENDFUNCSEPARATEIPROC _funcptr_glBlendFuncSeparatei;
#define glBlendFuncSeparatei _funcptr_glBlendFuncSeparatei
extern PFNGLBLENDFUNCIPROC _funcptr_glBlendFunci;
#define glBlendFunci _funcptr_glBlendFunci
extern PFNGLDELETETRANSFORMFEEDBACKSPROC _funcptr_glDeleteTransformFeedbacks;
#define glDeleteTransformFeedbacks _funcptr_glDeleteTransformFeedbacks
extern PFNGLDRAWARRAYSINDIRECTPROC _funcptr_glDrawArraysIndirect;
#define glDrawArraysIndirect _funcptr_glDrawArraysIndirect
extern PFNGLDRAWELEMENTSINDIRECTPROC _funcptr_glDrawElementsIndirect;
#define glDrawElementsIndirect _funcptr_glDrawElementsIndirect
extern PFNGLDRAWTRANSFORMFEEDBACKPROC _funcptr_glDrawTransformFeedback;
#define glDrawTransformFeedback _funcptr_glDrawTransformFeedback
extern PFNGLDRAWTRANSFORMFEEDBACKSTREAMPROC _funcptr_glDrawTransformFeedbackStream;
#define glDrawTransformFeedbackStream _funcptr_glDrawTransformFeedbackStream
extern PFNGLENDQUERYINDEXEDPROC _funcptr_glEndQueryIndexed;
#define glEndQueryIndexed _funcptr_glEndQueryIndexed
extern PFNGLGENTRANSFORMFEEDBACKSPROC _funcptr_glGenTransformFeedbacks;
#define glGenTransformFeedbacks _funcptr_glGenTransformFeedbacks
extern PFNGLGETACTIVESUBROUTINENAMEPROC _funcptr_glGetActiveSubroutineName;
#define glGetActiveSubroutineName _funcptr_glGetActiveSubroutineName
extern PFNGLGETACTIVESUBROUTINEUNIFORMNAMEPROC _funcptr_glGetActiveSubroutineUniformName;
#define glGetActiveSubroutineUniformName _funcptr_glGetActiveSubroutineUniformName
extern PFNGLGETACTIVESUBROUTINEUNIFORMIVPROC _funcptr_glGetActiveSubroutineUniformiv;
#define glGetActiveSubroutineUniformiv _funcptr_glGetActiveSubroutineUniformiv
extern PFNGLGETPROGRAMSTAGEIVPROC _funcptr_glGetProgramStageiv;
#define glGetProgramStageiv _funcptr_glGetProgramStageiv
extern PFNGLGETQUERYINDEXEDIVPROC _funcptr_glGetQueryIndexediv;
#define glGetQueryIndexediv _funcptr_glGetQueryIndexediv
extern PFNGLGETSUBROUTINEINDEXPROC _funcptr_glGetSubroutineIndex;
#define glGetSubroutineIndex _funcptr_glGetSubroutineIndex
extern PFNGLGETSUBROUTINEUNIFORMLOCATIONPROC _funcptr_glGetSubroutineUniformLocation;
#define glGetSubroutineUniformLocation _funcptr_glGetSubroutineUniformLocation
extern PFNGLGETUNIFORMSUBROUTINEUIVPROC _funcptr_glGetUniformSubroutineuiv;
#define glGetUniformSubroutineuiv _funcptr_glGetUniformSubroutineuiv
extern PFNGLGETUNIFORMDVPROC _funcptr_glGetUniformdv;
#define glGetUniformdv _funcptr_glGetUniformdv
extern PFNGLISTRANSFORMFEEDBACKPROC _funcptr_glIsTransformFeedback;
#define glIsTransformFeedback _funcptr_glIsTransformFeedback
extern PFNGLMINSAMPLESHADINGPROC _funcptr_glMinSampleShading;
#define glMinSampleShading _funcptr_glMinSampleShading
extern PFNGLPATCHPARAMETERFVPROC _funcptr_glPatchParameterfv;
#define glPatchParameterfv _funcptr_glPatchParameterfv
extern PFNGLPATCHPARAMETERIPROC _funcptr_glPatchParameteri;
#define glPatchParameteri _funcptr_glPatchParameteri
extern PFNGLPAUSETRANSFORMFEEDBACKPROC _funcptr_glPauseTransformFeedback;
#define glPauseTransformFeedback _funcptr_glPauseTransformFeedback
extern PFNGLRESUMETRANSFORMFEEDBACKPROC _funcptr_glResumeTransformFeedback;
#define glResumeTransformFeedback _funcptr_glResumeTransformFeedback
extern PFNGLUNIFORM1DPROC _funcptr_glUniform1d;
#define glUniform1d _funcptr_glUniform1d
extern PFNGLUNIFORM1DVPROC _funcptr_glUniform1dv;
#define glUniform1dv _funcptr_glUniform1dv
extern PFNGLUNIFORM2DPROC _funcptr_glUniform2d;
#define glUniform2d _funcptr_glUniform2d
extern PFNGLUNIFORM2DVPROC _funcptr_glUniform2dv;
#define glUniform2dv _funcptr_glUniform2dv
extern PFNGLUNIFORM3DPROC _funcptr_glUniform3d;
#define glUniform3d _funcptr_glUniform3d
extern PFNGLUNIFORM3DVPROC _funcptr_glUniform3dv;
#define glUniform3dv _funcptr_glUniform3dv
extern PFNGLUNIFORM4DPROC _funcptr_glUniform4d;
#define glUniform4d _funcptr_glUniform4d
extern PFNGLUNIFORM4DVPROC _funcptr_glUniform4dv;
#define glUniform4dv _funcptr_glUniform4dv
extern PFNGLUNIFORMMATRIX2DVPROC _funcptr_glUniformMatrix2dv;
#define glUniformMatrix2dv _funcptr_glUniformMatrix2dv
extern PFNGLUNIFORMMATRIX2X3DVPROC _funcptr_glUniformMatrix2x3dv;
#define glUniformMatrix2x3dv _funcptr_glUniformMatrix2x3dv
extern PFNGLUNIFORMMATRIX2X4DVPROC _funcptr_glUniformMatrix2x4dv;
#define glUniformMatrix2x4dv _funcptr_glUniformMatrix2x4dv
extern PFNGLUNIFORMMATRIX3DVPROC _funcptr_glUniformMatrix3dv;
#define glUniformMatrix3dv _funcptr_glUniformMatrix3dv
extern PFNGLUNIFORMMATRIX3X2DVPROC _funcptr_glUniformMatrix3x2dv;
#define glUniformMatrix3x2dv _funcptr_glUniformMatrix3x2dv
extern PFNGLUNIFORMMATRIX3X4DVPROC _funcptr_glUniformMatrix3x4dv;
#define glUniformMatrix3x4dv _funcptr_glUniformMatrix3x4dv
extern PFNGLUNIFORMMATRIX4DVPROC _funcptr_glUniformMatrix4dv;
#define glUniformMatrix4dv _funcptr_glUniformMatrix4dv
extern PFNGLUNIFORMMATRIX4X2DVPROC _funcptr_glUniformMatrix4x2dv;
#define glUniformMatrix4x2dv _funcptr_glUniformMatrix4x2dv
extern PFNGLUNIFORMMATRIX4X3DVPROC _funcptr_glUniformMatrix4x3dv;
#define glUniformMatrix4x3dv _funcptr_glUniformMatrix4x3dv
extern PFNGLUNIFORMSUBROUTINESUIVPROC _funcptr_glUniformSubroutinesuiv;
#define glUniformSubroutinesuiv _funcptr_glUniformSubroutinesuiv

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif /*OPENGL_GEN_CORE_4_0_H*/
