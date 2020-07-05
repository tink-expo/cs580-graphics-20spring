#ifndef _SIMPLECAMERA
#define _SIMPLECAMERA

#include <iostream>
using namespace std;

#ifndef _RAY
#include "ray.h"
#endif

/*defines a simple camera, where the view-plane window is on
the XY plane, the centre of projection is along the z-axis,
the VP window is divided into rectangular cells, each corresponding
to a pixel. The centre of each cell is mapped to a pixel*/

class SimpleCamera{
	float Xmin, Xmax, Ymin, Ymax;	//view plane window
	float Width, Height, Wo2, Ho2;  //width and height of cell
	int Nx, Ny;			//number of subdivisions
	float Zcop;			//centre of projection along z-axis
	
public:
	SimpleCamera(int,int);
	
	void setVPWindow(float,float,float,float);
	void setResolution(int,int);
	
	int& xResolution() {return Nx;}
	int& yResolution() {return Ny;}
	float& xmin() {return Xmin;}
	float& xmax() {return Xmax;}
	float& ymin() {return Ymin;}
	float& ymax() {return Ymax;}
	float& zcop() {return Zcop;}
	
	Ray ray(int,int) const; //finds ray through pixel (i,j)
	Ray StratifiedRandomRay(int, int, int, int, double);
	
	friend ostream& operator<<(ostream&,SimpleCamera);  //writing
	friend istream& operator>>(istream&,SimpleCamera&); //reading
};
	
#endif