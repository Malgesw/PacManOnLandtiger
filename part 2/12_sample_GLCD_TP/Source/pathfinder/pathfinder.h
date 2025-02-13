//#include "GLCD/GLCD.h"
//#include "map/map.h"

typedef struct{
	short int posX;
	short int posY;
}Point;

typedef struct{
	Point parent;
	double f, g, h;
}Node;

//void aStarSearch(Point start, Point dest);
//Point getNextNode(Point start, Point dest);
Point getNextDir(Point p, Point dest);
Point getNextRandomDir(Point p);
unsigned short int isValid(Point p);
unsigned short int isWall(Point p);
unsigned short int isDestination(Point p, Point dest);
double computeHeuristic(Point p, Point dest);
//void initCellGrid();
double euclideanDist(Point p, Point q);
