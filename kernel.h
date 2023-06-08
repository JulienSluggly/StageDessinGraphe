#ifndef KERNEL_H
#define KERNEL_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <math.h>
#define _USE_MATH_DEFINES

extern "C" void rechercheTabouGPU(int* nodes, const int* edges, int* scores,const int* newCoords, const int* nodeId, const int* commonNodeEdges, const int numNodes, const int numEdges, const int blockSize, const int gridSize, const int gridWidth, const int gridHeight);
extern "C" void rechercheTabouGPUReel(float* nodes, const int* edges, int* scores,const float* newCoords, const int* nodeId, const int* commonNodeEdges, const int numNodes, const int numEdges, const int blockSize, const int gridSize, const int gridWidth, const int gridHeight, const int placementScore);

#endif