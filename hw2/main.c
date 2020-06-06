#include "rad.h"
#include <time.h>


#define RADIOSITY_FINISHED    -1
#define PAUSE                 0
#define RUN                   1
#define DO_ONE_ITERATION      2

static int number_of_iterations, run_radiosity;
//static int bfc;
TView *displayview, *hemiview;


GLint bits_for_RGB;

/*==========================================================================
  render functions
  ==========================================================================*/
static void null_render(void)
{
        /* dummy function. ignore! */
}

static void update_other_window(void)
{
    glutSetWindow(displayview->wid);
    glutPostRedisplay();
    glutSetWindow(hemiview->wid);
}

static void single_iteration(void)
{
    int done;
    
	glutSetWindow(hemiview->wid);
    if (run_radiosity > 0) {
        done = doOneIteration();
        printf("number of iterations %d\n", ++number_of_iterations);
		if (done) {
            run_radiosity = RADIOSITY_FINISHED;
            printf("%d\n", number_of_iterations);
        }
		else if (run_radiosity == DO_ONE_ITERATION)
            run_radiosity = PAUSE;
		//[Min] for debug
		//if(number_of_iterations==4)
		//	run_radiosity = RADIOSITY_FINISHED;
		//[Min]
    }

    update_other_window();
}

static void render_results(void)
{
        /* display the current solution */
    DisplayResults(displayview);
}

static void
Key(unsigned char key, int x, int y)
{
  switch (key) {
      case 'q':
          exit(0);
      case 's':
          if (run_radiosity == RADIOSITY_FINISHED)
              printf("Cannot continue, radiosity solution is finished \n");
          else
              run_radiosity = !run_radiosity;
          break;
      default:
          printf("unkown option %c\n", key);
          break;
  }
}
 
static void
Key2(unsigned char key, int x, int y)
{
  switch (key) {
      case 'q':
          exit(0);
      default:
          printf("unkown option %c\n", key);
          glutPostRedisplay();
          break;
  }
}
 
/*==========================================================================
  creating the windows
  ==========================================================================*/

static int createHemicubeWindow(TView* hemiview)
{
    int id ;

    glutInitWindowSize(hemiview->xRes, hemiview->yRes);
    glutCreateWindow("hemicube view");
    id = glutGetWindow();


        /* initialise OpenGL */	
    glShadeModel (GL_FLAT); /* flat shading */
    glEnable(GL_DEPTH_TEST);  /*enable the depth buffer*/   
    glClearDepth(1.0); /*set the depth buffer of the depth buffer */
    glEnable(GL_CULL_FACE);  /*enable backface culling*/
 
    return id;
}

static int createRenderingWindow(TView* dv)
{
        /* create the window */
    int id ;

    glutInitWindowSize(dv->xRes, dv->yRes);
    glutCreateWindow("rendering window");
    id = glutGetWindow();

        /* initialise OpenGL */	
    glShadeModel (GL_FLAT); /* flat shading */
    glClearColor(0.0, 0.0, 0.0, 0.0); 
    glEnable(GL_DEPTH_TEST);  /*enable the depth buffer*/   
    glClearDepth(1.0); /*set the depth buffer of the depth buffer */

    glEnable(GL_CULL_FACE);  /*enable backface culling*/
 
    return id;
}

static int check_bits_per_color(void)
{
    GLint bits_per_component;
    glGetIntegerv(GL_RED_BITS, &bits_per_component);
    printf("number of bits for RED component = %d\n",(int)bits_per_component);
    if (bits_per_component == 8) {
        bits_for_RGB = 24;
        return 1;
    }
    if (bits_per_component == 3) {
        glGetIntegerv(GL_GREEN_BITS, &bits_per_component);
        printf("number of bits for GREEN component = %d\n", (int) bits_per_component);
        if (bits_per_component == 3) {
            glGetIntegerv(GL_BLUE_BITS, &bits_per_component);
            printf("number of bits for BLUE component = %d\n", (int) bits_per_component);
            if (bits_per_component == 2) {
                bits_for_RGB = 8;
                return 1;
            }
        }
    }
                bits_for_RGB = 8;
                return 1;
    return 0;
}

/*==========================================================================
  main
  ==========================================================================*/
int main(int argc, char *argv[])
{
    
    TRadParams *params;

	    /* don't show back facing polygons when rendering */
  //  bfc = 1;
    
        /* initialise the display module */
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

        /* set up the model (the cornel room) */
    params = InitParams();
        /* initialise the radiosity parameters */
    InitRad(params, &displayview, &hemiview);

        /* make the window for the hemicube */
    hemiview->wid = createHemicubeWindow(hemiview);
    glutDisplayFunc(null_render);

    glutIdleFunc(single_iteration);
	
	glutKeyboardFunc(Key);

    displayview->wid = createRenderingWindow(displayview);
    glutDisplayFunc(render_results);
    glutKeyboardFunc(Key2);
    
    if (!check_bits_per_color()) {
            printf("Number of bits per RGB colour not what expected");
            printf(" (not 24 and not 8) \n");
            printf("Don't know what to do. giving up\n");
            exit(0);
    }

    printf("press 's' on the small window to get it running\n");
    printf("press 's' again to pause or continue\n");
    
    glutMainLoop();
    CleanUpRad();
 }
