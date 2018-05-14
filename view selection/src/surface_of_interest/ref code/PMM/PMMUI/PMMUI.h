#ifndef _PMMUI_H_
#define _PMMUI_H

#define MAX_SOURCES 10000


bool initGL();
void createFrameBuffer(int width, int height);
void destroyFrameBuffer();
void display();
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



class GlobalVars
{
public:
	GlobalVars();

public:
	GLuint pbo;
	unsigned int width;
	unsigned int height;
	bool rightButtonDown;
	bool drawSinContour;
	int numIterations;
	int closestSourceIndex;
	Int2 sources[MAX_SOURCES];
	float sourcesValues[MAX_SOURCES];
	unsigned int numSources;
	bool nearest;
};


#endif //_PMMUI_H_