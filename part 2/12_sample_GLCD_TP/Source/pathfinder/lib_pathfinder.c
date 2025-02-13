#include "GLCD/GLCD.h"
#include "pathfinder.h"
#include "map/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

extern unsigned short int mapGrid[ROWS][COLUMNS];
extern unsigned short int ghostMode;

Node nodeGrid[ROWS][COLUMNS];
short int xDir[4] = {-1, 1, 0, 0};
short int yDir[4] = {0, 0, -1, 1};
short int dirX[4] = {-1, 1, 0, 0};
short int dirY[4] = {0, 0, -1, 1};

unsigned short int isValid(Point p){
	return (p.posX >= 0 && p.posX < COLUMNS) && (p.posY >= 0 && p.posY < ROWS);
}

unsigned short int isWall(Point p){
	return mapGrid[p.posY][p.posX] == 1;
}

unsigned short int isDestination(Point p, Point dest){
	return (p.posX == dest.posX) && (p.posY == dest.posY);
}

double computeHeuristic(Point p, Point dest){
	return fabs((double)(p.posX - dest.posX)) + fabs((double)(p.posY - dest.posY)); //Manhattan distance
}

double euclideanDist(Point p, Point q){
	return sqrt(pow((p.posX-q.posX), 2) + pow((p.posY-q.posY), 2));
}

Point getNextDir(Point p, Point dest){
	double maxDist = -1;
	double minDist = FLT_MAX;//euclideanDist((Point){p.posX + xDir[0], p.posY + yDir[0]}, dest);
	int minDistDir = -1;
	int i;
	for(i = 0; i < 4; i++){
		Point currentPoint = {p.posX + xDir[i], p.posY + yDir[i]};
		double currentDist = euclideanDist(currentPoint, dest);
		if(isValid(currentPoint) && !isWall(currentPoint) && xDir[i] != 2 && ghostMode == 0 && currentDist < minDist){
			minDist = currentDist;
			minDistDir = i;
		}
		else if(isValid(currentPoint) && !isWall(currentPoint) && xDir[i] != 2 &&  ghostMode == 1 && currentDist > maxDist){
			maxDist = currentDist;
			minDistDir = i;
		}
		else if(xDir[i] == 2){
			if(i == 0){
				xDir[i] = -1;
			}
			else if(i == 1){
				xDir[i] = 1;
			}
			else{
				xDir[i] = 0;
			}
		}
	}
	
	Point bestDir = {xDir[minDistDir], yDir[minDistDir]};
	if(!(p.posY == 15 && ((p.posX == 13 && xDir[minDistDir] == -1) || (p.posX == 15 && xDir[minDistDir] == 1)))){
		if((minDistDir % 2) == 0){
			xDir[minDistDir+1] = 2;
		}
		else{
			xDir[minDistDir-1] = 2;
		}
	}
	return bestDir;
}

Point getNextRandomDir(Point p){
	
	int i;
	//Point randomDir;
	srand(LPC_TIM0->TC);
	unsigned short int isOk = 0;
	do{
		i = rand() % 4;
		//Point randomDir = {dirX[i], dirY[i]};
		Point currentPoint = {p.posX + dirX[i], p.posY + dirY[i]};
		if(isValid(currentPoint) && !isWall(currentPoint) && dirX[i] != 2){
			isOk = 1;
		}
	}while(!isOk);
	
	int j;
	for(j = 0; j < 4; j++){
		if(dirX[j] == 2){
			if(j == 0){
				dirX[j] = -1;
			}
			else if(j == 1){
				dirX[j] = 1;
			}
			else{
				dirX[j] = 0;
			}
			break;
		}
	}
	if(!(p.posY == 15 && ((p.posX == 13 && xDir[i] == -1) || (p.posX == 15 && xDir[i] == 1)))){
		if((i % 2) == 0){
			dirX[i+1] = 2;
		}
		else{
			dirX[i-1] = 2;
		}
	}
	
	return (Point){dirX[i], dirY[i]};
}
/*
void initNodeGrid(){
	int i, j;
	for(i = 0; i < ROWS; i++){
		for(j = 0; j < COLUMNS; j++){
			nodeGrid[i][j].f = FLT_MAX;
			nodeGrid[i][j].g = FLT_MAX;
			nodeGrid[i][j].h = FLT_MAX;
			nodeGrid[i][j].parent.posX = -1;
			nodeGrid[i][j].parent.posY = -1;
		}
	}
}*/

/*
void aStarSearch(Point start, Point dest){
	
	if (isDestination(start, dest)){
		return;
	}
	
	static unsigned short int closedList[ROWS][COLUMNS];
	Point openList[ROWS*COLUMNS];
	int k = 0;
	int i, j;
	//initialize nodeGrid and closedList
	for(i = 0; i < ROWS; i++){
		for(j = 0; j < COLUMNS; j++){
			closedList[i][j] = 0;
			nodeGrid[i][j].f = FLT_MAX;
			nodeGrid[i][j].g = FLT_MAX;
			nodeGrid[i][j].h = FLT_MAX;
			nodeGrid[i][j].parent.posX = -1;
			nodeGrid[i][j].parent.posY = -1;
		}
	}
	
	//initialize starting point
	nodeGrid[start.posY][start.posX].f = 0;
	nodeGrid[start.posY][start.posX].g = 0;
	nodeGrid[start.posY][start.posX].h = 0;
	nodeGrid[start.posY][start.posX].parent = start;
	
	openList[k++] = start;
	
	//main loop
	while(k > 0){
		
		//find node in open list with the lowest f value
		int minFPos = 0;
		for(i = 1; i < k; i++){
			if(nodeGrid[openList[i].posY][openList[i].posX].f < 
				nodeGrid[openList[minFPos].posY][openList[minFPos].posX].f){
					minFPos = i;
				}
		}
		Point current = openList[minFPos];
		openList[minFPos] = openList[--k];
		closedList[current.posY][current.posX] = 1;
		
		//check neighbors
		for(i = 0; i < 4; i++){
			
			Point neighPos = {current.posX + xDir[i], current.posY + yDir[i]};
			
			if(isValid(neighPos)){
				if(isDestination(neighPos, dest)){
					nodeGrid[neighPos.posY][neighPos.posX].parent = current;
					return;
				}
				else if(!closedList[neighPos.posY][neighPos.posX] && !isWall(neighPos)){
					
					double neigh_g = nodeGrid[current.posY][current.posX].g + 1.0;
					double neigh_h = computeHeuristic(neighPos, dest);
					double neigh_f = neigh_g + neigh_h;
					
					if(nodeGrid[neighPos.posY][neighPos.posX].f == FLT_MAX || 
						nodeGrid[neighPos.posY][neighPos.posX].f > neigh_f){
							openList[k++] = neighPos;
							nodeGrid[neighPos.posY][neighPos.posX].f = neigh_f;
							nodeGrid[neighPos.posY][neighPos.posX].g = neigh_g;
							nodeGrid[neighPos.posY][neighPos.posX].h = neigh_h;
							nodeGrid[neighPos.posY][neighPos.posX].parent = neighPos;
					}
				}
			}
		}
	}
}

Point getNextNode(Point start, Point dest){
	
	//aStarSearch(start, dest); // update nodeGrid with A*
	
	Point path[ROWS*COLUMNS];
	int k = 0;
	uint16_t posX = dest.posX;
	uint16_t posY = dest.posY;
	
	while(nodeGrid[posY][posX].parent.posX != posX || nodeGrid[posY][posX].parent.posY != posY){
		path[k++] = (Point){posX, posY};
		Point tmp = nodeGrid[posY][posX].parent;
		posX = tmp.posX;
		posY = tmp.posY;
	}
	
	//path[k++] = start;
	
	int i;
	// reverse the path array to get the next node by swapping elements
	for(i = 0; i < k/2; i++){
		Point tmp = path[i];
		path[i] = path[k - i - 1];
		path[k - i - 1] = tmp;
	}
	
	if(k > 0){
		return path[0];
	}
	else{
		return start;
	}
}
*/

