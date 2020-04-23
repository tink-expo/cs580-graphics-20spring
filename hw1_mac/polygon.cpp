#include "math.h"
#include <algorithm>

#include "polygon.h"

namespace {

bool pointOn(const Vector& r_dir, const Point& r_origin, const Point& p, float& t)
{
  // assert r_dir is normalised.
  Vector p_dir = p - r_origin;
  t = r_dir.x() != 0 ? 
      p_dir.x() / r_dir.x() :
      (r_dir.y() != 0 ?
      p_dir.y() / r_dir.y() :
      p_dir.z() / r_dir.z());
  p_dir.normalise();

  return r_dir == p_dir ? t >= 0 : false;
}

Point add(const Point& p, const Vector& v) {
  Point q = p;
  q.x() += v.x();
  q.y() += v.y();
  q.z() += v.z();
  return q;
}

float intersectLineseg(const Point& p1, const Point& p2, const Ray& ray)
{
  Vector r_dir = ray.direction();
  float r_norm = r_dir.norm();
  if (r_norm == 0) {
    return (p1 == ray.origin() || p2 == ray.origin()) ? 0 : -1;
  }
  r_dir.normalise();

  float t1;
  float t2;
  bool b1 = pointOn(r_dir, ray.origin(), p1, t1);
  bool b2 = pointOn(r_dir, ray.origin(), p2, t2);
  if (b1 && b2) {
    return min(t1, t2);
  } else if (b1 && !b2) {
    return t1;
  } else if (!b1 && b2) {
    return t2;
  }

  Vector p_sub = p2 - p1;
  Vector o_to_p = p2 - ray.origin();
  float numer = (p_sub * o_to_p).norm();
  if (numer == 0) {
    return -1;
  }
  float denom = (p_sub * r_dir).norm();
  if (denom == 0) {
    return -1;
  }
  float t = numer / denom;
  Point q = add(ray.origin(), r_dir * t);
  if (q == p1 || q == p2) {
    return t;
  }

  float t_dummy;
  Vector pq = q - p1;
  pq.normalise();
  if (!pointOn(pq, p1, q, t_dummy)) {
    return -1;
  }
  pq = q - p2;
  pq.normalise();
  if (!pointOn(pq, p2, q, t_dummy)) {
    return -1;
  }
  return t / r_norm;
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

float Polygon::evalPlaneEquation(const Vector& v)
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

bool Polygon::intersect(const Ray& ray, float& t, Colour& colour)
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
    if (t_numer != 0) {
      return false;

    } else {
      if (pointInside(ray.origin())) {
        t = 0;
        return true;

      } else {
        t = -1;
        for (int i = 0; i < N; ++i) {
          float t_lineseg = 
              intersectLineseg(P[i], P[i + 1 < N ? i + 1 : 0], ray);
          if (t_lineseg >= 0) {
            t = t < 0 ? t_lineseg : min(t, t_lineseg);
          }
        }
        return t >= 0;

      }
    }
  }

  t = t_numer / t_denom;
  if (t < 0) {
    return false;
  }

  Point q = ray.pointAt(t);

  
  // colour = this->material().ambient();
  return pointInside(ray.pointAt(t));
}

int Polygon::addCrossSign(const Point& p1, const Point& p2, const Point& q)
{
  Vector cross = (p1 - q) * (p2 - q);
  Point added(q.x() + cross.x(), q.y() + cross.y(), q.z() + cross.z());
  float eval = evalPlaneEquation(added);
  return eval > 0 ? 1 : (eval < 0 ? -1 : 0);
}

bool Polygon::pointInside(const Point& q)
{
  int bm_sign = addCrossSign(P[N - 1], P[0], q);
  if (bm_sign != 0) {
    for (int i = 0; i < N - 1; ++i) {
      int cmp_sign = addCrossSign(P[i], P[i + 1], q);
      if (bm_sign * cmp_sign < 0) {
        return false;
      }
    }
  }
  return true;
}

