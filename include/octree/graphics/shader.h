#ifndef _SHADER_H
#define _SHADER_H

#include "gl_lite.h"
#include <string>
#include "uniform.h"

class Shader;
struct ShaderLoaderEntry;
struct UniformInfo;
struct ShaderLoaderEntry {
	const char* vertex_path;
	const char* frag_path;
	GLuint id;
	ShaderLoaderEntry* next;
	UniformInfo* uniform_info;
	const char str_buf[];
};


class Shader
{
public:
	Shader() {};
	Shader( ShaderLoaderEntry*_entry ) : entry{_entry} {};
	//static Shader Load(std::string vertex_file_path, std::string fragment_file_path);
	//static Shader Load(const char* vertex_file_path, const char* fragment_file_path, const char* geometry_file_path = nullptr);
	//static Shader LoadCompute(std::string path);
	//static Shader LoadCompute(const char* path);
	operator GLuint()
	{
		return entry->id;
	}
	void use() { if (entry->id) glUseProgram(entry->id); }

	template <class T>
	// nothing but auto works here... :(
	auto getUniform(const char* name) {
		UniformInfo* info = getUniformInfo(name);
		return Uniform<T>(info);
	}
private:
	UniformInfo* getUniformInfo(const char* name);
	ShaderLoaderEntry* entry;
};


class ShaderLoader 
{
public:
	Shader add( const char* vertex_path, const char* frag_path );
	void load();
private:
	ShaderLoaderEntry* first;
	ShaderLoaderEntry* last;
};

#endif