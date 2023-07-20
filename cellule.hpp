#ifndef CELLULE_HPP
#define CELLULE_HPP

#include <mutex>

class Cellule {
public:
	int _id; // id unique
    int _numeroX; // numero de la colone dans la grille
    int _numeroY; // numero de la ligne dans la grille

    double _x1reel, _y1reel; // Coin haut gauche
    double _x2reel, _y2reel; // Coin bas droite

    std::vector<int> vecAreteId;

    std::vector<int> containAreteId;

    std::vector<int> vecSingleNodeId;
    std::vector<int> containSingleNodeId;

    Cellule(int id, int nx, int ny, double x1, double y1, double x2, double y2, int nbArete, int nbNode) {
        _id = id; _numeroX = nx; _numeroY = ny;
        _x1reel = x1; _x2reel = x2; _y1reel = y1; _y2reel = y2;
        containAreteId.resize(nbArete,-1);
        containSingleNodeId.resize(nbNode,-1);
    }

    double getTopLeftXReel() { return _x1reel; }
    double getTopLeftYReel() { return _y1reel; }
    double getTopRightXReel() { return _x2reel; }
    double getTopRightYReel() { return _y1reel; }
    double getBottomLeftXReel() { return _x1reel; }
    double getBottomLeftYReel() { return _y2reel; }
    double getBottomRightXReel() { return _x2reel; }
    double getBottomRightYReel() { return _y2reel; }

};

#endif