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
#include "stressMaj.hpp"

void printDebugData(Graphe& G, double tempsBest, int bestIteration, int lastIteration, int nombreRecuit, std::chrono::time_point<std::chrono::system_clock> start) {
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> secondsTotal = end - start;
	std::cout << std::fixed << secondsTotal.count() << "s for placement.\n";
	if (tempsBest != -1) std::cout << tempsBest << "s meilleur resultat.\n";
	if (bestIteration != -1) std::cout << "A la " << bestIteration << "eme iteration\n";
	if (lastIteration != -1) std::cout << "Max iteration: " << lastIteration << "\n";
	if (nombreRecuit != 0) std::cout << "Nombre de rechauffe: " << nombreRecuit << "\n";
	if (G.estPlace()) { std::cout << "Nombre intersection apres placement: " << G.getNbCroisementConst() << std::endl;
		if (G.hasIllegalCrossing())  { 
			std::cout << "Solution illegale.\n";
			G.getNbCroisementDiff();
			std::cout << "Total Inter: " << G.nombreInter + G.nombreInterIll + G.nombreInterIllSelf << " normales: " << G.nombreInter << " illegales: " << G.nombreInterIll << " self: " << G.nombreInterIllSelf << std::endl;
		}
		G.debugEverything(false,false);
	}
	std::cout << "Setup complete!" << std::endl;
}

void runFuncOnAllGraphs() {
	for (int i=1;i<=12;i++) {
		std::cout << "---------------------\n";
		Graphe G;
		std::string numero = to_string(i);
		std::string nomFichierGraph = "graph-"+numero+"-input";
		std::string nomFichierSlots = numero+"-input-slots";
		std::cout << nomFichierGraph << " " << nomFichierSlots << std::endl;

		G.setupGraphe(nomFichierGraph,nomFichierSlots);
		G.DEBUG_GRAPHE = true;
		auto start = std::chrono::system_clock::now();
		double tempsBest = -1; int bestIteration = -1; int lastIteration = -1; int nombreRecuit=0;
		G.calcMaxAndAverageDegree();
		std::cout << "Max Deg: " << G.maxVoisin << " Avg: " << G.avgVoisin << std::endl;
		printDebugData(G,tempsBest,bestIteration,lastIteration,nombreRecuit,start);
	}
}

int main() {
	//initRandomSeed();
	initSameSeed(1462039345);
	//allRunsSingleThread(); return 0;
	//allRunsBySlots(); allRunsBySlotsSecondRun(); return 0;

	std::string nomFichierGraph = "graph-11-input";
	std::string nomFichierSlots = "3X-11-input-slots";
	//std::string nomFichierSlots = "Grid";
	std::cout << nomFichierGraph << " " << nomFichierSlots << std::endl;

	Graphe G(nomFichierGraph);
	G.setupGraphe(nomFichierGraph,nomFichierSlots);
	///G.readFromJsonOldGraph(chemin + "automatique/auto21-11.json");
	//ogdfReverse(G);

	std::cout << "Debut placement.\n";
	G.DEBUG_GRAPHE = true;
	auto start = std::chrono::system_clock::now();
	double tempsBest = -1; int bestIteration = -1; int lastIteration = -1; int nombreRecuit=0; 
	//G.grapheGenetique(tempsBest,bestIteration,lastIteration,100,1000,fileGraph,fileSlots,true,false,3);
	//G.grapheGenetique(tempsBest,bestIteration,lastIteration,300,1000,nomFichierGraph,nomFichierSlots,false,false,6);
	//std::cout << nombreIterationRecuit(150.0,0.999999,0.000001) << std::endl;

	G.placementAleatoire();
	//G.stepStressMajorization(); G.deleteGrille();
	//G.stressMajorization();
	//G.gloutonRevisiteGrid();
	//G.gloutonRevisite();

	//G.initGrilleCarre();
	//G.gloutonRevisiteGrid();

	//G.triangulationDelaunay();
	//ogdfPivotMDS(G);
	//ogdfRun(G);
	//ogdfStressMinimization(G);
	//ogdfOther(G);

	//ogdfReverseNonPlanar(G);

	//G.recuitSimule(tempsBest);
	//G.rerecuitSimule(tempsBest,nombreRecuit);
	//G.rerecuitSimule(tempsBest,nombreRecuit,{},-1,0.999999);

	//G.afficherInfo();
	printDebugData(G,tempsBest,bestIteration,lastIteration,nombreRecuit,start);
	
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