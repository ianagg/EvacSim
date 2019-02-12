#pragma once
#include "Grid.h"
#include "Person.h"

class Building {
private:
	static const GLfloat cellSize;
	
	int everyone;
	int deadone;
	bool endSim = false;


	GLint shaderProgram;

	std::map<int, Grid *> grids;
	std::vector<Person *> people;

	bool placingStairs = false;
	GridCell *tmpStairs = nullptr;
	std::list<std::pair<GridCell *, GridCell *>> stairPairs;

	GridCell *endPoint = nullptr;

	bool loadMap(bool runningSimulation, std::string fName);

	GridCell * loadStairData(char *bytes, int &i);

	bool loadResult;

public:
	Building(GLint shaderProgram, int numCols, int numRows);
	Building(GLint shaderProgram, bool runningSimulation, std::string fName);
	~Building();

	void saveMap(std::string fName);

	void addFloor(int floorNum, int floorPosition);

	void getValidMapPositions(std::vector<int> &vp);

	void getValidFloorNumbers(std::vector<int> &vf);

	void loadPersonCallback(GLfloat x, GLfloat y);

	bool determineCell(GLfloat x, GLfloat y, int &c, int &r, int &f);

	bool cellOnFire(GLfloat x, GLfloat y);

	void moveToPoint(GLfloat pointX, GLfloat pointY);

	void moveToExit();

	void placeObstacle(GLfloat cursorX, GLfloat cursorY);

	void placeStairs(GLfloat cursorX, GLfloat cursorY);

	void placePersonStart(GLfloat cursorX, GLfloat cursorY);

	void placeFire(GLfloat cursorX, GLfloat cursorY);

	void placeExit(GLfloat cursorX, GLfloat cursorY);

	void createGeometry();

	void draw(GLfloat interpolation, bool drawPeople);

	void updateScene();

	inline void resetStairPlacement() {
		placingStairs = false;
		tmpStairs = nullptr;
	}

	inline void setExitCell(GridCell *ex) {
		this->endPoint = ex;
	}

	bool getLoadResult() {
		return loadResult;
	}
	inline  int getEveryone() {
		return everyone;
	}
	inline  int getDeadone() {
		return deadone;
	}

	inline bool getEnd() {
		return endSim;
	}

	inline void setEnd(bool e) {
		endSim = e;
	}

};