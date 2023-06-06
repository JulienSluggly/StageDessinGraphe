#include <cuda_runtime.h>
#include <cuda.h>
#include <curand.h>
#include <curand_kernel.h>

#include "kernel.h"

__global__ void rechercheTabouGPU(const float* nodes,const int* edges, long* scores,const float* newCoords,const int nodeId) {

}