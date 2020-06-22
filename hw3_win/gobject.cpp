#include "gobject.h"

ostream& operator<<(ostream& s, eObjectType ot)
{
	switch (ot)
	{
	case OBJECT_POLYGON:
		s << "polygonObject";
		break;
	case OBJECT_SPHERE:
		s << "sphereObject";
		break;
	default:
		s << "unknownObject";
	}
	return s;
}