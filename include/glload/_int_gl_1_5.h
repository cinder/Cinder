#ifndef OPENGL_GEN_CORE_1_5_H
#define OPENGL_GEN_CORE_1_5_H

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/
#define GL_ARRAY_BUFFER 0x8892
#define GL_ARRAY_BUFFER_BINDING 0x8894
#define GL_BUFFER_ACCESS 0x88BB
#define GL_BUFFER_MAPPED 0x88BC
#define GL_BUFFER_MAP_POINTER 0x88BD
#define GL_BUFFER_SIZE 0x8764
#define GL_BUFFER_USAGE 0x8765
#define GL_CURRENT_QUERY 0x8865
#define GL_DYNAMIC_COPY 0x88EA
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_DYNAMIC_READ 0x88E9
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_ELEMENT_ARRAY_BUFFER_BINDING 0x8895
#define GL_QUERY_COUNTER_BITS 0x8864
#define GL_QUERY_RESULT 0x8866
#define GL_QUERY_RESULT_AVAILABLE 0x8867
#define GL_READ_ONLY 0x88B8
#define GL_SAMPLES_PASSED 0x8914
#define GL_STATIC_COPY 0x88E6
#define GL_STATIC_DRAW 0x88E4
#define GL_STATIC_READ 0x88E5
#define GL_STREAM_COPY 0x88E2
#define GL_STREAM_DRAW 0x88E0
#define GL_STREAM_READ 0x88E1
#define GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING 0x889F

typedef void (CODEGEN_FUNCPTR * PFNGLBEGINQUERYPROC)(GLenum target, GLuint id);
typedef void (CODEGEN_FUNCPTR * PFNGLBINDBUFFERPROC)(GLenum target, GLuint buffer);
typedef void (CODEGEN_FUNCPTR * PFNGLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage);
typedef void (CODEGEN_FUNCPTR * PFNGLBUFFERSUBDATAPROC)(GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data);
typedef void (CODEGEN_FUNCPTR * PFNGLDELETEBUFFERSPROC)(GLsizei n, const GLuint * buffers);
typedef void (CODEGEN_FUNCPTR * PFNGLDELETEQUERIESPROC)(GLsizei n, const GLuint * ids);
typedef void (CODEGEN_FUNCPTR * PFNGLENDQUERYPROC)(GLenum target);
typedef void (CODEGEN_FUNCPTR * PFNGLGENBUFFERSPROC)(GLsizei n, GLuint * buffers);
typedef void (CODEGEN_FUNCPTR * PFNGLGENQUERIESPROC)(GLsizei n, GLuint * ids);
typedef void (CODEGEN_FUNCPTR * PFNGLGETBUFFERPARAMETERIVPROC)(GLenum target, GLenum pname, GLint * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETBUFFERPOINTERVPROC)(GLenum target, GLenum pname, GLvoid ** params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETBUFFERSUBDATAPROC)(GLenum target, GLintptr offset, GLsizeiptr size, GLvoid * data);
typedef void (CODEGEN_FUNCPTR * PFNGLGETQUERYOBJECTIVPROC)(GLuint id, GLenum pname, GLint * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETQUERYOBJECTUIVPROC)(GLuint id, GLenum pname, GLuint * params);
typedef void (CODEGEN_FUNCPTR * PFNGLGETQUERYIVPROC)(GLenum target, GLenum pname, GLint * params);
typedef GLboolean (CODEGEN_FUNCPTR * PFNGLISBUFFERPROC)(GLuint buffer);
typedef GLboolean (CODEGEN_FUNCPTR * PFNGLISQUERYPROC)(GLuint id);
typedef void * (CODEGEN_FUNCPTR * PFNGLMAPBUFFERPROC)(GLenum target, GLenum access);
typedef GLboolean (CODEGEN_FUNCPTR * PFNGLUNMAPBUFFERPROC)(GLenum target);

extern PFNGLBEGINQUERYPROC _funcptr_glBeginQuery;
#define glBeginQuery _funcptr_glBeginQuery
extern PFNGLBINDBUFFERPROC _funcptr_glBindBuffer;
#define glBindBuffer _funcptr_glBindBuffer
extern PFNGLBUFFERDATAPROC _funcptr_glBufferData;
#define glBufferData _funcptr_glBufferData
extern PFNGLBUFFERSUBDATAPROC _funcptr_glBufferSubData;
#define glBufferSubData _funcptr_glBufferSubData
extern PFNGLDELETEBUFFERSPROC _funcptr_glDeleteBuffers;
#define glDeleteBuffers _funcptr_glDeleteBuffers
extern PFNGLDELETEQUERIESPROC _funcptr_glDeleteQueries;
#define glDeleteQueries _funcptr_glDeleteQueries
extern PFNGLENDQUERYPROC _funcptr_glEndQuery;
#define glEndQuery _funcptr_glEndQuery
extern PFNGLGENBUFFERSPROC _funcptr_glGenBuffers;
#define glGenBuffers _funcptr_glGenBuffers
extern PFNGLGENQUERIESPROC _funcptr_glGenQueries;
#define glGenQueries _funcptr_glGenQueries
extern PFNGLGETBUFFERPARAMETERIVPROC _funcptr_glGetBufferParameteriv;
#define glGetBufferParameteriv _funcptr_glGetBufferParameteriv
extern PFNGLGETBUFFERPOINTERVPROC _funcptr_glGetBufferPointerv;
#define glGetBufferPointerv _funcptr_glGetBufferPointerv
extern PFNGLGETBUFFERSUBDATAPROC _funcptr_glGetBufferSubData;
#define glGetBufferSubData _funcptr_glGetBufferSubData
extern PFNGLGETQUERYOBJECTIVPROC _funcptr_glGetQueryObjectiv;
#define glGetQueryObjectiv _funcptr_glGetQueryObjectiv
extern PFNGLGETQUERYOBJECTUIVPROC _funcptr_glGetQueryObjectuiv;
#define glGetQueryObjectuiv _funcptr_glGetQueryObjectuiv
extern PFNGLGETQUERYIVPROC _funcptr_glGetQueryiv;
#define glGetQueryiv _funcptr_glGetQueryiv
extern PFNGLISBUFFERPROC _funcptr_glIsBuffer;
#define glIsBuffer _funcptr_glIsBuffer
extern PFNGLISQUERYPROC _funcptr_glIsQuery;
#define glIsQuery _funcptr_glIsQuery
extern PFNGLMAPBUFFERPROC _funcptr_glMapBuffer;
#define glMapBuffer _funcptr_glMapBuffer
extern PFNGLUNMAPBUFFERPROC _funcptr_glUnmapBuffer;
#define glUnmapBuffer _funcptr_glUnmapBuffer

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif /*OPENGL_GEN_CORE_1_5_H*/
