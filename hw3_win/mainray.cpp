#ifdef WIN32
#include <windows.h>
#endif

#include "debug.h"

// Maximum number of times a ray bounces. If set to 1 you will only get local illumination.
#define MAX_PATH_BOUNCES 5

#include <GL/glut.h>
#include <GMmatlib.h>
#include "litscene.h"
#include "time.h"
#include "simplecamera.h"
#include "brdf.h"
#include "sphere.h"
#include "direct.h"

// Debugging Info ---
const char* friendlyTime(char time[9],double tsecs);
// ------------------

const int xSZ=250;	// Image X-resolution
const int ySZ=250;	// Image Y-resolution
const int previewStep=10;
LitScene TheScene(MAX_PATH_BOUNCES);
SimpleCamera TheCamera(xSZ,ySZ);
int Nx,Ny;				// set resolution
GLfloat* raytraceBuffer=NULL;
static char filename[256];

// this adjust the number of rays sent per pixel
static const int N_RAYS_PER_PIXEL=64; // 64

// use this to scale the image radiance for display, if you get a dark image try adjusting this
static float DISPLAY_SCALE=30.0f;

void keyboard(unsigned char key,int x,int y);
void glutResize(int w, int h);
void set_pixel(int x, int y, Colour col)
{
	glBegin(GL_POINTS);
	glColor3f(col.red(), col.green(), col.blue());
	glVertex2i(x, y);
	glEnd();
}

static char title[256];
static char timeS[9];
static int irt=0, jrt=0;
static bool done=false;
static Colour col;
static time_t start;
static double estimate_dur=0.0;
static double dur=0.0;


void screenshot(int windowWidth, int windowHeight, char* filename)
{
	int imageSize = windowWidth*windowHeight*4;
	char*  bmpBuffer = (char*)malloc(imageSize);
	if  (!bmpBuffer)
		return;

	glReadPixels((GLint)0, (GLint)0,(GLint)windowWidth, (GLint)windowHeight, GL_RGBA, GL_UNSIGNED_BYTE, bmpBuffer );

	FILE *filePtr = fopen( filename, "wb" );
	if (!filePtr)
		return;

	BITMAPFILEHEADER  bitmapFileHeader;
	BITMAPINFOHEADER  bitmapInfoHeader;

	bitmapFileHeader.bfType = 0x4D42;  //"BM"
	bitmapFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + imageSize;
	bitmapFileHeader.bfReserved1 = 0;
	bitmapFileHeader.bfReserved2 = 0;
	bitmapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) +  sizeof(BITMAPINFOHEADER);

	bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfoHeader.biWidth = windowWidth;
	bitmapInfoHeader.biHeight = windowHeight;
	bitmapInfoHeader.biPlanes = 1;
	bitmapInfoHeader.biBitCount  = 32;
	bitmapInfoHeader.biCompression = 0; //BI_RGB;
	bitmapInfoHeader.biSizeImage = imageSize;
	bitmapInfoHeader.biXPelsPerMeter = 0; // ?
	bitmapInfoHeader.biYPelsPerMeter = 0; //  ?
	bitmapInfoHeader.biClrUsed = 0;
	bitmapInfoHeader.biClrImportant =  0;

	fwrite( &bitmapFileHeader, 1, sizeof(BITMAPFILEHEADER), filePtr );
	fwrite( &bitmapInfoHeader, 1, sizeof(BITMAPINFOHEADER), filePtr );

	// Switch Red and Blue channels as source is RGB not BGR
	BYTE tmp;
	for ( int i = 0; i < imageSize; i+=4 ) 
	{
		tmp = bmpBuffer[i];
		bmpBuffer[i] = bmpBuffer[i+2];
		bmpBuffer[i+2] = tmp;
	}

	fwrite( bmpBuffer, 1, imageSize, filePtr );

	fclose(filePtr);

	free(bmpBuffer);
}

void idle(void)
{
	if (done) return;

	// compute image
	Colour col, avg_col, sum_col;
	Ray ray;
	for (int i=0; i<Nx; i++)
	{
		// get colour for this pixel
		avg_col=TheScene.renderPixel(i,Ny-jrt-1,TheCamera,N_RAYS_PER_PIXEL)* DISPLAY_SCALE;

    float gamma = 0.45454f;

    // set colour in buffer and on screen
    raytraceBuffer[((Ny-jrt-1)*Nx+i)*3]  =pow(avg_col.red(),gamma);
    raytraceBuffer[((Ny-jrt-1)*Nx+i)*3+1]=pow(avg_col.green(),gamma);
    raytraceBuffer[((Ny-jrt-1)*Nx+i)*3+2]=pow(avg_col.blue(),gamma);
		//set_pixel(i,(Ny-jrt-1),avg_col);
	}

	jrt++;	// next row

	// check if we have finished tracing all rows
	if (jrt==Ny)
	{
		time_t now=clock();
		dur=(double)(now-start)/CLOCKS_PER_SEC;
		friendlyTime(timeS,dur);
		done=true;
		std::cout << "Done raytracing frame!" << std::endl;
	}

	glutPostRedisplay();
}

void display(void)
{
	if (done)
		sprintf(title,"PT done in %s",timeS);
	else
	{
		if (jrt==0)
			start=clock();
		else
		{
			time_t now=clock();
			dur=(double)(now-start)/CLOCKS_PER_SEC;
			estimate_dur=dur/((double)jrt/Ny);
		}
		char time1[9];
		char time2[9];
		sprintf(title,"%.1f%%, time (%s / %s)",100.0f*((float)jrt/Ny),friendlyTime(time1,dur),friendlyTime(time2,estimate_dur)); 
	}

	glClear(GL_DEPTH_BUFFER_BIT);
	glRasterPos2i(0,0);
	glutSetIconTitle(title);
	glutSetWindowTitle(title);
	glDrawPixels(Nx,Ny,GL_RGB,GL_FLOAT,raytraceBuffer);	// set pixels from buffer to screen
	glFlush();

	//if( done )
	//{
	//	// output frame buffer to image
	//	char output[64];
	//	sprintf( output, "armacw1%ld.bmp", time( NULL ) );
	//	screenshot( xSZ, ySZ, output );
	//}
}

void glutResize(int w, int h)
{
	if (w!=Nx || h!=Ny) glutReshapeWindow(Nx,Ny);	// if resized, snap back to original size/shape
	glutPostRedisplay ();
}

void create_window(void)
{
	Nx = TheCamera.xResolution();
	Ny = TheCamera.yResolution();

	// create a window with the desired size and title
	glutInitWindowSize(Nx,Ny);
	glutCreateWindow("Path tracer");

	raytraceBuffer=new GLfloat[Nx*Ny*3];	// allocate space for buffer to hold colour data
	if (raytraceBuffer==NULL) exit(1);

	// fill ray-trace buffer with random junk
	for (int y=0;y<Ny;y++)
		for (int x=0;x<Nx;x++)
		{
			raytraceBuffer[(y*Nx+x)*3]  =frand();
			raytraceBuffer[(y*Nx+x)*3+1]=frand();
			raytraceBuffer[(y*Nx+x)*3+2]=frand();
		}

		// specify the function to be used for rendering in this window
		glutDisplayFunc(display);
		glutIdleFunc(idle);
		glutKeyboardFunc(keyboard);
		glutReshapeFunc(glutResize);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		gluOrtho2D(0.0,(double)Nx-1,0.0,(double)Ny-1);
		glViewport(0,0,Nx,Ny);
}

void usage(void)
{
	std::cout << "usage: pathtracer.exe <filename>" << std::endl;
}

/***************************************************************************/
/* The main function */
/***************************************************************************/
int main(int argc, char **argv)
{
	// initialise the window manager library
	glutInit(&argc, argv);

	// set up the camera - don't change these unless you understand what the parameters do
	TheCamera.setResolution(xSZ,ySZ);
	TheCamera.setVPWindow(-2.0f,2.0f,-2.0f,2.0f);
	TheCamera.zcop() =(2.0f+6.0f);

	char* arg_filename;

	if (argc<2) 
	{
		arg_filename = "../data/data.dat";
	}
	else 
	{
		arg_filename = argv[1];
	}
	if (!TheScene.load(arg_filename)) return 1;

	strcpy(filename, arg_filename);

	// open a window of the specified size
	create_window();

	// enter the event loop
	glutMainLoop();
};


void keyboard(unsigned char key,int x,int y)
{
	// nothing
}

const char* friendlyTime(char time[9],double tsecs)
{
	double rems=tsecs;
	int hrs=0,mins=0,secs=0;
	hrs=(int)(rems/3600.0);rems=rems-hrs*3600;
	mins=(int)(rems/60.0);rems=rems-mins*60;
	secs=(int)rems;
	sprintf(time,"%02d:%02d:%02d",hrs,mins,secs);
	return time;
}
