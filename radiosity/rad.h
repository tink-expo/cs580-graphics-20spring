
/*****************************************************************************
*	rad.h
*
*	This is the headerfile which defines the data structures used in rad.c
*
* 	Copyright (C) 1990-1991 Apple Computer, Inc.
* 	All rights reserved.
*
*	12/1990 S. Eric Chen	
******************************************************************************/

#ifndef __RAD__
#define __RAD__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include <GL/glut.h> 
extern GLint bits_for_RGB;
#define kNumberOfRadSamples	3

#define IDENTIFIER unsigned long int

typedef struct { float x, y, z; } TPoint3f;
typedef TPoint3f TVector3f;
typedef struct { unsigned char a, r, g, b;} TColor32b;
typedef struct { 
	int r, g, b;
	int count; 
} TColorSum;

typedef struct {
	double samples[kNumberOfRadSamples];
} TSpectra;

typedef struct {
	TSpectra* reflectance; /* diffuse reflectance of the patch */
	TSpectra* emission;	/* emission of the patch */
	TPoint3f center;	/* center of the patch where hemi-cubes will be placed */
	TVector3f normal;	/* normal of the patch; for orienting the hemi-cube */
	TSpectra unshotRad;	/* unshot radiosity of the patch */
	double area;	/* area of the patch */
} TPatch;

typedef struct {
	unsigned short nVerts;	/* number of vertices of the element */
	unsigned long* verts;	/* vertices */
	TVector3f normal;	/* normal of the element; for backface removal */
	TSpectra rad;	/* total radiosity of the element */
	double area;	/* area of the patch */
	TPatch* patch;	/* pointer to the parent patch */

	TColor32b color;
} TElement;

typedef struct {
	TPoint3f camera;	/* camera location */
	TPoint3f lookat;	/* point of interest */
	TVector3f up;	/* view up vector */
	float fovx, fovy;	/* field of view in x, y (in degree) */
	float near_dist, far_dist;	/* distance from the camera to the near and far planes */
	unsigned short xRes, yRes;	/* resolution of the frame buffer */
	IDENTIFIER* buffer;	/* pointer to the frame buffer */
	long wid;	/* id or pointer to the window associated with the view */
} TView;	

/* Radiosity input parameters */
typedef struct {
	double threshold; /* convergence threshold (fraction of the total emitted 						energy) */
	unsigned long nPatches; /* number of patches */
	TPatch *patches;	/* patches */
	unsigned long nElements; /* number of elements */
	TElement *elements;	/* elements */

	unsigned long nPoints;	/* number of element vertices */
	TPoint3f *points;	/* element vertices */

	TView displayView;	/* view to display the results */
	unsigned short hemicubeRes; /* hemi-cube resolution */
	float worldSize;	/* approximate diameter of the bounding sphere of the world.  			used for placing near and far planes in the hemi-cube computation*/
	float intensityScale;	/* used to scale intensity for display */
	int	addAmbient;		/* whether or not to add the ambient approximation in display */

	TColorSum *pointColorSums;
} TRadParams;


/* Initialize input parameters */
extern TRadParams *InitParams(void);

/* initialization */
void InitRad(TRadParams *p, TView **displayview, TView **hemiview);
/* main iterative loop */
void DoRad();
/* final clean up */
void CleanUpRad();

/* The following routines should be provided by the user */

/* Clear buffer. Set up view transformation.*/
/* Open a window if necessary (check if view->wid is zero) */
void BeginHCDraw(
TView *view, 	/* the viewing parameters and frame buffer to draw to*/
IDENTIFIER color, /* color used to clear the buffer */
double *plane_eq /* the plane equation of the shooting patch */
);

void BeginViewDraw(
TView *view, 	/* the viewing parameters and frame buffer to draw to*/
TColor32b *color /* color used to clear the buffer */
);

/* Draw a 3-d polygon with a constant color */
void DrawHCPolygon(
int nPts, /* number of points in the polygon */
TPoint3f *pts, /* points of the polygon */
TVector3f* normal, /* normal of the polygon */
IDENTIFIER color /* color to be drawn with */
);

/* Draw a 3-d polygon with a color */
extern void DrawViewPolygon(
int nPts, /* number of points in the polygon */
TPoint3f *pts, /* points of the polygon */
TVector3f* normal, /* normal of the polygon */
TColor32b *color  /* color to be drawn with */
);

extern void DrawViewElementInterp(
TElement* ep,
TPoint3f* points,
TColorSum* pointColorSums
);

/* Finish the drawing of polygons to the frame buffer*/
/* Display the buffer content in the window if necessary */
void EndHCDraw(TView* view);
void EndViewDraw(void);


/* NEW ADDITIONS */
    /* from room.c */
//extern void startUpRoom(void);
    /* from rad.c */
extern int doOneIteration(void);
extern void DisplayResults(TView* view);
    /* from main.c */
//extern void createBothWindows(TView* displayview, TView* hemiview);

extern int part_of_id;


#endif /* __RAD__ */

