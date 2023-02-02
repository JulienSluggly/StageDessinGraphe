#ifndef UTILITAIRE_HPP
#define UTILIRAIRE_HPP

#include <vector>
#include <iostream>

int findBestGraphe(std::vector<Graphe>& graphes)
{
	int meilleurID = 0;
	int nbRencontre = 0;
	int currentNbCroisement, meilleurNbCroisement = graphes[0].getNbCroisement();
	for (int i = 1; i < graphes.size(); ++i)
	{
		currentNbCroisement = graphes[i].getNbCroisement();
		if (currentNbCroisement < meilleurNbCroisement)
		{
			meilleurID = i;
			meilleurNbCroisement = currentNbCroisement;
			nbRencontre = 0;
		}
		else if (currentNbCroisement == meilleurNbCroisement)
		{
			++nbRencontre;
			int aleatoire = generateRand(nbRencontre);
			if (aleatoire == 1)
			{
				meilleurID = i;
				meilleurNbCroisement = currentNbCroisement;
			}
		}
	}
	return meilleurID;
}

double calculMoyenneGraphes(std::vector<Graphe>& graphes)
{
	double moyenne = 0;
	for (Graphe graphe : graphes)
	{
		moyenne += graphe.getNbCroisement();
	}
	return moyenne / graphes.size();
}

int perfectGraphe(std::vector<Graphe>& graphes)
{
	for (int i = 0; i < graphes.size(); ++i)
	{
		if (graphes[i].getNbCroisement() == 0)
		{
			return i;
		}
	}
	return -1;
}



std::vector<int> grapheGenetique(int population, int maxIteration, const std::string& nomGraphe, const std::string& nomSlot, bool useRecuit=false, bool useRand=false, int modeCroisement=0) {
	std::vector<Graphe> graphes;
	graphes.resize(population);
	readFromJsonGraph(graphes[0], nomGraphe);
	readFromJsonSlots(graphes[0], nomSlot);
	for (int i = 1; i < population; ++i) {
		graphes[i].copyFromGraphe(graphes[0]);
		graphes[i].placementAleatoire();
		graphes[i].getNbCroisement();
	}
	graphes[0].placementAleatoire();
	graphes[0].getNbCroisement();
	// Vecteur pour le mode half parent
	std::vector<int> sortedEmpId;
	if (modeCroisement == 1) {
		std::vector<Emplacement*> empPtrVec;
		empPtrVec.resize(graphes[0]._emplacementsPossibles.size());
		for (int i = 0; i < graphes[0]._emplacementsPossibles.size(); i++) {
			empPtrVec[i] = &graphes[0]._emplacementsPossibles[i];
		}
		// Tri des emplacements pas la coord X
		std::sort(empPtrVec.begin(), empPtrVec.end(), comparePtrEmplacement);
		sortedEmpId.resize(empPtrVec.size());
		for (int i=0;i<empPtrVec.size();i++) {
			sortedEmpId[i] = empPtrVec[i]->getId();
		}
	}
	sort(graphes.begin(), graphes.end());
	int currentIteration = 0;
	long bestCrossingResult = graphes[0].nombreCroisement;
	std::cout << bestCrossingResult << " Meilleur debut genetique\n";
	std::cout << "[";
		for (int i = 0; i<4;i++) {
			std::cout << graphes[i].nombreCroisement << " ";
		}
		std::cout << "]" << std::endl;
	bool noChange = false;
	while (currentIteration < maxIteration && bestCrossingResult>0 && !noChange) {
		int numberOfNoChange = 0;
		for (int i = population/2; i < population; ++i) {
			graphes[i].clearNodeEmplacement();
			//std::cout << "Enfant: " << i << std::endl;
			int grapheID1, grapheID2;
			grapheID1 = generateRand(population/2 - 1);
			grapheID2 = generateRand(population/2 - 1);
			while (grapheID2 == grapheID1) {
				grapheID2 = generateRand(population/2 - 1);
			}
			//std::cout << "P1: " << grapheID1 << " P2: " << grapheID2 << std::endl;
			bool result;
			if (modeCroisement == 0) {
				result = graphes[i].croisementVoisinageFrom(graphes[grapheID1], graphes[grapheID2], useRand);
			}
			else if (modeCroisement == 1) {
				graphes[i].croisementHalfParent(graphes[grapheID1], graphes[grapheID2], sortedEmpId, useRand);
			}
			if (!result) {
				numberOfNoChange++;
			}
			if (useRecuit) {
				graphes[i].recuitSimule(0.99, 100.0);
			}
			else {
				graphes[i].getNbCroisement();
			}
			if (graphes[i].nombreCroisement < bestCrossingResult) {
				bestCrossingResult = graphes[i].nombreCroisement;
			}
		}
		if (numberOfNoChange >= (population / 2)-1) {
			noChange = true;
		}
		++currentIteration;
		sort(graphes.begin(), graphes.end());
		std::cout << "Iteration: " << currentIteration << " Meilleur graphe : " << bestCrossingResult << " \n";
		std::cout << "[";
		for (int i = 0; i<4;i++) {
			std::cout << graphes[i].nombreCroisement << " ";
		}
		std::cout << "]" << std::endl;

	}

	return graphes[0].saveCopy();
}




#endif