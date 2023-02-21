#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <random>
#include "emplacement.hpp"

// Retourne une valeur réelle comprise dans [0.0,n[
double generateDoubleRand(double n);

// Retourne une valeur entiere comprise dans [0,n]
int generateRand(int n);

// Indique si une valeur est dans un vecteur ou non
bool isInVector(std::vector<int>& vectorInt, int x);

// Indique si une valeur est dans un vecteur ou non
bool isInVector(std::vector<std::string>& vectorString, std::string x);

// Enleve x du vecteur vectorInt
void removeFromVector(std::vector<int>& vectorInt, int x);

double moyenneVector(std::vector<int>& vec);

double moyenneVector(std::vector<double>& vec);

// Le vecteur doit etre trie avant l'appel de cette fonction
long medianeVector(std::vector<int> &vec);

int lowestInVector(std::vector<int> &vec);

// Fonction de sort par x
bool comparePtrEmplacement(Emplacement* a, Emplacement* b);

// Fonction de sort par x et y
bool comparePtrEmplacementTri(Emplacement* a, Emplacement*b);

void initSameSeed();

void initRandomSeed();

std::string getTypeSeed();

#endif