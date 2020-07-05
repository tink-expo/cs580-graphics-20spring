#define _VECTOR

#include <iostream>
using namespace std;

#ifndef _POINT
#include "point.h"
#endif

class Vector{
	float X;
	float Y;
	float Z;
public:
	Vector() {X=Y=Z=0.0;}
	Vector(float,float,float);
	
	Vector(const Vector&);	  		//initialisation from another
	Vector& operator=(const Vector&); 	//assignment
	
	friend Vector operator-(const Point&,const Point&);//Vector = Point-Point
	friend Vector operator+(Point&, Point&);//Vector = Point+Point
	friend Point operator+(Point&, Vector&);
	friend Vector operator+(Vector &, Vector&);

	float squarednorm() const;			//returns square of norm
	float norm() const;				//returns norm
	void normalise();			//normalises 
	void normalise(float& norm);			//normalises 
	Vector invert() const;
	Vector normalised() const;			//returns normalised vector
	Vector normalised(float& norm) const;
	friend Vector add(Vector&,Vector&);	       	//add two vectors
	friend Vector subtract(Vector&,Vector&);	       	//subtract
	friend float operator^(const Vector&,const Vector&);   //dot product

	friend Vector operator*(Vector&,Vector&); //cross product
	friend Vector operator*(Vector&, float);//Vector = vector * scalar
	
	float &x() {return X;}
	float &y() {return Y;}
	float &z() {return Z;}

	float x() const {return X;}
	float y() const {return Y;}
	float z() const {return Z;}
		
	friend ostream& operator<<(ostream&,Vector);  //writing
	friend istream& operator>>(istream&,Vector&); //reading
};
