#include "point.h"


Point::Point(float x, float y, float z)
//constructor
{
  X = x;
  Y = y;
  Z = z;
}

Point::Point(const Point& p)
//initialisation from another point
{
  X = p.X;
  Y = p.Y;
  Z = p.Z;
}

Point& Point::operator=(const Point& p)
//assignment
{

  if (this == &p) return *this;

  X = p.X;
  Y = p.Y;
  Z = p.Z;

  return *this;
}

ostream& operator<<(ostream& s, Point p)
//writing
{
  s << "(" << p.X << "," << p.Y << "," << p.Z << ")";
  return s;
}



istream& operator >> (istream& s, Point& p)
//reading
{
  s >> p.X;
  s >> p.Y;
  s >> p.Z;
  return s;
}

Vector Point::operator-(const Point& p)
{
  Vector v;

  v.x() = X - p.x();
  v.y() = Y - p.y();
  v.z() = Z - p.z();

  return v;
}
