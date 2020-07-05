#ifndef _COLOUR
#include "colour.h"
#endif

Colour::Colour(float red, float green, float blue)
{
  Red = red;
  Green = green;
  Blue = blue;
}

Colour::Colour(const Colour& colour)
{
  Red = colour.Red;
  Green = colour.Green;
  Blue = colour.Blue;
}

Colour& Colour::operator=(const Colour& colour)
{
  Red = colour.Red;
  Green = colour.Green;
  Blue = colour.Blue;

  return *this;
}

bool Colour::operator==(const Colour& colour) const
{
  return Red == colour.Red && Green == colour.Green && Blue == colour.Blue;
}

void Colour::reset(float red, float green, float blue)
{
  Red = red;
  Green = green;
  Blue = blue;
}


Colour operator*(const Colour& a, const Colour& b)
{
  return Colour(a.Red*b.Red, a.Green*b.Green, a.Blue*b.Blue);
}

Colour operator*(const Colour& a, float x)
{
  return Colour(a.Red*x, a.Green*x, a.Blue*x);
}

inline void setinterval(float& f)
{
  if (f < 0.0) f = 0.0;
  else
    if (f > 1.0) f = 1.0;
}

void Colour::check()
//checks if colour in range 0 to 1 and changes if not
{
  setinterval(Red);
  setinterval(Green);
  setinterval(Blue);
}

Colour operator+(const Colour& a, const Colour& b)
{
  return Colour(a.Red + b.Red, a.Green + b.Green, a.Blue + b.Blue);
}


ostream& operator<<(ostream& s, const Colour& colour)
//writing
{
  s << "(" << colour.Red << "R"
    << "," << colour.Green << "G"
    << "," << colour.Blue << "B"
    << ")";

  return s;
}

istream& operator >> (istream& s, Colour& colour)
//reading
{
  s >> colour.Red >> colour.Green >> colour.Blue;

  return s;
}
