#pragma once
#include <vector>
#include <list>
#include "Square.h"

class Person;

class GridCell {
public:
	enum class CellType { Walkable, Obstacle, TmpStairs, Stairs, Fire, StartPoint, Exit };

private:
	static GLfloat speed;


	glm::vec2 centre;
	const GLfloat lineWidth = 0.005f;
	GLfloat size;
	GLuint VAO;
	GLuint VBO[2];
	GLuint prog;

	//a* stuff
	int col;
	int row;
	int floorNum;
	GridCell *parent;
	double fCost;
	double gCost;
	std::vector<GridCell *> neighbours;

	int neighbourFireCount;
	DWORD lastPause;

	GridCell *target;
	std::vector<std::list<GridCell *>> otherStairPaths;//need path cost too

	Square *cellColour;
	CellType cellType;

	std::list<Person *> queue;

public:

	GridCell(GLfloat x, GLfloat y, GLfloat size, int col, int row, int floorNum);
	~GridCell();

	void createGeometry(GLuint program);

	void draw(GLuint program, glm::mat4 viewMat);

	void printCell();

	void printNeighbours();

	bool pointInCell(GLfloat x, GLfloat y);

	void toggleObstacle();

	void toggleFire(bool allowExtinguish);

	void update();

	bool setStairs(GridCell *target);

	void clearStairs();

	void toggleStartPoint();

	void toggleExit();

	void writeStairData(char *bytes, int &i);

	inline glm::vec2 getCentre() {
		return centre;
	}

	inline void setNeighbours(std::vector<GridCell *> neighbours) {
		this->neighbours = neighbours;
	}

	inline std::vector<GridCell*> getNeighbours() {
		return neighbours;
	}

	inline int getCol() {
		return col;
	}

	inline int getRow() {
		return row;
	}

	inline int getFloor() {
		return floorNum;
	}

	inline double getFCost() {
		return fCost;
	}

	inline double getGCost() {
		return gCost;
	}

	inline void setFCost(double fCost) {
		this->fCost = fCost;
	}

	inline void setGCost(double gCost) {
		this->gCost = gCost;
	}

	inline bool isWalkable() {
		return cellType == CellType::Walkable;
	}

	inline bool isObstacle() {
		return cellType == CellType::Obstacle;
	}

	inline bool isFire() {
		return cellType == CellType::Fire;
	}

	inline bool isStairs() {
		return cellType == CellType::Stairs;
	}

	inline bool isStartPoint() {
		return cellType == CellType::StartPoint;
	}

	inline bool isExit() {
		return cellType == CellType::Exit;
	}

	inline GridCell * getStairTarget() {
		return target;
	}

	inline void setParent(GridCell *parent) {
		this->parent = parent;
	}

	inline GridCell * getParent() {
		return parent;
	}

	inline void setStairPaths(std::vector<std::list<GridCell *>> otherStairPaths) {
		this->otherStairPaths = otherStairPaths;
	}

	inline std::vector<std::list<GridCell *>> getStairPaths() {
		return otherStairPaths;
	}

	inline static void setSpeed(GLfloat s) {
		speed = s;
	}
	inline static GLfloat getSpeed() {
		return speed;
	}

	inline void addToQueue(Person *p) {
		if (std::find(queue.begin(), queue.end(), p) == queue.end()) {
			queue.push_back(p);
		}
	}

	inline void removeFromQueue(Person *p) {
		if (std::find(queue.begin(), queue.end(), p) != queue.end()) {
			queue.remove(p);
		}
	}

	inline bool queueFrontCheck(Person *p) {
		return !queue.empty() && queue.front() == p;
	}
};