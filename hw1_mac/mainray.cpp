#include <GL/freeglut.h>

#include "litscene.h"
#include "simplecamera.h"

LitScene TheScene;
SimpleCamera TheCamera(250, 250);
int Nx, Ny;//resolution

#define DEPTH 2

void set_pixel(int x, int y, Colour col)
{
  glBegin(GL_POINTS);
  glColor3f(col.red(), col.green(), col.blue());
  glVertex2i(x, y);
  glEnd();
}

void display(void)
{
  int i, j;
  Colour col;

  /* clear the window, set it to the background colour */
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f); /* background colour = black */
  glClear(GL_COLOR_BUFFER_BIT);

  for (i = 0; i < Nx; i++)
    for (j = 0; j < Ny; j++) {
      Ray ray = TheCamera.ray(i, j);
      if (TheScene.intersect(NULL, ray, col, DEPTH, i, j))
      {
        set_pixel(i, j, col);
      }
    }

  /* force any unfinished OpenGL commands to be done */
  glFlush();

  cout << "Finished\n";
}



void create_window(void)
{
  Nx = TheCamera.xResolution();
  Ny = TheCamera.yResolution();


  /* create a window with the desired size and title */
  glutInitWindowSize(Nx, Ny);
  glutCreateWindow("simple graphics");

  /* specify the function to be used for rendering in this window */
  glutDisplayFunc(display);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  gluOrtho2D(0.0, (double)Nx - 1, 0.0, (double)Ny - 1);
  glViewport(0, 0, Nx, Ny);
}

/***************************************************************************/
/* The main function */
/***************************************************************************/
int main(int argc, char **argv)
{
  /* initialise the window manager library */
  glutInit(&argc, argv);

  /* set up the camera */
  //TheCamera.setResolution(250,250);
  TheCamera.setResolution(512, 512);
  TheCamera.setVPWindow(-2.0, 2.0, -2.0, 2.0);
  TheCamera.zcop() = 2.5;

  /* read the scene from standard input*/
  cin >> TheScene;

  cout << TheScene << "\n";
  /* open a window of the specified size */
  create_window();

  /* enter the event loop */
  glutMainLoop();
}
