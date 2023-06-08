#ifndef KERNEL_H
#define KERNEL_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <math.h>
#define _USE_MATH_DEFINES

extern "C" void rechercheTabouGPU(int* nodes, const int* edges, const int* slots, const int* slotsCoord, int* scores,const int* newSlotPos, const int* nodeId, const int* commonNodeEdges, const int numNodes, const int numEdges, const int numSlots, const int blockSize, const int gridSize, const int placementScore,const int PENALITE_MAX, const int PENALITE_MAX_SELF);
extern "C" void rechercheTabouGPUReel(float* nodes, const int* edges, int* scores,const float* newCoords, const int* nodeId, const int* commonNodeEdges, const int numNodes, const int numEdges, const int blockSize, const int gridSize, const int gridWidth, const int gridHeight, const int placementScore, const int PENALITE_MAX, const int PENALITE_MAX_SELF);

#endif