#include <math.h>

#include "scene.h"

#ifndef _SPHERE
#include "sphere.h"
#endif

#ifndef _POLYGON
#include "polygon.h"
#endif

void Scene::set(int n)
{
  N = n;
  ObjectArray = new GObject*[n];
}

ostream& operator<<(ostream& s, Scene scene)
{
  int i;

  for (i = 0; i < scene.N; ++i) scene.ObjectArray[i]->print(s);

  return s;
}

istream& operator >> (istream& s, Scene& scene)
{
  int n;

  s >> n; //read first line of file which is number of objects

  scene.set(n); //initialise


  for (int i = 0; i < n; ++i) {
    int objectcode;
    s >> objectcode;

    /*I don't see any way around this*/
    switch (objectcode) {
    case 0: scene.ObjectArray[i] = new Sphere();
      scene.ObjectArray[i]->read(s);
      break;

    case 1: Polygon* poly = new Polygon();
      poly->read(s);
      poly->planeEquation();
      scene.ObjectArray[i] = poly;
      break;


      //add additional cases

    }
  }

  return s;
}



bool Scene::intersect(Ray ray, Colour& colour)
/*returns true if the ray intersects the scene, and if so then the colour
at the intersection point*/
{
  float tmin = 9999999999.9;
  bool found = false;

  //for each object
  for (int i = 0; i < N; ++i) {
    float t;
    Colour col;
    if (ObjectArray[i]->intersect(ray, t, col)) {
      //intersection found
      if (t < tmin && t>0.0) {
        tmin = t;
        colour = col;
        found = true;
      }
    }
  }
  return found;
}



ostream& Scene::print(ostream& s)
{
  int i;

  for (i = 0; i < N; ++i) ObjectArray[i]->print(s);

  return s;
}


istream& Scene::read(istream &s)
{
  int n;

  cout << "Type # of objects: " << endl;
  s >> n; //read first line of file which is number of objects

  set(n); //initialise


  for (int i = 0; i < n; ++i) {
    int objectcode;
    s >> objectcode;

    /*I don't see any way around this*/
    switch (objectcode) {
    case 0: ObjectArray[i] = new Sphere();
      ObjectArray[i]->read(s);
      break;

    case 1: Polygon* poly = new Polygon();
      poly->read(s);
      poly->planeEquation();
      ObjectArray[i] = poly;
      break;

      //add additional cases

    }
  }

  return s;
}




