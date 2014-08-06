#ifndef OPENGL_GEN_CORE_REM1_4_H
#define OPENGL_GEN_CORE_REM1_4_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
#define GL_COLOR_SUM 0x8458
#define GL_COMPARE_R_TO_TEXTURE 0x884E
#define GL_CURRENT_FOG_COORDINATE 0x8453
#define GL_CURRENT_SECONDARY_COLOR 0x8459
#define GL_DEPTH_TEXTURE_MODE 0x884B
#define GL_FOG_COORDINATE 0x8451
#define GL_FOG_COORDINATE_ARRAY 0x8457
#define GL_FOG_COORDINATE_ARRAY_POINTER 0x8456
#define GL_FOG_COORDINATE_ARRAY_STRIDE 0x8455
#define GL_FOG_COORDINATE_ARRAY_TYPE 0x8454
#define GL_FOG_COORDINATE_SOURCE 0x8450
#define GL_FRAGMENT_DEPTH 0x8452
#define GL_GENERATE_MIPMAP 0x8191
#define GL_GENERATE_MIPMAP_HINT 0x8192
#define GL_POINT_DISTANCE_ATTENUATION 0x8129
#define GL_POINT_SIZE_MAX 0x8127
#define GL_POINT_SIZE_MIN 0x8126
#define GL_SECONDARY_COLOR_ARRAY 0x845E
#define GL_SECONDARY_COLOR_ARRAY_POINTER 0x845D
#define GL_SECONDARY_COLOR_ARRAY_SIZE 0x845A
#define GL_SECONDARY_COLOR_ARRAY_STRIDE 0x845C
#define GL_SECONDARY_COLOR_ARRAY_TYPE 0x845B
#define GL_TEXTURE_FILTER_CONTROL 0x8500

typedef void (CODEGEN_FUNCPTR * PFNGLFOGCOORDPOINTERPROC)(GLenum type, GLsizei stride, const GLvoid * pointer);
typedef void (CODEGEN_FUNCPTR * PFNGLFOGCOORDDPROC)(GLdouble coord);
typedef void (CODEGEN_FUNCPTR * PFNGLFOGCOORDDVPROC)(const GLdouble * coord);
typedef void (CODEGEN_FUNCPTR * PFNGLFOGCOORDFPROC)(GLfloat coord);
typedef void (CODEGEN_FUNCPTR * PFNGLFOGCOORDFVPROC)(const GLfloat * coord);
typedef void (CODEGEN_FUNCPTR * PFNGLSECONDARYCOLOR3BPROC)(GLbyte red, GLbyte green, GLbyte blue);
typedef void (CODEGEN_FUNCPTR * PFNGLSECONDARYCOLOR3BVPROC)(const GLbyte * v);
typedef void (CODEGEN_FUNCPTR * PFNGLSECONDARYCOLOR3DPROC)(GLdouble red, GLdouble green, GLdouble blue);
typedef void (CODEGEN_FUNCPTR * PFNGLSECONDARYCOLOR3DVPROC)(const GLdouble * v);
typedef void (CODEGEN_FUNCPTR * PFNGLSECONDARYCOLOR3FPROC)(GLfloat red, GLfloat green, GLfloat blue);
typedef void (CODEGEN_FUNCPTR * PFNGLSECONDARYCOLOR3FVPROC)(const GLfloat * v);
typedef void (CODEGEN_FUNCPTR * PFNGLSECONDARYCOLOR3IPROC)(GLint red, GLint green, GLint blue);
typedef void (CODEGEN_FUNCPTR * PFNGLSECONDARYCOLOR3IVPROC)(const GLint * v);
typedef void (CODEGEN_FUNCPTR * PFNGLSECONDARYCOLOR3SPROC)(GLshort red, GLshort green, GLshort blue);
typedef void (CODEGEN_FUNCPTR * PFNGLSECONDARYCOLOR3SVPROC)(const GLshort * v);
typedef void (CODEGEN_FUNCPTR * PFNGLSECONDARYCOLOR3UBPROC)(GLubyte red, GLubyte green, GLubyte blue);
typedef void (CODEGEN_FUNCPTR * PFNGLSECONDARYCOLOR3UBVPROC)(const GLubyte * v);
typedef void (CODEGEN_FUNCPTR * PFNGLSECONDARYCOLOR3UIPROC)(GLuint red, GLuint green, GLuint blue);
typedef void (CODEGEN_FUNCPTR * PFNGLSECONDARYCOLOR3UIVPROC)(const GLuint * v);
typedef void (CODEGEN_FUNCPTR * PFNGLSECONDARYCOLOR3USPROC)(GLushort red, GLushort green, GLushort blue);
typedef void (CODEGEN_FUNCPTR * PFNGLSECONDARYCOLOR3USVPROC)(const GLushort * v);
typedef void (CODEGEN_FUNCPTR * PFNGLSECONDARYCOLORPOINTERPROC)(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer);
typedef void (CODEGEN_FUNCPTR * PFNGLWINDOWPOS2DPROC)(GLdouble x, GLdouble y);
typedef void (CODEGEN_FUNCPTR * PFNGLWINDOWPOS2DVPROC)(const GLdouble * v);
typedef void (CODEGEN_FUNCPTR * PFNGLWINDOWPOS2FPROC)(GLfloat x, GLfloat y);
typedef void (CODEGEN_FUNCPTR * PFNGLWINDOWPOS2FVPROC)(const GLfloat * v);
typedef void (CODEGEN_FUNCPTR * PFNGLWINDOWPOS2IPROC)(GLint x, GLint y);
typedef void (CODEGEN_FUNCPTR * PFNGLWINDOWPOS2IVPROC)(const GLint * v);
typedef void (CODEGEN_FUNCPTR * PFNGLWINDOWPOS2SPROC)(GLshort x, GLshort y);
typedef void (CODEGEN_FUNCPTR * PFNGLWINDOWPOS2SVPROC)(const GLshort * v);
typedef void (CODEGEN_FUNCPTR * PFNGLWINDOWPOS3DPROC)(GLdouble x, GLdouble y, GLdouble z);
typedef void (CODEGEN_FUNCPTR * PFNGLWINDOWPOS3DVPROC)(const GLdouble * v);
typedef void (CODEGEN_FUNCPTR * PFNGLWINDOWPOS3FPROC)(GLfloat x, GLfloat y, GLfloat z);
typedef void (CODEGEN_FUNCPTR * PFNGLWINDOWPOS3FVPROC)(const GLfloat * v);
typedef void (CODEGEN_FUNCPTR * PFNGLWINDOWPOS3IPROC)(GLint x, GLint y, GLint z);
typedef void (CODEGEN_FUNCPTR * PFNGLWINDOWPOS3IVPROC)(const GLint * v);
typedef void (CODEGEN_FUNCPTR * PFNGLWINDOWPOS3SPROC)(GLshort x, GLshort y, GLshort z);
typedef void (CODEGEN_FUNCPTR * PFNGLWINDOWPOS3SVPROC)(const GLshort * v);

extern PFNGLFOGCOORDPOINTERPROC _funcptr_glFogCoordPointer;
#define glFogCoordPointer _funcptr_glFogCoordPointer
extern PFNGLFOGCOORDDPROC _funcptr_glFogCoordd;
#define glFogCoordd _funcptr_glFogCoordd
extern PFNGLFOGCOORDDVPROC _funcptr_glFogCoorddv;
#define glFogCoorddv _funcptr_glFogCoorddv
extern PFNGLFOGCOORDFPROC _funcptr_glFogCoordf;
#define glFogCoordf _funcptr_glFogCoordf
extern PFNGLFOGCOORDFVPROC _funcptr_glFogCoordfv;
#define glFogCoordfv _funcptr_glFogCoordfv
extern PFNGLSECONDARYCOLOR3BPROC _funcptr_glSecondaryColor3b;
#define glSecondaryColor3b _funcptr_glSecondaryColor3b
extern PFNGLSECONDARYCOLOR3BVPROC _funcptr_glSecondaryColor3bv;
#define glSecondaryColor3bv _funcptr_glSecondaryColor3bv
extern PFNGLSECONDARYCOLOR3DPROC _funcptr_glSecondaryColor3d;
#define glSecondaryColor3d _funcptr_glSecondaryColor3d
extern PFNGLSECONDARYCOLOR3DVPROC _funcptr_glSecondaryColor3dv;
#define glSecondaryColor3dv _funcptr_glSecondaryColor3dv
extern PFNGLSECONDARYCOLOR3FPROC _funcptr_glSecondaryColor3f;
#define glSecondaryColor3f _funcptr_glSecondaryColor3f
extern PFNGLSECONDARYCOLOR3FVPROC _funcptr_glSecondaryColor3fv;
#define glSecondaryColor3fv _funcptr_glSecondaryColor3fv
extern PFNGLSECONDARYCOLOR3IPROC _funcptr_glSecondaryColor3i;
#define glSecondaryColor3i _funcptr_glSecondaryColor3i
extern PFNGLSECONDARYCOLOR3IVPROC _funcptr_glSecondaryColor3iv;
#define glSecondaryColor3iv _funcptr_glSecondaryColor3iv
extern PFNGLSECONDARYCOLOR3SPROC _funcptr_glSecondaryColor3s;
#define glSecondaryColor3s _funcptr_glSecondaryColor3s
extern PFNGLSECONDARYCOLOR3SVPROC _funcptr_glSecondaryColor3sv;
#define glSecondaryColor3sv _funcptr_glSecondaryColor3sv
extern PFNGLSECONDARYCOLOR3UBPROC _funcptr_glSecondaryColor3ub;
#define glSecondaryColor3ub _funcptr_glSecondaryColor3ub
extern PFNGLSECONDARYCOLOR3UBVPROC _funcptr_glSecondaryColor3ubv;
#define glSecondaryColor3ubv _funcptr_glSecondaryColor3ubv
extern PFNGLSECONDARYCOLOR3UIPROC _funcptr_glSecondaryColor3ui;
#define glSecondaryColor3ui _funcptr_glSecondaryColor3ui
extern PFNGLSECONDARYCOLOR3UIVPROC _funcptr_glSecondaryColor3uiv;
#define glSecondaryColor3uiv _funcptr_glSecondaryColor3uiv
extern PFNGLSECONDARYCOLOR3USPROC _funcptr_glSecondaryColor3us;
#define glSecondaryColor3us _funcptr_glSecondaryColor3us
extern PFNGLSECONDARYCOLOR3USVPROC _funcptr_glSecondaryColor3usv;
#define glSecondaryColor3usv _funcptr_glSecondaryColor3usv
extern PFNGLSECONDARYCOLORPOINTERPROC _funcptr_glSecondaryColorPointer;
#define glSecondaryColorPointer _funcptr_glSecondaryColorPointer
extern PFNGLWINDOWPOS2DPROC _funcptr_glWindowPos2d;
#define glWindowPos2d _funcptr_glWindowPos2d
extern PFNGLWINDOWPOS2DVPROC _funcptr_glWindowPos2dv;
#define glWindowPos2dv _funcptr_glWindowPos2dv
extern PFNGLWINDOWPOS2FPROC _funcptr_glWindowPos2f;
#define glWindowPos2f _funcptr_glWindowPos2f
extern PFNGLWINDOWPOS2FVPROC _funcptr_glWindowPos2fv;
#define glWindowPos2fv _funcptr_glWindowPos2fv
extern PFNGLWINDOWPOS2IPROC _funcptr_glWindowPos2i;
#define glWindowPos2i _funcptr_glWindowPos2i
extern PFNGLWINDOWPOS2IVPROC _funcptr_glWindowPos2iv;
#define glWindowPos2iv _funcptr_glWindowPos2iv
extern PFNGLWINDOWPOS2SPROC _funcptr_glWindowPos2s;
#define glWindowPos2s _funcptr_glWindowPos2s
extern PFNGLWINDOWPOS2SVPROC _funcptr_glWindowPos2sv;
#define glWindowPos2sv _funcptr_glWindowPos2sv
extern PFNGLWINDOWPOS3DPROC _funcptr_glWindowPos3d;
#define glWindowPos3d _funcptr_glWindowPos3d
extern PFNGLWINDOWPOS3DVPROC _funcptr_glWindowPos3dv;
#define glWindowPos3dv _funcptr_glWindowPos3dv
extern PFNGLWINDOWPOS3FPROC _funcptr_glWindowPos3f;
#define glWindowPos3f _funcptr_glWindowPos3f
extern PFNGLWINDOWPOS3FVPROC _funcptr_glWindowPos3fv;
#define glWindowPos3fv _funcptr_glWindowPos3fv
extern PFNGLWINDOWPOS3IPROC _funcptr_glWindowPos3i;
#define glWindowPos3i _funcptr_glWindowPos3i
extern PFNGLWINDOWPOS3IVPROC _funcptr_glWindowPos3iv;
#define glWindowPos3iv _funcptr_glWindowPos3iv
extern PFNGLWINDOWPOS3SPROC _funcptr_glWindowPos3s;
#define glWindowPos3s _funcptr_glWindowPos3s
extern PFNGLWINDOWPOS3SVPROC _funcptr_glWindowPos3sv;
#define glWindowPos3sv _funcptr_glWindowPos3sv

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif /*OPENGL_GEN_CORE_REM1_4_H*/
