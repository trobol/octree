#define GL_RENDERBUFFER 0x8D41
#define GL_DEPTH_ATTACHMENT 0x8D00
#define GL_BGR 0x80E0
#define GL_RGBA32F 0x8814
#define GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS 0x90EB
#define GL_MAX_COMPUTE_WORK_GROUP_COUNT 0x91BE
#define GL_MAX_COMPUTE_WORK_GROUP_SIZE 0x91BF



#define GL_ARRAY_BUFFER 0x8892		   // Acquired from:
#define GL_ARRAY_BUFFER_BINDING 0x8894 // https://www.opengl.org/registry/api/GL/glext.h
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_COMPILE_STATUS 0x8B81
#define GL_CURRENT_PROGRAM 0x8B8D
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_ELEMENT_ARRAY_BUFFER_BINDING 0x8895
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_FRAMEBUFFER 0x8D40
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#define GL_FUNC_ADD 0x8006
#define GL_INVALID_FRAMEBUFFER_OPERATION 0x0506
#define GL_MAJOR_VERSION 0x821B
#define GL_MINOR_VERSION 0x821C
#define GL_STATIC_DRAW 0x88E4
#define GL_STREAM_DRAW 0x88E0
#define GL_TEXTURE0 0x84C0
#define GL_GEOMETRY_SHADER 0x8DD9
#define GL_VERTEX_SHADER 0x8B31
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_INFO_LOG_LENGTH 0x8B84

#define GL_UNIFORM_BUFFER 0x8A11

#define GL_LINK_STATUS 0x8B82
#define GL_CLAMP_TO_BORDER 0x812D

#define GL_TEXTURE_2D_ARRAY 0x8C1A
#define GL_WRITE_ONLY 0x88B9
#define GL_BUFFER_SIZE 0x8764
#define GL_MAP_WRITE_BIT 0x0002
#define GL_READ_WRITE 0x88BA


#define GL_COMPUTE_SHADER 0x91B9
#define GL_SHADER_IMAGE_ACCESS_BARRIER_BIT_EXT 0x00000020
#define GL_SHADER_IMAGE_ACCESS_BARRIER_BIT 0x00000020
#define GL_SHADER_STORAGE_BUFFER          0x90D2
#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif
#ifndef GLAPI
#define GLAPI extern
#endif

#define GL_RG                             0x8227
#define GL_RG_INTEGER                     0x8228
#define GL_R8                             0x8229
#define GL_R16                            0x822A
#define GL_RG8                            0x822B
#define GL_RG16                           0x822C
#define GL_R16F                           0x822D
#define GL_R32F                           0x822E
#define GL_RG16F                          0x822F
#define GL_RG32F                          0x8230
#define GL_R8I                            0x8231
#define GL_R8UI                           0x8232
#define GL_R16I                           0x8233
#define GL_R16UI                          0x8234
#define GL_R32I                           0x8235
#define GL_R32UI                          0x8236
#define GL_RG8I                           0x8237
#define GL_RG8UI                          0x8238
#define GL_RG16I                          0x8239
#define GL_RG16UI                         0x823A
#define GL_RG32I                          0x823B
#define GL_RG32UI                         0x823C
#define GL_RED_INTEGER                    0x8D94

#define GL_DEBUG_OUTPUT                   0x92E0
#define GL_DEBUG_TYPE_ERROR               0x824C


#define GL_DEBUG_SEVERITY_HIGH            0x9146
#define GL_DEBUG_SEVERITY_MEDIUM          0x9147
#define GL_DEBUG_SEVERITY_LOW             0x9148