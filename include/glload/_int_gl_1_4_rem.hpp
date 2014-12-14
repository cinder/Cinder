#ifndef OPENGL_GEN_CORE_REM1_4_HPP
#define OPENGL_GEN_CORE_REM1_4_HPP

#include "_int_load_test.hpp"
namespace gl
{
	enum
	{
		COLOR_SUM                        = 0x8458,
		COMPARE_R_TO_TEXTURE             = 0x884E,
		CURRENT_FOG_COORDINATE           = 0x8453,
		CURRENT_SECONDARY_COLOR          = 0x8459,
		DEPTH_TEXTURE_MODE               = 0x884B,
		FOG_COORDINATE                   = 0x8451,
		FOG_COORDINATE_ARRAY             = 0x8457,
		FOG_COORDINATE_ARRAY_POINTER     = 0x8456,
		FOG_COORDINATE_ARRAY_STRIDE      = 0x8455,
		FOG_COORDINATE_ARRAY_TYPE        = 0x8454,
		FOG_COORDINATE_SOURCE            = 0x8450,
		FRAGMENT_DEPTH                   = 0x8452,
		GENERATE_MIPMAP                  = 0x8191,
		GENERATE_MIPMAP_HINT             = 0x8192,
		POINT_DISTANCE_ATTENUATION       = 0x8129,
		POINT_SIZE_MAX                   = 0x8127,
		POINT_SIZE_MIN                   = 0x8126,
		SECONDARY_COLOR_ARRAY            = 0x845E,
		SECONDARY_COLOR_ARRAY_POINTER    = 0x845D,
		SECONDARY_COLOR_ARRAY_SIZE       = 0x845A,
		SECONDARY_COLOR_ARRAY_STRIDE     = 0x845C,
		SECONDARY_COLOR_ARRAY_TYPE       = 0x845B,
		TEXTURE_FILTER_CONTROL           = 0x8500,
	};
	
	namespace _detail
	{
		typedef void (CODEGEN_FUNCPTR * Proc_glFogCoordPointer)(GLenum type, GLsizei stride, const GLvoid * pointer);
		typedef void (CODEGEN_FUNCPTR * Proc_glFogCoordd)(GLdouble coord);
		typedef void (CODEGEN_FUNCPTR * Proc_glFogCoorddv)(const GLdouble * coord);
		typedef void (CODEGEN_FUNCPTR * Proc_glFogCoordf)(GLfloat coord);
		typedef void (CODEGEN_FUNCPTR * Proc_glFogCoordfv)(const GLfloat * coord);
		typedef void (CODEGEN_FUNCPTR * Proc_glSecondaryColor3b)(GLbyte red, GLbyte green, GLbyte blue);
		typedef void (CODEGEN_FUNCPTR * Proc_glSecondaryColor3bv)(const GLbyte * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glSecondaryColor3d)(GLdouble red, GLdouble green, GLdouble blue);
		typedef void (CODEGEN_FUNCPTR * Proc_glSecondaryColor3dv)(const GLdouble * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glSecondaryColor3f)(GLfloat red, GLfloat green, GLfloat blue);
		typedef void (CODEGEN_FUNCPTR * Proc_glSecondaryColor3fv)(const GLfloat * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glSecondaryColor3i)(GLint red, GLint green, GLint blue);
		typedef void (CODEGEN_FUNCPTR * Proc_glSecondaryColor3iv)(const GLint * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glSecondaryColor3s)(GLshort red, GLshort green, GLshort blue);
		typedef void (CODEGEN_FUNCPTR * Proc_glSecondaryColor3sv)(const GLshort * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glSecondaryColor3ub)(GLubyte red, GLubyte green, GLubyte blue);
		typedef void (CODEGEN_FUNCPTR * Proc_glSecondaryColor3ubv)(const GLubyte * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glSecondaryColor3ui)(GLuint red, GLuint green, GLuint blue);
		typedef void (CODEGEN_FUNCPTR * Proc_glSecondaryColor3uiv)(const GLuint * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glSecondaryColor3us)(GLushort red, GLushort green, GLushort blue);
		typedef void (CODEGEN_FUNCPTR * Proc_glSecondaryColor3usv)(const GLushort * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glSecondaryColorPointer)(GLint size, GLenum type, GLsizei stride, const GLvoid * pointer);
		typedef void (CODEGEN_FUNCPTR * Proc_glWindowPos2d)(GLdouble x, GLdouble y);
		typedef void (CODEGEN_FUNCPTR * Proc_glWindowPos2dv)(const GLdouble * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glWindowPos2f)(GLfloat x, GLfloat y);
		typedef void (CODEGEN_FUNCPTR * Proc_glWindowPos2fv)(const GLfloat * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glWindowPos2i)(GLint x, GLint y);
		typedef void (CODEGEN_FUNCPTR * Proc_glWindowPos2iv)(const GLint * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glWindowPos2s)(GLshort x, GLshort y);
		typedef void (CODEGEN_FUNCPTR * Proc_glWindowPos2sv)(const GLshort * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glWindowPos3d)(GLdouble x, GLdouble y, GLdouble z);
		typedef void (CODEGEN_FUNCPTR * Proc_glWindowPos3dv)(const GLdouble * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glWindowPos3f)(GLfloat x, GLfloat y, GLfloat z);
		typedef void (CODEGEN_FUNCPTR * Proc_glWindowPos3fv)(const GLfloat * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glWindowPos3i)(GLint x, GLint y, GLint z);
		typedef void (CODEGEN_FUNCPTR * Proc_glWindowPos3iv)(const GLint * v);
		typedef void (CODEGEN_FUNCPTR * Proc_glWindowPos3s)(GLshort x, GLshort y, GLshort z);
		typedef void (CODEGEN_FUNCPTR * Proc_glWindowPos3sv)(const GLshort * v);
	}
	
	extern _detail::Proc_glFogCoordPointer FogCoordPointer;
	extern _detail::Proc_glFogCoordd FogCoordd;
	extern _detail::Proc_glFogCoorddv FogCoorddv;
	extern _detail::Proc_glFogCoordf FogCoordf;
	extern _detail::Proc_glFogCoordfv FogCoordfv;
	extern _detail::Proc_glSecondaryColor3b SecondaryColor3b;
	extern _detail::Proc_glSecondaryColor3bv SecondaryColor3bv;
	extern _detail::Proc_glSecondaryColor3d SecondaryColor3d;
	extern _detail::Proc_glSecondaryColor3dv SecondaryColor3dv;
	extern _detail::Proc_glSecondaryColor3f SecondaryColor3f;
	extern _detail::Proc_glSecondaryColor3fv SecondaryColor3fv;
	extern _detail::Proc_glSecondaryColor3i SecondaryColor3i;
	extern _detail::Proc_glSecondaryColor3iv SecondaryColor3iv;
	extern _detail::Proc_glSecondaryColor3s SecondaryColor3s;
	extern _detail::Proc_glSecondaryColor3sv SecondaryColor3sv;
	extern _detail::Proc_glSecondaryColor3ub SecondaryColor3ub;
	extern _detail::Proc_glSecondaryColor3ubv SecondaryColor3ubv;
	extern _detail::Proc_glSecondaryColor3ui SecondaryColor3ui;
	extern _detail::Proc_glSecondaryColor3uiv SecondaryColor3uiv;
	extern _detail::Proc_glSecondaryColor3us SecondaryColor3us;
	extern _detail::Proc_glSecondaryColor3usv SecondaryColor3usv;
	extern _detail::Proc_glSecondaryColorPointer SecondaryColorPointer;
	extern _detail::Proc_glWindowPos2d WindowPos2d;
	extern _detail::Proc_glWindowPos2dv WindowPos2dv;
	extern _detail::Proc_glWindowPos2f WindowPos2f;
	extern _detail::Proc_glWindowPos2fv WindowPos2fv;
	extern _detail::Proc_glWindowPos2i WindowPos2i;
	extern _detail::Proc_glWindowPos2iv WindowPos2iv;
	extern _detail::Proc_glWindowPos2s WindowPos2s;
	extern _detail::Proc_glWindowPos2sv WindowPos2sv;
	extern _detail::Proc_glWindowPos3d WindowPos3d;
	extern _detail::Proc_glWindowPos3dv WindowPos3dv;
	extern _detail::Proc_glWindowPos3f WindowPos3f;
	extern _detail::Proc_glWindowPos3fv WindowPos3fv;
	extern _detail::Proc_glWindowPos3i WindowPos3i;
	extern _detail::Proc_glWindowPos3iv WindowPos3iv;
	extern _detail::Proc_glWindowPos3s WindowPos3s;
	extern _detail::Proc_glWindowPos3sv WindowPos3sv;
	
}
#endif /*OPENGL_GEN_CORE_REM1_4_HPP*/
