#define _LIGHT

#ifndef _COLOUR
#include "colour.h"
#endif

#ifndef _POINT
#include "point.h"
#endif

#ifndef _VECTOR
#include "vector.h"
#endif

//class for defining point or directional lights


class Light{
	bool Directional; //true if directional light
	float X,Y,Z;      //point or direction
	Colour Intensity; //intensity of the light
public:
	Light() : Intensity(1.0,1.0,1.0) {Directional = false; X = Y = Z = 1.0;} //default
	Light(Vector v) : Intensity(1.0,1.0,1.0) {Directional = true; X = v.x(); Y = v.y(); Z = v.z();}
	Light(Point p): Intensity(1.0,1.0,1.0)  {Directional = false; X = p.x(); Y = p.y(); Z = p.z();}
	
	Light(const Light&); 			//initialisation from another light
	Light& operator=(const Light&); 	//assignment	
	
	float &x() {return X;}
	float &y() {return Y;}
	float &z() {return Z;}
	Colour& intensity() {return Intensity;}
	
	Point point() {return Point(X,Y,Z);}
	
	Vector vector() {return Vector(X,Y,Z).normalised();}
	
	bool& directional() {return Directional;}
	
	friend ostream& operator<<(ostream&,Light);  //writing
	friend istream& operator>>(istream&,Light&); //reading
};
