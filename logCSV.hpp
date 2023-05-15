#ifndef LOGCSV_HPP
#define LOGCSV_HPP

#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include "ogdfFunctions.hpp"
#include "personnel.hpp"
#include "graphe.hpp"
#include <omp.h>
#include <climits>
#include <ctime>
#include <algorithm>

class LogGraphe {
public:

	std::string nomGraphe;
	std::string methodePlacementName;
	std::string methodeAlgoName;
	std::string fileGraph;
	std::string fileSlots;
	bool updateScore; // Est ce que le graphe utilise les algoritmes stockant le score de chaque noeud/aretes
	bool isGenetique;
	bool isRecuit;
	bool needTriangulation;
	bool needGrille; // Est ce que la structure de cellules est utilisée

	std::vector<long> scoreVector; // Nombre d'intersection AVEC pénalité de chaque run
	std::vector<double> tempsExecVector; // Temps total mis par une run (debut boucle a fin de la boucle)
	std::vector<double> tempsBestVector; // Temps mis pour trouver le meilleur resultat de chaque execution
	std::vector<double> tempsPlacementVector; // Temps de chaque placement
	std::vector<int> bestIterationVector; // meilleur itération GENETIQUE
	std::vector<int> lastIterationVector; // derniere itération GENETIQUE
	std::vector<int> nombreRecuitVector; // Nombre de rechauffement+1 a chaque run
	std::vector<int> totalInterVector; // Nombre d'intersection SANS pénalité de chaque run
	std::vector<int> totalInterIllVector; // Nombre d'intersection illégales de chaque run
	std::vector<int> placementInterVector; // Nombre d'intersection SANS pénalité de chaque run
	std::vector<int> edgeCostVector; // Si stressminimization custom, contient le edgecost choisi a chaque run
	std::vector<int> recuitCycleVague; // Contient le nombre de recuit qui améliore et qui n'améliore pas, ordonné

	std::vector<std::vector<double>> customParam;

	int nbSolutionIllegale = 0;
	int debugValue=-1; // Voir fonction dans grapheDebug.cpp pour la correspondance du code d'erreur si différent de -1.

	int tid; // Thread ID

	int timeLimitRecuit;
	int nombreRecuit = 0;
	int nombreSlots;
	int nombreCellule;

	double tempsBest = -1;
	int bestIteration = -1; // Genetique
	int lastIteration = -1; // Genetique
	int genetiquePopulation;
	int genetiqueMaxIteration; // Genetique

	int currentIteration = 1;

	std::chrono::_V2::system_clock::time_point start; // Temps de debut de run
	std::chrono::_V2::system_clock::time_point finPlacement; // Temps de fin de placement

    LogGraphe(const std::string& fileG, const std::string& fileS, const std::string& methodePlacement, const std::string& methodeAlgo, std::vector<std::vector<double>>& params,int threadId);

	void writeGrapheToFile(Graphe& G, int tid);
	void writeResultToFile();
	void writeRecuitResultToFile();
	void writeGenetiqueResultToFile();
	void updateDataFromGraphe(Graphe& G);
	std::string getParamAsString();
	std::string getRecuitCycleAsString();
	void saveGrapheDistanceCSV(Graphe& G, int tid);
	void saveGrapheScoreTempsCSV(Graphe& G, int tid);
	int setTimeLimitRecuit();
	void setupGrapheWithTypeFile(Graphe& G, std::string& typeFile, std::string& fileGraph, std::string& fileSlots);
	void placementGraphe(Graphe& G);
	void setupGrapheStruct(Graphe& G);
	void heuristiqueGraphe(Graphe& G);

};

void generateCSV(int nbEssay, const std::string& methodePlacementName, const std::string& methodeAlgoName, std::string fileGraph, std::string fileSlots, std::vector<std::vector<double>> customParam={{}}, bool useReel=false, int tid=0, std::string typeFile="JSON", int timeLimit=3600);


#endif
