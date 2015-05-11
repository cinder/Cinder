#ifndef OPENGL_GEN_CORE_REM1_3_HPP
#define OPENGL_GEN_CORE_REM1_3_HPP

#include "_int_load_test.hpp"
namespace gl
{
	enum
	{
		ADD_SIGNED                       = 0x8574,
		CLIENT_ACTIVE_TEXTURE            = 0x84E1,
		COMBINE                          = 0x8570,
		COMBINE_ALPHA                    = 0x8572,
		COMBINE_RGB                      = 0x8571,
		COMPRESSED_ALPHA                 = 0x84E9,
		COMPRESSED_INTENSITY             = 0x84EC,
		COMPRESSED_LUMINANCE             = 0x84EA,
		COMPRESSED_LUMINANCE_ALPHA       = 0x84EB,
		CONSTANT                         = 0x8576,
		DOT3_RGB                         = 0x86AE,
		DOT3_RGBA                        = 0x86AF,
		INTERPOLATE                      = 0x8575,
		MAX_TEXTURE_UNITS                = 0x84E2,
		MULTISAMPLE_BIT                  = 0x20000000,
		NORMAL_MAP                       = 0x8511,
		OPERAND0_ALPHA                   = 0x8598,
		OPERAND0_RGB                     = 0x8590,
		OPERAND1_ALPHA                   = 0x8599,
		OPERAND1_RGB                     = 0x8591,
		OPERAND2_ALPHA                   = 0x859A,
		OPERAND2_RGB                     = 0x8592,
		PREVIOUS                         = 0x8578,
		REFLECTION_MAP                   = 0x8512,
		RGB_SCALE                        = 0x8573,
		SOURCE0_ALPHA                    = 0x8588,
		SOURCE0_RGB                      = 0x8580,
		SOURCE1_ALPHA                    = 0x8589,
		SOURCE1_RGB                      = 0x8581,
		SOURCE2_ALPHA                    = 0x858A,
		SOURCE2_RGB                      = 0x8582,
		SUBTRACT                         = 0x84E7,
		TRANSPOSE_COLOR_MATRIX           = 0x84E6,
		TRANSPOSE_MODELVIEW_MATRIX       = 0x84E3,
		TRANSPOSE_PROJECTION_MATRIX      = 0x84E4,
		TRANSPOSE_TEXTURE_MATRIX         = 0x84E5,
	};
	
	namespace _detail
	{
		typedef void (CODEGEN_FUNCPTR * Proc_glClientActiveTexture)(GLenum texture);
		typedef void (CODEGEN_FUNCPTR * Proc_glLoadTransposeMatrixd)(const GLdouble * m);
		typedef void (CODEGEN_FUNCPTR * Proc_glLoadTransposeMatrixf)(const GLfloat * m);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultTransposeMatrixd)(const GLdouble * m);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultTransposeMatrixf)(const GLfloat * m);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord1d)(GLenum target, GLdouble s);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord1dv)(GLenum target, const GLdouble * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord1f)(GLenum target, GLfloat s);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord1fv)(GLenum target, const GLfloat * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord1i)(GLenum target, GLint s);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord1iv)(GLenum target, const GLint * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord1s)(GLenum target, GLshort s);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord1sv)(GLenum target, const GLshort * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord2d)(GLenum target, GLdouble s, GLdouble t);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord2dv)(GLenum target, const GLdouble * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord2f)(GLenum target, GLfloat s, GLfloat t);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord2fv)(GLenum target, const GLfloat * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord2i)(GLenum target, GLint s, GLint t);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord2iv)(GLenum target, const GLint * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord2s)(GLenum target, GLshort s, GLshort t);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord2sv)(GLenum target, const GLshort * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord3d)(GLenum target, GLdouble s, GLdouble t, GLdouble r);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord3dv)(GLenum target, const GLdouble * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord3f)(GLenum target, GLfloat s, GLfloat t, GLfloat r);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord3fv)(GLenum target, const GLfloat * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord3i)(GLenum target, GLint s, GLint t, GLint r);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord3iv)(GLenum target, const GLint * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord3s)(GLenum target, GLshort s, GLshort t, GLshort r);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord3sv)(GLenum target, const GLshort * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord4d)(GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord4dv)(GLenum target, const GLdouble * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord4f)(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord4fv)(GLenum target, const GLfloat * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord4i)(GLenum target, GLint s, GLint t, GLint r, GLint q);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord4iv)(GLenum target, const GLint * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord4s)(GLenum target, GLshort s, GLshort t, GLshort r, GLshort q);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiTexCoord4sv)(GLenum target, const GLshort * v);
	}
	
	extern _detail::Proc_glClientActiveTexture ClientActiveTexture;
	extern _detail::Proc_glLoadTransposeMatrixd LoadTransposeMatrixd;
	extern _detail::Proc_glLoadTransposeMatrixf LoadTransposeMatrixf;
	extern _detail::Proc_glMultTransposeMatrixd MultTransposeMatrixd;
	extern _detail::Proc_glMultTransposeMatrixf MultTransposeMatrixf;
	extern _detail::Proc_glMultiTexCoord1d MultiTexCoord1d;
	extern _detail::Proc_glMultiTexCoord1dv MultiTexCoord1dv;
	extern _detail::Proc_glMultiTexCoord1f MultiTexCoord1f;
	extern _detail::Proc_glMultiTexCoord1fv MultiTexCoord1fv;
	extern _detail::Proc_glMultiTexCoord1i MultiTexCoord1i;
	extern _detail::Proc_glMultiTexCoord1iv MultiTexCoord1iv;
	extern _detail::Proc_glMultiTexCoord1s MultiTexCoord1s;
	extern _detail::Proc_glMultiTexCoord1sv MultiTexCoord1sv;
	extern _detail::Proc_glMultiTexCoord2d MultiTexCoord2d;
	extern _detail::Proc_glMultiTexCoord2dv MultiTexCoord2dv;
	extern _detail::Proc_glMultiTexCoord2f MultiTexCoord2f;
	extern _detail::Proc_glMultiTexCoord2fv MultiTexCoord2fv;
	extern _detail::Proc_glMultiTexCoord2i MultiTexCoord2i;
	extern _detail::Proc_glMultiTexCoord2iv MultiTexCoord2iv;
	extern _detail::Proc_glMultiTexCoord2s MultiTexCoord2s;
	extern _detail::Proc_glMultiTexCoord2sv MultiTexCoord2sv;
	extern _detail::Proc_glMultiTexCoord3d MultiTexCoord3d;
	extern _detail::Proc_glMultiTexCoord3dv MultiTexCoord3dv;
	extern _detail::Proc_glMultiTexCoord3f MultiTexCoord3f;
	extern _detail::Proc_glMultiTexCoord3fv MultiTexCoord3fv;
	extern _detail::Proc_glMultiTexCoord3i MultiTexCoord3i;
	extern _detail::Proc_glMultiTexCoord3iv MultiTexCoord3iv;
	extern _detail::Proc_glMultiTexCoord3s MultiTexCoord3s;
	extern _detail::Proc_glMultiTexCoord3sv MultiTexCoord3sv;
	extern _detail::Proc_glMultiTexCoord4d MultiTexCoord4d;
	extern _detail::Proc_glMultiTexCoord4dv MultiTexCoord4dv;
	extern _detail::Proc_glMultiTexCoord4f MultiTexCoord4f;
	extern _detail::Proc_glMultiTexCoord4fv MultiTexCoord4fv;
	extern _detail::Proc_glMultiTexCoord4i MultiTexCoord4i;
	extern _detail::Proc_glMultiTexCoord4iv MultiTexCoord4iv;
	extern _detail::Proc_glMultiTexCoord4s MultiTexCoord4s;
	extern _detail::Proc_glMultiTexCoord4sv MultiTexCoord4sv;
	
}
#endif /*OPENGL_GEN_CORE_REM1_3_HPP*/
