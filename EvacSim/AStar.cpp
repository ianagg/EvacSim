#include "AStar.h"
#include <math.h>
#include <iostream>
#include <algorithm>
#include <limits>
//#include <map>
//#include <unordered_map>

static double root2 = sqrt(2.0);
//static double root2 = 1.414;
static double d = 1.0;

double astar::calcDist(GridCell *one, GridCell *two) {
	//return sqrt(abs(two->getCol() - one->getCol())) + sqrt(abs(two->getRow() - one->getRow()));
	//return sqrt(abs(two->getCol() - one->getCol()) + abs(two->getRow() - one->getRow()));
	//return 1.0;
	if (isDiag(one, two)) {
		//return root2;
		//return d;
		//return 2.0 - 0.00000000000001;//????????????????
		//return 2.0 * 0.9999;
		return 2.0;
	}
	else {
		return d;
	}
}

double astar::calcHeuristic(GridCell *one, GridCell *two) {
	//return sqrt(abs(two->getCol() - one->getCol())) + sqrt(abs(two->getRow() - one->getRow()));
	//return sqrt(abs(two->getCol() - one->getCol()) + abs(two->getRow() - one->getRow()));
	double dCol = abs(two->getCol() - one->getCol());
	double dRow = abs(two->getRow() - one->getRow());

	double dMax = std::max(dCol, dRow);
	double dMin = std::min(dCol, dRow);

	//return sqrt(dCol) + sqrt(dRow);
	return dMax;
	//return 0.0;
	//return (root2 * dMin + d * (dMax - dMin)) * 0.9;
	//return (dCol + dRow) * 0.9999 * 0.9999;
	//return (dCol + dRow) * 0.999;
	//return d * (dCol + dRow) + (root2 - 2.0 * d) * std::min(dCol, dRow);
}

bool astar::sameCell(GridCell *one, GridCell *two) {
	return one->getCol() == two->getCol() && one->getRow() == two->getRow();
}

bool astar::shareWall(GridCell *one, GridCell *two) {
	for (GridCell *a : one->getNeighbours()) {
		for (GridCell *b : two->getNeighbours()) {
			if (sameCell(a, b) && a->isObstacle()) {
				return true;
			}
		}
	}

	return false;
}

//this could break stairs, need to change to x +/- 1 and y +/- 1
//probably won't break stairs if not using neighbours but should fix anyway
bool astar::isDiag(GridCell *one, GridCell *two) {
	return one->getCol() != two->getCol() && one->getRow() != two->getRow();
}

//need to pass the path built so far
std::list<GridCell *> astar::rec(GridCell *cell, int targetFloor, GridCell *end, std::list<GridCell *> onCurrentFloor, std::list<GridCell *> &visited) {
	visited.push_back(cell);
	GridCell *next = cell->getStairTarget();
	//visited.push_back(next);

	std::list<GridCell *> path;

	if (end == next) {
		path.push_back(next);
		path.push_back(end);
		return path;
	}

	//check not in visited here?
	if (next->getFloor() == targetFloor) {
		//check path from next to end (need to pass end)
		//just return anyway regardless of size, zero length paths handled in previous call
		path = aStar(next, end);
		if (!path.empty()) {
			return path;//fullPath + path
		}
	}
	//if next not visited already
	if (std::find(visited.begin(), visited.end(), next) == visited.end()) {

		//for each of next's stairPaths
		std::list<GridCell *> shortest;
		double shortestLength = std::numeric_limits<double>::max();
		int sl = std::numeric_limits<int>::max();

		for (std::list<GridCell *> stairPath : next->getStairPaths()) {
			if (!stairPath.empty()) {
				//get the last cell in path
				GridCell *last = stairPath.back();
				path = rec(last, targetFloor, end, onCurrentFloor, visited);
				//for now just use path length, need to actually load cost from file
				if (!path.empty() && path.size() < sl) {
					path.splice(path.begin(), stairPath);
					shortest = path;
					sl = path.size();
				}
			}
			
			/*if (path.back()->getFCost() < shortestLength) {
				path.splice(path.begin(), stairPath);
				shortest = path;
				shortestLength = shortest.back()->getFCost();
			}*/
		}

		//visited.push_back(next);
		return shortest;
	}
	else {
		//return empty path
		return path;
	}
}

std::list<GridCell *> astar::findPath(GridCell *start, GridCell *end, std::list<std::pair<GridCell *, GridCell *>> stairPairs) {
	int currentFloor = start->getFloor();
	int targetFloor = end->getFloor();

	std::list<GridCell *> path;
	if (currentFloor == targetFloor) {
		path = aStar(start, end);
	}

	if (path.size() == 0 || currentFloor != targetFloor) {
		//search stairPairs for stairs on current floor and stairs on targetFloor
		std::list<GridCell *> onCurrentFloor;
		std::list<GridCell *> onTargetFloor;//may not need??

		for (std::pair<GridCell *, GridCell *> pair : stairPairs) {
			if (pair.first->getFloor() == currentFloor) {
				onCurrentFloor.push_back(pair.first);
			}
			if (pair.first->getFloor() == targetFloor) {
				onTargetFloor.push_back(pair.first);
			}
			if (pair.second->getFloor() == currentFloor) {
				onCurrentFloor.push_back(pair.second);
			}
			if (pair.second->getFloor() == targetFloor) {
				onTargetFloor.push_back(pair.second);
			}
		}

		//std::map<std::pair<std::pair<int, int>, int>, GridCell *> visited;
		std::list<GridCell *> visited;
		std::list<GridCell *> shortest;
		double shortestLength = std::numeric_limits<double>::max();
		int sl = std::numeric_limits<int>::max();
		for (GridCell *cell : onCurrentFloor) {
			//if there is a path from start to cell
			if (start != cell) {
				path = aStar(start, cell);
			}
			else {
				path.clear();
				path.push_back(start);
			}
			if (path.size() > 0) {
				//pass empty visited each time?
				std::cout << cell->getCol() << ", " << cell->getRow() << ", " << cell->getFloor() << std::endl;
				std::list<GridCell *> p2 = rec(cell, targetFloor, end, onCurrentFloor, visited);

				if (p2.size() > 0) {
					path.splice(path.end(), p2);

					if (path.size() < sl) {
						shortest = path;
						sl = path.size();
					}
				}
			}
		}

		path = shortest;
	}

	return path;
}

std::list<GridCell *> astar::aStar(GridCell *start, GridCell *end) {
	//all except open could be forward_list ??
	//std::list<GridCell*> closed;
	std::map<std::pair<int, int>, GridCell *> closed;

	std::map<std::pair<int, int>, GridCell *> open;
	open.insert(std::pair<std::pair<int, int>, GridCell *>(std::pair<int, int>(start->getCol(), start->getRow()), start));
	//std::list<GridCell*> open;
	//open.push_back(start);

	std::list<GridCell*> cameFrom;
	cameFrom.push_back(end);

	start->setGCost(0);
	//start->setFCost(calcDist(start, end));
	start->setFCost(calcHeuristic(start, end));
	
	while (!open.empty()) {
		GridCell *current = findMin(open);

		//std::cout << open.size() << std::endl;
		//open.remove(current);
		open.erase(std::pair<int, int>(current->getCol(), current->getRow()));
		//std::cout << open.size() << std::endl;

		//closed.push_back(current);
		//std::pair<int, int> p = std::pair<int, int>(current->getCol(), current->getRow());
		closed.insert(std::pair<std::pair<int, int>, GridCell *>(std::pair<int, int>(current->getCol(), current->getRow()), current));
		
		bool best = false;

		unsigned numNeighbours = current->getNeighbours().size();
		for (unsigned i = 0; i < numNeighbours; i++) {

			GridCell *next = current->getNeighbours().at(i);

			if (!next->isObstacle() && !(isDiag(current, next) && shareWall(current, next))) {

				//if closed doesn't contain next
				//if (std::find(closed.begin(), closed.end(), next) == closed.end()) {
				if(closed.count(std::pair<int, int>(next->getCol(), next->getRow())) == 0) {

					//if open doesn't contain next
					//if (std::find(open.begin(), open.end(), next) == open.end()) {
					if (open.count(std::pair<int, int>(next->getCol(), next->getRow())) == 0) {
						//std::cout << next->getCol() << ", " << next->getRow() << std::endl;
						//next->toggleFire(false);
						if ((next->getCol() == end->getCol()) && (next->getRow() == end->getRow())) {
							end->setParent(current);

							while ((end->getParent()->getCol() != start->getCol()) || (end->getParent()->getRow() != start->getRow())) {
								end = end->getParent();
								cameFrom.push_front(end);
							}

							cameFrom.push_front(start);

							//if cameFrom size is 1 then no solution
							if (cameFrom.size() == 1) {
								cameFrom.pop_back();
							}
							return cameFrom;
						}

						next->setGCost(calcDist(next, current) + current->getGCost());

						//next->setFCost(next->getGCost() + calcDist(next, end));
						next->setFCost(next->getGCost() + calcHeuristic(next, end));

						//open.push_back(next);
						open.insert(std::pair<std::pair<int, int>, GridCell *>(std::pair<int, int>(next->getCol(), next->getRow()), next));

						best = true;
					}
					else {
						best = current->getGCost() > next->getGCost();
					}

					if (best) {
						next->setParent(current);
					}
				}
			}
		}
	}

	//if cameFrom size is 1 then no solution
	if (cameFrom.size() == 1) {
		cameFrom.pop_back();
	}
	return cameFrom;
}

GridCell * astar::findMin(std::list<GridCell *> g) {
	double min = g.front()->getFCost();
	GridCell *result = g.front();

	for (std::list<GridCell*>::const_iterator it = g.begin(), end = g.end(); it != end; ++it) {
		//std::cout << (*it)->getFCost() << std::endl;
		if ((*it)->getFCost() < min) {
			min = (*it)->getFCost();
			result = *it;
		}
	}

	//std::cout << result->getCol() << ", " << result->getRow() << std::endl;
	return result;
}

GridCell * astar::findMin(std::map<std::pair<int, int>, GridCell *> open) {
	GridCell *result = open.begin()->second;
	double min = result->getFCost();
	//double min = 10000.0;
	//GridCell *result = nullptr;

	//GridCell *tmp;
	for (std::map<std::pair<int, int>, GridCell *>::iterator it = open.begin(), end = open.end(); it != end; ++it) {
		if (it->second->getFCost() < min) {
			min = it->second->getFCost();
			result = it->second;
		}
		/*tmp = it->second;
		//std::cout << tmp->getFCost() << std::endl;
		if (tmp->getFCost() < min) {
			result = tmp;
			min = result->getFCost();
			//std::cout << min << std::endl;
		}*/
	}
	
	//std::cout << open.begin()->first.first << ", " << open.begin()->first.second << "         " << result->getCol() << ", " << result->getRow() << std::endl;
	return result;
}