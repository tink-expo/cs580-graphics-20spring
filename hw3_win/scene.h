#define _SCENE

#ifndef _GOBJECT
#include "gobject.h"
#endif

#ifndef _RAY
#include "ray.h"
#endif

char skipWhiteSpace(istream& s);

class Scene{//an array of GObjects
	int N;
	GObject* *ObjectArray;
public:
	Scene() {N = 0; ObjectArray = 0;}
	Scene(int n) {N = n; ObjectArray = new GObject*[n];}
	
	void set(int n);
	
	void resize(int n);
	
	int numObjects() {return N;}
	
	GObject* at(int i) {return ObjectArray[i];}
	void setAt(int i,GObject* ptr)
	{
		if (i<N)
		{
			if (ObjectArray[i]) delete ObjectArray[i];
			ObjectArray[i]=ptr;
		}
	}
	
	bool intersect(Ray,Colour&);
	
	ostream& print(ostream& s);
	istream& read(istream& s);
	
	friend void setBRDF(Scene& scene,int i);
	friend ostream& operator<<(ostream&,Scene);  //writing
	friend istream& operator>>(istream&,Scene&); //reading
};
