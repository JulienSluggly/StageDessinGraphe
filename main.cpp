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
	Graphe g1;
	Graphe g2;
	Graphe g3;
	std::string g1file = chemin + "benchGraphs/runsDone/graph1recuit.json";
	std::string g2file = chemin + "benchGraphs/runsDone/graph2recuit.json";
	std::string g3file = chemin + "benchGraphs/runsDone/graph3recuit.json";
	g1.readFromJsonGraphReel(g1file);
	g2.readFromJsonGraphReel(g2file);
	g3.readFromJsonGraphReel(g3file);
	std::ofstream resultats("distanceRecuit.csv", std::ios_base::app);
	double maxXg1=0, maxYg1=0, maxXg2=0, maxYg2=0, maxXg3=0, maxYg3=0;
	double x1g1, x2g1, y1g1, y2g1;
	double x1g2, x2g2, y1g2, y2g2;
	double x1g3, x2g3, y1g3, y2g3;
	for (int i=0;i<g1._noeuds.size();i++) {
		if (g1._noeuds[i]._xreel > maxXg1) { maxXg1 = g1._noeuds[i]._xreel; }
		if (g1._noeuds[i]._yreel > maxYg1) { maxYg1 = g1._noeuds[i]._yreel; }
		if (g2._noeuds[i]._xreel > maxXg2) { maxXg2 = g2._noeuds[i]._xreel; }
		if (g2._noeuds[i]._yreel > maxYg2) { maxYg2 = g2._noeuds[i]._yreel; }
		if (g3._noeuds[i]._xreel > maxXg3) { maxXg3 = g3._noeuds[i]._xreel; }
		if (g3._noeuds[i]._yreel > maxYg3) { maxYg3 = g3._noeuds[i]._yreel; }
	}
	x1g1 = maxXg1/3.0;
	x2g1 = (maxXg1/3.0)*2.0;
	x1g2 = maxXg2/3.0;
	x2g2 = (maxXg2/3.0)*2.0;
	x1g3 = maxXg3/3.0;
	x2g3 = (maxXg3/3.0)*2.0;
	y1g1 = maxYg1/3.0;
	y2g1 = (maxYg1/3.0)*2.0;
	y1g2 = maxYg2/3.0;
	y2g2 = (maxYg2/3.0)*2.0;
	y1g3 = maxYg3/3.0;
	y2g3 = (maxYg3/3.0)*2.0;

	double lx1, lx2, ly1, ly2;
	Noeud* n;
	for (int i=0;i<g1._noeuds.size();i++) {
		for (int nbG=0;nbG<3;nbG++) {
			if (nbG==0) { n = &g1._noeuds[i]; lx1 = x1g1; lx2 = x2g1; ly1 = y1g1; ly2 = y2g1; }
			else if (nbG==1) { n = &g2._noeuds[i]; lx1 = x1g2; lx2 = x2g2; ly1 = y1g2; ly2 = y2g2; }
			else if (nbG==2) { n = &g3._noeuds[i]; lx1 = x1g3; lx2 = x2g3; ly1 = y1g3; ly2 = y2g3; }

			double xn = n->_xreel;
			double yn = n->_yreel;
			std::string position;
			if ((xn<=lx1)&&(yn<=ly1)) { position = "LL"; }
			else if ((xn<=lx2)&&(yn<=ly1)) { position = "ML"; }
			else if (yn<=ly1) { position = "RL"; }
			else if ((xn<=lx1)&&(yn<=ly2)) { position = "LM"; }
			else if ((xn<=lx2)&&(yn<=ly2)) { position = "MM"; }
			else if (yn<=ly2) { position = "RM"; }
			else if (xn<=lx1) { position = "LT"; }
			else if (xn<=lx2) { position = "MT"; }
			else { position = "RT"; }

			resultats << position;
			if (nbG !=2) { resultats << ","; }
			else { resultats << std::endl; }

		}
	}
	resultats.close();
}

int main(int argc, char *argv[]) {
	bool useProfiler = false;
#if defined(GPERF_INSTALLED)
	std::string cheminProfile = chemin + "profilerData/profile.output";
	if (useProfiler) { ProfilerStart(cheminProfile.c_str()); }
#endif
	if (argc > 2) { initCPUSet(std::stoi(argv[2])); }
	else { initCPUSet(); }
	//initRandomSeed();
	initSameSeed();
	//if (argc > 2) { allRunsByOnFolderSingleInput(argv[1],std::stoi(argv[2])); } else { allRunsByOnFolderSingleInput(argv[1]); } return 0;
	bool useCoordReel = true;
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
	//G.grapheGenetiqueV2(tempsBest,bestIteration,lastIteration,3,1800,nomFichierGraph,nomFichierSlots); 
	G.grapheGenetiqueReel(tempsBest,bestIteration,lastIteration,100,500,nomFichierGraph);
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