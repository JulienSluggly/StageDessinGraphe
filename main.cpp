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
#include "utilitaire.hpp"

int main() {
	//initSameSeed();
	initRandomSeed();
	//std::vector<std::string> files = {"graph-1-input","graph-2-input","graph-3-input","graph-4-input","graph-5-input","graph-6-input","graph-7-input","graph-8-input","graph-9-input","graph-10-input","graph-11-input","graph-12-input"};
	//std::vector<std::string> files = {"graph-1-input","graph-2-input","graph-3-input","graph-4-input","graph-5-input","graph-6-input","graph-7-input","graph-8-input","graph-10-input","graph-11-input","graph-12-input"};
	//std::vector<std::string> files = {"graph-10-input"};
	//ogdfCrossingNumbers(files);
	//customRecuit();
	//allRunsSingleThread();
	//allRunsLogged();
	//customRecuitAllRuns();
	allRunsBySlots();
	return 0;

	//performanceTest();
	//return 0;

	Graphe G;
	std::string nomFichierGraph = "graph-2-input";
	std::string nomFichierSlots = "3X-2-input-slots";
	std::string fileGraph = chemin + "exemple/Graphe/" + nomFichierGraph + ".json";
	std::string fileSlots = chemin + "exemple/Slots/" + nomFichierSlots + ".json";
	
	std::string fileGraphSlots = chemin + "combined/exemple3.json";
	
	std::string fileOldGraph = chemin + "automatique/auto21-13.json";
	G.readFromJsonGraph(fileGraph);
	//G.generateGrid();
	G.readFromJsonSlots(fileSlots);

	//G.readFromJsonOldGraph(fileOldGraph);
	//G.generateGrid(3000,3000);
	//readFromJsonGraphAndSlot(G,fileGraphSlots);

	std::cout << "Debut placement.\n";
	G.DEBUG_GRAPHE = true;
	auto start = std::chrono::system_clock::now();
	double tempsBest = -1; int bestIteration = -1; int lastIteration = -1; int nombreRecuit=0; 
	//G.grapheGenetique(tempsBest,bestIteration,lastIteration,100,1000,fileGraph,fileSlots,true,false,3);
	//G.grapheGenetique(tempsBest,bestIteration,lastIteration,1000,1000,fileGraph,fileSlots,false,false,5);
	//std::cout << nombreIterationRecuit(150.0,0.999999,0.000001) << std::endl;
	G.placementAleatoire();
	G.triangulationDelaunay();
	G.initGrille();
	G.registerSlotsAndEdgesInGrid();

	//G.recuitSimuleGrid(tempsBest,0.99999, 100.0,0.0001, 1, 0, 3);
	//G.rerecuitSimuleGrid(tempsBest,nombreRecuit,-1,0.999999,0.999,150.0,0.000001,1,0,3);
	//G.rerecuitSimuleGrid(tempsBest,nombreRecuit,-1,0.99999,0.99,100.0,0.0001,1,0,4);
	//G.recuitSimule(tempsBest,0.99999, 100.0,0.0001, 1, 0, 3);
	//G.afficherEmplacement();
	//G.afficherLiensEmp();
	//G.recuitSimuleCustom(tempsBest,0.99999, 100.0,0.0001, 1, 0, 3, {1,15});

	//G.initGraphAndNodeScoresAndCrossings();
	//G.afficherInfo();
	//G.recuitSimuleCustom(tempsBest,0.99999,100.0,0.0001,1,0,4,{7,5});

	///ogdfReverseNonPlanar(G);

	
	//G.recuitSimuleScore(tempsBest);

	//ogdfRun(G);
	//ogdfReverseNonPlanar(G);
	//return 0;
	
	//ogdfPlacementAuPlusProche(G);

	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> secondsTotal = end - start;
	std::cout << std::fixed << secondsTotal.count() << "s for placement.\n";
	if (tempsBest != -1) std::cout << tempsBest << "s meilleur resultat.\n";
	if (bestIteration != -1) std::cout << "A la " << bestIteration << "eme iteration\n";
	if (lastIteration != -1) std::cout << "Max iteration: " << lastIteration << "\n";
	if (nombreRecuit != -1) std::cout << "Nombre de rechauffe: " << nombreRecuit << "\n";
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