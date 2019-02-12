#include "ShaderLoader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

ShaderLoader::ShaderLoader(const GLchar *vertPath, const GLchar *fragPath) {
	std::string vertStrCode;
	std::string fragStrCode;
	std::ifstream vertFile;
	std::ifstream fragFile;

	vertFile.exceptions(std::ifstream::badbit);
	fragFile.exceptions(std::ifstream::badbit);

	try {
		vertFile.open(vertPath);
		fragFile.open(fragPath);

		std::stringstream vertStream;
		std::stringstream fragStream;

		vertStream << vertFile.rdbuf();
		fragStream << fragFile.rdbuf();

		vertFile.close();
		fragFile.close();

		vertStrCode = vertStream.str();
		fragStrCode = fragStream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "failure to load shader code" << std::endl;
	}

	
	//sort out error stuff
	//compile shaders

	GLuint vertShader, fragShader;
	const GLchar *vertCode = vertStrCode.c_str();
	const GLchar *fragCode = fragStrCode.c_str();
	const GLint vertLength = vertStrCode.length();
	const GLint fragLength = fragStrCode.length();
	GLint success;
	GLchar log[512];

	vertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertShader, 1, &vertCode, &vertLength);
	glCompileShader(vertShader);
	glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertShader, 512, NULL, log);
		std::cout << "failed to compile vertex shader\n" << log << std::endl;
	}

	fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShader, 1, &fragCode, &fragLength);
	glCompileShader(fragShader);
	glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragShader, 512, NULL, log);
		std::cout << "failed to compile fragment shader\n" << log << std::endl;
	}

	this->shaderProg = glCreateProgram();
	glAttachShader(this->shaderProg, vertShader);
	glAttachShader(this->shaderProg, fragShader);
	glLinkProgram(this->shaderProg);
	glGetProgramiv(this->shaderProg, GL_LINK_STATUS, &success);
	if(!success) {
		glGetProgramInfoLog(this->shaderProg, 512, NULL, log);
		std::cout << "failed when linking shaders\n" << log << std::endl;
	}

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);
}

GLuint ShaderLoader::LoadShader(const GLchar *shaderCode) {
	return 1;
}