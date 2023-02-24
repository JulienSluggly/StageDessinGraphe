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
#include <algorithm>

std::vector<std::string> methodeWithScore;
std::vector<std::string> methodeGenetique;
std::vector<std::string> methodeTriangulation;
std::vector<std::string> methodeGrille;
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
	methodeTriangulation.push_back("Rerecuit Simule Grille Best TRE");
	methodeTriangulation.push_back("Rerecuit Simule Grille TRE");
	methodeTriangulation.push_back("Rerecuit Simule Grille TRE Custom");
	methodeTriangulation.push_back("Recuit Simule Grille TRE Custom");
	methodeTriangulation.push_back("Rerecuit Simule Grille TRE Delay");
}

void fillVectorGrille() {
	methodeGrille.push_back("Recuit Simule Grille");
	methodeGrille.push_back("Recuit Simule Grille TME");
	methodeGrille.push_back("Rerecuit Simule Grille");
	methodeGrille.push_back("Rerecuit Simule Grille TME");
	methodeGrille.push_back("Rerecuit Simule Grille TRE");
	methodeGrille.push_back("Rerecuit Simule Grille Best");
	methodeGrille.push_back("Rerecuit Simule Grille Best TRE");
	methodeGrille.push_back("Rerecuit Simule Grille TME Custom");
	methodeGrille.push_back("Rerecuit Simule Grille TRE Custom");
	methodeGrille.push_back("Recuit Simule Grille TRE Custom");
	methodeGrille.push_back("Rerecuit Simule Grille TME Delay");
	methodeGrille.push_back("Rerecuit Simule Grille TRE Delay");
}

void fillLogsVector() {
	fillMap();
	fillVectorScore();
	fillVectorGenetique();
	fillVectorTriangulation();
	fillVectorGrille();
}

void generateCSV(int nbEssay, const std::string& methodeName, const std::string& methodeAlgoName, const std::string& nomGraphe, std::string fileGraph, std::string fileSlots, std::vector<double> customParam={}, int tid=0) {
	bool updateScore = isInVector(methodeWithScore,methodeAlgoName);
	bool isGenetique = isInVector(methodeGenetique,methodeAlgoName);
	bool needTriangulation = isInVector(methodeTriangulation,methodeAlgoName);
	bool needGrille = isInVector(methodeGrille,methodeAlgoName);
	double moyenneCroisement, medianCroisement;
	int meilleurCroisement = INT_MAX;
	int nbSolutionIllegale = 0, debugValue=-1;
	std::vector<int> croisementVector;
	std::vector<double> tempExecVector; std::vector<double> tempBestVector; std::vector<int> bestIterationVector; std::vector<int> lastIterationVector;
	double tempsExecMoyenne = 0; double tempsBestMoyenne = 0; double bestIterationMoyenne = 0; double lastIterationMoyenne = 0;
	bool saveResult = true;
	int population, maxIteration;
	int nombreRecuit = 0, nombreSlots, nombreCellule;
	auto totalStart = std::chrono::system_clock::now();
	std::chrono::duration<double> secondsTotalExec = totalStart - totalStart;
	for (int i = 1; (((i <= nbEssay)&&(secondsTotalExec.count() < 3600))||(nbEssay==-1&&secondsTotalExec.count() < 3600)); ++i) {
		resetSeed(tid);
		Graphe G;
		G.setupGraphe(fileGraph,fileSlots);
		double tempsBest = -1; int bestIteration = -1; int lastIteration = -1;
		if (isGenetique) {
			population = mapGraphPopGen[nomGraphe].first;
			maxIteration = mapGraphPopGen[nomGraphe].second;
		}
		saveResult = true;
		if (customParam.size() > 1) {
			printf("Tid: %d | Iter: %d Max: %d | %s | %s | Slots: %lu | Param: {%.0f,%.2f} | TotalRun: %.1fs\n",tid,i,nbEssay,nomGraphe.c_str(),methodeAlgoName.c_str(),G._emplacementsPossibles.size(),customParam[0],customParam[1],secondsTotalExec.count());
		}
		else {
			printf("Tid: %d | Iter: %d Max: %d | %s | %s | Slots: %lu | TotalRun: %.1fs\n",tid,i,nbEssay,nomGraphe.c_str(),methodeAlgoName.c_str(),G._emplacementsPossibles.size(),secondsTotalExec.count());
		}
		auto start = std::chrono::system_clock::now();
		if (methodeName == "Glouton") G.glouton();
		else if (methodeName == "Glouton Revisite") G.gloutonRevisite();
		else if (methodeName == "Glouton Gravite") G.gloutonRevisiteGravite();
		else if (methodeName == "Glouton Voisin") G.gloutonRevisiteVoisin();
		//else if (methodeName == "OGDF") ogdfPlacementAuPlusProche(G);
		else if (methodeName == "Aleatoire") G.placementAleatoire();
		else if (methodeName != "Aucun") {
			std::cout << "ERROR Aucune methode " << methodeName << " trouve !\n";
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
				if (customParam[0] == 10) {
					int row = (int)(ceil(sqrt(G._noeuds.size()))*customParam[1]);
					row = max(1,row);
					G.initGrille(row,row);
					G.registerSlotsAndEdgesInGrid();
				}
				else if (customParam[0] == 11) {
					int row = (int)(ceil(sqrt(G._liens.size()))*customParam[1]);
					row = max(1,row);
					G.initGrille(row,row);
					G.registerSlotsAndEdgesInGrid();
				}
				else {
					G.initGrille();
					G.registerSlotsAndEdgesInGrid();
				}
			}
			else {
				G.initGrille();
				G.registerSlotsAndEdgesInGrid();
			}
		}

		if (methodeAlgoName == "Recuit Simule") G.recuitSimule(tempsBest);
		else if (methodeAlgoName == "Rerecuit Simule Grille Best") G.rerecuitSimuleGrid(tempsBest,nombreRecuit,-1,0.999999,0.999,150.0,0.000001,1,0,3);
		else if (methodeAlgoName == "Rerecuit Simule Grille Best TRE") G.rerecuitSimuleGrid(tempsBest,nombreRecuit,-1,0.999999,0.999,150.0,0.000001,1,0,4);
		else if (methodeAlgoName == "Recuit Simule TME") G.recuitSimule(tempsBest,0.99999, 100.0,0.0001,1,0,3);
		else if (methodeAlgoName == "Rerecuit Simule TME") G.rerecuitSimule(tempsBest,nombreRecuit,-1,0.99999,0.99,100.0,0.0001,1,0,3);
		else if (methodeAlgoName == "Recuit Simule TRE") G.recuitSimule(tempsBest,0.99999, 100.0,0.0001,1,0,4);
		else if (methodeAlgoName == "Rerecuit Simule TRE") G.rerecuitSimule(tempsBest,nombreRecuit,-1,0.99999,0.99,100.0,0.0001,1,0,4);
		else if (methodeAlgoName == "Recuit Simule Grille TME") G.recuitSimuleGrid(tempsBest,0.99999, 100.0,0.0001,1,0,3);
		else if (methodeAlgoName == "Rerecuit Simule Grille TME") G.rerecuitSimuleGrid(tempsBest,nombreRecuit,-1,0.99999,0.99,100.0,0.0001,1,0,3);
		else if (methodeAlgoName == "Rerecuit Simule Grille TME Delay") G.rerecuitSimuleGrid(tempsBest,nombreRecuit,-1,0.99999,0.99,100.0,0.0001,10,0,3);
		else if (methodeAlgoName == "Recuit Simule Grille TRE") G.recuitSimuleGrid(tempsBest,0.99999, 100.0,0.0001,1,0,4);
		else if (methodeAlgoName == "Rerecuit Simule Grille TRE") G.rerecuitSimuleGrid(tempsBest,nombreRecuit,-1,0.99999,0.99,100.0,0.0001,1,0,4);
		else if (methodeAlgoName == "Rerecuit Simule Grille TRE Delay") G.rerecuitSimuleGrid(tempsBest,nombreRecuit,-1,0.99999,0.99,100.0,0.0001,10,0,4);
		else if (methodeAlgoName == "Recuit Simule TME Custom") G.recuitSimuleCustom(tempsBest,0.99999, 100.0,0.0001, 1, 0, 3, customParam);
		else if (methodeAlgoName == "Recuit Simule Delay TME Custom") G.recuitSimuleCustom(tempsBest,0.99999,100.0,0.0001,-1,0,3,customParam);
		else if (methodeAlgoName == "Rerecuit Simule Delay TME Custom") G.rerecuitSimuleCustom(tempsBest,nombreRecuit,-1,0.99999,0.99,100.0,0.0001,-1,0,3,customParam);
		else if (methodeAlgoName == "Rerecuit Simule Delay TME") G.rerecuitSimule(tempsBest,nombreRecuit,-1,0.99999,0.99,100.0,0.0001,-1,0,3);
		else if (methodeAlgoName == "Recuit Simule TRE Custom") G.recuitSimuleCustom(tempsBest,0.99999,100.0,0.0001,1,0,4,customParam);
		else if (methodeAlgoName == "Recuit Simule Grille TRE Custom") G.recuitSimuleGrid(tempsBest,0.99999, 100.0,0.0001, 1, 0, 4, customParam);
		else if (methodeAlgoName == "Rerecuit Simule Grille TRE Custom") G.rerecuitSimuleGrid(tempsBest,nombreRecuit,-1,0.99999,0.99,100.0,0.0001,1,0,4,customParam);
		else if (methodeAlgoName == "Rerecuit Simule Grille TME Custom") G.rerecuitSimuleGrid(tempsBest,nombreRecuit,-1,0.99999,0.99,100.0,0.0001,1,0,3,customParam);
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
		nombreSlots = G._emplacementsPossibles.size();
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
		debugValue = G.debugEverything();
		if (debugValue > 0) { break; }
	}
	if (saveResult) {
		std::sort(croisementVector.begin(), croisementVector.end());
		meilleurCroisement = croisementVector[0];
		moyenneCroisement = moyenneVector(croisementVector);
		medianCroisement = medianeVector(croisementVector);
		tempsExecMoyenne = moyenneVector(tempExecVector);
		tempsBestMoyenne = moyenneVector(tempBestVector);
		bestIterationMoyenne = moyenneVector(bestIterationVector);
		lastIterationMoyenne = moyenneVector(lastIterationVector);

		std::string nomFichier = chemin + "/resultats/" + nomGraphe + to_string(tid) + ".csv";
		std::ofstream resultats(nomFichier, std::ios_base::app);
		std::streampos position = resultats.tellp();
		if (position == 0) {
			//resultats << "MethodeUtilisee,AlgoDeplacement,NbRun,NbSlots,NbIllegal,BestCross,MoyCross,MedCross,MoyBest(s),MoyTotal(s),Population,MaxGen,MoyBestGen,MoyLastGen,Machine,CustomParams\n";
		}

		resultats << std::fixed;
		resultats << methodeName << ","
			<< methodeAlgoName << ","
			<< std::setprecision(0) << croisementVector.size() << ","
			<< nombreSlots << ","
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
		resultats << "," << nombreRecuit << "," << getTypeSeed() << "," << machine;
		if (customParam.size() > 1) {
			resultats << "," << std::setprecision(1) << customParam[0] << " " << std::setprecision(2) << customParam[1];
			for (int j=2;j<customParam.size();j++) {
				resultats << " " << to_string(customParam[j]) << " ";
			}
		}
		if (needGrille) {
			resultats << "," << nombreCellule;
		}
		resultats << "," << debugValue << "\n";
		resultats.close();
	}
}


#endif
