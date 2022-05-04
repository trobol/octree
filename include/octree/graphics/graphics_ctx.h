#ifndef _GRAPHICS_CTX_H
#define _GRAPHICS_CTX_H
#include "shader.h"
#include <string>

class GraphicsCtxPtr {

	Shader GetShader(std::string vert_path, std::string frag_path);
	Shader
	 GetShader(const char* vert_path, const char* frag_path);

	void ReloadShader(Shader* shader);

	struct GraphicsCtx* ctx;
};




#endif