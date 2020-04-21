#include "math.h"

#include "sphere.h"

Sphere::Sphere(Material mat, Point centre, float radius) : GObject()
{
  material() = mat;

  Centre = centre;
  Radius = radius;
}

ostream& operator<<(ostream& s, Sphere sphere)
{
  s << "\nSphere:" << "\n"
    << "Centre: " << sphere.Centre << " "
    << "Radius: " << sphere.Radius << "\n"
    << "Material: " << sphere.material();

  return s;
}


istream& operator >> (istream& s, Sphere& sphere)
{
  s >> sphere.Centre >> sphere.Radius >> sphere.material();
  return s;
}

void Sphere::print(ostream &s)
{
  s << *this;
}

void Sphere::read(istream& s)
{
  s >> *this;
}

bool Sphere::intersect(Ray ray, float& t, Colour& colour)
{

  //Your code here...Task - 1. Implement the intersection test
  //This function returns 'false' for now.
  //Your job is to fix it by some math you've learned from the course
  //It should return true if a given ray is intersect with the sphere
  //Update t and colour, which are the magnitude of the ray(vector) and the colour of intersecting point, respectively.

  float dx = ray.direction().x();
  float dy = ray.direction().y();
  float dz = ray.direction().z();
  float px = ray.origin().x();
  float py = ray.origin().y();
  float pz = ray.origin().z();
  float cx = Centre.x();
  float cy = Centre.y();
  float cz = Centre.z();

  float quad_a = dx * dx + dy * dy + dz * dz;
  if (quad_a == 0) {
    if ((ray.origin() - Centre).squarednorm() == Radius * Radius) {
      t = 0;
      colour = this->material().ambient();
      return true;
    } else {
      return false;
    }
  }

  float quad_b = 2 * dx * (px - cx) 
      + 2 * dy * (py - cy) 
      + 2 * dz * (pz - cz);
  float quad_c = (px - cx) * (px - cx) 
      + (py - cy) * (py - cy) 
      + (pz - cz) * (pz - cz) 
      - Radius * Radius;
  
  float sqrt_inner = quad_b * quad_b - 4 * quad_a * quad_c;
  if (sqrt_inner < 0) {
    return false;
  }
  t = (-quad_b - static_cast<float>(sqrt(sqrt_inner))) / (2 * quad_a);
  if (t < 0) {
    t = (-quad_b + static_cast<float>(sqrt(sqrt_inner))) / (2 * quad_a);
    if (t < 0) {
      return false;
    }
  }
  // colour = this->material().ambient();
  return true;
}




