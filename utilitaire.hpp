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

double moyenneVector(std::vector<int>& vec, int nbEssay);

double moyenneVector(std::vector<double>& vec, int nbEssay);

// Le vecteur doit etre trie avant l'appel de cette fonction
long medianeVector(std::vector<int> &vec);

int lowestInVector(std::vector<int> &vec);

bool comparePtrEmplacement(Emplacement* a, Emplacement* b);

#endif