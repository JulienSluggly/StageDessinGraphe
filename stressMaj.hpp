#ifndef STRESSMAJ_HPP
#define STRESSMAJ_HPP

#include "graphe.hpp"

class StressMajorization {
public:
    StressMajorization(){}

    double m_epsilon=0.0001;
    int m_npivot = 50;
    int m_edgeCosts = 100;

    void initMatrices(Graphe& G, std::vector<std::vector<double>>& shortestPathMatrix, std::vector<std::vector<double>>& weightMatrix);

    void runAlgo(Graphe& G);

    void bfs_SPAP(Graphe& G, std::vector<std::vector<double>>& shortestPathMatrix, int edgeCosts);

    void bfs_SPSS(Graphe& G, int nodeId, std::vector<double>& shortestPathArray, int edgeCosts);

    void calcWeights(Graphe& G, std::vector<std::vector<double>>& shortestPathMatrix, std::vector<std::vector<double>>& weightMatrix);

    void minimizeStress(Graphe& G, std::vector<std::vector<double>>& shortestPathMatrix, std::vector<std::vector<double>>& weightMatrix);
    
    void runAlgo(Graphe& G, std::vector<std::vector<double>>& shortestPathMatrix, std::vector<std::vector<double>>& weightMatrix);

};

#endif