#define _POLYGON

#ifndef _GOBJECT
#include "gobject.h"
#endif

class Polygon : public GObject{
	int Max;	//max number of points
	int N; 		//number of points so far
	Point* P; 	//array of points 
	Vector Nm;	//normal
	float D;	//plane equation is Nm.(x,y,z) = D*/

public :
	Polygon() : GObject() {N = 0; Max = 0;};
	Polygon(Material,int);
	Polygon(Material mat) {N = 0; Max = 0; material() = mat;};
	
	virtual Point objectCentre(void);

	void setMaxPoints(int m) {Max = m; P = new Point[m]; N = 0;};
	
	int numPoints() {return N;}
	
	Point point(int i) {return P[i];} //should check bounds of i
	
	void set(Point p); //sets the point p in the next available slot
	
	void planeEquation(void); //sets the plane equation
	void planeEquation(Vector& normal, float &d);
	
	float evalPlaneEquation(Point);
	float evalPlaneEquation(Vector v);
	
	Vector normal(Point) {return Nm;}; //Point is irrelevant here
	
	bool intersect(Ray ray, float& t, Colour& colour);
	
	//---------------------------------
	// specific path tracing extensions
	float TriangularSampling(Point& p,float s,float t, int Type);
	float RectangularSampling(Point& p,float s,float t, int Type);
	float PolygonSampling(Point& p,float s,float t, int Type);	
  bool sample(Point& p,float& pdf,const Point& f,float s,float t);
  bool isRefractive();
  float getRfrIndex();
	//---------------------------------

	friend ostream& operator<<(ostream&,Polygon);  //writing
	friend istream& operator>>(istream&,Polygon&); //reading
	void print(ostream&);
	void read(istream&);

	virtual eObjectType objectType(void) const {return OBJECT_POLYGON;}

};
