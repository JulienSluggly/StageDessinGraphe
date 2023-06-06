#ifndef KERNEL_H
#define KERNEL_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <math.h>
#define _USE_MATH_DEFINES

extern "C" void rechercheTabouGPU(const int* nodes, const int* edges, long* scores, const int* newCoords, const int* nodeId, const int* commonNodeEdges, const int numNodes, const int numEdges, const int blockSize, const int gridSize);
extern "C" void rechercheTabouGPUReel(const float* nodes, const int* edges, long* scores, const float* newCoords, const int* nodeId, const int* commonNodeEdges, const int numNodes, const int numEdges, const int blockSize, const int gridSize);

#endif