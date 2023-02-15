﻿#include <string>
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
#include "utilitaire.hpp"

using namespace std;

int main() {
	//initSameSeed();
	initRandomSeed();
	//customRecuit();
	//allRunsSingleThread();
	//return 0;

	Graphe G;
	
	string nomFichierGraph = "graph-10-input";
	string nomFichierSlots = "10-input-slots";
	string fileGraph = chemin + "exemple/Graphe/" + nomFichierGraph + ".json";
	string fileSlots = chemin + "exemple/Slots/" + nomFichierSlots + ".json";
	
	string fileGraphSlots = chemin + "combined/exemple3.json";
	
	string fileOldGraph = chemin + "automatique/auto21-13.json";
	G.readFromJsonGraph(fileGraph);
	//G.generateGrid();
	//ogdfReverse(G);
	ogdfCrossingNumbers({"graph-8-input"});
	return 0;
	G.readFromJsonSlots(fileSlots);

	//G.readFromJsonOldGraph(fileOldGraph);
	//G.generateGrid(3000,3000);
	//readFromJsonGraphAndSlot(G,fileGraphSlots);

	std::cout << "Debut placement.\n";
	auto start = std::chrono::system_clock::now();
	double timeBest = -1; int lastIteration = -1; int bestIteration = -1;
	//G.grapheGenetique(timeBest,bestIteration,lastIteration,100,1000,fileGraph,fileSlots,true,false,3);

	//G.placementAleatoire();
	//G.triangulationDelaunay();
	//G.recuitSimuleCustom(timeBest,0.99999, 100.0, 1, 0, 3, {1,15});

	//G.initGraphAndNodeScoresAndCrossings();

	//G.recuitSimule(timeBest,0.99999,100.0,1,0,4);

	
	//G.recuitSimuleScore(timeBest);

	//ogdfRun(G);
	ogdfReverseNonPlanar(G);
	//return 0;
	
	//ogdfPlacementAuPlusProche(G);

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> secondsTotal = end - start;
	std::cout << secondsTotal.count() << "s for placement.\n";
	if (timeBest != -1) std::cout << timeBest << "s meilleur resultat.\n";
	if (bestIteration != -1) std::cout << "A la " << bestIteration << "eme iteration\n";
	if (lastIteration != -1) std::cout << "Max iteration: " << lastIteration << "\n";
	if (G.estPlace()) std::cout << "Nombre intersection apres placement: " << G.getNbCroisementConst() << std::endl;
	std::cout << "Setup complete!" << std::endl;

	//G.afficherInfo();
	if (G.estPlace()) G.debugEverything(false,false);

	// OpenGL
	bool useOpenGL = true;
	if (useOpenGL) {
		G.DEBUG_OPENGL = true;
		int maxX = G.gridWidth, maxY = G.gridHeight;
		std::cout << "Grid: " << G.gridWidth << " " << G.gridHeight << std::endl;
		dispOpenGL(G, G.gridWidth+1, G.gridHeight+1, maxX, maxY);
	}
	return 0;
}