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
#include "utilitaire.hpp"
#include "kernel.h"

__device__ int area2CUDA(const int ax, const int ay, const int bx, const int by, const int cx, const int cy) {
    return (bx - ax) * (cy - ay) - (cx - ax) * (by - ay);
}

__device__ bool leftCUDA(const int ax, const int ay, const int bx, const int by, const int cx, const int cy) {
    return area2CUDA(ax, ay, bx, by, cx, cy) > 0;
}

__device__ bool collinearCUDA(const int ax, const int ay, const int bx, const int by, const int cx, const int cy) {
    return area2CUDA(ax, ay, bx, by, cx, cy) == 0;
}

__device__ bool xorBoolCUDA(const bool x, const bool y) {
    return !x ^ !y;
}

__device__ bool intersectPropCUDA(const int ax, const int ay, const int bx, const int by, const int cx, const int cy, const int dx, const int dy) {
    if (collinearCUDA(ax, ay, bx, by, cx, cy) || collinearCUDA(ax, ay, bx, by, dx, dy) || collinearCUDA(cx, cy, dx, dy, ax, ay) || collinearCUDA(cx, cy, dx, dy, bx, by))
        return false;
    return xorBoolCUDA(leftCUDA(ax, ay, bx, by, cx, cy), leftCUDA(ax, ay, bx, by, dx, dy)) && xorBoolCUDA(leftCUDA(cx, cy, dx, dy, ax, ay), leftCUDA(cx, cy, dx, dy, bx, by));
}

__device__ bool betweenCUDA(const int ax, const int ay, const int bx, const int by, const int cx, const int cy) {
    if (!collinearCUDA(ax, ay, bx, by, cx, cy))
        return false;
    if (ax != bx)
        return ((ax <= cx) && (cx <= bx)) || ((ax >= cx) && (cx >= bx));
    else
        return ((ay <= cy) && (cy <= by) || ((ay >= cy) && (cy >= by)));
}

__device__ bool seCroisentCUDA(const int ax, const int ay, const int bx, const int by, const int cx, const int cy, const int dx, const int dy, bool& isIllegal) {
    if (intersectPropCUDA(ax, ay, bx, by, cx, cy, dx, dy))
        return true;
    else if (betweenCUDA(ax, ay, bx, by, cx, cy) || betweenCUDA(ax, ay, bx, by, dx, dy) || betweenCUDA(cx, cy, dx, dy, ax, ay) || betweenCUDA(cx, cy, dx, dy, bx, by)) {
        isIllegal = true;
        return true;
    }
    return false;
}

__device__ bool surSegmentCUDA(const int ax, const int ay, const int bx, const int by, const int cx, const int cy) {
    if (!collinearCUDA(ax, ay, bx, by, cx, cy))
        return false;
    if (ax != bx)
        return ((ax <= cx) && (cx <= bx)) || ((ax >= cx) && (cx >= bx));
    else
        return ((ay <= cy) && (cy <= by) || ((ay >= cy) && (cy >= by)));
}

__device__ float area2ReelCUDA(const float ax, const float ay, const float bx, const float by, const float cx, const float cy) {
    return (bx - ax) * (cy - ay) - (cx - ax) * (by - ay);
}

__device__ bool leftReelCUDA(const float ax, const float ay, const float bx, const float by, const float cx, const float cy) {
    return area2ReelCUDA(ax, ay, bx, by, cx, cy) > 0;
}

__device__ bool collinearReelCUDA(const float ax, const float ay, const float bx, const float by, const float cx, const float cy) {
    return area2ReelCUDA(ax, ay, bx, by, cx, cy) == 0;
}

__device__ bool intersectPropReelCUDA(const float ax, const float ay, const float bx, const float by, const float cx, const float cy, const float dx, const float dy) {
    if (collinearReelCUDA(ax, ay, bx, by, cx, cy) || collinearReelCUDA(ax, ay, bx, by, dx, dy) || collinearReelCUDA(cx, cy, dx, dy, ax, ay) || collinearReelCUDA(cx, cy, dx, dy, bx, by))
        return false;
    return xorBoolCUDA(leftReelCUDA(ax, ay, bx, by, cx, cy), leftReelCUDA(ax, ay, bx, by, dx, dy)) && xorBoolCUDA(leftReelCUDA(cx, cy, dx, dy, ax, ay), leftReelCUDA(cx, cy, dx, dy, bx, by));
}

__device__ bool betweenReelCUDA(const float ax, const float ay, const float bx, const float by, const float cx, const float cy) {
    if (!collinearReelCUDA(ax, ay, bx, by, cx, cy))
        return false;
    if (ax != bx)
        return ((ax <= cx) && (cx <= bx)) || ((ax >= cx) && (cx >= bx));
    else
        return ((ay <= cy) && (cy <= by) || ((ay >= cy) && (cy >= by)));
}

__device__ bool seCroisentReelCUDA(const float ax, const float ay, const float bx, const float by, const float cx, const float cy, const float dx, const float dy, bool& isIllegal) {
    if (intersectPropReelCUDA(ax, ay, bx, by, cx, cy, dx, dy))
        return true;
    else if (betweenReelCUDA(ax, ay, bx, by, cx, cy) || betweenReelCUDA(ax, ay, bx, by, dx, dy) || betweenReelCUDA(cx, cy, dx, dy, ax, ay) || betweenReelCUDA(cx, cy, dx, dy, bx, by)) {
        isIllegal = true;
        return true;
    }
    return false;
}

__device__ bool surSegmentReelCUDA(const float ax, const float ay, const float bx, const float by, const float cx, const float cy) {
    if (!collinearReelCUDA(ax, ay, bx, by, cx, cy))
        return false;
    if (ax != bx)
        return ((ax <= cx) && (cx <= bx)) || ((ax >= cx) && (cx >= bx));
    else
        return ((ay <= cy) && (cy <= by) || ((ay >= cy) && (cy >= by)));
}

__global__ void kernelUpdateCrossings(int* nodes, const int* edges, int* slots, const int* slotsCoord, int* scores, int* newSlotsPos, const int* nodeId, const int numThreads, const int numNodes, const int numEdges, const int numSlots, const int* commonNodeEdges, const int PENALITE_MAX, const int PENALITE_MAX_SELF) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int firstNodeId = nodeId[i];
    int secondNodeId = -1;
    int newX1, newY1, newX2, newY2;
    curandState state;
    curand_init(clock(), i, 0, &state);
    if (i != 0) {
        int randomSlot = ceil(curand_uniform(&state) * (numSlots-1));
        newSlotsPos[i] = randomSlot;
        if (slots[randomSlot] != -1) {
            secondNodeId = slots[randomSlot];
            int oldSlotId = nodes[firstNodeId];
            newX2 = slotsCoord[oldSlotId * 2];
            newY2 = slotsCoord[oldSlotId * 2 + 1];
        }
        newX1 = slotsCoord[randomSlot * 2];
        newY1 = slotsCoord[randomSlot * 2 + 1];
    }
    int score = 0;
    int nodeIndex1, nodeIndex2, nodeIndex3, nodeIndex4;
    int areteIndex1, areteIndex2;
    int node1x, node1y, node2x, node2y, node3x, node3y, node4x, node4y;
    int commonNodeIndex;
    int commonNodex, commonNodey;
    int limiteIndex1 = (numEdges * 2) - 2;
    int limiteIndex2 = (numEdges * 2);
    for (int i = 0; i < limiteIndex1; i += 2) {
        areteIndex1 = i / 2;
        nodeIndex1 = edges[i];
        node1x = slotsCoord[nodes[nodeIndex1] * 2];
        node1y = slotsCoord[nodes[nodeIndex1] * 2 + 1];
        nodeIndex2 = edges[i + 1];
        node2x = slotsCoord[nodes[nodeIndex2] * 2];
        node2y = slotsCoord[nodes[nodeIndex2] * 2 + 1];
        if (nodeIndex1 == firstNodeId) {
            node1x = newX1;
            node1y = newY1;
        }
        else if (nodeIndex1 == secondNodeId) {
            node1x = newX2;
            node1y = newY2;
        }
        if (nodeIndex2 == firstNodeId) {
            node2x = newX1;
            node2y = newY1;
        }
        else if (nodeIndex2 == secondNodeId) {
            node2x = newX2;
            node2y = newY2;
        }
        for (int j = i + 2; j < limiteIndex2; j += 2) {
            areteIndex2 = j / 2;
            nodeIndex3 = edges[j];
            node3x = slotsCoord[nodes[nodeIndex3] * 2];
            node3y = slotsCoord[nodes[nodeIndex3] * 2 + 1];
            nodeIndex4 = edges[j + 1];
            node4x = slotsCoord[nodes[nodeIndex4] * 2];
            node4y = slotsCoord[nodes[nodeIndex4] * 2 + 1];
            bool isIllegal = false;
            if (nodeIndex3 == firstNodeId) {
                node3x = newX1;
                node3y = newY1;
            }
            else if (nodeIndex3 == secondNodeId) {
                node3x = newX2;
                node3y = newY2;
            }
            if (nodeIndex4 == firstNodeId) {
                node4x = newX1;
                node4y = newY1;
            }
            else if (nodeIndex4 == secondNodeId) {
                node4x = newX2;
                node4y = newY2;
            }
            if (commonNodeEdges[areteIndex1 * numEdges + areteIndex2] == -1) {
                if (seCroisentCUDA(node1x, node1y, node2x, node2y, node3x, node3y, node4x, node4y, isIllegal)) {
                    if (isIllegal) { score += PENALITE_MAX; }
                    else { score++; }
                }
            }
            else {
                commonNodeIndex = commonNodeEdges[areteIndex2 * numEdges + areteIndex1];
                if (commonNodeIndex == firstNodeId) {
                    commonNodex = newX1;
                    commonNodey = newY1;
                }
                else if (commonNodeIndex == secondNodeId) {
                    commonNodex = newX2;
                    commonNodey = newY2;
                }
                else {
                    commonNodex = nodes[commonNodeIndex * 2];
                    commonNodey = nodes[commonNodeIndex * 2 + 1];
                }
                if (surSegmentCUDA(node1x, node1y, node2x, node2y, commonNodex, commonNodey)) { score += PENALITE_MAX_SELF; isIllegal = true; }
                if (!isIllegal) {
                    commonNodeIndex = commonNodeEdges[areteIndex1 * numEdges + areteIndex2];
                    if (commonNodeIndex == firstNodeId) {
                        commonNodex = newX1;
                        commonNodey = newY1;
                    }
                    else if (commonNodeIndex == secondNodeId) {
                        commonNodex = newX2;
                        commonNodey = newY2;
                    }
                    else {
                        commonNodex = nodes[commonNodeIndex * 2];
                        commonNodey = nodes[commonNodeIndex * 2 + 1];
                    }
                    if (surSegmentCUDA(node3x, node3y, node4x, node4y, commonNodex, commonNodey)) { score += PENALITE_MAX_SELF; }
                }
            }
        }
    }
    scores[i] = score;
}

__global__ void kernelUpdateCrossingsReel(float* nodes, const int* edges, int* scores, float* newCoords, const int* nodeId, const int numThreads, const int numNodes, const int numEdges, const int* commonNodeEdges, const int gridWidth, const int gridHeight, const int PENALITE_MAX, const int PENALITE_MAX_SELF) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int id = nodeId[i];
    float newX, newY;
    curandState state;
    curand_init(clock(), i, 0, &state);
    if (i == 0) {
        newX = nodes[id *2];
        newY = nodes[id*2 + 1];
    }
    else {
        newX = curand_uniform(&state) * gridWidth;
        newY = curand_uniform(&state) * gridHeight;
        newCoords[i * 2] = newX;
        newCoords[i * 2 + 1] = newY;
    }
    int score = 0;
    int nodeIndex1, nodeIndex2, nodeIndex3, nodeIndex4;
    int areteIndex1, areteIndex2;
    float node1x, node1y, node2x, node2y, node3x, node3y, node4x, node4y;
    int commonNodeIndex;
    float commonNodex, commonNodey;
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
                    if (isIllegal) { score += PENALITE_MAX;  }
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
                if (surSegmentReelCUDA(node1x, node1y, node2x, node2y, commonNodex, commonNodey)) { score += PENALITE_MAX_SELF; isIllegal = true; }
                if (!isIllegal) {
                    commonNodeIndex = commonNodeEdges[areteIndex1 * numEdges + areteIndex2];
                    if (commonNodeIndex == id) {
                        commonNodex = newX;
                        commonNodey = newY;
                    }
                    else {
                        commonNodex = nodes[commonNodeIndex * 2];
                        commonNodey = nodes[commonNodeIndex * 2 + 1];
                    }
                    if (surSegmentReelCUDA(node3x, node3y, node4x, node4y, commonNodex, commonNodey)) { score += PENALITE_MAX_SELF; }
                }
            }
        }
    }
    scores[i] = score;
}

__global__ void kernelUpdateArray(int* nodes, int* slots, int* newSlotPos, int* nodeId, int bestIndex) {
    int firstNodeId = nodeId[bestIndex];
    int newSlotId = newSlotPos[bestIndex];
    int oldSlotId = nodes[firstNodeId];
    if (slots[newSlotId] != -1) {
        int secondNodeId = slots[newSlotId];
        nodes[secondNodeId] = oldSlotId;
        nodes[firstNodeId] = newSlotId;
        slots[oldSlotId] = secondNodeId;
        slots[newSlotId] = firstNodeId;
    }
    else {
        nodes[firstNodeId] = newSlotId;
        slots[oldSlotId] = -1;
        slots[newSlotId] = firstNodeId;
    }
}

__global__ void kernelUpdateArrayReel(float* nodes, float* newCoords, int* nodeId, int bestIndex) {
    int id = nodeId[bestIndex] * 2;
    nodes[id] = newCoords[bestIndex * 2];
    nodes[id + 1] = newCoords[bestIndex * 2 + 1];
}

extern "C" void rechercheTabouGPU(int* nodes, const int* edges, const int* slots, const int* slotsCoord, int* scores, const int* newSlotPos, const int* nodeId, const int* commonNodeEdges, const int numNodes, const int numEdges, const int numSlots, const int blockSize, const int gridSize, const int placementScore, const int PENALITE_MAX, const int PENALITE_MAX_SELF) {
    int* devNodes;
    int* devEdges;
    int* devSlots;
    int* devSlotsCoord;
    int* devScores;
    int* devNewSlotPos;
    int* devNodeId;
    int* devCommonNodeEdges;

    int numThreads = blockSize * gridSize;

    // Memory allocation
    cudaMalloc((void**)&devNodes, sizeof(int) * numNodes);
    cudaMalloc((void**)&devEdges, sizeof(int) * numEdges * 2);
    cudaMalloc((void**)&devSlots, sizeof(int) * numSlots);
    cudaMalloc((void**)&devSlotsCoord, sizeof(int) * numSlots * 2);
    cudaMalloc((void**)&devScores, sizeof(int) * numThreads);
    cudaMalloc((void**)&devNewSlotPos, sizeof(int) * numThreads * 2);
    cudaMalloc((void**)&devNodeId, sizeof(int) * numThreads);
    cudaMalloc((void**)&devCommonNodeEdges, sizeof(int) * numEdges * numEdges);

    // Copy vectors datas from host to device
    cudaMemcpy(devNodes, nodes, sizeof(int) * numNodes, cudaMemcpyHostToDevice);
    cudaMemcpy(devEdges, edges, sizeof(int) * numEdges * 2, cudaMemcpyHostToDevice);
    cudaMemcpy(devSlots, slots, sizeof(int) * numSlots, cudaMemcpyHostToDevice);
    cudaMemcpy(devSlotsCoord, slotsCoord, sizeof(int) * numSlots * 2, cudaMemcpyHostToDevice);
    cudaMemcpy(devScores, scores, sizeof(int) * numThreads, cudaMemcpyHostToDevice);
    cudaMemcpy(devNewSlotPos, newSlotPos, sizeof(int) * numThreads, cudaMemcpyHostToDevice);
    cudaMemcpy(devNodeId, nodeId, sizeof(int) * numThreads, cudaMemcpyHostToDevice);
    cudaMemcpy(devCommonNodeEdges, commonNodeEdges, sizeof(int) * numEdges * numEdges, cudaMemcpyHostToDevice);

    int bestOverallScore = placementScore;
    for (int iter = 0; iter < 100; iter++) {
        // Update crossing score
        kernelUpdateCrossings<<<gridSize,blockSize>>>(devNodes, devEdges, devSlots, devSlotsCoord, devScores, devNewSlotPos, devNodeId, numThreads, numNodes, numEdges, numSlots, devCommonNodeEdges, PENALITE_MAX, PENALITE_MAX_SELF);
        // Update vector device to host
        cudaMemcpy(scores, devScores, sizeof(int) * numThreads, cudaMemcpyDeviceToHost);
        int bestValue = scores[0];
        int bestIndex = 0;
        for (int i = 1; i < numThreads; i++) {
            if (scores[i] < bestValue) {
                bestValue = scores[i];
                bestIndex = i;
            }
        }
        kernelUpdateArray<<<1,1>>>(devNodes, devSlots, devNewSlotPos, devNodeId, bestIndex);
        cudaDeviceSynchronize();
        if (bestValue < bestOverallScore) {
            bestOverallScore = bestValue;
            cudaMemcpy(nodes, devNodes, sizeof(int) * numNodes, cudaMemcpyDeviceToHost);
        }
    }
    std::cout << "Best Score GPU: " << bestOverallScore << std::endl;
    //for (int i = 0; i < numThreads; i++) { std::cout << "i: " << i << " " << scores[i] << std::endl; }
    //std::cout << "score: " << scores[0] << std::endl;
    // cleanup
    cudaFree(devNodes);
    cudaFree(devEdges);
    cudaFree(devSlots);
    cudaFree(devSlotsCoord);
    cudaFree(devScores);
    cudaFree(devNewSlotPos);
    cudaFree(devNodeId);
    cudaFree(devCommonNodeEdges);
}

extern "C" void rechercheTabouGPUReel(float* nodes, const int* edges, int* scores,const float* newCoords, const int* nodeId, const int* commonNodeEdges, const int numNodes, const int numEdges, const int blockSize, const int gridSize, const int gridWidth, const int gridHeight, const int placementScore, const int PENALITE_MAX, const int PENALITE_MAX_SELF,const double timeLimit) {
    float* devNodes;
    int* devEdges;
    int* devScores;
    float* devNewCoords;
    int* devNodeId;
    int* devCommonNodeEdges;

    int numThreads = blockSize * gridSize;

    // Memory allocation
    cudaMalloc((void**)&devNodes, sizeof(float) * numNodes * 2);
    cudaMalloc((void**)&devEdges, sizeof(int) * numEdges * 2);
    cudaMalloc((void**)&devScores, sizeof(int) * numThreads);
    cudaMalloc((void**)&devNewCoords, sizeof(float) * numThreads * 2);
    cudaMalloc((void**)&devNodeId, sizeof(int) * numThreads);
    cudaMalloc((void**)&devCommonNodeEdges, sizeof(int) * numEdges * numEdges);

    // Copy vectors datas from host to device
    cudaMemcpy(devNodes, nodes, sizeof(float) * numNodes * 2, cudaMemcpyHostToDevice);
    cudaMemcpy(devEdges, edges, sizeof(int) * numEdges * 2, cudaMemcpyHostToDevice);
    cudaMemcpy(devScores, scores, sizeof(int) * numThreads, cudaMemcpyHostToDevice);
    cudaMemcpy(devNewCoords, newCoords, sizeof(float) * numThreads * 2, cudaMemcpyHostToDevice);
    cudaMemcpy(devNodeId, nodeId, sizeof(int) * numThreads, cudaMemcpyHostToDevice);
    cudaMemcpy(devCommonNodeEdges, commonNodeEdges, sizeof(int) * numEdges * numEdges, cudaMemcpyHostToDevice);
    auto start = std::chrono::system_clock::now();
    auto end = start;
    std::chrono::duration<double> secondsTotal = end - start;
    int bestOverallScore = placementScore;
    while(secondsTotal.count() < timeLimit) {
    //for (int iter = 0; iter < 10000; iter++) {
        // Update crossing score
        kernelUpdateCrossingsReel<<<gridSize, blockSize>>>(devNodes, devEdges, devScores, devNewCoords, devNodeId, numThreads, numNodes, numEdges, devCommonNodeEdges,gridWidth,gridHeight,PENALITE_MAX,PENALITE_MAX_SELF);
        // Update vector device to host
        cudaMemcpy(scores, devScores, sizeof(int) * numThreads, cudaMemcpyDeviceToHost);
        int bestValue = scores[0];
        int bestIndex = 0;
        for (int i = 1; i < numThreads; i++) {
            if (scores[i] < bestValue) {
                bestValue = scores[i];
                bestIndex = i;
            }
        }
        kernelUpdateArrayReel<<<1,1>>>(devNodes, devNewCoords, devNodeId, bestIndex);
        cudaDeviceSynchronize();
        tcout() << "GPU score actuel: " << bestValue << std::endl;
        if (bestValue < bestOverallScore) {
            bestOverallScore = bestValue;
            cudaMemcpy(nodes, devNodes, sizeof(float) * numNodes * 2, cudaMemcpyDeviceToHost);
#if defined(DEBUG_GRAPHE_PROGRESS)
            tcout() << "GPU new best: " << bestOverallScore << std::endl;
#endif
        }
        end = std::chrono::system_clock::now();
        secondsTotal = end - start;
    }
    tcout() << "Best Score GPU: " << bestOverallScore << std::endl;
    //for (int i = 0; i < numThreads; i++) { std::cout << "i: " << i << " " << scores[i] << std::endl; }
    //std::cout << "score: " << scores[0] << std::endl;
    // cleanup
    cudaFree(devNodes);
    cudaFree(devEdges);
    cudaFree(devScores);
    cudaFree(devNewCoords);
    cudaFree(devNodeId);
    cudaFree(devCommonNodeEdges);
}