#include "Grid.h"
#include <iostream>
#include <GLM\gtc\matrix_transform.hpp>
#include <GLM\gtc\type_ptr.hpp>
#include <fstream>
#include "FileUtils.h"
#include "AStar.h"
#include "Building.h"

Grid::loadPersonFunc Grid::loadPersonAction;
Building * Grid::b;

void Grid::setLoadPersonAction(loadPersonFunc action, Building *b) {
	loadPersonAction = action;
	Grid::b = b;
}

Grid::Grid(GLfloat cellSize, char *bytes, int &i, int floorNum, int numCols, int numRows, bool runningSimulation, int mapLocation) {
	this->cellSize = cellSize;
	this->centre = calcCentre(numCols, numRows, mapLocation);
	this->floorNum = floorNum;
	this->numCols = numCols;
	this->numRows = numRows;
	this->mapLocation = mapLocation;

	GLfloat startX = ((float)numCols * cellSize) / 2.0f;
	startX *= -1.0f;
	//startX += gridX;
	startX += centre.x;
	startX += cellSize / 2.0f;
	GLfloat x = startX;

	GLfloat startY = ((float)numRows * cellSize) / 2.0f;
	startY *= -1.0f;
	//startY += gridY;
	startY += centre.y;
	startY += cellSize / 2.0f;

	GLfloat y = startY;

	int total = 0;
	int colNum = 0;
	int rowNum = 0;

	while (colNum < numCols) {
		std::vector<GridCell*> col;
		rowNum = 0;

		while (rowNum < numRows) {
			GridCell *gc = new GridCell(x, y, cellSize / 2.0f, colNum, rowNum, floorNum);

			if (bytes[i] == 1) {
				gc->toggleObstacle();
			}
			else if (bytes[i] == 'f') {
				gc->toggleFire(false);
			}
			else if (bytes[i] == 't') {

			}
			else if (bytes[i] == 'p') {
				if (runningSimulation) {
					if (loadPersonAction) {
						//b->*loadPersonAction(x, y);
						b->loadPersonCallback(x, y);
					}
				}
				else {
					gc->toggleStartPoint();
				}
			}
			else if (bytes[i] == 'e') {
				gc->toggleExit();

				//yuck
				b->setExitCell(gc);
				//b->placeExit(gc->getCentre().x, gc->getCentre().y);
			}

			col.push_back(gc);
			y += cellSize;
			total++;
			rowNum++;
			i++;
		}

		cells.push_back(col);

		y = startY;
		x += cellSize;
		colNum++;
	}
}

Grid::Grid(GLfloat cellSize, int c, int r, int floorNum, int mapLocation) {
	this->cellSize = cellSize;
	this->centre = calcCentre(c, r, mapLocation);
	this->floorNum = floorNum;
	this->mapLocation = mapLocation;
	//create grid with c columns and r rows centred on 0,0
	GLfloat startX = ((float)c * cellSize) / 2.0f;
	startX *= -1.0f;
	//startX += gridX;
	startX += centre.x;
	startX += cellSize / 2.0f;
	//startX += gridX;

	GLfloat x = startX;

	GLfloat startY = ((float)r * cellSize) / 2.0f;
	startY *= -1.0f;
	//startY += gridY;
	startY += centre.y;
	startY += cellSize / 2.0f;
	//startY += gridY;

	GLfloat y = startY;

	int total = 0;
	int colNum = 0;
	int rowNum = 0;

	while (colNum < c) {
		std::vector<GridCell*> col;
		rowNum = 0;

		while (rowNum < r) {
			GridCell *gc = new GridCell(x, y, cellSize / 2.0f, colNum, rowNum, floorNum);

			col.push_back(gc);
			y += cellSize;
			total++;
			rowNum++;
		}

		cells.push_back(col);

		y = startY;
		x += cellSize;
		colNum++;
	}

	this->numCols = colNum;
	this->numRows = rowNum;
}

Grid::~Grid() {
	for (std::vector<GridCell*> col : cells) {
		for (GridCell *cell : col) {
			delete cell;
		}
	}
}

glm::vec2 Grid::calcCentre(int numCols, int numRows, int floorPosition) {
	GLfloat gridX, gridY;
	switch (floorPosition) {
		case 0: {//centre
			gridX = 0.0f;
			gridY = 0.0f;
			break;
		}
		case 1: {//above centre
			gridX = 0.0f;
			gridY = (numRows * cellSize / 2.0f) + (numRows * cellSize / 2.0f) + cellSize;
			break;
		}

		case 2: {//upper right corner
			gridX = (numCols * cellSize / 2.0f) + (numCols * cellSize / 2.0f) + cellSize;
			gridY = (numRows * cellSize / 2.0f) + (numRows * cellSize / 2.0f) + cellSize;
			break;
		}

		case 3: {//right of centre
			gridX = (numCols * cellSize / 2.0f) + (numCols * cellSize / 2.0f) + cellSize;
			gridY = 0.0f;
			break;
		}

		case 4: {//lower right corner
			gridX = (numCols * cellSize / 2.0f) + (numCols * cellSize / 2.0f) + cellSize;
			gridY = (numRows * cellSize / 2.0f) + (numRows * cellSize / 2.0f) + cellSize;
			gridY *= -1.0f;
			break;
		}

		case 5: {//below centre
			gridX = 0.0f;
			gridY = (numRows * cellSize / 2.0f) + (numRows * cellSize / 2.0f) + cellSize;
			gridY *= -1.0f;
			break;
		}

		case 6: {//lower left corner
			gridX = (numCols * cellSize / 2.0f) + (numCols * cellSize / 2.0f) + cellSize;
			gridX *= -1.0f;
			gridY = (numRows * cellSize / 2.0f) + (numRows * cellSize / 2.0f) + cellSize;
			gridY *= -1.0f;
			break;
		}

		case 7: {//left of centre
			gridX = (numCols * cellSize / 2.0f) + (numCols * cellSize / 2.0f) + cellSize;
			gridX *= -1.0f;
			gridY = 0.0f;
			break;
		}

		case 8: {//upper left corner
			gridX = (numCols * cellSize / 2.0f) + (numCols * cellSize / 2.0f) + cellSize;
			gridX *= -1.0f;
			gridY = (numRows * cellSize / 2.0f) + (numRows * cellSize / 2.0f) + cellSize;
			break;
		}
	}

	return glm::vec2(gridX, gridY);
}

void Grid::createGeometry(GLuint program) {
	for (std::vector<GridCell*> col : cells) {
		for (GridCell *cell : col) {
			cell->createGeometry(program);
		}
	}
}

void Grid::draw(GLuint program, glm::mat4 viewMat) {
	for (std::vector<GridCell*> col : cells) {
		for (GridCell *cell : col) {
			cell->draw(program, viewMat);
		}
	}
}

void Grid::calcNeighbours() {
	for (std::vector<GridCell*> col : cells) {
		for (GridCell *cell : col) {
			int c = cell->getCol();
			int r = cell->getRow();
			std::vector<GridCell*> neighbours;

			if (c - 1 >= 0) {
				neighbours.push_back(cells.at(c - 1).at(r));
			}

			if (r - 1 >= 0) {
				neighbours.push_back(cells.at(c).at(r - 1));
			}

			if (c + 1 < numCols) {
				neighbours.push_back(cells.at(c + 1).at(r));
			}

			if (r + 1 < numRows) {
				neighbours.push_back(cells.at(c).at(r + 1));
			}

			if ((c - 1 >= 0) && (r - 1 >= 0)) {
				neighbours.push_back(cells.at(c - 1).at(r - 1));
			}

			if ((c + 1 < numCols) && (r + 1 < numRows)) {
				neighbours.push_back(cells.at(c + 1).at(r + 1));
			}

			if ((c + 1 < numCols) && (r - 1 >= 0)) {
				neighbours.push_back(cells.at(c + 1).at(r - 1));
			}

			if ((c - 1 >= 0) && (r + 1 < numRows)) {
				neighbours.push_back(cells.at(c - 1).at(r + 1));
			}

			cell->setNeighbours(neighbours);
		}
	}
}

void Grid::update() {
	for (std::vector<GridCell*> col : cells) {
		for (GridCell *cell : col) {
			cell->update();
		}
	}
}

GridCell * Grid::getCell(int col, int row) {
	return cells.at(col).at(row);
}

void Grid::determineCell(GLfloat x, GLfloat y, int &col, int &row, int &floorNum) {
	col = static_cast<int>(floor(((x - centre.x) / cellSize) + numCols / 2.0f));
	row = static_cast<int>(floor(((y - centre.y) / cellSize) + numRows / 2.0f));

	if (col < 0 || col > (numCols - 1) || row < 0 || row > (numRows - 1)) {
		col = -1;
		row = -1;
		floorNum = -1;
	}
	else {
		floorNum = this->floorNum;
	}
}

void Grid::toByteArray(char *bytes, int &i) {
	//char *bytes = new char[sizeof(numCols) + sizeof(numRows) + numCols * numRows];

	for (int j = 0; j < 4; j++) {
		bytes[i + 3 - j] = (floorNum >> (j * 8));
		bytes[i + 7 - j] = (numCols >> (j * 8));
		bytes[i + 11 - j] = (numRows >> (j * 8));
		bytes[i + 15 - j] = (mapLocation >> (j * 8));
	}

	i += 16;
	for (std::vector<GridCell*> col : cells) {
		for (GridCell *cell : col) {
			if (cell->isObstacle()) {
				bytes[i] = 1;
			}
			else if (cell->isFire()) {
				bytes[i] = 'f';
			}
			else if (cell->isStartPoint()) {
				bytes[i] = 'p';
			}
			else if (cell->isExit()) {
				bytes[i] = 'e';
			}
			else {
				bytes[i] = 0;
			}

			i++;
		}
	}
}

std::vector<std::list<GridCell *>> Grid::getStairPaths(GridCell * cell) {
	std::vector<std::list<GridCell *>> paths;

	for (std::vector<GridCell*> col : cells) {
		for (GridCell *cell2 : col) {
			if (cell2->isStairs() && cell2 != cell) {
				std::list<GridCell *> path = astar::aStar(cell, cell2);
				if (!path.empty()) {
					paths.push_back(astar::aStar(cell, cell2));
				}
			}
		}
	}

	return paths;
}