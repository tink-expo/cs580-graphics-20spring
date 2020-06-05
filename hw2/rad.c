
/*****************************************************************************
*  rad.c
*
*  This program contains three functions that should be called in sequence to
*  perform radiosity rendering:
*  InitRad(): Initialize radiosity.
*  DoRad(): Perform the main radiosity iteration loop.
*  CleanUpRad(): Clean up.
*
*  The following routines are assumed to be provided by the user:
*  BeginDraw()
*  DrawPolygon()
*  EndDraw()
*  Refer to rad.h for details
*
*  Copyright (C) 1990-1991 Apple Computer, Inc.
*  All rights reserved.
*
*  12/1990 S. Eric Chen	
******************************************************************************/


#include "rad.h"
#include <math.h>
#include <stdio.h>


#define kMaxPolyPoints	255
#define PI	3.1415926
#define AddVector(c,a,b) (c).x=(a).x+(b).x, (c).y=(a).y+(b).y, (c).z=(a).z+(b).z
#define SubVector(c,a,b) (c).x=(a).x-(b).x, (c).y=(a).y-(b).y, (c).z=(a).z-(b).z
#define CrossVector(c,a,b)	(c).x = (a).y*(b).z - (a).z*(b).y, \
				(c).y = (a).z*(b).x - (a).x*(b).z, \
				(c).z = (a).x*(b).y - (a).y*(b).x
#define DotVector(a,b) (a).x*(b).x + (a).y*(b).y + (a).z*(b).z
#define ScaleVector(c,s) (c).x*=(s), (c).y*=(s), (c).z*=(s)
#define NormalizeVector(n,a) 	((n)=sqrt(DotVector(a,a)), \
				(n)?((a).x/=n, (a).y/=n, (a).z/=n):0)

typedef struct {
	TView	view;	/* we only need to store one face of the hemi-cube */
	double*	topFactors;	/* delta form-factors(weight for each pixel) of the top face */
	double*	sideFactors; /* delta form-factors of the side faces */
} THemicube;

static TRadParams *params;	/* input parameters */
static THemicube hemicube;	/* one hemi-cube */
static double *formfactors;	/* a form-factor array which has the same length as the number of elements */
static double totalEnergy;	/* total emitted energy; used for convergence checking */

static const TSpectra black = { {0, 0, 0} };	/* for initialization */
static int FindShootPatch(unsigned long *shootPatch);
static void SumFactors(double* formfs, int xRes, int yRes, 
	IDENTIFIER* buf, double* deltaFactors,int startY);
static void MakeTopFactors(int hres, double* deltaFactors);
static void MakeSideFactors(int hres, double* deltaFactors);
static void ComputeFormfactors(unsigned long shootPatch);
static void DistributeRad(unsigned long shootPatch);
static void DrawHCElement(TElement* ep, IDENTIFIER color);
static void DrawViewElement(TElement* ep, TColor32b *colour);
static TColor32b SpectraToRGB(TSpectra* spectra);

int radiosityDone;

/* Initialize radiosity based on the input parameters p */
void InitRad(TRadParams *p, TView **displayview, TView **hemiview)
{
	int n;
	int hRes;
	unsigned long i;
	int j;
	TPatch*	pp;
	TElement* ep;
	
	params = p;
	
	/* initialize hemi-cube */
	hemicube.view.fovx = 90;
	hemicube.view.fovy = 90;
	/* make sure hemicube resolution is an even number */
	hRes = ((int)(params->hemicubeRes/2.0+0.5))*2;
	hemicube.view.xRes = hemicube.view.yRes = hRes;
	n = hRes*hRes;
	hemicube.view.buffer = (unsigned long *) calloc(n, sizeof(IDENTIFIER));
	hemicube.view.wid=0;
	hemicube.view.near_dist = params->worldSize*0.001;
	hemicube.view.far_dist = params->worldSize;
	
	/* take advantage of the symmetry in the delta form-factors */
	hemicube.topFactors= (double *) calloc(n/4, sizeof(double));
	hemicube.sideFactors= (double *) calloc(n/4, sizeof(double));
	MakeTopFactors(hRes/2, hemicube.topFactors);
	MakeSideFactors(hRes/2, hemicube.sideFactors);
	
	formfactors = (double *) calloc(params->nElements, sizeof(double));
	
	/* initialize radiosity */
	pp = params->patches;
	for (i=params->nPatches; i--; pp++)
		pp->unshotRad = *(pp->emission);
	ep = params->elements;
	for (i=params->nElements; i--; ep++)
		ep->rad = *(ep->patch->emission);

	/* compute total energy */
	totalEnergy = 0;
	pp = params->patches;
	for (i=params->nPatches; i--; pp++)
		for (j=0; j<kNumberOfRadSamples; j++) 
			totalEnergy += pp->emission->samples[j] * pp->area;

            /* YIORGOS' EXTRA CODE */
        *displayview = &params->displayView;
        *hemiview = &hemicube.view;

	radiosityDone = 0;
}

/* Main iterative loop */
void DoRad()
{
	unsigned long shootPatch;
	
	while (FindShootPatch(&shootPatch)) 
	{
            printf("One\n");
		ComputeFormfactors(shootPatch);
		DistributeRad(shootPatch);
		DisplayResults(&params->displayView);
	}
	
}

/* Main iterative loop, as called from the render function*/
unsigned long shootPatch_n;

int doOneIteration(void)
/* does one radiosity iteration only, returns TRUE when finished */
{	
	// Patch descending order iteration. So find max patch.
    if (FindShootPatch(&shootPatch_n)) 
    {
      
        ComputeFormfactors(shootPatch_n);
        DistributeRad(shootPatch_n);
        return 0; /*FALSE */
    } else {
        printf("Radiosity done \n");
		radiosityDone = 1;
        return 1; /* TRUE */
    }

}

/* Clean up */
void CleanUpRad()
{
	free(hemicube.topFactors);
	free(hemicube.sideFactors);
	free(hemicube.view.buffer);
	free(formfactors);

}

/* Find the next shooting patch based on the unshot energy of each patch */
/* Return 0 if convergence is reached; otherwise, return 1 */
static int FindShootPatch(unsigned long *shootPatch)
{

	// [CS580 GLOBAL ILLUMINATION] YOUR CODE HERE
	// TASK#2 Shooting
	// SUBTASK(#1) Find the shoot Patch
	// PSUEDO CODE IS DESCRIBED IN THE COURSE MATERIAL
	// CURRENTLY IT ALWAYS RETURNING 0

	double energySum, error, maxEnergySum=0;

	for (int i = 0; i < (int) params->nPatches; ++i) {
		energySum = 0;

		for (int j = 0; j < kNumberOfRadSamples; ++j) {
			energySum += params->patches[i].unshotRad.samples[j] * 
					params->patches[i].area;
		}

		if (energySum > maxEnergySum) {
			*shootPatch = i;
			maxEnergySum = energySum;
		}
	}
	
	error = maxEnergySum / totalEnergy;
	/* check convergence */
	if (error < params->threshold)
		return 0;		/* converged */
	else
		return 1;

}

/* Find out the index to the delta form-factors array */
#define Index(i, hres)	((i)<hres? i: (hres-1- ((i)%hres)))

/* Use the largest 32bit unsigned long for background */
/* #define kBackgroundItem 0xffffffff */
/* actually we will use only 24 bits */
#define kBackgroundItem 16777215

/* Convert a hemi-cube face to form-factors */
static void SumFactors(
double* formfs, /* output */
int xRes, int yRes, /* resolution of the hemi-cube face */
IDENTIFIER* buf, /* we only need the storage of the top hemi-cube face */
double* deltaFactors, /* delta form-factors for each hemi-cube pixel */
int startY
)
{
	// [CS580 GLOBAL ILLUMINATION] YOUR CODE HERE
	// TASK#1 Computing Form Factor
	// SUBTASK(#3) Convert the hemicube into form factors.
	// Convert the hemicube into form factors by using delta form factors 
	// In the hemicube image, which is stored in buf, each element(faces) is projected and marked as an integer id
	// See DrawHCPolygon() for more details about color(integer id) marking.
	// You may want to use current_backItem to check if an element is projected onto a pixel of a hemicube image
	// save form factors in formfs[] (the size of this array = nElements)
	// Currently all the form factors are set to 0.0

	register unsigned long int current_backItem;
	if (bits_for_RGB == 24)	current_backItem = kBackgroundItem;
	else					current_backItem = 65535;			/* 2^16 - 1 */

	unsigned long* bufIter = buf;
	int hres = xRes / 2;

	for (int yIdx = 0; yIdx < yRes; ++yIdx) {
		int yDfIdx = Index(yIdx, hres) * hres;

		for (int xIdx = 0; xIdx < xRes; ++xIdx) {
			++bufIter;
			unsigned long bufVal = *bufIter;
			if (bufVal != current_backItem) {
				int xDfIdx = Index(xIdx, hres);

				formfs[bufVal] += deltaFactors[xDfIdx + yDfIdx];
			}
		}
	}
}

/* Create the delta form-factors for the top face of hemi-cube */
/* Only need to compute 1/4 of the form-factors because of the 4-way symmetry */
static void MakeTopFactors(
int hres, /* half resolution of the face */
double* deltaFactors /* output */
)
{
	// [CS580 GLOBAL ILLUMINATION] YOUR CODE HERE
	// TASK#1 Computing Form Factor
	// SUBTASK(#1) Compute Delta Form-Factors for the top of a hemicube
	// For all pixels in the top of a hemicube, compute delta form factors. 
	// The formula is well written in a course material.

	double* out_ptr = deltaFactors;

	for (int yIdx = 0; yIdx < hres; ++yIdx) {
		double y = 1.0 - (0.5 + yIdx) / hres;
		double ySq = y * y;

		for (int xIdx = 0; xIdx < hres; ++xIdx) {
			double x = 1.0 - (0.5 + xIdx) / hres;
			double xSq = x * x;

			double rSq = xSq + ySq + 1;

			*out_ptr = 1.0 / (PI * (rSq * rSq) * (hres * hres));
			++out_ptr;
		}
	}
}

/* Create the delta form-factors for the side face of hemi-cube */
/* Only need to compute 1/4 of the form-factors because of the 4-way symmetry */
static void MakeSideFactors(
int hres, /* half resolution of the face */
double* deltaFactors /* output */
)
{
	// [CS580 GLOBAL ILLUMINATION] YOUR CODE HERE
	// TASK#1 Computing Form Factor
	// SUBTASK(#2) Compute Delta Form-Factors for the side of a hemicube
	// This is very similar to what you've done in SUBTASK(#1)
	
	double* out_ptr = deltaFactors;

	for (int z_idx = 0; z_idx < hres; ++z_idx) {
		double z = 1.0 - (0.5 + z_idx) / hres;
		double z_sq = z * z;

		for (int yIdx = 0; yIdx < hres; ++yIdx) {
			double y = 1.0 - (0.5 + yIdx) / hres;
			double ySq = y * y;

			double rSq = ySq + z_sq + 1;

			*out_ptr = z / (PI * (rSq * rSq) * (hres * hres));
			++out_ptr;
		}
	}
}

/* Use drand48 instead if it is supported */
#define RandomFloat ((float)(rand())/(float)RAND_MAX)

/* Compute form-factors from the shooting patch to every elements */
static void ComputeFormfactors(unsigned long shootPatch)
{
	unsigned long i;
	TVector3f	up[5]; 
	TPoint3f	lookat[5];
	TPoint3f	center;
	TVector3f	normal, tangentU, tangentV, vec;
	int face;
	double		norm;
	TPatch*		sp;
	double*		fp;
	TElement*	ep;
        double          plane_eq[4];

	/* get the center of shootPatch */
	sp = &(params->patches[shootPatch]);
	center = sp->center;
	normal = sp->normal;
        plane_eq[0] = (double)normal.x;
        plane_eq[1] = (double)normal.y;
        plane_eq[2] = (double)normal.z;
        plane_eq[3] = (double)-(normal.x*center.x + normal.y*center.y +
                                normal.z*center.z);
	
	/* rotate the hemi-cube along the normal axis of the patch randomly */
	/* this will reduce the hemi-cube aliasing artifacts */
	do {
		vec.x = RandomFloat;
		vec.y = RandomFloat;
		vec.z = RandomFloat;
		/* get a tangent vector */
		CrossVector(tangentU, normal, vec);
		NormalizeVector(norm, tangentU);
	} while (norm==0);	/* bad choice of the random vector */
	
	/* compute tangentV */
	CrossVector(tangentV, normal, tangentU);
	
	/* assign the lookats and ups for each hemicube face */
	AddVector(lookat[0], center, normal);
	up[0] = tangentU;
	AddVector(lookat[1], center, tangentU);
	up[1] = normal;
	AddVector(lookat[2], center, tangentV);
	up[2] = normal;
	SubVector(lookat[3], center, tangentU);
	up[3] = normal;
	SubVector(lookat[4], center, tangentV);
	up[4] = normal;
	
	/* position the hemicube slightly above the center of the shooting patch */
	ScaleVector(normal, params->worldSize*0.00000001);
	AddVector(hemicube.view.camera, center, normal);
	
	/* clear the formfactors */
	fp = formfactors;
	for (i=params->nElements; i--; fp++)
		*fp = 0.0;
		
	for (face=0; face < 5; face++)
	{
		hemicube.view.lookat = lookat[face];
		hemicube.view.up = up[face];

		/* draw elements */
		if (bits_for_RGB == 24) { /* a 24-bit display */
			BeginHCDraw(&(hemicube.view), kBackgroundItem, plane_eq);
			for (i=0; i< params->nElements; i++)
				DrawHCElement(&params->elements[i], i);	
			/* color element i with its index */
			EndHCDraw(&(hemicube.view));

		} else if (bits_for_RGB == 8) { /* an 8-bit display */
			/* this is a quick hack to make it work for 8-bit
			displays maybe a better way could be found ??? */
			part_of_id = 1; /* processing first half of polygon ids */
			BeginHCDraw(&(hemicube.view), 255, plane_eq);
			for (i=0; i< params->nElements; i++)
				DrawHCElement(&params->elements[i], i);	
			/* color element i with its index */
			EndHCDraw(&(hemicube.view));

			part_of_id = 2; /* second half of polygon ids */
			BeginHCDraw(&(hemicube.view), 255, plane_eq);
			for (i=0; i< params->nElements; i++)
				DrawHCElement(&params->elements[i], i);	
			/* color element i with its index */
			EndHCDraw(&(hemicube.view));

		} else {
			printf("Unexpected bits per RGB colour, exiting");
			//exit(0);
		}

		/* get formfactors */
		if (face == 0)
			SumFactors(formfactors, hemicube.view.xRes, hemicube.view.yRes, 
			hemicube.view.buffer, hemicube.topFactors,0);
		else
			SumFactors(formfactors, hemicube.view.xRes, hemicube.view.yRes, 
			hemicube.view.buffer, hemicube.sideFactors,
			hemicube.view.yRes/2);
	}
	
	/* compute reciprocal form-factors */
	ep = params->elements;
	fp = formfactors;
	for (i=params->nElements; i--; ep++, fp++)
	{
		*fp *= sp->area / ep->area;
		/* This is a potential source of hemi-cube aliasing */
		/* To do this right, we need to subdivide the shooting patch
		and reshoot. For now we just clip it to unity */
		if ((*fp) > 1.0) 	*fp = 1.0;	
	}

}

/* Distribute radiosity form shootPatch to every element */
/* Reset the shooter's unshot radiosity to 0 */
static void DistributeRad(unsigned long shootPatch)
{
	// [CS580 GLOBAL ILLUMINATION] YOUR CODE HERE
	// TASK#2 Shooting
	// SUBTASK(#2) Distribute unshot radiosity of shoot patch to other elements
	// HINT: TPatch* sp = &(params->patches[shootPatch]); 
	//       Iterates TElement* ep for all elements
	//       Send to sp->unshotRad to ep->rad
	// PSEUDO CODE IS DESCRIBED IN THE COURSE MATERIAL
	// BEFORE RETURNING THIS FUNCTION, write 'sp->unshotRad = black;'

	TPatch* sp = &(params->patches[shootPatch]);
	TElement* ep = params->elements;

	for (int jq = 0; jq < params->nElements; ++jq, ++ep) {
		if (formfactors[jq] != 0.0) {
			for (int k = 0; k < kNumberOfRadSamples; ++k) {
				double delta_rad =
						ep->patch->reflectance->samples[k] *
						sp->unshotRad.samples[k] *
						formfactors[jq]; 
						// (Ai / Aj) term included in ComputeFormfactors.

				// Adding to element's individual rad.
				ep->rad.samples[k] += delta_rad;
				// Accumulating to element's parent patch's unshot rad. 
				// => Mult (elem area / parent patch area).
				ep->patch->unshotRad.samples[k] += delta_rad * (ep->area / ep->patch->area);
			}
		}
	}

	sp->unshotRad = black;
}

/* Convert a TSpectra (radiosity) to a TColor32b (rgb color) */
/* Assume the first three samples of the spectra are the r, g, b colors */
/* More elaborated color space transformation could be performed here */
static TColor32b
SpectraToRGB(TSpectra* spectra)
{
	TColor32b	c;
	TSpectra	r;
	double 	max=1.0;
	int k;

	for (k=kNumberOfRadSamples; k--;) {
		if (spectra->samples[k] > max)
			max = spectra->samples[k];
	}
	/* Clip the intensity*/
	r = *spectra;
	if (max>1.0) {
		for (k=kNumberOfRadSamples; k--; )
			r.samples[k] /= max;
	}
	
	/* Convert to a 32-bit color; Assume the first 3 samples in TSpectra 
	are the r, g, b colors we want. Otherwise, do color conversion here */
	c.a= 0;
	c.r= (unsigned char) (r.samples[0] * 255.0 + 0.5);
	c.g= (unsigned char) (r.samples[1] * 255.0 + 0.5);
	c.b= (unsigned char) (r.samples[2] * 255.0 + 0.5);
	
	return c;
}

static void
GetAmbient(TSpectra* ambient)
{
	// [CS580 GLOBAL ILLUMINATION] YOUR CODE HERE
	// TASK#3 Add Ambient Term
	// Now the ambient term(the value in uSum, which will be moved to the variable 'ambient', is set to be black
	// Modify this part, so that correct value can be assigned.
	// This 'GetAmbient' is called in the function DisplayResults(). 
	// If the input scene (TQuad) is configured to have ambient, the ambient term will be added when drawing the scene in DisplayResults()
	
	// R == 1 / (1 - avg.ref) == aSum / (aSum - rSum)

	// Compute the average reflectance (this part should be run only one time).
	static TSpectra arSumSub;
	static int computeAverageReflectance = 1;
	if (computeAverageReflectance) {
		computeAverageReflectance = 0;

		double aSum = 0;
		TSpectra rSum = black;
		for (int i = 0; i < params->nPatches; ++i) {
			aSum += params->patches[i].area;
			for (int k = 0; k < kNumberOfRadSamples; ++k) {
				rSum.samples[k] +=
						params->patches[i].reflectance->samples[k] *
						params->patches[i].area;
			}
		}

		for (int k = 0; k < kNumberOfRadSamples; ++k) {
			arSumSub.samples[k] = aSum - rSum.samples[k];
		}
	}

	// sum (unshot radiosity * area).
	TSpectra uSum = black;
	for (int i = 0; i < params->nPatches; ++i) {
		for (int k = 0; k < kNumberOfRadSamples; ++k) {
			uSum.samples[k] += 
					params->patches[i].unshotRad.samples[k] *
					params->patches[i].area;
		}
	}

	// compute ambient.
	for (int k = 0; k < kNumberOfRadSamples; ++k) {
		ambient->samples[k] = uSum.samples[k] / arSumSub.samples[k];
	}
}

void DisplayResults(TView* view)
{
	// [CS580 GLOBAL ILLUMINATION] 
	// TASK#5 Smoothening
	// You better understand this function

	register TElement* ep;
	TSpectra ambient;
	GetAmbient(&ambient);
	
	BeginViewDraw(view, 0);
	ep = params->elements;
	for (int i=0; i< params->nElements; i++, ep++) {
		TColor32b	c;
		TSpectra  s;
		int k;
		/* add ambient approximation */
		if (params->addAmbient) {
			for (k=kNumberOfRadSamples; k--;)
				s.samples[k] = (ep->rad.samples[k] + (ambient.samples[k]*ep->patch->reflectance->samples[k])) * params->intensityScale;
		} else {
			for (k=kNumberOfRadSamples; k--; )
				s.samples[k] = ep->rad.samples[k] * params->intensityScale;
		}
		/* quantize color */
		ep->color = SpectraToRGB(&s);
	}

	if (radiosityDone) {
		// for (int q = 0; q < params->nQuads; ++q) {
		// 	int qDim = params->quadElemDim[q];

		// 	for (int i = 0; i < qDim; ++i) {
		// 		for (int j = 0; j < qDim; ++j) {

		// 		}
		// 	}
		// }
		ep = params->elements;
		for (int i = 0; i < params->nElements; ++i, ++ep) {
			for (int v = 0; v < ep->nVerts; ++v) {
				
			}
		}
	} else {
		ep = params->elements;
		for (int i=0; i< params->nElements; i++, ep++) {
			DrawViewElement(ep, &ep->color);
		}
	}
			
	EndViewDraw();
}

static void
DrawHCElement(TElement* ep, IDENTIFIER color)
{
	static TPoint3f pts[kMaxPolyPoints];
	int nPts = ep->nVerts;
	int j;
	for (j=0; j<nPts; j++)
		pts[j] = params->points[ep->verts[j]];
	
	DrawHCPolygon(nPts, pts, &ep->normal, color);

}

static void
DrawViewElement(TElement* ep, TColor32b *color)
{
	// [CS580 GLOBAL ILLUMINATION] 
	// TASK#5 Smoothening
	// You better understand this function
	
	static TPoint3f pts[kMaxPolyPoints];
	int nPts = ep->nVerts;
	int j;
	for (j=0; j<nPts; j++)
		pts[j] = params->points[ep->verts[j]];

	DrawViewPolygon(nPts, pts, &ep->normal, color);
}




