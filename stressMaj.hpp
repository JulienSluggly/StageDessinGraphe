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

    int m_edgeCosts;
    int m_iterations = 400;

    bool m_useGrille = true;
    bool areVectorsSetup = false;

    int totalIterationDone = 0;

    void initMatrices();

    void runAlgo(std::vector<double> customParam={});

    void bfs_SPAP(int edgeCosts);

    void bfs_SPSS(int nodeId, std::vector<double>& shortestPathArray, int edgeCosts);

    void calcWeights();

    void minimizeStress(std::vector<double> customParam={});

    bool nextIteration(std::vector<double> customParam={});

    bool nextIterationDelay(int iteration,std::vector<double> customParam={});

    bool finished(int numberOfPerformedIterations);

    void replaceInfinityDistances(double newVal);

    void runStepAlgo(std::vector<double> customParam={});

};

#endif