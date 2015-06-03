#ifndef OPENGL_GEN_CORE_1_1_HPP
#define OPENGL_GEN_CORE_1_1_HPP

#include "_int_load_test.hpp"
namespace gl
{
	enum
	{
		ALWAYS                           = 0x0207,
		AND                              = 0x1501,
		AND_INVERTED                     = 0x1504,
		AND_REVERSE                      = 0x1502,
		BACK                             = 0x0405,
		BACK_LEFT                        = 0x0402,
		BACK_RIGHT                       = 0x0403,
		BLEND                            = 0x0BE2,
		BLEND_DST                        = 0x0BE0,
		BLEND_SRC                        = 0x0BE1,
		BYTE                             = 0x1400,
		CLEAR                            = 0x1500,
		COLOR                            = 0x1800,
		COLOR_BUFFER_BIT                 = 0x00004000,
		COLOR_CLEAR_VALUE                = 0x0C22,
		COLOR_LOGIC_OP                   = 0x0BF2,
		COLOR_WRITEMASK                  = 0x0C23,
		COPY                             = 0x1503,
		COPY_INVERTED                    = 0x150C,
		CULL_FACE                        = 0x0B44,
		CULL_FACE_MODE                   = 0x0B45,
		DECR                             = 0x1E03,
		DEPTH                            = 0x1801,
		DEPTH_BUFFER_BIT                 = 0x00000100,
		DEPTH_CLEAR_VALUE                = 0x0B73,
		DEPTH_COMPONENT                  = 0x1902,
		DEPTH_FUNC                       = 0x0B74,
		DEPTH_TEST                       = 0x0B71,
		DEPTH_WRITEMASK                  = 0x0B72,
		DITHER                           = 0x0BD0,
		DONT_CARE                        = 0x1100,
		DOUBLEBUFFER                     = 0x0C32,
		DRAW_BUFFER                      = 0x0C01,
		DST_ALPHA                        = 0x0304,
		DST_COLOR                        = 0x0306,
		EQUIV                            = 0x1509,
		EXTENSIONS                       = 0x1F03,
		FALSE_                           = 0,
		FASTEST                          = 0x1101,
		FILL                             = 0x1B02,
		FLOAT                            = 0x1406,
		FRONT                            = 0x0404,
		FRONT_AND_BACK                   = 0x0408,
		FRONT_FACE                       = 0x0B46,
		FRONT_LEFT                       = 0x0400,
		FRONT_RIGHT                      = 0x0401,
		GEQUAL                           = 0x0206,
		GREATER                          = 0x0204,
		INCR                             = 0x1E02,
		INT                              = 0x1404,
		INVALID_ENUM                     = 0x0500,
		INVALID_OPERATION                = 0x0502,
		INVALID_VALUE                    = 0x0501,
		KEEP                             = 0x1E00,
		LEFT                             = 0x0406,
		LEQUAL                           = 0x0203,
		LESS                             = 0x0201,
		LINE                             = 0x1B01,
		LINEAR                           = 0x2601,
		LINEAR_MIPMAP_LINEAR             = 0x2703,
		LINEAR_MIPMAP_NEAREST            = 0x2701,
		LINES                            = 0x0001,
		LINE_LOOP                        = 0x0002,
		LINE_SMOOTH                      = 0x0B20,
		LINE_SMOOTH_HINT                 = 0x0C52,
		LINE_STRIP                       = 0x0003,
		LINE_WIDTH                       = 0x0B21,
		LINE_WIDTH_GRANULARITY           = 0x0B23,
		LINE_WIDTH_RANGE                 = 0x0B22,
		LOGIC_OP_MODE                    = 0x0BF0,
		MAX_TEXTURE_SIZE                 = 0x0D33,
		MAX_VIEWPORT_DIMS                = 0x0D3A,
		NAND                             = 0x150E,
		NEAREST                          = 0x2600,
		NEAREST_MIPMAP_LINEAR            = 0x2702,
		NEAREST_MIPMAP_NEAREST           = 0x2700,
		NEVER                            = 0x0200,
		NICEST                           = 0x1102,
		NOOP                             = 0x1505,
		NOR                              = 0x1508,
		NOTEQUAL                         = 0x0205,
		ONE                              = 1,
		ONE_MINUS_DST_ALPHA              = 0x0305,
		ONE_MINUS_DST_COLOR              = 0x0307,
		ONE_MINUS_SRC_ALPHA              = 0x0303,
		ONE_MINUS_SRC_COLOR              = 0x0301,
		OR                               = 0x1507,
		OR_INVERTED                      = 0x150D,
		OR_REVERSE                       = 0x150B,
		OUT_OF_MEMORY                    = 0x0505,
		PACK_ALIGNMENT                   = 0x0D05,
		PACK_LSB_FIRST                   = 0x0D01,
		PACK_ROW_LENGTH                  = 0x0D02,
		PACK_SKIP_PIXELS                 = 0x0D04,
		PACK_SKIP_ROWS                   = 0x0D03,
		PACK_SWAP_BYTES                  = 0x0D00,
		POINT                            = 0x1B00,
		POINTS                           = 0x0000,
		POINT_SIZE                       = 0x0B11,
		POINT_SIZE_GRANULARITY           = 0x0B13,
		POINT_SIZE_RANGE                 = 0x0B12,
		POLYGON_MODE                     = 0x0B40,
		POLYGON_OFFSET_FACTOR            = 0x8038,
		POLYGON_OFFSET_FILL              = 0x8037,
		POLYGON_OFFSET_LINE              = 0x2A02,
		POLYGON_OFFSET_POINT             = 0x2A01,
		POLYGON_OFFSET_UNITS             = 0x2A00,
		POLYGON_SMOOTH                   = 0x0B41,
		POLYGON_SMOOTH_HINT              = 0x0C53,
		PROXY_TEXTURE_1D                 = 0x8063,
		PROXY_TEXTURE_2D                 = 0x8064,
		R3_G3_B2                         = 0x2A10,
		READ_BUFFER                      = 0x0C02,
		RENDERER                         = 0x1F01,
		REPEAT                           = 0x2901,
		REPLACE                          = 0x1E01,
		RGB                              = 0x1907,
		RGB10                            = 0x8052,
		RGB10_A2                         = 0x8059,
		RGB12                            = 0x8053,
		RGB16                            = 0x8054,
		RGB4                             = 0x804F,
		RGB5                             = 0x8050,
		RGB5_A1                          = 0x8057,
		RGB8                             = 0x8051,
		RGBA                             = 0x1908,
		RGBA12                           = 0x805A,
		RGBA16                           = 0x805B,
		RGBA2                            = 0x8055,
		RGBA4                            = 0x8056,
		RGBA8                            = 0x8058,
		RIGHT                            = 0x0407,
		SET                              = 0x150F,
		SHORT                            = 0x1402,
		SRC_ALPHA                        = 0x0302,
		SRC_ALPHA_SATURATE               = 0x0308,
		SRC_COLOR                        = 0x0300,
		STENCIL                          = 0x1802,
		STENCIL_BUFFER_BIT               = 0x00000400,
		STENCIL_CLEAR_VALUE              = 0x0B91,
		STENCIL_FAIL                     = 0x0B94,
		STENCIL_FUNC                     = 0x0B92,
		STENCIL_PASS_DEPTH_FAIL          = 0x0B95,
		STENCIL_PASS_DEPTH_PASS          = 0x0B96,
		STENCIL_REF                      = 0x0B97,
		STENCIL_TEST                     = 0x0B90,
		STENCIL_VALUE_MASK               = 0x0B93,
		STENCIL_WRITEMASK                = 0x0B98,
		STEREO                           = 0x0C33,
		SUBPIXEL_BITS                    = 0x0D50,
		TEXTURE                          = 0x1702,
		TEXTURE_ALPHA_SIZE               = 0x805F,
		TEXTURE_BINDING_1D               = 0x8068,
		TEXTURE_BINDING_2D               = 0x8069,
		TEXTURE_BLUE_SIZE                = 0x805E,
		TEXTURE_BORDER_COLOR             = 0x1004,
		TEXTURE_GREEN_SIZE               = 0x805D,
		TEXTURE_HEIGHT                   = 0x1001,
		TEXTURE_INTERNAL_FORMAT          = 0x1003,
		TEXTURE_MAG_FILTER               = 0x2800,
		TEXTURE_MIN_FILTER               = 0x2801,
		TEXTURE_RED_SIZE                 = 0x805C,
		TEXTURE_WIDTH                    = 0x1000,
		TEXTURE_WRAP_S                   = 0x2802,
		TEXTURE_WRAP_T                   = 0x2803,
		TRIANGLE_FAN                     = 0x0006,
		TRIANGLE_STRIP                   = 0x0005,
		TRUE_                            = 1,
		UNPACK_ALIGNMENT                 = 0x0CF5,
		UNPACK_LSB_FIRST                 = 0x0CF1,
		UNPACK_ROW_LENGTH                = 0x0CF2,
		UNPACK_SKIP_PIXELS               = 0x0CF4,
		UNPACK_SKIP_ROWS                 = 0x0CF3,
		UNPACK_SWAP_BYTES                = 0x0CF0,
		UNSIGNED_BYTE                    = 0x1401,
		UNSIGNED_INT                     = 0x1405,
		UNSIGNED_SHORT                   = 0x1403,
		VENDOR                           = 0x1F00,
		VERSION                          = 0x1F02,
	};
	
	namespace _detail
	{
		typedef void (CODEGEN_FUNCPTR * Proc_glBindTexture)(GLenum target, GLuint texture);
		typedef void (CODEGEN_FUNCPTR * Proc_glCopyTexImage1D)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
		typedef void (CODEGEN_FUNCPTR * Proc_glCopyTexImage2D)(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
		typedef void (CODEGEN_FUNCPTR * Proc_glCopyTexSubImage1D)(GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
		typedef void (CODEGEN_FUNCPTR * Proc_glCopyTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
		typedef void (CODEGEN_FUNCPTR * Proc_glDeleteTextures)(GLsizei n, const GLuint * textures);
		typedef void (CODEGEN_FUNCPTR * Proc_glDrawArrays)(GLenum mode, GLint first, GLsizei count);
		typedef void (CODEGEN_FUNCPTR * Proc_glDrawElements)(GLenum mode, GLsizei count, GLenum type, const GLvoid * indices);
		typedef void (CODEGEN_FUNCPTR * Proc_glGenTextures)(GLsizei n, GLuint * textures);
		typedef void (CODEGEN_FUNCPTR * Proc_glGetPointerv)(GLenum pname, GLvoid ** params);
		typedef GLboolean (CODEGEN_FUNCPTR * Proc_glIsTexture)(GLuint texture);
		typedef void (CODEGEN_FUNCPTR * Proc_glPolygonOffset)(GLfloat factor, GLfloat units);
		typedef void (CODEGEN_FUNCPTR * Proc_glTexSubImage1D)(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid * pixels);
		typedef void (CODEGEN_FUNCPTR * Proc_glTexSubImage2D)(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid * pixels);
	}
	
	extern _detail::Proc_glBindTexture BindTexture;
	extern _detail::Proc_glCopyTexImage1D CopyTexImage1D;
	extern _detail::Proc_glCopyTexImage2D CopyTexImage2D;
	extern _detail::Proc_glCopyTexSubImage1D CopyTexSubImage1D;
	extern _detail::Proc_glCopyTexSubImage2D CopyTexSubImage2D;
	extern _detail::Proc_glDeleteTextures DeleteTextures;
	extern _detail::Proc_glDrawArrays DrawArrays;
	extern _detail::Proc_glDrawElements DrawElements;
	extern _detail::Proc_glGenTextures GenTextures;
	extern _detail::Proc_glGetPointerv GetPointerv;
	extern _detail::Proc_glIsTexture IsTexture;
	extern _detail::Proc_glPolygonOffset PolygonOffset;
	extern _detail::Proc_glTexSubImage1D TexSubImage1D;
	extern _detail::Proc_glTexSubImage2D TexSubImage2D;
	
}
#endif /*OPENGL_GEN_CORE_1_1_HPP*/
