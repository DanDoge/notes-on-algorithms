#include <windows.h>
#include <stdlib.h>
#include <GL\glew.h>
#include <GL\glut.h>
#include "CudaWrapper.h"
#include "PMMUI.h"

GlobalVars global;

#define MAX_FILENAME_LENGTH 2000

GlobalVars::GlobalVars()
{
	pbo = 0;
	width = 512;
	height = 512;
	rightButtonDown = false;
	drawSinContour = true;
	numIterations = 1;
	closestSourceIndex = -1;
	numSources = 0;
	nearest = true;
}

int main(int argc, char** argv)
{
	bool result = PMM_init();
	glutInit(&argc, argv);
 	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

	char fileName[MAX_FILENAME_LENGTH];
	if(!openFile(fileName))
	{
		return 1;
	}
	if(!PMM_buildGeometry(fileName, global.width, global.height, global.numSources, global.sources, global.sourcesValues))
	{
		return 1;
	}
	glutInitWindowSize(global.width, global.height);
	glutCreateWindow("Parallel Marching Method on NVIDIA's GPU");

	if(!initGL())
	{
		return 1;
	}
	createFrameBuffer(global.width, global.height);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutDisplayFunc(display);

	update();

	glutMainLoop();
	destroyFrameBuffer();
	PMM_destroyGeometry();
	PMM_destroy();
}

bool openFile(char* fileName)
{
	char staticFileName[MAX_FILENAME_LENGTH] = {0};

	OPENFILENAMEA ofn =
	{
		sizeof(OPENFILENAMEA),
		NULL,
		NULL, 
		"Matlab Geometry Images Data Files (*.mat)\0*.mat\0All Files (*.*)\0*.*\0\0",
		NULL, 0,
		1,
		staticFileName, sizeof(staticFileName),
		NULL, 0,
		NULL,
		"Load Matlab Geometry Images data files from...",
		0,
		0,
		0,
		"mat",
		0, 
		NULL, 
		NULL
	};

	if(!GetOpenFileNameA(&ofn))
	{
		return false;
	}
	else
	{
		memcpy(fileName, staticFileName, MAX_FILENAME_LENGTH*sizeof(char));
		return true;
	}
}

bool saveFile(char* fileName)
{
	char staticFileName[MAX_FILENAME_LENGTH] = {0};

	OPENFILENAMEA ofn =
	{
		sizeof(OPENFILENAMEA),
		NULL,
		NULL, 
		"Matlab Distance Map Files (*.mat)\0*.mat\0All Files (*.*)\0*.*\0\0",
		NULL, 0,
		1,
		staticFileName, sizeof(staticFileName),
		NULL, 0,
		NULL,
		"Save Distance Map into Matlab data files...",
		0,
		0,
		0,
		"mat",
		0, 
		NULL, 
		NULL
	};

	if(!GetSaveFileNameA(&ofn))
	{
		return false;
	}
	else
	{
		memcpy(fileName, staticFileName, MAX_FILENAME_LENGTH*sizeof(char));
		return true;
	}
}

void createFrameBuffer(int width, int height)
{
	GLuint tex = 0;
	glGenBuffers(1, &global.pbo);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, global.pbo);
	glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, width*height* 4, NULL, GL_STREAM_COPY);
	PMM_cudaGLRegisterBufferObject(global.pbo);

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, global.nearest ? GL_NEAREST : GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
}

void destroyFrameBuffer()
{
	if(global.pbo != 0)
	{
		PMM_cudaGLUnregisterBufferObject(global.pbo);
		glDeleteBuffers(1, &global.pbo);
		global.pbo = 0;
	}
}

bool initGL()
{
	glewInit();
	if(!glewIsSupported("GL_VERSION_2_0"))
	{
		fprintf(stderr, "ERROR: Support for necessary OpenGL extensions missing.");
		fflush(stderr);
		return false;
	}
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glDisable(GL_DEPTH_TEST);
	return true;
}

void update()
{
	unsigned int* dDisplay = NULL;

	PMM_cudaGLMapBufferObject((void**)&dDisplay, global.pbo);
	PMM_computeAndDisplayDistanceMap(global.width, global.height, dDisplay, global.numSources, global.sources, global.sourcesValues, global.numIterations, global.drawSinContour, 0.0);
	PMM_cudaGLUnmapBufferObject(global.pbo);
	display();
}

void display()
{
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, global.width, global.height, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, 1.0f);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, 1.0f);
	glEnd();

 	glutSwapBuffers();
 	glFinish();
}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}

void keyboard(unsigned char key, int x, int y)
{
	char fileName[MAX_FILENAME_LENGTH];
	float* hostDistanceMap = NULL;

	switch(key)
	{
	case('c'):
	case('C'):
		global.drawSinContour = !global.drawSinContour;
		update();
		break;

	case('f'):
	case('F'):
		global.nearest = !global.nearest;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, global.nearest ? GL_NEAREST : GL_LINEAR);
		display();
		break;

	case 'r':
	case 'R':
		clearSourcesList();
		update();
		break;

	case '+':
	case '=':
		global.numIterations++;
		update();
		break;

	case '-':
	case '_':
		global.numIterations--;
		if(global.numIterations < 0)
		{
			global.numIterations = 0;
		}
		update();
		break;

	case 's':
	case 'S':
		if(!saveFile(fileName))
		{
			return;
		}
		PMM_cudaMallocHost((void**)(&hostDistanceMap), sizeof(float)*global.width*global.height);
		PMM_uploadDistanceMapToCPU(hostDistanceMap, global.width, global.height, true);
		PMM_saveDistanceMapToMatFile(hostDistanceMap, fileName, global.width, global.height);
		PMM_cudaFreeHost(hostDistanceMap);
		hostDistanceMap = NULL;
		break;

	case 'o':
	case 'O':
		if(!openFile(fileName))
		{
			return;
		}
		PMM_destroyGeometry();
		clearSourcesList();
		if(!PMM_buildGeometry(fileName, global.width, global.height, global.numSources, global.sources, global.sourcesValues))
		{
			return;
		}
		glutReshapeWindow(global.width, global.height);
		destroyFrameBuffer();
		createFrameBuffer(global.width, global.height);
		update();
		break;

	case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
		global.numIterations = key - '0';
		update();
		break;
	}
}

void clearSourcesList()
{
	for(int i = 0; i < MAX_SOURCES; i++)
	{
		global.sources[i].x = 0;
		global.sources[i].y = 0;
		global.sourcesValues[i] = 0.0f;
	}
	global.numSources = 0;
}

void mouse(int button, int state, int x, int y)
{
	if(!translateMouseCoordinates(x, y))
	{
		return;
	}
	switch(button)
	{
	case GLUT_LEFT_BUTTON:
		if(global.numSources >= MAX_SOURCES)
		{
			printf("Cannot add more sources\n");
			return;
		}
		if(state != GLUT_DOWN)
		{
			return;
		}
		addToSourceList(x, y);
		update();
		break;

	case GLUT_RIGHT_BUTTON:
		if(state == GLUT_DOWN)
		{
			global.rightButtonDown = true;
			findClosestSource(x, y, &(global.closestSourceIndex));
		}
		else if(state == GLUT_UP)
		{
			global.rightButtonDown = false;
		}
		break;

	default:
		break;
	}
}

bool translateMouseCoordinates(int& x, int& y)
{
	unsigned int actualWidth = glutGet(GLUT_WINDOW_WIDTH);
	unsigned int actualHeight = glutGet(GLUT_WINDOW_HEIGHT);

	x = (int)((x*global.width) / (float)actualWidth);
	y = (int)((y*global.height) / (float)actualHeight);

	//y = global.height - y - 1;
	if(x < 0 || x >= (int)(global.width) || y < 0 || y >= (int)(global.height))
	{
		return false;
	}
	else
	{
		return true;
	}
}

void addToSourceList(int x, int y)
{
	global.sources[global.numSources].x = x;
	global.sources[global.numSources].y = y;
	global.sourcesValues[global.numSources] = 0.0f;
	(global.numSources)++;
}

bool findClosestSource(int x, int y, int* closestSourceIndex)
{
	*closestSourceIndex = -1;
	if(global.numSources > 0)
	{
		float minDistance = PMM_infinity();
		for(unsigned int i = 0;  i < global.numSources; i++)
		{
			float distance_2 = (global.sources[i].x - x)*(global.sources[i].x - x) + (global.sources[i].y - y)*(global.sources[i].y - y);
			if(distance_2 < minDistance)
			{
				minDistance = distance_2;
				*closestSourceIndex = i;
			}
		}
		return true;
	}
	else
	{
		return false;
	}
}

void motion(int x, int y)
{
	if(!translateMouseCoordinates(x, y))
	{
		return;
	}
	if(global.rightButtonDown)
	{
		replaceOneSourceInSourceList(x, y, global.closestSourceIndex);
		update();
	}
}

void replaceOneSourceInSourceList(int x, int y, int closestSourceIndex)
{
	if(closestSourceIndex < (int)(global.numSources) && closestSourceIndex >= 0)
	{
		global.sources[closestSourceIndex].x = x;
		global.sources[closestSourceIndex].y = y;
	}	
}
