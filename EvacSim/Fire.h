#pragma once
#include <glad\glad.h>
#include <GLM\glm.hpp>


class Fire {
private:
	GLuint VAO;
	GLuint VBO[2];
	GLfloat x;
	GLfloat y;
	GLfloat size;

public:
	Fire(GLfloat x, GLfloat y, GLfloat size);
	~Fire();

	void createGeometry(GLuint program);
	void draw(GLuint program, glm::mat4 viewMat);

	inline glm::vec2 getCentre() {
		return glm::vec2(x, y);
	}

	inline GLfloat getSize() {
		return size;
	}

};