#include <fstream>
#include <iostream>

#include <octree/graphics/shader.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <stdlib.h>
#include <vector>
#include <sstream>

std::string LoadFile(std::string path)
{
	std::ifstream inputFile(path);

	if (inputFile)
	{
		return std::string((std::istreambuf_iterator<char>(inputFile)),
						   std::istreambuf_iterator<char>());
	}
	std::cout << "Unable to load: " << path << std::endl;
	return std::string();
}

bool printErrorMessage(GLuint id, const char* shader_path)
{
	GLsizei infoLogLength;
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
	glCheckError();
	if (infoLogLength > 0)
	{
		printf("error while loading shader: %s\n", shader_path);
		char *errorMessage = new char[(size_t)infoLogLength + 1];
		glGetShaderInfoLog(id, infoLogLength, NULL, errorMessage);
		glCheckError();
		puts(errorMessage);
		delete[] errorMessage;
		return true;
	}
	return false;
}

bool LoadSingleShader(GLuint shaderID, const char* path) {
	char first_section[2048] = { 0 };
	char* pointers[256] = { first_section };
	GLint lens[256] = { 0 };
	GLsizei section_count = 0;
	FILE* fp = fopen(path, "r");
	if (fp == nullptr) {
		printf("SHADER ERROR: Unable to open \"%s\"\n", path);
		return true;
	}
	size_t bytes;
	do {
		if (section_count > 0) {
			pointers[section_count] = new char[2048];
		}
		bytes = fread(pointers[section_count], 1, 2048, fp);
		lens[section_count] = (GLsizei)bytes;
		section_count++;
	} while(bytes == 2048);
	fclose(fp);

	glShaderSource(shaderID, section_count, pointers, lens);
	glCheckError();
	glCompileShader(shaderID);
	glCheckError();

	for (size_t i = 1; i < section_count; i++)
		delete[] pointers[i];

	GLint result = GL_FALSE;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	printErrorMessage(shaderID, path);
	return result == GL_FALSE;
}


/*
Shader Shader::LoadCompute(std::string path) {
	return LoadCompute(path.c_str());
}

Shader Shader::LoadCompute(const char* path) {
	GLuint shaderID = glCreateShader(GL_COMPUTE_SHADER);
	LoadSingleShader(shaderID, path);


	GLuint programID = glCreateProgram();

	glAttachShader(programID, shaderID);
	glLinkProgram(programID);
	GLint result = GL_FALSE;
	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	

	glDetachShader(programID, shaderID);
	glDeleteShader(shaderID);

	return Shader(programID);
}


Shader Shader::Load(std::string vertex_file_path, std::string frag_file_path) {
	return Load(vertex_file_path.c_str(), frag_file_path.c_str());
}
*/
GLuint loadshader( const char* vertex_path, const char* frag_path )
{
	// Create the shaders
	glCheckError();
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glCheckError();
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glCheckError();
	bool vert_fail = LoadSingleShader(vertexShaderId, vertex_path);
	bool frag_fail = LoadSingleShader(fragmentShaderId, frag_path);

	if ( vert_fail || frag_fail ) 
		return 0;

	glCheckError();
	// Link the program
	//printf("Linking program\n");
	GLuint programId = glCreateProgram();

	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);
	glCheckError();
	// Check the program
	GLint result = GL_FALSE;
	glGetProgramiv(programId, GL_LINK_STATUS, &result);

	glDetachShader(programId, vertexShaderId);
	glDetachShader(programId, fragmentShaderId);
	
	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);

	if ( result == GL_FALSE ) {
		glDeleteProgram(programId);
		return 0;
	}

	return programId;
}

UniformInfo* Shader::getUniformInfo(const char* name) {
	const size_t name_len = strlen(name)+1;
	UniformInfo* info = (UniformInfo*)::operator new(sizeof(UniformInfo) + name_len);
	memcpy((void*)info->name, name, name_len);
	UniformInfo* lastinfo = entry->uniform_info;
	info->next = lastinfo;
	entry->uniform_info = info;
	if (entry->id) info->location = glGetUniformLocation( entry->id, info->name );
	glCheckError();
	entry->uniform_info = info;
	return info;
}

/*
Shader Shader::Load(const char* vertex_file_path, const char* fragment_file_path, const char* geometry_file_path )
{
	// Create the shaders
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint geometryShaderId = 0;
	if (geometry_file_path) geometryShaderId = glCreateShader(GL_GEOMETRY_SHADER);

	LoadSingleShader(vertexShaderId, vertex_file_path);
	LoadSingleShader(fragmentShaderId, fragment_file_path);
	if (geometry_file_path) LoadSingleShader(geometryShaderId, geometry_file_path);

	if ( glCheckError() ) exit( 1 );
	// Link the program
	//printf("Linking program\n");
	GLuint programId = glCreateProgram();
	if (geometry_file_path) glAttachShader(programId, geometryShaderId);
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);
	if ( glCheckError() ) exit( 1 );
	// Check the program
	GLint result = GL_FALSE;
	glGetProgramiv(programId, GL_LINK_STATUS, &result);

	if (geometry_file_path) glDetachShader(programId, geometryShaderId);
	glDetachShader(programId, vertexShaderId);
	glDetachShader(programId, fragmentShaderId);
	if (geometry_file_path) glDeleteShader( geometryShaderId );
	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);
	
	return Shader(programId);
}
*/

Shader ShaderLoader::add( const char* vertex_path, const char* frag_path )
{
	size_t vertex_path_len = strlen(vertex_path)+1;
	size_t frag_path_len = strlen(frag_path)+1;
	ShaderLoaderEntry* entry = (ShaderLoaderEntry*) ::operator new( sizeof(ShaderLoaderEntry) + vertex_path_len + frag_path_len );
	entry->frag_path = entry->str_buf + vertex_path_len;
	entry->vertex_path = entry->str_buf;
	entry->next = first;
	first = entry;
	entry->uniform_info = nullptr;
	memcpy( (void*)entry->vertex_path, vertex_path, vertex_path_len );
	memcpy( (void*)entry->frag_path, frag_path, frag_path_len );
	glCheckError();
	GLuint newprogram = loadshader(entry->vertex_path, entry->frag_path);
	glCheckError();
	if ( newprogram != 0 ) entry->id = newprogram;
	return Shader(entry);
}


void ShaderLoader::load()
{
	for( ShaderLoaderEntry* entry = first; entry != nullptr; entry = entry->next ) {
		glCheckError();
		GLuint newprogram = loadshader(entry->vertex_path, entry->frag_path);
		glCheckError();
		if ( newprogram != 0 ) {
			GLuint oldprogram = entry->id;
			entry->id = newprogram;
			if (oldprogram != 0) glDeleteProgram( oldprogram );
			for( UniformInfo* info = entry->uniform_info; info != nullptr; info = info->next ) {
				info->location = glGetUniformLocation( newprogram, info->name );
				glCheckError();
			}
		}
	}
}


