#include "simplecamera.h"
#include "random.h"
#include <math.h>

void SimpleCamera::setVPWindow(float xmin, float xmax, float ymin, float ymax)
{
	Xmin = xmin;
	Xmax = xmax;
	Ymin = ymin;
	Ymax = ymax;
	Width = (Xmax-Xmin)/(float)Nx;
	Wo2 = Width/2.0f;
	Height = (Ymax-Ymin)/(float)Ny;
	Ho2 = Height/2.0f;
}    

SimpleCamera::SimpleCamera(int nx, int ny)
/*a new camera where the view plane is mapped to nx*ny pixels*/
//defaults: VPwindow from -1.0 to 1.0 in x and y
//          zcop is at 1.0 along positive z axis
//          the direction of view is therefore down negative z-axis
{
	Nx = nx;
	Ny = ny;
	Zcop = 1.0;
	setVPWindow(-1.0,1.0,-1.0,1.0);
}

void SimpleCamera::setResolution(int nx, int ny)
{
	Nx = nx;
	Ny = ny;
	Width = (Xmax-Xmin)/(float)Nx;
	Wo2 = Width/2.0f;
	Height = (Ymax-Ymin)/(float)Ny;
	Ho2 = Height/2.0f;

}

ostream& operator<<(ostream& s, SimpleCamera camera)
{
	s << "\nCamera:\n"
		<< "\nResolution: " << camera.Nx << "x" << camera.Ny
		<< "\nVP Window: [" 
		<< camera.Xmin << "," << camera.Xmax << "," << camera.Ymin << "," << camera.Ymax 
		<< "]"
		<< "\nCOP: (0,0," << camera.Zcop << ")";

	return s;
}


istream& operator>>(istream& s, SimpleCamera& camera)
{
	s >> camera.Nx >> camera.Ny 
		>> camera.Xmin >> camera.Xmax >> camera.Ymin >> camera.Ymax 
		>> camera.Zcop;

	return s;
}



Ray SimpleCamera::ray(int i, int j) const
{
	/*_invisible_*/
	Point pixel_pos(Xmin+(float)i*Width+Wo2, Ymin+(float)j*Height+Ho2, 2.0);

	Ray ray;
	Point cop(0.0,0.0,Zcop);
	//Point cop(Xmin+(float)i*Width+Wo2, Ymin+(float)j*Height+Ho2,Zcop);

	ray.origin() = cop;
	ray.direction() = pixel_pos - cop;

	return ray;
	/*_invisible_*/
}