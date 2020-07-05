#define _SPHERE

#ifndef _GOBJECT
#include "gobject.h"
#endif

#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

class Sphere : public GObject{
	Point Centre;
	float Radius; 
	bool  Transmitted;
	float Refraction;

public :
	Sphere() : GObject(), Centre(0.0,0.0,0.0), Radius(1.0), Transmitted(0), Refraction(0.0) {};
	Sphere(Material mat, Point centre, float radius, bool transmitted=false, float refraction=0.0f);
	
	Point& centre() {return Centre;}
	float& radius() {return Radius;}
	const float& radius() const {return Radius;}
	
	virtual Point objectCentre(void) {return Centre;};

	Vector normal(Point p) {return (p-Centre).normalised();}
	bool intersect(Ray ray, float& t, Colour& colour);
	
	//---------------------------------
	// specific path tracing extensions
	bool sample(Point& p,float& pdf,const Point& f,float s,float t);
  bool isRefractive();
  float getRfrIndex();
	//---------------------------------


	friend ostream& operator<<(ostream&,Sphere);  //writing
	friend istream& operator>>(istream&,Sphere&); //reading
	void print(ostream&);
	void read(istream&);

	virtual eObjectType objectType(void) const {return OBJECT_SPHERE;}
};
	
	
