#ifndef OPENGL_GEN_CORE_1_0_HPP
#define OPENGL_GEN_CORE_1_0_HPP

#include "_int_load_test.hpp"
namespace gl
{
	namespace _detail
	{
		typedef void (CODEGEN_FUNCPTR * Proc_glBlendFunc)(GLenum sfactor, GLenum dfactor);
		typedef void (CODEGEN_FUNCPTR * Proc_glClear)(GLbitfield mask);
		typedef void (CODEGEN_FUNCPTR * Proc_glClearColor)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
		typedef void (CODEGEN_FUNCPTR * Proc_glClearDepth)(GLdouble depth);
		typedef void (CODEGEN_FUNCPTR * Proc_glClearStencil)(GLint s);
		typedef void (CODEGEN_FUNCPTR * Proc_glColorMask)(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
		typedef void (CODEGEN_FUNCPTR * Proc_glCullFace)(GLenum mode);
		typedef void (CODEGEN_FUNCPTR * Proc_glDepthFunc)(GLenum func);
		typedef void (CODEGEN_FUNCPTR * Proc_glDepthMask)(GLboolean flag);
		typedef void (CODEGEN_FUNCPTR * Proc_glDepthRange)(GLdouble ren_near, GLdouble ren_far);
		typedef void (CODEGEN_FUNCPTR * Proc_glDisable)(GLenum cap);
		typedef void (CODEGEN_FUNCPTR * Proc_glDrawBuffer)(GLenum mode);
		typedef void (CODEGEN_FUNCPTR * Proc_glEnable)(GLenum cap);
		typedef void (CODEGEN_FUNCPTR * Proc_glFinish)();
		typedef void (CODEGEN_FUNCPTR * Proc_glFlush)();
		typedef void (CODEGEN_FUNCPTR * Proc_glFrontFace)(GLenum mode);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetBooleanv)(GLenum pname, GLboolean * params);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetDoublev)(GLenum pname, GLdouble * params);
		typedef GLenum (CODEGEN_FUNCPTR * Proc_glGetError)();
		typedef void (CODEGEN_FUNCPTR * Proc_glGetFloatv)(GLenum pname, GLfloat * params);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetIntegerv)(GLenum pname, GLint * params);
		typedef const GLubyte * (CODEGEN_FUNCPTR * Proc_glGetString)(GLenum name);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetTexImage)(GLenum target, GLint level, GLenum format, GLenum type, GLvoid * pixels);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetTexLevelParameterfv)(GLenum target, GLint level, GLenum pname, GLfloat * params);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetTexLevelParameteriv)(GLenum target, GLint level, GLenum pname, GLint * params);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetTexParameterfv)(GLenum target, GLenum pname, GLfloat * params);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetTexParameteriv)(GLenum target, GLenum pname, GLint * params);
		typedef void (CODEGEN_FUNCPTR * Proc_glHint)(GLenum target, GLenum mode);
		typedef GLboolean (CODEGEN_FUNCPTR * Proc_glIsEnabled)(GLenum cap);
		typedef void (CODEGEN_FUNCPTR * Proc_glLineWidth)(GLfloat width);
		typedef void (CODEGEN_FUNCPTR * Proc_glLogicOp)(GLenum opcode);
		typedef void (CODEGEN_FUNCPTR * Proc_glPixelStoref)(GLenum pname, GLfloat param);
		typedef void (CODEGEN_FUNCPTR * Proc_glPixelStorei)(GLenum pname, GLint param);
		typedef void (CODEGEN_FUNCPTR * Proc_glPointSize)(GLfloat size);
		typedef void (CODEGEN_FUNCPTR * Proc_glPolygonMode)(GLenum face, GLenum mode);
		typedef void (CODEGEN_FUNCPTR * Proc_glReadBuffer)(GLenum mode);
		typedef void (CODEGEN_FUNCPTR * Proc_glReadPixels)(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid * pixels);
		typedef void (CODEGEN_FUNCPTR * Proc_glScissor)(GLint x, GLint y, GLsizei width, GLsizei height);
		typedef void (CODEGEN_FUNCPTR * Proc_glStencilFunc)(GLenum func, GLint ref, GLuint mask);
		typedef void (CODEGEN_FUNCPTR * Proc_glStencilMask)(GLuint mask);
		typedef void (CODEGEN_FUNCPTR * Proc_glStencilOp)(GLenum fail, GLenum zfail, GLenum zpass);
		typedef void (CODEGEN_FUNCPTR * Proc_glTexImage1D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid * pixels);
		typedef void (CODEGEN_FUNCPTR * Proc_glTexImage2D)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * pixels);
		typedef void (CODEGEN_FUNCPTR * Proc_glTexParameterf)(GLenum target, GLenum pname, GLfloat param);
		typedef void (CODEGEN_FUNCPTR * Proc_glTexParameterfv)(GLenum target, GLenum pname, const GLfloat * params);
		typedef void (CODEGEN_FUNCPTR * Proc_glTexParameteri)(GLenum target, GLenum pname, GLint param);
		typedef void (CODEGEN_FUNCPTR * Proc_glTexParameteriv)(GLenum target, GLenum pname, const GLint * params);
		typedef void (CODEGEN_FUNCPTR * Proc_glViewport)(GLint x, GLint y, GLsizei width, GLsizei height);
	}
	
	extern _detail::Proc_glBlendFunc BlendFunc;
	extern _detail::Proc_glClear Clear;
	extern _detail::Proc_glClearColor ClearColor;
	extern _detail::Proc_glClearDepth ClearDepth;
	extern _detail::Proc_glClearStencil ClearStencil;
	extern _detail::Proc_glColorMask ColorMask;
	extern _detail::Proc_glCullFace CullFace;
	extern _detail::Proc_glDepthFunc DepthFunc;
	extern _detail::Proc_glDepthMask DepthMask;
	extern _detail::Proc_glDepthRange DepthRange;
	extern _detail::Proc_glDisable Disable;
	extern _detail::Proc_glDrawBuffer DrawBuffer;
	extern _detail::Proc_glEnable Enable;
	extern _detail::Proc_glFinish Finish;
	extern _detail::Proc_glFlush Flush;
	extern _detail::Proc_glFrontFace FrontFace;
	extern _detail::Proc_glGetBooleanv GetBooleanv;
	extern _detail::Proc_glGetDoublev GetDoublev;
	extern _detail::Proc_glGetError GetError;
	extern _detail::Proc_glGetFloatv GetFloatv;
	extern _detail::Proc_glGetIntegerv GetIntegerv;
	extern _detail::Proc_glGetString GetString;
	extern _detail::Proc_glGetTexImage GetTexImage;
	extern _detail::Proc_glGetTexLevelParameterfv GetTexLevelParameterfv;
	extern _detail::Proc_glGetTexLevelParameteriv GetTexLevelParameteriv;
	extern _detail::Proc_glGetTexParameterfv GetTexParameterfv;
	extern _detail::Proc_glGetTexParameteriv GetTexParameteriv;
	extern _detail::Proc_glHint Hint;
	extern _detail::Proc_glIsEnabled IsEnabled;
	extern _detail::Proc_glLineWidth LineWidth;
	extern _detail::Proc_glLogicOp LogicOp;
	extern _detail::Proc_glPixelStoref PixelStoref;
	extern _detail::Proc_glPixelStorei PixelStorei;
	extern _detail::Proc_glPointSize PointSize;
	extern _detail::Proc_glPolygonMode PolygonMode;
	extern _detail::Proc_glReadBuffer ReadBuffer;
	extern _detail::Proc_glReadPixels ReadPixels;
	extern _detail::Proc_glScissor Scissor;
	extern _detail::Proc_glStencilFunc StencilFunc;
	extern _detail::Proc_glStencilMask StencilMask;
	extern _detail::Proc_glStencilOp StencilOp;
	extern _detail::Proc_glTexImage1D TexImage1D;
	extern _detail::Proc_glTexImage2D TexImage2D;
	extern _detail::Proc_glTexParameterf TexParameterf;
	extern _detail::Proc_glTexParameterfv TexParameterfv;
	extern _detail::Proc_glTexParameteri TexParameteri;
	extern _detail::Proc_glTexParameteriv TexParameteriv;
	extern _detail::Proc_glViewport Viewport;
	
}
#endif /*OPENGL_GEN_CORE_1_0_HPP*/
