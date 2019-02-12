#include "GridCell.h"
#include <GLM\gtc\matrix_transform.hpp>
#include <GLM\gtc\type_ptr.hpp>
#include <iostream>
#include <chrono>
#include "Person.h"

using ms = std::chrono::milliseconds;
using get_time = std::chrono::steady_clock;
GLfloat GridCell::speed = 1020;


GridCell::GridCell(GLfloat x, GLfloat y, GLfloat size, int col, int row, int floorNum) {
	this->size = size;
	this->centre = glm::vec2(x, y);
	this->col = col;
	this->row = row;
	this->floorNum = floorNum;

	//fCost = 0.0;
	//gCost = 1.0;

	speed = 1020;

	cellType = CellType::Walkable;
	cellColour = nullptr;
	lastPause = -1;
}

GridCell::~GridCell() {
	if (cellColour != nullptr) {
		delete cellColour;
	}
	glDeleteBuffers(2, VBO);
	glDeleteVertexArrays(1, &VAO);
}

void GridCell::createGeometry(GLuint program) {
	this->prog = program;

	/*(todo)
	inefficient to draw every line for each cell, just do top + left for most cells
	if a cell is in rightmost col then draw right line
	if in bottom most row draw bottom line*/
	GLfloat verts[] = {
		-size, size + lineWidth, 0.0f,
		-size, size - lineWidth, 0.0f,
		size, size - lineWidth, 0.0f,
		-size, size + lineWidth, 0.0f,
		size, size - lineWidth, 0.0f,
		size, size + lineWidth, 0.0f,

		-size + lineWidth, size, 0.0f,
		-size - lineWidth, size, 0.0f,
		-size - lineWidth, -size, 0.0f,
		-size + lineWidth, size, 0.0f,
		-size - lineWidth, -size, 0.0f,
		-size + lineWidth, -size, 0.0f,

		//only draw if in bottom row (not implemented)
		-size, -size + lineWidth, 0.0f,
		size, -size - lineWidth, 0.0f,
		size, -size + lineWidth, 0.0f,
		-size, -size + lineWidth, 0.0f,
		-size, -size - lineWidth, 0.0f,
		size, -size - lineWidth, 0.0f,

		//only draw if in rightmost col (not implemented)
		size - lineWidth, size, 0.0f,
		size - lineWidth, -size, 0.0f,
		size + lineWidth, size, 0.0f,
		size + lineWidth, size, 0.0f,
		size - lineWidth, -size, 0.0f,
		size + lineWidth, -size, 0.0f
	};

	GLfloat cols[72] = {};
	std::fill_n(cols, 72, 0.0f);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(2, VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
	GLint vertexLocation = glGetAttribLocation(program, "in_Position");
	glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(vertexLocation);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cols), cols, GL_STATIC_DRAW);
	GLint colorLocation = glGetAttribLocation(program, "in_Color");
	glVertexAttribPointer(colorLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(colorLocation);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	if (cellColour != nullptr) {
		cellColour->createGeometry(program);
	}
}

void GridCell::draw(GLuint program, glm::mat4 viewMat) {
	//std::cout << program << std::endl;
	glUseProgram(program);
	//translate stuff here
	//glm::mat4 mvm = glm::mat4(1.0f);
	//mvm = glm::translate(mvm, glm::vec3(centre.x, centre.y, 0.0f));
	glm::mat4 mvm = glm::translate(viewMat, glm::vec3(centre.x, centre.y, 0.0f));

	glUniformMatrix4fv(glGetUniformLocation(program, "ModelViewMatrix"), 1, GL_FALSE, glm::value_ptr(mvm));

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 24);//was 72 but that's wrong
	glBindVertexArray(0);
	glUseProgram(0);

	if (cellColour != nullptr) {
		cellColour->draw(program, viewMat);
	}
}

void GridCell::printCell() {
	std::cout << "(" << col << ", " << row << ")" << std::endl;
}

void GridCell::printNeighbours() {
	std::cout << "Cell: " << col << "," << row << " printing neighbours" << std::endl;
	std::cout << "Neighbour count: " << neighbours.size() << std::endl;
	for (unsigned i = 0; i < neighbours.size(); i++) {
		neighbours.at(i)->printCell();
	}
}

bool GridCell::pointInCell(GLfloat x, GLfloat y) {
	return (x <= centre.x + size) && (x >= centre.x - size) && (y <= centre.y + size) && (y >= centre.y - size);
}

//split into set and clear, check for stairs etc
void GridCell::toggleObstacle() {
	if (cellType == CellType::Walkable) {
		cellType = CellType::Obstacle;

		if (cellColour != nullptr) {
			delete cellColour;
			cellColour = nullptr;
		}

		cellColour = new Obstacle(centre.x, centre.y, size);
		cellColour->createGeometry(prog);
	}
	else if (cellType == CellType::Obstacle) {
		cellType = CellType::Walkable;
		delete cellColour;
		cellColour = nullptr;
	}
}

void GridCell::toggleFire(bool allowExtinguish) {
	if (cellType == CellType::Walkable) {
		cellType = CellType::Fire;
		lastPause = -1;

		if (cellColour != nullptr) {
			delete cellColour;
			cellColour = nullptr;
		}

		cellColour = new Fire(centre.x, centre.y, size);
		cellColour->createGeometry(prog);
	}
	else if (allowExtinguish && cellType == CellType::Fire) {
		cellType = CellType::Walkable;
		delete cellColour;
		cellColour = nullptr;
	}
}

void GridCell::update() {
	if (!queue.empty() && queue.front()->isDead()) {
		queue.pop_front();
	}

	if (cellType == CellType::Fire) {
		if (lastPause == -1) {
			lastPause = std::chrono::duration_cast<ms>(get_time::now().time_since_epoch()).count();
		}
		else if (std::chrono::duration_cast<ms>(get_time::now().time_since_epoch()).count() - lastPause > speed) {
			int r = rand() % neighbours.size();
			if (!(neighbours.at(r)->cellType == CellType::Stairs)) {
				neighbours.at(r)->toggleFire(false);
			}
			else {
				int rr = rand() % neighbours.at(r)->target->neighbours.size();
				neighbours.at(r)->target->neighbours.at(rr)->toggleFire(false);
			}

			lastPause = std::chrono::duration_cast<ms>(get_time::now().time_since_epoch()).count();
		}
	}
}

bool GridCell::setStairs(GridCell *target) {
	if (cellType == CellType::Walkable) {
		this->target = target;
		cellType = CellType::Stairs;

		if (cellColour != nullptr) {
			delete cellColour;
			cellColour = nullptr;
		}

		cellColour = new Stairs(centre.x, centre.y, size);
		cellColour->createGeometry(prog);

		return true;
	}
	else {
		//delete cellColour;
		//cellColour = nullptr;

		return false;
	}
}

void GridCell::clearStairs() {
	if (cellType == CellType::Stairs) {
		this->target->target = nullptr;
		this->target->otherStairPaths.clear();
		this->target->cellType = CellType::Walkable;
		delete this->target->cellColour;
		this->target->cellColour = nullptr;

		this->target = nullptr;
		this->otherStairPaths.clear();
		cellType = CellType::Walkable;
		delete cellColour;
		cellColour = nullptr;
	}
}

void GridCell::toggleStartPoint() {
	if (cellType == CellType::Walkable) {
		cellType = CellType::StartPoint;

		if (cellColour != nullptr) {
			delete cellColour;
			cellColour = nullptr;
		}

		cellColour = new StartPoint(centre.x, centre.y, size);
		cellColour->createGeometry(prog);
	}
	else if (cellType == CellType::StartPoint) {
		cellType = CellType::Walkable;
		delete cellColour;
		cellColour = nullptr;
	}
}

void GridCell::toggleExit() {
	if (cellType == CellType::Walkable) {
		cellType = CellType::Exit;

		if (cellColour != nullptr) {
			delete cellColour;
			cellColour = nullptr;
		}

		cellColour = new Exit(centre.x, centre.y, size);
		cellColour->createGeometry(prog);
	}
	else if (cellType == CellType::Exit) {
		cellType = CellType::Walkable;
		delete cellColour;
		cellColour = nullptr;
	}
}

void GridCell::writeStairData(char *bytes, int &i) {
	//write cell co-ordinates and floor
	for (int j = 0; j < 4; j++) {
		bytes[i + 3 - j] = (floorNum >> (j * 8));
		bytes[i + 7 - j] = (col >> (j * 8));
		bytes[i + 11 - j] = (row >> (j * 8));
	}

	i += 12;//i is position after the last byte of floorNum

	//write number of paths for the cell
	int numPaths = otherStairPaths.size();
	for (int j = 0; j < 4; j++) {
		bytes[i + 3 - j] = (numPaths >> (j * 8));
	}

	i += 4;//i is position after the last byte of numPaths

	//for each path
	int pathLength;
	for (std::list<GridCell *> path : otherStairPaths) {
		//save path length
		pathLength = path.size();
		for (int j = 0; j < 4; j++) {
			bytes[i + 3 - j] = (pathLength >> (j * 8));
		}

		i += 4;//i is position after last byte of pathLength;

		//for each cell in path
		for (GridCell * inPath : path) {
			//save coordinates of cell in path (and floor)
			int pFloor = inPath->getFloor();
			int pCol = inPath->getCol();
			int pRow = inPath->getRow();

			std::cout << "Writing cell: " << pCol << ", " << pRow << ", " << pFloor << std::endl;

			for (int j = 0; j < 4; j++) {
				bytes[i + 3 - j] = (pFloor >> (j * 8));
				bytes[i + 7 - j] = (pCol >> (j * 8));
				bytes[i + 11 - j] = (pRow >> (j * 8));
			}

			i += 12;//i is position after last byte of pFloor
		}

		//save path total fCost
		//double pFCost = (*path.end() - 1)->getFCost();
		if (!path.empty()) {
			double pFCost = path.back()->getFCost();
			const unsigned char *ptr = reinterpret_cast<const unsigned char *>(&pFCost);
			for (int j = 0; j < 8; j++) {
				bytes[i + 7 - j] = ptr[j];
			}
		}
		else {
			for (int j = 0; j < 8; j++) {
				bytes[i + 7 - j] = 0;
			}
		}

		i += 8;//i is position after last byte of pFCost
	}
}