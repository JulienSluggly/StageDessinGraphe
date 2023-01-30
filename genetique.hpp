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



std::vector<int> grapheGenetique(int population, int maxIteration, const std::string& nomGraphe, const std::string& nomSlot, bool useRecuit=false, bool useRand=false) {
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
	int indexBestGraphe = -1;
	int currentIteration = 0;
	long bestCrossingResult = 99999999;
	bool noChange = false;
	while (currentIteration < maxIteration && bestCrossingResult>0 && !noChange) {
		sort(graphes.begin(), graphes.end());
		indexBestGraphe = 0;
		int numberOfNoChange = 0;
		for (int i = population/2; i < population; ++i) {
			graphes[i].clearNodeEmplacement();
			//std::cout << "Enfant: " << i << std::endl;
			int grapheID1, grapheID2;
			grapheID1 = generateRand(population/2 - 1);
			grapheID2 = generateRand(population/2 - 1);
			//std::cout << "P1: " << grapheID1 << " P2: " << grapheID2 << std::endl;
			if (!graphes[i].croisementVoisinageFrom(graphes[grapheID1], graphes[grapheID2], useRand)) {
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
				indexBestGraphe = i;
			}
		}
		if (numberOfNoChange == (population / 2)-1) {
			noChange = true;
		}
		++currentIteration;

		std::cout << "Iteration: " << currentIteration << " Meilleur graphe : " << bestCrossingResult << " \n";

	}

	return graphes[indexBestGraphe].saveCopy();
}




#endif