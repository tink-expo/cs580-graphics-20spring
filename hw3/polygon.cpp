#include "math.h"
#include "random.h"
#include "polygon.h"

Polygon::Polygon(Material mat, int m) : GObject()
{
	material() = mat;
	
	N = 0; //number used so far
	Max = m; //max number
	P = new Point[m];
}

void Polygon::set(Point p)
{
	if(N<Max){
		P[N] = p;
		N++;
	}
}

Point Polygon::objectCentre(void)
{
	Vector p(0.0f,0.0f,0.0f);
	for (int i=0;i<numPoints();i++)
		p=p+Vector(point(i).x(),point(i).y(),point(i).z());
	return Point(p.x()/numPoints(),p.y()/numPoints(),p.z()/numPoints());
}

void Polygon::planeEquation(void)
/*this establishes the plane equation - must be used after points have been read in*/
{
	/*take first 3 points - assumes they are not colinear*/
	Vector va = P[1]-P[0];
	Vector vb = P[2]-P[0];
	
	/*cross product*/
	Nm = (va*vb).normalised(); 
	
	/*dot product*/
	D = Nm^(Vector &)P[0];
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
	return (Nm^(Vector &)p) - D;
}

float Polygon::evalPlaneEquation(Vector v)
//evaluates the plane equation at p
{
	/*ax+by+cz-d*/
	return (Nm^v) - D;
}
static float xVal(Point p, int s)
//returns the appropriate x-coordinate depending on s
{
	if(s==0) return p.y();
	else     return p.x();
}

static float yVal(Point p, int s)
//returns the appropriate y-coordinate
{
	if(s==2) return p.y();
	else     return p.z();
}

static float dxVal(Point p0, Point p1, int s)
{
	return xVal(p1,s) - xVal(p0,s);
}

static float dyVal(Point p0, Point p1, int s)
{
	return yVal(p1,s) - yVal(p0,s);
}

static float myabs(float x)
{
	if(x < 0.0) return -x;
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
	
	if(absB > absA){
		s = 1;
		max = absB;
	}
	if(absC > max){
		s = 2;
	}
	
	return s;
}

bool Polygon::intersect(Ray ray, float& t, Colour& colour)
//intersects ray with a polygon
{
  /*find the parametric value t at intersection point*/

  float denom = evalPlaneEquation(ray.direction()) + D;
  if(denom > -0.00000001) return false;

  t = -evalPlaneEquation(ray.origin())/denom;

  if (t < 0) return false;

  /*intersection point*/
  float x = ray.origin().x() + t*ray.direction().x();
  float y = ray.origin().y() + t*ray.direction().y();
  float z = ray.origin().z() + t*ray.direction().z();
  Point p(x,y,z);

  int test =0;

  /*now need to check if p is inside the polygon*/
  /*forget the coordinate with the biggest coefficient in the plane equation*/
  float a = Nm.x(); float b = Nm.y(); float c = Nm.z();

  int s = maxCoeff(a,b,c);
  x = xVal(p,s); //this is the representation of the point
  y = yVal(p,s);
  float dx,dy,x1,y1;

  for (int i=1; i<N; ++i)
  {
    //for each edge P[i-1] to P[i]
    //form the edge equation
    dx = dxVal(P[i-1],P[i],s);
    dy = dyVal(P[i-1],P[i],s);
    x1 = xVal(P[i-1],s);
    y1 = yVal(P[i-1],s);

    /*do the test*/
    if ((x-x1)*dy - (y-y1)*dx > 0.0)
      test--;
    else
      test++;
  }
  //now we have to do the last edge
  dx = dxVal(P[N-1],P[0],s);
  dy = dyVal(P[N-1],P[0],s);
  x1 = xVal(P[N-1],s);
  y1 = yVal(P[N-1],s);
  if ((x-x1)*dy - (y-y1)*dx > 0.0)
    test--;
  else
    test++;

  //if it survived all that then...
  return ((test==-N) || (test==N));	
}

ostream& operator<<(ostream& s, Polygon poly)
{
	s << "\nPolygon:" << "\n";
	s << "Number of points: " << poly.N << "\n";
	for(int i = 0; i < poly.N; ++i){
		s << "(" << i << ")" << poly.P[i] << "\n";
	}
	s << "Normal: " << poly.Nm << "\n";
	 
	s << "Material: " << poly.material();
	  
	return s;
}


istream& operator>>(istream& s, Polygon& poly)
{
	int m;
	s >> m; //number of points in this polygon
	poly.setMaxPoints(m);
	
	for(int i = 0; i<m; ++i){
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


float Polygon::PolygonSampling(Point& p,float s,float t, int Type = 0)
{
    std::cerr << "ERROR: Polygon::PolygonSampling, not implemented, exiting." << std::endl;
    exit(1);
}

bool Polygon::sample(Point& p, float& probability, const Point& from, float s, float t) 
{
    // depending on the number of vertices of polygon, select appropriate sampling scheme
    if (Max==3)			probability = TriangularSampling(p, s, t, 0);
    else if (Max==4)	probability = RectangularSampling(p, s, t, 0);
    else					probability = PolygonSampling(p, s, t, 0);
    return true; 
}

bool Polygon::isRefractive() {
    return false;
}

float Polygon::getRfrIndex() {
    return -1.;
}