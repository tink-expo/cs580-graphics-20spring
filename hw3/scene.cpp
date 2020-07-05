#include <math.h>

#include "scene.h"
#include <limits>

#ifndef _SPHERE
#include "sphere.h"
#endif

#ifndef _POLYGON
#include "polygon.h"
#endif

#include <GMmatlib.h>

void Scene::set(int n) 
{
	N = n; 
	ObjectArray = new GObject*[n];
}
void Scene::resize(int n)
{
	if (n==N) return;
	int num=(*this).numObjects();
	GObject** tempArray = new GObject*[n];
	for (int i=0;i<n;i++)
	{	
		if (i<num)
			tempArray[i]=ObjectArray[i];
		else
			tempArray[i]=NULL;
	}
	N=n;
	delete [] ObjectArray;
	ObjectArray=tempArray;
};

ostream& operator<<(ostream& s, Scene scene)
{
	for (int i=0; i<scene.N; ++i)
		scene.ObjectArray[i]->print(s);
	
	return s;
}

void setBRDF(Scene& scene,int i)
{
	// set BRDF properties
	if (scene.ObjectArray[i]->material().specular().sum()>0.e7f)
	{	
		// phong brdf material
		phongBRDF* brdf=new phongBRDF(scene.ObjectArray[i]->material().diffuse(),scene.ObjectArray[i]->material().specular(),scene.ObjectArray[i]->material().shininess(),scene.ObjectArray[i]->material().emission());
		scene.ObjectArray[i]->setBrdf(brdf);
	}
	else
	{
		// ideal diffuse brdf
		lambertianBRDF* brdf=new lambertianBRDF(scene.ObjectArray[i]->material().diffuse(),scene.ObjectArray[i]->material().emission());
		scene.ObjectArray[i]->setBrdf(brdf);
	}

	if (scene.ObjectArray[i]->brdf()->emission().sum()>0.e7f)
		scene.ObjectArray[i]->isEmitter()=true;
	else
		scene.ObjectArray[i]->isEmitter()=false;
}

istream& operator>>(istream& s, Scene& scene)
{
	int n;
	
	char c=skipWhiteSpace(s);
	char d;s>>d;
	if (c!='o'||d!='c')
	{
		cerr << "ERROR: expected object count code 'oc', got unknown code '" << c << d << "', exiting" << endl;
		exit(1);
	}

	s>>n; //read first line of file which is number of objects
	
	scene.set(n); //initialise

	for(int i=0; i<n; ++i)
	{
		char c=skipWhiteSpace(s);
		if (c!='s'&&c!='p'&&c!='b')
		{
			cerr << "ERROR: expected object code 's', 'b' or 'p', got unknown code '" << c << "', exiting" << endl;
			exit(1);
		}

		/*I don't see any way around this*/
		switch(c)
		{
		case 's':
			{
				scene.ObjectArray[i] = new Sphere();
				scene.ObjectArray[i]->read(s);
				setBRDF(scene,i);
				break;
			}				 
		case 'p':
			{
				Polygon* poly = new Polygon();
				poly->read(s);
				poly->planeEquation();
				scene.ObjectArray[i] = poly;
				setBRDF(scene,i);
				break;
			}			 
		case 'b':
			{
				n+=5;
				//n+=1;
				scene.resize(n);

				Point c;
				Vector sz,rot;
				Material mat;
				s >> c >> sz >> rot;
				s >> mat;

				// points
				Point u0=c+Vector(sz.x(),sz.y(),sz.z());
				Point u1=c+Vector(-sz.x(),sz.y(),sz.z());
				Point u2=c+Vector(-sz.x(),sz.y(),-sz.z());
				Point u3=c+Vector(sz.x(),sz.y(),-sz.z());
				Point l0=c+Vector(sz.x(),-sz.y(),sz.z());
				Point l1=c+Vector(-sz.x(),-sz.y(),sz.z());
				Point l2=c+Vector(-sz.x(),-sz.y(),-sz.z());
				Point l3=c+Vector(sz.x(),-sz.y(),-sz.z());

				// rotate points
				GPVector U(1.0f,0.0f,0.0f,1.0f);
				GPVector V(0.0f,1.0f,0.0f,1.0f);
				GPVector W(0.0f,0.0f,1.0f,1.0f);
				GPMatrix mX;mX.IdentityMatrix();mX.RotateXMatrix((float)rot.x()*((float)PI)/180.0f);
				GPMatrix mY;mY.IdentityMatrix();mY.RotateYMatrix((float)rot.y()*((float)PI)/180.0f);
				GPMatrix mZ;mZ.IdentityMatrix();mZ.RotateZMatrix((float)rot.z()*((float)PI)/180.0f);
				U=U*mX*mY*mZ;V=V*mX*mY*mZ;W=W*mX*mY*mZ;
				GPMatrix ml2wc;ml2wc.IdentityMatrix();ml2wc.local2WCxForm(GPVector3(U.x/U.w,U.y/U.w,U.z/U.w),GPVector3(V.x/V.w,V.y/V.w,V.z/V.w),GPVector3(W.x/W.w,W.y/W.w,W.z/W.w));
				GPMatrix c2orig;c2orig.IdentityMatrix();c2orig.TranslateMatrix(-c.x(),-c.y(),-c.z());
				GPMatrix orig2c;orig2c.IdentityMatrix();orig2c.TranslateMatrix(c.x(),c.y(),c.z());
				GPVector tmp;
				tmp=GPVector(u0.x(),u0.y(),u0.z(),1.0f)*(c2orig*ml2wc*orig2c);
				u0=Point(tmp.x/tmp.w,tmp.y/tmp.w,tmp.z/tmp.w);
				tmp=GPVector(u1.x(),u1.y(),u1.z(),1.0f)*(c2orig*ml2wc*orig2c);
				u1=Point(tmp.x/tmp.w,tmp.y/tmp.w,tmp.z/tmp.w);
				tmp=GPVector(u2.x(),u2.y(),u2.z(),1.0f)*(c2orig*ml2wc*orig2c);
				u2=Point(tmp.x/tmp.w,tmp.y/tmp.w,tmp.z/tmp.w);
				tmp=GPVector(u3.x(),u3.y(),u3.z(),1.0f)*(c2orig*ml2wc*orig2c);
				u3=Point(tmp.x/tmp.w,tmp.y/tmp.w,tmp.z/tmp.w);
				tmp=GPVector(l0.x(),l0.y(),l0.z(),1.0f)*(c2orig*ml2wc*orig2c);
				l0=Point(tmp.x/tmp.w,tmp.y/tmp.w,tmp.z/tmp.w);
				tmp=GPVector(l1.x(),l1.y(),l1.z(),1.0f)*(c2orig*ml2wc*orig2c);
				l1=Point(tmp.x/tmp.w,tmp.y/tmp.w,tmp.z/tmp.w);
				tmp=GPVector(l2.x(),l2.y(),l2.z(),1.0f)*(c2orig*ml2wc*orig2c);
				l2=Point(tmp.x/tmp.w,tmp.y/tmp.w,tmp.z/tmp.w);
				tmp=GPVector(l3.x(),l3.y(),l3.z(),1.0f)*(c2orig*ml2wc*orig2c);
				l3=Point(tmp.x/tmp.w,tmp.y/tmp.w,tmp.z/tmp.w);

/*	N.x=normal.x();
	N.y=normal.y();
	N.z=normal.z();

	// the vector is given in a right-handed orthonormal basis [U,normal,V], form that basis
	U=N.orthogonal();
	V=U%N;

	// now form a matrix that transforms the local coordinates to world coordinates
	local2WC.local2WCxForm(U,N,V);
	D=D*local2WC;
	r.direction().x()=D.x;
	r.direction().y()=D.y;
	r.direction().z()=D.z;
*/
				// top poly
				{
					Polygon* poly = new Polygon();
					poly->setMaxPoints(4);
					poly->set(u3);poly->set(u2);poly->set(u1);poly->set(u0);
					poly->material()=mat;
					poly->planeEquation();
					scene.ObjectArray[i] = poly;
					setBRDF(scene,i);
				}
				// bottom poly
				{
					Polygon* poly = new Polygon();
					poly->setMaxPoints(4);
					poly->set(l0);poly->set(l1);poly->set(l2);poly->set(l3);
					poly->material()=mat;
					poly->planeEquation();
					scene.ObjectArray[++i] = poly;
					setBRDF(scene,i);
				}
				// front poly
				{
					Polygon* poly = new Polygon();
					poly->setMaxPoints(4);
					poly->set(u0);poly->set(u1);poly->set(l1);poly->set(l0);
					poly->material()=mat;
					poly->planeEquation();
					scene.ObjectArray[++i] = poly;
					setBRDF(scene,i);
				}
				// back poly
				{
					Polygon* poly = new Polygon();
					poly->setMaxPoints(4);
					poly->set(u2);poly->set(u3);poly->set(l3);poly->set(l2);
					poly->material()=mat;
					poly->planeEquation();
					scene.ObjectArray[++i] = poly;
					setBRDF(scene,i);
				}
				// left poly
				{
					Polygon* poly = new Polygon();
					poly->setMaxPoints(4);
					poly->set(u1);poly->set(u2);poly->set(l2);poly->set(l1);
					poly->material()=mat;
					poly->planeEquation();
					scene.ObjectArray[++i] = poly;
					setBRDF(scene,i);
				}
				// right poly
				{
					Polygon* poly = new Polygon();
					poly->setMaxPoints(4);
					poly->set(u3);poly->set(u0);poly->set(l0);poly->set(l3);
					poly->material()=mat;
					poly->planeEquation();
					scene.ObjectArray[++i] = poly;
					setBRDF(scene,i);
				}

				break;
			}			 
		}
	}		
	return s;
}



bool Scene::intersect(Ray ray, Colour& colour)
/*returns true if the ray intersects the scene, and if so then the colour
at the intersection point*/
{

    //=============================PREV-HW1===============================
    //You need to fill this part with the implementation of your PREV-HW1.

    return false;
}


ostream& Scene::print(ostream& s)
{
	int i;
	
	for (i=0; i<N; ++i) ObjectArray[i]->print(s);
	
	return s;
}


istream& Scene::read(istream &s)
{
	s>>(*this);
	return s;
}


char skipWhiteSpace(istream& s)
{
	// skip over whitespace & comments
	char c;
	do
	{
		s>>c;
		if (c=='#') s.ignore(numeric_limits<int>::max(),'\n');
	}
	while (c=='\t'||c=='\n'||c==' '||c=='#');
	return c;
}
