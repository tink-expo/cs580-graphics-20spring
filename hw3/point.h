#define _POINT

#include <iostream>

using namespace std;

class Point{
	float X;
	float Y;
	float Z;
public:
	Point() {X = 0.0; Y = 0.0; Z = 0.0;}	//constructor
	Point(float,float,float); 		//constructor
	
	Point(const Point&);	  		//initialisation from another point
	Point& operator=(const Point&); 	//assignment
	
	float& x() {return X;}
	float& y() {return Y;}
	float& z() {return Z;}
	const float& x() const {return X;}
	const float& y() const {return Y;}
	const float& z() const {return Z;}
	
	
	
	friend ostream& operator<<(ostream&,Point);  //writing
	friend istream& operator>>(istream&,Point&); //reading
};
