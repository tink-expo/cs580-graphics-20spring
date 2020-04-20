#include "litscene.h"
#include <math.h>

namespace {

float clamp0to1(float val)
{
  return max(0.0f, min(val, 1.0f));
}

}  // namespace


ostream& operator<<(ostream& s, LitScene scene)
{
  scene.print(s);

  s << "\n\nLighting:\n";

  int i;
  s << "Ambient: " << scene.Ambient << "\n";
  for (i = 0; i < scene.numberOfLights(); ++i) s << "Light: " << i << " " << scene.lightAt(i) << "\n";

  return s;
}




istream& operator >> (istream& s, LitScene& scene)
//reading
{
  scene.read(s);

  //read the ambient lighting must always be there
  s >> scene.Ambient;

  //read how many lights
  int n; s >> n;

  if (n == 0) return s;

  scene.setLighting(n);

  Light light; int i;
  for (i = 0; i < n; ++i) {
    s >> light;
    scene.lightAt(i) = light;
  }
  return s;
}

static float power(float x, int n)
{
  if (n < 1) return 1.0;
  if (n == 1) return x;
  else return x * power(x, n - 1);
}

Colour LitScene::colourOnObject(GObject *object, Point p, Point eye)
{
  bool shadow = false;
  Vector  n = object->normal(p); //normal vector
  Ray shadowray;

  Colour colour;


  //compute ambient component
  Colour ka = object->material().ambient();
  colour = ka*Ambient;

  // Your code here! (in the loop below - some hints are given) 
  // Task 2, 3, 4 must be implemented in this for statement

  for (int i = 0; i < NLights; ++i) {

    Light light = lightAt(i);

    Vector light_dir;
    float light_sq_dist;
    if (!light.directional()) {
      Vector light_dir_sized = light.point() - p;
      light_dir = light_dir_sized.normalised();
      light_sq_dist = light_dir_sized.squarednorm();
    } else {
      light_dir = light.vector();  // normalised
      light_dir = light_dir * (-1);
    }

    float diffuse_factor = clamp0to1(n ^ light_dir);
    if (!light.directional()) {
      diffuse_factor /= light_sq_dist;
    }
    Colour diffuse_colour = object->material().diffuse() * light.intensity();
    diffuse_colour = diffuse_colour * diffuse_factor;

    Vector view_dir = (eye - p).normalised();
    Vector half_dir = (add(light_dir, view_dir)).normalised();
    float spec_factor = power(clamp0to1(n ^ half_dir), object->material().shininess());
    if (!light.directional()) {
      spec_factor /= light_sq_dist;
    }
    Colour spec_colour = object->material().specular() * light.intensity();
    spec_colour = spec_colour * spec_factor;

    colour = colour + diffuse_colour;
    colour = colour + spec_colour;
  }
  // No more code to add after here! 
  //check that the colour is in the bounds 0 to 1
  colour.check();


  return colour;
}




bool LitScene::intersect(GObject* me, Ray ray, Colour& colour, int depth)
/*returns true if the ray intersects the scene, and if so then the colour
at the intersection point. This overrides the method in Scene*/
{
  float tmin = 9999999999.9;
  bool found = false;
  GObject* object = NULL;
  Ray reflected;
  if (depth == 0) return false; /* end of recursion */


//for each object
  for (int i = 0; i < numObjects(); ++i) {
    float t;
    Colour col;
    if ((at(i) != me) && at(i)->intersect(ray, t, col)) {
      //intersection found
      if ((t < tmin) && (t > 0.0)) {
        object = at(i);
        tmin = t;
        colour = col;
        found = true;
      }
    }
  }
  if (found) {//an object has been found with the smallest intersection
    Colour refl_col;
    /*find the intersection point*/
    Point p = ray.pointAt(tmin);

    //Your code here
    //Task - 5 -recursive ray tracing must be written here.

    //now we want the colour computed at point p on the object
    colour = colourOnObject(object, p, ray.origin());

    colour.check();
    return true;
  }
  else
  {
    return false;
  }
}
