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
	//allRunsSingleThread();
	//return 0;

	Graphe G;
	
	string nomFichierGraph = "graph-4-input";
	string nomFichierSlots = "4-input-slots";
	string fileGraph = chemin + "exemple/Graphe/" + nomFichierGraph + ".json";
	string fileSlots = chemin + "exemple/Slots/" + nomFichierSlots + ".json";
	readFromJsonGraph(G, fileGraph);
	readFromJsonSlots(G, fileSlots);

	auto start = std::chrono::system_clock::now();
	G.loadCopy(grapheGenetique(100,10,fileGraph,fileSlots,false,false,1));
	//G.placementAleatoire();
	//G.afficherNoeudSeul();
	//G.afficherAreteDouble();
	//G.afficherInfo();

	//ogdfRun(G);
	//return 0;
	//ogdfReverse(G);
	//G.placementAleatoire();
	//G.gloutonRevisite();
	//G.recuitSimule();
	//G.gloutonRevisite();
	//ogdfPlacementAuPlusProche(G);
	//G.generateMoreEmplacement(10);

	//G.gloutonRevisiteVoisin();
	//G.afficherNoeuds();
	//G.afficherEmplacement();
	//G.afficherLiens();
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> secondsTotal = end - start;
	std::cout << secondsTotal.count() << "s for placement.\n";
	std::cout << "Nombre intersection apres placement: " << G.getNbCroisement() << std::endl;
	std::cout << "Setup complete!" << std::endl;

	// OpenGL
	bool useOpenGL = true;
	srand(static_cast<unsigned int>(time(NULL)));
	if (useOpenGL) {
		int maxX = G.gridWidth, maxY = G.gridHeight;
		std::cout << "Grid: " << G.gridWidth << " " << G.gridHeight << std::endl;
		dispOpenGL(G, G.gridWidth+1, G.gridHeight+1, maxX, maxY);
	}
	return 0;
}