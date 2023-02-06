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


// Applique un algorithme genetique sur une population, la moitié de la population est remplacée à chaque génération en croisant 2 parents appartenant aux 50% des meilleurs.
// useRecuit indique si on applique un court recuit simulé à chaque nouvel enfant créé
// useRand indique si l'on doit utiliser l'algorithme aléatoire plutôt que le glouton pour le replacage des noeuds lors du croisement
// modeCroisement indique quel algorithme de croisement utiliser 0=Voisinage, 1=HalfParent, 2=Aléatoire, 3=VoisinageV2
std::vector<int> grapheGenetique(int population, int maxIteration, const std::string& nomGraphe, const std::string& nomSlot, bool useRecuit=false, bool useRand=false, int modeCroisement=0) {
	std::vector<Graphe> graphes;
	graphes.resize(population);
	readFromJsonGraph(graphes[0], nomGraphe);
	readFromJsonSlots(graphes[0], nomSlot);
	for (int i = 1; i < population; ++i) {
		graphes[i].nomGraphe = "Graphe" + std::to_string(i);
		graphes[i].copyFromGraphe(graphes[0]);
		graphes[i].placementAleatoire();
		if (modeCroisement != 3) { graphes[i].getNbCroisement(); }
		else { graphes[i].initGraphAndNodeScoresAndCrossings(); }
	}
	graphes[0].nomGraphe = "Graphe0";
	graphes[0].placementAleatoire();
	if (modeCroisement != 3) { graphes[0].getNbCroisement(); }
	else { graphes[0].initGraphAndNodeScoresAndCrossings(); }
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
	for (int i = 0; i<10;i++) {
		std::cout << graphes[i].nombreCroisement << "&" << graphes[i].getNbCroisementConst() << " ";
	}
	std::cout << "]" << std::endl;
	for (int i = 0; i<graphes.size();i++) {
		graphes[i].debugEverything();
	}
	std::cout << "Debut Croisement Genetique." << std::endl;
	bool noChange = false;
	while (currentIteration < maxIteration && bestCrossingResult>0 && !noChange) {
		int numberOfNoChange = 0;
		for (int i = population/2; i < population; ++i) {
			graphes[i].clearNodeEmplacement();
			int grapheID1, grapheID2;
			grapheID1 = generateRand(population/2 - 1);
			grapheID2 = generateRand(population/2 - 1);
			while (grapheID2 == grapheID1) {
				grapheID2 = generateRand(population/2 - 1);
			}
			bool result;
			if (modeCroisement == 0) {
				result = graphes[i].croisementVoisinageFrom(graphes[grapheID1], graphes[grapheID2], useRand);
			}
			else if (modeCroisement == 1) {
				result = graphes[i].croisementHalfParent(graphes[grapheID1], graphes[grapheID2], sortedEmpId, useRand);
			}
			else if (modeCroisement == 2) {
				result = graphes[i].croisementAleatoire(graphes[grapheID1], graphes[grapheID2], useRand);
			}
			else if (modeCroisement == 3) {
				result = graphes[i].croisementVoisinageScore(graphes[grapheID1], graphes[grapheID2], useRand);
			}
			else if (modeCroisement == 4) {
				result = graphes[i].croisementBestOfBoth(graphes[grapheID1], graphes[grapheID2], useRand);
			}
			if (!result) {
				numberOfNoChange++;
			}
			if (useRecuit) { // Le recuit met le nombre de croisement à jour.
				graphes[i].recuitSimule(0.99, 100.0);
			}
			else if (!graphes[i].isNombreCroisementUpdated){ // Si le nombre de croisement n'est pas à jour, on le recalcule.
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
		for (int i = 0; i<10;i++) {
			std::cout << graphes[i].nombreCroisement << "&" << graphes[i].getNbCroisementConst() << " ";
		}
		std::cout << "]" << std::endl;
		for (int i = 0; i<graphes.size();i++) {
			graphes[i].debugEverything();
		}
	}

	return graphes[0].saveCopy();
}




#endif