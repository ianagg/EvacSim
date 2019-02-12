#pragma once

#include <glad\glad.h>
#include <GLM\glm.hpp>

class Square {
private:
	GLuint VAO;
	GLuint VBO[2];
	GLfloat x;
	GLfloat y;
	GLfloat size;

protected:
	GLfloat *colours;
	int coloursSize = 18;

public:
	Square(GLfloat x, GLfloat y, GLfloat size);
	virtual ~Square() = 0;

	void createGeometry(GLuint program);

	void draw(GLuint program, glm::mat4 viewMat);

	inline glm::vec2 getCentre() {
		return glm::vec2(x, y);
	}

	inline GLfloat getSize() {
		return size;
	}
};

class Obstacle : public Square {
public:
	Obstacle(GLfloat x, GLfloat y, GLfloat size);
	~Obstacle() {};
};

class Stairs : public Square {
public:
	Stairs(GLfloat x, GLfloat y, GLfloat size);
	~Stairs() {};
};

class TmpStairs : public Square {
public:
	TmpStairs(GLfloat x, GLfloat y, GLfloat size);
	~TmpStairs() {};
};

class Fire : public Square {
public:
	Fire(GLfloat x, GLfloat y, GLfloat size);
	~Fire() {};
};

class StartPoint : public Square {
public:
	StartPoint(GLfloat x, GLfloat y, GLfloat size);
	~StartPoint() {};
};

class Exit : public Square {
public:
	Exit(GLfloat x, GLfloat y, GLfloat size);
	~Exit() {};
};