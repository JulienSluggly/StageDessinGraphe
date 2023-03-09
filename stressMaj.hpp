#ifndef STRESSMAJ_HPP
#define STRESSMAJ_HPP

#include <vector>

class Graphe;
class StressMajorization {
public:
    std::vector<std::vector<double>> shortestPathMatrix;
    std::vector<std::vector<double>> weightMatrix;
    Graphe* G = nullptr;

    StressMajorization(){}
    StressMajorization(Graphe* _g){ G = _g; }

    double m_epsilon=0.0001;
    int m_npivot = 50;
    //int m_edgeCosts = 30;
    int m_edgeCosts = 45;
    int m_iterations = 400;

    bool m_useGrille = true;
    bool areVectorsSetup = false;

    int totalIterationDone = 0;

    void initMatrices();

    void runAlgo();

    void bfs_SPAP(int edgeCosts);

    void bfs_SPSS(int nodeId, std::vector<double>& shortestPathArray, int edgeCosts);

    void calcWeights();

    void minimizeStress();

    bool nextIteration();

    bool nextIterationDelay(int iteration);

    bool finished(int numberOfPerformedIterations);

    void replaceInfinityDistances(double newVal);

    void runStepAlgo();

};

#endif