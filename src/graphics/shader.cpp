#include <fstream>
#include <iostream>

#include <graphics/shader.h>
#include <windows.h>

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


void printErrorMessage(GLuint id) {
	GLsizei infoLogLength;
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0)
	{
		char* errorMessage = new char[(size_t)infoLogLength + 1];
		glGetShaderInfoLog(id, infoLogLength, NULL, errorMessage);
		std::cout << &errorMessage[0] << '\n';
		delete[] errorMessage;
	}
}

Shader Shader::Load(std::string& vertex_file_path, std::string& fragment_file_path)
{
	// Create the shaders
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string vertexShaderCode;
	std::ifstream vertexShaderStream(vertex_file_path, std::ios::in);
	if (vertexShaderStream.is_open())
	{
		std::stringstream sstr;
		sstr << vertexShaderStream.rdbuf();
		vertexShaderCode = sstr.str();
		vertexShaderStream.close();
	}
	else
	{
		std::cout << "Unable to open " << vertex_file_path << '\n';
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string fragmentShaderCode;
	std::ifstream fragmentShaderStream(fragment_file_path, std::ios::in);
	if (fragmentShaderStream.is_open())
	{
		std::stringstream sstr;
		sstr << fragmentShaderStream.rdbuf();
		fragmentShaderCode = sstr.str();
		fragmentShaderStream.close();
	}
	else {
		std::cout << "Unable to open " << fragment_file_path << '\n';
		return 0;
	}

	GLint result = GL_FALSE;
	GLsizei infoLogLength;

	// Compile Vertex Shader
	char const* vertexSourcePointer = vertexShaderCode.c_str();
	glShaderSource(vertexShaderId, 1, &vertexSourcePointer, NULL);
	glCompileShader(vertexShaderId);

	// Check Vertex Shader
	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &result);
	printErrorMessage(vertexShaderId);

	// Compile Fragment Shader
	char const* fragmentSourcePointer = fragmentShaderCode.c_str();
	glShaderSource(fragmentShaderId, 1, &fragmentSourcePointer, NULL);
	glCompileShader(fragmentShaderId);

	// Check Fragment Shader
	glGetShaderiv(fragmentShaderId, GL_COMPILE_STATUS, &result);
	printErrorMessage(fragmentShaderId);

	// Link the program
	printf("Linking program\n");
	GLuint programId = glCreateProgram();
	glAttachShader(programId, vertexShaderId);
	glAttachShader(programId, fragmentShaderId);
	glLinkProgram(programId);

	// Check the program
	glGetProgramiv(programId, GL_LINK_STATUS, &result);
	printErrorMessage(programId);

	glDetachShader(programId, vertexShaderId);
	glDetachShader(programId, fragmentShaderId);

	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);

	return Shader(programId);
}