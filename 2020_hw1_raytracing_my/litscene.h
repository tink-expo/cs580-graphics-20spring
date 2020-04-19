#define _LITSCENE

//Same as a scene but with lights

#ifndef _SCENE
#include "scene.h"
#endif

#ifndef _LIGHT
#include "light.h"
#endif

using namespace std;

class LitScene : public Scene{
	int NLights; //number of lights;
	Light *Lighting;
	Colour Ambient;
public:
	LitScene() : Scene(), Ambient(0.2,0.2,0.2) {NLights = 0;}
	
	void setLighting(int n) {NLights=n; Lighting = new Light[n];}
	
	void setAmbient(Colour colour) {Ambient = colour;}
	
	Light& lightAt(int i) {return Lighting[i];}
	
	int& numberOfLights() {return NLights;}
	
	Colour colourOnObject(GObject*,Point, Point);
	bool intersect(GObject*,Ray,Colour&,int);
		
	friend ostream& operator<<(ostream&,LitScene);  //writing
	friend istream& operator>>(istream&,LitScene&); //reading
};
	
	
