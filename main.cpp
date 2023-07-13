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
#include "noDispRuns.hpp"
#include "personnel.hpp"
#include "ogdfFunctions.hpp"
#include "utilitaire.hpp"
#include "stressMaj.hpp"

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
		long nbInter;
		nbInter = G.getNbCroisementConst();
		tcout() << "Nombre intersection apres placement: " << nbInter << std::endl;
		if (G.hasIllegalCrossing())  {
			tcout() << "Solution illegale.\n";
			G.getNbCroisementDiff();
			tcout() << "Total Inter: " << G.nombreInter + G.nombreInterIll + G.nombreInterIllSelf << " normales: " << G.nombreInter << " illegales: " << G.nombreInterIll << " self: " << G.nombreInterIllSelf << std::endl;
		}
		G.debugEverything(false,false);
	}
	tcout() << "Setup complete!" << std::endl;
}

int main(int argc, char *argv[]) {
	initRandomSeed();
	bool useCoordReel = false;
	std::string nomFichierGraph = "graph-10-input";
	if (argc > 1) { nomFichierGraph = argv[1]; }
	std::string nomFichierSlots = "3X-10-input-slots";
	tcout() << "Fichier Graphe: " + nomFichierGraph << " Fichier Slots: " << nomFichierSlots << std::endl;
	Graphe G(nomFichierGraph); G.useCoordReel = useCoordReel;
	G.setupGraphe(nomFichierGraph,nomFichierSlots);
	G.calcMaxAndAverageDegree();
	G.fillCommonNodeVectors();
	tcout() << "Coordonnees entieres, utilisation d'emplacements\n";
	tcout() << "Nombre Noeuds: " << G._noeuds.size() << " Nombre Aretes: " << G._aretes.size() << " Nombre Emplacement: " << G._emplacements.size() << " Nombre Cellules: " << (int)ceil(sqrt(G._aretes.size())*1.5)*(int)ceil(sqrt(G._aretes.size())*1.5) << " Connexe: " << G.isGrapheConnected() << " Max Degre: " << G.maxVoisin << " Average Degre: " << G.avgVoisin << " Penalite: " << G.PENALITE_MAX << std::endl;
	tcout() << "Debut placement.\n";
	auto start = std::chrono::system_clock::now();
	double tempsBest = -1; int bestIteration = -1; int lastIteration = -1; int nombreRecuit=0;
	G.placementFMME();
#if defined(LINUX_OS)
	sched_setaffinity(0, sizeof(cpuset), &cpuset);
#endif
	if (G.estPlace()) {
		G.setupGridAndRegistration({});
		tcout() << "Fin du placement.\n";
		auto finPlacement = std::chrono::system_clock::now();
		G.rerecuitSimuleChallenge();
		printDebugData(G,tempsBest,bestIteration,lastIteration,nombreRecuit,start,finPlacement);
	}
	return 0;
}