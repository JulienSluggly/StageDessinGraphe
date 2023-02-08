#ifndef UTILITAIRE_HPP
#define UTILIRAIRE_HPP

#include <vector>
#include "aretes.hpp"
#include "point.hpp"
#include "intersection.hpp"
#include "emplacement.hpp"


using std::min;
using std::max;

// Retourne une valeur réelle comprise dans [0.0,n[
double generateDoubleRand(double n) {
    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(0.0, n);
    return dis(gen);
}

// Retourne une valeur entiere comprise dans [0,n]
int generateRand(int n) {
    std::random_device rd;  // Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
    std::uniform_int_distribution<> dis(0, n);
    return dis(gen);
}

// Indique si une valeur est dans un vecteur ou non
bool isInVector(std::vector<int>& vectorInt, int x) {
    for (int i = 0; i < vectorInt.size(); i++) {
        if (vectorInt[i] == x) return true;
    }
    return false;
}

bool comparePtrEmplacement(Emplacement* a, Emplacement* b) { return (a->getX() < b->getX()); }

#endif