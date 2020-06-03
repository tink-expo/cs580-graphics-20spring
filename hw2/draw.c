
/****************************************************************************
*	draw.c
*
*	This is a skeleton polygon drawing program
*
* 	Copyright (C) 1990-1991 Apple Computer, Inc.
* 	All rights reserved.
*
*	8/27/1991 S. Eric Chen	
*****************************************************************************/
#include "rad.h"
#include <math.h>



#ifndef GL_UNSIGNED_BYTE_3_3_2_EXT
#define GL_UNSIGNED_BYTE_3_3_2_EXT GL_UNSIGNED_BYTE
#endif

static unsigned char *read_buff;
int part_of_id; /* usefull only when the display has 8-bit for RGB */

/****************************************************************************
  Hemicube functions
*****************************************************************************/
void BeginHCDraw(TView *view, IDENTIFIER color, double *plane_eq)
{
    if (!read_buff)
      {
        if (bits_for_RGB == 24)
            read_buff = (unsigned char *) calloc(view->xRes*view->yRes, 3 * sizeof(unsigned char));
        else
            read_buff = (unsigned char *) calloc(view->xRes*view->yRes, sizeof(unsigned char));
      }

	        /* clear the frame buffer with color */
    if (bits_for_RGB == 24)
        glClearColor((float)(color / 65536)/255.0,
                     (float)((color % 65536) / 256)/255.0,
                     (float)(color % 256)/ 255.0, 0.0);
    else 
        glClearColor((float)(color / 32)/7.0,
                     (float)((color % 32) / 4)/7.0,
                     (float)(color % 4)/ 3.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* set up view transformation from the parameters in view */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(view->fovx, view->fovx/view->fovy, view->near_dist, view->far_dist);
	
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(view->camera.x, view->camera.y, view->camera.z,
              view->lookat.x, view->lookat.y, view->lookat.z, 
	      view->up.x, view->up.y, view->up.z);

        /* this is to ensure that the hemicube does not send light behind
           the plane of the shooting patch */
     glClipPlane(GL_CLIP_PLANE0, plane_eq); 
     glEnable(GL_CLIP_PLANE0); 

}

void DrawHCPolygon(int nPts, TPoint3f *pts, TVector3f* n, IDENTIFIER color)
{
    int i, id;

	/* draw a polygon with the given color */
	/* buffer the polygon if you are drawing in a display list mode */
    if (bits_for_RGB == 24)
        glColor3ub(color / 65536 , (color % 65536) / 256, color % 256);
    else {
            /* We split the patch id into 2 repeat the hemicube twice and
               combine to get the overall id. The last integer says which
               part we are processing now */
        if (part_of_id == 1) {
           id =  color % 256;
           glColor3f((float)(id / 32)/7.0, (float)((id % 32) / 4)/7.0,
                     (float)(id % 4)/ 3.0);
        } else {
           id =  color / 256;
           glColor3f((float)(id / 32)/7.0, (float)((id % 32) / 4)/7.0,
                     (float)(id % 4)/ 3.0);
        }
    }
       
    glBegin(GL_POLYGON);
    i = nPts-1;
        do  
	  {
            glVertex3f(pts[i].x, pts[i].y, pts[i].z);
	    i--;
	  }
        while (i>=0);
    glEnd();
}

void EndHCDraw(TView* view)
{
    int i, j;

    glFlush();

    if (bits_for_RGB == 24) {
        glReadPixels(0, 0, view->xRes, view->yRes, GL_RGB, GL_UNSIGNED_BYTE,
                     (GLvoid *)read_buff);
        for (j=0; j<view->yRes; j++)  
            for (i=0; i<view->xRes; i++) {
                view->buffer[i * view->yRes + j] =
                    (read_buff[i * 3 * view->yRes + j * 3] * 65536) +
                    (read_buff[i * 3 * view->yRes + j * 3 + 1] * 256) +
                    read_buff[i * 3 * view->yRes + j * 3 + 2];
            }
    } else { /* 8-bit display */
        glReadPixels(0, 0, view->xRes, view->yRes, GL_RGB, 
                     GL_UNSIGNED_BYTE_3_3_2_EXT, (GLvoid *)read_buff);  
       if (part_of_id == 1)
            for (j=0; j<view->yRes; j++)  
                for (i=0; i<view->xRes; i++) 
                    view->buffer[i * view->yRes + j] = 
                        read_buff[i * view->yRes + j];
        else
            for (j=0; j<view->yRes; j++)  
                for (i=0; i<view->xRes; i++)
                    view->buffer[i * view->yRes + j] +=
                        256 * read_buff[i * view->yRes + j];
    }
    
    glDisable(GL_CLIP_PLANE0);
    glutSwapBuffers();
}

/****************************************************************************
  Rendering functions
*****************************************************************************/
void BeginViewDraw(TView *view, TColor32b *color)
{
	/* clear the frame buffer with color */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/* set up view transformation from the parameters in view */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(view->fovx, view->fovx/view->fovy, view->near_dist, view->far_dist);
	
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(view->camera.x, view->camera.y, view->camera.z,
              view->lookat.x, view->lookat.y, view->lookat.z, 
	      view->up.x, view->up.y, view->up.z); 

}

void DrawViewPolygon(int nPts, TPoint3f *pts, TVector3f* n, TColor32b *color)
{
	/* draw a polygon with the given color */
	/* buffer the polygon if you are drawing in a display list mode */
  
  // [CS580 GLOBAL ILLUMINATION] 
  // TASK#5 Smoothening
  // You better understand this function
    int i;
	glColor3ub(color->r, color->g, color->b); //[min]
	
	glBegin(GL_POLYGON);
    i = nPts-1;
    do  
	{
		glVertex3f(pts[i].x, pts[i].y, pts[i].z);
		i--;

	}while ( i >= 0);

    glEnd();
}

void EndViewDraw()
{
    glFlush();
    glutSwapBuffers();
}


