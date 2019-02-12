#include "Camera.h"

Camera::Camera() {

}

Camera::~Camera() {

}

bool Camera::update() {
	bool modified = false;
	if (heldU && cameraY < 10) {
		cameraY += cameraInc;
		modified = true;
	}
	if (heldD && cameraY > -10) {
		cameraY -= cameraInc;
		modified = true;
	}
	if (heldL && cameraX > -10) {
		cameraX -= cameraInc;
		modified = true;
	}
	if (heldR && cameraX < 10) {
		cameraX += cameraInc;
		modified = true;
	}

	if (zoomIn) {
		zoom += zoomInc;
		modified = true;
		zoomIn = false;
	}
	else if (zoomOut) {
		zoom -= zoomInc;
		modified = true;
		zoomOut = false;
	}

	return modified;
}

void Camera::changeZoom(double xoffset, double yoffset) {
	if (yoffset < oldYOffset && (zoom + zoomInc) <= 4.0f) {
		zoomIn = true;
		zoomOut = false;
	}
	else if (yoffset > oldYOffset && (zoom - zoomInc) >= 0.25f) {
		zoomOut = true;
		zoomIn = false;
	}
}

void Camera::reset() {
	heldU = false;
	heldD = false;
	heldL = false;
	heldR = false;
	zoomIn = false;
	zoomOut = false;
	oldYOffset = 0.0f;

	cameraX = 0.0f;
	cameraY = 0.0f;
	zoom = 1.0f;
}