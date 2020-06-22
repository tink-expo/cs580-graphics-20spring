#define _GOBJECT

#include "brdf.h"

#ifndef _COLOUR
#include "colour.h"
#endif

#ifndef _MATERIAL
#include "material.h"
#endif

#ifndef _RAY
#include "ray.h"
#endif

#include <iostream>
using namespace std;

enum eObjectType {OBJECT_POLYGON,OBJECT_SPHERE};
ostream& operator<<(ostream& s, eObjectType ot);

//an abstract class
class GObject{
	Material Mat;	//the material reflectance properties of the object
	bool m_isEmitter;
  bool m_isRefractive;
	baseBRDF* m_brdf;
public:
	GObject() : Mat(),m_isEmitter(false),m_brdf(NULL),m_isRefractive(false) {};

	//---------------------------------
	// specific path tracing extensions
	baseBRDF* brdf(void) {return m_brdf;};
  void setBrdf(baseBRDF* b) {if (m_brdf) delete m_brdf;m_brdf=b;};
  virtual bool isRefractive(void) = 0;
  virtual float getRfrIndex(void) = 0;
	//---------------------------------

	Material& material() {return Mat;}	
	
	bool& isEmitter(void) {return m_isEmitter;}

	virtual bool intersect(Ray ray, float& t, Colour& colour) = 0;
	virtual Vector normal(Point) = 0;
	virtual Point objectCentre(void) = 0;

	virtual eObjectType objectType(void) const = 0;
	
	virtual void print(ostream&) = 0;
	
	virtual bool sample(Point& p,float& pdf,const Point& from,float s,float t)=0;

	//for each different kind of object need one of these
	virtual void read(istream&) = 0;
};
