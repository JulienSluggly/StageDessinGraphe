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

void recuitSimuleCurrentFolder(int argc, char** argv) {
	std::string cheminFichier;
	std::string nomFichier;
	std::string nomGraphe;

	if (argc == 1) {
		std::filesystem::path graphFolder = std::filesystem::current_path() / "originalGraph";
		for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(graphFolder)) {
			cheminFichier = dirEntry.path().string();
			nomFichier = dirEntry.path().string();
			std::reverse(nomFichier.begin(), nomFichier.end());
			nomFichier = nomFichier.substr(0, nomFichier.find('/'));
			std::reverse(nomFichier.begin(), nomFichier.end());

			if (containsString(nomFichier,".json")) {
				nomGraphe = nomFichier.substr(0, nomFichier.find('.'));
				break;
			}
		}
	}
	else {
		cheminFichier = argv[1];
	}

	if (argc < 3) {
		initRandomSeed();
	}
	else {
		std::string seed = argv[2];
		initSameSeed(stoi(seed));
	}
	Graphe G(nomGraphe);
	G.readFromJsonChallenge(cheminFichier);
	G.calcMaxAndAverageDegree();
	G.fillCommonNodeVectors();
	std::vector<int> graphCopy;
	long nbCroisementPlacement = G.TRES_GRANDE_VALEUR;
	if (G.estPlace()) { 
		graphCopy = G.saveCopy();
		nbCroisementPlacement = G.getNbCroisement();
		tcout() << "Le fichier de depart contient un placement avec " << nbCroisementPlacement << " croisements.\n";
	}
	G.placementFMME(true);
	long nbCroisementFMME = G.getNbCroisement();
	if (nbCroisementFMME > nbCroisementPlacement) {
		tcout() << "Nombre de croisements apres placement FMME: " << nbCroisementFMME << std::endl;
		tcout() << "Le placement de depart est conserve car meilleur que le FMME.\n";
		G.loadCopy(graphCopy);
		G.nombreCroisement = nbCroisementPlacement;
	}
	else {
		tcout() << "Nombre de croisements apres placement FMME: " << nbCroisementFMME << std::endl;
	}
	G.setupGridAndRegistration({});
	//G.rerecuitSimuleChallenge();
	G.bestDeplacementLimite();
	std::filesystem::path resultFolder = std::filesystem::current_path() / "resultats";
	std::string outputNameFinal = resultFolder.string() + "/" + to_string(G.nombreCroisement) + "-" + nomGraphe + "Final.json";
	G.writeToJsonChallenge(outputNameFinal);
	exit(0);
}

void debugCurrentFolder(int argc, char** argv) {
	std::string cheminFichier;
	std::string nomFichier;
	std::string nomGraphe;

	if (argc == 1) {
		std::filesystem::path graphFolder = std::filesystem::current_path() / "originalGraph";
		std::cout << graphFolder.string();
		for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(graphFolder)) {
			cheminFichier = dirEntry.path().string();
			nomFichier = dirEntry.path().string();
			std::reverse(nomFichier.begin(), nomFichier.end());
			nomFichier = nomFichier.substr(0, nomFichier.find('/'));
			std::reverse(nomFichier.begin(), nomFichier.end());

			if (containsString(nomFichier,".json")) {
				nomGraphe = nomFichier.substr(0, nomFichier.find('.'));
				break;
			}
		}
	}
	else {
		cheminFichier = argv[1];
	}

	if (argc < 3) {
		initRandomSeed();
	}
	else {
		std::string seed = argv[2];
		initSameSeed(stoi(seed));
	}
	Graphe G(nomGraphe);
	G.readFromJsonChallenge(cheminFichier);
	G.calcMaxAndAverageDegree();
	G.fillCommonNodeVectors();
	G.debugEverything(false,true);
	exit(0);
}

int main(int argc, char *argv[]) {
	recuitSimuleCurrentFolder(argc, argv); return 0;
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