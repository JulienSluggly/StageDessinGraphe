#include "graphe.hpp"
#include "intersection.hpp"
#include "ogdfFunctions.hpp"
#include <iostream>
#include <climits>

void Graphe::tirageCoordReel(std::pair<double,double>& coord) {
    coord.first = generateDoubleRandRecuitX();
    coord.second = generateDoubleRandRecuitY();
}

void Graphe::placementAleatoireReel() {
    #if defined(DEBUG_GRAPHE_PROGRESS)
        tcout() << "Placement aleatoire coord reel" << std::endl;
    #endif
    if ((gridWidth == 10)&&(gridHeight == 10)) {
        gridWidth = std::min((int)_noeuds.size()*2,6000);
        gridHeight = gridWidth;
    }
    for (int i = 0; i < _noeuds.size(); ++i) {
        int randLargeur = generateRand(gridWidth-1);
        int randHauteur = generateRand(gridHeight-1);
        double incrementX = generateDoubleRand(1);
        double incrementY = generateDoubleRand(1);
        _noeuds[i]._xreel = (double)randLargeur + incrementX;
        _noeuds[i]._yreel = (double)randHauteur + incrementY;
    }
    isNombreCroisementUpdated = false;
}

void Graphe::placementFMME(bool minute) {
#if defined(OGDF_INSTALLED)
    if (useCoordReel) {
        if (minute) {
            ogdfFastMultipoleMultilevelEmbedderReelMinute(*this);
        }
        else {
            ogdfFastMultipoleMultilevelEmbedderReel(*this);
        }
    }
#else
    if (useCoordReel) {
        placementAleatoireReel();
    }
    else {
        placementAleatoire();
    }
#endif
}