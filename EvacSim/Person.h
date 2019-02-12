#pragma once

#include "Grid.h"
#include "GridCell.h"
#include <list>
#include <iostream>
#include <map>

class Building;

class Person {
private:
	Building *building;

	static GLfloat speed;
	static int MAX_HEALTH;
	static int YELLOW_HEALTH;
	static int RED_HEALTH;

	GLuint VAO;
	GLuint VBO[2];
	const double PI = 3.1415926535897932384626433832795;
	int everyNdeg = 2;
	GLfloat x;
	GLfloat y;
	GLfloat rad;

	//int id;
	int currentHealth;

	GridCell *currentCell;

	GLfloat oldMoveX;
	GLfloat oldMoveY;
	GLfloat moveX;
	GLfloat moveY;

	std::list<GridCell*> path;

	bool skipInterpolation;
	bool finishedStep;
	bool reachedTarget;

	void changeColour();
	GLuint program;

public:
	Person(GLfloat startX, GLfloat startY, GLfloat size, GLfloat speed, Building *building);
	~Person();

	void createGeometry(GLuint program);

	void moveTo(GLfloat targetX, GLfloat targetY);

	void update();

	void draw(GLuint program, GLfloat interpolation, glm::mat4 viewMat);

	inline GLfloat getX() {
		return x;
	}

	inline GLfloat getY() {
		return y;
	}

	inline static void setSpeed(GLfloat s) {
		speed = s;
	}

	inline static GLfloat getSpeed() {
		return speed;
	}


	inline GLfloat getRad() {
		return rad;
	}

	inline std::list<GridCell *> getPath() {
		return path;
	}


	inline void setPath(std::list<GridCell*> path) {
		if (this->path.empty()) {
			this->path = path;
		}
	}

	inline bool isDead() {
		return currentHealth == 0;
	}

	inline bool hasReachedTarget() {
		return reachedTarget;
	}
};