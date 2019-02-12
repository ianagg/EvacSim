#include "Person.h"

#include <math.h>
//#include <vector>
#include <iostream>
#include <math.h>

#include <GLM\glm.hpp>
#include <GLM\gtc\matrix_transform.hpp>
#include <GLM\gtc\type_ptr.hpp>
#include "Building.h"

GLfloat Person::speed = 0.005f;
int Person::MAX_HEALTH = 100;
int Person::YELLOW_HEALTH = 80;
int Person::RED_HEALTH = 25;

Person::Person(GLfloat startX, GLfloat startY, GLfloat size, GLfloat speed, Building *building) {
	x = startX;
	y = startY;
	rad = size;
	this->speed = 0.005f;
	skipInterpolation = true;
	finishedStep = false;
	reachedTarget = false;


	currentCell = nullptr;
	this->building = building;
	currentHealth = MAX_HEALTH;
}

Person::~Person() {
	glDeleteBuffers(2, VBO);
	glDeleteVertexArrays(1, &VAO);
}

void Person::createGeometry(GLuint program) {
	this->program = program;
	std::vector<GLfloat> v;
	std::vector<GLfloat> col;

	for (float i = 0.0f; i < 360.0f; i += everyNdeg) {
		GLfloat x1 = rad * cos(i * (PI / 180));
		GLfloat y1 = rad * sin(i * (PI / 180));

		GLfloat x2 = rad * cos((i + everyNdeg) * (PI / 180));
		GLfloat y2 = rad * sin((i + everyNdeg) * (PI / 180));

		v.push_back(x1);
		v.push_back(y1);
		v.push_back(0.0f);
		col.push_back(0.0f);
		col.push_back(1.0f);
		col.push_back(0.0f);

		v.push_back(x2);
		v.push_back(y2);
		v.push_back(0.0f);
		col.push_back(0.0f);
		col.push_back(1.0f);
		col.push_back(0.0f);

		//3rd point of triangle is centre of circle
		v.push_back(0.0f);
		v.push_back(0.0f);
		v.push_back(0.0f);
		col.push_back(0.0f);
		col.push_back(1.0f);
		col.push_back(0.0f);
	}

	GLfloat *verts = v.data();
	GLfloat *cols = col.data();

	glGenVertexArrays(1, &VAO);
	glGenBuffers(2, VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(GL_FLOAT), verts, GL_STATIC_DRAW);
	GLint vertexLocation = glGetAttribLocation(program, "in_Position");
	glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*) 0 );
	glEnableVertexAttribArray(vertexLocation);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, col.size() * sizeof(GL_FLOAT), cols, GL_STATIC_DRAW);
	GLint colorLocation = glGetAttribLocation(program, "in_Color");
	glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(colorLocation);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Person::changeColour() {
	std::vector<GLfloat> col;

	for (float i = 0.0f; i < 360.0f; i += everyNdeg) {
		GLfloat x1 = rad * cos(i * (PI / 180));
		GLfloat y1 = rad * sin(i * (PI / 180));

		GLfloat x2 = rad * cos((i + everyNdeg) * (PI / 180));
		GLfloat y2 = rad * sin((i + everyNdeg) * (PI / 180));

		if (currentHealth <= YELLOW_HEALTH && currentHealth > RED_HEALTH) {
			col.push_back(1.0f);
			col.push_back(1.0f);
			col.push_back(0.0f);
			col.push_back(1.0f);
			col.push_back(1.0f);
			col.push_back(0.0f);
			col.push_back(1.0f);
			col.push_back(1.0f);
			col.push_back(0.0f);

		}
		else if (currentHealth <= RED_HEALTH) {
			col.push_back(0.0f);
			col.push_back(0.0f);
			col.push_back(0.0f);
			col.push_back(0.0f);
			col.push_back(0.0f);
			col.push_back(0.0f);
			col.push_back(1.0f);
			col.push_back(0.0f);
			col.push_back(0.0f);
		}
	}

	GLfloat *cols = col.data();

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, col.size() * sizeof(GL_FLOAT), cols, GL_STATIC_DRAW);
	GLint colorLocation = glGetAttribLocation(program, "in_Color");
	glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(colorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Person::moveTo(GLfloat targetX, GLfloat targetY) {
	moveX = targetX - x;
	moveY = targetY - y;
	float length = sqrt(moveX * moveX + moveY * moveY);
	if (length == 0.0f) {
		moveX = 0.0f;
		moveY = 0.0f;
	}
	else {
		moveX /= length;
		moveY /= length;
	}

	GLfloat tmpX, tmpY;
	if (length < speed) {
		skipInterpolation = true;
		tmpX = targetX;
		tmpY = targetY;
	}
	else {
		tmpX = x + speed * moveX;
		tmpY = y + speed * moveY;
	}

	x = tmpX;
	y = tmpY;

	if (x == targetX && y == targetY) {
		finishedStep = true;
	}
}

void Person::update() {
	//check to see if current cell is fire outside movement part
	int oldHealth = currentHealth;
	if (building->cellOnFire(x, y) && currentHealth > 0) {
		currentHealth -= 1;
	}

	if ((currentHealth <= YELLOW_HEALTH && oldHealth > YELLOW_HEALTH) || (currentHealth <= RED_HEALTH && oldHealth > RED_HEALTH)) {
		changeColour();
	}

	if (!path.empty() && currentHealth > 0) {
		GridCell *next = path.front();
		next->addToQueue(this);
		if (next->queueFrontCheck(this)) {
			if (currentCell != nullptr && currentCell->queueFrontCheck(this)) {
				currentCell->removeFromQueue(this);
			}

			moveTo(next->getCentre().x, next->getCentre().y);
		}
		else {

		}

		if (finishedStep) {
			currentCell = next;
			path.pop_front();

			if (next->isStairs() && !path.empty() && next->getStairTarget() == path.front()) {
				x = path.front()->getCentre().x;
				y = path.front()->getCentre().y;
			}

			finishedStep = false;

			if (path.empty()) {
				currentCell->removeFromQueue(this);
				reachedTarget = true;
			}
		}
	}
}

void Person::draw(GLuint program, GLfloat interpolation, glm::mat4 viewMat) {
	GLfloat viewX;
	GLfloat viewY;

	if (skipInterpolation) {
		//dont add interpolation if very close to target cell
		viewX = x;
		viewY = y;
	}
	else {
		//interpolate
		viewX = x + moveX * speed * interpolation;
		viewY = y + moveY * speed * interpolation;
	}

	//translate stuff here
	glUseProgram(program);
	//glm::mat4 mvm = glm::mat4(1.0f);
	//mvm = glm::translate(mvm, glm::vec3(viewX, viewY, 0.0f));
	glm::mat4 mvm = glm::translate(viewMat, glm::vec3(viewX, viewY, 0.0f));

	GLint mvmLoc = glGetUniformLocation(program, "ModelViewMatrix");
	glUniformMatrix4fv(mvmLoc, 1, GL_FALSE, glm::value_ptr(mvm));

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, (360 / everyNdeg) * 3);
	glBindVertexArray(0);
	glUseProgram(0);
}