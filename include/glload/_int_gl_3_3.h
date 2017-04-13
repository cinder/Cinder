#ifndef OPENGL_GEN_CORE_3_3_H
#define OPENGL_GEN_CORE_3_3_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
#define GL_VERTEX_ATTRIB_ARRAY_DIVISOR 0x88FE

typedef void (CODEGEN_FUNCPTR * PFNGLBINDFRAGDATALOCATIONINDEXEDPROC)(GLuint program, GLuint colorNumber, GLuint index, const GLchar * name);
typedef void (CODEGEN_FUNCPTR * PFNGLBINDSAMPLERPROC)(GLuint unit, GLuint sampler);
typedef void (CODEGEN_FUNCPTR * PFNGLDELETESAMPLERSPROC)(GLsizei count, const GLuint * samplers);
typedef void (CODEGEN_FUNCPTR * PFNGLGENSAMPLERSPROC)(GLsizei count, GLuint * samplers);
typedef GLint (CODEGEN_FUNCPTR * PFNGLGETFRAGDATAINDEXPROC)(GLuint program, const GLchar * name);
typedef void (CODEGEN_FUNCPTR * PFNGLGETQUERYOBJECTI64VPROC)(GLuint id, GLenum pname, GLint64 * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETQUERYOBJECTUI64VPROC)(GLuint id, GLenum pname, GLuint64 * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETSAMPLERPARAMETERIIVPROC)(GLuint sampler, GLenum pname, GLint * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETSAMPLERPARAMETERIUIVPROC)(GLuint sampler, GLenum pname, GLuint * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETSAMPLERPARAMETERFVPROC)(GLuint sampler, GLenum pname, GLfloat * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETSAMPLERPARAMETERIVPROC)(GLuint sampler, GLenum pname, GLint * params);
typedef GLboolean (CODEGEN_FUNCPTR * PFNGLISSAMPLERPROC)(GLuint sampler);
typedef void (CODEGEN_FUNCPTR * PFNGLQUERYCOUNTERPROC)(GLuint id, GLenum target);
typedef void (CODEGEN_FUNCPTR * PFNGLSAMPLERPARAMETERIIVPROC)(GLuint sampler, GLenum pname, const GLint * param);
typedef void (CODEGEN_FUNCPTR * PFNGLSAMPLERPARAMETERIUIVPROC)(GLuint sampler, GLenum pname, const GLuint * param);
typedef void (CODEGEN_FUNCPTR * PFNGLSAMPLERPARAMETERFPROC)(GLuint sampler, GLenum pname, GLfloat param);
typedef void (CODEGEN_FUNCPTR * PFNGLSAMPLERPARAMETERFVPROC)(GLuint sampler, GLenum pname, const GLfloat * param);
typedef void (CODEGEN_FUNCPTR * PFNGLSAMPLERPARAMETERIPROC)(GLuint sampler, GLenum pname, GLint param);
typedef void (CODEGEN_FUNCPTR * PFNGLSAMPLERPARAMETERIVPROC)(GLuint sampler, GLenum pname, const GLint * param);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIBDIVISORPROC)(GLuint index, GLuint divisor);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIBP1UIPROC)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIBP1UIVPROC)(GLuint index, GLenum type, GLboolean normalized, const GLuint * value);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIBP2UIPROC)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIBP2UIVPROC)(GLuint index, GLenum type, GLboolean normalized, const GLuint * value);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIBP3UIPROC)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIBP3UIVPROC)(GLuint index, GLenum type, GLboolean normalized, const GLuint * value);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIBP4UIPROC)(GLuint index, GLenum type, GLboolean normalized, GLuint value);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEXATTRIBP4UIVPROC)(GLuint index, GLenum type, GLboolean normalized, const GLuint * value);

CI_API extern PFNGLBINDFRAGDATALOCATIONINDEXEDPROC _funcptr_glBindFragDataLocationIndexed;
#define glBindFragDataLocationIndexed _funcptr_glBindFragDataLocationIndexed
CI_API extern PFNGLBINDSAMPLERPROC _funcptr_glBindSampler;
#define glBindSampler _funcptr_glBindSampler
CI_API extern PFNGLDELETESAMPLERSPROC _funcptr_glDeleteSamplers;
#define glDeleteSamplers _funcptr_glDeleteSamplers
CI_API extern PFNGLGENSAMPLERSPROC _funcptr_glGenSamplers;
#define glGenSamplers _funcptr_glGenSamplers
CI_API extern PFNGLGETFRAGDATAINDEXPROC _funcptr_glGetFragDataIndex;
#define glGetFragDataIndex _funcptr_glGetFragDataIndex
CI_API extern PFNGLGETQUERYOBJECTI64VPROC _funcptr_glGetQueryObjecti64v;
#define glGetQueryObjecti64v _funcptr_glGetQueryObjecti64v
CI_API extern PFNGLGETQUERYOBJECTUI64VPROC _funcptr_glGetQueryObjectui64v;
#define glGetQueryObjectui64v _funcptr_glGetQueryObjectui64v
CI_API extern PFNGLGETSAMPLERPARAMETERIIVPROC _funcptr_glGetSamplerParameterIiv;
#define glGetSamplerParameterIiv _funcptr_glGetSamplerParameterIiv
CI_API extern PFNGLGETSAMPLERPARAMETERIUIVPROC _funcptr_glGetSamplerParameterIuiv;
#define glGetSamplerParameterIuiv _funcptr_glGetSamplerParameterIuiv
CI_API extern PFNGLGETSAMPLERPARAMETERFVPROC _funcptr_glGetSamplerParameterfv;
#define glGetSamplerParameterfv _funcptr_glGetSamplerParameterfv
CI_API extern PFNGLGETSAMPLERPARAMETERIVPROC _funcptr_glGetSamplerParameteriv;
#define glGetSamplerParameteriv _funcptr_glGetSamplerParameteriv
CI_API extern PFNGLISSAMPLERPROC _funcptr_glIsSampler;
#define glIsSampler _funcptr_glIsSampler
CI_API extern PFNGLQUERYCOUNTERPROC _funcptr_glQueryCounter;
#define glQueryCounter _funcptr_glQueryCounter
CI_API extern PFNGLSAMPLERPARAMETERIIVPROC _funcptr_glSamplerParameterIiv;
#define glSamplerParameterIiv _funcptr_glSamplerParameterIiv
CI_API extern PFNGLSAMPLERPARAMETERIUIVPROC _funcptr_glSamplerParameterIuiv;
#define glSamplerParameterIuiv _funcptr_glSamplerParameterIuiv
CI_API extern PFNGLSAMPLERPARAMETERFPROC _funcptr_glSamplerParameterf;
#define glSamplerParameterf _funcptr_glSamplerParameterf
CI_API extern PFNGLSAMPLERPARAMETERFVPROC _funcptr_glSamplerParameterfv;
#define glSamplerParameterfv _funcptr_glSamplerParameterfv
CI_API extern PFNGLSAMPLERPARAMETERIPROC _funcptr_glSamplerParameteri;
#define glSamplerParameteri _funcptr_glSamplerParameteri
CI_API extern PFNGLSAMPLERPARAMETERIVPROC _funcptr_glSamplerParameteriv;
#define glSamplerParameteriv _funcptr_glSamplerParameteriv
CI_API extern PFNGLVERTEXATTRIBDIVISORPROC _funcptr_glVertexAttribDivisor;
#define glVertexAttribDivisor _funcptr_glVertexAttribDivisor
CI_API extern PFNGLVERTEXATTRIBP1UIPROC _funcptr_glVertexAttribP1ui;
#define glVertexAttribP1ui _funcptr_glVertexAttribP1ui
CI_API extern PFNGLVERTEXATTRIBP1UIVPROC _funcptr_glVertexAttribP1uiv;
#define glVertexAttribP1uiv _funcptr_glVertexAttribP1uiv
CI_API extern PFNGLVERTEXATTRIBP2UIPROC _funcptr_glVertexAttribP2ui;
#define glVertexAttribP2ui _funcptr_glVertexAttribP2ui
CI_API extern PFNGLVERTEXATTRIBP2UIVPROC _funcptr_glVertexAttribP2uiv;
#define glVertexAttribP2uiv _funcptr_glVertexAttribP2uiv
CI_API extern PFNGLVERTEXATTRIBP3UIPROC _funcptr_glVertexAttribP3ui;
#define glVertexAttribP3ui _funcptr_glVertexAttribP3ui
CI_API extern PFNGLVERTEXATTRIBP3UIVPROC _funcptr_glVertexAttribP3uiv;
#define glVertexAttribP3uiv _funcptr_glVertexAttribP3uiv
CI_API extern PFNGLVERTEXATTRIBP4UIPROC _funcptr_glVertexAttribP4ui;
#define glVertexAttribP4ui _funcptr_glVertexAttribP4ui
CI_API extern PFNGLVERTEXATTRIBP4UIVPROC _funcptr_glVertexAttribP4uiv;
#define glVertexAttribP4uiv _funcptr_glVertexAttribP4uiv

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif /*OPENGL_GEN_CORE_3_3_H*/
