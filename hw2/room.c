
/*****************************************************************************
*   room.c 
*
*   This is a test program which constrcuts the Cornell radiosity room with 
*   a ceiling light and two boxes inside. The side faces of the boxes are not 
*   directly illuminated by the light. Therefore, they are a good example of 
*   the color bleeding effects.
*   This program calls IniRad(), DoRad() and CleanUpRad() in rad.c to perform 
*   the radiosity rendering.
*
*	Copyright (C) 1990-1991 Apple Computer, Inc.
*	All rights reserved.
*
*	12/1990 S. Eric Chen	
******************************************************************************/

#include "rad.h"
#include <stdio.h>

/* a quadrilateral */
typedef struct {
	short verts[4];	/* vertices of the quadrilateral */
	short patchLevel; /* patch subdivision level (how fine to subdivide the quadrilateral?) */
	short elementLevel; /* element subdivision level (how fine to subdivide a patch?) */
	float area; /* area of the quadrilateral */
	TVector3f normal; /* normal of the quadrilateral */
	TSpectra* reflectance; /* diffuse reflectance of the quadrilateral */
	TSpectra* emission; /* emission of the quadrilateral */
} TQuad;

/* input parameters */
TRadParams	params = {
	0.0001,			/* convergence threshold */
	0, 0, 0, 0, 0, 0,	/* patches, elements and points; initialize these in InitParams */
	{{ 108, 120, 400 },	/* camera location */
	{ 108, 100, 100 },	/* look at point */
	{ 0, 1, 0 },	/* up vector */
	60, 60,			/* field of view in x, y*/
	1, 550,			/* near, far */
	//[Min] Camera Resolution
	250, 250,		/* resolution x, y */
	0 },			/* buffer */
	100,			/* hemi-cube resolution */
	250,			/* approximate diameter of the room */
	50,				/* intensity scale */
	1				/* add the ambient term */
};

TPoint3f roomPoints[] = {
	{ 0, 0, 0 },
	216, 0, 0,
	216, 0, 215,
	0, 0, 215,
	0, 221, 0,
	216, 221, 0,
	216, 221, 215,
	0, 221, 215,

	85.5, 220, 90,
	130.5, 220, 90,
	130.5, 220, 130,
	85.5, 220, 130,

	53.104, 0, 64.104,
	109.36, 0, 96.604,
	76.896, 0, 152.896,
	20.604, 0, 120.396,
	53.104, 65, 64.104,
	109.36, 65, 96.604,
	76.896, 65, 152.896,
	20.604, 65, 120.396,
	

	
	111.2545,         0,   101.4049,
	168.5951,         0,  61.2545,
	208.7455,         0,   118.5951,
	151.4049,         0,  158.7455,
	111.2545,  130.0000,   101.4049,
	168.5951,  130.0000,   61.2545,
	208.7455,  130.0000,   118.5951,
	151.4049,  130.0000,  158.7455,
};

static TSpectra red = {{ 0.80, 0.10, 0.075 }};
static TSpectra yellow = {{ 0.9, 0.8, 0.1 }};
static TSpectra blue = {{ 0.075, 0.10, 0.35 }};;
static TSpectra white = {{ 1.0, 1.0, 1.0 }};
static TSpectra lightGrey = {{ 0.9, 0.9, 0.9 }};
static TSpectra black = {{ 0.0, 0.0, 0.0 }};
static TSpectra Green = {{ 0.0, 0.40, 0.0 }};

/* Assume a right-handed coordinate system */
/* Polygon vertices follow counter-clockwise order when viewing from front */
#define numberOfPolys 	19
TQuad roomPolys[numberOfPolys] = {
	// [Min] TQuad struc = {vertices, patchLevel, elementLevel, area, normal, reflectance, emission}
	// [Min] patchLevel (2nd) and elementLevel (3rd) decides the patch and element subdivision level
	// [Min] patchLevel == resolution of surface
	//====================================================================//
	// Original parameters
	//{{4, 5, 6, 7}, 2, 8, 216*215, {0, -1, 0}, &lightGrey, &black}, /* ceiling */
	//{{0, 3, 2, 1}, 3, 8, 216*215, {0, 1, 0}, &lightGrey, &black}, /* floor */
	//{{0, 4, 7, 3}, 2, 8, 221*215, {1, 0, 0}, &red, &black}, /* wall */
	//{{0, 1, 5, 4}, 2, 8, 221*216, {0, 0, 1}, &lightGrey, &black}, /* wall */
	//{{2, 6, 5, 1}, 2, 8, 221*215, {-1, 0, 0}, &blue, &black}, /* wall */
	//{{2, 3, 7, 6}, 2, 8, 221*216, {0, 0,-1}, &lightGrey, &black}, /* ADDED wall */
	//{{8, 9, 10, 11}, 2, 1, 40*45, {0, -1, 0}, &black, &white}, /* light */
	//{{16, 19, 18, 17}, 1, 5, 65*65, {0, 1, 0}, &yellow, &black}, /* box 1 */
	//{{12, 13, 14, 15}, 1, 1, 65*65, {0, -1, 0}, &yellow, &black},
	//{{12, 15, 19, 16}, 1, 5, 65*65, {-0.866, 0, -0.5}, &yellow, &black},
	//{{12, 16, 17, 13}, 1, 5, 65*65, {0.5, 0, -0.866}, &yellow, &black},
	//{{14, 13, 17, 18}, 1, 5, 65*65, {0.866, 0, 0.5}, &yellow, &black},
	//{{14, 18, 19, 15}, 1, 5, 65*65, {-0.5, 0, 0.866}, &yellow, &black},
	//{{24, 27, 26, 25}, 1, 5, 65*65, {0, 1, 0}, &lightGrey, &black}, /* box 2 */
	//{{20, 21, 22, 23}, 1, 1, 65*65, {0, -1, 0}, &lightGrey, &black},
	//{{20, 23, 27, 24}, 1, 6, 65*130, {-0.866, 0, -0.5}, &lightGrey, &black},
	//{{20, 24, 25, 21}, 1, 6, 65*130, {0.5, 0, -0.866}, &lightGrey, &black},
	//{{22, 21, 25, 26}, 1, 6, 65*130, {0.866, 0, 0.5}, &lightGrey, &black},
	//{{22, 26, 27, 23}, 1, 6, 65*130, {-0.5, 0, 0.866}, &lightGrey, &black},
	//====================================================================//
	{{4, 5, 6, 7}, 4, 8, 216*215, {0, -1, 0}, &white, &black}, /* ceiling */ // mainly viewed
	{{0, 3, 2, 1}, 6, 8, 216*215, {0, 1, 0}, &white, &black}, /* floor */ // mainly viewed
	{{0, 4, 7, 3}, 4, 8, 221*215, {1, 0, 0}, &red, &black}, /* wall */ // mainly viewed
	{{0, 1, 5, 4}, 4, 8, 221*216, {0, 0, 1}, &white, &black}, /* wall */ // mainly viewed
	{{2, 6, 5, 1}, 4, 8, 221*215, {-1, 0, 0}, &blue, &black}, /* wall */ // mainly viewed
	{{2, 3, 7, 6}, 4, 8, 221*216, {0, 0,-1}, &white, &black}, /* ADDED wall */ // mainly viewed

	{{8, 9, 10, 11}, 2, 1, 40*45, {0, -1, 0}, &black, &white}, /* light */

	{{16, 19, 18, 17}, 4, 5, 65*65, {0, 1, 0}, &yellow, &black}, /* box 1 */ // top - view
	{{12, 13, 14, 15}, 1, 1, 65*65, {0, -1, 0}, &yellow, &black},  // bottom - not
	{{12, 15, 19, 16}, 1, 5, 65*65, {-0.866, 0, -0.5}, &yellow, &black}, // not
	{{12, 16, 17, 13}, 1, 5, 65*65, {0.5, 0, -0.866}, &yellow, &black},  // not
	{{14, 13, 17, 18}, 4, 5, 65*65, {0.866, 0, 0.5}, &yellow, &black},  // right side  - view
	{{14, 18, 19, 15}, 4, 5, 65*65, {-0.5, 0, 0.866}, &yellow, &black}, // left side - view

	{{24, 27, 26, 25}, 1, 5, 65*65, {0, 1, 0}, &Green, &black}, /* box 2 */ // top - not 
	{{20, 21, 22, 23}, 1, 1, 65*65, {0, -1, 0}, &Green, &black}, // bottom - not 
	{{20, 23, 27, 24}, 1, 6, 65*130, {-0.866, 0, -0.5}, &Green, &black},  // not
	{{20, 24, 25, 21}, 1, 6, 65*130, {0.5, 0, -0.866}, &Green, &black},  // not
	{{22, 21, 25, 26}, 3, 6, 65*130, {0.866, 0, 0.5}, &Green, &black},  // right side - view
	{{22, 26, 27, 23}, 4, 6, 65*130, {-0.5, 0, 0.866}, &Green, &black},  // left side - view
};


/* Compute the xyz coordinates of a point on a quadrilateral given its u, v coordinates using bi-linear mapping */
void UVToXYZ(const TPoint3f quad[4], float u, float v, TPoint3f* xyz)
{
	xyz->x = quad[0].x * (1-u)*(1-v) + quad[1].x * (1-u)*v + quad[2].x * u*v + 			quad[3].x * u*(1-v);
	xyz->y = quad[0].y * (1-u)*(1-v) + quad[1].y * (1-u)*v + quad[2].y * u*v + 			quad[3].y * u*(1-v);
	xyz->z = quad[0].z * (1-u)*(1-v) + quad[1].z * (1-u)*v + quad[2].z * u*v + 			quad[3].z * u*(1-v);
}

#define Index(i, j, nv) ((i)*(nv+1)+(j))

int iOffset; 	/* index offset to the point array */
TPatch* pPatch;
TElement* pElement;
TPoint3f* pPoint;

/* Mesh a quadrilateral into patches and elements */
/* Output goes to pPatch, pElement, pPoint */
void MeshQuad(int quadIdx)
{
	// [CS580 GLOBAL ILLUMINATION] YOUR CODE HERE
	// TASK#4 Substructuring 
	// SUBTASK(#3) Meshing
	// So far, the rendered scene looks messy, since the number of patch(or elements) is small.
	// Your job here is to divide the scene more finely.
	// A surface in the scene, which is represented by TQuad, consists of number of patches(TPatch).
	// Each patch(TPatch) is composed of number of elements(TElement)
	// But for this moment the patches are not meshed. 
	// What this means is that 
	// "A surface(TQuad) consists of number of patches(TPatch), but the patch consists of just an element(TElement) having the same area to it"
	// Therefore, divide each patch into many elements with smaller size.
	// BEFORE FIXING THIS FUNCTION, GO TO InitParams() AND MODIFY THE SIZE OF ARRAYS(DO SUBTASKS FIRST)
	TQuad* quad = &roomPolys[quadIdx];
	params.quadElemDim[quadIdx] = quad->patchLevel * quad->elementLevel;
	TPoint3f pts[4];
	int nPts = 0;

	/* Calculate element vertices(points) */
	for (int i = 0; i < 4; ++i)
	{
		pts[i] = roomPoints[quad->verts[i]];
	}
	
	int nu = quad->patchLevel * quad->elementLevel;
	double du = 1.0 / nu;

	int nv = nu;
	double dv = du;

	for (int i = 0; i <= nu; ++i) {
		for (int j = 0; j <= nv; ++j, ++pPoint) {
			++nPts;
			UVToXYZ(pts, (float) (i * du), (float) (j * dv), pPoint);
		}
	}

	/* Calculate elements. */
	for (int i = 0; i < nu; ++i) {
		for (int j = 0; j < nv; ++j, ++pElement) {
			pElement->normal = quad->normal;
			pElement->nVerts = 4;
			pElement->verts = (unsigned long*)calloc(pElement->nVerts, sizeof(unsigned long));
			pElement->verts[0] = Index(i, j, nv) + iOffset;
			pElement->verts[1] = Index(i+1, j, nv) + iOffset;
			pElement->verts[2] = Index(i+1, j+1, nv) + iOffset;
			pElement->verts[3] = Index(i, j+1, nv) + iOffset;
			pElement->area = quad->area / (nu * nv);

			pElement->patch = pPatch +
					((int) ((float) i / nu * quad->patchLevel)) * quad->patchLevel +
					((int) ((float) j / nv * quad->patchLevel));
		}
	}

	/* Calculate patches. */
	nu = quad->patchLevel;
	du = 1.0 / nu;

	nv = nu;
	dv = du;

	for (int i = 0; i < nu; ++i) {
		for (int j = 0; j < nv; ++j, ++pPatch) {
			UVToXYZ(pts, (float) ((i + 0.5) * du), (float) ((j + 0.5) * dv), &pPatch->center);
			pPatch->normal = quad->normal;
			pPatch->reflectance = quad->reflectance;
			pPatch->emission = quad->emission;
			pPatch->area = quad->area / (nu * nv);
		}
	}

	iOffset += nPts;
}

/* Initialize input parameters */
TRadParams *InitParams(void)
{
	/* compute the total number of patches */
	params.nPatches=0;
	for (int i = 0; i < numberOfPolys; ++i) {
		params.nPatches += roomPolys[i].patchLevel*roomPolys[i].patchLevel;
	}
	params.patches = (TPatch*)calloc(params.nPatches, sizeof(TPatch));


	// [CS580 GLOBAL ILLUMINATION] YOUR CODE HERE
	// TASK#4 Substructuring 
	// SUBTASK(#1) Modify the number of elements(nElements).
	// Refer to the above code, where the number of patces is computed.( you may want to use roomPolys[i].elementLevel. )
	// For now nElements = nPatches, because a patch is composed of an element now.

	/* compute the total number of elements */
	params.nElements = 0;
	for (int i = 0; i < numberOfPolys; ++i) {
		int nElems1D = roomPolys[i].elementLevel * roomPolys[i].patchLevel;
		params.nElements += nElems1D * nElems1D;
	}
	params.elements = (TElement*)calloc(params.nElements, sizeof(TElement));


	// [CS580 GLOBAL ILLUMINATION] YOUR CODE HERE
	// TASK#4 Substructuring 
	// SUBTASK(#2) Modify the number of total points(nPoints).
	// Refer to the above code, where the number of patces is computed.( you may want to use roomPolys[i].elementLevel. )
	// For now nPoints = (nPatches + 1) * (nPatches + 1), because an element does not produce additional points.
	// Fix this so that nPoints can count the additional points due to elements.
	/* compute the total number of element vertices */
	params.nPoints = 0;
	for (int i = 0; i < numberOfPolys; ++i) {
		int nPts1D = roomPolys[i].elementLevel * roomPolys[i].patchLevel + 1;
		params.nPoints += nPts1D * nPts1D;
	}	
	params.points = (TPoint3f*)calloc(params.nPoints, sizeof(TPoint3f));

	/* mesh the room to patches and elements */
	iOffset = 0;
	pPatch= params.patches;
	pElement= params.elements;
	pPoint= params.points;

	params.nQuads = numberOfPolys;
	params.quadElemDim = (int*) calloc(numberOfPolys, sizeof(int));

	for (int i=0; i<numberOfPolys; i++)
		MeshQuad(i);
	
	params.displayView.buffer= (IDENTIFIER*)calloc(
		params.displayView.xRes*params.displayView.yRes, sizeof(IDENTIFIER));
	params.displayView.wid=0;

        return &params;
}
