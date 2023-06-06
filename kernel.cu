#include <cuda_runtime.h>
#include <cuda.h>
#include <curand.h>
#include <curand_kernel.h>
#include "device_launch_parameters.h"

#include "kernel.h"

__device__ int area2(int ax, int ay, int bx, int by, int cx, int cy) {
    return (bx - ax) * (cy - ay) - (cx - ax) * (by - ay);
}

__device__ bool left(int ax, int ay, int bx, int by, int cx, int cy) {
    return area2(ax, ay, bx, by, cx, cy) > 0;
}

__device__ bool collinear(int ax, int ay, int bx, int by, int cx, int cy) {
    return area2(ax, ay, bx, by, cx, cy) == 0;
}

__device__ bool xorBool(bool x, bool y) {
    return !x ^ !y;
}

__device__ bool intersectProp(int ax, int ay, int bx, int by, int cx, int cy, int dx, int dy) {
    if (collinear(ax, ay, bx, by, cx, cy) || collinear(ax, ay, bx, by, dx, dy) || collinear(cx, cy, dx, dy, ax, ay) || collinear(cx, cy, dx, dy, bx, by))
        return false;
    return xorBool(left(ax, ay, bx, by, cx, cy), left(ax, ay, bx, by, dx, dy)) && xorBool(left(cx, cy, dx, dy, ax, ay), left(cx, cy, dx, dy, bx, by));
}

__device__ bool between(int ax, int ay, int bx, int by, int cx, int cy) {
    if (!collinear(ax, ay, bx, by, cx, cy))
        return false;
    if (ax != bx)
        return ((ax <= cx) && (cx <= bx)) || ((ax >= cx) && (cx >= bx));
    else
        return ((ay <= cy) && (cy <= by) || ((ay >= cy) && (cy >= by)));
}

__device__ bool seCroisent(int ax, int ay, int bx, int by, int cx, int cy, int dx, int dy, bool& isIllegal) {
    if (intersectProp(ax, ay, bx, by, cx, cy, dx, dy))
        return true;
    else if (between(ax, ay, bx, by, cx, cy) || between(ax, ay, bx, by, dx, dy) || between(cx, cy, dx, dy, ax, ay) || between(cx, cy, dx, dy, bx, by)) {
        isIllegal = true;
        return true;
    }
    return false;
}

__device__ bool surSegment(int ax, int ay, int bx, int by, int cx, int cy) {
    if (!collinear(ax, ay, bx, by, cx, cy))
        return false;
    if (ax != bx)
        return ((ax <= cx) && (cx <= bx)) || ((ax >= cx) && (cx >= bx));
    else
        return ((ay <= cy) && (cy <= by) || ((ay >= cy) && (cy >= by)));
}

__device__ float area2Reel(float ax, float ay, float bx, float by, float cx, float cy) {
    return (bx - ax) * (cy - ay) - (cx - ax) * (by - ay);
}

__device__ bool leftReel(float ax, float ay, float bx, float by, float cx, float cy) {
    return area2Reel(ax, ay, bx, by, cx, cy) > 0;
}

__device__ bool collinearReel(float ax, float ay, float bx, float by, float cx, float cy) {
    return area2Reel(ax, ay, bx, by, cx, cy) == 0;
}

__device__ bool intersectPropReel(float ax, float ay, float bx, float by, float cx, float cy, float dx, float dy) {
    if (collinearReel(ax, ay, bx, by, cx, cy) || collinearReel(ax, ay, bx, by, dx, dy) || collinearReel(cx, cy, dx, dy, ax, ay) || collinearReel(cx, cy, dx, dy, bx, by))
        return false;
    return xorBool(leftReel(ax, ay, bx, by, cx, cy), leftReel(ax, ay, bx, by, dx, dy)) && xorBool(leftReel(cx, cy, dx, dy, ax, ay), leftReel(cx, cy, dx, dy, bx, by));
}

__device__ bool betweenReel(float ax, float ay, float bx, float by, float cx, float cy) {
    if (!collinearReel(ax, ay, bx, by, cx, cy))
        return false;
    if (ax != bx)
        return ((ax <= cx) && (cx <= bx)) || ((ax >= cx) && (cx >= bx));
    else
        return ((ay <= cy) && (cy <= by) || ((ay >= cy) && (cy >= by)));
}

__device__ bool seCroisentReel(float ax, float ay, float bx, float by, float cx, float cy, float dx, float dy, bool& isIllegal) {
    if (intersectPropReel(ax, ay, bx, by, cx, cy, dx, dy))
        return true;
    else if (betweenReel(ax, ay, bx, by, cx, cy) || betweenReel(ax, ay, bx, by, dx, dy) || betweenReel(cx, cy, dx, dy, ax, ay) || betweenReel(cx, cy, dx, dy, bx, by)) {
        isIllegal = true;
        return true;
    }
    return false;
}

__device__ bool surSegmentReel(float ax, float ay, float bx, float by, float cx, float cy) {
    if (!collinearReel(ax, ay, bx, by, cx, cy))
        return false;
    if (ax != bx)
        return ((ax <= cx) && (cx <= bx)) || ((ax >= cx) && (cx >= bx));
    else
        return ((ay <= cy) && (cy <= by) || ((ay >= cy) && (cy >= by)));
}

__global__ void kernelUpdateCrossings(int* nodes, int* edges, long* scores, int* newCoords, int* nodeId, int numThreads,int numNodes, int numEdges, int* commonNodeEdges) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i > numThreads) { return; }
    int id = nodeId[i]*2;
    int newX = newCoords[i*2];
    int newY = newCoords[i*2+1];
    long score = 0;
    int nodeIndex1, nodeIndex2, nodeIndex3, nodeIndex4;
    int node1x, node1y, node2x, node2y, node3x, node3y, node4x, node4y;
    int commonNodeIndex;
    int commonNodex, commonNodey;
    for (int i = 0; i < numEdges-1; i+=2) {
        nodeIndex1 = edges[i];
        node1x = nodes[nodeIndex1*2];
        node1y = nodes[nodeIndex1*2 + 1];
        nodeIndex2 = edges[i + 1];
        node2x = nodes[nodeIndex2*2];
        node2y = nodes[nodeIndex2*2 + 1];
        for (int j = i + 1; j < numEdges; j+=2) {
            nodeIndex3 = edges[j];
            node3x = nodes[nodeIndex3*2];
            node3y = nodes[nodeIndex3*2 + 1];
            nodeIndex4 = edges[j+1];
            node4x = nodes[nodeIndex4*2];
            node4y = nodes[nodeIndex4*2 + 1];
            bool isIllegal = false;
            if (commonNodeEdges[i*numNodes+j] == -1) {
                if (seCroisent(node1x, node1y, node2x, node2y, node3x, node3y, node4x, node4y, isIllegal)) {
                    if (isIllegal) { score += 1000; }
                    else { score++; }
                }
            }
            else {
                commonNodeIndex = commonNodeEdges[j*numNodes+i];
                commonNodex = nodes[commonNodeIndex * 2];
                commonNodey = nodes[commonNodeIndex * 2 + 1];
                if (surSegment(node1x, node1y, node2x, node2y, commonNodex, commonNodey)) { score += 1001; isIllegal = true; }
                if (!isIllegal) {
                    commonNodeIndex = commonNodeEdges[i*numNodes+j];
                    commonNodex = nodes[commonNodeIndex * 2];
                    commonNodey = nodes[commonNodeIndex * 2 + 1];
                    if (surSegment(node3x, node3y, node4x, node4y, commonNodex, commonNodey)) { score += 1001; }
                }
            }
        }
    }
    scores[i] = score;
}

__global__ void kernelUpdateCrossingsReel(float* nodes, int* edges, long* scores, float* newCoords, int* nodeId, int numThreads, int numNodes, int numEdges, int* commonNodeEdges) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i > numThreads) { return; }
    int id = nodeId[i] * 2;
    float newX = newCoords[i * 2];
    float newY = newCoords[i * 2 + 1];
    long score = 0;
    int nodeIndex1, nodeIndex2, nodeIndex3, nodeIndex4;
    float node1x, node1y, node2x, node2y, node3x, node3y, node4x, node4y;
    int commonNodeIndex;
    float commonNodex, commonNodey;
    for (int i = 0; i < numEdges - 1; i += 2) {
        nodeIndex1 = edges[i];
        node1x = nodes[nodeIndex1 * 2];
        node1y = nodes[nodeIndex1 * 2 + 1];
        nodeIndex2 = edges[i + 1];
        node2x = nodes[nodeIndex2 * 2];
        node2y = nodes[nodeIndex2 * 2 + 1];
        for (int j = i + 1; j < numEdges; j += 2) {
            nodeIndex3 = edges[j];
            node3x = nodes[nodeIndex3 * 2];
            node3y = nodes[nodeIndex3 * 2 + 1];
            nodeIndex4 = edges[j + 1];
            node4x = nodes[nodeIndex4 * 2];
            node4y = nodes[nodeIndex4 * 2 + 1];
            bool isIllegal = false;
            if (commonNodeEdges[i*numNodes+j] == -1) {
                if (seCroisentReel(node1x, node1y, node2x, node2y, node3x, node3y, node4x, node4y, isIllegal)) {
                    if (isIllegal) { score += 1000; }
                    else { score++; }
                }
            }
            else {
                commonNodeIndex = commonNodeEdges[j*numNodes+i];
                commonNodex = nodes[commonNodeIndex * 2];
                commonNodey = nodes[commonNodeIndex * 2 + 1];
                if (surSegmentReel(node1x, node1y, node2x, node2y, commonNodex, commonNodey)) { score += 1001; isIllegal = true; }
                if (!isIllegal) {
                    commonNodeIndex = commonNodeEdges[i*numNodes+j];
                    commonNodex = nodes[commonNodeIndex * 2];
                    commonNodey = nodes[commonNodeIndex * 2 + 1];
                    if (surSegmentReel(node3x, node3y, node4x, node4y, commonNodex, commonNodey)) { score += 1001; }
                }
            }
        }
    }
    scores[i] = score;
}

extern "C" void rechercheTabouGPU(const int* nodes, const int* edges, long* scores, const int* newCoords, const int* nodeId, const int numNodes, const int numEdges, const int blockSize, const int gridSize, const int* commonNodeEdges) {
    int* devNodes;
    int* devEdges;
    long* devScores;
    int* devNewCoords;
    int* devNodeId;
    int* devCommonNodeEdges;

    int numThreads = blockSize * gridSize;

    // Memory allocation
    cudaMalloc((void**)&devNodes, sizeof(int) * numNodes * 2);
    cudaMalloc((void**)&devEdges, sizeof(int) * numEdges * 2);
    cudaMalloc((void**)&devScores, sizeof(long) * numThreads);
    cudaMalloc((void**)&devNewCoords, sizeof(int) * numThreads * 2);
    cudaMalloc((void**)&devNodeId, sizeof(int) * numThreads);

    // Copy vectors datas from host to device
    cudaMemcpy(devNodes, nodes, sizeof(int) * numNodes * 2, cudaMemcpyHostToDevice);
    cudaMemcpy(devEdges, edges, sizeof(int) * numEdges * 2, cudaMemcpyHostToDevice);
    cudaMemcpy(devScores, scores, sizeof(long) * numThreads, cudaMemcpyHostToDevice);
    cudaMemcpy(devNewCoords, newCoords, sizeof(int) * numThreads * 2, cudaMemcpyHostToDevice);
    cudaMemcpy(devNodeId, nodeId, sizeof(int) * numThreads, cudaMemcpyHostToDevice);

    // Update crossing score
    kernelUpdateCrossings<<<gridSize, blockSize>>>(devNodes,devEdges,devScores,devNewCoords,devNodeId,numThreads,numNodes,numEdges,devCommonNodeEdges);
    // Update vector device to host
    cudaMemcpy(scores, devScores, sizeof(long) * numThreads, cudaMemcpyDeviceToHost);

    for (int i = 0; i < numThreads; i++) { std::cout << "i: " << i << " " << scores[i] << std::endl; }

    // cleanup
    cudaFree(devNodes);
    cudaFree(devEdges);
    cudaFree(devScores);
    cudaFree(devNewCoords);
    cudaFree(devNodeId);
}

extern "C" void rechercheTabouGPUReel(const float* nodes, const int* edges, long* scores, const float* newCoords, const int* nodeId, const int numNodes, const int numEdges, const int blockSize, const int gridSize, const int* commonNodeEdges) {
    float* devNodes;
    int* devEdges;
    long* devScores;
    float* devNewCoords;
    int* devNodeId;
    int* devCommonNodeEdges;

    int numThreads = blockSize * gridSize;

    // Memory allocation
    cudaMalloc((void**)&devNodes, sizeof(float) * numNodes * 2);
    cudaMalloc((void**)&devEdges, sizeof(int) * numEdges * 2);
    cudaMalloc((void**)&devScores, sizeof(long) * numThreads);
    cudaMalloc((void**)&devNewCoords, sizeof(float) * numThreads * 2);
    cudaMalloc((void**)&devNodeId, sizeof(int) * numThreads);
    cudaMalloc((void**)&devCommonNodeEdges, sizeof(int) * numNodes * numNodes);

    // Copy vectors datas from host to device
    cudaMemcpy(devNodes, nodes, sizeof(float) * numNodes * 2, cudaMemcpyHostToDevice);
    cudaMemcpy(devEdges, edges, sizeof(int) * numEdges * 2, cudaMemcpyHostToDevice);
    cudaMemcpy(devScores, scores, sizeof(long) * numThreads, cudaMemcpyHostToDevice);
    cudaMemcpy(devNewCoords, newCoords, sizeof(float) * numThreads * 2, cudaMemcpyHostToDevice);
    cudaMemcpy(devNodeId, nodeId, sizeof(int) * numThreads, cudaMemcpyHostToDevice);
    cudaMemcpy(devCommonNodeEdges, nodeId, sizeof(int) * numNodes * numNodes, cudaMemcpyHostToDevice);

    // Update crossing score
    kernelUpdateCrossingsReel<<<gridSize, blockSize>>>(devNodes, devEdges, devScores, devNewCoords, devNodeId, numThreads,numNodes, numEdges, devCommonNodeEdges);
    // Update vector device to host
    cudaMemcpy(scores, devScores, sizeof(long) * numThreads, cudaMemcpyDeviceToHost);

    for (int i = 0; i < numThreads; i++) { std::cout << "i: " << i << " " << scores[i] << std::endl; }

    // cleanup
    cudaFree(devNodes);
    cudaFree(devEdges);
    cudaFree(devScores);
    cudaFree(devNewCoords);
    cudaFree(devNodeId);
    cudaFree(devCommonNodeEdges);
}