#define _COLOUR

//#include <iostream.h>
#include <iostream>

using namespace std;


class Colour{
	float Red, Green, Blue;
public:
	Colour() {Red = 0.0; Green = 0.0; Blue = 0.0;}
	Colour(float,float,float);	
	Colour(const Colour&);	  		//initialisation from another colour
	Colour& operator=(const Colour&); 	//assignment

	bool operator==(const Colour&) const;
	
	
	float& red() {return Red;}
	float& green() {return Green;}
	float& blue() {return Blue;}
	float red() const {return Red;}
	float green() const {return Green;}
	float blue() const {return Blue;}
	
	void reset(float, float, float);	//resets the colour
	
	void check(); //checks in range 0 to 1 and adjusts if not
	
	friend Colour operator*(const Colour&, const Colour&); //returns product of two colours
	
	friend Colour operator+(const Colour&, const Colour&); //returns sum of two colours
	
	friend Colour operator*(const Colour&,float); //colour * scalar
	
	friend ostream& operator<<(ostream&, const Colour&);  //writing
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
