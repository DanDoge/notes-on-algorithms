#ifndef _PMM_H_
#define _PMM_H_

float compute(const char* fileName, int numIterations, float* meanTimePerIteration, float* standardDeviationPerIteration, unsigned int numRepetitions);

void usage();

void timeStatistics(const char* fileName, float* kernelExecutionTime, float* meanTimePerIteration, float* standardDeviationPerIteration, unsigned int numIterations, unsigned int numRepetitions, unsigned int width, unsigned int height);



#endif //_PMM_H_