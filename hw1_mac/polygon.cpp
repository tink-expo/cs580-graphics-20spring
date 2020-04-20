#include "math.h"

#include "polygon.h"

namespace {

float ClockwisednessProjZ(
    Point& p1, Point& p2, Point& p3) {
  return (p3.x() - p2.x()) * (p1.y() - p2.y()) -
      (p3.y() - p2.y()) * (p1.x() - p2.x());
}

float ClockwisednessProjX(
    Point& p1, Point& p2, Point& p3) {
  return (p3.y() - p2.y()) * (p1.z() - p2.z()) -
      (p3.z() - p2.z()) * (p1.y() - p2.y());
}

}  // namespace

Polygon::Polygon(Material mat, int m) : GObject()
{
  material() = mat;

  N = 0; //number used so far
  Max = m; //max number
  P = new Point[m];
}

void Polygon::set(Point p)
{
  if (N < Max) {
    P[N] = p;
    N++;
  }
}


void Polygon::planeEquation(void)
/*this establishes the plane equation - must be used after points have been
read in*/
{
  /*take first 3 points - assumes they are not colinear*/
  Vector va = P[1] - P[0];
  Vector vb = P[2] - P[0];

  /*cross product*/
  Nm = (va*vb).normalised();

  /*dot product*/
  D = Nm ^ (Vector &)P[0];
}


void Polygon::planeEquation(Vector& normal, float &d)
//this simply returns the plane equation
{
  normal = Nm;
  d = D;
}


float Polygon::evalPlaneEquation(Point p)
//evaluates the plane equation at p
{
  /*ax+by+cz-d*/
  return (Nm ^ (Vector &)p) - D;
}

float Polygon::evalPlaneEquation(Vector v)
//evaluates the plane equation at p
{
  /*ax+by+cz-d*/
  return (Nm^v) - D;
}

ostream& operator<<(ostream& s, Polygon poly)
{
  s << "\nPolygon:" << "\n";
  s << "Number of points: " << poly.N << "\n";
  for (int i = 0; i < poly.N; ++i) {
    s << "(" << i << ")" << poly.P[i] << "\n";
  }
  s << "Normal: " << poly.Nm << "\n";

  s << "Material: " << poly.material();

  return s;
}


istream& operator >> (istream& s, Polygon& poly)
{
  int m;
  s >> m; //number of points in this polygon
  poly.setMaxPoints(m);

  for (int i = 0; i < m; ++i) {
    Point p;
    s >> p;
    poly.set(p);
  }

  s >> poly.material();
  return s;
}

void Polygon::print(ostream &s)
{
  s << *this;
}

void Polygon::read(istream& s)
{
  s >> *this;
}

static float myabs(float x)
{
  if (x < 0.0) return -x;
  else return x;
}

static int maxCoeff(float a, float b, float c)
//returns 0, 1 or 2 depending on whether |a|, |b|, or |c| is max
{
  float absA = myabs(a);
  float absB = myabs(b);
  float absC = myabs(c);
  int s = 0;
  float max = absA;

  if (absB > absA) {
    s = 1;
    max = absB;
  }
  if (absC > max) {
    s = 2;
  }

  return s;
}

static float xVal(Point p, int s)
//returns the appropriate x-coordinate depending on s
{
  if (s == 0) return p.y();
  else     return p.x();
}

static float yVal(Point p, int s)
//returns the appropriate y-coordinate
{
  if (s == 2) return p.y();
  else     return p.z();
}

static float dxVal(Point p0, Point p1, int s)
{
  return xVal(p1, s) - xVal(p0, s);
}

static float dyVal(Point p0, Point p1, int s)
{
  return yVal(p1, s) - yVal(p0, s);
}

bool Polygon::intersect(Ray ray, float& t, Colour& colour)
{
  //Your code here...Task - 1. Implement the intersection test
  //This function returns 'false' for now.
  //Your job is to fix it by some math you've learned from the course
  //It should return true if a given ray is intersect with the polygon
  //Update t and colour, which are the magnitude of the ray(vector) and the colour of intersecting point, respectively.
  if (N < 3) {
    return false;
  }

  float t_numer = -evalPlaneEquation(ray.origin());
  float t_denom = evalPlaneEquation(ray.direction()) + D;
  if (t_denom == 0) {
    return false;
  }
  t = t_numer / t_denom;

  Point q = ray.pointAt(t);
  float clock_z = ClockwisednessProjZ(P[N - 1], q, P[0]);
  if (clock_z == 0) {
    float clock_x = ClockwisednessProjX(P[N - 1], q, P[0]);
    for (int i = 0; i < N - 1; ++i) {
      if (clock_x * ClockwisednessProjX(P[i], q, P[i + 1]) < 0) {
        return false;
      }
    }

  } else {
    for (int i = 0; i < N - 1; ++i) {
      if (clock_z * ClockwisednessProjZ(P[i], q, P[i + 1]) < 0) {
        return false;
      }
    }
  }
  // colour = Colour(1.0, 1.0, 0.0);
  colour = this->material().ambient();
  return true;
}

