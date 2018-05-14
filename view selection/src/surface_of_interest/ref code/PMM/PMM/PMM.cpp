#include <stdlib.h>
#include <stdio.h>
#include <cutil.h>
#include <memory.h>
#include <math.h>

#include "PMM.h"
#include "CudaWrapper.h"


int main(int argc, const char** argv)
{
	if(argc < 2) //at least a file name is expected
	{
		usage();
		exit(1);
	}

	CUTBoolean res = cutCheckCmdLineFlag(argc, argv, "niters");

	int numIterations = 1;
	if(cutGetCmdLineArgumenti(argc, argv, "niters", &numIterations))
	{
		if(numIterations < 1)
		{
			numIterations = 1;
		}
	}
	else
	{
		numIterations = 1;
	}
	char* fileName = NULL;
	if(!cutGetCmdLineArgumentstr(argc, argv, "filename", &fileName))
	{
		usage();
		exit(1);
	}

	bool result = PMM_init();

	unsigned int numRepetitions = 1;
	float* meanTimePerIteration = (float*)malloc(numIterations*sizeof(float));
	float* standardDeviationPerIteration = (float*)malloc(numIterations*sizeof(float));

	compute(fileName, numIterations, meanTimePerIteration, standardDeviationPerIteration, numRepetitions);
	free(meanTimePerIteration);
	free(standardDeviationPerIteration);
	PMM_destroy();

	return 0;
}

float compute(const char* fileName, int numIterations, float* meanTimePerIteration, float* standardDeviationPerIteration, unsigned int numRepetitions)
{
	MATFile* pMat = NULL;
	unsigned int width = 0;
	unsigned int height = 0;
	unsigned int numSources = 0;
	mxArray* pX = NULL;
	mxArray* pY = NULL;
	mxArray* pZ = NULL;

	mxArray* px = NULL;
	mxArray* py = NULL;

	mxArray* pV = NULL;

	if(!PMM_loadMatFile(fileName, &pMat, width, height, &pX, &pY, &pZ, numSources, &px, &py, &pV))
	{
		return 0.0f;
	}

	Float4* hostVertexPositions = NULL;
	PMM_cudaMallocHost((void**)&hostVertexPositions ,sizeof(Float4)*width*height);

	Int2* sources = (Int2*)malloc(numSources*sizeof(Int2));
 	float* sourcesValues = (float*)malloc(numSources*sizeof(float));

	PMM_loadParametricSurfaceFromMatFile((Float4*)hostVertexPositions, width, height, pX, pY, pZ, px, py, (Int2*)sources, numSources, sourcesValues, pV);

	float* hostDistanceMap = NULL;
	PMM_cudaMallocHost((void**)(&hostDistanceMap), sizeof(float)*width*height);

	float* kernelExecutionTime = (float*)malloc(numIterations*numRepetitions*sizeof(float));
	memset(kernelExecutionTime, 0, numIterations*numRepetitions*sizeof(float));
	float rate = 0.0f;
	for(unsigned int i = 0; i < numRepetitions; i++)
	{
		//Rate of Million Distances per second
		rate = PMM_calculateGeodesicsDistances(hostVertexPositions, width, height, sources, numSources, sourcesValues, hostDistanceMap, numIterations, fileName, kernelExecutionTime + i*numIterations);
	}
	timeStatistics(fileName, kernelExecutionTime, meanTimePerIteration, standardDeviationPerIteration, numIterations, numRepetitions, width, height);
	free(kernelExecutionTime);
	free(sources);
	free(sourcesValues);

	PMM_cudaFreeHost(hostVertexPositions);
	hostVertexPositions = NULL;

	mxArray* distanceMapArray = NULL;
	bool result = PMM_addDistanceMapToMatFile(hostDistanceMap, pMat, &distanceMapArray, width, height);
	result = PMM_unloadMatFile(pMat, pX, pY, pZ, distanceMapArray, px, py, pV);

	PMM_cudaFreeHost(hostDistanceMap);
	hostDistanceMap = NULL;

	return rate;
}

void timeStatistics(const char* fileName, float* kernelExecutionTime, float* meanTimePerIteration, float* standardDeviationPerIteration, unsigned int numIterations, unsigned int numRepetitions, unsigned int width, unsigned int height)
{
	memset(meanTimePerIteration, 0, numIterations*sizeof(float));
	memset(standardDeviationPerIteration, 0, numIterations*sizeof(float));
	for(unsigned int j = 0; j < numIterations; j++)
	{
		for(unsigned int i = 0; i < numRepetitions; i++)
		{
			meanTimePerIteration[j] += kernelExecutionTime[i*numIterations + j];
		}
		meanTimePerIteration[j] /= numRepetitions;
	}
	for(unsigned int j = 0; j < numIterations; j++)
	{
		for(unsigned int i = 0; i < numRepetitions; i++)
		{
			float deviation = (meanTimePerIteration[j] - kernelExecutionTime[i*numIterations + j]);
			standardDeviationPerIteration[j] += deviation*deviation;
		}
		standardDeviationPerIteration[j] = sqrt(standardDeviationPerIteration[j] / numRepetitions);
	}
	printf("\nAverage Time Per Iteration over %d repetitions for %s with Grid size of %dX%d\n", numRepetitions, fileName, width, height);
	for(unsigned int j = 0; j < numIterations; j++)
	{
		printf("%0.6f ", meanTimePerIteration[j]);
	}
	printf("\nStandard Deviation Per Iteration:\n");
	for(unsigned int j = 0; j < numIterations; j++)
	{
		printf("%0.6f ", standardDeviationPerIteration[j]);
	}
	printf("\n");
}

void usage()
{
	printf("-----------------------------------------------------------------------------------------------------------------------\n");
	printf("Wrong usage. See correct usage below:\n");
	printf("PMM.exe [--niters=N] --filename=\"path\\filename\"\n");
	printf("     --niters - number of iterations to perform (default is 1 iteration)\n");
	printf("     --filename - path name of .mat (Matlab) file that contains the parametric surface and a list of source points\n");
	printf("     usage example: PMM.exe --niters=3 --filename=\"..\\Matlab\\surface.mat\"\n");
	printf("-----------------------------------------------------------------------------------------------------------------------\n");
}
