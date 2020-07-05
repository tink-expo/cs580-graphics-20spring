#ifndef __BRDF_H_
#define __BRDF_H_

#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif

#ifndef _COLOUR
#include "colour.h"
#endif

#ifndef _RAY
#include "ray.h"
#endif

#ifndef _VECTOR
#include "vector.h"
#endif

#ifndef _POINT
#include "point.h"
#endif

enum eBRDFType {BRDF_DIFFUSE,BRDF_SPECULAR,BRDF_GLOSSY};

//+----------------------------------------------------------------------------+
//| CLASS: baseMaterial
//+----------------------------------------------------------------------------+
//| DESCRIPTION:
//| This defines an interface to a material as viewed as a black box. See
//| "Physically Based Lighting Calculations for Computer Graphics" by Peter
//| Shirley, this is what inspired this implementation
//+----------------------------------------------------------------------------+
class baseBRDF
{
	//+----------------------------------------
	//| PRIVATE INTERFACE
	//+----------------------------------------
private:

	//+----------------------------------------
	//| PUBLIC INTERFACE
	//+----------------------------------------
public:
	baseBRDF()
	{
	}
	~baseBRDF()
	{
	}

	
	// virtuals, must be implemented
	virtual Colour emission(void) const=0;
	virtual Colour& emission(void)=0;
	virtual eBRDFType brdfType(void) const=0;
  
	virtual Ray reflection(
		const Ray& incoming,		// incoming ray
		const Vector& normal,	// surface normal
		const Point& point,		// intersection point with surface
		float s,float t,			// canonical random variables (on unit square)
		float& pdf
		) const=0;

	virtual Colour brdf(
		const Point& x,			// surface point to evaluate brdf at
		const Vector& in,			// incoming vector
		const Vector& out,		// outgoing vector
		const Vector& Nx,			// normal at x (must be unit length)
		const float s
		) const=0;


}; // baseMaterial


//+----------------------------------------------------------------------------+
//| CLASS: lambertianBRDF
//+----------------------------------------------------------------------------+
//| DESCRIPTION:
//| This defines an ideal lambertian material
//+----------------------------------------------------------------------------+
class lambertianBRDF:public baseBRDF
{
	//+----------------------------------------
	//| PRIVATE INTERFACE
	//+----------------------------------------
private:

	//+----------------------------------------
	//| PUBLIC INTERFACE
	//+----------------------------------------
public:
	Colour m_kd;				// diffuse term
	Colour m_le;				// emission term	

	lambertianBRDF(Colour kd,Colour le):baseBRDF(),m_kd(kd),m_le(le)
	{
	}

	eBRDFType brdfType(void) const {return BRDF_DIFFUSE;};

	// generate a reflected ray stochastically with a cosine distribution
	virtual Ray reflection(
		const Ray& incoming,		// incoming ray
		const Vector& normal,	// surface normal
		const Point& point,		// intersection point with surface
		float s,float t,			// canonical random variables (on unit square)
		float& pdf
		) const;
	virtual Colour brdf(
		const Point& x,			// surface point to evaluate brdf at
		const Vector& in,			// incoming vector
		const Vector& out,		// outgoing vector
		const Vector& Nx,			// normal at x (must be unit length)
		const float s
		) const;
	virtual Colour emission(void) const
	{
		return m_le;
	};
	virtual Colour& emission(void)
	{
		return m_le;
	};

}; // lambertianBRDF

//+----------------------------------------------------------------------------+
//| CLASS: phongBRDF
//+----------------------------------------------------------------------------+
//| DESCRIPTION:
//| This defines an ideal lambertian material
//+----------------------------------------------------------------------------+
class phongBRDF:public baseBRDF
{
	//+----------------------------------------
	//| PRIVATE INTERFACE
	//+----------------------------------------
private:

	//+----------------------------------------
	//| PUBLIC INTERFACE
	//+----------------------------------------
public:
	float m_k;					// phong coeff
	Colour m_kd;				// diffuse term
	Colour m_ks;				// specular term
	Colour m_le;				// emission term
	phongBRDF(Colour kd,Colour ks,float k,Colour le):baseBRDF(),m_kd(kd),m_ks(ks),m_k(k),m_le(le)
	{
	}

	eBRDFType brdfType(void) const {return BRDF_SPECULAR;};

	// generate a reflected ray stochastically with a cosine distribution
	virtual Ray reflection(
		const Ray& incoming,		// incoming ray
		const Vector& normal,	// surface normal
		const Point& point,		// intersection point with surface
		float s,float t,			// canonical random variables (on unit square)
		float& pdf
		) const;
	virtual Colour brdf(
		const Point& x,			// surface point to evaluate brdf at
		const Vector& in,			// incoming vector
		const Vector& out,		// outgoing vector
		const Vector& Nx,			// normal at x (must be unit length)
		const float s
		) const;
	virtual Colour emission(void) const
	{
		return m_le;
	};
	virtual Colour& emission(void)
	{
		return m_le;
	};

}; // lambertianBRDF


#endif
