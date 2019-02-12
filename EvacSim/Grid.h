#pragma once

#include "GridCell.h"
//#include <glad\glad.h>
//#include <GLM\glm.hpp>
//#include <vector>
#include <string>

class Building;
class Grid {
private:
	using loadPersonFunc = void(Building::*)(GLfloat, GLfloat);
	static loadPersonFunc loadPersonAction;

	static Building *b;

	std::vector<std::vector<GridCell*>> cells;
	GLfloat cellSize;
	int numCols;
	int numRows;
	glm::vec2 centre;
	int floorNum;
	int mapLocation;//position of the floor on the building map (north-east, north etc.)

	glm::vec2 calcCentre(int numCols, int numRows, int floorPosition);

public:
	static void setLoadPersonAction(loadPersonFunc action, Building *b);

	Grid(GLfloat cellSize, char *bytes, int &i, int floorNum, int numCols, int numRows, bool runningSimulation, int mapLocation);
	Grid(GLfloat cellSize, int c, int r, int floorNum, int mapLocation);
	~Grid();

	void createGeometry(GLuint program);

	void draw(GLuint program, glm::mat4 viewMat);

	void calcNeighbours();

	void update();

	GridCell * getCell(int col, int row);

	void determineCell(GLfloat x, GLfloat y, int &col, int &row, int &floorNum);

	void toByteArray(char *bytes, int &i);

	std::vector<std::list<GridCell *>> getStairPaths(GridCell * cell);

	inline int byteArraySize() {
		return sizeof(floorNum) + sizeof(numCols) + sizeof(numRows) + numCols * numRows + sizeof(mapLocation);
	}


	inline int getNumCols() {
		return numCols;
	}

	inline int getNumRows() {
		return numRows;
	}

	inline int getFloorNum() {
		return floorNum;
	}

	inline int getMapPosition() {
		return mapLocation;
	}
};