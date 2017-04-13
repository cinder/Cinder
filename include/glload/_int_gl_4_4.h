#ifndef OPENGL_GEN_CORE_4_4_H
#define OPENGL_GEN_CORE_4_4_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
#define GL_MAX_VERTEX_ATTRIB_STRIDE 0x82E5

typedef void (CODEGEN_FUNCPTR * PFNGLBINDBUFFERSBASEPROC)(GLenum target, GLuint first, GLsizei count, const GLuint * buffers);
typedef void (CODEGEN_FUNCPTR * PFNGLBINDBUFFERSRANGEPROC)(GLenum target, GLuint first, GLsizei count, const GLuint * buffers, const GLintptr * offsets, const GLsizeiptr * sizes);
typedef void (CODEGEN_FUNCPTR * PFNGLBINDIMAGETEXTURESPROC)(GLuint first, GLsizei count, const GLuint * textures);
typedef void (CODEGEN_FUNCPTR * PFNGLBINDSAMPLERSPROC)(GLuint first, GLsizei count, const GLuint * samplers);
typedef void (CODEGEN_FUNCPTR * PFNGLBINDTEXTURESPROC)(GLuint first, GLsizei count, const GLuint * textures);
typedef void (CODEGEN_FUNCPTR * PFNGLBINDVERTEXBUFFERSPROC)(GLuint first, GLsizei count, const GLuint * buffers, const GLintptr * offsets, const GLsizei * strides);
typedef void (CODEGEN_FUNCPTR * PFNGLBUFFERSTORAGEPROC)(GLenum target, GLsizeiptr size, const void * data, GLbitfield flags);
typedef void (CODEGEN_FUNCPTR * PFNGLCLEARTEXIMAGEPROC)(GLuint texture, GLint level, GLenum format, GLenum type, const void * data);
typedef void (CODEGEN_FUNCPTR * PFNGLCLEARTEXSUBIMAGEPROC)(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void * data);

CI_API extern PFNGLBINDBUFFERSBASEPROC _funcptr_glBindBuffersBase;
#define glBindBuffersBase _funcptr_glBindBuffersBase
CI_API extern PFNGLBINDBUFFERSRANGEPROC _funcptr_glBindBuffersRange;
#define glBindBuffersRange _funcptr_glBindBuffersRange
CI_API extern PFNGLBINDIMAGETEXTURESPROC _funcptr_glBindImageTextures;
#define glBindImageTextures _funcptr_glBindImageTextures
CI_API extern PFNGLBINDSAMPLERSPROC _funcptr_glBindSamplers;
#define glBindSamplers _funcptr_glBindSamplers
CI_API extern PFNGLBINDTEXTURESPROC _funcptr_glBindTextures;
#define glBindTextures _funcptr_glBindTextures
CI_API extern PFNGLBINDVERTEXBUFFERSPROC _funcptr_glBindVertexBuffers;
#define glBindVertexBuffers _funcptr_glBindVertexBuffers
CI_API extern PFNGLBUFFERSTORAGEPROC _funcptr_glBufferStorage;
#define glBufferStorage _funcptr_glBufferStorage
CI_API extern PFNGLCLEARTEXIMAGEPROC _funcptr_glClearTexImage;
#define glClearTexImage _funcptr_glClearTexImage
CI_API extern PFNGLCLEARTEXSUBIMAGEPROC _funcptr_glClearTexSubImage;
#define glClearTexSubImage _funcptr_glClearTexSubImage

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif /*OPENGL_GEN_CORE_4_4_H*/
