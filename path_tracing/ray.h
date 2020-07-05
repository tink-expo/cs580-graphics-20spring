#define _RAY

#ifndef _POINT
#include "point.h"
#endif

#ifndef _VECTOR
#include "vector.h"
#endif

class Ray{
	Point Origin;
	Vector Direction;
public:
	Ray() : Origin(0.0,0.0,0.0), Direction(0.0,0.0,1.0) {};
	Ray(Point, Vector);

	Point& origin() {return Origin;}
	const Point& origin() const {return Origin;}
	Vector& direction() {return Direction;}
	const Vector& direction() const {return Direction;}
	
	Point pointAt(float);
	
	friend ostream& operator<<(ostream&,Ray);  //writing
	
};
	
