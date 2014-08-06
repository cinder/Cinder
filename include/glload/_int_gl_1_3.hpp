#ifndef OPENGL_GEN_CORE_1_3_HPP
#define OPENGL_GEN_CORE_1_3_HPP

#include "_int_load_test.hpp"
namespace gl
{
	enum
	{
		ACTIVE_TEXTURE                   = 0x84E0,
		CLAMP_TO_BORDER                  = 0x812D,
		COMPRESSED_RGB                   = 0x84ED,
		COMPRESSED_RGBA                  = 0x84EE,
		COMPRESSED_TEXTURE_FORMATS       = 0x86A3,
		MAX_CUBE_MAP_TEXTURE_SIZE        = 0x851C,
		MULTISAMPLE                      = 0x809D,
		NUM_COMPRESSED_TEXTURE_FORMATS   = 0x86A2,
		PROXY_TEXTURE_CUBE_MAP           = 0x851B,
		SAMPLE_ALPHA_TO_COVERAGE         = 0x809E,
		SAMPLE_ALPHA_TO_ONE              = 0x809F,
		SAMPLE_BUFFERS                   = 0x80A8,
		SAMPLE_COVERAGE                  = 0x80A0,
		SAMPLE_COVERAGE_INVERT           = 0x80AB,
		SAMPLE_COVERAGE_VALUE            = 0x80AA,
		TEXTURE0                         = 0x84C0,
		TEXTURE1                         = 0x84C1,
		TEXTURE10                        = 0x84CA,
		TEXTURE11                        = 0x84CB,
		TEXTURE12                        = 0x84CC,
		TEXTURE13                        = 0x84CD,
		TEXTURE14                        = 0x84CE,
		TEXTURE15                        = 0x84CF,
		TEXTURE16                        = 0x84D0,
		TEXTURE17                        = 0x84D1,
		TEXTURE18                        = 0x84D2,
		TEXTURE19                        = 0x84D3,
		TEXTURE2                         = 0x84C2,
		TEXTURE20                        = 0x84D4,
		TEXTURE21                        = 0x84D5,
		TEXTURE22                        = 0x84D6,
		TEXTURE23                        = 0x84D7,
		TEXTURE24                        = 0x84D8,
		TEXTURE25                        = 0x84D9,
		TEXTURE26                        = 0x84DA,
		TEXTURE27                        = 0x84DB,
		TEXTURE28                        = 0x84DC,
		TEXTURE29                        = 0x84DD,
		TEXTURE3                         = 0x84C3,
		TEXTURE30                        = 0x84DE,
		TEXTURE31                        = 0x84DF,
		TEXTURE4                         = 0x84C4,
		TEXTURE5                         = 0x84C5,
		TEXTURE6                         = 0x84C6,
		TEXTURE7                         = 0x84C7,
		TEXTURE8                         = 0x84C8,
		TEXTURE9                         = 0x84C9,
		TEXTURE_BINDING_CUBE_MAP         = 0x8514,
		TEXTURE_COMPRESSED_IMAGE_SIZE    = 0x86A0,
		TEXTURE_COMPRESSION_HINT         = 0x84EF,
		TEXTURE_CUBE_MAP_NEGATIVE_X      = 0x8516,
		TEXTURE_CUBE_MAP_NEGATIVE_Y      = 0x8518,
		TEXTURE_CUBE_MAP_NEGATIVE_Z      = 0x851A,
		TEXTURE_CUBE_MAP_POSITIVE_X      = 0x8515,
		TEXTURE_CUBE_MAP_POSITIVE_Y      = 0x8517,
		TEXTURE_CUBE_MAP_POSITIVE_Z      = 0x8519,
	};
	
	namespace _detail
	{
		typedef void (CODEGEN_FUNCPTR * Proc_glActiveTexture)(GLenum texture);
		typedef void (CODEGEN_FUNCPTR * Proc_glCompressedTexImage1D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid * data);
		typedef void (CODEGEN_FUNCPTR * Proc_glCompressedTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid * data);
		typedef void (CODEGEN_FUNCPTR * Proc_glCompressedTexImage3D)(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid * data);
		typedef void (CODEGEN_FUNCPTR * Proc_glCompressedTexSubImage1D)(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid * data);
		typedef void (CODEGEN_FUNCPTR * Proc_glCompressedTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid * data);
		typedef void (CODEGEN_FUNCPTR * Proc_glCompressedTexSubImage3D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid * data);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetCompressedTexImage)(GLenum target, GLint level, GLvoid * img);
		typedef void (CODEGEN_FUNCPTR * Proc_glSampleCoverage)(GLfloat value, GLboolean invert);
	}
	
	extern _detail::Proc_glActiveTexture ActiveTexture;
	extern _detail::Proc_glCompressedTexImage1D CompressedTexImage1D;
	extern _detail::Proc_glCompressedTexImage2D CompressedTexImage2D;
	extern _detail::Proc_glCompressedTexImage3D CompressedTexImage3D;
	extern _detail::Proc_glCompressedTexSubImage1D CompressedTexSubImage1D;
	extern _detail::Proc_glCompressedTexSubImage2D CompressedTexSubImage2D;
	extern _detail::Proc_glCompressedTexSubImage3D CompressedTexSubImage3D;
	extern _detail::Proc_glGetCompressedTexImage GetCompressedTexImage;
	extern _detail::Proc_glSampleCoverage SampleCoverage;
	
}
#endif /*OPENGL_GEN_CORE_1_3_HPP*/
