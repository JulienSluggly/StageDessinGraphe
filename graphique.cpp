#include "graphique.hpp"
#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>
#include <iomanip>

double GraphPoint::sizePoint = 7.0;

GraphPoint::GraphPoint(double valeur) { y = valeur; }

bool GraphPoint::isPlaced() { return (x == -1); }

bool GraphPoint::isCrossing(GraphPoint& p) { return (((p.x-x)*(p.x-x)+(p.y-y)*(p.y-y)) <= GraphPoint::sizePoint); }

bool isPointCrossingOthers(GraphPoint& p, std::vector<GraphPoint*>& pointsPlace) {
    for (GraphPoint* gp : pointsPlace) {
        if (gp->isCrossing(p)) return true;
    }
    return false;
}

void decallePoint(GraphPoint& gp, double currentStep, double centreColonne, int side) {
    if (side == 0) { gp.x = centreColonne + currentStep; }
    else { gp.x = centreColonne - currentStep; }
}

void createGraphiqueFromData(std::string output, std::vector<double>& data, double largeurColonne, double taillePoint) {
    GraphPoint::sizePoint = taillePoint;
    std::vector<GraphPoint> points;
    for (const double& value : data) { points.push_back(GraphPoint(value)); }
    sort(points.begin(), points.end());
    double centreColonne = largeurColonne/2.0;
    std::vector<GraphPoint*> pointsPlace;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);

    for (GraphPoint& gp : points) {
        gp.x = centreColonne;
        bool limiteCheck = false;
        double step = taillePoint / 10.0;
        double currentStep = step;
        int side = dis(gen);
        while(isPointCrossingOthers(gp,pointsPlace)) {
            decallePoint(gp,currentStep,centreColonne,side);
            if (isPointCrossingOthers(gp,pointsPlace)) {
                side = (side+1)%2;
                decallePoint(gp,currentStep,centreColonne,side);
                currentStep += step;
                if (currentStep >= centreColonne) { break; }
            }
            else { break; }
        }
        pointsPlace.push_back(&gp);
    }
    std::string nomFichier = output + ".csv";
	std::ofstream resultats(nomFichier, std::ios_base::app);
    for (const GraphPoint& gp : points) {
        resultats << std::fixed << gp.x << "," << gp.y << std::endl;
    }
    resultats.close();
}

void generateRandomGraphique(std::string output, int nbPoint, double taillePoint, double largeurColonne) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 150);
    std::vector<double> data;
    for (int i=0;i<nbPoint;i++) {
        data.push_back((double)dis(gen));
    }

    std::vector<double> data1 = {4,48,39,27,21,32,52,28,40,10,23,9,34,31,23,26,33,18,23,25,26,24,13,7,35,17,6,10,36,26,19,32,29,52,18,16,38,10,16,22,35,9,36,22,9,22,21,12,27,28,18,9,33,14,43,32,32,46,67,38,31,27,14,31,42,20,13,28,64,23,18,37,12,16,35,31,44,21,15,32,25,29,45,15,8,37,36,29,29,41,19,15,11,56,14,14,28,12,9,28,25,14,22,31,11,16,3,18,34,12,31,46,39,14,18,11,15,22,45,21,29,38,32,28,20,26,23,19,26,17,62,24,28,84,36,15,17,17,45,14};
    std::vector<double> data2 = {23,26,36,27,32,21,27,25,20,17,24,33,27,26,32,29,33,17,48,23,40,12,13,26,7,18,29,21,42,29,10,34,35,31,18,44,45,34,18,34,15,23,37,32,33,67,27,42,29,29,17,15,16,13,4,19,11,40,85,23,16,5,18,9,19,14,21,15,52,13,27,4,9,9,34,20,13,32,9,32,17,22,39,22,30,9,9,34,40,15,24,21,46,15,17,43,19,28,14,37,10,38,65,12,52,25,30,16,38,44,40,24,15,31,21,24,23,33,29,14,31,29,10,14,31,34,27,12,38,27,36,18,62,14,10,10,46,27,18,57};
    std::vector<double> data3 = {12,112,121,53,51,81,130,72,100,27,62,24,101,79,54,72,77,45,45,83,51,64,35,20,94,25,21,30,86,56,50,85,83,113,39,42,97,20,53,51,95,25,90,61,30,51,64,28,61,72,45,25,82,30,115,92,97,92,171,100,79,80,44,93,102,57,36,79,181,61,47,89,34,42,87,79,111,69,34,76,59,68,109,49,24,95,74,85,70,106,47,38,38,133,32,43,64,35,20,74,65,35,60,81,24,43,11,51,94,44,80,126,101,33,36,27,50,59,121,56,61,100,77,75,58,67,62,46,69,38,154,56,71,199,97,37,44,34,116,36};
    std::vector<double> data4 = {102,99,111,93,132,86,82,99,80,66,86,105,111,84,107,104,110,63,170,100,140,57,56,92,38,69,103,108,157,102,53,136,122,145,79,166,140,92,54,113,71,107,151,107,120,254,90,149,104,133,68,51,88,46,21,83,39,152,271,103,71,36,82,30,80,56,78,54,168,65,97,23,46,37,141,93,68,134,52,104,60,102,146,81,161,48,41,160,143,46,70,75,127,53,75,151,94,119,50,146,30,140,257,96,177,97,105,70,129,155,157,92,50,121,100,91,83,109,103,61,139,107,58,47,128,125,117,60,137,118,154,67,175,66,46,41,189,92,77,191};
    createGraphiqueFromData("data1", data1,largeurColonne,taillePoint);
    createGraphiqueFromData("data2", data2,largeurColonne,taillePoint);
    createGraphiqueFromData("data3", data3,largeurColonne,taillePoint);
    createGraphiqueFromData("data4", data4,largeurColonne,taillePoint);
}