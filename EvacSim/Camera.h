#pragma once

#include <glad\glad.h>
#include <GLM\glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

class Camera {
private:
	GLfloat zoom = 1.0f;
	GLfloat zoomInc = 0.2f;
	double oldYOffset = 0.0;
	bool zoomOut;
	bool zoomIn;

	GLfloat cameraX = 0.0f;
	GLfloat cameraY = 0.0f;
	GLfloat cameraInc = 0.025f;
	bool heldU = false;
	bool heldD = false;
	bool heldL = false;
	bool heldR = false;
	glm::vec3 pos;

public:
	Camera();
	~Camera();

	bool update();

	void changeZoom(double xoffset, double yoffset);

	void reset();

	inline glm::mat4 getViewMat() {
		return glm::mat4(1.0f);
		//return glm::lookAt(pos, lookAtPoint, glm::vec3(0, 1, 0));
	}

	inline glm::vec3 getPosition() {
		return pos;
	}

	inline GLfloat getX() {
		return cameraX;
	}

	inline GLfloat getY() {
		return cameraY;
	}

	inline GLfloat getZoom() {
		return zoom;
	}

	inline void setHeldU(bool pressed) {
		heldU = pressed;
	}

	inline void setHeldD(bool pressed) {
		heldD = pressed;
	}

	inline void setHeldL(bool pressed) {
		heldL = pressed;
	}

	inline void setHeldR(bool pressed) {
		heldR = pressed;
	}
};