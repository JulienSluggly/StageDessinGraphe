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

void initSameSeed(unsigned int n=0,bool resetting=true);

void initRandomSeed(bool resetting=false);

void resetSeed(int numThread, bool resetSameSeed=false, bool forceReset=false);

std::string getTypeSeed();

int nombreIterationRecuit(double t,double cool,double seuil);

unsigned int getSeed(int tid);

// Renvoie vrai si la seed est fixe.
bool isSeedFixe();

// Renvoie vrai si la seed est reset a chaque run dans logCSV
bool isSeedResetting(int numThread);

// Renvoie true si le string b est présent dans le string a
bool containsString(std::string a, std::string b);

#endif