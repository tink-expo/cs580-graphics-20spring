#include "debug.h"
#include "assert.h"
#include "brdf.h"
#include "litscene.h"
#include "polygon.h"
#include "sphere.h"
#include "simplecamera.h"
#include <GMmatlib.h>	
#include <math.h>
#include <float.h>


Ray SimpleCamera::StratifiedRandomRay(int i, int j, int k, int l, double offset)
{
	// [CS580 COMPUTER GRAPHICS] YOUR CODE HERE
	//==================================================
	//Skeleton for TASK2
	//Subtask 1
	//==================================================

	//(i,j) is the pixel location, 
	//and (k,l) is a stratified offset each for i and j. 
	//Finally offset is a coefficient for jittering.

	//Currently, we only sample a fixed point per a pixel. 
	//Implement the stratification and jittering technique in order to generate smooth shadow as shown in Figure 1.

	float lambda1 = frand() - 0.5f;
	float lambda2 = frand() - 0.5f;
	Point pixel_pos(Xmin + (i + lambda1) * Width, Ymin + (j + lambda2)  * Height, 2.0);

	Ray ray;
	Point cop(0.0, 0.0, Zcop);

	ray.origin() = cop;
	ray.direction() = pixel_pos - cop;
	return ray;
}

Colour LitScene::renderPixel(int i, int j, SimpleCamera& TheCamera, int N_RAYS_PER_PIXEL)
{
	// [CS580 COMPUTER GRAPHICS] YOUR CODE HERE
    //==================================================
    //Skeleton for TASK2
    //Subtask 2
    //==================================================
	// (i,j): pixel location
	// You need to shoot N_RAYS_PER_PIXEL rays for a pixel (i,j) with the StratifiedRandomRay function, which you already implemented in SUBTASK(#1).

	Colour col(0, 0, 0);
	for (int n = 0; n < N_RAYS_PER_PIXEL; ++n) 
	{
		col = col + tracePath(TheCamera.StratifiedRandomRay(i, j, 0, 0, 0)) / (float) N_RAYS_PER_PIXEL;
	}
	return col;
}


// ===========================================
// TASK 3-Area light sources
// ===========================================

float Polygon::TriangularSampling(Point& p, float s, float t, int Type = 0)
{
    // [CS580 COMPUTER GRAPHICS] YOUR CODE HERE
    //==================================================
    //Skeleton for TASK3
    //Subtask 1
    //==================================================
    // p is the sampled position of the light source. 
    // s and t are the coefficients for sampling. 
    // You can ignore the type parameter. 
    // Return value is the pdf for the p.
	Vector P01 = P[1] - P[0];
	Vector P02 = P[2] - P[0];
	float sqrt_1ms = sqrt(1 - s);
	p = P[0] + P01 * t * sqrt_1ms + P02 * (1 - sqrt_1ms);
	float pdf = 2.0 / (P01 * P02).norm();
	return pdf;
}

float Polygon::RectangularSampling(Point& p, float s, float t, int Type = 0)
{
	// [CS580 COMPUTER GRAPHICS] YOUR CODE HERE
	//==================================================
	//Skeleton for TASK3
	//Subtask 2
	//==================================================
	// p is the sampled position of the light source. 
	// s and t are the coefficients for sampling. 
	// You can ignore the type parameter. 
	// Return value is the pdf for the p.

	Vector P01 = P[1] - P[0];
	Vector P03 = P[3] - P[0];
	p = P[0] + P01 * s + P03 * t;
	float pdf = 1.0 / (P01 * P03).norm();
	return pdf;
}

bool Sphere::sample(Point& p, float& probability, const Point& from, float s, float t)
{
	// [CS580 COMPUTER GRAPHICS] YOUR CODE HERE
	//==================================================
	//Solution for TASK3
	//Subtask 3
	//==================================================
	// p is the sampled position of the light source. 
	// s and t are the coefficients for sampling. probability is the pdf for the p. 
	// from is the intersection point of a ray with an intersection object. 
	// *Reference: Section 3.2 ¡®Sampling Spherical Luminaries¡¯ in ¡°Monte Carlo Techniques for Direct Lighting Calculations,¡± ACM Transactions on Graphics, 1996
	s = frand();
	t = frand();
	p.x() = Centre.x() + 2 * Radius * cos(2 * PI * t) * sqrt(s * (1 - s));
	p.y() = Centre.y() + 2 * Radius * sin(2 * PI * t) * sqrt(s * (1 - s));
	p.z() = Centre.z() + Radius * (1 - 2 * s);

	float numer = (from - p).normalised() ^ (p - Centre).normalised();
	float denom = 2 * PI * (from - p).squarednorm() *
			(1 - sqrt(1 - pow(Radius / (from - Centre).norm(), 2)));
	probability = numer / denom;

	return true;
}

// ===========================================
// TASK 4-Modified Phong BRDF-Diffuse, Specular
// ===========================================

Colour phongBRDF::brdf(
	const Point& x,				// surface point to evaluate brdf at
	const Vector& in,			// incoming vector
	const Vector& out,			// outgoing vector
	const Vector& Nx,			// normal at x (must be unit length)
	const float s
) const
{

	// [CS580 COMPUTER GRAPHICS] YOUR CODE HERE
	//==================================================
	//Skeleton for TASK4
	//Subtask 1
	//==================================================
	// A parameter s is the random variable in the range between 0 and 1. 
	// We use s in order to determine which term is employed from diffuse and specular BRDF.

	return m_kd / (float)PI;
}


// ===========================================
// TASK 5-Modified phong reflection
// ===========================================

Ray phongBRDF::reflection(	  const Ray& incoming,	// incoming ray
    const Vector& normal,	// surface normal
    const Point& point,	// intersection point with surface
    float s,float t,		// canonical random variables (on unit square)							  
    float& pdf
    ) const
{

    GPMatrix local2WC;
    GPVector3 U,N,V;
    GPVector	D;
    D.w=1.0f;

    Ray r;
    r.origin()=point;

    float pd = m_kd.sum()/3.0f;
    float ps = m_ks.sum()/3.0f;
    float s2 = frand()*(pd+ps);
    if( s2 >= 0 && s2 < pd )
    {
        //==================================================
        //Diffuse part
        //==================================================

        D.x=cos(2.0f*(float)PI*s2)*sqrt(1-t);
        D.y=sqrt(t);
        D.z=sin(2.0f*(float)PI*s2)*sqrt(1-t);

        N.x=normal.x();
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

        pdf = ( r.direction().normalised() ^ normal) / (float)PI;
        return r;
    }
    else if( s2 >= pd && s2 < pd + ps )
    {
        // [CS580 GLOBAL ILLUMINATION] YOUR CODE HERE
        //==================================================
        //TASK5
        //Specular part

    }

    pdf = 0;
    return r;
}

// ===========================================
// TASK 1-Direct illumiation
// TASK 6-Indirect illumination
// TASK 7-Russian roulette
// TASK 8-Refraction
// ===========================================

Colour LitScene::tracePath(const Ray& ray, GObject* object, Colour weightIn, int depth)
{
	float Lamda1 = frand(), Lamda2 = frand();   // random variables in cannonical 2d space in range of [0..1]

    Colour colourOut(0,0,0);

    // ----------------------------
    // check to see whether we should terminate here
    // ----------------------------
    if (depth >= m_maxBounces) return colourOut;
    if( weightIn.sum() < m_cutOffThreshold) return colourOut;

    Point ixp;		// intersection point
    Vector normal;	// normal at intersection point

    // ----------------------------
    // intersect ray with scene
    // ----------------------------
    GObject* hitObject = intersect(ray,ixp,normal);

    if (!hitObject ) return colourOut;

    if (hitObject->isEmitter())
    {
        // ray struck an emitter - return emission.
        // you could possibly argue that there should be a slight chance that a ray is scattered 
        // off an emitter depending on its brdf - this is however usually ignored.
        float ndotl = normal ^ ray.direction().invert().normalised();
        if (ndotl > 0.0f)
            colourOut = hitObject->material().emission();
        return colourOut;
    }

	// [CS580 COMPUTER GRAPHICS] YOUR CODE HERE
	//==================================================
	//Skeleton for TASK1 
	//Subtask 1
	//==================================================
	// Direct illumination

	Colour colorAdd(0, 0, 0);

	for (int i = 0; i < numberOfAreaLights(); ++i) 
	{
		GObject* areaLight = areaLightAt(i);
		float probability = 0;
		Point syp;

		int count = 0;
		while (probability <= 0 && count < 100) {
			areaLight->sample(syp, probability, ixp, Lamda1, Lamda2);
			++count;
		}

		if (probability > 0)
		{
			Vector xyRayDir = (syp - ixp).normalised();

			Point iyp;
			Vector yNormal;
			GObject* yObject = intersect(Ray(ixp, xyRayDir), iyp, yNormal);
			if (yObject == areaLight)
			{
				float cos_x_phi = normal ^ xyRayDir;
				float cos_y_nphi = yNormal ^ xyRayDir.invert();
				float r_sq = (ixp - syp).squarednorm();  // syp? iyp?
				float geo_term = cos_x_phi * cos_y_nphi / r_sq;

				colorAdd = colorAdd +
					areaLight->material().emission() *
					hitObject->brdf()->brdf(ixp, ray.direction().normalised(), xyRayDir, normal, Lamda1) *
					geo_term /
					probability;
			}
		}
	}

    // Indirect illumination
    // [CS580 GLOBAL ILLUMINATION] YOUR CODE HERE
    //==================================================
    //Task6, Task7, Task8
    //==================================================

    colourOut = colourOut + colorAdd;
    return colourOut;
}

// ===========================================
// Other functions
// ===========================================

Ray lambertianBRDF::reflection(
    const Ray& incoming,	// incoming ray
    const Vector& normal,	// surface normal
    const Point& point,	// intersection point with surface
    float s,float t,		// canonical random variables (on unit square)
    float& pdf
    ) const
{
    GPMatrix local2WC;
    GPVector3 U,N,V;
    GPVector	D;
    D.w=1.0f;

    Ray r;
    r.origin()=point;

    D.x=cos(2.0f*(float)PI*s)*sqrt(1-t);
    D.y=sqrt(t);
    D.z=sin(2.0f*(float)PI*s)*sqrt(1-t);

    N.x=normal.x();
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

    pdf = ( r.direction().normalised() ^ normal) / (float)PI;
    return r;
}

Colour lambertianBRDF::brdf(
    const Point& x,				// surface point to evaluate brdf at
    const Vector& in,			// incoming vector
    const Vector& out,			// outgoing vector
    const Vector& Nx,			// normal at x (must be unit length)
    const float s
    ) const
{
    return m_kd/(float)PI;
}
