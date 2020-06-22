#ifndef _COLOUR
#define _COLOUR

#include <iostream>
using namespace std;

class Colour{
	float Red, Green, Blue;
public:
	Colour() {Red = 0.0; Green = 0.0; Blue = 0.0;}
	Colour(float,float,float);	
	Colour(const Colour&);	  		//initialisation from another colour
	Colour& operator=(const Colour&); 	//assignment
	
	
	float& red() {return Red;}
	float& green() {return Green;}
	float& blue() {return Blue;}
	const float& red() const {return Red;}
	const float& green() const {return Green;}
	const float& blue() const {return Blue;}

	float  sum() const  {return (Red+Green+Blue); }
	
	void reset(float, float, float);	//resets the colour
	
	void check(); //checks in range 0 to 1 and adjusts if not
	
	friend Colour operator*(Colour&,Colour&); //returns product of two colours
	friend Colour operator*(const Colour&,Colour&); //returns product of two colours
	
	friend Colour operator+(Colour&,Colour&); //returns sum of two colours
	
	friend Colour operator*(Colour&,float); //colour * scalar
	friend Colour operator*(Colour&,double); //colour * scalar
	friend Colour operator*(const Colour&,float); //colour * scalar	
	friend Colour operator/(const Colour&,float); //colour / scalar
	
	friend ostream& operator<<(ostream&,Colour);  //writing
	friend istream& operator>>(istream&,Colour&); //reading
};

const Colour White(1.0,1.0,1.0);
const Colour Black(0.0,0.0,0.0);
const Colour Red(1.0,0.0,0.0);
const Colour Green(0.0,1.0,0.0);
const Colour Blue(0.0,0.0,1.0);
const Colour Cyan(0.0,1.0,1.0);
const Colour Magenta(1.0,0.0,1.0);
const Colour Yellow(1.0,1.0,0.0);

#endif