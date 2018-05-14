#ifndef _CUDA_WRAPPER_H_
#define _CUDA_WRAPPER_H_

#include <C:\Program Files\NVIDIA Corporation\NVIDIA CUDA SDK\common\inc\GL\glut.h>
#include "Types.h"

#include <mat.h>
#include <matrix.h>

extern "C"
{
	bool PMM_init();

	void PMM_destroy();

	float PMM_calculateGeodesicsDistances(const Float4* hostVertexPositions, const unsigned int width, const unsigned int height, const Int2* sources, const unsigned int numSources, const float* sourcesValues, float* hostDistanceMap, const unsigned int numIterations, const char* modelName, float* kernelExecutionTimePerIteration);

	bool PMM_loadMatFile(const char* fileName, MATFile** pMat, unsigned int& width, unsigned int& height, mxArray** pX, mxArray** pY, mxArray** pZ, unsigned int& numSources, mxArray** px, mxArray** py, mxArray** pV);

	bool PMM_unloadMatFile(MATFile* pMat, mxArray* pX, mxArray* pY, mxArray* pZ, mxArray* distanceMapArray, mxArray* px, mxArray* py, mxArray* pV);

	bool PMM_loadParametricSurfaceFromMatFile(Float4* hostVertexPositions, unsigned int width, unsigned int height, mxArray* pX, mxArray* pY, mxArray* pZ, mxArray* px, mxArray* py, Int2* sources, unsigned int numSources, float* sourcesValues, mxArray* pV);

	bool PMM_addDistanceMapToMatFile(float* hostDistanceMap, MATFile* pMat, mxArray** distanceMapArray, unsigned int width, unsigned int height);

	bool PMM_saveDistanceMapToMatFile(float* hostDistanceMap, char* fileName, unsigned int width, unsigned int height);

	int PMM_cudaMallocHost(void** hostPtr, size_t size);

	int PMM_cudaFreeHost(void* hostVertexPositions);

	int PMM_cudaGLRegisterBufferObject(GLuint pbo);

	int PMM_cudaGLUnregisterBufferObject(GLuint pbo);

	int PMM_cudaGLMapBufferObject(void** d_dst, GLuint gl_PBO);

	int PMM_cudaGLUnmapBufferObject(GLuint gl_PBO);

	float PMM_infinity();

	bool PMM_buildGeometry(const char* fileName, unsigned int& width, unsigned int& height, unsigned int& numSources, Int2* sources, float* sourcesValues);

	void PMM_computeAndDisplayDistanceMap(unsigned int width, unsigned int height, unsigned int* dDisplay, unsigned int numSources, Int2* sources, float* sourcesValues, unsigned int numIterations, bool drawSinContour, float maxDistance);

	void PMM_destroyGeometry();

	void PMM_uploadDistanceMapToCPU(float* hostDistanceMap, unsigned int width, unsigned int height, bool transposed);

};

#endif //_CUDA_WRAPPER_H_