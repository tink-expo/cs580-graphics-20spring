#include "debug.h"

ostream& operator<<(ostream& s,cDebug d)
{
	for (int i=0;i<d.m_depth;i++)
	{
		int div=1;
		if (d._pathLengths[i]>0) div=d._pathLengths[i];

		s << "\tstats for path depth\t" << i << "\t" <<
			"count\t" << d._pathLengths[i] << "\t" <<
			"lighthits\t" << d._lighthits[i] << "\t" <<
			"avg miscLH\t" << d._miscLH[i]/((d._lighthits[i]>0)?((float)d._lighthits[i]):(1.0f))<< "\t" <<
			"avg misc\t" << d._misc[i]/(float)div << "\t" <<
			"avg direct\t" << (d._direct[i].sum()/3.0f)/(float)div << "\t" <<
			"avg indirect\t" << (d._indirect[i].sum()/3.0f)/(float)div << "\t" <<
			"avg indirect reflectance\t" << (d._inrefl[i].sum()/3.0f)/(float)div << "\t" <<
			"avg weight\t" << (d._weight[i].sum()/3.0f)/(float)div;
	}
	return s;
};