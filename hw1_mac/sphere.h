#define _SPHERE

#ifndef _GOBJECT
#include "gobject.h"
#endif

using namespace std;

class Sphere : public GObject{
	Point Centre;
	float Radius; 

public :
	Sphere() : GObject(), Centre(0.0,0.0,0.0), Radius(1.0) {};
	Sphere(Material,Point,float);
	
	Point& centre() {return Centre;}
	float& radius() {return Radius;}
	
	Vector normal(Point p) {return (p-Centre).normalised();}
	bool intersect(const Ray& ray, float& t, Colour& colour);
	
	friend ostream& operator<<(ostream&,Sphere);  //writing
	friend istream& operator>>(istream&,Sphere&); //reading
	void print(ostream&);
	void read(istream&);
};
	
	

