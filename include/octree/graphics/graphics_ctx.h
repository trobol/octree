#ifndef _GRAPHICS_CTX_H
#define _GRAPHICS_CTX_H

#include <string>

class Shader;
class GraphicsCtx;
class GraphicsCtxPtr {

	Shader* GetShader(std::string vert_path, std::string frag_path);
	Shader* GetShader(const char* vert_path, const char* frag_path);

	GraphicsCtx* ctx;
};


#endif