#ifndef STRESSMAJ_HPP
#define STRESSMAJ_HPP

#include "graphe.hpp"

class StressMajorization {
public:
    StressMajorization(){}

    double m_epsilon=0.0001;
    int m_npivot = 50;
    //int m_edgeCosts = 30;
    int m_edgeCosts = 45;
    int m_iterations = 2000;

    bool m_useGrille = true;

    void initMatrices(Graphe& G, std::vector<std::vector<double>>& shortestPathMatrix, std::vector<std::vector<double>>& weightMatrix);

    void runAlgo(Graphe& G);

    void bfs_SPAP(Graphe& G, std::vector<std::vector<double>>& shortestPathMatrix, int edgeCosts);

    void bfs_SPSS(Graphe& G, int nodeId, std::vector<double>& shortestPathArray, int edgeCosts);

    void calcWeights(Graphe& G, std::vector<std::vector<double>>& shortestPathMatrix, std::vector<std::vector<double>>& weightMatrix);

    void minimizeStress(Graphe& G, std::vector<std::vector<double>>& shortestPathMatrix, std::vector<std::vector<double>>& weightMatrix);
    
    void runAlgo(Graphe& G, std::vector<std::vector<double>>& shortestPathMatrix, std::vector<std::vector<double>>& weightMatrix);

    void nextIteration(Graphe& G, std::vector<std::vector<double>>& shortestPathMatrix, std::vector<std::vector<double>>& weightMatrix);

    bool finished(Graphe& G, int numberOfPerformedIterations, std::vector<double>& newX, std::vector<double>& newY, double prevStress, double curStress);

    void replaceInfinityDistances(Graphe& G,std::vector<std::vector<double>>& shortestPathMatrix,double newVal);

};

#endif