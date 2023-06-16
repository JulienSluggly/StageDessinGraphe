#ifndef GRAPHIQUE_HPP
#define GRAPHIQUE_HPP

#include <string>
#include <vector>

class GraphPoint {
public:
    double x=-1;
    double y;

    static double sizePoint;

    GraphPoint(double value);
    bool isPlaced();
    bool isCrossing(GraphPoint& p);

    bool operator < (const GraphPoint& gp) const { return (y < gp.y); }

};

void createGraphiqueFromData(std::string output, std::vector<double>& data, double largeurColonne, double taillePoint=7);
void generateRandomGraphique(std::string output, int nbPoint=50, double taillePoint=7, double largeurColonne=100);

#endif