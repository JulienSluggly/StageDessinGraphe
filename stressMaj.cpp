#include "stressMaj.hpp"

void StressMajorization::initMatrices(Graphe& G, std::vector<std::vector<double>>& shortestPathMatrix, std::vector<std::vector<double>>& weightMatrix) {
    for (int i=0;i<G._noeuds.size();i++) {
        std::vector<double> tmpArray;
        for (int j=0;j<G._noeuds.size();j++) {
            if (i==j) {
                tmpArray.push_back(0);
            }
            else {
                tmpArray.push_back(std::numeric_limits<double>::infinity());
            }
        }
        shortestPathMatrix.push_back(tmpArray);
    }
}

void StressMajorization::bfs_SPSS(Graphe& G, int nodeId, std::vector<double>& shortestPathArray, int edgeCosts) {
    std::vector<bool> mark(G._noeuds.size(),false);
    std::vector<int> bfs;
    bfs.push_back(nodeId);
    // mark s and set distance to itself 0
    mark[nodeId] = true;
    shortestPathArray[nodeId] = 0;
    int numero = 0;
    while (numero < bfs.size()) {
        int id = bfs[numero];
        double dist = shortestPathArray[id] + edgeCosts;
        for (Noeud* voisin : G._noeuds[id].voisins) {
            int voisinId = voisin->_id;
            if (!mark[voisinId]) {
                mark[voisinId] = true;
                bfs.push_back(voisinId);
                shortestPathArray[voisinId] = dist;
            }
        }
        numero++;
    }
}

void StressMajorization::bfs_SPAP(Graphe& G, std::vector<std::vector<double>>& shortestPathMatrix, int edgeCosts) {
    for (int i=0;i<G._noeuds.size();i++) {
        bfs_SPSS(G,i,shortestPathMatrix[i],edgeCosts);
    }
}

void StressMajorization::calcWeights(Graphe& G, std::vector<std::vector<double>>& shortestPathMatrix, std::vector<std::vector<double>>& weightMatrix) {
    for (int i=0;i<G._noeuds.size();i++) {
        for (int j=0;j<G._noeuds.size();j++) {
            if (i!=j) {
                // w_ij = d_ij^-2
                weightMatrix[i][j] = 1 / (shortestPathMatrix[i][j] * shortestPathMatrix[i][j]);
            }
        }
    }
}

void StressMajorization::minimizeStress(Graphe& G, std::vector<std::vector<double>>& shortestPathMatrix, std::vector<std::vector<double>>& weightMatrix) {
    printf("NYI\n");
}

void StressMajorization::runAlgo(Graphe& G, std::vector<std::vector<double>>& shortestPathMatrix, std::vector<std::vector<double>>& weightMatrix) {
    printf("NYI\n");
}

void StressMajorization::runAlgo(Graphe& G) {
    std::vector<std::vector<double>> shortestPathMatrix;
    std::vector<std::vector<double>> weightMatrix;
    initMatrices(G,shortestPathMatrix,weightMatrix);
    bfs_SPAP(G,shortestPathMatrix,m_edgeCosts);
    runAlgo(G,shortestPathMatrix,weightMatrix);
}