#pragma once

#include <glad\glad.h>
#include <GLM\glm.hpp>

class Exit {
private:
	GLuint VAO;
	GLuint VBO[2];
	GLfloat x;
	GLfloat y;
	GLfloat size;

public:
	Exit(GLfloat x, GLfloat y, GLfloat size);
	~Exit();

	void createGeometry(GLuint program);

	void draw(GLuint program, glm::mat4 viewMat);

	inline glm::vec2 getCentre() {
		return glm::vec2(x, y);
	}

	inline GLfloat getSize() {
		return size;
	}
};