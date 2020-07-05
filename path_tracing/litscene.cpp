#include "litscene.h"
#include "random.h"
#include <math.h>
#include <limits>
#include <fstream>

bool LitScene::load(const char* filename)
{
	std::fstream fin(filename,std::ios::in|std::ios::binary);
	if (fin.is_open())
	{
		// read data
		fin >> *this;
		if (fin.fail())
		{
			std::cerr << "ERROR: LitScene::load, error loading file: " << filename << std::endl;
			fin.close();
			return false;
		}
		fin.close();
		return true;
	}
	else
	{
		std::cerr << "ERROR: LitScene::load, could not open file: " << filename << std::endl;
		return false;
	}
}

ostream& operator<<(ostream& s, LitScene scene)
{
	scene.print(s);
	s << "\n\nLighting:\n";

	int i;
	s << "Ambient: " << scene.Ambient <<"\n";
	for(i=0;i<scene.numberOfLights();++i) s << "Light: " << i << " " << scene.lightAt(i) << "\n";
	for(i=0;i<scene.numberOfAreaLights();++i)
	{
		s << "Area light: " << i << " type: " << scene.areaLightAt(i)->objectType() << " ";
		scene.areaLightAt(i)->print(s);
		s << "\n";
	}

	return s;
}

istream& operator>>(istream& s, LitScene& scene)
//reading
{
	scene.read(s);

	char c=skipWhiteSpace(s);
	if (c!='a')
	{
		cerr << "ERROR: expected ambient code 'a', got unknown code '" << c << "', exiting" << endl;
		exit(1);
	}

	// read the ambient lighting must always be there
	s >> scene.Ambient;

	// read how many lights
	c=skipWhiteSpace(s);
	char d;s>>d;
	if (c!='l'||d!='c')
	{
		cerr << "ERROR: expected light count code 'lc', got unknown code '" << c << d << "', exiting" << endl;
		exit(1);
	}

	int n;
	s >> n;

	if(n==0) return s;

	scene.setLighting(n);

	Light light; int i;		
	for(i=0;i<n;++i)
	{
		c=skipWhiteSpace(s);s>>d;
		if (c!='p'||d!='l')
		{
			cerr << "ERROR: expected point light code 'pl', got unknown code '" << c << d << "', exiting" << endl;
			exit(1);
		}
		s >> light;
		scene.lightAt(i) = light;
	}

	// construct list of area lights
	scene.buildAreaLightList();
	return s;
}

static float power(float x, int n)
{
	if (n < 1) return 1.0;
	if (n == 1) return x;
	else return x * power(x, n - 1);
}

//

Colour LitScene::colourOnObject(GObject *object, Point p, Point eye)
{
	bool shadow = false;
	Vector  n = object->normal(p); //normal vector
	Ray shadowray;

	Colour colour;

	//compute ambient component
	Colour ka = object->material().ambient();
	colour = ka*Ambient;

  //=============================PREV-HW1===============================
  //You need to fill this part with the implementation of your PREV-HW1.
  for(int i=0; i<NLights; ++i){

      Light light = lightAt(i);


  }
  // No more code to add after here! 
  //check that the colour is in the bounds 0 to 1
  colour.check();


  return colour;
}

GObject* LitScene::intersect(Ray ray,Point& p,Vector& n)
{
	double tmin =1E+20;
	bool found = false;GObject* object = NULL;
	//for each object
	for (int i=0; i<numObjects(); ++i)
	{
		float t;
		Colour col;
		if (at(i)->intersect(ray,t,col))
		{
			//intersection found
			if ( (t<tmin) && (t>0.0f) )
			{
				object= at(i);
				tmin = t;
				found = true;				
			}
		} 
	}

	if (found)
	{
		p = ray.pointAt((float)tmin);
		n = object->normal(p);
		return object;
	}

	return NULL;
}

bool LitScene::intersect(GObject* me, Ray ray, Colour& colour, int depth)
/**returns true if the ray intersects the scene, and if so then the colour
at the intersection point. This overrides the method in Scene**/
{
	double tmin =1E+20;
	bool found =false;
	GObject* object =NULL;
	Ray reflected;
	if (depth==0) return false; /* end of recursion */

	//for each object
	for (int i=0; i<numObjects(); ++i)
	{
		float t;
		Colour col;
		if ((at(i)!= me) && at(i)->intersect(ray,t,col))
		{
			//intersection found
			if ( (t<tmin) && (t>0.0f) )
			{
				object = at(i);
				tmin = t;
				colour = col;
				found = true;
			}
		} 
	}

  if(found){//an object has been found with the smallest intersection
      Colour refl_col;
      /*find the intersection point*/
      Point p = ray.pointAt(tmin);

      //=============================PREV-HW1===============================
      //You need to fill this part with the implementation of your PREV-HW1.

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

bool LitScene::hitLight(GObject* me, Ray ray)
{
	double tmin = 1E+20;
	bool found = false;
	GObject* object = NULL;

	//for each object
	for(int i = 0; i < numObjects(); ++i)
	{
		float t;
		Colour col;
		if((at(i) != me) && at(i)->intersect(ray,t,col)){
			//intersection found
			if((t < tmin) && (t > 0.0) ){
				object = at(i);
				tmin = t;
				found = true;
			}
		} 
	}
	if(found)
	{  
		return object->isEmitter();
	}
	else 
	{
		return false;
	}
}

GObject* LitScene::isShadowed(GObject* me, Ray ray,bool directional)
/*returns true if the ray intersects the scene, and if so then the colour
at the intersection point. This overrides the method in Scene*/
{
	double tmin = 1e+20;
	if (directional) tmin = 1e+20;
	bool found = false;
	GObject* object = NULL;
	Ray reflected;

	//for each object
	for (int i=0; i<numObjects(); ++i)
	{
		float t;
		Colour col;
		if ((at(i)!= me) && at(i)->intersect(ray,t,col))
		{
			//intersection found
			if ( (t<tmin) && (t>0.0) )
			{
				object = at(i);
				tmin = t;
				found = true;
			}
		} 
	}

	if (found) return object;
	
	return NULL;
}

void LitScene::buildAreaLightList(void)
{
	// clear current list
	if (m_areaLights) delete [] m_areaLights;
	m_areaLights=NULL;
	m_areaLightCount=0;

	// count the number of emitting objects
	for (int i=0; i<numObjects(); i++)
	{
		if (at(i))
			if (at(i)->isEmitter())
				m_areaLightCount++;
	}

	// allocate new array
	m_areaLights=new GObject*[m_areaLightCount];
	if (!m_areaLights) exit(1);

	// add emitters to array
	int j=0;
	for (int i=0; i<numObjects(); i++)
	{
		if (at(i))
			if (at(i)->isEmitter())
			{
				m_areaLights[j++]=at(i);
			}
	}
} // buildAreaLightList