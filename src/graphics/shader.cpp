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

void printErrorMessage(GLuint id)
{
	GLsizei infoLogLength;
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0)
	{
		char *errorMessage = new char[(size_t)infoLogLength + 1];
		glGetShaderInfoLog(id, infoLogLength, NULL, errorMessage);
		puts(errorMessage);
		delete[] errorMessage;
	}
}

void LoadSingleShader(GLuint shaderID, const char* path) {
	char first_section[2048] = {};
	char* pointers[16] = { first_section };
	size_t section_count = 0;
	FILE* fp = fopen(path, "r");
	if (fp == nullptr) {
		printf("SHADER ERROR: Unable to open \"%s\"\n", path);
		return;
	}
	size_t bytes;
	do {
		if (section_count > 1) {
			pointers[section_count] = new char[2048];
		}
		bytes = fread(pointers[section_count], 1, 2047, fp);
		section_count++;
	} while(bytes == 2047);
	fclose(fp);

	glShaderSource(shaderID, (GLsizei)section_count, pointers, NULL);
	glCompileShader(shaderID);

	for (size_t i = 1; i < section_count; i++)
		delete[] pointers[i];

	GLint result = GL_FALSE;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	printErrorMessage(shaderID);


}



Shader Shader::LoadCompute(std::string path) {
	return LoadCompute(path.c_str());
}

Shader Shader::LoadCompute(const char* path) {
	GLuint shaderID = glCreateShader(GL_COMPUTE_SHADER);
	LoadSingleShader(shaderID, path);


	GLuint programID = glCreateProgram();

	glAttachShader(programID, shaderID);
	glLinkProgram(programID);
	glDetachShader(programID, shaderID);
	glDeleteShader(shaderID);

	return Shader(programID);
}


Shader Shader::Load(std::string vertex_file_path, std::string frag_file_path) {
	return Load(vertex_file_path.c_str(), frag_file_path.c_str());
}

Shader Shader::Load(const char* vertex_file_path, const char* fragment_file_path)
{
	// Create the shaders
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	LoadSingleShader(vertexShaderId, vertex_file_path);
	LoadSingleShader(fragmentShaderId, fragment_file_path);

	// Link the program
	printf("Linking program\n");
	GLuint programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);

	// Check the program
	GLint result = GL_FALSE;
	glGetProgramiv(programId, GL_LINK_STATUS, &result);
	printErrorMessage(programId);

	glDetachShader(programId, vertexShaderId);
	glDetachShader(programId, fragmentShaderId);

	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);

	return Shader(programId);
}