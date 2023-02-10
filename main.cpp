#include <ogdf/basic/GridLayout.h> //Ne pas supprimer car magie noire
#include <string>
#include <stdio.h>
#include <random>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <set>

#include "graphe.hpp"
#include "dispOpenGL.hpp"
#include "noDispRuns.hpp"
#include "personnel.hpp"
#include "ogdfFunctions.hpp"
#include "logCSV.hpp"

using namespace std;

int main() {

	srand(static_cast<unsigned int>(time(NULL)));
	//srand(0);

	//allRunsSingleThread();
	//return 0;

	Graphe G;
	
	string nomFichierGraph = "auto21-13";
	string nomFichierSlots = "auto21-13";
	string fileGraph = chemin + "exemple/Graphe/" + nomFichierGraph + ".json";
	//string fileSlots = chemin + "exemple/Slots/" + nomFichierSlots + ".json";
	string fileSlots = chemin + "automatique/" + nomFichierSlots + ".json";
	string fileGraphSlots = chemin + "combined/exemple3.json";
	//G.readFromJsonGraph(fileGraph);
	//G.generateGrid(32,32);
	//G.readFromJsonSlots(fileSlots);
	G.readFromJsonOldGraph(fileSlots);
	G.generateGrid(3000,3000);
	//readFromJsonGraphAndSlot(G,fileGraphSlots);

	//G.testFunc(0);

	auto start = std::chrono::system_clock::now();
	double timeBest = -1; int lastIteration = -1; int bestIteration = -1;
	//G.grapheGenetique(timeBest,bestIteration,lastIteration,100,1000,fileGraph,fileSlots,true,false,3);

	G.placementAleatoire();

	//G.initGraphAndNodeScoresAndCrossings();
	G.afficherInfo();

	G.recuitSimule(timeBest,0.99999,100.0,1,0,0);

	
	//G.recuitSimuleScore(timeBest);

	//ogdfRun(G);
	//return 0;
	//ogdfReverse(G);

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> secondsTotal = end - start;
	std::cout << secondsTotal.count() << "s for placement.\n";
	if (timeBest != -1) std::cout << timeBest << "s meilleur resultat.\n";
	if (bestIteration != -1) std::cout << "A la " << bestIteration << "eme iteration\n";
	if (lastIteration != -1) std::cout << "Max iteration: " << lastIteration << "\n";
	std::cout << "Nombre intersection apres placement: " << G.getNbCroisementConst() << std::endl;
	std::cout << "Setup complete!" << std::endl;

	G.debugEverything(false,true);

	// OpenGL
	bool useOpenGL = true;
	if (useOpenGL) {
		int maxX = G.gridWidth, maxY = G.gridHeight;
		std::cout << "Grid: " << G.gridWidth << " " << G.gridHeight << std::endl;
		dispOpenGL(G, G.gridWidth+1, G.gridHeight+1, maxX, maxY);
	}
	return 0;
}