#include "Building.h"
#include "FileUtils.h"
#include "AStar.h"

#include <limits>

const GLfloat Building::cellSize = 0.2f;

Building::Building(GLint shaderProgram, int numCols, int numRows) {
	this->shaderProgram = shaderProgram;
	Grid::setLoadPersonAction(&Building::loadPersonCallback, this);

	everyone = 0;
	deadone = 0;

	Grid *g = new Grid(cellSize, numCols, numRows, 0, 0);
	g->calcNeighbours();
	g->createGeometry(shaderProgram);
	grids.insert(std::pair<int, Grid *>(g->getFloorNum(), g));
	loadResult = true;
}

Building::Building(GLint shaderProgram, bool runningSimulation, std::string fName) {
	this->shaderProgram = shaderProgram;
	Grid::setLoadPersonAction(&Building::loadPersonCallback, this);

	everyone = 0;

	loadResult = loadMap(runningSimulation, fName);
}

Building::~Building() {
	for (Person *p : people) {
		delete p;
	}
	people.clear();

	for (auto const &g : grids) {
		delete g.second;
	}
	grids.clear();

	stairPairs.clear();

	tmpStairs = nullptr;
	endPoint = nullptr;
}

bool Building::loadMap(bool runningSimulation, std::string fName) {
	//read file till eof
	int i = 0;
	int size = -1;
	char *bytes = fileutils::readData(fName, size);

	if (bytes != nullptr) {
		//get numGrids from bytes
		int numGrids = (int)bytes[3] | (int)bytes[2] << 8 | (int)bytes[1] << 16 | (int)bytes[0] << 24;

		i += 4;

		//for 0 - numGrids create grid with data in bytes
		for (int j = 0; j < numGrids; j++) {
			int floorNum = (int)bytes[i + 3] | (int)bytes[i + 2] << 8 | (int)bytes[i + 1] << 16 | (int)bytes[i + 0] << 24;
			int numCols = (int)bytes[i + 7] | (int)bytes[i + 6] << 8 | (int)bytes[i + 5] << 16 | (int)bytes[i + 4] << 24;
			int numRows = (int)bytes[i + 11] | (int)bytes[i + 10] << 8 | (int)bytes[i + 9] << 16 | (int)bytes[i + 8] << 24;
			int mapLocation = (int)bytes[i + 15] | (int)bytes[i + 14] << 8 | (int)bytes[i + 13] << 16 | (int)bytes[i + 12] << 24;
			i += 16;

			bool validFloor = mapLocation >= 0 && mapLocation <= 9;

			if (validFloor) {
				Grid *g = new Grid(cellSize, bytes, i, floorNum, numCols, numRows, runningSimulation, mapLocation);
				g->calcNeighbours();
				g->createGeometry(shaderProgram);

				grids.insert(std::pair<int, Grid *>(floorNum, g));
			}
		}

		//after grids created get numStairPairs

		int numStairPairs = (int)bytes[i + 3] | (int)bytes[i + 2] << 8 | (int)bytes[i + 1] << 16 | (int)bytes[i + 0] << 24;
		i += 4;

		for (int j = 0; j < numStairPairs; j++) {
			GridCell * first = loadStairData(bytes, i);
			GridCell * second = loadStairData(bytes, i);

			//add pair to stairPairs
			first->setStairs(second);
			second->setStairs(first);

			stairPairs.push_back(std::pair<GridCell *, GridCell *>(first, second));
		}

		delete[] bytes;
		return true;
	}
	else {
		return false;
	}
}

GridCell * Building::loadStairData(char *bytes, int &i) {
	//get first cell in stair pair
	int floorNum = (int)bytes[i + 3] | (int)bytes[i + 2] << 8 | (int)bytes[i + 1] << 16 | (int)bytes[i + 0] << 24;
	int colNum = (int)bytes[i + 7] | (int)bytes[i + 6] << 8 | (int)bytes[i + 5] << 16 | (int)bytes[i + 4] << 24;
	int rowNum = (int)bytes[i + 11] | (int)bytes[i + 10] << 8 | (int)bytes[i + 9] << 16 | (int)bytes[i + 8] << 24;
	int numPaths = (int)bytes[i + 15] | (int)bytes[i + 14] << 8 | (int)bytes[i + 13] << 16 | (int)bytes[i + 12] << 24;
	i += 16;

	GridCell * first = grids.at(floorNum)->getCell(colNum, rowNum);

	//read its path data
	int pathLength;
	std::vector<std::list<GridCell *>> otherStairPaths;
	for (int j = 0; j < numPaths; j++) {
		pathLength = (int)bytes[i + 3] | (int)bytes[i + 2] << 8 | (int)bytes[i + 1] << 16 | (int)bytes[i + 0] << 24;
		i += 4;

		std::list<GridCell *> path;
		for (int k = 0; k < pathLength; k++) {
			floorNum = (int)bytes[i + 3] | (int)bytes[i + 2] << 8 | (int)bytes[i + 1] << 16 | (int)bytes[i + 0] << 24;
			colNum = (int)bytes[i + 7] | (int)bytes[i + 6] << 8 | (int)bytes[i + 5] << 16 | (int)bytes[i + 4] << 24;
			rowNum = (int)bytes[i + 11] | (int)bytes[i + 10] << 8 | (int)bytes[i + 9] << 16 | (int)bytes[i + 8] << 24;
			i += 12;

			path.push_back(grids.at(floorNum)->getCell(colNum, rowNum));

		}

		otherStairPaths.push_back(path);

		i += 8;//skip cost for now
	}

	first->setStairPaths(otherStairPaths);

	return first;
}

void Building::saveMap(std::string fName) {
	//track total size needed for final byte array
	int totalSize = 0;
	totalSize += 4;//number of grids
	for (auto const &g : grids) {
		//get number of bytes to write for each grid
		totalSize += g.second->byteArraySize();
	}

	//TODO
	//can avoid re-calculation by inverting paths between stairs
	totalSize += 4;//number of stairPairs
	std::vector<std::pair<GridCell *, std::vector<std::list<GridCell *>>>> allStairPaths;
	for (std::pair<GridCell *, GridCell *> pair : stairPairs) {
		GridCell *gc = pair.first;
		std::vector<std::list<GridCell *>> paths = grids.at(gc->getFloor())->getStairPaths(gc);
		gc->setStairPaths(paths);
		allStairPaths.push_back(std::pair<GridCell *, std::vector<std::list<GridCell *>>>(gc, paths));

		totalSize += 12;//first cell co-ordinates
		totalSize += 4;//number of paths
		for (std::list<GridCell *> path : paths) {
			totalSize += 4;//path length
			totalSize += path.size() * 12;//co-ordinates for each cell
			totalSize += sizeof(double);//path cost
		}


		gc = pair.second;
		paths = grids.at(gc->getFloor())->getStairPaths(gc);
		gc->setStairPaths(paths);
		allStairPaths.push_back(std::pair<GridCell *, std::vector<std::list<GridCell *>>>(gc, paths));

		totalSize += 12;//second cell co-ordinates
		totalSize += 4;//number of paths
		for (std::list<GridCell *> path : paths) {
			totalSize += 4;//path length
			totalSize += path.size() * 12;//co-ordinates for each cell
			totalSize += sizeof(double);//path cost
		}
	}

	totalSize++;//eof character


	char *bytes = new char[totalSize];
	int i = 0;

	//save grids
	int numGrids = grids.size();

	for (int j = 0; j < 4; j++) {
		bytes[i + 3 - j] = (numGrids >> (j * 8));
	}

	i += 4;

	for (auto const &g : grids) {
		g.second->toByteArray(bytes, i);
	}

	//save stairs
	int numStairPairs = stairPairs.size();
	for (int j = 0; j < 4; j++) {
		bytes[i + 3 - j] = (numStairPairs >> (j * 8));
	}

	i += 4;

	for (std::pair<GridCell *, GridCell *> p : stairPairs) {
		p.first->writeStairData(bytes, i);
		p.second->writeStairData(bytes, i);
	}

	bytes[i] = EOF;

	std::cout << totalSize << ",  " << i << std::endl;
	fileutils::writeData(fName, bytes, totalSize);

	delete[] bytes;
}

void Building::addFloor(int floorNum, int floorPosition) {
	int numCols = grids.at(0)->getNumCols();
	int numRows = grids.at(0)->getNumRows();

	bool validNumber = floorPosition >= 0 && floorPosition <= 9;

	if (validNumber) {
		Grid *ng = new Grid(cellSize, numCols, numRows, floorNum, floorPosition);
		ng->calcNeighbours();
		ng->createGeometry(shaderProgram);
		grids.insert(std::pair<int, Grid *>(floorNum, ng));
	}
}

void Building::getValidMapPositions(std::vector<int> &vp) {
	bool contains[9] = { false };
	for (auto const &g : grids) {
		contains[g.second->getMapPosition()] = true;
	}

	for (int i = 0; i < 9; i++) {
		if (!contains[i]) {
			vp.push_back(i);
		}
	}
}

void Building::getValidFloorNumbers(std::vector<int> &vf) {
	int min = std::numeric_limits<int>::max();
	int max = std::numeric_limits<int>::min();

	for (auto const &g : grids) {
		int f = g.second->getFloorNum();

		min = (f < min) ? f : min;
		max = (f > max) ? f : max;
	}

	if (grids.size() > 0) {
		vf.push_back(min - 1);
		vf.push_back(max + 1);
	}
}

void Building::loadPersonCallback(GLfloat x, GLfloat y) {
	Person *p = new Person(x, y, cellSize / 2.0f, 0.01f, this);
	p->createGeometry(shaderProgram);
	people.push_back(p);
	++everyone;
}

//just return GridCell *??
bool Building::determineCell(GLfloat x, GLfloat y, int &c, int &r, int &f) {
	for (auto const &pair : grids) {
		Grid *g = pair.second;
		if (c == -1 || r == -1 || f == -1) {
			g->determineCell(x, y, c, r, f);
		}
	}

	return (c != -1 && r != -1 && f != -1);
}

bool Building::cellOnFire(GLfloat x, GLfloat y) {
	int c = -1;
	int r = -1;
	int f = -1;
	determineCell(x, y, c, r, f);
	if (c != -1 && r != -1 && f != -1) {
		return grids.at(f)->getCell(c, r)->isFire();
	}
}

void Building::moveToPoint(GLfloat pointX, GLfloat pointY) {
	int pointCol = -1;
	int pointRow = -1;
	int pointFloor = -1;

	if (determineCell(pointX, pointY, pointCol, pointRow, pointFloor)) {
		for (Person *p : people) {
			int persCol = -1;
			int persRow = -1;
			int persFloor = -1;

			determineCell(p->getX(), p->getY(), persCol, persRow, persFloor);

			if (grids.count(pointFloor) > 0 && grids.count(persFloor) > 0) {
				p->setPath(astar::findPath(grids.at(persFloor)->getCell(persCol, persRow), grids.at(pointFloor)->getCell(pointCol, pointRow), stairPairs));
			}
		}
	}
}

void Building::moveToExit() {
	if (endPoint != nullptr) {
		moveToPoint(endPoint->getCentre().x, endPoint->getCentre().y);
	}
}

void Building::placeObstacle(GLfloat cursorX, GLfloat cursorY) {
	if (!placingStairs) {
		int mouseCol = -1;
		int mouseRow = -1;
		int mouseFloor = -1;

		if (determineCell(cursorX, cursorY, mouseCol, mouseRow, mouseFloor)) {
			grids.at(mouseFloor)->getCell(mouseCol, mouseRow)->toggleObstacle();
		}
	}
}

void Building::placeStairs(GLfloat cursorX, GLfloat cursorY) {
	int mouseCol = -1;
	int mouseRow = -1;
	int mouseFloor = -1;

	if (determineCell(cursorX, cursorY, mouseCol, mouseRow, mouseFloor)) {
		if (!placingStairs) {
			placingStairs = true;
			tmpStairs = grids.at(mouseFloor)->getCell(mouseCol, mouseRow);

			//if selected cell is already stairs then change back to regular cell
			if (tmpStairs->isStairs()) {
				GridCell *s1, *s2;
				s1 = tmpStairs;
				s2 = tmpStairs->getStairTarget();

				std::pair<GridCell *, GridCell *> p1, p2;
				p1 = std::pair<GridCell *, GridCell *>(s1, s2);
				p2 = std::pair<GridCell *, GridCell *>(s2, s1);

				std::list<std::pair<GridCell *, GridCell *>>::iterator it;
				it = std::find(stairPairs.begin(), stairPairs.end(), p1);
				if (it != stairPairs.end()) {
					stairPairs.erase(it);
				}
				else {
					it = std::find(stairPairs.begin(), stairPairs.end(), p2);
					if (it != stairPairs.end()) {
						stairPairs.erase(it);
					}
				}

				tmpStairs->clearStairs();
				tmpStairs = nullptr;
				placingStairs = false;
			}
			else if (!tmpStairs->isWalkable()) {
				placingStairs = false;
				tmpStairs = nullptr;
			}
		}
		else {
			//make a bunch of stuff internal to GridCell
			GridCell *tS2 = grids.at(mouseFloor)->getCell(mouseCol, mouseRow);
			if (tS2 == tmpStairs) {
				tmpStairs = nullptr;
				placingStairs = false;
			}
			else if (tS2->getFloor() == tmpStairs->getFloor()) {
				std::cout << "Target is on the same floor, please select a different target!" << std::endl;
			}
			else if (tS2->isStairs()) {
				std::cout << "Target is already stairs, please select a different target!" << std::endl;
			}
			else if (tS2->isWalkable()) {
				tmpStairs->setStairs(tS2);
				tS2->setStairs(tmpStairs);
				stairPairs.push_back(std::pair<GridCell *, GridCell *>(tmpStairs, tS2));
				tmpStairs = nullptr;
				placingStairs = false;
			}
		}
	}
}

void Building::placePersonStart(GLfloat cursorX, GLfloat cursorY) {
	if (!placingStairs) {
		int mouseCol = -1;
		int mouseRow = -1;
		int mouseFloor = -1;

		if (determineCell(cursorX, cursorY, mouseCol, mouseRow, mouseFloor)) {
			grids.at(mouseFloor)->getCell(mouseCol, mouseRow)->toggleStartPoint();
		}
	}
}

void Building::placeFire(GLfloat cursorX, GLfloat cursorY) {
	if (!placingStairs) {
		int mouseCol = -1;
		int mouseRow = -1;
		int mouseFloor = -1;

		if (determineCell(cursorX, cursorY, mouseCol, mouseRow, mouseFloor)) {
			grids.at(mouseFloor)->getCell(mouseCol, mouseRow)->toggleFire(true);
		}
	}
}

void Building::placeExit(GLfloat cursorX, GLfloat cursorY) {
	if (!placingStairs) {
		int mouseCol = -1;
		int mouseRow = -1;
		int mouseFloor = -1;

		if (determineCell(cursorX, cursorY, mouseCol, mouseRow, mouseFloor)) {
			GridCell *gc = grids.at(mouseFloor)->getCell(mouseCol, mouseRow);

			//if no exit exists then set gc to the exit
			if (endPoint == nullptr) {
				gc->toggleExit();
				endPoint = gc;
			}
			//if an exit exits and gc is the exit, remove it
			else if (gc == endPoint) {
				gc->toggleExit();
				endPoint = nullptr;
			}
			else {
				std::cout << "Cannot have more than one exit" << std::endl;
			}

			//grids.at(mouseFloor)->getCell(mouseCol, mouseRow)->toggleExit();
		}
	}
}

void Building::createGeometry() {
	for (Person *p : people) {
		p->createGeometry(shaderProgram);
	}

	for (auto const &g : grids) {
		g.second->createGeometry(shaderProgram);
	}
}

void Building::draw(GLfloat interpolation, bool drawPeople) {
	glm::mat4 viewMat = glm::mat4(1.0f);
	if (drawPeople) {
		for (Person *p : people) {
			p->draw(shaderProgram, interpolation, viewMat);
		}
	}

	for (auto const &g : grids) {
		g.second->draw(shaderProgram, viewMat);
	}
}

void Building::updateScene() {
	int count = 0;
	for (Person *p : people) {
		if (p->isDead())
			count++;
		if (!p->hasReachedTarget()) {
			p->update();
		}
		else {
			for (std::vector<Person*>::iterator iter = people.begin(); iter != people.end(); ++iter) {
				if (*iter == p) {
					people.erase(iter);
					break;
				}
			}
		}
	}

	for (auto const &g : grids) {
		g.second->update();
	}
	deadone = count;

	if (people.size() == count) {
		endSim = true;
	}

}