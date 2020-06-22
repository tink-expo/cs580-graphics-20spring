#ifndef __DEBUG_H_
#define __DEBUG_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "colour.h"

class cDebug
{
public:
	int* _pathLengths;
	int* _lighthits;
	float* _miscLH;
	Colour* _indirect;
	Colour* _inrefl;
	Colour* _direct;
	Colour* _weight;
	float* _misc;
	int m_depth;
	cDebug(unsigned int depth):m_depth(depth),_pathLengths(NULL),_indirect(NULL),_direct(NULL),_misc(NULL),_inrefl(NULL),_weight(NULL),_lighthits(NULL),_miscLH(NULL)
	{
		_pathLengths=new int[m_depth];
		_lighthits=new int[m_depth];
		_indirect=new Colour[m_depth];
		_direct=new Colour[m_depth];
		_misc=new float[m_depth];
		_miscLH=new float[m_depth];
		_inrefl=new Colour[m_depth];
		_weight=new Colour[m_depth];
		reset();
	}
	void reset(void)
	{
		for (int i=0;i<m_depth;i++)
		{
			_pathLengths[i]=0;
			_lighthits[i]=0;
			_indirect[i].reset(0.0f,0.0f,0.0f);
			_direct[i].reset(0.0f,0.0f,0.0f);
			_misc[i]=0.0f;
			_miscLH[i]=0.0f;
			_inrefl[i].reset(0.0f,0.0f,0.0f);
			_weight[i].reset(0.0f,0.0f,0.0f);
		}
	}
};
ostream& operator<<(ostream& s,cDebug d);

#endif