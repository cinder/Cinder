#ifndef OPENGL_GEN_CORE_3_1_H
#define OPENGL_GEN_CORE_3_1_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
#define GL_INT_SAMPLER_2D_RECT 0x8DCD
#define GL_INT_SAMPLER_BUFFER 0x8DD0
#define GL_MAX_RECTANGLE_TEXTURE_SIZE 0x84F8
#define GL_MAX_TEXTURE_BUFFER_SIZE 0x8C2B
#define GL_PRIMITIVE_RESTART 0x8F9D
#define GL_PRIMITIVE_RESTART_INDEX 0x8F9E
#define GL_PROXY_TEXTURE_RECTANGLE 0x84F7
#define GL_SAMPLER_2D_RECT 0x8B63
#define GL_SAMPLER_2D_RECT_SHADOW 0x8B64
#define GL_SAMPLER_BUFFER 0x8DC2
#define GL_TEXTURE_BINDING_BUFFER 0x8C2C
#define GL_TEXTURE_BINDING_RECTANGLE 0x84F6
#define GL_TEXTURE_BUFFER_DATA_STORE_BINDING 0x8C2D
#define GL_UNSIGNED_INT_SAMPLER_2D_RECT 0x8DD5
#define GL_UNSIGNED_INT_SAMPLER_BUFFER 0x8DD8

typedef void (CODEGEN_FUNCPTR * PFNGLCOPYBUFFERSUBDATAPROC)(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);
typedef void (CODEGEN_FUNCPTR * PFNGLDRAWARRAYSINSTANCEDPROC)(GLenum mode, GLint first, GLsizei count, GLsizei instancecount);
typedef void (CODEGEN_FUNCPTR * PFNGLDRAWELEMENTSINSTANCEDPROC)(GLenum mode, GLsizei count, GLenum type, const GLvoid * indices, GLsizei instancecount);
typedef void (CODEGEN_FUNCPTR * PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC)(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei * length, GLchar * uniformBlockName);
typedef void (CODEGEN_FUNCPTR * PFNGLGETACTIVEUNIFORMBLOCKIVPROC)(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETACTIVEUNIFORMNAMEPROC)(GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei * length, GLchar * uniformName);
typedef void (CODEGEN_FUNCPTR * PFNGLGETACTIVEUNIFORMSIVPROC)(GLuint program, GLsizei uniformCount, const GLuint * uniformIndices, GLenum pname, GLint * params);
typedef GLuint (CODEGEN_FUNCPTR * PFNGLGETUNIFORMBLOCKINDEXPROC)(GLuint program, const GLchar * uniformBlockName);
typedef void (CODEGEN_FUNCPTR * PFNGLGETUNIFORMINDICESPROC)(GLuint program, GLsizei uniformCount, const GLchar *const* uniformNames, GLuint * uniformIndices);
typedef void (CODEGEN_FUNCPTR * PFNGLPRIMITIVERESTARTINDEXPROC)(GLuint index);
typedef void (CODEGEN_FUNCPTR * PFNGLTEXBUFFERPROC)(GLenum target, GLenum internalformat, GLuint buffer);
typedef void (CODEGEN_FUNCPTR * PFNGLUNIFORMBLOCKBINDINGPROC)(GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);

extern PFNGLCOPYBUFFERSUBDATAPROC _funcptr_glCopyBufferSubData;
#define glCopyBufferSubData _funcptr_glCopyBufferSubData
extern PFNGLDRAWARRAYSINSTANCEDPROC _funcptr_glDrawArraysInstanced;
#define glDrawArraysInstanced _funcptr_glDrawArraysInstanced
extern PFNGLDRAWELEMENTSINSTANCEDPROC _funcptr_glDrawElementsInstanced;
#define glDrawElementsInstanced _funcptr_glDrawElementsInstanced
extern PFNGLGETACTIVEUNIFORMBLOCKNAMEPROC _funcptr_glGetActiveUniformBlockName;
#define glGetActiveUniformBlockName _funcptr_glGetActiveUniformBlockName
extern PFNGLGETACTIVEUNIFORMBLOCKIVPROC _funcptr_glGetActiveUniformBlockiv;
#define glGetActiveUniformBlockiv _funcptr_glGetActiveUniformBlockiv
extern PFNGLGETACTIVEUNIFORMNAMEPROC _funcptr_glGetActiveUniformName;
#define glGetActiveUniformName _funcptr_glGetActiveUniformName
extern PFNGLGETACTIVEUNIFORMSIVPROC _funcptr_glGetActiveUniformsiv;
#define glGetActiveUniformsiv _funcptr_glGetActiveUniformsiv
extern PFNGLGETUNIFORMBLOCKINDEXPROC _funcptr_glGetUniformBlockIndex;
#define glGetUniformBlockIndex _funcptr_glGetUniformBlockIndex
extern PFNGLGETUNIFORMINDICESPROC _funcptr_glGetUniformIndices;
#define glGetUniformIndices _funcptr_glGetUniformIndices
extern PFNGLPRIMITIVERESTARTINDEXPROC _funcptr_glPrimitiveRestartIndex;
#define glPrimitiveRestartIndex _funcptr_glPrimitiveRestartIndex
extern PFNGLTEXBUFFERPROC _funcptr_glTexBuffer;
#define glTexBuffer _funcptr_glTexBuffer
extern PFNGLUNIFORMBLOCKBINDINGPROC _funcptr_glUniformBlockBinding;
#define glUniformBlockBinding _funcptr_glUniformBlockBinding

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif /*OPENGL_GEN_CORE_3_1_H*/
