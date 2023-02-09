#include "utilitaire.hpp"
#include <random>
#include <algorithm>
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
    if (std::find(vectorInt.begin(),vectorInt.end(),x) != vectorInt.end())
        return true;
    return false;
}

// Indique si une valeur est dans un vecteur ou non
bool isInVector(std::vector<std::string>& vectorString, std::string x) {
    if (std::find(vectorString.begin(),vectorString.end(),x) != vectorString.end())
        return true;
    return false;
}

double moyenneVector(std::vector<int>& vec, int nbEssay) {
	double moyenne = 0;
	for (const int& elem : vec) {
		moyenne += elem;
	}
	moyenne = moyenne / (double)nbEssay;
	return moyenne;
}

double moyenneVector(std::vector<double>& vec, int nbEssay) {
	double moyenne = 0;
	for (const double& elem : vec) {
		moyenne += elem;
	}
	moyenne = moyenne / (double)nbEssay;
	return moyenne;
}

// Le vecteur doit etre trie avant l'appel de cette fonction
long medianeVector(std::vector<int> &vec) {
    int taille = vec.size();
    if (taille % 2 == 1) {
        return vec[taille / 2];
    }
    else {
        int nbCroisement1 = vec[taille / 2];
        int nbCroisement2 = vec[(taille / 2) - 1];

        return ((nbCroisement1 + nbCroisement2) / 2.0);
    }
}

int lowestInVector(std::vector<int> &vec) {
    int lowest = vec[0];
    for (int i=1;i<vec.size();i++) {
        if (vec[i] < lowest) {
            lowest = vec[i];
        }
    }
    return lowest;
}

bool comparePtrEmplacement(Emplacement* a, Emplacement* b) { return (a->getX() < b->getX()); }