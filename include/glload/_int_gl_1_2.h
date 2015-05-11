#ifndef OPENGL_GEN_CORE_1_2_H
#define OPENGL_GEN_CORE_1_2_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
#define GL_ALIASED_LINE_WIDTH_RANGE 0x846E
#define GL_BGR 0x80E0
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_MAX_3D_TEXTURE_SIZE 0x8073
#define GL_MAX_ELEMENTS_INDICES 0x80E9
#define GL_MAX_ELEMENTS_VERTICES 0x80E8
#define GL_PACK_IMAGE_HEIGHT 0x806C
#define GL_PACK_SKIP_IMAGES 0x806B
#define GL_PROXY_TEXTURE_3D 0x8070
#define GL_SMOOTH_LINE_WIDTH_GRANULARITY 0x0B23
#define GL_SMOOTH_LINE_WIDTH_RANGE 0x0B22
#define GL_SMOOTH_POINT_SIZE_GRANULARITY 0x0B13
#define GL_SMOOTH_POINT_SIZE_RANGE 0x0B12
#define GL_TEXTURE_BASE_LEVEL 0x813C
#define GL_TEXTURE_BINDING_3D 0x806A
#define GL_TEXTURE_DEPTH 0x8071
#define GL_TEXTURE_MAX_LEVEL 0x813D
#define GL_TEXTURE_MAX_LOD 0x813B
#define GL_TEXTURE_MIN_LOD 0x813A
#define GL_TEXTURE_WRAP_R 0x8072
#define GL_UNPACK_IMAGE_HEIGHT 0x806E
#define GL_UNPACK_SKIP_IMAGES 0x806D
#define GL_UNSIGNED_BYTE_2_3_3_REV 0x8362
#define GL_UNSIGNED_BYTE_3_3_2 0x8032
#define GL_UNSIGNED_INT_10_10_10_2 0x8036
#define GL_UNSIGNED_INT_8_8_8_8 0x8035
#define GL_UNSIGNED_INT_8_8_8_8_REV 0x8367
#define GL_UNSIGNED_SHORT_1_5_5_5_REV 0x8366
#define GL_UNSIGNED_SHORT_4_4_4_4 0x8033
#define GL_UNSIGNED_SHORT_4_4_4_4_REV 0x8365
#define GL_UNSIGNED_SHORT_5_5_5_1 0x8034
#define GL_UNSIGNED_SHORT_5_6_5 0x8363
#define GL_UNSIGNED_SHORT_5_6_5_REV 0x8364

typedef void (CODEGEN_FUNCPTR * PFNGLBLENDCOLORPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void (CODEGEN_FUNCPTR * PFNGLBLENDEQUATIONPROC)(GLenum mode);
typedef void (CODEGEN_FUNCPTR * PFNGLCOPYTEXSUBIMAGE3DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (CODEGEN_FUNCPTR * PFNGLDRAWRANGEELEMENTSPROC)(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid * indices);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXIMAGE3DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid * pixels);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXSUBIMAGE3DPROC)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid * pixels);

extern PFNGLBLENDCOLORPROC _funcptr_glBlendColor;
#define glBlendColor _funcptr_glBlendColor
extern PFNGLBLENDEQUATIONPROC _funcptr_glBlendEquation;
#define glBlendEquation _funcptr_glBlendEquation
extern PFNGLCOPYTEXSUBIMAGE3DPROC _funcptr_glCopyTexSubImage3D;
#define glCopyTexSubImage3D _funcptr_glCopyTexSubImage3D
extern PFNGLDRAWRANGEELEMENTSPROC _funcptr_glDrawRangeElements;
#define glDrawRangeElements _funcptr_glDrawRangeElements
extern PFNGLTEXIMAGE3DPROC _funcptr_glTexImage3D;
#define glTexImage3D _funcptr_glTexImage3D
extern PFNGLTEXSUBIMAGE3DPROC _funcptr_glTexSubImage3D;
#define glTexSubImage3D _funcptr_glTexSubImage3D

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif /*OPENGL_GEN_CORE_1_2_H*/
