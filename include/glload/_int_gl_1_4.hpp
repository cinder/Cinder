#ifndef OPENGL_GEN_CORE_1_4_HPP
#define OPENGL_GEN_CORE_1_4_HPP

#include "_int_load_test.hpp"
namespace gl
{
	enum
	{
		BLEND_DST_ALPHA                  = 0x80CA,
		BLEND_DST_RGB                    = 0x80C8,
		BLEND_SRC_ALPHA                  = 0x80CB,
		BLEND_SRC_RGB                    = 0x80C9,
		DECR_WRAP                        = 0x8508,
		DEPTH_COMPONENT16                = 0x81A5,
		DEPTH_COMPONENT24                = 0x81A6,
		DEPTH_COMPONENT32                = 0x81A7,
		INCR_WRAP                        = 0x8507,
		MAX_TEXTURE_LOD_BIAS             = 0x84FD,
		MIRRORED_REPEAT                  = 0x8370,
		POINT_FADE_THRESHOLD_SIZE        = 0x8128,
		TEXTURE_COMPARE_FUNC             = 0x884D,
		TEXTURE_COMPARE_MODE             = 0x884C,
		TEXTURE_DEPTH_SIZE               = 0x884A,
		TEXTURE_LOD_BIAS                 = 0x8501,
	};
	
	namespace _detail
	{
		typedef void (CODEGEN_FUNCPTR * Proc_glBlendFuncSeparate)(GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiDrawArrays)(GLenum mode, const GLint * first, const GLsizei * count, GLsizei drawcount);
		typedef void (CODEGEN_FUNCPTR * Proc_glMultiDrawElements)(GLenum mode, const GLsizei * count, GLenum type, const GLvoid *const* indices, GLsizei drawcount);
		typedef void (CODEGEN_FUNCPTR * Proc_glPointParameterf)(GLenum pname, GLfloat param);
		typedef void (CODEGEN_FUNCPTR * Proc_glPointParameterfv)(GLenum pname, const GLfloat * params);
		typedef void (CODEGEN_FUNCPTR * Proc_glPointParameteri)(GLenum pname, GLint param);
		typedef void (CODEGEN_FUNCPTR * Proc_glPointParameteriv)(GLenum pname, const GLint * params);
	}
	
	extern _detail::Proc_glBlendFuncSeparate BlendFuncSeparate;
	extern _detail::Proc_glMultiDrawArrays MultiDrawArrays;
	extern _detail::Proc_glMultiDrawElements MultiDrawElements;
	extern _detail::Proc_glPointParameterf PointParameterf;
	extern _detail::Proc_glPointParameterfv PointParameterfv;
	extern _detail::Proc_glPointParameteri PointParameteri;
	extern _detail::Proc_glPointParameteriv PointParameteriv;
	
}
#endif /*OPENGL_GEN_CORE_1_4_HPP*/
