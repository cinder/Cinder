#ifndef OPENGL_GEN_CORE_1_4_H
#define OPENGL_GEN_CORE_1_4_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
#define GL_BLEND_DST_ALPHA 0x80CA
#define GL_BLEND_DST_RGB 0x80C8
#define GL_BLEND_SRC_ALPHA 0x80CB
#define GL_BLEND_SRC_RGB 0x80C9
#define GL_DECR_WRAP 0x8508
#define GL_DEPTH_COMPONENT16 0x81A5
#define GL_DEPTH_COMPONENT24 0x81A6
#define GL_DEPTH_COMPONENT32 0x81A7
#define GL_INCR_WRAP 0x8507
#define GL_MAX_TEXTURE_LOD_BIAS 0x84FD
#define GL_MIRRORED_REPEAT 0x8370
#define GL_POINT_FADE_THRESHOLD_SIZE 0x8128
#define GL_TEXTURE_COMPARE_FUNC 0x884D
#define GL_TEXTURE_COMPARE_MODE 0x884C
#define GL_TEXTURE_DEPTH_SIZE 0x884A
#define GL_TEXTURE_LOD_BIAS 0x8501

typedef void (CODEGEN_FUNCPTR * PFNGLBLENDFUNCSEPARATEPROC)(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
typedef void (CODEGEN_FUNCPTR * PFNGLMULTIDRAWARRAYSPROC)(GLenum mode, const GLint * first, const GLsizei * count, GLsizei drawcount);
typedef void (CODEGEN_FUNCPTR * PFNGLMULTIDRAWELEMENTSPROC)(GLenum mode, const GLsizei * count, GLenum type, const GLvoid *const* indices, GLsizei drawcount);
typedef void (CODEGEN_FUNCPTR * PFNGLPOINTPARAMETERFPROC)(GLenum pname, GLfloat param);
typedef void (CODEGEN_FUNCPTR * PFNGLPOINTPARAMETERFVPROC)(GLenum pname, const GLfloat * params);
typedef void (CODEGEN_FUNCPTR * PFNGLPOINTPARAMETERIPROC)(GLenum pname, GLint param);
typedef void (CODEGEN_FUNCPTR * PFNGLPOINTPARAMETERIVPROC)(GLenum pname, const GLint * params);

extern PFNGLBLENDFUNCSEPARATEPROC _funcptr_glBlendFuncSeparate;
#define glBlendFuncSeparate _funcptr_glBlendFuncSeparate
extern PFNGLMULTIDRAWARRAYSPROC _funcptr_glMultiDrawArrays;
#define glMultiDrawArrays _funcptr_glMultiDrawArrays
extern PFNGLMULTIDRAWELEMENTSPROC _funcptr_glMultiDrawElements;
#define glMultiDrawElements _funcptr_glMultiDrawElements
extern PFNGLPOINTPARAMETERFPROC _funcptr_glPointParameterf;
#define glPointParameterf _funcptr_glPointParameterf
extern PFNGLPOINTPARAMETERFVPROC _funcptr_glPointParameterfv;
#define glPointParameterfv _funcptr_glPointParameterfv
extern PFNGLPOINTPARAMETERIPROC _funcptr_glPointParameteri;
#define glPointParameteri _funcptr_glPointParameteri
extern PFNGLPOINTPARAMETERIVPROC _funcptr_glPointParameteriv;
#define glPointParameteriv _funcptr_glPointParameteriv

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif /*OPENGL_GEN_CORE_1_4_H*/
