#include "random.h"
#include <time.h>

float frand(float min,float max)
{
	return ((float)rand()/RAND_MAX)*(max-min)+min;
}
