#define _RAY

#ifndef _POINT
#include "point.h"
#endif

#ifndef _VECTOR
#include "vector.h"
#endif

using namespace std;

class Ray{
	Point Origin;
	Vector Direction;
public:
	Ray() : Origin(0.0,0.0,0.0), Direction(0.0,0.0,1.0) {};
	
	Point& origin() {return Origin;}
	Vector& direction() {return Direction;}
	
	Point pointAt(float);
	
	friend ostream& operator<<(ostream&,Ray);  //writing
	
};
	


