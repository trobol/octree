/*
	gl_lite.h - Single-header multi-platform OpenGL function loader

	----------------------------------------------------------------------------
	USAGE
	----------------------------------------------------------------------------
	1) Add the following lines in exactly one of your cpp files to compile the
	   implementation:

		   #define GL_LITE_IMPLEMENTATION
		   #include "gl_lite.h"

	2) In all other files in which you want to use OpenGL functions, simply
	   include this header file as follows:

		   #include "gl_lite.h"

	3) Call gl_lite_init() before using any OpenGL function and after you have a
	   valid OpenGL context.

	----------------------------------------------------------------------------
	LICENSE
	----------------------------------------------------------------------------
	This software is in the public domain. Where that dedication is not
	recognized, you are granted a perpetual, irrevocable license to copy,
	distribute, and modify this file as you see fit.
*/
#ifndef GL_LITE_H
#define GL_LITE_H

#if defined(__linux__)
#define GLDECL // Empty define
#define PAPAYA_GL_LIST_WIN32 // Empty define
#include <dlfcn.h>
#endif // __linux__

#if defined(_WIN32)
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define GLDECL WINAPI

#include "gl_defines.h"

typedef char GLchar;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;

#define PAPAYA_GL_LIST_WIN32 \
    /* ret, name, params */ \
    GLE(void,      BlendEquation,           GLenum mode) \
    GLE(void,      ActiveTexture,           GLenum texture) \
    /* end */

#endif // _WIN32


#include <GL/glu.h>


 typedef void (APIENTRY *DEBUGPROC)(GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar *message,
            const void *userParam);

#define PAPAYA_GL_LIST                                                                                                                                                                                \
	/* ret, name, params */                                                                                                                                                                           \
	GLE(void, AttachShader, GLuint program, GLuint shader)                                                                                                                                            \
	GLE(void, BindBuffer, GLenum target, GLuint buffer)                                                                                                                                               \
	GLE(void, BindBufferBase, GLenum target, GLuint index, GLuint buffer)                                                                                                                             \
	GLE(void, BindBufferRange, GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)                                                                                          \
	GLE(void, BindVertexArray, GLuint array)                                                                                                                                                          \
	GLE(void, BindFramebuffer, GLenum target, GLuint framebuffer)                                                                                                                                     \
	GLE(void, BufferData, GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage)                                                                                                           \
	GLE(void, BufferSubData, GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data)                                                                                                     \
	GLE(GLenum, CheckFramebufferStatus, GLenum target)                                                                                                                                                \
	GLE(void, ClearBufferfv, GLenum buffer, GLint drawbuffer, const GLfloat *value)                                                                                                                   \
	GLE(void, CompileShader, GLuint shader)                                                                                                                                                           \
	GLE(GLuint, CreateProgram, void)                                                                                                                                                                  \
	GLE(GLuint, CreateShader, GLenum type)                                                                                                                                                            \
	GLE(void, DeleteShader, GLuint shader)                                                                                                                                                            \
	GLE(void, DeleteBuffers, GLsizei n, const GLuint *buffers)                                                                                                                                        \
	GLE(void, DeleteFramebuffers, GLsizei n, const GLuint *framebuffers)                                                                                                                              \
	GLE(void, DrawBuffers, GLsizei n, const GLenum *bufs)                                                                                                                                             \
	GLE(void, EnableVertexAttribArray, GLuint index)                                                                                                                                                  \
	GLE(void, FramebufferTexture2D, GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)                                                                                  \
	GLE(void, GenBuffers, GLsizei n, GLuint *buffers)                                                                                                                                                 \
	GLE(void, GenerateMipmap, GLenum target)                                                                                                                                                          \
	GLE(void, GenVertexArrays, GLsizei n, GLuint *arrays)                                                                                                                                             \
	GLE(void, GenFramebuffers, GLsizei n, GLuint *framebuffers)                                                                                                                                       \
	GLE(GLint, GetAttribLocation, GLuint program, const GLchar *name)                                                                                                                                 \
	GLE(void, GetShaderInfoLog, GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog)                                                                                                     \
	GLE(void, GetShaderiv, GLuint shader, GLenum pname, GLint *params)                                                                                                                                \
	GLE(void, GetProgramiv, GLuint program, GLenum pname, GLint *params)                                                                                                                              \
	GLE(void, GetProgramInfoLog, GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog)                                                                                                   \
	GLE(GLint, GetUniformLocation, GLuint program, const GLchar *name)                                                                                                                                \
	GLE(GLuint, GetUniformBlockIndex, GLuint program, const GLchar *uniformBlockName)                                                                                                                 \
	GLE(void, LinkProgram, GLuint program)                                                                                                                                                            \
	GLE(void, ShaderSource, GLuint shader, GLsizei count, const GLchar *const *string, const GLint *length)                                                                                           \
	GLE(void, Uniform1i, GLint location, GLint v0)                                                                                                                                                    \
	GLE(void, Uniform1f, GLint location, GLfloat v0)                                                                                                                                                  \
	GLE(void, Uniform2f, GLint location, GLfloat v0, GLfloat v1)                                                                                                                                      \
	GLE(void, Uniform4f, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3)                                                                                                              \
	GLE(void, UniformMatrix4fv, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value)                                                                                             \
	GLE(void, UniformBlockBinding, GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding)                                                                                              \
	GLE(void, UseProgram, GLuint program)                                                                                                                                                             \
	GLE(void, VertexAttribPointer, GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer)                                                                \
	GLE(void, TexStorage3D, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth)                                                                       \
	GLE(void *, MapBuffer, GLenum target, GLenum access)                                                                                                                                              \
	GLE(GLboolean, UnmapBuffer, GLenum target)                                                                                                                                                        \
	GLE(void *, MapBufferRange, GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access)                                                                                                 \
	GLE(void, GetBufferParameteriv, GLenum target, GLenum value, GLint *data)                                                                                                                         \
	GLE(void, BindTextureUnit, GLuint unit, GLuint texture)                                                                                                                                           \
	GLE(void, VertexAttribDivisor, GLuint index, GLuint divisor)                                                                                                                                      \
	GLE(void, DrawElementsInstanced, GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount)                                                                             \
	GLE(void, DrawArraysInstanced, GLenum mode, GLint first, GLsizei count, GLsizei instancecount)                                                                                                    \
	GLE(void, VertexAttribIPointer, GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer)                                                                                     \
	GLE(void, BindRenderbuffer, GLenum target, GLuint renderbuffer)                                                                                                                                   \
	GLE(void, GenRenderbuffers, GLsizei n, GLuint *renderbuffers)                                                                                                                                     \
	GLE(void, RenderbufferStorage, GLenum target, GLenum internalformat, GLsizei width, GLsizei height)                                                                                               \
	GLE(void, FramebufferRenderbuffer, GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)                                                                              \
	GLE(void, FramebufferTexture, GLenum target, GLenum attachment, GLuint texture, GLint level)                                                                                                      \
	GLE(void, BindImageTexture, GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format)                                                               \
	GLE(void, GetIntegeri_v, GLenum pname, GLuint index, GLint *data)                                                                                                                                 \
	GLE(void, DetachShader, GLuint program, GLuint shader)                                                                                                                                            \
	GLE(void, DebugMessageCallback, DEBUGPROC callback, const void* userParam)
//	GLE(void, TexSubImage3D, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels) \
//	GLE(void, GetIntegerv, GLenum pname, GLint *params)                                                                                                                                               



/* end */

#define GLE(ret, name, ...)                     \
	typedef ret GLDECL name##proc(__VA_ARGS__); \
	extern name##proc *gl##name;
PAPAYA_GL_LIST
PAPAYA_GL_LIST_WIN32
#undef GLE

bool gl_lite_init();

#endif //GL_LITE_H

// =============================================================================

#ifdef GL_LITE_IMPLEMENTATION


#define GLE(ret, name, ...) name##proc *gl##name;
PAPAYA_GL_LIST
PAPAYA_GL_LIST_WIN32
#undef GLE

bool gl_lite_init()
{
#if defined(__linux__)

	void* libGL = dlopen("libGL.so", RTLD_LAZY);
	if (!libGL)
	{
		printf("ERROR: libGL.so couldn't be loaded\n");
		return false;
	}

#define GLE(ret, name, ...)                                                \
	gl##name = (name##proc *)dlsym(libGL, "gl" #name);                     \
	if (!gl##name)                                                         \
	{                                                                      \
		printf("Function gl" #name " couldn't be loaded from libGL.so\n"); \
		return false;                                                      \
	}
	PAPAYA_GL_LIST
#undef GLE

#elif defined(_WIN32)

	HINSTANCE dll = LoadLibraryA("opengl32.dll");
	typedef PROC WINAPI wglGetProcAddressproc(LPCSTR lpszProc);
	if (!dll)
	{
		OutputDebugStringA("opengl32.dll not found.\n");
		return false;
	}
	wglGetProcAddressproc* wglGetProcAddress =
		(wglGetProcAddressproc*)GetProcAddress(dll, "wglGetProcAddress");

#define GLE(ret, name, ...)                                                                \
	gl##name = (name##proc *)wglGetProcAddress("gl" #name);                                \
	if (!gl##name)                                                                         \
	{                                                                                      \
		OutputDebugStringA("Function gl" #name " couldn't be loaded from opengl32.dll\n"); \
		return false;                                                                      \
	}
	PAPAYA_GL_LIST
		PAPAYA_GL_LIST_WIN32
#undef GLE

#else
#error "GL loading for this platform is not implemented yet."
#endif

	return true;
}

#endif //GL_LITE_IMPLEMENTATION