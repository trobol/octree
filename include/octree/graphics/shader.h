#ifndef _SHADER_H
#define _SHADER_H

#include "gl_lite.h"
#include <string>

class Shader
{
public:
	Shader(){};
	static Shader Load(std::string vertex_file_path, std::string fragment_file_path);
	static Shader Load(const char* vertex_file_path, const char* fragment_file_path);
	static Shader LoadCompute(std::string path);
	static Shader LoadCompute(const char* path);
	operator GLuint()
	{
		return mId;
	}
	void use() { glUseProgram(mId); }

private:
	Shader(GLuint id) : mId{id} {};
	GLuint mId = 0;
};

#endif