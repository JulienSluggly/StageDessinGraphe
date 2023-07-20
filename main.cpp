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
#include "arg.h"

void initCPUSet(int core=0) {
#if defined(LINUX_OS)
    CPU_ZERO(&cpuset);
    CPU_SET(core, &cpuset);
	sched_setaffinity(0, sizeof(cpuset), &cpuset);
#endif
}

void recuitSimuleCurrentFolder(std::string pathFile, int seed, int core) {
	std::string cheminFichier;
	std::string nomFichier;
	std::string nomGraphe;

	if (pathFile == "") {
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
	else { cheminFichier = pathFile; }

	if (seed == -1) { initRandomSeed(); }
	else { initSameSeed(seed); }

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
	if (core != -1) { initCPUSet(core); }

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
	G.rerecuitSimuleChallenge();
	G.bestDeplacementLimite();
	std::filesystem::path resultFolder = std::filesystem::current_path() / "resultats";
	std::string outputNameFinal = resultFolder.string() + "/" + to_string(G.nombreCroisement) + "-" + nomGraphe + "Final.json";
	G.writeToJsonChallenge(outputNameFinal);
	exit(0);
}

void debugCurrentFolder(std::string pathFile) {
	std::string cheminFichier;
	std::string nomFichier;
	std::string nomGraphe;

	if (pathFile == "") {
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
		cheminFichier = pathFile;
	}

	Graphe G(nomGraphe);
	G.readFromJsonChallenge(cheminFichier);
	G.calcMaxAndAverageDegree();
	G.fillCommonNodeVectors();
	G.debugEverything(false,true);
	exit(0);
}

int main(int argc, char *argv[]) {
	argStr pathFileArg = {"", "-p","Path of .json graph file."};
	argInt seedArg = {-1, "-s","Unsigned Int value."};
	argInt coreArg = {-1, "-c","Int value between 0 and number of cpu cores."};

	char pathFile[100] = "";
	int seed;
	int core;

	giveArgStr(pathFile,pathFileArg,argc,argv);
	giveArgInt(&seed,seedArg,argc,argv);   
	giveArgInt(&core,coreArg,argc,argv); 

	recuitSimuleCurrentFolder((std::string)pathFile, seed,core); return 0;
	//debugCurrentFolder(pathFile); return 0;
}