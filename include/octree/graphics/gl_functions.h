#ifndef _GL_FUNCTIONS_H
#define _GL_FUNCTIONS_H

#include <GL/glu.h>

GLboolean glUnmapBuffer( GLenum target);
GLint GetUniformLocation( GLuint program, const GLchar *name);
GLint GetAttribLocation( GLuint program, const GLchar *name);                                                                  
GLuint glGetUniformBlockIndex(GLuint program, const GLchar *uniformBlockName);
GLuint glCreateProgram(void);
GLuint glCreateShader(GLenum type);
GLenum glCheckFramebufferStatus(GLenum target);

void* glMapBuffer( GLenum target, GLenum access);                                                                                                                                                                  
void* glMapBufferRange( GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);

void glAttachShader( GLuint program, GLuint shader);                                                                             
void glBindBuffer( GLenum target, GLuint buffer);                                                                                
void glBindBufferBase( GLenum target, GLuint index, GLuint buffer);                                                              
void glBindBufferRange( GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size);                           
void glBindVertexArray( GLuint array);                                                                                           
void glBindFramebuffer( GLenum target, GLuint framebuffer);                                                                      
void glBufferData( GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);                                            
void glBufferSubData( GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid *data);                                                                                                             
void glClearBufferfv( GLenum buffer, GLint drawbuffer, const GLfloat *value);                                                    
void glCompileShader( GLuint shader);                                                                                                                                                                                    
void glDeleteShader( GLuint shader);                                                                                             
void glDeleteBuffers( GLsizei n, const GLuint *buffers);                                                                         
void glDeleteFramebuffers( GLsizei n, const GLuint *framebuffers);                                                               
void glDrawBuffers( GLsizei n, const GLenum *bufs);                                                                              
void glEnableVertexAttribArray( GLuint index);                                                                                   
void glFramebufferTexture2D( GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);                   
void glGenBuffers( GLsizei n, GLuint *buffers);                                                                                  
void glGenerateMipmap( GLenum target);                                                                                           
void glGenVertexArrays( GLsizei n, GLuint *arrays);                                                                              
void glGenFramebuffers( GLsizei n, GLuint *framebuffers);                                                                                                                                   
void glGetShaderInfoLog( GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);                                      
void glGetShaderiv( GLuint shader, GLenum pname, GLint *params);                                                                 
void glGetProgramiv( GLuint program, GLenum pname, GLint *params);                                                               
void glGetProgramInfoLog( GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);                                                                                
void glLinkProgram( GLuint program);                                                                                             
void glShaderSource( GLuint shader, GLsizei count, const GLchar *const *string, const GLint *length);                            
void glUniform1i( GLint location, GLint v0);                                                                                     
void glUniform1f( GLint location, GLfloat v0);                                                                                   
void glUniform2f( GLint location, GLfloat v0, GLfloat v1);                                                                       
void glUniform4f( GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);                                               
void glUniformMatrix4fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);                              
void glUniformBlockBinding( GLuint program, GLuint uniformBlockIndex, GLuint uniformBlockBinding);                               
void glUseProgram( GLuint program);                                                                                              
void glVertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer); 
void glTexStorage3D( GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth);                                   
void glGetBufferParameteriv( GLenum target, GLenum value, GLint *data);                                                          
void glBindTextureUnit( GLuint unit, GLuint texture);                                                                            
void glVertexAttribDivisor( GLuint index, GLuint divisor);                                                                       
void glDrawElementsInstanced( GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount);              
void glDrawArraysInstanced( GLenum mode, GLint first, GLsizei count, GLsizei instancecount);                                     
void glVertexAttribIPointer( GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);                      
void glBindRenderbuffer( GLenum target, GLuint renderbuffer);                                                                    
void glGenRenderbuffers( GLsizei n, GLuint *renderbuffers);                                                                      
void glRenderbufferStorage( GLenum target, GLenum internalformat, GLsizei width, GLsizei height);                                
void glFramebufferRenderbuffer( GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);               
void glFramebufferTexture( GLenum target, GLenum attachment, GLuint texture, GLint level);                                       
void glBindImageTexture( GLuint unit, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLenum format);
void glGetIntegeri_v( GLenum pname, GLuint index, GLint *data);                                                                  
void glDispatchCompute( GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z);                                          
void glMemoryBarrier( GLbitfield barriers);                                                                                      
void glDetachShader( GLuint program, GLuint shader);                                                                             
void glDebugMessageCallback(DEBUGPROC callback, const void* userParam);

#endif