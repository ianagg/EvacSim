#include "FileUtils.h"
#include <fstream>
#include <algorithm>
#include <iostream>

void fileutils::writeData(std::string fName, char *bytes, int size) {
	std::ofstream file(fName, std::ios::binary);

	file.write(bytes, size);
	file.close();
}

char * fileutils::readData(std::string fName, int &size) {
	std::ifstream in(fName, std::ios::binary);

	if (in) {
		in.seekg(0, in.end);
		size = in.tellg();
		in.seekg(0, in.beg);

		char *bytes = new char[size];

		in.read(bytes, size);
		in.close();
		try {
			if (validateMap(bytes, size)) {
				std::cout << "Valid map" << std::endl;
				return bytes;
			}
			else {
				std::cout << "NOT a valid map" << std::endl;
				delete[] bytes;
				return nullptr;
				//return bytes;
			}
		}
		catch (...) {
			std::cout << "Exception ocurred, invalid map" << std::endl;
			delete[] bytes;
			return nullptr;
		}
	}

	return nullptr;
}

bool fileutils::validateMap(char *bytes, int size) {
	int i = 0;
	if (i + 4 > size) {
		return false;
	}

	int numGrids = (int)bytes[3] | (int)bytes[2] << 8 | (int)bytes[1] << 16 | (int)bytes[0] << 24;
	i += 4;

	if (numGrids < 1 || numGrids > 9) {
		//pointless to load a file with zero floors
		//maximum of 9 floors can exist on the map
		return false;
	}

	bool contains[9] = { false };
	std::vector<int> floorNumbers;
	int c = -1;
	int r = -1;
	for (int j = 0; j < numGrids; j++) {
		if (i + 16 > size) {
			return false;
		}

		int floorNum = (int)bytes[i + 3] | (int)bytes[i + 2] << 8 | (int)bytes[i + 1] << 16 | (int)bytes[i + 0] << 24;
		int numCols = (int)bytes[i + 7] | (int)bytes[i + 6] << 8 | (int)bytes[i + 5] << 16 | (int)bytes[i + 4] << 24;
		int numRows = (int)bytes[i + 11] | (int)bytes[i + 10] << 8 | (int)bytes[i + 9] << 16 | (int)bytes[i + 8] << 24;
		int mapLocation = (int)bytes[i + 15] | (int)bytes[i + 14] << 8 | (int)bytes[i + 13] << 16 | (int)bytes[i + 12] << 24;
		i += 16;

		/*add floorNum to list for validation later (after all other aspects of each floor have been validated)*/
		floorNumbers.push_back(floorNum);

		/*validate the position of the floor on the map*/
		if (!(mapLocation >= 0 && mapLocation <= 9)) {
			//maximum of 9 floors can exist on the map
			return false;
		}

		if (!contains[mapLocation]) {
			//set this location on map to occupied
			contains[mapLocation] = true;
		}
		else {
			//two floors cannot occupy the same location on the map
			return false;
		}

		/*Validate number of columns and rows*/
		if (c == -1 && r == -1 && numCols <= 50 && numCols >= 5 && numRows <= 50 && numRows >= 5) {
			c = numCols;
			r = numRows;
		}
		else if (c == -1 || r == -1) {
			//either both should have initialised correctly or neither has
			return false;
		}
		else if (numCols != c || numRows != r) {
			//all floors must be the same size
			return false;
		}

		/*check that each of the following bytes holds a valid value for a cell*/
		int totalCells = numCols * numRows;
		for (int k = 0; k < totalCells; k++) {
			if (i + 1 > size || !(bytes[i] == 1 || bytes[i] == 0 || bytes[i] == 'f' || bytes[i] == 't' || bytes[i] == 'p' || bytes[i] == 'e')) {
				return false;
			}

			i++;
		}
	}

	/*validate floor numbering*/
	if (floorNumbers.size() > 1) {
		std::sort(floorNumbers.begin(), floorNumbers.end());
		int last = *floorNumbers.begin();
		for (int k = 1; k < floorNumbers.size(); k++) {
			int current = floorNumbers.at(k);
			if (!(current == last + 1)) {//should always be in same order (e.g. smallest -> largest)
				return false;
			}

			last = current;
		}
	}

	/*finished validating grid data*/

	/*validate stair data*/
	if (i + 4 > size) {
		return false;
	}

	int numStairPairs = (int)bytes[i + 3] | (int)bytes[i + 2] << 8 | (int)bytes[i + 1] << 16 | (int)bytes[i + 0] << 24;
	i += 4;

	for (int j = 0; j < numStairPairs; j++) {
		//validate first stair cell of pair
		bool validStairCell = validateStairCell(bytes, i, size, c, r, &floorNumbers);

		if (!validStairCell) {
			//std::cout << "Stair data invalid 1, j = " << j << std::endl;
			return false;
		}

		//validate second stair cell of pair
		validStairCell = validateStairCell(bytes, i, size, c, r, &floorNumbers);

		if (!validStairCell) {
			//std::cout << "Stair data invalid 2, j = " << j << std::endl;
			return false;
		}
	}

	std::cout << "i = " << i << ", size = " << size << std::endl;

	//std::cout << bytes[i - 2] << ", " << bytes[i - 1] << ", " << bytes[i] << std::endl;

	return bytes[i] == EOF && i + 1 == size;

	//return true;
}

bool fileutils::validateStairCell(char *bytes, int &i, int size, int c, int r, std::vector<int> *floorNumbers) {
	if (i + 16 > size) {
		return false;
	}

	int floorNum = (int)bytes[i + 3] | (int)bytes[i + 2] << 8 | (int)bytes[i + 1] << 16 | (int)bytes[i + 0] << 24;
	int colNum = (int)bytes[i + 7] | (int)bytes[i + 6] << 8 | (int)bytes[i + 5] << 16 | (int)bytes[i + 4] << 24;
	int rowNum = (int)bytes[i + 11] | (int)bytes[i + 10] << 8 | (int)bytes[i + 9] << 16 | (int)bytes[i + 8] << 24;
	int numPaths = (int)bytes[i + 15] | (int)bytes[i + 14] << 8 | (int)bytes[i + 13] << 16 | (int)bytes[i + 12] << 24;
	i += 16;

	if (std::find(floorNumbers->begin(), floorNumbers->end(), floorNum) == floorNumbers->end()) {
		return false;
	}

	if (colNum < 0 || colNum > c || rowNum < 0 || rowNum > r) {
		return false;
	}

	if (numPaths < 0) {
		return false;
	}

	for (int j = 0; j < numPaths; j++) {
		if (i + 4 > size) {
			return false;
		}

		int pathLength = (int)bytes[i + 3] | (int)bytes[i + 2] << 8 | (int)bytes[i + 1] << 16 | (int)bytes[i + 0] << 24;
		i += 4;

		if (pathLength < 0) {
			return false;
		}

		for (int k = 0; k < pathLength; k++) {
			if (i + 12 > size) {
				return false;
			}

			floorNum = (int)bytes[i + 3] | (int)bytes[i + 2] << 8 | (int)bytes[i + 1] << 16 | (int)bytes[i + 0] << 24;
			colNum = (int)bytes[i + 7] | (int)bytes[i + 6] << 8 | (int)bytes[i + 5] << 16 | (int)bytes[i + 4] << 24;
			rowNum = (int)bytes[i + 11] | (int)bytes[i + 10] << 8 | (int)bytes[i + 9] << 16 | (int)bytes[i + 8] << 24;
			i += 12;

			if (std::find(floorNumbers->begin(), floorNumbers->end(), floorNum) == floorNumbers->end()) {
				return false;
			}

			if (colNum < 0 || colNum > c || rowNum < 0 || rowNum > r) {
				return false;
			}
		}

		if (i + 8 > size) {
			return false;
		}

		i += 8;
	}

	return true;
}