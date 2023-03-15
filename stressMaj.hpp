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

    double m_edgeCosts;
    int m_iterations = 400;

    bool m_useGrille = true;
    bool areVectorsSetup = false;

    bool DEBUG_STRESS = false;

    int totalIterationDone = 0;

    void initMatrices();

    void runAlgo(std::vector<double> customParam={});

    void bfs_SPAP();

    void bfs_SPSS(int nodeId, std::vector<double>& shortestPathArray);

    void calcWeights();

    void minimizeStress(std::vector<double> customParam={});

    double minimizeStressDynStress(int nombreIter=400, std::vector<double> customParam={});

    long minimizeStressDynCross(int nombreIter=400, std::vector<double> customParam={});

    bool nextIteration(std::vector<double> customParam={});

    bool nextIterationDelay(int iteration);

    bool nextIterationDelayTest(int iteration,std::vector<double> customParam={});

    bool finished(int numberOfPerformedIterations);

    void replaceInfinityDistances(double newVal);

    void runStepAlgo(std::vector<double> customParam={});

    double calcStress();

    void runAlgoDynStress();

    void addToEdgeCost(double n);

    void setEdgeCost(double n);

    void updateStressCoord();

    void runAlgoDynCross();

    void runAlgoDynDichStress();

};

#endif