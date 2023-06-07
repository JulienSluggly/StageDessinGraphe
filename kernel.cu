#include <cuda_runtime.h>
#include <cuda.h>
#include <curand.h>
#include <curand_kernel.h>
#include "device_launch_parameters.h"
#include <ctime>
#include <chrono>
#include <iomanip>
#include <fstream>

#include "personnel.hpp"
#include "kernel.h"

__forceinline__ __device__ float diff_of_products(int a, int b, int c, int d) {
    int w = d * c;
    int e = fmaf(-d, c, w);
    int f = fmaf(a, b, -w);
    return f + e;
}

__device__ int area2CUDA(int ax, int ay, int bx, int by, int cx, int cy) {
    return (bx - ax) * (cy - ay) - (cx - ax) * (by - ay);
}

__device__ bool leftCUDA(int ax, int ay, int bx, int by, int cx, int cy) {
    return area2CUDA(ax, ay, bx, by, cx, cy) > 0;
}

__device__ bool collinearCUDA(int ax, int ay, int bx, int by, int cx, int cy) {
    return area2CUDA(ax, ay, bx, by, cx, cy) == 0;
}

__device__ bool xorBoolCUDA(bool x, bool y) {
    return !x ^ !y;
}

__device__ bool intersectPropCUDA(int ax, int ay, int bx, int by, int cx, int cy, int dx, int dy) {
    if (collinearCUDA(ax, ay, bx, by, cx, cy) || collinearCUDA(ax, ay, bx, by, dx, dy) || collinearCUDA(cx, cy, dx, dy, ax, ay) || collinearCUDA(cx, cy, dx, dy, bx, by))
        return false;
    return xorBoolCUDA(leftCUDA(ax, ay, bx, by, cx, cy), leftCUDA(ax, ay, bx, by, dx, dy)) && xorBoolCUDA(leftCUDA(cx, cy, dx, dy, ax, ay), leftCUDA(cx, cy, dx, dy, bx, by));
}

__device__ bool betweenCUDA(int ax, int ay, int bx, int by, int cx, int cy) {
    if (!collinearCUDA(ax, ay, bx, by, cx, cy))
        return false;
    if (ax != bx)
        return ((ax <= cx) && (cx <= bx)) || ((ax >= cx) && (cx >= bx));
    else
        return ((ay <= cy) && (cy <= by) || ((ay >= cy) && (cy >= by)));
}

__device__ bool seCroisentCUDA(int ax, int ay, int bx, int by, int cx, int cy, int dx, int dy, bool& isIllegal) {
    if (intersectPropCUDA(ax, ay, bx, by, cx, cy, dx, dy))
        return true;
    else if (betweenCUDA(ax, ay, bx, by, cx, cy) || betweenCUDA(ax, ay, bx, by, dx, dy) || betweenCUDA(cx, cy, dx, dy, ax, ay) || betweenCUDA(cx, cy, dx, dy, bx, by)) {
        isIllegal = true;
        return true;
    }
    return false;
}

__device__ bool surSegmentCUDA(int ax, int ay, int bx, int by, int cx, int cy) {
    if (!collinearCUDA(ax, ay, bx, by, cx, cy))
        return false;
    if (ax != bx)
        return ((ax <= cx) && (cx <= bx)) || ((ax >= cx) && (cx >= bx));
    else
        return ((ay <= cy) && (cy <= by) || ((ay >= cy) && (cy >= by)));
}

__forceinline__ __device__ float diff_of_productsReel(double a, double b, double c, double d) {
    float w = d * c;
    float e = fmaf(-d, c, w);
    float f = fmaf(a, b, -w);
    return f + e;
}

__device__ double area2ReelCUDA(double ax, double ay, double bx, double by, double cx, double cy) {
    //return diff_of_productsReel((bx - ax), (cy - ay), (cx - ax), (by - ay));
    return (bx - ax) * (cy - ay) - (cx - ax) * (by - ay);
}

__device__ bool leftReelCUDA(double ax, double ay, double bx, double by, double cx, double cy) {
    return area2ReelCUDA(ax, ay, bx, by, cx, cy) > 0;
}

__device__ bool collinearReelCUDA(double ax, double ay, double bx, double by, double cx, double cy) {
    return area2ReelCUDA(ax, ay, bx, by, cx, cy) == 0;
}

__device__ bool intersectPropReelCUDA(double ax, double ay, double bx, double by, double cx, double cy, double dx, double dy) {
    if (collinearReelCUDA(ax, ay, bx, by, cx, cy) || collinearReelCUDA(ax, ay, bx, by, dx, dy) || collinearReelCUDA(cx, cy, dx, dy, ax, ay) || collinearReelCUDA(cx, cy, dx, dy, bx, by))
        return false;
    return xorBoolCUDA(leftReelCUDA(ax, ay, bx, by, cx, cy), leftReelCUDA(ax, ay, bx, by, dx, dy)) && xorBoolCUDA(leftReelCUDA(cx, cy, dx, dy, ax, ay), leftReelCUDA(cx, cy, dx, dy, bx, by));
}

__device__ bool betweenReelCUDA(double ax, double ay, double bx, double by, double cx, double cy) {
    if (!collinearReelCUDA(ax, ay, bx, by, cx, cy))
        return false;
    if (ax != bx)
        return ((ax <= cx) && (cx <= bx)) || ((ax >= cx) && (cx >= bx));
    else
        return ((ay <= cy) && (cy <= by) || ((ay >= cy) && (cy >= by)));
}

__device__ bool seCroisentReelCUDA(double ax, double ay, double bx, double by, double cx, double cy, double dx, double dy, bool& isIllegal) {
    if (intersectPropReelCUDA(ax, ay, bx, by, cx, cy, dx, dy))
        return true;
    else if (betweenReelCUDA(ax, ay, bx, by, cx, cy) || betweenReelCUDA(ax, ay, bx, by, dx, dy) || betweenReelCUDA(cx, cy, dx, dy, ax, ay) || betweenReelCUDA(cx, cy, dx, dy, bx, by)) {
        isIllegal = true;
        return true;
    }
    return false;
}

__device__ bool surSegmentReelCUDA(double ax, double ay, double bx, double by, double cx, double cy) {
    if (!collinearReelCUDA(ax, ay, bx, by, cx, cy))
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
                if (seCroisentCUDA(node1x, node1y, node2x, node2y, node3x, node3y, node4x, node4y, isIllegal)) {
                    if (isIllegal) { score += 1000; }
                    else { score++; }
                }
            }
            else {
                commonNodeIndex = commonNodeEdges[j*numNodes+i];
                commonNodex = nodes[commonNodeIndex * 2];
                commonNodey = nodes[commonNodeIndex * 2 + 1];
                if (surSegmentCUDA(node1x, node1y, node2x, node2y, commonNodex, commonNodey)) { score += 1001; isIllegal = true; }
                if (!isIllegal) {
                    commonNodeIndex = commonNodeEdges[i*numNodes+j];
                    commonNodex = nodes[commonNodeIndex * 2];
                    commonNodey = nodes[commonNodeIndex * 2 + 1];
                    if (surSegmentCUDA(node3x, node3y, node4x, node4y, commonNodex, commonNodey)) { score += 1001; }
                }
            }
        }
    }
    scores[i] = score;
}

__global__ void kernelUpdateCrossingsReel(double* nodes, int* edges, long* scores, double* newCoords, int* nodeId, int numThreads, int numNodes, int numEdges, int* commonNodeEdges) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int id = nodeId[i] * 2;
    double newX, newY;
    if (i == 0) {
        newX = nodes[id];
        newY = nodes[id + 1];
    }
    else {
        newX = newCoords[i * 2];
        newY = newCoords[i * 2 + 1];
    }
    long score = 0;
    int nodeIndex1, nodeIndex2, nodeIndex3, nodeIndex4;
    int areteIndex1, areteIndex2;
    double node1x, node1y, node2x, node2y, node3x, node3y, node4x, node4y;
    int commonNodeIndex;
    double commonNodex, commonNodey;
    int limiteIndex1 = (numEdges * 2) - 2;
    int limiteIndex2 = (numEdges * 2);
    for (int i = 0; i < limiteIndex1; i += 2) {
        areteIndex1 = i / 2;
        nodeIndex1 = edges[i];
        node1x = nodes[nodeIndex1 * 2];
        node1y = nodes[nodeIndex1 * 2 + 1];
        nodeIndex2 = edges[i + 1];
        node2x = nodes[nodeIndex2 * 2];
        node2y = nodes[nodeIndex2 * 2 + 1];
        if (nodeIndex1 == id) {
            node1x = newX;
            node1y = newY;
        }
        else if (nodeIndex2 == id) {
            node2x = newX;
            node2y = newY;
        }
        for (int j = i + 2; j < limiteIndex2; j += 2) {
            areteIndex2 = j / 2;
            nodeIndex3 = edges[j];
            node3x = nodes[nodeIndex3 * 2];
            node3y = nodes[nodeIndex3 * 2 + 1];
            nodeIndex4 = edges[j + 1];
            node4x = nodes[nodeIndex4 * 2];
            node4y = nodes[nodeIndex4 * 2 + 1];
            bool isIllegal = false;
            if (nodeIndex3 == id) {
                node3x = newX;
                node3y = newY;
            }
            else if (nodeIndex4 == id) {
                node4x = newX;
                node4y = newY;
            }
            if (commonNodeEdges[areteIndex1 * numEdges + areteIndex2] == -1) {
                if (seCroisentReelCUDA(node1x, node1y, node2x, node2y, node3x, node3y, node4x, node4y, isIllegal)) {
                    if (isIllegal) { score += 1000;  }
                    else { score++; }
                }
            }
            else {
                commonNodeIndex = commonNodeEdges[areteIndex2 * numEdges + areteIndex1];
                if (commonNodeIndex == id) {
                    commonNodex = newX;
                    commonNodey = newY;
                }
                else {
                    commonNodex = nodes[commonNodeIndex * 2];
                    commonNodey = nodes[commonNodeIndex * 2 + 1];
                }
                if (surSegmentReelCUDA(node1x, node1y, node2x, node2y, commonNodex, commonNodey)) { score += 1001; isIllegal = true; }
                if (!isIllegal) {
                    commonNodeIndex = commonNodeEdges[areteIndex1 * numEdges + areteIndex2];
                    commonNodex = nodes[commonNodeIndex * 2];
                    commonNodey = nodes[commonNodeIndex * 2 + 1];
                    if (surSegmentReelCUDA(node3x, node3y, node4x, node4y, commonNodex, commonNodey)) { score += 1001; }
                }
            }
        }
    }
    scores[i] = score;
}

extern "C" void rechercheTabouGPU(const int* nodes, const int* edges, long* scores, const int* newCoords, const int* nodeId, const int* commonNodeEdges, const int numNodes, const int numEdges, const int blockSize, const int gridSize) {
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

extern "C" void rechercheTabouGPUReel(const double* nodes, const int* edges, long* scores, const double* newCoords, const int* nodeId, const int* commonNodeEdges, const int numNodes, const int numEdges, const int blockSize, const int gridSize) {
    double* devNodes;
    int* devEdges;
    long* devScores;
    double* devNewCoords;
    int* devNodeId;
    int* devCommonNodeEdges;

    int numThreads = blockSize * gridSize;

    // Memory allocation
    cudaMalloc((void**)&devNodes, sizeof(double) * numNodes * 2);
    cudaMalloc((void**)&devEdges, sizeof(int) * numEdges * 2);
    cudaMalloc((void**)&devScores, sizeof(long) * numThreads);
    cudaMalloc((void**)&devNewCoords, sizeof(double) * numThreads * 2);
    cudaMalloc((void**)&devNodeId, sizeof(int) * numThreads);
    cudaMalloc((void**)&devCommonNodeEdges, sizeof(int) * numEdges * numEdges);

    // Copy vectors datas from host to device
    cudaMemcpy(devNodes, nodes, sizeof(double) * numNodes * 2, cudaMemcpyHostToDevice);
    cudaMemcpy(devEdges, edges, sizeof(int) * numEdges * 2, cudaMemcpyHostToDevice);
    cudaMemcpy(devScores, scores, sizeof(long) * numThreads, cudaMemcpyHostToDevice);
    cudaMemcpy(devNewCoords, newCoords, sizeof(double) * numThreads * 2, cudaMemcpyHostToDevice);
    cudaMemcpy(devNodeId, nodeId, sizeof(int) * numThreads, cudaMemcpyHostToDevice);
    cudaMemcpy(devCommonNodeEdges, commonNodeEdges, sizeof(int) * numEdges * numEdges, cudaMemcpyHostToDevice);

    for (int iter = 0; iter < 1; iter++) {
        // Update crossing score
        auto start = std::chrono::system_clock::now();
        kernelUpdateCrossingsReel<<<gridSize, blockSize>>>(devNodes, devEdges, devScores, devNewCoords, devNodeId, numThreads, numNodes, numEdges, devCommonNodeEdges);
        // Update vector device to host
        cudaMemcpy(scores, devScores, sizeof(long) * numThreads, cudaMemcpyDeviceToHost);
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> secondsTotal = end - start;
        std::string nomFichier = chemin + "/resultats/cuda.csv";
        std::ofstream resultats(nomFichier, std::ios_base::app);
        resultats << std::fixed << std::setprecision(8) << gridSize << "," << blockSize << "," << secondsTotal.count() << std::endl;
        resultats.close();
    }

    for (int i = 0; i < numThreads; i++) { std::cout << "i: " << i << " " << scores[i] << std::endl; }
    //std::cout << "score: " << scores[0] << std::endl;
    // cleanup
    cudaFree(devNodes);
    cudaFree(devEdges);
    cudaFree(devScores);
    cudaFree(devNewCoords);
    cudaFree(devNodeId);
    cudaFree(devCommonNodeEdges);
}