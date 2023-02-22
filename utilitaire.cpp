#include "utilitaire.hpp"
#include <random>
#include <algorithm>
#include "emplacement.hpp"
#include <iostream>
#include <omp.h>

using std::min;
using std::max;

std::vector<std::mt19937*> genVector;
thread_local int numGen=0;

bool isSeedRandom;
int seed;

std::string typeSeed;

//std::random_device rd;  // Will be used to obtain a seed for the random number engine
//std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
//std::mt19937 gen(0); // Standard mersenne_twister_engine seeded with rd()

// Retourne une valeur réelle comprise dans [0.0,n[
double generateDoubleRand(double n) {
    std::uniform_real_distribution<> dis(0.0, n);
    return dis(*genVector[numGen]);
}

// Retourne une valeur entiere comprise dans [0,n]
int generateRand(int n) {
    std::uniform_int_distribution<> dis(0, n);
    return dis(*genVector[numGen]);
}

void initSameSeed(int n) {
    std::cout << "---------- SEED FIXE: 0 ----------\n";
    typeSeed = "FIXE";
    isSeedRandom = false;
    seed = n;
    int maxThread = omp_get_max_threads();
    for (int i=0;i<maxThread;i++) {
        genVector.push_back(new std::mt19937(n));
    }
}

void initRandomSeed() {
    std::cout << "---------- SEED RANDOM ----------\n";
    typeSeed = "RANDOM";
    isSeedRandom = true;
    int maxThread = omp_get_max_threads();
    for (int i=0;i<maxThread;i++) {
        std::random_device rd;
        genVector.push_back(new std::mt19937(rd()));
    }
}

void resetSeed(int numThread) {
    numGen = numThread;
    if (isSeedRandom) {
        std::random_device rd;
        genVector[numThread] = new std::mt19937(rd());
    }
    else {
        genVector[numThread] = new std::mt19937(seed);
    }
}

std::string getTypeSeed() {
    return typeSeed;
}

// Indique si une valeur est dans un vecteur ou non
bool isInVector(std::vector<int>& vectorInt, int x) {
    if (std::find(vectorInt.begin(),vectorInt.end(),x) != vectorInt.end())
        return true;
    return false;
}

bool isInVector(std::vector<std::string>& vectorString, std::string x) {
    if (std::find(vectorString.begin(),vectorString.end(),x) != vectorString.end())
        return true;
    return false;
}

double moyenneVector(std::vector<int>& vec) {
	double moyenne = 0;
	for (const int& elem : vec) {
		moyenne += elem;
	}
	moyenne = moyenne / (double)vec.size();
	return moyenne;
}

double moyenneVector(std::vector<double>& vec) {
	double moyenne = 0;
	for (const double& elem : vec) {
		moyenne += elem;
	}
	moyenne = moyenne / (double)vec.size();
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

bool comparePtrEmplacementTri(Emplacement* a, Emplacement*b) {
    if (a->getX() == b->getX()) {
        return (a->getY() < b->getY());
    }
    return (a->getX() < b->getX());
}

void removeFromVector(std::vector<int>& vec, int x) {
    for (int i=0;i<vec.size();i++) {
        if (vec[i] == x) {
            vec[i] = vec[vec.size()-1];
            vec.pop_back();
            return;
        }
    }
}

int nombreIterationRecuit(double t,double cool,double seuil) {
    return (int)(((log(seuil/t)))/(log(cool)));
}