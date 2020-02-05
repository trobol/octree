#ifndef _SHADER_H
#define _SHADER_H

#include "gl_lite.h"
#include <string>

class Shader
{
public:
	Shader() {};
	static Shader Load(std::string& vertex_file_path, std::string& fragment_file_path);
	operator unsigned int()
	{
		return mId;
	}

private:
	Shader(GLuint id) : mId{ id } {};
	GLuint mId = 0;
};

#endif