#ifndef LOGCSV_HPP
#define LOGCSV_HPP

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include "ogdfFunctions.hpp"
#include <omp.h>
#include "genetique.hpp"

void generateCSV(int nbEssay, const std::string& methodeName, const std::string& methodeAlgoName, const std::string& nomGraphe, Graphe& G, std::string fileGraph="None", std::string fileSlots="None") {
	double moyenneCroisement = 0, medianCroisement;
	int meilleurCroisement = INT_MAX;
	int nbSolutionIllegale = 0;
	double tempsExecMoyenne = 0;
	std::vector<int> croisementVector(nbEssay);
	std::vector<double> tempExecVector(nbEssay);
	bool saveResult = true;
	for (int i = 0; i < nbEssay; ++i)
	{
		saveResult = true;
		std::cout << "Current Execution: " << i << "\n";
		auto start = std::chrono::system_clock::now();
		if (methodeName == "Glouton") G.glouton();
		else if (methodeName == "Glouton Revisité") G.gloutonRevisite();
		else if (methodeName == "Glouton Gravité") G.gloutonRevisiteGravite();
		else if (methodeName == "Glouton Voisin") G.gloutonRevisiteVoisin();
		else if (methodeName == "OGDF") ogdfPlacementAuPlusProche(G);
		else if (methodeName == "Aléatoire") G.placementAleatoire();
		else if (methodeName != "Aucun") {
			std::cout << "ERROR Aucune methode " << methodeName << " trouve !";
			return;
		}

		if (methodeName == "Aucun" || G.estPlace()) {

			if (methodeAlgoName == "Recuit Simulé") G.recuitSimule();
			else if (methodeAlgoName == "Recuit Simulé Delay") G.recuitSimuleDelay();
			else if (methodeAlgoName == "Recuit Simulé Tournoi Binaire") G.recuitSimuleTournoiBinaire();
			else if (methodeAlgoName == "Recuit Simulé Tournoi Multiple") G.recuitSimuleTournoiMultiple();
			else if (methodeAlgoName == "Recuit Simulé M1") G.recuitSimule(0.99999, 100, 1);
			else if (methodeAlgoName == "Recuit Simulé M2") G.recuitSimule(0.99999, 100, 2);
			else if (methodeAlgoName == "Recuit Simulé Delay M1") G.recuitSimuleDelay(0.99999, 100, 10, 1);
			else if (methodeAlgoName == "Recuit Simulé Delay M2") G.recuitSimuleDelay(0.99999, 100, 10, 2);
			else if (methodeAlgoName == "Recuit Simulé Tournoi Binaire M1") G.recuitSimuleTournoiBinaire(0.99999, 100, 1);
			else if (methodeAlgoName == "Recuit Simulé Tournoi Binaire M2") G.recuitSimuleTournoiBinaire(0.99999, 100, 2);
			else if (methodeAlgoName == "Recuit Simulé Tournoi Multiple M1") G.recuitSimuleTournoiMultiple(0.99999, 100, 1);
			else if (methodeAlgoName == "Recuit Simulé Tournoi Multiple M2") G.recuitSimuleTournoiMultiple(0.99999, 100, 2);
			else if (methodeAlgoName == "Best Deplacement") G.bestDeplacement();
			else if (methodeAlgoName == "Génétique Recuit") G.loadCopy(grapheGenetique(100, 10, fileGraph, fileSlots, true));
			else if (methodeAlgoName == "Génétique Recuit Random") G.loadCopy(grapheGenetique(100, 10, fileGraph, fileSlots, true, true));
			else if (methodeAlgoName == "Génétique No Recuit") G.loadCopy(grapheGenetique(100, 10, fileGraph, fileSlots, false));
			else if (methodeAlgoName == "Génétique No Recuit Random") G.loadCopy(grapheGenetique(100, 10, fileGraph, fileSlots, false, true));
			else if (methodeAlgoName != "Aucun") {
				std::cout << "ERROR Aucun algo " << methodeAlgoName << " trouve !";
				return;
			}


			auto end = std::chrono::system_clock::now();
			std::chrono::duration<double> secondsTotal = end - start;
			croisementVector[i] = G.getNbCroisement();
			//cout << croisementVector[i] << "\n";
			tempExecVector[i] = secondsTotal.count();
			if (G.hasIllegalCrossing()) {
				nbSolutionIllegale++;
			}
			G.clearNodeEmplacement();
		}
		else {
			saveResult = false;
		}
	}
	if (saveResult) {
		for (int croisement : croisementVector)
		{
			moyenneCroisement += croisement;
			if (meilleurCroisement > croisement)
			{
				meilleurCroisement = croisement;
			}
		}
		moyenneCroisement /= nbEssay;

		for (double tempExec : tempExecVector)
		{
			tempsExecMoyenne += tempExec;
		}
		tempsExecMoyenne /= nbEssay;

		sort(croisementVector.begin(), croisementVector.end());
		if (croisementVector.size() % 2 == 1)
		{
			medianCroisement = croisementVector[croisementVector.size() / 2];
		}
		else
		{
			int nbCroisement1 = croisementVector[croisementVector.size() / 2];
			int nbCroisement2 = croisementVector[(croisementVector.size() / 2) - 1];

			medianCroisement = (nbCroisement1 + nbCroisement2) / 2.0;
		}

		string nomFichier = "./resultats/" + nomGraphe + ".csv";
		std::ofstream resultats(nomFichier, std::ios_base::app);

		resultats << std::fixed;
		resultats << methodeName << ","
			<< methodeAlgoName << ","
			<< nbEssay << ","
			<< G._noeuds.size() << ","
			<< G._liens.size() << ","
			<< G._emplacementsPossibles.size() << ","
			<< nbSolutionIllegale << ","
			<< meilleurCroisement << ",";
		if (moyenneCroisement > 100) { resultats << std::setprecision(0) << moyenneCroisement << ","; }
		else { resultats << std::setprecision(1) << moyenneCroisement << ","; }
		if (medianCroisement > 100) { resultats << std::setprecision(0) << medianCroisement << ","; }
		else { resultats << std::setprecision(1) << medianCroisement << ","; }
		resultats << std::setprecision(7) << tempsExecMoyenne << "\n";
		resultats.close();
	}
}


#endif
