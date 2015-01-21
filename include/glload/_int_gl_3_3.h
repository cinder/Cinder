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

extern PFNGLBINDFRAGDATALOCATIONINDEXEDPROC _funcptr_glBindFragDataLocationIndexed;
#define glBindFragDataLocationIndexed _funcptr_glBindFragDataLocationIndexed
extern PFNGLBINDSAMPLERPROC _funcptr_glBindSampler;
#define glBindSampler _funcptr_glBindSampler
extern PFNGLDELETESAMPLERSPROC _funcptr_glDeleteSamplers;
#define glDeleteSamplers _funcptr_glDeleteSamplers
extern PFNGLGENSAMPLERSPROC _funcptr_glGenSamplers;
#define glGenSamplers _funcptr_glGenSamplers
extern PFNGLGETFRAGDATAINDEXPROC _funcptr_glGetFragDataIndex;
#define glGetFragDataIndex _funcptr_glGetFragDataIndex
extern PFNGLGETQUERYOBJECTI64VPROC _funcptr_glGetQueryObjecti64v;
#define glGetQueryObjecti64v _funcptr_glGetQueryObjecti64v
extern PFNGLGETQUERYOBJECTUI64VPROC _funcptr_glGetQueryObjectui64v;
#define glGetQueryObjectui64v _funcptr_glGetQueryObjectui64v
extern PFNGLGETSAMPLERPARAMETERIIVPROC _funcptr_glGetSamplerParameterIiv;
#define glGetSamplerParameterIiv _funcptr_glGetSamplerParameterIiv
extern PFNGLGETSAMPLERPARAMETERIUIVPROC _funcptr_glGetSamplerParameterIuiv;
#define glGetSamplerParameterIuiv _funcptr_glGetSamplerParameterIuiv
extern PFNGLGETSAMPLERPARAMETERFVPROC _funcptr_glGetSamplerParameterfv;
#define glGetSamplerParameterfv _funcptr_glGetSamplerParameterfv
extern PFNGLGETSAMPLERPARAMETERIVPROC _funcptr_glGetSamplerParameteriv;
#define glGetSamplerParameteriv _funcptr_glGetSamplerParameteriv
extern PFNGLISSAMPLERPROC _funcptr_glIsSampler;
#define glIsSampler _funcptr_glIsSampler
extern PFNGLQUERYCOUNTERPROC _funcptr_glQueryCounter;
#define glQueryCounter _funcptr_glQueryCounter
extern PFNGLSAMPLERPARAMETERIIVPROC _funcptr_glSamplerParameterIiv;
#define glSamplerParameterIiv _funcptr_glSamplerParameterIiv
extern PFNGLSAMPLERPARAMETERIUIVPROC _funcptr_glSamplerParameterIuiv;
#define glSamplerParameterIuiv _funcptr_glSamplerParameterIuiv
extern PFNGLSAMPLERPARAMETERFPROC _funcptr_glSamplerParameterf;
#define glSamplerParameterf _funcptr_glSamplerParameterf
extern PFNGLSAMPLERPARAMETERFVPROC _funcptr_glSamplerParameterfv;
#define glSamplerParameterfv _funcptr_glSamplerParameterfv
extern PFNGLSAMPLERPARAMETERIPROC _funcptr_glSamplerParameteri;
#define glSamplerParameteri _funcptr_glSamplerParameteri
extern PFNGLSAMPLERPARAMETERIVPROC _funcptr_glSamplerParameteriv;
#define glSamplerParameteriv _funcptr_glSamplerParameteriv
extern PFNGLVERTEXATTRIBDIVISORPROC _funcptr_glVertexAttribDivisor;
#define glVertexAttribDivisor _funcptr_glVertexAttribDivisor
extern PFNGLVERTEXATTRIBP1UIPROC _funcptr_glVertexAttribP1ui;
#define glVertexAttribP1ui _funcptr_glVertexAttribP1ui
extern PFNGLVERTEXATTRIBP1UIVPROC _funcptr_glVertexAttribP1uiv;
#define glVertexAttribP1uiv _funcptr_glVertexAttribP1uiv
extern PFNGLVERTEXATTRIBP2UIPROC _funcptr_glVertexAttribP2ui;
#define glVertexAttribP2ui _funcptr_glVertexAttribP2ui
extern PFNGLVERTEXATTRIBP2UIVPROC _funcptr_glVertexAttribP2uiv;
#define glVertexAttribP2uiv _funcptr_glVertexAttribP2uiv
extern PFNGLVERTEXATTRIBP3UIPROC _funcptr_glVertexAttribP3ui;
#define glVertexAttribP3ui _funcptr_glVertexAttribP3ui
extern PFNGLVERTEXATTRIBP3UIVPROC _funcptr_glVertexAttribP3uiv;
#define glVertexAttribP3uiv _funcptr_glVertexAttribP3uiv
extern PFNGLVERTEXATTRIBP4UIPROC _funcptr_glVertexAttribP4ui;
#define glVertexAttribP4ui _funcptr_glVertexAttribP4ui
extern PFNGLVERTEXATTRIBP4UIVPROC _funcptr_glVertexAttribP4uiv;
#define glVertexAttribP4uiv _funcptr_glVertexAttribP4uiv

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif /*OPENGL_GEN_CORE_3_3_H*/
