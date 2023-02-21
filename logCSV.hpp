#ifndef LOGCSV_HPP
#define LOGCSV_HPP

#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include "ogdfFunctions.hpp"
#include "personnel.hpp"
#include <omp.h>
#include <climits>

std::vector<std::string> methodeWithScore;
std::vector<std::string> methodeGenetique;
std::vector<std::string> methodeTriangulation;
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

void fillVectorScore() {
	methodeWithScore.push_back("Recuit Simule Score");
	methodeWithScore.push_back("Genetique Score");
	methodeWithScore.push_back("Genetique Score Recuit");
}

void fillVectorGenetique() {
	methodeGenetique.push_back("Genetique");
	methodeGenetique.push_back("Genetique Recuit");
	methodeGenetique.push_back("Genetique Recuit Random");
	methodeGenetique.push_back("Genetique Random");
	methodeGenetique.push_back("Genetique Score");
	methodeGenetique.push_back("Genetique Score Recuit");
	methodeGenetique.push_back("Genetique Enfant");
	methodeGenetique.push_back("Genetique Enfant Recuit");
}

void fillVectorTriangulation() {
	methodeTriangulation.push_back("Recuit Simule TRE");
	methodeTriangulation.push_back("Rerecuit Simule TRE");
	methodeTriangulation.push_back("Recuit Simule TRE Custom");
	methodeTriangulation.push_back("Rerecuit Simule TRE Custom");
}

void fillLogsVector() {
	fillMap();
	fillVectorScore();
	fillVectorGenetique();
	fillVectorTriangulation();
}

void generateCSV(double nbEssay, const std::string& methodeName, const std::string& methodeAlgoName, const std::string& nomGraphe, Graphe& G, std::string fileGraph="None", std::string fileSlots="None", std::vector<double> customParam={}, std::string tid="") {
	bool updateScore = isInVector(methodeWithScore,methodeAlgoName);
	bool isGenetique = isInVector(methodeGenetique,methodeAlgoName);
	bool needTriangulation = isInVector(methodeTriangulation,methodeAlgoName);
	double moyenneCroisement = 0, medianCroisement;
	int meilleurCroisement = INT_MAX;
	int nbSolutionIllegale = 0;
	std::vector<int> croisementVector;
	std::vector<double> tempExecVector; std::vector<double> tempBestVector; std::vector<int> bestIterationVector; std::vector<int> lastIterationVector;
	double tempsExecMoyenne = 0; double tempsBestMoyenne = 0; double bestIterationMoyenne = 0; double lastIterationMoyenne = 0;
	bool saveResult = true;
	auto totalStart = std::chrono::system_clock::now();
	std::chrono::duration<double> secondsTotal = totalStart - totalStart;
	int population, maxIteration;
	int nombreRecuit = 0;
	for (int i = 1; (i <= nbEssay||(nbEssay==-1&&secondsTotal.count() < 1800)); ++i) {
		double tempsBest = -1; int bestIteration = -1; int lastIteration = -1;
		if (isGenetique) {
			population = mapGraphPopGen[nomGraphe].first;
			maxIteration = mapGraphPopGen[nomGraphe].second;
		}
		saveResult = true;
		std::cout << "Current Execution: " << i << "\n";
		auto start = std::chrono::system_clock::now();
		if (methodeName == "Glouton") G.glouton();
		else if (methodeName == "Glouton Revisite") G.gloutonRevisite();
		else if (methodeName == "Glouton Gravite") G.gloutonRevisiteGravite();
		else if (methodeName == "Glouton Voisin") G.gloutonRevisiteVoisin();
		else if (methodeName == "OGDF") ogdfPlacementAuPlusProche(G);
		else if (methodeName == "Aleatoire") G.placementAleatoire();
		else if (methodeName != "Aucun") {
			std::cout << "ERROR Aucune methode " << methodeName << " trouve !";
			return;
		}

		if (updateScore) {
			G.initGraphAndNodeScoresAndCrossings();
		}
		if (needTriangulation) {
			if (!G.isCarteSetUp) {
				G.triangulationDelaunay();
			}
		}

		if (methodeAlgoName == "Recuit Simule") G.recuitSimule(tempsBest);
		else if (methodeAlgoName == "Recuit Simule Score") G.recuitSimuleScore(tempsBest);
		else if (methodeAlgoName == "Recuit Simule Delay") G.recuitSimule(tempsBest,0.99999,100.0,0.0001,10);
		else if (methodeAlgoName == "Recuit Simule TBN") G.recuitSimule(tempsBest,0.99999,100.0,0.0001,1,1);
		else if (methodeAlgoName == "Recuit Simule TMN") G.recuitSimule(tempsBest,0.99999,100.0,0.0001,1,2);
		else if (methodeAlgoName == "Recuit Simule TBE") G.recuitSimule(tempsBest,0.99999, 100.0,0.0001,1,0,1);
		else if (methodeAlgoName == "Recuit Simule TME") G.recuitSimule(tempsBest,0.99999, 100.0,0.0001,1,0,2);
		else if (methodeAlgoName == "Recuit Simule Delay TBE") G.recuitSimule(tempsBest,0.99999, 100.0,0.0001, 10, 0, 1);
		else if (methodeAlgoName == "Recuit Simule Delay TME") G.recuitSimule(tempsBest,0.99999, 100.0,0.0001, -1, 0, 3);
		else if (methodeAlgoName == "Recuit Simule TBNE") G.recuitSimule(tempsBest,0.99999, 100.0,0.0001, 1, 1, 1);
		else if (methodeAlgoName == "Recuit Simule TBN TME") G.recuitSimule(tempsBest,0.99999, 100.0,0.0001, 1, 1, 3);
		else if (methodeAlgoName == "Recuit Simule TMN TBE") G.recuitSimule(tempsBest,0.99999, 100.0,0.0001, 1, 2, 1);
		else if (methodeAlgoName == "Recuit Simule TMNE") G.recuitSimule(tempsBest,0.99999, 100.0,0.0001, 1, 2, 2);
		else if (methodeAlgoName == "Recuit Simule TME Custom") G.recuitSimuleCustom(tempsBest,0.99999, 100.0,0.0001, 1, 0, 3, customParam);
		else if (methodeAlgoName == "Recuit Simule Delay TME Custom") G.recuitSimuleCustom(tempsBest,0.99999,100.0,0.0001,-1,0,3,customParam);
		else if (methodeAlgoName == "Rerecuit Simule Delay TME Custom") G.rerecuitSimuleCustom(tempsBest,nombreRecuit,-1,0.99999,0.99,100.0,0.0001,-1,0,3,customParam);
		else if (methodeAlgoName == "Rerecuit Simule TME") G.rerecuitSimule(tempsBest,nombreRecuit,-1,0.99999,0.99,100.0,0.0001,1,0,3);
		else if (methodeAlgoName == "Rerecuit Simule Delay TME") G.rerecuitSimule(tempsBest,nombreRecuit,-1,0.99999,0.99,100.0,0.0001,-1,0,3);
		else if (methodeAlgoName == "Recuit Simule TRE") G.recuitSimule(tempsBest,0.99999, 100.0,0.0001,1,0,4);
		else if (methodeAlgoName == "Rerecuit Simule TRE") G.rerecuitSimule(tempsBest,nombreRecuit,-1,0.99999,0.99,100.0,0.0001,1,0,4);
		else if (methodeAlgoName == "Recuit Simule TRE Custom") G.recuitSimuleCustom(tempsBest,0.99999,100.0,0.0001,1,0,4,customParam);
		else if (methodeAlgoName == "Rerecuit Simule TRE Custom") G.rerecuitSimuleCustom(tempsBest,nombreRecuit,-1,0.99999,0.99,100.0,0.0001,1,0,4,customParam);
		else if (methodeAlgoName == "Rerecuit Simule TME Custom") G.rerecuitSimuleCustom(tempsBest,nombreRecuit,-1,0.99999,0.99,100.0,0.0001,1,0,3,customParam);
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
			std::cout << "ERROR Aucun algo " << methodeAlgoName << " trouve !";
			return;
		}

		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> secondsTotal = end - start;
		secondsTotal = end - totalStart;
		tempExecVector.push_back(secondsTotal.count());
		tempBestVector.push_back(tempsBest);
		bestIterationVector.push_back(bestIteration);
		lastIterationVector.push_back(lastIteration);
		if (G.isNombreCroisementUpdated) {
			croisementVector.push_back(G.nombreCroisement);
		}
		else {
			croisementVector.push_back(G.getNbCroisementConst());
		}
		if (G.hasIllegalCrossing()) {
			nbSolutionIllegale++;
		}
		G.clearNodeEmplacement();

	}
	if (saveResult) {
		sort(croisementVector.begin(), croisementVector.end());
		meilleurCroisement = croisementVector[0];
		moyenneCroisement = moyenneVector(croisementVector);
		medianCroisement = medianeVector(croisementVector);
		tempsExecMoyenne = moyenneVector(tempExecVector);
		tempsBestMoyenne = moyenneVector(tempBestVector);
		bestIterationMoyenne = moyenneVector(bestIterationVector);
		lastIterationMoyenne = moyenneVector(lastIterationVector);
		

		std::string nomFichier = chemin + "/resultats/" + nomGraphe + tid + ".csv";
		std::ofstream resultats(nomFichier, std::ios_base::app);
		std::streampos position = resultats.tellp();
		if (position == 0) {
			resultats << "MethodeUtilisee,AlgoDeplacement,NbRun,NbSlots,NbIllegal,BestCross,MoyCross,MedCross,MoyBest(s),MoyTotal(s),Population,MaxGen,MoyBestGen,MoyLastGen,Machine,CustomParams\n";
		}

		resultats << std::fixed;
		resultats << methodeName << ","
			<< methodeAlgoName << ","
			<< std::setprecision(0) << croisementVector.size() << ","
			<< G._emplacementsPossibles.size() << ","
			<< nbSolutionIllegale << ","
			<< meilleurCroisement << ",";
		if (moyenneCroisement > 100) { resultats << std::setprecision(0) << moyenneCroisement << ","; }
		else { resultats << std::setprecision(1) << moyenneCroisement << ","; }
		if (medianCroisement > 100) { resultats << std::setprecision(0) << medianCroisement << ","; }
		else { resultats << std::setprecision(1) << medianCroisement << ","; }
		resultats << std::setprecision(7) << tempsBestMoyenne << "," << tempsExecMoyenne;
		if (isGenetique) {
			resultats << std::setprecision(0) << "," << population << "," << maxIteration << "," << bestIterationMoyenne << "," << lastIterationMoyenne;
		}
		resultats << "," << nombreRecuit << "," << machine;
		if (customParam.size() > 0) {
			resultats << ",";
			for (int j=0;j<customParam.size();j++) {
				resultats << to_string(customParam[j]) << " ";
			}
		}
		resultats << "\n";
		resultats.close();
	}
}


#endif
