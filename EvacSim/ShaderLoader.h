#pragma once

#include <glad\glad.h>

class ShaderLoader {
private:
	GLuint shaderProg;
public:
	ShaderLoader() {};
	ShaderLoader(const GLchar *vertPath, const GLchar *fragPath);
	//~ShaderLoader();

	GLuint LoadShader(const GLchar *shaderCode);

	inline GLuint getProgram() {
		return shaderProg;
	}
};