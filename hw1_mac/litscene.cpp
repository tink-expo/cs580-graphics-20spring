#include "litscene.h"
#include <math.h>
#include <algorithm>

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

Colour LitScene::colourOnObject(GObject *object, const Point& p, const Point& eye)
{
  Vector  n = object->normal(p); //normal vector
  Ray shadowray;
  shadowray.origin() = p;
  Colour colour_dummy;

  Colour colour;
  
  //compute ambient component
  Colour ka = object->material().ambient();
  colour = ka*Ambient;

  // Your code here! (in the loop below - some hints are given) 
  // Task 2, 3, 4 must be implemented in this for statement

  for (int i = 0; i < NLights; ++i) {

    Light light = lightAt(i);

    Vector light_dir;
    float light_dist_sq;
    if (!light.directional()) {
      light_dir = light.point() - p;
      light_dist_sq = light_dir.squarednorm();
      light_dir.normalise();
    } else {
      light_dir = light.vector();  // normalised
      light_dir = light_dir * (-1);
    }

    bool shadow = false;
    shadowray.direction() = light_dir;
    for (int i = 0; i < numObjects(); ++i) {
      float t;
      if (at(i) != object && at(i)->intersect(shadowray, t, colour_dummy) &&
          (light.directional() || (t > 0 && t * t <= light_dist_sq))) {
        shadow = true;
        break;
      }
    }

    if (!shadow) {
      if (!(object->material().diffuse() == Black)) {
        float diffuse_factor = clamp0to1(n ^ light_dir);
        Colour diffuse_colour = object->material().diffuse() * light.intensity();
        colour = colour + diffuse_colour * diffuse_factor;
      }

      if (!(object->material().specular() == Black)) {
        Vector view_dir = eye - p;
        view_dir.normalise();
        Vector half_dir = add(light_dir, view_dir);
        half_dir.normalise();
        float spec_factor = pow(clamp0to1(n ^ half_dir), object->material().shininess());
        Colour spec_colour = object->material().specular() * light.intensity();
        colour = colour + spec_colour * spec_factor;
      }
    }
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

  Colour colour_dummy;
  //for each object
  for (int i = 0; i < numObjects(); ++i) {
    float t;
    Colour col;
    if ((at(i) != me) && at(i)->intersect(ray, t, colour_dummy)) {
      //intersection found
      if ((t < tmin) && (t > 0.0)) {
        object = at(i);
        tmin = t;
        // colour = col;
        found = true;
      }
    }
  }
  if (found) {//an object has been found with the smallest intersection
    /*find the intersection point*/
    Point p = ray.pointAt(tmin);

    //now we want the colour computed at point p on the object
    colour = colourOnObject(object, p, ray.origin());

    //Your code here
    //Task - 5 -recursive ray tracing must be written here.
    if (!(object->material().specular() == Black)) {
      Ray refl_ray;
      refl_ray.origin() = p;

      Vector r_dir = ray.direction();
      r_dir.normalise();
      Vector n = object->normal(p);
      refl_ray.direction() = subtract(r_dir, n * (2.0f * (r_dir ^ n)));

      Colour refl_col;
      if (intersect(object, refl_ray, refl_col, depth - 1)) {
        colour = colour + refl_col * object->material().specular();
      }
    }

    colour.check();
    return true;
  }
  else
  {
    return false;
  }
}
