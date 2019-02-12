#include "Fire.h"
#include <GLM\glm.hpp>
#include <GLM\gtc\matrix_transform.hpp>
#include <GLM\gtc\type_ptr.hpp>

Fire::Fire(GLfloat x, GLfloat y, GLfloat size) {
	this->x = x;
	this->y = y;
	this->size = size;

}

Fire::~Fire()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(2, VBO);
}



void Fire::createGeometry(GLuint program) {
	GLfloat verts[] = {
		-size, size, 0.0f,
		-size, -size, 0.0f,
		size, -size, 0.0f,
		size, size, 0.0f,
		-size, size, 0.0f,
		size, -size, 0.0f
	};

	GLfloat cols[] = {
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(2, VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	GLint vertexLocation = glGetAttribLocation(program, "in_Position");
	glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(vertexLocation);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cols), cols, GL_STATIC_DRAW);
	GLint colorLocation = glGetAttribLocation(program, "in_Color");
	glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(colorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void Fire::draw(GLuint program, glm::mat4 viewMat) {
	glUseProgram(program);
	//translate stuff here
	//glm::mat4 mvm = glm::mat4(1.0f);
	//mvm = glm::translate(mvm, glm::vec3(x, y, 0.0f));
	glm::mat4 mvm = glm::translate(viewMat, glm::vec3(x, y, 0.0f));

	GLint mvmLoc = glGetUniformLocation(program, "ModelViewMatrix");
	glUniformMatrix4fv(mvmLoc, 1, GL_FALSE, glm::value_ptr(mvm));

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glUseProgram(0);
}