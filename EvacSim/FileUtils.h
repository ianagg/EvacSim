#pragma once
#include <string>
#include <vector>
namespace fileutils {
	void writeData(std::string fName, char *bytes, int size);

	char * readData(std::string fName, int &size);

	bool validateStairCell(char *bytes, int &i, int size, int c, int r, std::vector<int> *floorNumbers);

	bool validateMap(char *bytes, int size);
}