#pragma once
#include "GridCell.h"
//#include <forward_list>
#include <list>
#include <map>

namespace astar {

	double calcDist(GridCell *one, GridCell *two);

	double calcHeuristic(GridCell *one, GridCell *two);

	bool sameCell(GridCell *one, GridCell *two);

	bool shareWall(GridCell *one, GridCell *two);

	bool isDiag(GridCell *one, GridCell *two);

	std::list<GridCell *> rec(GridCell *cell, int targetFloor, GridCell *end, std::list<GridCell *> onCurrentFloor, std::list<GridCell *> &visited);

	std::list<GridCell *> findPath(GridCell *start, GridCell *end, std::list<std::pair<GridCell *, GridCell *>> stairPairs);

	std::list<GridCell *> aStar(GridCell *start, GridCell *end);
	
	GridCell * findMin(std::map<std::pair<int, int>, GridCell *> open);
	GridCell * findMin(std::list<GridCell *> g);
}