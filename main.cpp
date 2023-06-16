#include <string>
#include <stdio.h>
#include <random>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <set>
#include <cstdlib>

#include "graphe.hpp"
#include "dispOpenGL.hpp"
#include "noDispRuns.hpp"
#include "personnel.hpp"
#include "ogdfFunctions.hpp"
#include "logCSV.hpp"
#include "utilitaire.hpp"
#include "stressMaj.hpp"
#include "solver.hpp"
#include "graphique.hpp"

#if defined(GPERF_INSTALLED)
	#include <gperftools/profiler.h>
#endif

void initCPUSet(int decallage=0) {
	omp_set_nested(1);
#if defined(LINUX_OS)
    int num_threads = ::omp_get_max_threads();
    CPU_ZERO(&cpuset);
    for (int i = 0; i < num_threads; i++) {
      CPU_SET(i+decallage, &cpuset);
    }
	sched_setaffinity(0, sizeof(cpuset), &cpuset);
#endif
}

void printDebugData(Graphe& G, double tempsBest, int bestIteration, int lastIteration, int nombreRecuit, std::chrono::time_point<std::chrono::system_clock> start, std::chrono::time_point<std::chrono::system_clock> finPlacement) {
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> secondsTotal = end - start;
	std::chrono::duration<double> secondsPlacement = finPlacement - start;
	std::chrono::duration<double> secondsAlgo = end - finPlacement;
	tcout() << std::fixed << secondsPlacement.count() << "s placement. " << secondsAlgo.count() << "s algo. " << secondsTotal.count() << "s total.\n";
	if (tempsBest != -1) tcout() << tempsBest << "s meilleur resultat.\n";
	if (bestIteration != -1) tcout() << "A la " << bestIteration << "eme iteration\n";
	if (lastIteration != -1) tcout() << "Max iteration: " << lastIteration << "\n";
	if (nombreRecuit != 0) tcout() << "Nombre de rechauffe: " << nombreRecuit << "\n";
	if ((G.useCoordReel)||(G.estPlace())) { 
		tcout() << "Nombre intersection apres placement: ";
		if (!G.useCoordReel) { std::cout << G.getNbCroisementConst() << std::endl; }
		else { std::cout << G.getNbCroisementReelConst() << std::endl; }
		if (G.hasIllegalCrossing())  {
			tcout() << "Solution illegale.\n";
			if (!G.useCoordReel) { G.getNbCroisementDiff(); } else { G.getNbCroisementDiffReel(); }
			tcout() << "Total Inter: " << G.nombreInter + G.nombreInterIll + G.nombreInterIllSelf << " normales: " << G.nombreInter << " illegales: " << G.nombreInterIll << " self: " << G.nombreInterIllSelf << std::endl;
		}
		G.debugEverything(false,false);
	}
	tcout() << "Setup complete!" << std::endl;
}

void runFuncOnFolder() {
	tcout() << "---------------------\n";
	std::string path = chemin + "benchGraphs/runs/";
	long totalCross = 0;
	for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(path)) {
		Graphe G;
		G.useCoordReel = true;
		std::string graphPath = dirEntry.path().string();
		G.readFromJsonGraph(graphPath);
		G.fillCommonNodeVectors();
		//ogdfFastMultipoleMultilevelEmbedderReel(G);
		ogdfMultilevelLayout(G);
		long nbCroisement = G.getNbCroisementDiffReel();
		tcout() << nbCroisement << " " << graphPath << std::endl;
		totalCross += nbCroisement;
	}
	tcout() << "Total croisement: " << totalCross << std::endl;
}

void runFuncOnAllGraphs() {
	for (int i=1;i<=12;i++) {
		tcout() << "---------------------\n";
		Graphe G;
		std::string numero = to_string(i);
		std::string nomFichierGraph = "graph-"+numero+"-input";
		std::string nomFichierSlots = numero+"-input-slots";
		tcout() << nomFichierGraph << " " << nomFichierSlots << std::endl;

		G.setupGraphe(nomFichierGraph,nomFichierSlots);
		auto start = std::chrono::system_clock::now();
		double tempsBest = -1; int bestIteration = -1; int lastIteration = -1; int nombreRecuit=0;
		ogdfOther(G);
		auto finPlacement = std::chrono::system_clock::now();
		printDebugData(G,tempsBest,bestIteration,lastIteration,nombreRecuit,start,finPlacement);
	}
}

void runFuncOnAllGraphsAllSlots(bool useGrid=true) {
	int nbSlot = 4;
	if (!useGrid) { nbSlot--; }
	for (int i=5;i<=12;i++) {
		for (int j=0;j<nbSlot;j++) {
			tcout() << "---------------------\n";
			Graphe G;
			std::string numero = to_string(i);
			std::string nomFichierGraph = "graph-"+numero+"-input";
			std::string nomFichierSlots;
			if (j == 0) { nomFichierSlots = numero+"-input-slots"; }
			else if (j < 3) { nomFichierSlots = to_string(j+1) + "X-"+numero+"-input-slots"; }
			else { nomFichierSlots = "Grid"; }
			tcout() << nomFichierGraph << " " << nomFichierSlots << std::endl;

			G.setupGraphe(nomFichierGraph,nomFichierSlots);
			auto start = std::chrono::system_clock::now();
			double tempsBest = -1; int bestIteration = -1; int lastIteration = -1; int nombreRecuit=0;
			tcout() << "Moyenne: " << G.distMoyNTirage(1000000) << std::endl;
			auto finPlacement = std::chrono::system_clock::now();
			printDebugData(G,tempsBest,bestIteration,lastIteration,nombreRecuit,start,finPlacement);
		}
	}
}

void stopGprofProfiler(bool useProfiler) {
#if defined(GPERF_INSTALLED)
	if (useProfiler) {
		ProfilerFlush();
		ProfilerStop();
		if (std::system("/home/uha/Documents/DessinGrapheCmake/build/generate_svg.sh") == -1) { tcout() << "Could not generate svg file.\n";  }
		else { tcout() << "SVG file generated successfully.\n"; }
	}
#endif
}

void createQuickCrossData(Graphe& G, std::string nomFichierGraph) {
	ogdfFastMultipoleMultilevelEmbedderReel(G);
	G.translateGrapheToOriginReel(-1);
	G.writeToGraphEdgeList(nomFichierGraph + ".txt");
	G.writeCoordsNodeReel(nomFichierGraph + "-coords.txt");
	ogdfWriteToGraph6(G,nomFichierGraph + "-g6.txt");
}

void runFunc() {
	for (int i=40;i<70;i++) {
		Graphe G;
		G.initCompleteGraph(i);
		tcout() << "Nombre Noeuds: " << G._noeuds.size() << " Nombre Aretes: " << G._aretes.size() << std::endl;
		std::string nomGraphe = std::to_string(i) + "complet.json";
		G.writeToJsonGraph(nomGraphe);
	}
}

int main(int argc, char *argv[]) {
	//generateRandomGraphique("",200,2,100.0); return 0;
	bool useProfiler = true;
#if defined(GPERF_INSTALLED)
	std::string cheminProfile = chemin + "profilerData/profile.output";
	if (useProfiler) { ProfilerStart(cheminProfile.c_str()); }
#endif
	if (argc > 2) { initCPUSet(std::stoi(argv[2])); }
	else { initCPUSet(); }
	//initRandomSeed();
	initSameSeed();
	//allRunsByOnFolderSingleInput(argv[1]); return 0;
	bool useCoordReel = false;
	std::string nomFichierGraph = "graph-10-input";
	if (argc > 1) { nomFichierGraph = argv[1]; }
	std::string nomFichierSlots = "3X-10-input-slots";
	//std::string nomFichierSlots = "Grid";
	if (useCoordReel) { tcout() << nomFichierGraph << std::endl; }
	else { tcout() << nomFichierGraph << " " << nomFichierSlots << std::endl; }
	Graphe G(nomFichierGraph); G.useCoordReel = useCoordReel;
	std::string pathGraph = chemin + "exemple/Graphe/" + nomFichierGraph + ".json";
	G.setupGraphe(nomFichierGraph,nomFichierSlots);
	//std::string kregularFile = chemin + "benchGraphs/runsSingle/r3/grafo10781.100.graphml";
	//G.readFromGraphmlGraph(kregularFile);
	G.calcMaxAndAverageDegree();
	G.fillCommonNodeVectors();
	int nbNoeud = std::min((int)G._noeuds.size()*2,6000);
	if (useCoordReel) { tcout() << "Coordonnees flottantes, pas d'emplacements.\n"; }
	else { tcout() << "Coordonnees entieres, utilisation d'emplacements\n"; }
	tcout() << "Nombre Noeuds: " << G._noeuds.size() << " Nombre Aretes: " << G._aretes.size() << " Nombre Emplacement: " << G._emplacements.size() << " Nombre Cellules: " << (int)ceil(sqrt(G._aretes.size())*1.5)*(int)ceil(sqrt(G._aretes.size())*1.5) << " Connexe: " << G.isGrapheConnected() << " Max Degre: " << G.maxVoisin << " Average Degre: " << G.avgVoisin << std::endl;
	tcout() << "Debut placement.\n";
	auto start = std::chrono::system_clock::now();
	double tempsBest = -1; int bestIteration = -1; int lastIteration = -1; int nombreRecuit=0;
	G.grapheGenetiqueV2(tempsBest,bestIteration,lastIteration,10,3,nomFichierGraph,nomFichierSlots); 
	//G.placementFMME();
	//G.stressMajorizationReel();
#if defined(LINUX_OS)
	sched_setaffinity(0, sizeof(cpuset), &cpuset);
#endif
	if (useCoordReel) { G.translateGrapheToOriginReel(-1); }
	G.setupGridAndRegistration({});
	tcout() << "Fin du placement.\n";
	auto finPlacement = std::chrono::system_clock::now();
	//G.rerecuitSimule(tempsBest,nombreRecuit,start,{{15,1200}},-1,0.99999,0.99,100.0,0.0001,1,0,2,true,false,-1,true,true);
	//G.rerecuitSimule(tempsBest,nombreRecuit,start,{},-1,0.99999,0.99,100.0,0.0001,1,0,2,true,false,-1,true,false);
	//G.reinitGrille();
	//G.rechercheTabou();
	//G.triangulationDelaunay();
	//G.recuitSimule(tempsBest,start,{},0.99999,100.0,0.0001,1,0,2,true);
	//G.recuitSimuleReel(tempsBest,start,{},0.99999,100.0,0.0001,1,0,2,true);
	//G.recuitSimuleReelLimite(tempsBest,start,{},0.99999,100.0,0.0001,1,0,2,true);

	//G.rerecuitSimule(tempsBest,nombreRecuit,start,{},-1,0.99999,0.99,100.0,0.0001,1,0,2,true);
	//G.recuitSimuleChallenge();
	//G.rerecuitSimuleChallenge();
	//G.recuitSimule(tempsBest,start,{});
	//G.recuitSimuleReel(tempsBest,start,{},0.99999,100.0,0.0001,1,0,2,true,false,500);
	//G.rerecuitSimuleReel(tempsBest,nombreRecuit,start,{},-1,0.99999,0.99,100.0,0.0001,1,0,2,true,false,500);
	//G.recuitSimuleReel(tempsBest,start,{},0.99999,100.0,0.0001,1,0,2,true,false,500);
	//G.afficherInfo();

	//G.rerecuitSimuleReel(tempsBest,nombreRecuit,start,{{15,7200}},-1,0.99999,0.99,100.0,0.0001,1,0,2,true,false,300,true,true);

	stopGprofProfiler(useProfiler);
	printDebugData(G,tempsBest,bestIteration,lastIteration,nombreRecuit,start,finPlacement);
	#ifdef OPENGL_INSTALLED
	bool useOpenGL = true;
	if (useOpenGL) {
		tcout() << "Grid: " << G.gridWidth << " " << G.gridHeight << std::endl;
		dispOpenGL(G,G.gridWidth,G.gridHeight,useCoordReel);
	}
	#endif

	return 0;
}