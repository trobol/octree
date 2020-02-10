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

/* ------------------- BEGIN SYSTEM/COMPILER SPECIFIC -------------------- */

/* Please report any probles that you find with your compiler, which may
 * be solved in this section! There are several compilers that I have not
 * been able to test this file with yet.
 *
 * First: If we are we on Windows, we want a single define for it (_WIN32)
 * (Note: For Cygwin the compiler flag -mwin32 should be used, but to
 * make sure that things run smoothly for Cygwin users, we add __CYGWIN__
 * to the list of "valid Win32 identifiers", which removes the need for
 * -mwin32)
 */
#if !defined(_WIN32) && (defined(__WIN32__) || defined(WIN32) || defined(__CYGWIN__))
#define _WIN32
#endif /* _WIN32 */

 /* In order for extension support to be portable, we need to define an
   * OpenGL function call method. We use the keyword APIENTRY, which is
   * defined for Win32. (Note: Windows also needs this for <GL/gl.h>)
   */
#ifndef APIENTRY
#ifdef _WIN32
#define APIENTRY __stdcall
#else
#define APIENTRY
#endif
#define GL_APIENTRY_DEFINED
#endif /* APIENTRY */

   /* The following three defines are here solely to make some Windows-based
	  * <GL/gl.h> files happy. Theoretically we could include <windows.h>, but
	  * it has the major drawback of severely polluting our namespace.
	  */

	  /* Under Windows, we need WINGDIAPI defined */
#if !defined(WINGDIAPI) && defined(_WIN32)
#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__POCC__)
/* Microsoft Visual C++, Borland C++ Builder and Pelles C */
#define WINGDIAPI __declspec(dllimport)
#elif defined(__LCC__)
/* LCC-Win32 */
#define WINGDIAPI __stdcall
#else
/* Others (e.g. MinGW, Cygwin) */
#define WINGDIAPI extern
#endif
#define GL_WINGDIAPI_DEFINED
#endif /* WINGDIAPI */

/* Some <GL/glu.h> files also need CALLBACK defined */
#if !defined(CALLBACK) && defined(_WIN32)
#if defined(_MSC_VER)
/* Microsoft Visual C++ */
#if (defined(_M_MRX000) || defined(_M_IX86) || defined(_M_ALPHA) || defined(_M_PPC)) && !defined(MIDL_PASS)
#define CALLBACK __stdcall
#else
#define CALLBACK
#endif
#else
/* Other Windows compilers */
#define CALLBACK __stdcall
#endif
#define GLU_CALLBACK_DEFINED
#endif /* CALLBACK */

/* Microsoft Visual C++, Borland C++ and Pelles C <GL*glu.h> needs wchar_t */
#if defined(_WIN32) && (defined(_MSC_VER) || defined(__BORLANDC__) || defined(__POCC__)) && !defined(_WCHAR_T_DEFINED)
typedef unsigned short wchar_t;
#define _WCHAR_T_DEFINED
#endif /* _WCHAR_T_DEFINED */

/* ---------------- GLFW related system specific defines ----------------- */

#if defined(_WIN32) && defined(GLFW_BUILD_DLL)

/* We are building a Win32 DLL */
#define GLFWAPI __declspec(dllexport)
#define GLFWAPIENTRY __stdcall
#define GLFWCALL __stdcall

#elif defined(_WIN32) && defined(GLFW_DLL)

/* We are calling a Win32 DLL */
#if defined(__LCC__)
#define GLFWAPI extern
#else
#define GLFWAPI __declspec(dllimport)
#endif
#define GLFWAPIENTRY __stdcall
#define GLFWCALL __stdcall

#else

/* We are either building/calling a static lib or we are non-win32 */
#define GLFWAPIENTRY
#define GLFWAPI
#define GLFWCALL

#endif

/* -------------------- END SYSTEM/COMPILER SPECIFIC --------------------- */

#if defined(__linux__)
#include <dlfcn.h>
#define GLDECL				 // Empty define
#define PAPAYA_GL_LIST_WIN32 // Empty define
#endif						 // __linux__

#if defined(_WIN32)
//#define NOMINMAX
#define WIN32_LEAN_AND_MEAN

#include <stddef.h>

#define GLDECL __stdcall

#include "gl_defines.h"

typedef char GLchar;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;

#define PAPAYA_GL_LIST_WIN32                 \
	/* ret, name, params */                  \
	GLE(void, BlendEquation, GLenum mode)    \
	GLE(void, ActiveTexture, GLenum texture) \
	/* end */

#endif // _WIN32

#include <GL/gl.h>

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
	GLE(void, TexSubImage3D, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels) \
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
	GLE(void, DetachShader, GLuint program, GLuint shader)
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