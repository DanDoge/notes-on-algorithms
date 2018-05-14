#ifndef _PMMUI_H_
#define _PMMUI_H

#define MAX_SOURCES 10000

class Vector3;

bool initGL();
void createColorBuffer(int width, int height);
void destroyBuffers();
void display();
void rotateTimer(int value);
void scaleTimer(int value);
void replaceOneSourceTimer(int value);
void animateSources(int value);
bool calculateNormal(int i, int j, float* vertexPositions, unsigned int numVertices, int width, int height, Vector3& normal);
void reshape(int width, int height);
void motion(int x, int y);
void mouse(int button, int state, int x, int y);
void keyboard(unsigned char key, int x, int y);
void reshape(int width, int height);
bool translateMouseCoordinates(int& x, int& y);
void addAllSourcesToDistanceMap();
void addOneSourceToDistanceMap(int x, int y);
void addToSourceList(int x, int y);
bool findClosestSource(int x, int y, int* closestSourceIndex);
void replaceOneSourceInSourceList(int x, int y, int closestSourceIndex);
bool openFile(char* fileName);
bool saveFile(char* fileName);
void clearSourcesList();
void update();
void initArrays(char* fileName);
int renderPickColorsToBackBuffer(int x, int y);

class GlobalVariables
{
public:
	GlobalVariables();
	void init();

public:
	int windowWidth; //dimensions of window
	int windowHeight;
	unsigned int width; //dimensions of distance map
	unsigned int height;
	int parametricWidth; //dimensions of parametric space part of window
	int parametricHeight;
	double parametricWidthRatio;
	GLuint pbo;
	GLuint verticesPositionsVbo;
	GLuint normalsVbo;
	GLuint colorsVbo;
	GLuint textureVbo;
	GLuint objectIbo;
	bool rightButtonDown;
	bool leftButtonDown;
	bool middleButtonDown;
	bool rotating;
	bool translating;
	bool scaling;
	bool draggingSource;
	bool drawSinContour;
	int numIterations;
	int numValidVertices;
	int closestSourceIndex;
	Int2 sources[MAX_SOURCES];
	float sourcesValues[MAX_SOURCES];
	unsigned int numSources;
	unsigned int numIndices;
	bool nearest;
	double minX, maxX, minY, maxY, minZ, maxZ;
	double centerX, centerY, centerZ;
	double maxBB;
	double rotateX;
	double rotateY;
	int mouseOldX;
	int mouseOldY;
	double cameraPosition[3];
	bool pointsRenderMode;
	bool textureMapOn;
	bool uvSpace;
	bool autoRotate;
	bool autoScale;
	int timeStep;
	int motionX;
	int motionY;
	bool movingSources;
};


#endif //_PMMUI_H_