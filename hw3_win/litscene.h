#define _LITSCENE

//Same as a scene but with lights

#ifndef _SCENE
#include "scene.h"
#endif

#ifndef _LIGHT
#include "light.h"
#endif

#ifndef _SIMPLECAMERA
#include "simplecamera.h"
#endif

#include "random.h"

class LitScene : public Scene{
	int NLights; //number of lights;
	Light *Lighting;
	Colour Ambient;
	int m_areaLightCount;
	GObject** m_areaLights;
	int m_maxBounces;
	float m_cutOffThreshold;
private:
	void buildAreaLightList(void);
public:
	LitScene(unsigned int maxBounces) : Scene(), Ambient(0.2f,0.2f,0.2f), m_areaLightCount(0), m_areaLights(0), m_maxBounces((int)maxBounces), m_cutOffThreshold(0.00001f)
	{
		NLights = 0;
	}

	LitScene() : Scene(), Ambient(0.2f,0.2f,0.2f), m_areaLightCount(0), m_areaLights(0), m_maxBounces(20), m_cutOffThreshold(0.00001f)
	{
		NLights = 0;
	}
	
	void setLighting(int n) {NLights=n; Lighting = new Light[n];}
	
	void setAmbient(Colour colour) {Ambient = colour;}
	
	Light& lightAt(int i) {return Lighting[i];}
	
	int& numberOfLights() {return NLights;}

	//---------------------------------
	// specific path tracing extensions
	Colour tracePath(const Ray& ray,GObject* object=NULL,Colour weightIn=Colour(1.0f,1.0f,1.0f),int depth=0);

	bool hitLight(GObject* me, Ray ray);
	Colour renderPixel(int i,int j,SimpleCamera& TheCamera,int N_RAYS_PER_PIXEL);

	int& numberOfAreaLights() {return m_areaLightCount;}
	GObject* areaLightAt(int i) {if (i>=m_areaLightCount) exit(1);return m_areaLights[i];};
	int& maxBounces(void) {return m_maxBounces;};
	float& cutOffThreshold(void) {return m_cutOffThreshold;};
	// --------------------------------
	
	Colour falseColour(const Ray& ray);
	Colour colourOnObject(GObject*,Point, Point);
	bool intersect(GObject*,Ray,Colour&,int);
	GObject* intersect(Ray ray,Point& p,Vector& n);
	GObject* isShadowed(GObject* me, Ray ray,bool directional);
	bool load(const char* filename);
	friend ostream& operator<<(ostream&,LitScene);  //writing
	friend istream& operator>>(istream&,LitScene&); //reading
};
	
	
