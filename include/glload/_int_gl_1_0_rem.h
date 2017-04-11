#ifndef OPENGL_GEN_CORE_REM1_0_H
#define OPENGL_GEN_CORE_REM1_0_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
typedef void (CODEGEN_FUNCPTR * PFNGLACCUMPROC)(GLenum op, GLfloat value);
typedef void (CODEGEN_FUNCPTR * PFNGLALPHAFUNCPROC)(GLenum func, GLfloat ref);
typedef void (CODEGEN_FUNCPTR * PFNGLBEGINPROC)(GLenum mode);
typedef void (CODEGEN_FUNCPTR * PFNGLBITMAPPROC)(GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte * bitmap);
typedef void (CODEGEN_FUNCPTR * PFNGLCALLLISTPROC)(GLuint list);
typedef void (CODEGEN_FUNCPTR * PFNGLCALLLISTSPROC)(GLsizei n, GLenum type, const GLvoid * lists);
typedef void (CODEGEN_FUNCPTR * PFNGLCLEARACCUMPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void (CODEGEN_FUNCPTR * PFNGLCLEARINDEXPROC)(GLfloat c);
typedef void (CODEGEN_FUNCPTR * PFNGLCLIPPLANEPROC)(GLenum plane, const GLdouble * equation);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR3BPROC)(GLbyte red, GLbyte green, GLbyte blue);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR3BVPROC)(const GLbyte * v);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR3DPROC)(GLdouble red, GLdouble green, GLdouble blue);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR3DVPROC)(const GLdouble * v);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR3FPROC)(GLfloat red, GLfloat green, GLfloat blue);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR3FVPROC)(const GLfloat * v);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR3IPROC)(GLint red, GLint green, GLint blue);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR3IVPROC)(const GLint * v);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR3SPROC)(GLshort red, GLshort green, GLshort blue);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR3SVPROC)(const GLshort * v);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR3UBPROC)(GLubyte red, GLubyte green, GLubyte blue);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR3UBVPROC)(const GLubyte * v);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR3UIPROC)(GLuint red, GLuint green, GLuint blue);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR3UIVPROC)(const GLuint * v);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR3USPROC)(GLushort red, GLushort green, GLushort blue);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR3USVPROC)(const GLushort * v);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR4BPROC)(GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR4BVPROC)(const GLbyte * v);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR4DPROC)(GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR4DVPROC)(const GLdouble * v);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR4FPROC)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR4FVPROC)(const GLfloat * v);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR4IPROC)(GLint red, GLint green, GLint blue, GLint alpha);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR4IVPROC)(const GLint * v);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR4SPROC)(GLshort red, GLshort green, GLshort blue, GLshort alpha);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR4SVPROC)(const GLshort * v);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR4UBPROC)(GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR4UBVPROC)(const GLubyte * v);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR4UIPROC)(GLuint red, GLuint green, GLuint blue, GLuint alpha);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR4UIVPROC)(const GLuint * v);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR4USPROC)(GLushort red, GLushort green, GLushort blue, GLushort alpha);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLOR4USVPROC)(const GLushort * v);
typedef void (CODEGEN_FUNCPTR * PFNGLCOLORMATERIALPROC)(GLenum face, GLenum mode);
typedef void (CODEGEN_FUNCPTR * PFNGLCOPYPIXELSPROC)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum type);
typedef void (CODEGEN_FUNCPTR * PFNGLDELETELISTSPROC)(GLuint list, GLsizei range);
typedef void (CODEGEN_FUNCPTR * PFNGLDRAWPIXELSPROC)(GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * pixels);
typedef void (CODEGEN_FUNCPTR * PFNGLEDGEFLAGPROC)(GLboolean flag);
typedef void (CODEGEN_FUNCPTR * PFNGLEDGEFLAGVPROC)(const GLboolean * flag);
typedef void (CODEGEN_FUNCPTR * PFNGLENDPROC)();
typedef void (CODEGEN_FUNCPTR * PFNGLENDLISTPROC)();
typedef void (CODEGEN_FUNCPTR * PFNGLEVALCOORD1DPROC)(GLdouble u);
typedef void (CODEGEN_FUNCPTR * PFNGLEVALCOORD1DVPROC)(const GLdouble * u);
typedef void (CODEGEN_FUNCPTR * PFNGLEVALCOORD1FPROC)(GLfloat u);
typedef void (CODEGEN_FUNCPTR * PFNGLEVALCOORD1FVPROC)(const GLfloat * u);
typedef void (CODEGEN_FUNCPTR * PFNGLEVALCOORD2DPROC)(GLdouble u, GLdouble v);
typedef void (CODEGEN_FUNCPTR * PFNGLEVALCOORD2DVPROC)(const GLdouble * u);
typedef void (CODEGEN_FUNCPTR * PFNGLEVALCOORD2FPROC)(GLfloat u, GLfloat v);
typedef void (CODEGEN_FUNCPTR * PFNGLEVALCOORD2FVPROC)(const GLfloat * u);
typedef void (CODEGEN_FUNCPTR * PFNGLEVALMESH1PROC)(GLenum mode, GLint i1, GLint i2);
typedef void (CODEGEN_FUNCPTR * PFNGLEVALMESH2PROC)(GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2);
typedef void (CODEGEN_FUNCPTR * PFNGLEVALPOINT1PROC)(GLint i);
typedef void (CODEGEN_FUNCPTR * PFNGLEVALPOINT2PROC)(GLint i, GLint j);
typedef void (CODEGEN_FUNCPTR * PFNGLFEEDBACKBUFFERPROC)(GLsizei size, GLenum type, GLfloat * buffer);
typedef void (CODEGEN_FUNCPTR * PFNGLFOGFPROC)(GLenum pname, GLfloat param);
typedef void (CODEGEN_FUNCPTR * PFNGLFOGFVPROC)(GLenum pname, const GLfloat * params);
typedef void (CODEGEN_FUNCPTR * PFNGLFOGIPROC)(GLenum pname, GLint param);
typedef void (CODEGEN_FUNCPTR * PFNGLFOGIVPROC)(GLenum pname, const GLint * params);
typedef void (CODEGEN_FUNCPTR * PFNGLFRUSTUMPROC)(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
typedef GLuint (CODEGEN_FUNCPTR * PFNGLGENLISTSPROC)(GLsizei range);
typedef void (CODEGEN_FUNCPTR * PFNGLGETCLIPPLANEPROC)(GLenum plane, GLdouble * equation);
typedef void (CODEGEN_FUNCPTR * PFNGLGETLIGHTFVPROC)(GLenum light, GLenum pname, GLfloat * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETLIGHTIVPROC)(GLenum light, GLenum pname, GLint * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETMAPDVPROC)(GLenum target, GLenum query, GLdouble * v);
typedef void (CODEGEN_FUNCPTR * PFNGLGETMAPFVPROC)(GLenum target, GLenum query, GLfloat * v);
typedef void (CODEGEN_FUNCPTR * PFNGLGETMAPIVPROC)(GLenum target, GLenum query, GLint * v);
typedef void (CODEGEN_FUNCPTR * PFNGLGETMATERIALFVPROC)(GLenum face, GLenum pname, GLfloat * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETMATERIALIVPROC)(GLenum face, GLenum pname, GLint * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETPIXELMAPFVPROC)(GLenum map, GLfloat * values);
typedef void (CODEGEN_FUNCPTR * PFNGLGETPIXELMAPUIVPROC)(GLenum map, GLuint * values);
typedef void (CODEGEN_FUNCPTR * PFNGLGETPIXELMAPUSVPROC)(GLenum map, GLushort * values);
typedef void (CODEGEN_FUNCPTR * PFNGLGETPOLYGONSTIPPLEPROC)(GLubyte * mask);
typedef void (CODEGEN_FUNCPTR * PFNGLGETTEXENVFVPROC)(GLenum target, GLenum pname, GLfloat * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETTEXENVIVPROC)(GLenum target, GLenum pname, GLint * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETTEXGENDVPROC)(GLenum coord, GLenum pname, GLdouble * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETTEXGENFVPROC)(GLenum coord, GLenum pname, GLfloat * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETTEXGENIVPROC)(GLenum coord, GLenum pname, GLint * params);
typedef void (CODEGEN_FUNCPTR * PFNGLINDEXMASKPROC)(GLuint mask);
typedef void (CODEGEN_FUNCPTR * PFNGLINDEXDPROC)(GLdouble c);
typedef void (CODEGEN_FUNCPTR * PFNGLINDEXDVPROC)(const GLdouble * c);
typedef void (CODEGEN_FUNCPTR * PFNGLINDEXFPROC)(GLfloat c);
typedef void (CODEGEN_FUNCPTR * PFNGLINDEXFVPROC)(const GLfloat * c);
typedef void (CODEGEN_FUNCPTR * PFNGLINDEXIPROC)(GLint c);
typedef void (CODEGEN_FUNCPTR * PFNGLINDEXIVPROC)(const GLint * c);
typedef void (CODEGEN_FUNCPTR * PFNGLINDEXSPROC)(GLshort c);
typedef void (CODEGEN_FUNCPTR * PFNGLINDEXSVPROC)(const GLshort * c);
typedef void (CODEGEN_FUNCPTR * PFNGLINITNAMESPROC)();
typedef GLboolean (CODEGEN_FUNCPTR * PFNGLISLISTPROC)(GLuint list);
typedef void (CODEGEN_FUNCPTR * PFNGLLIGHTMODELFPROC)(GLenum pname, GLfloat param);
typedef void (CODEGEN_FUNCPTR * PFNGLLIGHTMODELFVPROC)(GLenum pname, const GLfloat * params);
typedef void (CODEGEN_FUNCPTR * PFNGLLIGHTMODELIPROC)(GLenum pname, GLint param);
typedef void (CODEGEN_FUNCPTR * PFNGLLIGHTMODELIVPROC)(GLenum pname, const GLint * params);
typedef void (CODEGEN_FUNCPTR * PFNGLLIGHTFPROC)(GLenum light, GLenum pname, GLfloat param);
typedef void (CODEGEN_FUNCPTR * PFNGLLIGHTFVPROC)(GLenum light, GLenum pname, const GLfloat * params);
typedef void (CODEGEN_FUNCPTR * PFNGLLIGHTIPROC)(GLenum light, GLenum pname, GLint param);
typedef void (CODEGEN_FUNCPTR * PFNGLLIGHTIVPROC)(GLenum light, GLenum pname, const GLint * params);
typedef void (CODEGEN_FUNCPTR * PFNGLLINESTIPPLEPROC)(GLint factor, GLushort pattern);
typedef void (CODEGEN_FUNCPTR * PFNGLLISTBASEPROC)(GLuint base);
typedef void (CODEGEN_FUNCPTR * PFNGLLOADIDENTITYPROC)();
typedef void (CODEGEN_FUNCPTR * PFNGLLOADMATRIXDPROC)(const GLdouble * m);
typedef void (CODEGEN_FUNCPTR * PFNGLLOADMATRIXFPROC)(const GLfloat * m);
typedef void (CODEGEN_FUNCPTR * PFNGLLOADNAMEPROC)(GLuint name);
typedef void (CODEGEN_FUNCPTR * PFNGLMAP1DPROC)(GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble * points);
typedef void (CODEGEN_FUNCPTR * PFNGLMAP1FPROC)(GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat * points);
typedef void (CODEGEN_FUNCPTR * PFNGLMAP2DPROC)(GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble * points);
typedef void (CODEGEN_FUNCPTR * PFNGLMAP2FPROC)(GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat * points);
typedef void (CODEGEN_FUNCPTR * PFNGLMAPGRID1DPROC)(GLint un, GLdouble u1, GLdouble u2);
typedef void (CODEGEN_FUNCPTR * PFNGLMAPGRID1FPROC)(GLint un, GLfloat u1, GLfloat u2);
typedef void (CODEGEN_FUNCPTR * PFNGLMAPGRID2DPROC)(GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2);
typedef void (CODEGEN_FUNCPTR * PFNGLMAPGRID2FPROC)(GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2);
typedef void (CODEGEN_FUNCPTR * PFNGLMATERIALFPROC)(GLenum face, GLenum pname, GLfloat param);
typedef void (CODEGEN_FUNCPTR * PFNGLMATERIALFVPROC)(GLenum face, GLenum pname, const GLfloat * params);
typedef void (CODEGEN_FUNCPTR * PFNGLMATERIALIPROC)(GLenum face, GLenum pname, GLint param);
typedef void (CODEGEN_FUNCPTR * PFNGLMATERIALIVPROC)(GLenum face, GLenum pname, const GLint * params);
typedef void (CODEGEN_FUNCPTR * PFNGLMATRIXMODEPROC)(GLenum mode);
typedef void (CODEGEN_FUNCPTR * PFNGLMULTMATRIXDPROC)(const GLdouble * m);
typedef void (CODEGEN_FUNCPTR * PFNGLMULTMATRIXFPROC)(const GLfloat * m);
typedef void (CODEGEN_FUNCPTR * PFNGLNEWLISTPROC)(GLuint list, GLenum mode);
typedef void (CODEGEN_FUNCPTR * PFNGLNORMAL3BPROC)(GLbyte nx, GLbyte ny, GLbyte nz);
typedef void (CODEGEN_FUNCPTR * PFNGLNORMAL3BVPROC)(const GLbyte * v);
typedef void (CODEGEN_FUNCPTR * PFNGLNORMAL3DPROC)(GLdouble nx, GLdouble ny, GLdouble nz);
typedef void (CODEGEN_FUNCPTR * PFNGLNORMAL3DVPROC)(const GLdouble * v);
typedef void (CODEGEN_FUNCPTR * PFNGLNORMAL3FPROC)(GLfloat nx, GLfloat ny, GLfloat nz);
typedef void (CODEGEN_FUNCPTR * PFNGLNORMAL3FVPROC)(const GLfloat * v);
typedef void (CODEGEN_FUNCPTR * PFNGLNORMAL3IPROC)(GLint nx, GLint ny, GLint nz);
typedef void (CODEGEN_FUNCPTR * PFNGLNORMAL3IVPROC)(const GLint * v);
typedef void (CODEGEN_FUNCPTR * PFNGLNORMAL3SPROC)(GLshort nx, GLshort ny, GLshort nz);
typedef void (CODEGEN_FUNCPTR * PFNGLNORMAL3SVPROC)(const GLshort * v);
typedef void (CODEGEN_FUNCPTR * PFNGLORTHOPROC)(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
typedef void (CODEGEN_FUNCPTR * PFNGLPASSTHROUGHPROC)(GLfloat token);
typedef void (CODEGEN_FUNCPTR * PFNGLPIXELMAPFVPROC)(GLenum map, GLsizei mapsize, const GLfloat * values);
typedef void (CODEGEN_FUNCPTR * PFNGLPIXELMAPUIVPROC)(GLenum map, GLsizei mapsize, const GLuint * values);
typedef void (CODEGEN_FUNCPTR * PFNGLPIXELMAPUSVPROC)(GLenum map, GLsizei mapsize, const GLushort * values);
typedef void (CODEGEN_FUNCPTR * PFNGLPIXELTRANSFERFPROC)(GLenum pname, GLfloat param);
typedef void (CODEGEN_FUNCPTR * PFNGLPIXELTRANSFERIPROC)(GLenum pname, GLint param);
typedef void (CODEGEN_FUNCPTR * PFNGLPIXELZOOMPROC)(GLfloat xfactor, GLfloat yfactor);
typedef void (CODEGEN_FUNCPTR * PFNGLPOLYGONSTIPPLEPROC)(const GLubyte * mask);
typedef void (CODEGEN_FUNCPTR * PFNGLPOPATTRIBPROC)();
typedef void (CODEGEN_FUNCPTR * PFNGLPOPMATRIXPROC)();
typedef void (CODEGEN_FUNCPTR * PFNGLPOPNAMEPROC)();
typedef void (CODEGEN_FUNCPTR * PFNGLPUSHATTRIBPROC)(GLbitfield mask);
typedef void (CODEGEN_FUNCPTR * PFNGLPUSHMATRIXPROC)();
typedef void (CODEGEN_FUNCPTR * PFNGLPUSHNAMEPROC)(GLuint name);
typedef void (CODEGEN_FUNCPTR * PFNGLRASTERPOS2DPROC)(GLdouble x, GLdouble y);
typedef void (CODEGEN_FUNCPTR * PFNGLRASTERPOS2DVPROC)(const GLdouble * v);
typedef void (CODEGEN_FUNCPTR * PFNGLRASTERPOS2FPROC)(GLfloat x, GLfloat y);
typedef void (CODEGEN_FUNCPTR * PFNGLRASTERPOS2FVPROC)(const GLfloat * v);
typedef void (CODEGEN_FUNCPTR * PFNGLRASTERPOS2IPROC)(GLint x, GLint y);
typedef void (CODEGEN_FUNCPTR * PFNGLRASTERPOS2IVPROC)(const GLint * v);
typedef void (CODEGEN_FUNCPTR * PFNGLRASTERPOS2SPROC)(GLshort x, GLshort y);
typedef void (CODEGEN_FUNCPTR * PFNGLRASTERPOS2SVPROC)(const GLshort * v);
typedef void (CODEGEN_FUNCPTR * PFNGLRASTERPOS3DPROC)(GLdouble x, GLdouble y, GLdouble z);
typedef void (CODEGEN_FUNCPTR * PFNGLRASTERPOS3DVPROC)(const GLdouble * v);
typedef void (CODEGEN_FUNCPTR * PFNGLRASTERPOS3FPROC)(GLfloat x, GLfloat y, GLfloat z);
typedef void (CODEGEN_FUNCPTR * PFNGLRASTERPOS3FVPROC)(const GLfloat * v);
typedef void (CODEGEN_FUNCPTR * PFNGLRASTERPOS3IPROC)(GLint x, GLint y, GLint z);
typedef void (CODEGEN_FUNCPTR * PFNGLRASTERPOS3IVPROC)(const GLint * v);
typedef void (CODEGEN_FUNCPTR * PFNGLRASTERPOS3SPROC)(GLshort x, GLshort y, GLshort z);
typedef void (CODEGEN_FUNCPTR * PFNGLRASTERPOS3SVPROC)(const GLshort * v);
typedef void (CODEGEN_FUNCPTR * PFNGLRASTERPOS4DPROC)(GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (CODEGEN_FUNCPTR * PFNGLRASTERPOS4DVPROC)(const GLdouble * v);
typedef void (CODEGEN_FUNCPTR * PFNGLRASTERPOS4FPROC)(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (CODEGEN_FUNCPTR * PFNGLRASTERPOS4FVPROC)(const GLfloat * v);
typedef void (CODEGEN_FUNCPTR * PFNGLRASTERPOS4IPROC)(GLint x, GLint y, GLint z, GLint w);
typedef void (CODEGEN_FUNCPTR * PFNGLRASTERPOS4IVPROC)(const GLint * v);
typedef void (CODEGEN_FUNCPTR * PFNGLRASTERPOS4SPROC)(GLshort x, GLshort y, GLshort z, GLshort w);
typedef void (CODEGEN_FUNCPTR * PFNGLRASTERPOS4SVPROC)(const GLshort * v);
typedef void (CODEGEN_FUNCPTR * PFNGLRECTDPROC)(GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2);
typedef void (CODEGEN_FUNCPTR * PFNGLRECTDVPROC)(const GLdouble * v1, const GLdouble * v2);
typedef void (CODEGEN_FUNCPTR * PFNGLRECTFPROC)(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2);
typedef void (CODEGEN_FUNCPTR * PFNGLRECTFVPROC)(const GLfloat * v1, const GLfloat * v2);
typedef void (CODEGEN_FUNCPTR * PFNGLRECTIPROC)(GLint x1, GLint y1, GLint x2, GLint y2);
typedef void (CODEGEN_FUNCPTR * PFNGLRECTIVPROC)(const GLint * v1, const GLint * v2);
typedef void (CODEGEN_FUNCPTR * PFNGLRECTSPROC)(GLshort x1, GLshort y1, GLshort x2, GLshort y2);
typedef void (CODEGEN_FUNCPTR * PFNGLRECTSVPROC)(const GLshort * v1, const GLshort * v2);
typedef GLint (CODEGEN_FUNCPTR * PFNGLRENDERMODEPROC)(GLenum mode);
typedef void (CODEGEN_FUNCPTR * PFNGLROTATEDPROC)(GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
typedef void (CODEGEN_FUNCPTR * PFNGLROTATEFPROC)(GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
typedef void (CODEGEN_FUNCPTR * PFNGLSCALEDPROC)(GLdouble x, GLdouble y, GLdouble z);
typedef void (CODEGEN_FUNCPTR * PFNGLSCALEFPROC)(GLfloat x, GLfloat y, GLfloat z);
typedef void (CODEGEN_FUNCPTR * PFNGLSELECTBUFFERPROC)(GLsizei size, GLuint * buffer);
typedef void (CODEGEN_FUNCPTR * PFNGLSHADEMODELPROC)(GLenum mode);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD1DPROC)(GLdouble s);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD1DVPROC)(const GLdouble * v);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD1FPROC)(GLfloat s);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD1FVPROC)(const GLfloat * v);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD1IPROC)(GLint s);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD1IVPROC)(const GLint * v);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD1SPROC)(GLshort s);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD1SVPROC)(const GLshort * v);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD2DPROC)(GLdouble s, GLdouble t);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD2DVPROC)(const GLdouble * v);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD2FPROC)(GLfloat s, GLfloat t);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD2FVPROC)(const GLfloat * v);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD2IPROC)(GLint s, GLint t);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD2IVPROC)(const GLint * v);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD2SPROC)(GLshort s, GLshort t);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD2SVPROC)(const GLshort * v);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD3DPROC)(GLdouble s, GLdouble t, GLdouble r);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD3DVPROC)(const GLdouble * v);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD3FPROC)(GLfloat s, GLfloat t, GLfloat r);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD3FVPROC)(const GLfloat * v);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD3IPROC)(GLint s, GLint t, GLint r);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD3IVPROC)(const GLint * v);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD3SPROC)(GLshort s, GLshort t, GLshort r);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD3SVPROC)(const GLshort * v);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD4DPROC)(GLdouble s, GLdouble t, GLdouble r, GLdouble q);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD4DVPROC)(const GLdouble * v);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD4FPROC)(GLfloat s, GLfloat t, GLfloat r, GLfloat q);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD4FVPROC)(const GLfloat * v);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD4IPROC)(GLint s, GLint t, GLint r, GLint q);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD4IVPROC)(const GLint * v);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD4SPROC)(GLshort s, GLshort t, GLshort r, GLshort q);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXCOORD4SVPROC)(const GLshort * v);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXENVFPROC)(GLenum target, GLenum pname, GLfloat param);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXENVFVPROC)(GLenum target, GLenum pname, const GLfloat * params);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXENVIPROC)(GLenum target, GLenum pname, GLint param);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXENVIVPROC)(GLenum target, GLenum pname, const GLint * params);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXGENDPROC)(GLenum coord, GLenum pname, GLdouble param);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXGENDVPROC)(GLenum coord, GLenum pname, const GLdouble * params);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXGENFPROC)(GLenum coord, GLenum pname, GLfloat param);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXGENFVPROC)(GLenum coord, GLenum pname, const GLfloat * params);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXGENIPROC)(GLenum coord, GLenum pname, GLint param);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXGENIVPROC)(GLenum coord, GLenum pname, const GLint * params);
typedef void (CODEGEN_FUNCPTR * PFNGLTRANSLATEDPROC)(GLdouble x, GLdouble y, GLdouble z);
typedef void (CODEGEN_FUNCPTR * PFNGLTRANSLATEFPROC)(GLfloat x, GLfloat y, GLfloat z);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEX2DPROC)(GLdouble x, GLdouble y);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEX2DVPROC)(const GLdouble * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEX2FPROC)(GLfloat x, GLfloat y);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEX2FVPROC)(const GLfloat * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEX2IPROC)(GLint x, GLint y);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEX2IVPROC)(const GLint * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEX2SPROC)(GLshort x, GLshort y);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEX2SVPROC)(const GLshort * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEX3DPROC)(GLdouble x, GLdouble y, GLdouble z);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEX3DVPROC)(const GLdouble * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEX3FPROC)(GLfloat x, GLfloat y, GLfloat z);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEX3FVPROC)(const GLfloat * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEX3IPROC)(GLint x, GLint y, GLint z);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEX3IVPROC)(const GLint * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEX3SPROC)(GLshort x, GLshort y, GLshort z);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEX3SVPROC)(const GLshort * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEX4DPROC)(GLdouble x, GLdouble y, GLdouble z, GLdouble w);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEX4DVPROC)(const GLdouble * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEX4FPROC)(GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEX4FVPROC)(const GLfloat * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEX4IPROC)(GLint x, GLint y, GLint z, GLint w);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEX4IVPROC)(const GLint * v);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEX4SPROC)(GLshort x, GLshort y, GLshort z, GLshort w);
typedef void (CODEGEN_FUNCPTR * PFNGLVERTEX4SVPROC)(const GLshort * v);

CI_API extern PFNGLACCUMPROC _funcptr_glAccum;
#define glAccum _funcptr_glAccum
CI_API extern PFNGLALPHAFUNCPROC _funcptr_glAlphaFunc;
#define glAlphaFunc _funcptr_glAlphaFunc
CI_API extern PFNGLBEGINPROC _funcptr_glBegin;
#define glBegin _funcptr_glBegin
CI_API extern PFNGLBITMAPPROC _funcptr_glBitmap;
#define glBitmap _funcptr_glBitmap
CI_API extern PFNGLCALLLISTPROC _funcptr_glCallList;
#define glCallList _funcptr_glCallList
CI_API extern PFNGLCALLLISTSPROC _funcptr_glCallLists;
#define glCallLists _funcptr_glCallLists
CI_API extern PFNGLCLEARACCUMPROC _funcptr_glClearAccum;
#define glClearAccum _funcptr_glClearAccum
CI_API extern PFNGLCLEARINDEXPROC _funcptr_glClearIndex;
#define glClearIndex _funcptr_glClearIndex
CI_API extern PFNGLCLIPPLANEPROC _funcptr_glClipPlane;
#define glClipPlane _funcptr_glClipPlane
CI_API extern PFNGLCOLOR3BPROC _funcptr_glColor3b;
#define glColor3b _funcptr_glColor3b
CI_API extern PFNGLCOLOR3BVPROC _funcptr_glColor3bv;
#define glColor3bv _funcptr_glColor3bv
CI_API extern PFNGLCOLOR3DPROC _funcptr_glColor3d;
#define glColor3d _funcptr_glColor3d
CI_API extern PFNGLCOLOR3DVPROC _funcptr_glColor3dv;
#define glColor3dv _funcptr_glColor3dv
CI_API extern PFNGLCOLOR3FPROC _funcptr_glColor3f;
#define glColor3f _funcptr_glColor3f
CI_API extern PFNGLCOLOR3FVPROC _funcptr_glColor3fv;
#define glColor3fv _funcptr_glColor3fv
CI_API extern PFNGLCOLOR3IPROC _funcptr_glColor3i;
#define glColor3i _funcptr_glColor3i
CI_API extern PFNGLCOLOR3IVPROC _funcptr_glColor3iv;
#define glColor3iv _funcptr_glColor3iv
CI_API extern PFNGLCOLOR3SPROC _funcptr_glColor3s;
#define glColor3s _funcptr_glColor3s
CI_API extern PFNGLCOLOR3SVPROC _funcptr_glColor3sv;
#define glColor3sv _funcptr_glColor3sv
CI_API extern PFNGLCOLOR3UBPROC _funcptr_glColor3ub;
#define glColor3ub _funcptr_glColor3ub
CI_API extern PFNGLCOLOR3UBVPROC _funcptr_glColor3ubv;
#define glColor3ubv _funcptr_glColor3ubv
CI_API extern PFNGLCOLOR3UIPROC _funcptr_glColor3ui;
#define glColor3ui _funcptr_glColor3ui
CI_API extern PFNGLCOLOR3UIVPROC _funcptr_glColor3uiv;
#define glColor3uiv _funcptr_glColor3uiv
CI_API extern PFNGLCOLOR3USPROC _funcptr_glColor3us;
#define glColor3us _funcptr_glColor3us
CI_API extern PFNGLCOLOR3USVPROC _funcptr_glColor3usv;
#define glColor3usv _funcptr_glColor3usv
CI_API extern PFNGLCOLOR4BPROC _funcptr_glColor4b;
#define glColor4b _funcptr_glColor4b
CI_API extern PFNGLCOLOR4BVPROC _funcptr_glColor4bv;
#define glColor4bv _funcptr_glColor4bv
CI_API extern PFNGLCOLOR4DPROC _funcptr_glColor4d;
#define glColor4d _funcptr_glColor4d
CI_API extern PFNGLCOLOR4DVPROC _funcptr_glColor4dv;
#define glColor4dv _funcptr_glColor4dv
CI_API extern PFNGLCOLOR4FPROC _funcptr_glColor4f;
#define glColor4f _funcptr_glColor4f
CI_API extern PFNGLCOLOR4FVPROC _funcptr_glColor4fv;
#define glColor4fv _funcptr_glColor4fv
CI_API extern PFNGLCOLOR4IPROC _funcptr_glColor4i;
#define glColor4i _funcptr_glColor4i
CI_API extern PFNGLCOLOR4IVPROC _funcptr_glColor4iv;
#define glColor4iv _funcptr_glColor4iv
CI_API extern PFNGLCOLOR4SPROC _funcptr_glColor4s;
#define glColor4s _funcptr_glColor4s
CI_API extern PFNGLCOLOR4SVPROC _funcptr_glColor4sv;
#define glColor4sv _funcptr_glColor4sv
CI_API extern PFNGLCOLOR4UBPROC _funcptr_glColor4ub;
#define glColor4ub _funcptr_glColor4ub
CI_API extern PFNGLCOLOR4UBVPROC _funcptr_glColor4ubv;
#define glColor4ubv _funcptr_glColor4ubv
CI_API extern PFNGLCOLOR4UIPROC _funcptr_glColor4ui;
#define glColor4ui _funcptr_glColor4ui
CI_API extern PFNGLCOLOR4UIVPROC _funcptr_glColor4uiv;
#define glColor4uiv _funcptr_glColor4uiv
CI_API extern PFNGLCOLOR4USPROC _funcptr_glColor4us;
#define glColor4us _funcptr_glColor4us
CI_API extern PFNGLCOLOR4USVPROC _funcptr_glColor4usv;
#define glColor4usv _funcptr_glColor4usv
CI_API extern PFNGLCOLORMATERIALPROC _funcptr_glColorMaterial;
#define glColorMaterial _funcptr_glColorMaterial
CI_API extern PFNGLCOPYPIXELSPROC _funcptr_glCopyPixels;
#define glCopyPixels _funcptr_glCopyPixels
CI_API extern PFNGLDELETELISTSPROC _funcptr_glDeleteLists;
#define glDeleteLists _funcptr_glDeleteLists
CI_API extern PFNGLDRAWPIXELSPROC _funcptr_glDrawPixels;
#define glDrawPixels _funcptr_glDrawPixels
CI_API extern PFNGLEDGEFLAGPROC _funcptr_glEdgeFlag;
#define glEdgeFlag _funcptr_glEdgeFlag
CI_API extern PFNGLEDGEFLAGVPROC _funcptr_glEdgeFlagv;
#define glEdgeFlagv _funcptr_glEdgeFlagv
CI_API extern PFNGLENDPROC _funcptr_glEnd;
#define glEnd _funcptr_glEnd
CI_API extern PFNGLENDLISTPROC _funcptr_glEndList;
#define glEndList _funcptr_glEndList
CI_API extern PFNGLEVALCOORD1DPROC _funcptr_glEvalCoord1d;
#define glEvalCoord1d _funcptr_glEvalCoord1d
CI_API extern PFNGLEVALCOORD1DVPROC _funcptr_glEvalCoord1dv;
#define glEvalCoord1dv _funcptr_glEvalCoord1dv
CI_API extern PFNGLEVALCOORD1FPROC _funcptr_glEvalCoord1f;
#define glEvalCoord1f _funcptr_glEvalCoord1f
CI_API extern PFNGLEVALCOORD1FVPROC _funcptr_glEvalCoord1fv;
#define glEvalCoord1fv _funcptr_glEvalCoord1fv
CI_API extern PFNGLEVALCOORD2DPROC _funcptr_glEvalCoord2d;
#define glEvalCoord2d _funcptr_glEvalCoord2d
CI_API extern PFNGLEVALCOORD2DVPROC _funcptr_glEvalCoord2dv;
#define glEvalCoord2dv _funcptr_glEvalCoord2dv
CI_API extern PFNGLEVALCOORD2FPROC _funcptr_glEvalCoord2f;
#define glEvalCoord2f _funcptr_glEvalCoord2f
CI_API extern PFNGLEVALCOORD2FVPROC _funcptr_glEvalCoord2fv;
#define glEvalCoord2fv _funcptr_glEvalCoord2fv
CI_API extern PFNGLEVALMESH1PROC _funcptr_glEvalMesh1;
#define glEvalMesh1 _funcptr_glEvalMesh1
CI_API extern PFNGLEVALMESH2PROC _funcptr_glEvalMesh2;
#define glEvalMesh2 _funcptr_glEvalMesh2
CI_API extern PFNGLEVALPOINT1PROC _funcptr_glEvalPoint1;
#define glEvalPoint1 _funcptr_glEvalPoint1
CI_API extern PFNGLEVALPOINT2PROC _funcptr_glEvalPoint2;
#define glEvalPoint2 _funcptr_glEvalPoint2
CI_API extern PFNGLFEEDBACKBUFFERPROC _funcptr_glFeedbackBuffer;
#define glFeedbackBuffer _funcptr_glFeedbackBuffer
CI_API extern PFNGLFOGFPROC _funcptr_glFogf;
#define glFogf _funcptr_glFogf
CI_API extern PFNGLFOGFVPROC _funcptr_glFogfv;
#define glFogfv _funcptr_glFogfv
CI_API extern PFNGLFOGIPROC _funcptr_glFogi;
#define glFogi _funcptr_glFogi
CI_API extern PFNGLFOGIVPROC _funcptr_glFogiv;
#define glFogiv _funcptr_glFogiv
CI_API extern PFNGLFRUSTUMPROC _funcptr_glFrustum;
#define glFrustum _funcptr_glFrustum
CI_API extern PFNGLGENLISTSPROC _funcptr_glGenLists;
#define glGenLists _funcptr_glGenLists
CI_API extern PFNGLGETCLIPPLANEPROC _funcptr_glGetClipPlane;
#define glGetClipPlane _funcptr_glGetClipPlane
CI_API extern PFNGLGETLIGHTFVPROC _funcptr_glGetLightfv;
#define glGetLightfv _funcptr_glGetLightfv
CI_API extern PFNGLGETLIGHTIVPROC _funcptr_glGetLightiv;
#define glGetLightiv _funcptr_glGetLightiv
CI_API extern PFNGLGETMAPDVPROC _funcptr_glGetMapdv;
#define glGetMapdv _funcptr_glGetMapdv
CI_API extern PFNGLGETMAPFVPROC _funcptr_glGetMapfv;
#define glGetMapfv _funcptr_glGetMapfv
CI_API extern PFNGLGETMAPIVPROC _funcptr_glGetMapiv;
#define glGetMapiv _funcptr_glGetMapiv
CI_API extern PFNGLGETMATERIALFVPROC _funcptr_glGetMaterialfv;
#define glGetMaterialfv _funcptr_glGetMaterialfv
CI_API extern PFNGLGETMATERIALIVPROC _funcptr_glGetMaterialiv;
#define glGetMaterialiv _funcptr_glGetMaterialiv
CI_API extern PFNGLGETPIXELMAPFVPROC _funcptr_glGetPixelMapfv;
#define glGetPixelMapfv _funcptr_glGetPixelMapfv
CI_API extern PFNGLGETPIXELMAPUIVPROC _funcptr_glGetPixelMapuiv;
#define glGetPixelMapuiv _funcptr_glGetPixelMapuiv
CI_API extern PFNGLGETPIXELMAPUSVPROC _funcptr_glGetPixelMapusv;
#define glGetPixelMapusv _funcptr_glGetPixelMapusv
CI_API extern PFNGLGETPOLYGONSTIPPLEPROC _funcptr_glGetPolygonStipple;
#define glGetPolygonStipple _funcptr_glGetPolygonStipple
CI_API extern PFNGLGETTEXENVFVPROC _funcptr_glGetTexEnvfv;
#define glGetTexEnvfv _funcptr_glGetTexEnvfv
CI_API extern PFNGLGETTEXENVIVPROC _funcptr_glGetTexEnviv;
#define glGetTexEnviv _funcptr_glGetTexEnviv
CI_API extern PFNGLGETTEXGENDVPROC _funcptr_glGetTexGendv;
#define glGetTexGendv _funcptr_glGetTexGendv
CI_API extern PFNGLGETTEXGENFVPROC _funcptr_glGetTexGenfv;
#define glGetTexGenfv _funcptr_glGetTexGenfv
CI_API extern PFNGLGETTEXGENIVPROC _funcptr_glGetTexGeniv;
#define glGetTexGeniv _funcptr_glGetTexGeniv
CI_API extern PFNGLINDEXMASKPROC _funcptr_glIndexMask;
#define glIndexMask _funcptr_glIndexMask
CI_API extern PFNGLINDEXDPROC _funcptr_glIndexd;
#define glIndexd _funcptr_glIndexd
CI_API extern PFNGLINDEXDVPROC _funcptr_glIndexdv;
#define glIndexdv _funcptr_glIndexdv
CI_API extern PFNGLINDEXFPROC _funcptr_glIndexf;
#define glIndexf _funcptr_glIndexf
CI_API extern PFNGLINDEXFVPROC _funcptr_glIndexfv;
#define glIndexfv _funcptr_glIndexfv
CI_API extern PFNGLINDEXIPROC _funcptr_glIndexi;
#define glIndexi _funcptr_glIndexi
CI_API extern PFNGLINDEXIVPROC _funcptr_glIndexiv;
#define glIndexiv _funcptr_glIndexiv
CI_API extern PFNGLINDEXSPROC _funcptr_glIndexs;
#define glIndexs _funcptr_glIndexs
CI_API extern PFNGLINDEXSVPROC _funcptr_glIndexsv;
#define glIndexsv _funcptr_glIndexsv
CI_API extern PFNGLINITNAMESPROC _funcptr_glInitNames;
#define glInitNames _funcptr_glInitNames
CI_API extern PFNGLISLISTPROC _funcptr_glIsList;
#define glIsList _funcptr_glIsList
CI_API extern PFNGLLIGHTMODELFPROC _funcptr_glLightModelf;
#define glLightModelf _funcptr_glLightModelf
CI_API extern PFNGLLIGHTMODELFVPROC _funcptr_glLightModelfv;
#define glLightModelfv _funcptr_glLightModelfv
CI_API extern PFNGLLIGHTMODELIPROC _funcptr_glLightModeli;
#define glLightModeli _funcptr_glLightModeli
CI_API extern PFNGLLIGHTMODELIVPROC _funcptr_glLightModeliv;
#define glLightModeliv _funcptr_glLightModeliv
CI_API extern PFNGLLIGHTFPROC _funcptr_glLightf;
#define glLightf _funcptr_glLightf
CI_API extern PFNGLLIGHTFVPROC _funcptr_glLightfv;
#define glLightfv _funcptr_glLightfv
CI_API extern PFNGLLIGHTIPROC _funcptr_glLighti;
#define glLighti _funcptr_glLighti
CI_API extern PFNGLLIGHTIVPROC _funcptr_glLightiv;
#define glLightiv _funcptr_glLightiv
CI_API extern PFNGLLINESTIPPLEPROC _funcptr_glLineStipple;
#define glLineStipple _funcptr_glLineStipple
CI_API extern PFNGLLISTBASEPROC _funcptr_glListBase;
#define glListBase _funcptr_glListBase
CI_API extern PFNGLLOADIDENTITYPROC _funcptr_glLoadIdentity;
#define glLoadIdentity _funcptr_glLoadIdentity
CI_API extern PFNGLLOADMATRIXDPROC _funcptr_glLoadMatrixd;
#define glLoadMatrixd _funcptr_glLoadMatrixd
CI_API extern PFNGLLOADMATRIXFPROC _funcptr_glLoadMatrixf;
#define glLoadMatrixf _funcptr_glLoadMatrixf
CI_API extern PFNGLLOADNAMEPROC _funcptr_glLoadName;
#define glLoadName _funcptr_glLoadName
CI_API extern PFNGLMAP1DPROC _funcptr_glMap1d;
#define glMap1d _funcptr_glMap1d
CI_API extern PFNGLMAP1FPROC _funcptr_glMap1f;
#define glMap1f _funcptr_glMap1f
CI_API extern PFNGLMAP2DPROC _funcptr_glMap2d;
#define glMap2d _funcptr_glMap2d
CI_API extern PFNGLMAP2FPROC _funcptr_glMap2f;
#define glMap2f _funcptr_glMap2f
CI_API extern PFNGLMAPGRID1DPROC _funcptr_glMapGrid1d;
#define glMapGrid1d _funcptr_glMapGrid1d
CI_API extern PFNGLMAPGRID1FPROC _funcptr_glMapGrid1f;
#define glMapGrid1f _funcptr_glMapGrid1f
CI_API extern PFNGLMAPGRID2DPROC _funcptr_glMapGrid2d;
#define glMapGrid2d _funcptr_glMapGrid2d
CI_API extern PFNGLMAPGRID2FPROC _funcptr_glMapGrid2f;
#define glMapGrid2f _funcptr_glMapGrid2f
CI_API extern PFNGLMATERIALFPROC _funcptr_glMaterialf;
#define glMaterialf _funcptr_glMaterialf
CI_API extern PFNGLMATERIALFVPROC _funcptr_glMaterialfv;
#define glMaterialfv _funcptr_glMaterialfv
CI_API extern PFNGLMATERIALIPROC _funcptr_glMateriali;
#define glMateriali _funcptr_glMateriali
CI_API extern PFNGLMATERIALIVPROC _funcptr_glMaterialiv;
#define glMaterialiv _funcptr_glMaterialiv
CI_API extern PFNGLMATRIXMODEPROC _funcptr_glMatrixMode;
#define glMatrixMode _funcptr_glMatrixMode
CI_API extern PFNGLMULTMATRIXDPROC _funcptr_glMultMatrixd;
#define glMultMatrixd _funcptr_glMultMatrixd
CI_API extern PFNGLMULTMATRIXFPROC _funcptr_glMultMatrixf;
#define glMultMatrixf _funcptr_glMultMatrixf
CI_API extern PFNGLNEWLISTPROC _funcptr_glNewList;
#define glNewList _funcptr_glNewList
CI_API extern PFNGLNORMAL3BPROC _funcptr_glNormal3b;
#define glNormal3b _funcptr_glNormal3b
CI_API extern PFNGLNORMAL3BVPROC _funcptr_glNormal3bv;
#define glNormal3bv _funcptr_glNormal3bv
CI_API extern PFNGLNORMAL3DPROC _funcptr_glNormal3d;
#define glNormal3d _funcptr_glNormal3d
CI_API extern PFNGLNORMAL3DVPROC _funcptr_glNormal3dv;
#define glNormal3dv _funcptr_glNormal3dv
CI_API extern PFNGLNORMAL3FPROC _funcptr_glNormal3f;
#define glNormal3f _funcptr_glNormal3f
CI_API extern PFNGLNORMAL3FVPROC _funcptr_glNormal3fv;
#define glNormal3fv _funcptr_glNormal3fv
CI_API extern PFNGLNORMAL3IPROC _funcptr_glNormal3i;
#define glNormal3i _funcptr_glNormal3i
CI_API extern PFNGLNORMAL3IVPROC _funcptr_glNormal3iv;
#define glNormal3iv _funcptr_glNormal3iv
CI_API extern PFNGLNORMAL3SPROC _funcptr_glNormal3s;
#define glNormal3s _funcptr_glNormal3s
CI_API extern PFNGLNORMAL3SVPROC _funcptr_glNormal3sv;
#define glNormal3sv _funcptr_glNormal3sv
CI_API extern PFNGLORTHOPROC _funcptr_glOrtho;
#define glOrtho _funcptr_glOrtho
CI_API extern PFNGLPASSTHROUGHPROC _funcptr_glPassThrough;
#define glPassThrough _funcptr_glPassThrough
CI_API extern PFNGLPIXELMAPFVPROC _funcptr_glPixelMapfv;
#define glPixelMapfv _funcptr_glPixelMapfv
CI_API extern PFNGLPIXELMAPUIVPROC _funcptr_glPixelMapuiv;
#define glPixelMapuiv _funcptr_glPixelMapuiv
CI_API extern PFNGLPIXELMAPUSVPROC _funcptr_glPixelMapusv;
#define glPixelMapusv _funcptr_glPixelMapusv
CI_API extern PFNGLPIXELTRANSFERFPROC _funcptr_glPixelTransferf;
#define glPixelTransferf _funcptr_glPixelTransferf
CI_API extern PFNGLPIXELTRANSFERIPROC _funcptr_glPixelTransferi;
#define glPixelTransferi _funcptr_glPixelTransferi
CI_API extern PFNGLPIXELZOOMPROC _funcptr_glPixelZoom;
#define glPixelZoom _funcptr_glPixelZoom
CI_API extern PFNGLPOLYGONSTIPPLEPROC _funcptr_glPolygonStipple;
#define glPolygonStipple _funcptr_glPolygonStipple
CI_API extern PFNGLPOPATTRIBPROC _funcptr_glPopAttrib;
#define glPopAttrib _funcptr_glPopAttrib
CI_API extern PFNGLPOPMATRIXPROC _funcptr_glPopMatrix;
#define glPopMatrix _funcptr_glPopMatrix
CI_API extern PFNGLPOPNAMEPROC _funcptr_glPopName;
#define glPopName _funcptr_glPopName
CI_API extern PFNGLPUSHATTRIBPROC _funcptr_glPushAttrib;
#define glPushAttrib _funcptr_glPushAttrib
CI_API extern PFNGLPUSHMATRIXPROC _funcptr_glPushMatrix;
#define glPushMatrix _funcptr_glPushMatrix
CI_API extern PFNGLPUSHNAMEPROC _funcptr_glPushName;
#define glPushName _funcptr_glPushName
CI_API extern PFNGLRASTERPOS2DPROC _funcptr_glRasterPos2d;
#define glRasterPos2d _funcptr_glRasterPos2d
CI_API extern PFNGLRASTERPOS2DVPROC _funcptr_glRasterPos2dv;
#define glRasterPos2dv _funcptr_glRasterPos2dv
CI_API extern PFNGLRASTERPOS2FPROC _funcptr_glRasterPos2f;
#define glRasterPos2f _funcptr_glRasterPos2f
CI_API extern PFNGLRASTERPOS2FVPROC _funcptr_glRasterPos2fv;
#define glRasterPos2fv _funcptr_glRasterPos2fv
CI_API extern PFNGLRASTERPOS2IPROC _funcptr_glRasterPos2i;
#define glRasterPos2i _funcptr_glRasterPos2i
CI_API extern PFNGLRASTERPOS2IVPROC _funcptr_glRasterPos2iv;
#define glRasterPos2iv _funcptr_glRasterPos2iv
CI_API extern PFNGLRASTERPOS2SPROC _funcptr_glRasterPos2s;
#define glRasterPos2s _funcptr_glRasterPos2s
CI_API extern PFNGLRASTERPOS2SVPROC _funcptr_glRasterPos2sv;
#define glRasterPos2sv _funcptr_glRasterPos2sv
CI_API extern PFNGLRASTERPOS3DPROC _funcptr_glRasterPos3d;
#define glRasterPos3d _funcptr_glRasterPos3d
CI_API extern PFNGLRASTERPOS3DVPROC _funcptr_glRasterPos3dv;
#define glRasterPos3dv _funcptr_glRasterPos3dv
CI_API extern PFNGLRASTERPOS3FPROC _funcptr_glRasterPos3f;
#define glRasterPos3f _funcptr_glRasterPos3f
CI_API extern PFNGLRASTERPOS3FVPROC _funcptr_glRasterPos3fv;
#define glRasterPos3fv _funcptr_glRasterPos3fv
CI_API extern PFNGLRASTERPOS3IPROC _funcptr_glRasterPos3i;
#define glRasterPos3i _funcptr_glRasterPos3i
CI_API extern PFNGLRASTERPOS3IVPROC _funcptr_glRasterPos3iv;
#define glRasterPos3iv _funcptr_glRasterPos3iv
CI_API extern PFNGLRASTERPOS3SPROC _funcptr_glRasterPos3s;
#define glRasterPos3s _funcptr_glRasterPos3s
CI_API extern PFNGLRASTERPOS3SVPROC _funcptr_glRasterPos3sv;
#define glRasterPos3sv _funcptr_glRasterPos3sv
CI_API extern PFNGLRASTERPOS4DPROC _funcptr_glRasterPos4d;
#define glRasterPos4d _funcptr_glRasterPos4d
CI_API extern PFNGLRASTERPOS4DVPROC _funcptr_glRasterPos4dv;
#define glRasterPos4dv _funcptr_glRasterPos4dv
CI_API extern PFNGLRASTERPOS4FPROC _funcptr_glRasterPos4f;
#define glRasterPos4f _funcptr_glRasterPos4f
CI_API extern PFNGLRASTERPOS4FVPROC _funcptr_glRasterPos4fv;
#define glRasterPos4fv _funcptr_glRasterPos4fv
CI_API extern PFNGLRASTERPOS4IPROC _funcptr_glRasterPos4i;
#define glRasterPos4i _funcptr_glRasterPos4i
CI_API extern PFNGLRASTERPOS4IVPROC _funcptr_glRasterPos4iv;
#define glRasterPos4iv _funcptr_glRasterPos4iv
CI_API extern PFNGLRASTERPOS4SPROC _funcptr_glRasterPos4s;
#define glRasterPos4s _funcptr_glRasterPos4s
CI_API extern PFNGLRASTERPOS4SVPROC _funcptr_glRasterPos4sv;
#define glRasterPos4sv _funcptr_glRasterPos4sv
CI_API extern PFNGLRECTDPROC _funcptr_glRectd;
#define glRectd _funcptr_glRectd
CI_API extern PFNGLRECTDVPROC _funcptr_glRectdv;
#define glRectdv _funcptr_glRectdv
CI_API extern PFNGLRECTFPROC _funcptr_glRectf;
#define glRectf _funcptr_glRectf
CI_API extern PFNGLRECTFVPROC _funcptr_glRectfv;
#define glRectfv _funcptr_glRectfv
CI_API extern PFNGLRECTIPROC _funcptr_glRecti;
#define glRecti _funcptr_glRecti
CI_API extern PFNGLRECTIVPROC _funcptr_glRectiv;
#define glRectiv _funcptr_glRectiv
CI_API extern PFNGLRECTSPROC _funcptr_glRects;
#define glRects _funcptr_glRects
CI_API extern PFNGLRECTSVPROC _funcptr_glRectsv;
#define glRectsv _funcptr_glRectsv
CI_API extern PFNGLRENDERMODEPROC _funcptr_glRenderMode;
#define glRenderMode _funcptr_glRenderMode
CI_API extern PFNGLROTATEDPROC _funcptr_glRotated;
#define glRotated _funcptr_glRotated
CI_API extern PFNGLROTATEFPROC _funcptr_glRotatef;
#define glRotatef _funcptr_glRotatef
CI_API extern PFNGLSCALEDPROC _funcptr_glScaled;
#define glScaled _funcptr_glScaled
CI_API extern PFNGLSCALEFPROC _funcptr_glScalef;
#define glScalef _funcptr_glScalef
CI_API extern PFNGLSELECTBUFFERPROC _funcptr_glSelectBuffer;
#define glSelectBuffer _funcptr_glSelectBuffer
CI_API extern PFNGLSHADEMODELPROC _funcptr_glShadeModel;
#define glShadeModel _funcptr_glShadeModel
CI_API extern PFNGLTEXCOORD1DPROC _funcptr_glTexCoord1d;
#define glTexCoord1d _funcptr_glTexCoord1d
CI_API extern PFNGLTEXCOORD1DVPROC _funcptr_glTexCoord1dv;
#define glTexCoord1dv _funcptr_glTexCoord1dv
CI_API extern PFNGLTEXCOORD1FPROC _funcptr_glTexCoord1f;
#define glTexCoord1f _funcptr_glTexCoord1f
CI_API extern PFNGLTEXCOORD1FVPROC _funcptr_glTexCoord1fv;
#define glTexCoord1fv _funcptr_glTexCoord1fv
CI_API extern PFNGLTEXCOORD1IPROC _funcptr_glTexCoord1i;
#define glTexCoord1i _funcptr_glTexCoord1i
CI_API extern PFNGLTEXCOORD1IVPROC _funcptr_glTexCoord1iv;
#define glTexCoord1iv _funcptr_glTexCoord1iv
CI_API extern PFNGLTEXCOORD1SPROC _funcptr_glTexCoord1s;
#define glTexCoord1s _funcptr_glTexCoord1s
CI_API extern PFNGLTEXCOORD1SVPROC _funcptr_glTexCoord1sv;
#define glTexCoord1sv _funcptr_glTexCoord1sv
CI_API extern PFNGLTEXCOORD2DPROC _funcptr_glTexCoord2d;
#define glTexCoord2d _funcptr_glTexCoord2d
CI_API extern PFNGLTEXCOORD2DVPROC _funcptr_glTexCoord2dv;
#define glTexCoord2dv _funcptr_glTexCoord2dv
CI_API extern PFNGLTEXCOORD2FPROC _funcptr_glTexCoord2f;
#define glTexCoord2f _funcptr_glTexCoord2f
CI_API extern PFNGLTEXCOORD2FVPROC _funcptr_glTexCoord2fv;
#define glTexCoord2fv _funcptr_glTexCoord2fv
CI_API extern PFNGLTEXCOORD2IPROC _funcptr_glTexCoord2i;
#define glTexCoord2i _funcptr_glTexCoord2i
CI_API extern PFNGLTEXCOORD2IVPROC _funcptr_glTexCoord2iv;
#define glTexCoord2iv _funcptr_glTexCoord2iv
CI_API extern PFNGLTEXCOORD2SPROC _funcptr_glTexCoord2s;
#define glTexCoord2s _funcptr_glTexCoord2s
CI_API extern PFNGLTEXCOORD2SVPROC _funcptr_glTexCoord2sv;
#define glTexCoord2sv _funcptr_glTexCoord2sv
CI_API extern PFNGLTEXCOORD3DPROC _funcptr_glTexCoord3d;
#define glTexCoord3d _funcptr_glTexCoord3d
CI_API extern PFNGLTEXCOORD3DVPROC _funcptr_glTexCoord3dv;
#define glTexCoord3dv _funcptr_glTexCoord3dv
CI_API extern PFNGLTEXCOORD3FPROC _funcptr_glTexCoord3f;
#define glTexCoord3f _funcptr_glTexCoord3f
CI_API extern PFNGLTEXCOORD3FVPROC _funcptr_glTexCoord3fv;
#define glTexCoord3fv _funcptr_glTexCoord3fv
CI_API extern PFNGLTEXCOORD3IPROC _funcptr_glTexCoord3i;
#define glTexCoord3i _funcptr_glTexCoord3i
CI_API extern PFNGLTEXCOORD3IVPROC _funcptr_glTexCoord3iv;
#define glTexCoord3iv _funcptr_glTexCoord3iv
CI_API extern PFNGLTEXCOORD3SPROC _funcptr_glTexCoord3s;
#define glTexCoord3s _funcptr_glTexCoord3s
CI_API extern PFNGLTEXCOORD3SVPROC _funcptr_glTexCoord3sv;
#define glTexCoord3sv _funcptr_glTexCoord3sv
CI_API extern PFNGLTEXCOORD4DPROC _funcptr_glTexCoord4d;
#define glTexCoord4d _funcptr_glTexCoord4d
CI_API extern PFNGLTEXCOORD4DVPROC _funcptr_glTexCoord4dv;
#define glTexCoord4dv _funcptr_glTexCoord4dv
CI_API extern PFNGLTEXCOORD4FPROC _funcptr_glTexCoord4f;
#define glTexCoord4f _funcptr_glTexCoord4f
CI_API extern PFNGLTEXCOORD4FVPROC _funcptr_glTexCoord4fv;
#define glTexCoord4fv _funcptr_glTexCoord4fv
CI_API extern PFNGLTEXCOORD4IPROC _funcptr_glTexCoord4i;
#define glTexCoord4i _funcptr_glTexCoord4i
CI_API extern PFNGLTEXCOORD4IVPROC _funcptr_glTexCoord4iv;
#define glTexCoord4iv _funcptr_glTexCoord4iv
CI_API extern PFNGLTEXCOORD4SPROC _funcptr_glTexCoord4s;
#define glTexCoord4s _funcptr_glTexCoord4s
CI_API extern PFNGLTEXCOORD4SVPROC _funcptr_glTexCoord4sv;
#define glTexCoord4sv _funcptr_glTexCoord4sv
CI_API extern PFNGLTEXENVFPROC _funcptr_glTexEnvf;
#define glTexEnvf _funcptr_glTexEnvf
CI_API extern PFNGLTEXENVFVPROC _funcptr_glTexEnvfv;
#define glTexEnvfv _funcptr_glTexEnvfv
CI_API extern PFNGLTEXENVIPROC _funcptr_glTexEnvi;
#define glTexEnvi _funcptr_glTexEnvi
CI_API extern PFNGLTEXENVIVPROC _funcptr_glTexEnviv;
#define glTexEnviv _funcptr_glTexEnviv
CI_API extern PFNGLTEXGENDPROC _funcptr_glTexGend;
#define glTexGend _funcptr_glTexGend
CI_API extern PFNGLTEXGENDVPROC _funcptr_glTexGendv;
#define glTexGendv _funcptr_glTexGendv
CI_API extern PFNGLTEXGENFPROC _funcptr_glTexGenf;
#define glTexGenf _funcptr_glTexGenf
CI_API extern PFNGLTEXGENFVPROC _funcptr_glTexGenfv;
#define glTexGenfv _funcptr_glTexGenfv
CI_API extern PFNGLTEXGENIPROC _funcptr_glTexGeni;
#define glTexGeni _funcptr_glTexGeni
CI_API extern PFNGLTEXGENIVPROC _funcptr_glTexGeniv;
#define glTexGeniv _funcptr_glTexGeniv
CI_API extern PFNGLTRANSLATEDPROC _funcptr_glTranslated;
#define glTranslated _funcptr_glTranslated
CI_API extern PFNGLTRANSLATEFPROC _funcptr_glTranslatef;
#define glTranslatef _funcptr_glTranslatef
CI_API extern PFNGLVERTEX2DPROC _funcptr_glVertex2d;
#define glVertex2d _funcptr_glVertex2d
CI_API extern PFNGLVERTEX2DVPROC _funcptr_glVertex2dv;
#define glVertex2dv _funcptr_glVertex2dv
CI_API extern PFNGLVERTEX2FPROC _funcptr_glVertex2f;
#define glVertex2f _funcptr_glVertex2f
CI_API extern PFNGLVERTEX2FVPROC _funcptr_glVertex2fv;
#define glVertex2fv _funcptr_glVertex2fv
CI_API extern PFNGLVERTEX2IPROC _funcptr_glVertex2i;
#define glVertex2i _funcptr_glVertex2i
CI_API extern PFNGLVERTEX2IVPROC _funcptr_glVertex2iv;
#define glVertex2iv _funcptr_glVertex2iv
CI_API extern PFNGLVERTEX2SPROC _funcptr_glVertex2s;
#define glVertex2s _funcptr_glVertex2s
CI_API extern PFNGLVERTEX2SVPROC _funcptr_glVertex2sv;
#define glVertex2sv _funcptr_glVertex2sv
CI_API extern PFNGLVERTEX3DPROC _funcptr_glVertex3d;
#define glVertex3d _funcptr_glVertex3d
CI_API extern PFNGLVERTEX3DVPROC _funcptr_glVertex3dv;
#define glVertex3dv _funcptr_glVertex3dv
CI_API extern PFNGLVERTEX3FPROC _funcptr_glVertex3f;
#define glVertex3f _funcptr_glVertex3f
CI_API extern PFNGLVERTEX3FVPROC _funcptr_glVertex3fv;
#define glVertex3fv _funcptr_glVertex3fv
CI_API extern PFNGLVERTEX3IPROC _funcptr_glVertex3i;
#define glVertex3i _funcptr_glVertex3i
CI_API extern PFNGLVERTEX3IVPROC _funcptr_glVertex3iv;
#define glVertex3iv _funcptr_glVertex3iv
CI_API extern PFNGLVERTEX3SPROC _funcptr_glVertex3s;
#define glVertex3s _funcptr_glVertex3s
CI_API extern PFNGLVERTEX3SVPROC _funcptr_glVertex3sv;
#define glVertex3sv _funcptr_glVertex3sv
CI_API extern PFNGLVERTEX4DPROC _funcptr_glVertex4d;
#define glVertex4d _funcptr_glVertex4d
CI_API extern PFNGLVERTEX4DVPROC _funcptr_glVertex4dv;
#define glVertex4dv _funcptr_glVertex4dv
CI_API extern PFNGLVERTEX4FPROC _funcptr_glVertex4f;
#define glVertex4f _funcptr_glVertex4f
CI_API extern PFNGLVERTEX4FVPROC _funcptr_glVertex4fv;
#define glVertex4fv _funcptr_glVertex4fv
CI_API extern PFNGLVERTEX4IPROC _funcptr_glVertex4i;
#define glVertex4i _funcptr_glVertex4i
CI_API extern PFNGLVERTEX4IVPROC _funcptr_glVertex4iv;
#define glVertex4iv _funcptr_glVertex4iv
CI_API extern PFNGLVERTEX4SPROC _funcptr_glVertex4s;
#define glVertex4s _funcptr_glVertex4s
CI_API extern PFNGLVERTEX4SVPROC _funcptr_glVertex4sv;
#define glVertex4sv _funcptr_glVertex4sv

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif /*OPENGL_GEN_CORE_REM1_0_H*/
