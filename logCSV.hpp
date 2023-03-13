#ifndef LOGCSV_HPP
#define LOGCSV_HPP

#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
//#include "ogdfFunctions.hpp"
#include "personnel.hpp"
#include <omp.h>
#include <climits>
#include <ctime>
#include <algorithm>

std::unordered_map<std::string,std::pair<int,int>> mapGraphPopGen;

void fillMap() {
	mapGraphPopGen["graph-1-input"] = std::pair<int,int>(1000,1000);
	mapGraphPopGen["graph-2-input"] = std::pair<int,int>(1000,1000);
	mapGraphPopGen["graph-3-input"] = std::pair<int,int>(1000,1000);
	mapGraphPopGen["graph-4-input"] = std::pair<int,int>(1000,1000);
	mapGraphPopGen["graph-5-input"] = std::pair<int,int>(1000,200);
	mapGraphPopGen["graph-6-input"] = std::pair<int,int>(500,100);
	mapGraphPopGen["graph-7-input"] = std::pair<int,int>(750,100);
	mapGraphPopGen["graph-8-input"] = std::pair<int,int>(70,100);
	mapGraphPopGen["graph-9-input"] = std::pair<int,int>(70,100);
	mapGraphPopGen["graph-10-input"] = std::pair<int,int>(20,50);
	mapGraphPopGen["graph-11-input"] = std::pair<int,int>(10,30);
	mapGraphPopGen["graph-12-input"] = std::pair<int,int>(10,30);
}

void fillLogsVector() {
	fillMap();
}

std::string getParamAsString(std::vector<std::vector<double>>& customParam) {
	std::stringstream paramStream;
	paramStream << std::fixed;
	paramStream << "{";
	for (std::vector<double>& param : customParam) {
		paramStream << "{";
		if (param.size() > 0) {
			for (int i=0;i<param.size();i++) {
				if (i<=1) { paramStream << std::setprecision(0) << param[i]; }
				else { 
					if (param[i] > 0.1) { paramStream << std::setprecision(2) << param[i]; }
					else { paramStream << std::setprecision(9) << param[i]; }
				}
				if (i<param.size()-1) { paramStream << " "; }
			}
		}
		paramStream << "}";
	}
	paramStream << "}";
	return paramStream.str();
}

void generateCSV(int nbEssay, const std::string& methodePlacementName, const std::string& methodeAlgoName, const std::string& nomGraphe, std::string fileGraph, std::string fileSlots, std::vector<std::vector<double>> customParam={{}}, int tid=0) {
	bool updateScore = containsString(methodeAlgoName,"Score");
	bool isGenetique = containsString(methodeAlgoName,"Genetique");
	bool isRecuit = containsString(methodeAlgoName,"Recuit");
	bool needTriangulation = containsString(methodeAlgoName,"TRE");
	bool needGrille = containsString(methodeAlgoName,"Grille");
	double moyenneCroisement, medianCroisement;
	int meilleurCroisement = INT_MAX;
	int nbSolutionIllegale = 0, debugValue=-1;
	std::vector<int> croisementVector;
	std::vector<double> tempExecVector, tempBestVector; std::vector<int> bestIterationVector, lastIterationVector, nombreRecuitVector, totalInterVector, totalInterIllVector;
	double tempsExecMoyenne, tempsBestMoyenne, bestIterationMoyenne, lastIterationMoyenne, nombreRecuitMoyenne, totalInterMoyenne, totalInterIllMoyenne;
	bool saveResult = true;
	int population, maxIteration;
	int nombreRecuit, nombreSlots, nombreCellule;
	auto totalStart = std::chrono::system_clock::now();
	std::chrono::duration<double> secondsTotalExec = totalStart - totalStart;
	getParamAsString(customParam);
	for (int i = 1; ((((i <= nbEssay)&&(secondsTotalExec.count() < 3600))||(nbEssay==-1&&secondsTotalExec.count() < 3600))&&(i <= 100)); ++i) {
		resetSeed(tid,true);
		auto start = std::chrono::system_clock::now();
		Graphe G;
		G.setupGraphe(fileGraph,fileSlots);
		double tempsBest = -1; int bestIteration = -1; int lastIteration = -1;
		if (isGenetique) {
			population = mapGraphPopGen[nomGraphe].first;
			maxIteration = mapGraphPopGen[nomGraphe].second;
		}
		saveResult = true;
		printf("Tid: %d | Iter: %d Max: %d | %s | %s | %s | Slots: %lu | Param: %s | TotalRun: %.1fs\n",tid,i,nbEssay,nomGraphe.c_str(),methodePlacementName.c_str(),methodeAlgoName.c_str(),G._emplacements.size(),getParamAsString(customParam).c_str(),secondsTotalExec.count());
		if (methodePlacementName == "Glouton") G.glouton();
		else if (methodePlacementName == "Glouton Revisite") G.gloutonRevisite();
		else if (methodePlacementName == "Glouton Gravite") G.gloutonRevisiteGravite();
		else if (methodePlacementName == "Glouton Voisin") G.gloutonRevisiteVoisin();
		//else if (methodePlacementName == "OGDF") ogdfPlacementAuPlusProche(G);
		else if (methodePlacementName == "Stress") { G.stressMajorization(customParam); }
		else if (methodePlacementName == "Stress Dyn Stress") { G.stressMajorization(customParam,-1,400,1); }
		else if (methodePlacementName == "Stress Dyn Cross") { G.stressMajorization(customParam,-1,400,2); }
		else if (methodePlacementName == "Glouton Grille") { G.gloutonRevisiteGrid(); }
		else if (methodePlacementName == "Aleatoire") G.placementAleatoire();
		else if (methodePlacementName != "Aucun") {
			std::cout << "ERROR Aucune methode " << methodePlacementName << " trouve !\n";
			return;
		}
		if (updateScore) {
			G.initGraphAndNodeScoresAndCrossings();
		}
		if (needTriangulation) {
			G.triangulationDelaunay();
		}
		if (needGrille) {
			if (customParam.size() > 0) {
				bool found = false;
				for (std::vector<double>& param : customParam) {
					if (param.size() > 0) {
						if (param[0] == 10) {
							int row = (int)(ceil(sqrt(G._noeuds.size()))*param[1]);
							row = max(1,row);
							G.initGrille(row,row);
							G.registerSlotsAndEdgesInGrid();
							found = true;
						}
						else if (param[0] == 11) {
							int row = (int)(ceil(sqrt(G._aretes.size()))*param[1]);
							row = max(1,row);
							G.initGrille(row,row);
							G.registerSlotsAndEdgesInGrid();
							found = true;
						}
					}
				}
				if (!found) {
					G.initGrille();
					G.registerSlotsAndEdgesInGrid();
				}
			}
			else {
				G.initGrille();
				G.registerSlotsAndEdgesInGrid();
			}
		}

		if (methodeAlgoName == "Recuit Simule") G.recuitSimule(tempsBest,customParam,0.99999,100.0,0.0001,1,0,0,false,false);
		else if (methodeAlgoName == "Rerecuit Simule Grille TME Custom") G.rerecuitSimule(tempsBest,nombreRecuit,customParam);
		else if (methodeAlgoName == "Rerecuit Simule Grille TME Cool") G.rerecuitSimule(tempsBest,nombreRecuit,customParam,-1,0.999999);
		else if (methodeAlgoName == "Rerecuit Simule Grille TME Cooler") G.rerecuitSimule(tempsBest,nombreRecuit,customParam,-1,0.9999999);
		else if (methodeAlgoName == "Rerecuit Simule Grille TME Cool Delay") G.rerecuitSimule(tempsBest,nombreRecuit,customParam,-1,0.999999,0.99,100.0,0.0001,2);
		else if (methodeAlgoName == "Rerecuit Simule Grille TME Cool Delay Temp") G.rerecuitSimule(tempsBest,nombreRecuit,customParam,-1,0.999999,0.99,0.1,0.0001,2);
		else if (methodeAlgoName == "Best Deplacement") G.bestDeplacement();
		else if (methodeAlgoName == "Genetique Recuit") G.grapheGenetique(tempsBest,bestIteration,lastIteration, population, maxIteration, fileGraph, fileSlots, true);
		else if (methodeAlgoName == "Genetique Recuit Random") G.grapheGenetique(tempsBest,bestIteration,lastIteration, population, maxIteration, fileGraph, fileSlots, true, true);
		else if (methodeAlgoName == "Genetique") G.grapheGenetique(tempsBest,bestIteration,lastIteration, population, maxIteration, fileGraph, fileSlots);
		else if (methodeAlgoName == "Genetique Random") G.grapheGenetique(tempsBest,bestIteration,lastIteration, population, maxIteration, fileGraph, fileSlots, false, true);
		else if (methodeAlgoName == "Genetique Score") G.grapheGenetique(tempsBest,bestIteration,lastIteration, population,maxIteration,fileGraph,fileSlots,false,false,3);
		else if (methodeAlgoName == "Genetique Score Recuit") G.grapheGenetique(tempsBest,bestIteration,lastIteration, population,maxIteration,fileGraph,fileSlots,true,false,3);
		else if (methodeAlgoName == "Genetique Enfant") G.grapheGenetique(tempsBest,bestIteration,lastIteration, population,maxIteration,fileGraph,fileSlots,false,false,5);
		else if (methodeAlgoName == "Genetique Enfant Recuit") G.grapheGenetique(tempsBest,bestIteration,lastIteration, population,maxIteration,fileGraph,fileSlots,true,false,5);
		else if (methodeAlgoName != "Aucun") {
			std::cout << "ERROR Aucun algo " << methodeAlgoName << " trouve !\n";
			return;
		}

		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> secondsTotal = end - start;
		secondsTotalExec = end - totalStart;
		tempExecVector.push_back(secondsTotal.count());
		tempBestVector.push_back(tempsBest);
		bestIterationVector.push_back(bestIteration);
		lastIterationVector.push_back(lastIteration);
		nombreRecuitVector.push_back(nombreRecuit);
		nombreSlots = G._emplacements.size();
		nombreCellule = G.grillePtr.size();
		if (G.isNombreCroisementUpdated) {
			croisementVector.push_back(G.nombreCroisement);
		}
		else {
			croisementVector.push_back(G.getNbCroisementConst());
		}
		if (G.hasIllegalCrossing()) {
			nbSolutionIllegale++;
		}
		totalInterVector.push_back(G.getNbCroisementDiff());
		totalInterIllVector.push_back(G.nombreInterIll + G.nombreInterIllSelf);
		debugValue = G.debugEverything();
		if (debugValue > 0) { break; }
	}
	resetSeed(tid,false,true);
	if (saveResult) {
		std::sort(croisementVector.begin(), croisementVector.end());
		meilleurCroisement = croisementVector[0];
		moyenneCroisement = moyenneVector(croisementVector);
		medianCroisement = medianeVector(croisementVector);
		tempsExecMoyenne = moyenneVector(tempExecVector);
		tempsBestMoyenne = moyenneVector(tempBestVector);
		bestIterationMoyenne = moyenneVector(bestIterationVector);
		lastIterationMoyenne = moyenneVector(lastIterationVector);
		nombreRecuitMoyenne = moyenneVector(nombreRecuitVector);
		totalInterMoyenne = moyenneVector(totalInterVector);
		totalInterIllMoyenne = moyenneVector(totalInterIllVector);

		std::string nomFichier = chemin + "/resultats/" + nomGraphe + to_string(tid) + ".csv";
		std::ofstream resultats(nomFichier, std::ios_base::app);

		char date[80];
		time_t t = time(0);
    	strftime(date, 80, "%d/%m/%Y", localtime(&t));

		resultats << std::fixed;
		resultats << methodePlacementName << ","
			<< methodeAlgoName << ","
			<< std::setprecision(0) << croisementVector.size() << ","
			<< nombreSlots << ","
			<< nbSolutionIllegale << ","
			<< totalInterMoyenne << "," << totalInterIllMoyenne << ","
			<< meilleurCroisement << ",";
		if (moyenneCroisement > 100) { resultats << std::setprecision(0) << moyenneCroisement << ","; }
		else { resultats << std::setprecision(1) << moyenneCroisement << ","; }
		if (medianCroisement > 100) { resultats << std::setprecision(0) << medianCroisement << ","; }
		else { resultats << std::setprecision(1) << medianCroisement << ","; }
		resultats << std::setprecision(0) << tempsBestMoyenne << "," << tempsExecMoyenne;
		if (isGenetique) {
			resultats << std::setprecision(0) << "," << population << "," << maxIteration << "," << bestIterationMoyenne << "," << lastIterationMoyenne;
		}
		else if (isRecuit) {
			resultats << "," << std::setprecision(1) << nombreRecuitMoyenne;
		}
		else {
			resultats << ",";
		}
		resultats << ",";
		if (isSeedResetting(tid)) { resultats << "R-"; }
		else { resultats << "NR-"; }
		resultats << getSeed(tid) << "," << machine;
		resultats << "," << getParamAsString(customParam);
		resultats << "," << nombreCellule;
		resultats << "," << debugValue << "," << date << "\n";
		resultats.close();
	}
}


#endif
