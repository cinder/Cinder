#ifndef OPENGL_GEN_CORE_1_0_H
#define OPENGL_GEN_CORE_1_0_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
typedef void (CODEGEN_FUNCPTR * PFNGLBLENDFUNCPROC)(GLenum sfactor, GLenum dfactor);
typedef void (CODEGEN_FUNCPTR * PFNGLCLEARPROC)(GLbitfield mask);
typedef void (CODEGEN_FUNCPTR * PFNGLCLEARCOLORPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void (CODEGEN_FUNCPTR * PFNGLCLEARDEPTHPROC)(GLdouble depth);
typedef void (CODEGEN_FUNCPTR * PFNGLCLEARSTENCILPROC)(GLint s);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLORMASKPROC)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
typedef void (CODEGEN_FUNCPTR * PFNGLCULLFACEPROC)(GLenum mode);
typedef void (CODEGEN_FUNCPTR * PFNGLDEPTHFUNCPROC)(GLenum func);
typedef void (CODEGEN_FUNCPTR * PFNGLDEPTHMASKPROC)(GLboolean flag);
typedef void (CODEGEN_FUNCPTR * PFNGLDEPTHRANGEPROC)(GLdouble ren_near, GLdouble ren_far);
typedef void (CODEGEN_FUNCPTR * PFNGLDISABLEPROC)(GLenum cap);
typedef void (CODEGEN_FUNCPTR * PFNGLDRAWBUFFERPROC)(GLenum mode);
typedef void (CODEGEN_FUNCPTR * PFNGLENABLEPROC)(GLenum cap);
typedef void (CODEGEN_FUNCPTR * PFNGLFINISHPROC)();
typedef void (CODEGEN_FUNCPTR * PFNGLFLUSHPROC)();
typedef void (CODEGEN_FUNCPTR * PFNGLFRONTFACEPROC)(GLenum mode);
typedef void (CODEGEN_FUNCPTR * PFNGLGETBOOLEANVPROC)(GLenum pname, GLboolean * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETDOUBLEVPROC)(GLenum pname, GLdouble * params);
typedef GLenum (CODEGEN_FUNCPTR * PFNGLGETERRORPROC)();
typedef void (CODEGEN_FUNCPTR * PFNGLGETFLOATVPROC)(GLenum pname, GLfloat * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETINTEGERVPROC)(GLenum pname, GLint * params);
typedef const GLubyte * (CODEGEN_FUNCPTR * PFNGLGETSTRINGPROC)(GLenum name);
typedef void (CODEGEN_FUNCPTR * PFNGLGETTEXIMAGEPROC)(GLenum target, GLint level, GLenum format, GLenum type, GLvoid * pixels);
typedef void (CODEGEN_FUNCPTR * PFNGLGETTEXLEVELPARAMETERFVPROC)(GLenum target, GLint level, GLenum pname, GLfloat * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETTEXLEVELPARAMETERIVPROC)(GLenum target, GLint level, GLenum pname, GLint * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETTEXPARAMETERFVPROC)(GLenum target, GLenum pname, GLfloat * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETTEXPARAMETERIVPROC)(GLenum target, GLenum pname, GLint * params);
typedef void (CODEGEN_FUNCPTR * PFNGLHINTPROC)(GLenum target, GLenum mode);
typedef GLboolean (CODEGEN_FUNCPTR * PFNGLISENABLEDPROC)(GLenum cap);
typedef void (CODEGEN_FUNCPTR * PFNGLLINEWIDTHPROC)(GLfloat width);
typedef void (CODEGEN_FUNCPTR * PFNGLLOGICOPPROC)(GLenum opcode);
typedef void (CODEGEN_FUNCPTR * PFNGLPIXELSTOREFPROC)(GLenum pname, GLfloat param);
typedef void (CODEGEN_FUNCPTR * PFNGLPIXELSTOREIPROC)(GLenum pname, GLint param);
typedef void (CODEGEN_FUNCPTR * PFNGLPOINTSIZEPROC)(GLfloat size);
typedef void (CODEGEN_FUNCPTR * PFNGLPOLYGONMODEPROC)(GLenum face, GLenum mode);
typedef void (CODEGEN_FUNCPTR * PFNGLREADBUFFERPROC)(GLenum mode);
typedef void (CODEGEN_FUNCPTR * PFNGLREADPIXELSPROC)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid * pixels);
typedef void (CODEGEN_FUNCPTR * PFNGLSCISSORPROC)(GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (CODEGEN_FUNCPTR * PFNGLSTENCILFUNCPROC)(GLenum func, GLint ref, GLuint mask);
typedef void (CODEGEN_FUNCPTR * PFNGLSTENCILMASKPROC)(GLuint mask);
typedef void (CODEGEN_FUNCPTR * PFNGLSTENCILOPPROC)(GLenum fail, GLenum zfail, GLenum zpass);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXIMAGE1DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid * pixels);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXIMAGE2DPROC)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * pixels);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXPARAMETERFPROC)(GLenum target, GLenum pname, GLfloat param);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXPARAMETERFVPROC)(GLenum target, GLenum pname, const GLfloat * params);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXPARAMETERIPROC)(GLenum target, GLenum pname, GLint param);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXPARAMETERIVPROC)(GLenum target, GLenum pname, const GLint * params);
typedef void (CODEGEN_FUNCPTR * PFNGLVIEWPORTPROC)(GLint x, GLint y, GLsizei width, GLsizei height);

extern PFNGLBLENDFUNCPROC _funcptr_glBlendFunc;
#define glBlendFunc _funcptr_glBlendFunc
extern PFNGLCLEARPROC _funcptr_glClear;
#define glClear _funcptr_glClear
extern PFNGLCLEARCOLORPROC _funcptr_glClearColor;
#define glClearColor _funcptr_glClearColor
extern PFNGLCLEARDEPTHPROC _funcptr_glClearDepth;
#define glClearDepth _funcptr_glClearDepth
extern PFNGLCLEARSTENCILPROC _funcptr_glClearStencil;
#define glClearStencil _funcptr_glClearStencil
extern PFNGLCOLORMASKPROC _funcptr_glColorMask;
#define glColorMask _funcptr_glColorMask
extern PFNGLCULLFACEPROC _funcptr_glCullFace;
#define glCullFace _funcptr_glCullFace
extern PFNGLDEPTHFUNCPROC _funcptr_glDepthFunc;
#define glDepthFunc _funcptr_glDepthFunc
extern PFNGLDEPTHMASKPROC _funcptr_glDepthMask;
#define glDepthMask _funcptr_glDepthMask
extern PFNGLDEPTHRANGEPROC _funcptr_glDepthRange;
#define glDepthRange _funcptr_glDepthRange
extern PFNGLDISABLEPROC _funcptr_glDisable;
#define glDisable _funcptr_glDisable
extern PFNGLDRAWBUFFERPROC _funcptr_glDrawBuffer;
#define glDrawBuffer _funcptr_glDrawBuffer
extern PFNGLENABLEPROC _funcptr_glEnable;
#define glEnable _funcptr_glEnable
extern PFNGLFINISHPROC _funcptr_glFinish;
#define glFinish _funcptr_glFinish
extern PFNGLFLUSHPROC _funcptr_glFlush;
#define glFlush _funcptr_glFlush
extern PFNGLFRONTFACEPROC _funcptr_glFrontFace;
#define glFrontFace _funcptr_glFrontFace
extern PFNGLGETBOOLEANVPROC _funcptr_glGetBooleanv;
#define glGetBooleanv _funcptr_glGetBooleanv
extern PFNGLGETDOUBLEVPROC _funcptr_glGetDoublev;
#define glGetDoublev _funcptr_glGetDoublev
extern PFNGLGETERRORPROC _funcptr_glGetError;
#define glGetError _funcptr_glGetError
extern PFNGLGETFLOATVPROC _funcptr_glGetFloatv;
#define glGetFloatv _funcptr_glGetFloatv
extern PFNGLGETINTEGERVPROC _funcptr_glGetIntegerv;
#define glGetIntegerv _funcptr_glGetIntegerv
extern PFNGLGETSTRINGPROC _funcptr_glGetString;
#define glGetString _funcptr_glGetString
extern PFNGLGETTEXIMAGEPROC _funcptr_glGetTexImage;
#define glGetTexImage _funcptr_glGetTexImage
extern PFNGLGETTEXLEVELPARAMETERFVPROC _funcptr_glGetTexLevelParameterfv;
#define glGetTexLevelParameterfv _funcptr_glGetTexLevelParameterfv
extern PFNGLGETTEXLEVELPARAMETERIVPROC _funcptr_glGetTexLevelParameteriv;
#define glGetTexLevelParameteriv _funcptr_glGetTexLevelParameteriv
extern PFNGLGETTEXPARAMETERFVPROC _funcptr_glGetTexParameterfv;
#define glGetTexParameterfv _funcptr_glGetTexParameterfv
extern PFNGLGETTEXPARAMETERIVPROC _funcptr_glGetTexParameteriv;
#define glGetTexParameteriv _funcptr_glGetTexParameteriv
extern PFNGLHINTPROC _funcptr_glHint;
#define glHint _funcptr_glHint
extern PFNGLISENABLEDPROC _funcptr_glIsEnabled;
#define glIsEnabled _funcptr_glIsEnabled
extern PFNGLLINEWIDTHPROC _funcptr_glLineWidth;
#define glLineWidth _funcptr_glLineWidth
extern PFNGLLOGICOPPROC _funcptr_glLogicOp;
#define glLogicOp _funcptr_glLogicOp
extern PFNGLPIXELSTOREFPROC _funcptr_glPixelStoref;
#define glPixelStoref _funcptr_glPixelStoref
extern PFNGLPIXELSTOREIPROC _funcptr_glPixelStorei;
#define glPixelStorei _funcptr_glPixelStorei
extern PFNGLPOINTSIZEPROC _funcptr_glPointSize;
#define glPointSize _funcptr_glPointSize
extern PFNGLPOLYGONMODEPROC _funcptr_glPolygonMode;
#define glPolygonMode _funcptr_glPolygonMode
extern PFNGLREADBUFFERPROC _funcptr_glReadBuffer;
#define glReadBuffer _funcptr_glReadBuffer
extern PFNGLREADPIXELSPROC _funcptr_glReadPixels;
#define glReadPixels _funcptr_glReadPixels
extern PFNGLSCISSORPROC _funcptr_glScissor;
#define glScissor _funcptr_glScissor
extern PFNGLSTENCILFUNCPROC _funcptr_glStencilFunc;
#define glStencilFunc _funcptr_glStencilFunc
extern PFNGLSTENCILMASKPROC _funcptr_glStencilMask;
#define glStencilMask _funcptr_glStencilMask
extern PFNGLSTENCILOPPROC _funcptr_glStencilOp;
#define glStencilOp _funcptr_glStencilOp
extern PFNGLTEXIMAGE1DPROC _funcptr_glTexImage1D;
#define glTexImage1D _funcptr_glTexImage1D
extern PFNGLTEXIMAGE2DPROC _funcptr_glTexImage2D;
#define glTexImage2D _funcptr_glTexImage2D
extern PFNGLTEXPARAMETERFPROC _funcptr_glTexParameterf;
#define glTexParameterf _funcptr_glTexParameterf
extern PFNGLTEXPARAMETERFVPROC _funcptr_glTexParameterfv;
#define glTexParameterfv _funcptr_glTexParameterfv
extern PFNGLTEXPARAMETERIPROC _funcptr_glTexParameteri;
#define glTexParameteri _funcptr_glTexParameteri
extern PFNGLTEXPARAMETERIVPROC _funcptr_glTexParameteriv;
#define glTexParameteriv _funcptr_glTexParameteriv
extern PFNGLVIEWPORTPROC _funcptr_glViewport;
#define glViewport _funcptr_glViewport

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif /*OPENGL_GEN_CORE_1_0_H*/
