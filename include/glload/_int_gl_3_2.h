#ifndef OPENGL_GEN_CORE_3_2_H
#define OPENGL_GEN_CORE_3_2_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
#define GL_CONTEXT_COMPATIBILITY_PROFILE_BIT 0x00000002
#define GL_CONTEXT_CORE_PROFILE_BIT 0x00000001
#define GL_CONTEXT_PROFILE_MASK 0x9126
#define GL_FRAMEBUFFER_ATTACHMENT_LAYERED 0x8DA7
#define GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS 0x8DA8
#define GL_GEOMETRY_INPUT_TYPE 0x8917
#define GL_GEOMETRY_OUTPUT_TYPE 0x8918
#define GL_GEOMETRY_SHADER 0x8DD9
#define GL_GEOMETRY_VERTICES_OUT 0x8916
#define GL_LINES_ADJACENCY 0x000A
#define GL_LINE_STRIP_ADJACENCY 0x000B
#define GL_MAX_FRAGMENT_INPUT_COMPONENTS 0x9125
#define GL_MAX_GEOMETRY_INPUT_COMPONENTS 0x9123
#define GL_MAX_GEOMETRY_OUTPUT_COMPONENTS 0x9124
#define GL_MAX_GEOMETRY_OUTPUT_VERTICES 0x8DE0
#define GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS 0x8C29
#define GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS 0x8DE1
#define GL_MAX_GEOMETRY_UNIFORM_COMPONENTS 0x8DDF
#define GL_MAX_VERTEX_OUTPUT_COMPONENTS 0x9122
#define GL_PROGRAM_POINT_SIZE 0x8642
#define GL_TRIANGLES_ADJACENCY 0x000C
#define GL_TRIANGLE_STRIP_ADJACENCY 0x000D

typedef GLenum (CODEGEN_FUNCPTR * PFNGLCLIENTWAITSYNCPROC)(GLsync sync, GLbitfield flags, GLuint64 timeout);
typedef void (CODEGEN_FUNCPTR * PFNGLDELETESYNCPROC)(GLsync sync);
typedef void (CODEGEN_FUNCPTR * PFNGLDRAWELEMENTSBASEVERTEXPROC)(GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLint basevertex);
typedef void (CODEGEN_FUNCPTR * PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC)(GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLsizei instancecount, GLint basevertex);
typedef void (CODEGEN_FUNCPTR * PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC)(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid * indices, GLint basevertex);
typedef GLsync (CODEGEN_FUNCPTR * PFNGLFENCESYNCPROC)(GLenum condition, GLbitfield flags);
typedef void (CODEGEN_FUNCPTR * PFNGLFRAMEBUFFERTEXTUREPROC)(GLenum target, GLenum attachment, GLuint texture, GLint level);
typedef void (CODEGEN_FUNCPTR * PFNGLGETBUFFERPARAMETERI64VPROC)(GLenum target, GLenum pname, GLint64 * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETINTEGER64I_VPROC)(GLenum target, GLuint index, GLint64 * data);
typedef void (CODEGEN_FUNCPTR * PFNGLGETINTEGER64VPROC)(GLenum pname, GLint64 * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETMULTISAMPLEFVPROC)(GLenum pname, GLuint index, GLfloat * val);
typedef void (CODEGEN_FUNCPTR * PFNGLGETSYNCIVPROC)(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei * length, GLint * values);
typedef GLboolean (CODEGEN_FUNCPTR * PFNGLISSYNCPROC)(GLsync sync);
typedef void (CODEGEN_FUNCPTR * PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC)(GLenum mode, const GLsizei * count, GLenum type, const GLvoid *const* indices, GLsizei drawcount, const GLint * basevertex);
typedef void (CODEGEN_FUNCPTR * PFNGLPROVOKINGVERTEXPROC)(GLenum mode);
typedef void (CODEGEN_FUNCPTR * PFNGLSAMPLEMASKIPROC)(GLuint index, GLbitfield mask);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXIMAGE2DMULTISAMPLEPROC)(GLenum target, GLsizei samples, GLint internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXIMAGE3DMULTISAMPLEPROC)(GLenum target, GLsizei samples, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
typedef void (CODEGEN_FUNCPTR * PFNGLWAITSYNCPROC)(GLsync sync, GLbitfield flags, GLuint64 timeout);

extern PFNGLCLIENTWAITSYNCPROC _funcptr_glClientWaitSync;
#define glClientWaitSync _funcptr_glClientWaitSync
extern PFNGLDELETESYNCPROC _funcptr_glDeleteSync;
#define glDeleteSync _funcptr_glDeleteSync
extern PFNGLDRAWELEMENTSBASEVERTEXPROC _funcptr_glDrawElementsBaseVertex;
#define glDrawElementsBaseVertex _funcptr_glDrawElementsBaseVertex
extern PFNGLDRAWELEMENTSINSTANCEDBASEVERTEXPROC _funcptr_glDrawElementsInstancedBaseVertex;
#define glDrawElementsInstancedBaseVertex _funcptr_glDrawElementsInstancedBaseVertex
extern PFNGLDRAWRANGEELEMENTSBASEVERTEXPROC _funcptr_glDrawRangeElementsBaseVertex;
#define glDrawRangeElementsBaseVertex _funcptr_glDrawRangeElementsBaseVertex
extern PFNGLFENCESYNCPROC _funcptr_glFenceSync;
#define glFenceSync _funcptr_glFenceSync
extern PFNGLFRAMEBUFFERTEXTUREPROC _funcptr_glFramebufferTexture;
#define glFramebufferTexture _funcptr_glFramebufferTexture
extern PFNGLGETBUFFERPARAMETERI64VPROC _funcptr_glGetBufferParameteri64v;
#define glGetBufferParameteri64v _funcptr_glGetBufferParameteri64v
extern PFNGLGETINTEGER64I_VPROC _funcptr_glGetInteger64i_v;
#define glGetInteger64i_v _funcptr_glGetInteger64i_v
extern PFNGLGETINTEGER64VPROC _funcptr_glGetInteger64v;
#define glGetInteger64v _funcptr_glGetInteger64v
extern PFNGLGETMULTISAMPLEFVPROC _funcptr_glGetMultisamplefv;
#define glGetMultisamplefv _funcptr_glGetMultisamplefv
extern PFNGLGETSYNCIVPROC _funcptr_glGetSynciv;
#define glGetSynciv _funcptr_glGetSynciv
extern PFNGLISSYNCPROC _funcptr_glIsSync;
#define glIsSync _funcptr_glIsSync
extern PFNGLMULTIDRAWELEMENTSBASEVERTEXPROC _funcptr_glMultiDrawElementsBaseVertex;
#define glMultiDrawElementsBaseVertex _funcptr_glMultiDrawElementsBaseVertex
extern PFNGLPROVOKINGVERTEXPROC _funcptr_glProvokingVertex;
#define glProvokingVertex _funcptr_glProvokingVertex
extern PFNGLSAMPLEMASKIPROC _funcptr_glSampleMaski;
#define glSampleMaski _funcptr_glSampleMaski
extern PFNGLTEXIMAGE2DMULTISAMPLEPROC _funcptr_glTexImage2DMultisample;
#define glTexImage2DMultisample _funcptr_glTexImage2DMultisample
extern PFNGLTEXIMAGE3DMULTISAMPLEPROC _funcptr_glTexImage3DMultisample;
#define glTexImage3DMultisample _funcptr_glTexImage3DMultisample
extern PFNGLWAITSYNCPROC _funcptr_glWaitSync;
#define glWaitSync _funcptr_glWaitSync

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif /*OPENGL_GEN_CORE_3_2_H*/
