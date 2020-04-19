#include "ray.h"

ostream& operator<<(ostream& s, Ray ray)
//writing
{
  s << "{" << ray.Origin << "," << ray.Direction << "}";
  return s;
}

Point Ray::pointAt(float t)
//returns the point along the ray at t
{
  return Point(Origin.x() + t*Direction.x(),
    Origin.y() + t*Direction.y(),
    Origin.z() + t*Direction.z());
}

