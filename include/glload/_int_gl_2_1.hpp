#ifndef OPENGL_GEN_CORE_2_1_HPP
#define OPENGL_GEN_CORE_2_1_HPP

#include "_int_load_test.hpp"
namespace gl
{
	enum
	{
		COMPRESSED_SRGB                  = 0x8C48,
		COMPRESSED_SRGB_ALPHA            = 0x8C49,
		FLOAT_MAT2x3                     = 0x8B65,
		FLOAT_MAT2x4                     = 0x8B66,
		FLOAT_MAT3x2                     = 0x8B67,
		FLOAT_MAT3x4                     = 0x8B68,
		FLOAT_MAT4x2                     = 0x8B69,
		FLOAT_MAT4x3                     = 0x8B6A,
		PIXEL_PACK_BUFFER                = 0x88EB,
		PIXEL_PACK_BUFFER_BINDING        = 0x88ED,
		PIXEL_UNPACK_BUFFER              = 0x88EC,
		PIXEL_UNPACK_BUFFER_BINDING      = 0x88EF,
		SRGB                             = 0x8C40,
		SRGB8                            = 0x8C41,
		SRGB8_ALPHA8                     = 0x8C43,
		SRGB_ALPHA                       = 0x8C42,
	};
	
	namespace _detail
	{
		typedef void (CODEGEN_FUNCPTR * Proc_glUniformMatrix2x3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniformMatrix2x4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniformMatrix3x2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniformMatrix3x4fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniformMatrix4x2fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
		typedef void (CODEGEN_FUNCPTR * Proc_glUniformMatrix4x3fv)(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value);
	}
	
	extern _detail::Proc_glUniformMatrix2x3fv UniformMatrix2x3fv;
	extern _detail::Proc_glUniformMatrix2x4fv UniformMatrix2x4fv;
	extern _detail::Proc_glUniformMatrix3x2fv UniformMatrix3x2fv;
	extern _detail::Proc_glUniformMatrix3x4fv UniformMatrix3x4fv;
	extern _detail::Proc_glUniformMatrix4x2fv UniformMatrix4x2fv;
	extern _detail::Proc_glUniformMatrix4x3fv UniformMatrix4x3fv;
	
}
#endif /*OPENGL_GEN_CORE_2_1_HPP*/
