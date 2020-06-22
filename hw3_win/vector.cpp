#include "vector.h"
#include "math.h"


Vector Vector::invert() const
//constructor
{
	return Vector(-X,-Y,-Z);
}

Vector::Vector(float x, float y, float z)
//constructor
{
	X = x;
	Y = y;
	Z = z;
}

Vector::Vector(const Vector& p)
//initialisation from another Vector
{
	X = p.X;
	Y = p.Y;
	Z = p.Z;
}

Vector& Vector::operator=(const Vector& p)
//assignment
{
	
	if(this == &p) return *this;
	
	X = p.X;
	Y = p.Y;
	Z = p.Z;
	
	return *this;
}

float Vector::squarednorm() const
{
	return X*X+Y*Y+Z*Z;
}


float Vector::norm() const
//returns norm of vector
{
	return (float)sqrt(squarednorm());
}

void Vector::normalise()
//normalises the vector
{
	float d = this->norm();
	
	X /= d;
	Y /= d;
	Z /= d;
}
void Vector::normalise(float& norm)
//normalises the vector
{
	norm = this->norm();
	
	X /= norm;
	Y /= norm;
	Z /= norm;
}

Vector Vector::normalised() const
//returns the normalised vector
{
	float d = this->norm();
	return Vector(X / d,Y / d,Z / d);
}
Vector Vector::normalised(float& norm) const
//returns the normalised vector
{
	norm = this->norm();
	return Vector(X / norm,Y / norm,Z / norm);
}

ostream& operator<<(ostream& s,Vector p)
//writing
{	
	s << "[" << p.X << "," << p.Y << "," << p.Z << "]";
	return s;
}


istream& operator>>(istream& s,Vector& p)
//reading
{
	s >> p.X;
	s >> p.Y;
	s >> p.Z;
	return s;
}

Vector operator-(const Point& p1,const Point& p2)
//returns p1 - p2
{
	return Vector(p1.x()-p2.x(),p1.y()-p2.y(),p1.z()-p2.z());
}

Vector operator+(Vector & p1, Vector& p2)
{
	return Vector(p1.x()+p2.x(),p1.y()+p2.y(),p1.z()+p2.z());
}
Vector operator+(Point& p1, Point& p2)
//returns p1 + p2
{
	return Vector(p1.x()+p2.x(),p1.y()+p2.y(),p1.z()+p2.z());
}

Point operator+(Point& p1, Vector& p2)
//returns p1 + p2
{
	return Point(p1.x()+p2.x(),p1.y()+p2.y(),p1.z()+p2.z());
}

Vector add(Vector& p1, Vector& p2)
//returns p1 + p2
{
	return Vector(p1.x()+p2.x(),p1.y()+p2.y(),p1.z()+p2.z());}

Vector subtract(Vector& p1, Vector& p2)
//returns p1 + p2
{
	return Vector(p1.x()-p2.x(),p1.y()-p2.y(),p1.z()-p2.z());
}

float operator^(const Vector& u,const Vector& v)
//returns dot product
{
	return u.x()*v.x() + u.y()*v.y() + u.z()*v.z();
}

Vector operator*(Vector& v1, Vector& v2)
//returns cross product
{
	return Vector(v1.Y*v2.Z - v1.Z*v2.Y,v1.Z*v2.X - v1.X*v2.Z,v1.X*v2.Y - v1.Y*v2.X);
}
	
	
Vector operator*(Vector& v1, float a)
//vector by scalar
{
	return Vector(v1.X*a,v1.Y*a,v1.Z*a);
}
	

	
	
