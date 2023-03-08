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
	methodeGrille.push_back("Rerecuit Simule Grille TME Cool");
}

void fillLogsVector() {
	fillMap();
	fillVectorScore();
	fillVectorGenetique();
	fillVectorTriangulation();
	fillVectorGrille();
}

void generateCSV(int nbEssay, const std::string& methodePlacementName, const std::string& methodeAlgoName, const std::string& nomGraphe, std::string fileGraph, std::string fileSlots, std::vector<double> customParam={}, int tid=0) {
	bool updateScore = isInVector(methodeWithScore,methodeAlgoName);
	bool isGenetique = isInVector(methodeGenetique,methodeAlgoName);
	bool needTriangulation = isInVector(methodeTriangulation,methodeAlgoName);
	bool needGrille = isInVector(methodeGrille,methodeAlgoName);
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
	resetSeed(tid);
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
		if (customParam.size() > 1) {
			if (customParam.size() > 2) {
				printf("Tid: %d | Iter: %d Max: %d | %s | %s | %s | Slots: %lu | Param: {%.0f,%.0f,%.2f} | TotalRun: %.1fs\n",tid,i,nbEssay,nomGraphe.c_str(),methodePlacementName.c_str(),methodeAlgoName.c_str(),G._emplacements.size(),customParam[0],customParam[1],customParam[2],secondsTotalExec.count());
			}
			else {
				printf("Tid: %d | Iter: %d Max: %d | %s | %s | %s | Slots: %lu | Param: {%.0f,%.2f} | TotalRun: %.1fs\n",tid,i,nbEssay,nomGraphe.c_str(),methodePlacementName.c_str(),methodeAlgoName.c_str(),G._emplacements.size(),customParam[0],customParam[1],secondsTotalExec.count());
			}
		}
		else {
			printf("Tid: %d | Iter: %d Max: %d | %s | %s | %s | Slots: %lu | TotalRun: %.1fs\n",tid,i,nbEssay,nomGraphe.c_str(),methodePlacementName.c_str(),methodeAlgoName.c_str(),G._emplacements.size(),secondsTotalExec.count());
		}
		if (methodePlacementName == "Glouton") G.glouton();
		else if (methodePlacementName == "Glouton Revisite") G.gloutonRevisite();
		else if (methodePlacementName == "Glouton Gravite") G.gloutonRevisiteGravite();
		else if (methodePlacementName == "Glouton Voisin") G.gloutonRevisiteVoisin();
		//else if (methodePlacementName == "OGDF") ogdfPlacementAuPlusProche(G);
		else if (methodePlacementName == "Stress") { G.stressMajorization(); }
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
				if (customParam[0] == 10) {
					int row = (int)(ceil(sqrt(G._noeuds.size()))*customParam[1]);
					row = max(1,row);
					G.initGrille(row,row);
					G.registerSlotsAndEdgesInGrid();
				}
				else if (customParam[0] == 11) {
					int row = (int)(ceil(sqrt(G._aretes.size()))*customParam[1]);
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

		if (methodeAlgoName == "Recuit Simule") G.recuitSimule(tempsBest,customParam,0.99999,100.0,0.0001,1,0,0,false,false);
		else if (methodeAlgoName == "Rerecuit Simule Grille TME Custom") G.rerecuitSimule(tempsBest,nombreRecuit,customParam);
		else if (methodeAlgoName == "Rerecuit Simule Grille TME Cool") G.rerecuitSimule(tempsBest,nombreRecuit,customParam,-1,0.999999);
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
		std::streampos position = resultats.tellp();
		if (position == 0) {
			//resultats << "MethodeUtilisee,AlgoDeplacement,NbRun,NbSlots,NbIllegal,BestCross,MoyCross,MedCross,MoyBest(s),MoyTotal(s),Population,MaxGen,MoyBestGen,MoyLastGen,Machine,CustomParams\n";
		}

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
		else {
			resultats << "," << std::setprecision(1) << nombreRecuitMoyenne;
		}
		resultats << ",";
		if (isSeedResetting(tid)) { resultats << "R-"; }
		else { resultats << "NR-"; }
		resultats << getSeed(tid) << "," << machine;
		if (customParam.size() > 1) {
			resultats << "," << std::setprecision(0) << customParam[0] << " " << std::setprecision(2) << customParam[1];
			for (int j=2;j<customParam.size();j++) {
				resultats << " " << to_string(customParam[j]);
			}
		}
		else {
			resultats << ",";
		}
		resultats << "," << nombreCellule;
		resultats << "," << debugValue << "," << date << "\n";
		resultats.close();
	}
}


#endif
