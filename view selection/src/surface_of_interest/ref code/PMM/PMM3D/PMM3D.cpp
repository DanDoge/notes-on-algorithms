#include <windows.h>
#include <GL\glew.h>
#include <GL\glut.h>
#include <float.h>
#include <iostream>

#include "CudaWrapper.h"
#include "PMM3D.h"
#include "Vector3.h"

using namespace std;

GlobalVariables gv;

#define MAX_FILENAME_LENGTH 2000

GlobalVariables::GlobalVariables()
{
}

void GlobalVariables::init()
{
	windowWidth = 1440;
	windowHeight = 900;
	parametricWidthRatio = 0.45;
	parametricWidth = parametricWidthRatio*gv.windowWidth;
	parametricHeight = gv.windowHeight;
	width = 0;
	height = 0;
	pbo = 0;
	verticesPositionsVbo = 0;
	normalsVbo = 0;
	colorsVbo = 0;
	textureVbo = 0;
	objectIbo = 0;
	rightButtonDown = false;
	leftButtonDown = false;
	middleButtonDown = false;
	rotating = false;
	translating = false;
	scaling = false;
	draggingSource = false;
	drawSinContour = true;
	numIterations = 1;
	numValidVertices = 0;
	closestSourceIndex = -1;
	numSources = 0;
	clearSourcesList();
	numIndices = 0;
	nearest = true;
	minX = PMM_infinity();
	maxX = -PMM_infinity();
	minY = PMM_infinity();
	maxY = -PMM_infinity();
	minZ = PMM_infinity();
	maxZ = -PMM_infinity();
	centerX = 0.0;
	centerY = 0.0;
	centerZ = 0.0;
	maxBB = 0.0;
	rotateX = 0.0;
	rotateY = 0.0;
	mouseOldX = 0;
	mouseOldY = 0;
	cameraPosition[0] = 0.0;
	cameraPosition[1] = 0.0;
	cameraPosition[2] = -2.5;
	pointsRenderMode = false;
	textureMapOn = true;
	uvSpace = false;
	autoRotate = false;
	autoScale = false;
	timeStep = 25;
	motionX = 0;
	motionY = 0;
	movingSources = false;
}

int main(int argc, char** argv)
{
	gv.init();
	bool result = PMM_init();
	glutInit(&argc, argv);
 	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(gv.windowWidth, gv.windowHeight);
	glutCreateWindow("Parallel Marching Method on NVIDIA's GPU");

	if(!initGL())
	{
		return 1;
	}

	timeBeginPeriod(1);

	char fileName[MAX_FILENAME_LENGTH];
	if(!openFile(fileName))
	{
		return 1;
	}
	initArrays(fileName);

	if(!PMM_buildGeometry(fileName, gv.width, gv.height, gv.numSources, gv.sources, gv.sourcesValues))
	{
		return 1;
	}
	createColorBuffer(gv.width, gv.height);

	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);

	update();

	glutMainLoop();
 	destroyBuffers();
 	PMM_destroyGeometry();
	PMM_destroy();
	timeEndPeriod(1);
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

void initArrays(char* fileName)
{
	MATFile* pMat = NULL;
	mxArray* pX = NULL;
	mxArray* pY = NULL;
	mxArray* pZ = NULL;

	mxArray* px = NULL;
	mxArray* py = NULL;

	mxArray* pV = NULL;

	bool result = PMM_loadMatFile(fileName, &pMat, gv.width, gv.height, &pX, &pY, &pZ, gv.numSources, &px, &py, &pV);
	if(result == false)
	{
		cout << "Failed to load file" << endl;
		exit(1);
	}

	int numVertices = gv.width*gv.height;

	float* vertexPositions4 = NULL;
	PMM_cudaMallocHost((void**)&vertexPositions4 ,4*sizeof(float)*numVertices);
	result = PMM_loadParametricSurfaceFromMatFile((Float4*)vertexPositions4, gv.width, gv.height, pX, pY, pZ, px, py, gv.sources, gv.numSources, gv.sourcesValues, pV);

	int* validVerticesIndices = new int[numVertices];

	gv.numValidVertices = 0;
	//vertices positions
	for(int i = 0; i < numVertices; i++)
	{
		Vector3 vertex(vertexPositions4[4*i], vertexPositions4[4*i + 1], vertexPositions4[4*i + 2]);
		if(vertex.isValid())
		{
			validVerticesIndices[i] = gv.numValidVertices;
			gv.numValidVertices++;
		}
		else
		{
			validVerticesIndices[i] = -1;
		}
	}
	float* vertexPositions3 = NULL;
	PMM_cudaMallocHost((void**)&vertexPositions3 ,3*sizeof(float)*gv.numValidVertices);
	GLubyte* vertexColors;
	PMM_cudaMallocHost((void**)&vertexColors ,3*sizeof(GLubyte)*gv.numValidVertices);

	int index = 0;
	//vertices positions
	for(int i = 0; i < numVertices; i++)
	{
		if(validVerticesIndices[i] >= 0)
		{
			Vector3 vertex(vertexPositions4[4*i], vertexPositions4[4*i + 1], vertexPositions4[4*i + 2]);

			vertexPositions3[3*index] = vertex._x;
			vertexPositions3[3*index + 1] = vertex._y;
			vertexPositions3[3*index + 2] = vertex._z;

			gv.minX = min(gv.minX, vertex._x);
			gv.maxX = max(gv.maxX, vertex._x);
			gv.minY = min(gv.minY, vertex._y);
			gv.maxY = max(gv.maxY, vertex._y);
			gv.minZ = min(gv.minZ, vertex._z);
			gv.maxZ = max(gv.maxZ, vertex._z);

			vertexColors[3*index] = (i & 0xFF);
			vertexColors[3*index + 1] = ((i >> 8) & 0xFF);
			vertexColors[3*index + 2] = ((i >> 16) & 0xFF);

			index++;
		}
	}
	gv.centerX = 0.5*(gv.minX + gv.maxX);
	gv.centerY = 0.5*(gv.minY + gv.maxY);
	gv.centerZ = 0.5*(gv.minZ + gv.maxZ);

	gv.maxBB = max(max(gv.maxX - gv.minX, gv.maxY - gv.minY), gv.maxZ - gv.minZ);

	float* normals = NULL; 
	PMM_cudaMallocHost((void**)&normals ,3*sizeof(float)*gv.numValidVertices);
	float* texCoordinates = NULL;
	PMM_cudaMallocHost((void**)(&texCoordinates), 2*sizeof(float)*gv.numValidVertices);

	index = 0;
	for(unsigned int i = 0; i < gv.height; i++)
	{
		for(unsigned int j = 0; j < gv.width; j++)
		{
			if(validVerticesIndices[gv.width*i + j] >= 0)
			{
				Vector3 normal;
				
				calculateNormal(i, j, vertexPositions4, numVertices, gv.width, gv.height, normal);
				normals[3*index] = normal._x;
				normals[3*index + 1] = normal._y;
				normals[3*index + 2] = normal._z;

				texCoordinates[2*index] = (float)j/(gv.width - 1.0);
				texCoordinates[2*index + 1] = (float)i/(gv.height - 1.0);
				index++;
			}
		}
	}

	gv.numIndices = 4*(gv.width - 1)*(gv.height - 1); //regular grid of quads
	unsigned int* objectIndices = NULL;
	PMM_cudaMallocHost((void**)(&objectIndices), gv.numIndices*sizeof(unsigned int));

	index = 0;
	for(unsigned int i = 0; i < gv.height - 1; i++)
	{
		for(unsigned int j = 0; j < gv.width - 1; j++)
		{
			int index1 = i*gv.width + j;
			int index2 = i*gv.width + j + 1;
			int index3 = (i + 1)*gv.width + j + 1;
			int index4 = (i + 1)*gv.width + j;

			//if all 4 vertices of the QUAD are valid
			if(validVerticesIndices[index1] >= 0 && validVerticesIndices[index2] >= 0 && validVerticesIndices[index3] >= 0 && validVerticesIndices[index4] >= 0)
			{
				objectIndices[4*index] = (unsigned int)validVerticesIndices[index1];
				objectIndices[4*index + 1] = (unsigned int)validVerticesIndices[index2];
				objectIndices[4*index + 2] = (unsigned int)validVerticesIndices[index3];
				objectIndices[4*index + 3] = (unsigned int)validVerticesIndices[index4];
				index++;
			}
		}
	}
	gv.numIndices = 4*index;

 	glGenBuffers(1, &gv.verticesPositionsVbo);
	glBindBuffer(GL_ARRAY_BUFFER, gv.verticesPositionsVbo);
	glBufferData(GL_ARRAY_BUFFER, gv.numValidVertices*3*sizeof(float), vertexPositions3, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gv.normalsVbo);
	glBindBuffer(GL_ARRAY_BUFFER, gv.normalsVbo);
	glBufferData(GL_ARRAY_BUFFER, gv.numValidVertices*3*sizeof(float), normals, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gv.colorsVbo);
	glBindBuffer(GL_ARRAY_BUFFER, gv.colorsVbo);
	glBufferData(GL_ARRAY_BUFFER, gv.numValidVertices*3*sizeof(GLubyte), vertexColors, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gv.textureVbo);
	glBindBuffer(GL_ARRAY_BUFFER, gv.textureVbo);
	glBufferData(GL_ARRAY_BUFFER, gv.numValidVertices*2*sizeof(float), texCoordinates, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &gv.objectIbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gv.objectIbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, gv.numIndices*sizeof(unsigned int), objectIndices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



	PMM_cudaFreeHost(vertexPositions4);
	vertexPositions4 = NULL;
	PMM_cudaFreeHost(vertexPositions3);
	vertexPositions3 = NULL;
	PMM_cudaFreeHost(texCoordinates);
	texCoordinates = NULL;
	PMM_cudaFreeHost(objectIndices);
	objectIndices = NULL;
	PMM_cudaFreeHost(normals);
	normals = NULL;
	PMM_cudaFreeHost(vertexColors);
	vertexColors = NULL;
	delete[] validVerticesIndices;
	validVerticesIndices = NULL;
}

void createColorBuffer(int width, int height)
{
	GLuint tex = 0;
	glGenBuffers(1, &gv.pbo);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, gv.pbo);
	glBufferData(GL_PIXEL_UNPACK_BUFFER_ARB, width*height*4, NULL, GL_DYNAMIC_COPY);
	PMM_cudaGLRegisterBufferObject(gv.pbo);

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
}

void destroyBuffers()
{
	if(gv.pbo != 0)
	{
		PMM_cudaGLUnregisterBufferObject(gv.pbo);
		glDeleteBuffers(1, &gv.pbo);
		gv.pbo = 0;
	}
	if(gv.verticesPositionsVbo != 0)
	{
		glDeleteBuffers(1, &gv.verticesPositionsVbo);
		gv.verticesPositionsVbo = 0;
	}
	if(gv.normalsVbo != 0)
	{
		glDeleteBuffers(1, &gv.normalsVbo);
		gv.normalsVbo = 0;
	}
	if(gv.colorsVbo != 0)
	{
		glDeleteBuffers(1, &gv.colorsVbo);
		gv.colorsVbo = 0;
	}
	if(gv.textureVbo != 0)
	{
		glDeleteBuffers(1, &gv.textureVbo);
		gv.textureVbo = 0;
	}
	if(gv.objectIbo != 0)
	{
		glDeleteBuffers(1, &gv.objectIbo);
		gv.objectIbo = 0;
	}
}

bool initGL()
{
	glewInit();
	if(!glewIsSupported("GL_VERSION_2_0"))
	{
		cout << "ERROR: Support for necessary OpenGL extensions missing." << endl;
		return false;
	}
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_NORMALIZE);
	glPointSize(1.0f);
	//glEnable(GL_POINT_SMOOTH);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_TEXTURE_2D);

	GLint redBits, greenBits, blueBits;
	glGetIntegerv(GL_RED_BITS, &redBits);
	glGetIntegerv(GL_GREEN_BITS, &greenBits);
	glGetIntegerv(GL_BLUE_BITS, &blueBits);
	if(!(redBits == 8 && greenBits == 8 && blueBits == 8))
	{
		return false;
	}

	return true;
}

void update()
{
	unsigned int* dDisplay = NULL;

	PMM_cudaGLMapBufferObject((void**)&dDisplay, gv.pbo);
	PMM_computeAndDisplayDistanceMap(gv.width, gv.height, dDisplay, gv.numSources, gv.sources, gv.sourcesValues, gv.numIterations, gv.drawSinContour, 0.5f*gv.maxBB);
	PMM_cudaGLUnmapBufferObject(gv.pbo);
	display();
}

int renderPickColorsToBackBuffer(int x, int y)
{
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_DITHER);
	glDisable(GL_FOG);
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_1D);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_3D);
	glShadeModel(GL_FLAT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	GLint viewport[] = {x, y, 1, 1};
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	viewport[0] = 0;
	viewport[1] = 0;
	viewport[2] = gv.windowWidth;
	viewport[3] = gv.windowHeight;
	gluPickMatrix(x, y, 1, 1, viewport);
	gluPerspective(40.0, (double)gv.windowWidth / (double)gv.windowHeight, 0.01*gv.maxBB, 100.0*gv.maxBB);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslated(gv.cameraPosition[0], gv.cameraPosition[1], gv.centerZ + gv.cameraPosition[2]*gv.maxBB);
	glRotated(gv.rotateX, 0.0, 1.0, 0.0);
	glRotated(gv.rotateY, 1.0, 0.0, 0.0);
	glTranslated(-gv.centerX, -gv.centerY, -gv.centerZ);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, gv.verticesPositionsVbo);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, gv.colorsVbo);
	glColorPointer(3, GL_UNSIGNED_BYTE, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gv.objectIbo);

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawElements(GL_QUADS, gv.numIndices, GL_UNSIGNED_INT, NULL);

	//unbind vbo, ibo and pbo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	GLubyte pixels[3] = {0};
	glReadPixels(x, y, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	int index = (pixels[2] << 16) | (pixels[1] << 8) | (pixels[0]);

//	glutSwapBuffers();
	glFinish();

	return index;
}

void display()
{
	DWORD start = timeGetTime();

	gv.textureMapOn ? glEnable(GL_TEXTURE_2D) : glDisable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);

	glViewport(0, 0, gv.windowWidth, gv.windowHeight);

	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, gv.pbo);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, gv.width, gv.height, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(gv.uvSpace)
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		cout << gv.windowWidth << gv.windowHeight << endl;
		gluOrtho2D(-gv.windowWidth / 2.0, gv.windowWidth / 2.0, -gv.windowHeight / 2.0, gv.windowHeight / 2.0);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glDisable(GL_LIGHTING);
		glDisable(GL_DEPTH_TEST);
		glPolygonMode(GL_FRONT, GL_FILL);

		glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f); glVertex2d(-gv.windowWidth / 2.0, -gv.windowHeight / 2.0);
		glTexCoord2f(1.0f, 1.0f); glVertex2d(-gv.windowWidth / 2.0 + gv.parametricWidth, -gv.windowHeight / 2.0);
		glTexCoord2f(1.0f, 0.0f); glVertex2d(-gv.windowWidth / 2.0 + gv.parametricWidth, -gv.windowHeight / 2.0 + gv.parametricHeight);
		glTexCoord2f(0.0f, 0.0f); glVertex2d(-gv.windowWidth / 2.0, -gv.windowHeight / 2.0 + gv.parametricHeight);
		glEnd();
	}

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	GLfloat mat_specular[] = {0.3, 0.3, 0.3, 1.0};
	GLfloat mat_ambient[] = {0.25, 0.25, 0.25, 1.0};
	GLfloat mat_diffuse[] = {0.85, 0.85, 0.85, 1.0};
	GLfloat mat_shininess[] = {8.0};

	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, 1.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(40.0, (double)gv.windowWidth / (double)gv.windowHeight, 0.01*gv.maxBB, 100.0*gv.maxBB);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	GLfloat light_position[] = {3.0*gv.maxBB, 4.0*gv.maxBB, 6.0*gv.maxBB, 1.0};
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glTranslated(gv.cameraPosition[0], gv.cameraPosition[1], gv.centerZ + gv.cameraPosition[2]*gv.maxBB);
	glRotated(gv.rotateX, 0.0, 1.0, 0.0);
	glRotated(gv.rotateY, 1.0, 0.0, 0.0);
	glTranslated(-gv.centerX, -gv.centerY, -gv.centerZ);

	if(gv.pointsRenderMode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glBindBuffer(GL_ARRAY_BUFFER, gv.verticesPositionsVbo);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, gv.normalsVbo);
	glNormalPointer(GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, gv.textureVbo);
	glTexCoordPointer(2, GL_FLOAT, 0, NULL);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gv.objectIbo);

	glDrawElements(GL_QUADS, gv.numIndices, GL_UNSIGNED_INT, NULL);

	//unbind vbo, ibo and pbo
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

 	glutSwapBuffers();
 	glFinish();

	DWORD end = timeGetTime();
	DWORD time = end - start;
}

bool calculateNormal(int i, int j, float* vertexPositions, unsigned int numVertices, int width, int height, Vector3& normal)
{
// 	normal._x = 0;
// 	normal._y = 0;
// 	normal._z = 1;
// 	return true;



	int xCurr = j;
	int yCurr = i;
	int xNext = min(j + 1, width - 1);
	int yNext = min(i + 1, height - 1);
	int xPrev = max(j - 1, 0);
	int yPrev = max(i - 1, 0);

	Vector3 v0(vertexPositions[4*width*yCurr + 4*xCurr], vertexPositions[4*width*yCurr + 4*xCurr + 1], vertexPositions[4*width*yCurr + 4*xCurr + 2]);
	Vector3 v1(vertexPositions[4*width*yNext + 4*xCurr], vertexPositions[4*width*yNext + 4*xCurr + 1], vertexPositions[4*width*yNext + 4*xCurr + 2]);
	Vector3 v2(vertexPositions[4*width*yCurr + 4*xNext], vertexPositions[4*width*yCurr + 4*xNext + 1], vertexPositions[4*width*yCurr + 4*xNext + 2]);
	Vector3 v3(vertexPositions[4*width*yPrev + 4*xCurr], vertexPositions[4*width*yPrev + 4*xCurr + 1], vertexPositions[4*width*yPrev + 4*xCurr + 2]);
	Vector3 v4(vertexPositions[4*width*yCurr + 4*xPrev], vertexPositions[4*width*yCurr + 4*xPrev + 1], vertexPositions[4*width*yCurr + 4*xPrev + 2]);

	Vector3 v10 = v1 - v0;
	Vector3 v20 = v2 - v0;
	Vector3 v30 = v3 - v0;
	Vector3 v40 = v4 - v0;

	Vector3 n1 = v20.cross(v10);
	Vector3 n2 = v30.cross(v20);
	Vector3 n3 = v40.cross(v30);
	Vector3 n4 = v10.cross(v40);

	normal = (n1 + n2 + n3 + n4) / 4.0;
	normal.normalize();
	return true;
}

void reshape(int width, int height)
{
	gv.windowWidth = width;
	gv.windowHeight = height;
	gv.parametricWidth = gv.parametricWidthRatio*gv.windowWidth;
	gv.parametricHeight = gv.windowHeight;
	display();
}

void keyboard(unsigned char key, int x, int y)
{
	char fileName[MAX_FILENAME_LENGTH];
	float* hostDistanceMap = NULL;

	switch(key)
	{
	case('c'):
	case('C'):
		gv.drawSinContour = !gv.drawSinContour;
		update();
		break;
	case('a'):
	case('A'):
		gv.autoRotate = !gv.autoRotate;
		if(gv.autoRotate)
		{
			glutTimerFunc(gv.timeStep, rotateTimer, 0);
		}
		break;

	case('k'):
	case('K'):
		gv.autoScale = !gv.autoScale;
		if(gv.autoScale)
		{
			glutTimerFunc(gv.timeStep, scaleTimer, 0);
		}
		break;

	case('f'):
	case('F'):
		gv.nearest = !gv.nearest;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gv.nearest ? GL_NEAREST : GL_LINEAR);
		display();
		break;

	case 'r':
	case 'R':
		clearSourcesList();
		update();
		break;

	case 'u':
	case 'U':
		gv.uvSpace = !gv.uvSpace;
		display();
		break;

	case '+':
	case '=':
		gv.numIterations++;
		update();
		break;

	case '-':
	case '_':
		gv.numIterations--;
		if(gv.numIterations < 0)
		{
			gv.numIterations = 0;
		}
		update();
		break;

	case 's':
	case 'S':
		if(!saveFile(fileName))
		{
			return;
		}
		PMM_cudaMallocHost((void**)(&hostDistanceMap), sizeof(float)*gv.width*gv.height);
		PMM_uploadDistanceMapToCPU(hostDistanceMap, gv.width, gv.height, true);
		PMM_saveDistanceMapToMatFile(hostDistanceMap, fileName, gv.width, gv.height);
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
		destroyBuffers();
		gv.init();
		gv.windowWidth = glutGet(GLUT_WINDOW_WIDTH);
		gv.windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
		gv.parametricHeight = gv.windowHeight;

		initArrays(fileName);

		if(!PMM_buildGeometry(fileName, gv.width, gv.height, gv.numSources, gv.sources, gv.sourcesValues))
		{
			return;
		}
		createColorBuffer(gv.width, gv.height);
		update();
		break;

	case 'p':
	case 'P':
		if(gv.pointsRenderMode)
		{
			gv.pointsRenderMode = false;
		}
		else
		{
			gv.pointsRenderMode = true;
		}
		display();
		break;

	case 't':
	case 'T':
		gv.textureMapOn = !gv.textureMapOn;
		display();
		break;

	case 'z':
	case 'Z':
		gv.movingSources = !gv.movingSources;
		if(gv.movingSources)
		{
			glutTimerFunc(0, animateSources, 0);
		}
		break;

	case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9':
		gv.numIterations = key - '0';
		update();
		break;
	}
}

void clearSourcesList()
{
	for(int i = 0; i < MAX_SOURCES; i++)
	{
		gv.sources[i].x = 0;
		gv.sources[i].y = 0;
		gv.sourcesValues[i] = 0.0f;
	}
	gv.numSources = 0;
}

void motion(int x, int y)
{
	gv.motionX = x;
	gv.motionY = y;

	if(gv.draggingSource)
	{
		glutTimerFunc(10, replaceOneSourceTimer, 0);
		return;
	}
	double dx, dy;
	dx = x - gv.mouseOldX;
	dy = y - gv.mouseOldY;

	if(gv.rotating)
	{
		gv.rotateX += dx*0.3;
		gv.rotateY += dy*0.3;
			
		gv.mouseOldX = x;
		gv.mouseOldY = y;
		display();
	}
	if(gv.translating)
	{
		gv.cameraPosition[0] += dx*gv.maxBB*0.005;
		gv.cameraPosition[1] -= dy*gv.maxBB*0.005;

		gv.mouseOldX = x;
		gv.mouseOldY = y;
		display();
	}
	if(gv.scaling)
	{
		//gv.cameraPosition[2] += dx*gv.maxBB*0.001;
		gv.cameraPosition[2] += dx*0.001;
		gv.mouseOldX = x;
		gv.mouseOldY = y;
		display();
	}
}

void mouse(int button, int state, int x, int y)
{
	int modifier = glutGetModifiers();
	switch(button)
	{
	case GLUT_LEFT_BUTTON:
		if(state == GLUT_DOWN)
		{
			gv.leftButtonDown = true;
			//rotate model
			if(modifier == GLUT_ACTIVE_ALT)
			{
				gv.mouseOldX = x;
				gv.mouseOldY = y;
				gv.rotating = true;
			}
			else //add source point
			{
				if(gv.numSources >= MAX_SOURCES)
				{
					cout << "Cannot add more sources" << endl;
					return;
				}
				if(gv.uvSpace)
				{
					translateMouseCoordinates(x, y);
				}
				else{
					int index = renderPickColorsToBackBuffer(x, gv.windowHeight - y - 1);
					x = index % gv.width;
					y = index / gv.width;
				}
				addToSourceList(x, y);
 				update();
			}
		}
		else if(state == GLUT_UP)
		{
			gv.rotating = false;
			gv.leftButtonDown = false;
		}
		break;

	case GLUT_MIDDLE_BUTTON:
		if(state == GLUT_DOWN)
		{
			gv.middleButtonDown = true;
			//translate model
			if(modifier == GLUT_ACTIVE_ALT)
			{
				gv.mouseOldX = x;
				gv.mouseOldY = y;
				gv.translating = true;
			}
		}
		else if(state == GLUT_UP)
		{
			gv.translating = false;
			gv.middleButtonDown = false;
		}
		break;

	case GLUT_RIGHT_BUTTON:
		if(state == GLUT_DOWN)
		{
			gv.rightButtonDown = true;
			//scaling model
			if(modifier == GLUT_ACTIVE_ALT)
			{
				gv.mouseOldX = x;
				gv.mouseOldY = y;
				gv.scaling = true;
			}
			else
			{
				gv.draggingSource = true;
				findClosestSource(x, y, &(gv.closestSourceIndex));
			}
		}
		else if(state == GLUT_UP)
		{
			gv.rightButtonDown = false;
			gv.draggingSource = false;
			gv.scaling = false;
		}
		break;

	default:
		break;
	}
}

bool translateMouseCoordinates(int& x, int& y)
{
	bool result = true;

	if(gv.uvSpace)
	{
		x = (int)(((float)x*(float)gv.width) / (float)gv.parametricWidth);
		y = (int)(((float)y*(float)gv.height) / (float)gv.parametricHeight);


		if(x < 0)
		{
			x = 0;
			result = false;
		}
		if(x >= (int)(gv.width))
		{
			x = gv.width - 1;
			result = false;
		}
		if(y < 0)
		{
			y = 0;
			result = false;
		}
		if(y >= (int)(gv.height))
		{
			y = gv.height - 1;
			result = false;
		}
	}
	return result;
}

void addToSourceList(int x, int y)
{
	if(x >= 0 && y >= 0 && x < (int)gv.width && y < (int)gv.height)
	{
		gv.sources[gv.numSources].x = x;
		gv.sources[gv.numSources].y = y;
		gv.sourcesValues[gv.numSources] = 0.0f;
		(gv.numSources)++;
	}
}

bool findClosestSource(int x, int y, int* closestSourceIndex)
{
	if(gv.uvSpace)
	{
		translateMouseCoordinates(x, y);
	}
	else
	{
		int index = renderPickColorsToBackBuffer(x, gv.windowHeight - y - 1);
		x = index % gv.width;
		y = index / gv.width;
	}
	*closestSourceIndex = -1;
	if(gv.numSources > 0)
	{
		float minDistance = PMM_infinity();
		for(unsigned int i = 0;  i < gv.numSources; i++)
		{
			float distance_2 = (gv.sources[i].x - x)*(gv.sources[i].x - x) + (gv.sources[i].y - y)*(gv.sources[i].y - y);
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

void replaceOneSourceInSourceList(int x, int y, int closestSourceIndex)
{
	translateMouseCoordinates(x, y);
	if(closestSourceIndex < (int)(gv.numSources) && closestSourceIndex >= 0 && x >= 0 && y >= 0 && x < (int)gv.width && y < (int)gv.height)
	{
		gv.sources[closestSourceIndex].x = x;
		gv.sources[closestSourceIndex].y = y;
	}	
}

void rotateTimer(int value)
{
	if(gv.autoRotate)
	{
		gv.rotateX+=1.0;
		display();
		glutTimerFunc(gv.timeStep, rotateTimer, 0);
	}
}

void scaleTimer(int value)
{
	if(gv.autoScale)
	{
		gv.cameraPosition[2] += gv.maxBB*0.01;
		display();
		glutTimerFunc(gv.timeStep, scaleTimer, 0);
	}
}

void replaceOneSourceTimer(int value)
{
	int x = gv.motionX;
	int y = gv.motionY;

	//if we are outside the windows boundary, do not anything
	if(x < 0 || x >= gv.windowWidth || y < 0 || y >= gv.windowHeight)
	{
		return;
	}
	if(!gv.uvSpace)
	{
		int index = renderPickColorsToBackBuffer(x, gv.windowHeight - y - 1);
		x = index % gv.width;
		y = index / gv.width;
	}
	replaceOneSourceInSourceList(x, y, gv.closestSourceIndex);
	update();
}

void animateSources(int value)
{
	if(gv.movingSources)
	{
		int step = int(0.005*(gv.width + gv.height));
		step = max(step, 1);

		for(int i = 0; i < gv.numSources; i++)
		{
			double upDown = rand()/(double)RAND_MAX;
			double leftRight = rand()/(double)RAND_MAX;
			
			if(upDown < 0.5)
			{
				gv.sources[i].x += step;
			}
			else
			{
				gv.sources[i].x -= step;
			}
			if(leftRight < 0.5)
			{
				gv.sources[i].y += step;
			}
			else
			{
				gv.sources[i].y -= step;
			}

			if(gv.sources[i].x >= gv.width)
			{
				gv.sources[i].x = gv.width - 1;
			}
			if(gv.sources[i].y >= gv.height)
			{
				gv.sources[i].y = gv.height - 1;
			}
			if(gv.sources[i].x < 0)
			{
				gv.sources[i].x = 0;
			}
			if(gv.sources[i].y < 0)
			{
				gv.sources[i].y = 0;
			}
		}
		glutTimerFunc(100, animateSources, 0);
		update();
	}
}
