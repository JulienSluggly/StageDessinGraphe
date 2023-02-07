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
#include "jsonIO.hpp"
#include "dispOpenGL.hpp"
#include "noDispRuns.hpp"
#include "personnel.hpp"
#include "ogdfFunctions.hpp"
//#include "genetique.hpp"
#include "logCSV.hpp"

using namespace std;

int main() {
	//allRunsLogged();
	allRunsSingleThread();
	return 0;

	Graphe G;
	
	string nomFichierGraph = "graph-6-input";
	string nomFichierSlots = "6-input-slots";
	string fileGraph = chemin + "exemple/Graphe/" + nomFichierGraph + ".json";
	string fileSlots = chemin + "exemple/Slots/" + nomFichierSlots + ".json";
	string fileGraphSlots = chemin + "combined/exemple3.json";
	readFromJsonGraph(G, fileGraph);
	readFromJsonSlots(G, fileSlots);
	//readFromJsonGraphAndSlot(G,fileGraphSlots);

	srand(static_cast<unsigned int>(time(NULL)));
	//srand(0);


	specificGraphMulti(fileGraph, fileSlots);
	
	return 0;

	auto start = std::chrono::system_clock::now();
	//G.loadCopy(grapheGenetique(100,100,fileGraph,fileSlots,true,false,3));
	//G.placementFixe();
	G.placementAleatoire();
	//G.initGraphAndNodeScoresAndCrossings();
	G.recuitSimule();

	//ogdfRun(G);
	//return 0;
	//ogdfReverse(G);

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> secondsTotal = end - start;
	std::cout << secondsTotal.count() << "s for placement.\n";
	std::cout << "Nombre intersection apres placement: " << G.getNbCroisementConst() << std::endl;
	std::cout << "Setup complete!" << std::endl;

	G.debugEverything();

	// OpenGL
	bool useOpenGL = true;
	if (useOpenGL) {
		int maxX = G.gridWidth, maxY = G.gridHeight;
		std::cout << "Grid: " << G.gridWidth << " " << G.gridHeight << std::endl;
		dispOpenGL(G, G.gridWidth+1, G.gridHeight+1, maxX, maxY);
	}
	return 0;
}