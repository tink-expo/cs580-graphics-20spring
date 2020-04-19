#define _GOBJECT

#ifndef _COLOUR
#include "colour.h"
#endif

#ifndef _MATERIAL
#include "material.h"
#endif

#ifndef _RAY
#include "ray.h"
#endif

//#include <iostream.h>
#include <iostream>




//an abstract class
class GObject{
	Material Mat;	//the material reflectance properties of the object
public:
	GObject() : Mat() {};
	
	Material& material() {return Mat;}	
	
	virtual bool intersect(Ray ray, float& t, Colour& colour) = 0;
	virtual Vector normal(Point) = 0;
	
	virtual void print(ostream&) = 0;
	
	//for each different kind of object need one of these
	virtual void read(istream&) = 0;
};
