#include "material.h"

Material::Material(const Material& mat)
{
	Ambient = mat.Ambient;
	Diffuse = mat.Diffuse;
	Specular = mat.Specular;
	Emission = mat.Emission;
	Shininess = mat.Shininess;
}

Material& Material::operator=(const Material& mat)
{
	Ambient = mat.Ambient;
	Diffuse = mat.Diffuse;
	Specular = mat.Specular;
	Emission = mat.Emission;
	Shininess = mat.Shininess;
	
	return *this;
}

ostream& operator<<(ostream& s,Material mat)
//writing
{
	s << "Ambient: " << mat.Ambient << " " <<
	     "Diffuse: " << mat.Diffuse << " " <<
	     "Specular: " << mat.Specular << " " <<
		  "Emission: " << mat.Emission << " " <<
	     "Shininess: " << mat.Shininess;
	    
	return s;
}

istream& operator>>(istream& s,Material& mat)
//reading
{
	s >> mat.Ambient >> mat.Diffuse >> mat.Specular >> mat.Emission >> mat.Shininess;
	
	return s;
}
