#include <string>
#include <stdio.h>
#include <random>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
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

void runFuncOnFolder() {
	std::string path = chemin + "benchGraphs/dimacsClean/";
	for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(path)) {
		std::cout << "---------------------\n";
		Graphe G;
		G.readFromJsonGraph(dirEntry.path());
		G.generateGrid();
		std::cout << dirEntry.path() << " Grid" << std::endl;
		G.DEBUG_GRAPHE = true;
		auto start = std::chrono::system_clock::now();
		double tempsBest = -1; int bestIteration = -1; int lastIteration = -1; int nombreRecuit=0;
		std::cout << "Noeud: " << G._noeuds.size() << " Arete: " << G._aretes.size() << std::endl;
		printDebugData(G,tempsBest,bestIteration,lastIteration,nombreRecuit,start);
	}
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
		ogdfOther(G);
		printDebugData(G,tempsBest,bestIteration,lastIteration,nombreRecuit,start);
	}
}

void runFuncOnAllGraphsAllSlots(bool useGrid=true) {
	int nbSlot = 4;
	if (!useGrid) { nbSlot--; }
	for (int i=5;i<=12;i++) {
		for (int j=0;j<nbSlot;j++) {
			std::cout << "---------------------\n";
			Graphe G;
			std::string numero = to_string(i);
			std::string nomFichierGraph = "graph-"+numero+"-input";
			std::string nomFichierSlots;
			if (j == 0) { nomFichierSlots = numero+"-input-slots"; }
			else if (j < 3) { nomFichierSlots = to_string(j+1) + "X-"+numero+"-input-slots"; }
			else { nomFichierSlots = "Grid"; }
			std::cout << nomFichierGraph << " " << nomFichierSlots << std::endl;

			G.setupGraphe(nomFichierGraph,nomFichierSlots);
			G.DEBUG_GRAPHE = true;
			auto start = std::chrono::system_clock::now();
			double tempsBest = -1; int bestIteration = -1; int lastIteration = -1; int nombreRecuit=0;
			std::cout << "Moyenne: " << G.distMoyNTirage(1000000) << std::endl;
			printDebugData(G,tempsBest,bestIteration,lastIteration,nombreRecuit,start);
		}
	}
}

int main() {
	initRandomSeed();
	//runFuncOnAllGraphsAllSlots(); return 0;
	//initSameSeed();
	//runFuncOnAllGraphs(); return 0;
	//runFuncOnFolder(); return 0;
	// allRunsBySlotsSecondRun(); testRomeGraphs(); return 0;

	std::string nomFichierGraph = "graph-10-input";
	std::string nomFichierSlots = "10-input-slots";
	//std::string nomFichierSlots = "Grid";
	std::cout << nomFichierGraph << " " << nomFichierSlots << std::endl;

	Graphe G(nomFichierGraph);
	//G.setupGraphe(nomFichierGraph,nomFichierSlots);
	//G.readFromJsonOldGraph(chemin + "automatique/auto21-10.json"); G.generateGrid(G._noeuds.size()/2,G._noeuds.size()/2);
	//ogdfReverse(G);
	std::ifstream file("/home/uha/Documents/DessinGrapheCmake/src/benchGraphs/sparceMC/1138_bus.mtx");
    if (file) {
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        ogdfReadFromMM(G, buffer);
    }
	//std::istringstream graphMM("/home/uha/Documents/DessinGrapheCmake/src/benchGraphs/sparceMC/1138_bus.mtx");
	//ogdfReadFromMM(G, graphMM);
	//G.readFromJsonGraph("/home/uha/Documents/DessinGrapheCmake/src/benchGraphs/dimacsClean/celegansneural.graphclean");
	//G.readFromDimacsGraphClean("/home/uha/Documents/DessinGrapheCmake/src/benchGraphs/dimacs/celegansneural.graph");
	G.generateGrid();
	std::cout << "Debut placement. Nombre Noeuds: " << G._noeuds.size() << " Nombre Aretes: " << G._aretes.size() << " Nombre Emplacement: " << G._emplacements.size() << " Connexe: " << G.isGrapheConnected() << std::endl;
	G.DEBUG_GRAPHE = true; //G.DEBUG_PROGRESS = true;
	auto start = std::chrono::system_clock::now();
	double tempsBest = -1; int bestIteration = -1; int lastIteration = -1; int nombreRecuit=0; 
	//G.grapheGenetique(tempsBest,bestIteration,lastIteration,100,1000,fileGraph,fileSlots,true,false,3);
	//G.grapheGenetique(tempsBest,bestIteration,lastIteration,300,1000,nomFichierGraph,nomFichierSlots,false,false,6);
	//std::cout << nombreIterationRecuit(150.0,0.999999,0.000001) << std::endl;
	//ogdfOtherTest(G);
	//ogdfFastMultipoleMultilevelEmbedder(G);
	G.stressMajorization();
	//G.stressMajorization({{}},1);
	
	//G.initGrille(); G.registerSlotsAndEdgesInGrid(); G.recuitSimule(tempsBest,start);

	//G.afficherInfo();
	printDebugData(G,tempsBest,bestIteration,lastIteration,nombreRecuit,start);
	
	// OpenGL
	bool useOpenGL = true;
	if (useOpenGL) {
		G.DEBUG_OPENGL = true;
		int maxX = G.gridWidth, maxY = G.gridHeight;
		std::cout << "Grid: " << G.gridWidth << " " << G.gridHeight << std::endl;
		dispOpenGL(G, G.gridWidth, G.gridHeight, maxX, maxY);
	}
	return 0;
}