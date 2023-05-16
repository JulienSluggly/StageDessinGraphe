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
#include "graphe.hpp"
#include <omp.h>
#include <climits>
#include <ctime>
#include <algorithm>

class LogGraphe {
public:

	std::string nomGraphe;
	std::string methodePlacementName;
	std::string methodeAlgoName;
	std::string fileGraph;
	std::string fileSlots;
	bool updateScore; // Est ce que le graphe utilise les algoritmes stockant le score de chaque noeud/aretes
	bool isGenetique;
	bool isRecuit;
	bool needTriangulation;
	bool needGrille; // Est ce que la structure de cellules est utilisée

	std::vector<long> scoreVector; // Nombre d'intersection AVEC pénalité de chaque run
	std::vector<double> tempsExecVector; // Temps total mis par une run (debut boucle a fin de la boucle)
	std::vector<double> tempsBestVector; // Temps mis pour trouver le meilleur resultat de chaque execution
	std::vector<double> tempsPlacementVector; // Temps de chaque placement
	std::vector<int> bestIterationVector; // meilleur itération GENETIQUE
	std::vector<int> lastIterationVector; // derniere itération GENETIQUE
	std::vector<int> nombreRecuitVector; // Nombre de rechauffement+1 a chaque run
	std::vector<int> totalInterVector; // Nombre d'intersection SANS pénalité de chaque run
	std::vector<int> totalInterIllVector; // Nombre d'intersection illégales de chaque run
	std::vector<int> placementInterVector; // Nombre d'intersection SANS pénalité de chaque run
	std::vector<int> edgeCostVector; // Si stressminimization custom, contient le edgecost choisi a chaque run
	std::vector<int> recuitCycleVague; // Contient le nombre de recuit qui améliore et qui n'améliore pas, ordonné

	std::vector<std::vector<double>> customParam;

	int nbSolutionIllegale = 0;
	int debugValue=-1; // Voir fonction dans grapheDebug.cpp pour la correspondance du code d'erreur si différent de -1.

	int tid; // Thread ID

	int timeLimitRecuit;
	int nombreRecuit = 0;
	int nombreSlots;
	int nombreCellule;

	double tempsBest = -1;
	int bestIteration = -1; // Genetique
	int lastIteration = -1; // Genetique
	int genetiquePopulation;
	int genetiqueMaxIteration; // Genetique

	int currentIteration = 1;

	std::chrono::_V2::system_clock::time_point start; // Temps de debut de run
	std::chrono::_V2::system_clock::time_point finPlacement; // Temps de fin de placement

    LogGraphe(const std::string& fileG, const std::string& fileS, const std::string& methodePlacement, const std::string& methodeAlgo, std::vector<std::vector<double>>& params,int threadId);

	void writeGrapheToFile(Graphe& G, int tid);
	void writeResultToFile();
	void writeRecuitResultToFile();
	void writeGenetiqueResultToFile();
	void updateDataFromGraphe(Graphe& G);
	std::string getParamAsString();
	std::string getRecuitCycleAsString();
	void saveGrapheDistanceCSV(Graphe& G, int tid);
	void saveGrapheScoreTempsCSV(Graphe& G, int tid);
	int setTimeLimitRecuit();
	void setupGrapheWithTypeFile(Graphe& G, std::string& typeFile, std::string& fileGraph, std::string& fileSlots);
	void placementGraphe(Graphe& G);
	void setupGrapheStruct(Graphe& G);
	void heuristiqueGraphe(Graphe& G);

};


LogGraphe::LogGraphe(const std::string& fileG, const std::string& fileS, const std::string& methodePlacement, const std::string& methodeAlgo, std::vector<std::vector<double>>& params,int threadId) {
    customParam = params;
    tid = threadId;
    
    nomGraphe = fileG;
	std::reverse(nomGraphe.begin(), nomGraphe.end());
	nomGraphe = nomGraphe.substr(nomGraphe.find(".") + 1);
	nomGraphe = nomGraphe.substr(0, nomGraphe.find('/'));
	std::reverse(nomGraphe.begin(), nomGraphe.end());

    fileGraph = fileG;
    fileSlots = fileS;

    methodePlacementName = methodePlacement;
    methodeAlgoName = methodeAlgo;

    updateScore = containsString(methodeAlgoName,"Score");
	isGenetique = containsString(methodeAlgoName,"Genetique");
	isRecuit = containsString(methodeAlgoName,"Recuit");
	needTriangulation = containsString(methodeAlgoName,"TRE");
	needGrille = containsString(methodeAlgoName,"Grille");

    timeLimitRecuit = setTimeLimitRecuit();
}

void LogGraphe::updateDataFromGraphe(Graphe& G) {
    nombreSlots = G._emplacements.size();
    nombreCellule = G.grillePtr.size();
    if (G.isNombreCroisementUpdated) { scoreVector.push_back(G.nombreCroisement); }
    else { if (!G.useCoordReel) { scoreVector.push_back(G.getNbCroisementConst()); } else { scoreVector.push_back(G.getNbCroisementReelConst()); } }
    if (G.hasIllegalCrossing()) { nbSolutionIllegale++; }
    if (G._sm.G != nullptr) { edgeCostVector.push_back(G._sm.m_edgeCosts); }
    if (!G.useCoordReel) { totalInterVector.push_back(G.getNbCroisementDiff()); } else { totalInterVector.push_back(G.getNbCroisementDiffReel()); }
    totalInterIllVector.push_back(G.nombreInterIll + G.nombreInterIllSelf);
    debugValue = G.debugEverything();
    if ((isRecuit)&&(currentIteration==1)) { recuitCycleVague = G.recuitCycleVague; }
}

void LogGraphe::writeGrapheToFile(Graphe& G, int tid) {
    std::string outputFile = cheminOutputGraphs + nomGraphe + "-t" + to_string(tid) + "-r" + to_string(currentIteration) + ".json";
    if (!G.useCoordReel) { G.writeToJsonGraph(outputFile); }
    else { G.writeToJsonGraphReel(outputFile); }
}

void LogGraphe::writeResultToFile() {
	if (isRecuit) { writeRecuitResultToFile(); }
	else if (isGenetique) { writeGenetiqueResultToFile(); }
}

void LogGraphe::writeGenetiqueResultToFile() {
	//if (isGenetique) { resultats << std::setprecision(0) << "," << population << "," << maxIteration << "," << bestIterationMoyenne << "," << lastIterationMoyenne; } // GENETIQUE POPULATION, MAX ITERATION, BEST ITERATION, LAST ITERATION
}

void LogGraphe::writeRecuitResultToFile() {
    long medianScore;
	double moyenneCroisement, ecartTypeScore;
	double tempsExecMoyenne, tempsBestMoyenne, tempsPlacementMoyenne;
	double bestIterationMoyenne, lastIterationMoyenne, nombreRecuitMoyenne, totalInterMoyenne, totalInterIllMoyenne, edgeCostMoyenne, placementInterMoyenne;
	std::sort(scoreVector.begin(), scoreVector.end());
	long meilleurScore = scoreVector[0];
	moyenneCroisement = moyenneVector(scoreVector);
	medianScore = medianeVector(scoreVector);
	ecartTypeScore = ecartTypeVector(scoreVector,moyenneCroisement);
	tempsPlacementMoyenne = moyenneVector(tempsPlacementVector);
	tempsExecMoyenne = moyenneVector(tempsExecVector);
	tempsBestMoyenne = moyenneVector(tempsBestVector);
	bestIterationMoyenne = moyenneVector(bestIterationVector);
	lastIterationMoyenne = moyenneVector(lastIterationVector);
	nombreRecuitMoyenne = moyenneVector(nombreRecuitVector);
	totalInterMoyenne = moyenneVector(totalInterVector);
	totalInterIllMoyenne = moyenneVector(totalInterIllVector);
	edgeCostMoyenne = moyenneVector(edgeCostVector);
	placementInterMoyenne = moyenneVector(placementInterVector);

	std::string nomFichier = chemin + "/resultats/" + nomGraphe + to_string(tid) + "-" + to_string(nombreSlots) + ".csv";
	std::ofstream resultats(nomFichier, std::ios_base::app);

	char date[80];
#if defined(LINUX_OS)
	time_t t = time(0);
	strftime(date, 80, "%d/%m/%Y", localtime(&t));
#endif

	resultats << std::fixed;
	resultats << nomGraphe << ","; // NOM GRAPHE
	resultats << methodePlacementName << "," // ALGO PLACEMENT
		<< methodeAlgoName << "," // ALGO DEPLACEMENT
		<< std::setprecision(0) << scoreVector.size() << ","; // NOMBRE RUN
	if (nombreSlots > 0) { resultats << nombreSlots << ","; } // NOMBRE EMPLACEMENT OU FLOTTANTS
	else { resultats << "FLOTTANTS,"; } // NOMBRE EMPLACEMENT OU FLOTTANTS
	resultats << nbSolutionIllegale << "," // NOMBRE DE RUN AVEC RESULTATS ILLEGAL
		<< totalInterMoyenne << "," << totalInterIllMoyenne << "," // NOMBRE TOTAL INTER ET NOMBRE TOTAL INTER ILLEGAL
		<< meilleurScore << ","; // MEILLEUR SCORE PARMIS LES RUNS
	if (moyenneCroisement > 100) { resultats << std::setprecision(0) << moyenneCroisement << ","; } // MOYENNE SCORE DES RUNS
	else { resultats << std::setprecision(1) << moyenneCroisement << ","; } // MOYENNE SCORE DES RUNS
	if (medianScore > 100) { resultats << std::setprecision(0) << medianScore << ","; } // MEDIANE SCORE DES RUNS
	else { resultats << std::setprecision(1) << medianScore << ","; } // MEDIANE SCORE DES RUNS
	if (ecartTypeScore > 100) { resultats << std::setprecision(0) << ecartTypeScore << ","; } // ECART TYPE SCORE DES RUNS
	else { resultats << std::setprecision(1) << ecartTypeScore << ","; } // ECART TYPE SCORE DES RUNS
	resultats << placementInterMoyenne << ","; // MOYENNE INTERSECTION APRES PLACEMENT DES RUNS
	resultats << std::setprecision(0) << tempsPlacementMoyenne << "," << tempsBestMoyenne << "," << tempsExecMoyenne; // TEMPS MOYEN (PLACEMENT, MEILLEUR RESULTATS ALGO, TOTAL ALGO) DES RUNS
	resultats << "," << std::setprecision(1) << nombreRecuitMoyenne; // NOMBRE RECHAUFFE RECUIT
	resultats << ",";
	if (isSeedFixe()) { resultats << "F-"; } // SEED FIXE
	else { resultats << "R-"; } // SEED RESET
	resultats << getSeed(tid) << "," << machine; // SEED
	resultats << "," << getParamAsString(); // CUSTOM PARAMS
	resultats << "," << nombreCellule; // NOMBRE CELLULE
	resultats << "," << debugValue << "," << date; // DEBUG VALUE ET DATE
	if (edgeCostVector.size() > 0) { resultats << ",EC:" << edgeCostMoyenne; } // MOYENNE EDGE COST
	if ((isRecuit)&&(nombreRecuitMoyenne>1)) { resultats << "," << getRecuitCycleAsString(); } // CYCLE VAGUE RECUIT
	resultats << "\n";
	resultats.close();
}

std::string LogGraphe::getParamAsString() {
	std::stringstream paramStream;
	paramStream << std::fixed;
	paramStream << "{";
	for (std::vector<double>& param : customParam) {
		paramStream << "{";
		if (param.size() > 0) {
			for (int i=0;i<param.size();i++) {
				if (i==0) { paramStream << std::setprecision(0) << param[i]; }
				else { 
					if (param[i] > 1) { paramStream << std::setprecision(2) << param[i]; }
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

std::string LogGraphe::getRecuitCycleAsString() {
	std::stringstream paramStream;
	paramStream << std::fixed;
	paramStream << "{";
	for (int i=0;i<recuitCycleVague.size();i++) {
		paramStream << recuitCycleVague[i];
		if (i<recuitCycleVague.size()-1) { paramStream << "|"; }
	}
	paramStream << "}";
	return paramStream.str();
}

void LogGraphe::saveGrapheDistanceCSV(Graphe& G, int tid) {
	if (G.recuitDistanceAll.size() > 0) {
		std::string nomFichier = chemin + "/resultats/" + to_string(tid) + "-recuitDistAll.csv";
		std::ofstream resultats(nomFichier, std::ios_base::app);
		for (int i=0;i<G.recuitDistanceAll.size();i+=100) {
			resultats << i << "," << G.recuitDistanceAll[i] << std::endl;
		}
		resultats.close();
	}
	if (G.recuitDistanceUpgrade.size() > 0) {
		std::string nomFichier = chemin + "/resultats/" + to_string(tid) + "-recuitDistUp.csv";
		std::ofstream resultats(nomFichier, std::ios_base::app);
		for (int i=0;i<G.recuitDistanceUpgrade.size();i+=100) {
			resultats << G.recuitDistanceUpgrade[i].first << "," << G.recuitDistanceUpgrade[i].second << std::endl;
		}
		resultats.close();
	}
	if (G.recuitDistanceUpgradeGlobal.size() > 0) {
		std::string nomFichier = chemin + "/resultats/" + to_string(tid) + "-recuitDistUpG.csv";
		std::ofstream resultats(nomFichier, std::ios_base::app);
		for (int i=0;i<G.recuitDistanceUpgradeGlobal.size();i++) {
			resultats << G.recuitDistanceUpgradeGlobal[i].first << "," << G.recuitDistanceUpgradeGlobal[i].second << std::endl;
		}
		resultats.close();
	}
}

void LogGraphe::saveGrapheScoreTempsCSV(Graphe& G, int tid) {
	if (G.recuitScoreTemps.size() > 0) {
		std::string nomFichier = chemin + "/resultats/" + G.nomGraphe + to_string(tid) + "-recuitScoreTemps.csv";
		std::ofstream resultats(nomFichier, std::ios_base::app);
		for (int i=0;i<G.recuitScoreTemps.size();i++) {
			resultats << i << "," << G.recuitScoreTemps[i].first << "," << G.recuitScoreTemps[i].second << std::endl;
		}
		resultats.close();
	}
}

int LogGraphe::setTimeLimitRecuit() {
	if (customParam.size() > 0) {
        for (std::vector<double>& param : customParam) {
            if (param.size() > 0) {
                if (param[0] == 15) { return param[1]; }
            }
        }
    }
	return -1;
}

void LogGraphe::setupGrapheWithTypeFile(Graphe& G, std::string& typeFile, std::string& fileGraph, std::string& fileSlots) {
	if (typeFile == "JSON") {
        if (!G.useCoordReel) { G.setupGraphe(fileGraph,fileSlots); }
        else { G.setupGrapheReel(fileGraph); }
    }
    else if (typeFile == "GRAPHML") {
        G.readFromGraphmlGraph(fileGraph);
        if (!G.useCoordReel) {
            int nbNoeud = std::min((int)G._noeuds.size()*2,6000);
            G.generateGrid(nbNoeud,nbNoeud);
        }
    }
    else {
        std::cout << typeFile << " not supported.\n";
        exit(2);
    }
    G.fillCommonNodeVectors();
}

void LogGraphe::placementGraphe(Graphe& G) {
    if (methodePlacementName == "Glouton") { if (!G.useCoordReel) { G.glouton(); } }
    else if (methodePlacementName == "Glouton Revisite") { if (!G.useCoordReel) { G.gloutonRevisite(); } }
    else if (methodePlacementName == "Glouton Gravite") { if (!G.useCoordReel) { G.gloutonRevisiteGravite(); } }
    else if (methodePlacementName == "Glouton Voisin") { if (!G.useCoordReel) { G.gloutonRevisiteVoisin(); } }
    else if (methodePlacementName == "OGDF") { 
#if defined(OGDF_INSTALLED)
        if (!G.useCoordReel) { ogdfPlacementAuPlusProche(G); }
#else
        std::cout << "OGDF NOT INSTALLED.\n";
        return;
#endif
    }
    else if (methodePlacementName == "OGDFFMMM") {
#if defined(OGDF_INSTALLED)
        if (!G.useCoordReel) { ogdfFastMultipoleMultilevelEmbedder(G); } else { ogdfFastMultipoleMultilevelEmbedderReel(G); }
#else
        std::cout << "OGDF NOT INSTALLED.\n";
        return;
#endif			
    }
    else if (methodePlacementName == "OGDFFMMMM") {
#if defined(OGDF_INSTALLED)
        if (G.useCoordReel) { ogdfFastMultipoleMultilevelEmbedderReelMinute(G); }
#else
        std::cout << "OGDF NOT INSTALLED.\n";
        return;
#endif		
    }
    else if (methodePlacementName == "Stress") { if (!G.useCoordReel) { G.stressMajorization(customParam); } else { G.stressMajorizationReel(); } }
    else if (methodePlacementName == "Stress Dyn Stress") { if (!G.useCoordReel) { G.stressMajorization(customParam,1); } }
    else if (methodePlacementName == "Stress Dyn Cross") { if (!G.useCoordReel) { G.stressMajorization(customParam,2); } }
    else if (methodePlacementName == "Glouton Grille") { if (!G.useCoordReel) { G.gloutonRevisiteGrid(); } }
    else if (methodePlacementName == "Aleatoire") { if (!G.useCoordReel) { G.placementAleatoire(); } else { G.placementAleatoireReel(); } }
    else if (methodePlacementName != "Aucun") {
        std::cout << "ERROR Aucune methode " << methodePlacementName << " trouve !\n";
        return;
    }
    finPlacement = std::chrono::system_clock::now();
	sched_setaffinity(0, sizeof(cpuset), &cpuset); // Fix bug multithreading FMMM
    if (methodePlacementName != "Aucun") { if (!G.useCoordReel) { placementInterVector.push_back(G.getNbCroisementDiff()); } else { placementInterVector.push_back(G.getNbCroisementDiffReel()); } }
}

void LogGraphe::setupGrapheStruct(Graphe& G) {
    if (G.useCoordReel) { G.translateGrapheToOriginReel(-1); }
    if (updateScore) { G.initGraphAndNodeScoresAndCrossings(); }
    if (needTriangulation) { G.triangulationDelaunay(); }
    if (needGrille) { G.setupGridAndRegistration(customParam); }
}

void LogGraphe::heuristiqueGraphe(Graphe& G) {
    if (methodeAlgoName == "Recuit Simule") { if (!G.useCoordReel) { G.recuitSimule(tempsBest,start,customParam,0.99999,100.0,0.0001,1,0,0,false,false,timeLimitRecuit); } else { G.recuitSimuleReel(tempsBest,start,customParam,0.99999,100.0,0.0001,1,0,0,false,false,timeLimitRecuit); } }
    else if (methodeAlgoName == "Recuit Simule Grille") { if (!G.useCoordReel) { G.recuitSimule(tempsBest,start,customParam,0.99999,100.0,0.0001,1,0,0,true,false,timeLimitRecuit); } else { G.recuitSimuleReel(tempsBest,start,customParam,0.99999,100.0,0.0001,1,0,0,true,false,timeLimitRecuit); } }
    else if (methodeAlgoName == "Recuit Simule Grille TME") { if (!G.useCoordReel) { G.recuitSimule(tempsBest,start,customParam,0.99999,100.0,0.0001,1,0,2,true,false,timeLimitRecuit); } else { G.recuitSimuleReel(tempsBest,start,customParam,0.99999,100.0,0.0001,1,0,2,true,false,timeLimitRecuit); } }
    else if (methodeAlgoName == "Recuit Simule Grille BOX") { if (G.useCoordReel) { G.recuitSimuleReel(tempsBest,start,customParam,0.99999,100.0,0.0001,1,0,4,true,false,timeLimitRecuit); } }
    else if (methodeAlgoName == "Rerecuit Simule Grille TME")  { if (!G.useCoordReel) { G.rerecuitSimule(tempsBest,nombreRecuit,start,customParam); } else { G.rerecuitSimuleReel(tempsBest,nombreRecuit,start,customParam); } }
    else if (methodeAlgoName == "Rerecuit Simule Grille TME Opti")  { if (!G.useCoordReel) { G.rerecuitSimule(tempsBest,nombreRecuit,start,customParam,-1,0.99999,0.99,100.0,0.0001,1,0,2,true,false,timeLimitRecuit,true,true); } else { G.rerecuitSimuleReel(tempsBest,nombreRecuit,start,customParam,-1,0.99999,0.99,100.0,0.0001,1,0,2,true,false,timeLimitRecuit,true,true); } }
    else if (methodeAlgoName == "Rerecuit Simule Grille BOX") { if (G.useCoordReel) { G.rerecuitSimuleReel(tempsBest,nombreRecuit,start,customParam,-1,0.99999,0.99,100.0,0.0001,1,0,4,true,false); } }
    else if (methodeAlgoName == "Rerecuit Simule Grille TME Temp") G.rerecuitSimule(tempsBest,nombreRecuit,start,customParam,-1,0.99999,0.99,0.1);
    else if (methodeAlgoName == "Rerecuit Simule Grille TME Cool") if (!G.useCoordReel) { G.rerecuitSimule(tempsBest,nombreRecuit,start,customParam,-1,0.999999); } else { G.rerecuitSimuleReel(tempsBest,nombreRecuit,start,customParam,-1,0.999999); }
    else if (methodeAlgoName == "Rerecuit Simule Grille TME Cooler") G.rerecuitSimule(tempsBest,nombreRecuit,start,customParam,-1,0.9999999);
    else if (methodeAlgoName == "Rerecuit Simule Grille TME Cool Delay") G.rerecuitSimule(tempsBest,nombreRecuit,start,customParam,-1,0.999999,0.99,100.0,0.0001,2);
    else if (methodeAlgoName == "Rerecuit Simule Grille TME Cool Delay Temp") G.rerecuitSimule(tempsBest,nombreRecuit,start,customParam,-1,0.999999,0.99,0.1,0.0001,2);
    else if (methodeAlgoName == "Best Deplacement") G.bestDeplacement();
    else if (methodeAlgoName == "Genetique Recuit") G.grapheGenetique(tempsBest,bestIteration,lastIteration, genetiquePopulation, genetiqueMaxIteration, fileGraph, fileSlots, true);
    else if (methodeAlgoName == "Genetique Recuit Random") G.grapheGenetique(tempsBest,bestIteration,lastIteration, genetiquePopulation, genetiqueMaxIteration, fileGraph, fileSlots, true, true);
    else if (methodeAlgoName == "Genetique") G.grapheGenetique(tempsBest,bestIteration,lastIteration, genetiquePopulation, genetiqueMaxIteration, fileGraph, fileSlots);
    else if (methodeAlgoName == "Genetique Random") G.grapheGenetique(tempsBest,bestIteration,lastIteration, genetiquePopulation, genetiqueMaxIteration, fileGraph, fileSlots, false, true);
    else if (methodeAlgoName == "Genetique Score") G.grapheGenetique(tempsBest,bestIteration,lastIteration, genetiquePopulation,genetiqueMaxIteration,fileGraph,fileSlots,false,false,3);
    else if (methodeAlgoName == "Genetique Score Recuit") G.grapheGenetique(tempsBest,bestIteration,lastIteration, genetiquePopulation,genetiqueMaxIteration,fileGraph,fileSlots,true,false,3);
    else if (methodeAlgoName == "Genetique Enfant") G.grapheGenetique(tempsBest,bestIteration,lastIteration, genetiquePopulation,genetiqueMaxIteration,fileGraph,fileSlots,false,false,5);
    else if (methodeAlgoName == "Genetique Enfant Recuit") G.grapheGenetique(tempsBest,bestIteration,lastIteration, genetiquePopulation,genetiqueMaxIteration,fileGraph,fileSlots,true,false,5);
    else if (methodeAlgoName != "Aucun") {
        std::cout << "ERROR Aucun algo " << methodeAlgoName << " trouve !\n";
        return;
    }

    auto end = std::chrono::system_clock::now();
    saveGrapheDistanceCSV(G,tid);
    saveGrapheScoreTempsCSV(G,tid);
    std::chrono::duration<double> secondsTotal = end - start;
    std::chrono::duration<double> secondsPlacement = finPlacement - start;
    tempsExecVector.push_back(secondsTotal.count());
    tempsBestVector.push_back(tempsBest);
    tempsPlacementVector.push_back(secondsPlacement.count());
    bestIterationVector.push_back(bestIteration);
    lastIterationVector.push_back(lastIteration);
    nombreRecuitVector.push_back(nombreRecuit);
}

void generateCSV(int nbEssay, const std::string& methodePlacementName, const std::string& methodeAlgoName, std::string fileGraph, std::string fileSlots, std::vector<std::vector<double>> customParam={{}}, bool useReel=false, int tid=0, std::string typeFile="JSON", int timeLimit=3600) {
	LogGraphe LG(fileGraph,fileSlots,methodePlacementName,methodeAlgoName,customParam,tid);
	bool useTimeLimit = (timeLimit != -1);
	bool saveResult = true;
	auto totalStart = std::chrono::system_clock::now();
	std::chrono::duration<double> secondsTotalExec = totalStart - totalStart;
	for (;((((LG.currentIteration <= nbEssay)&&((!useTimeLimit)||(secondsTotalExec.count() < timeLimit)))||(nbEssay==-1&&secondsTotalExec.count() < timeLimit))&&(LG.currentIteration <= 100)); LG.currentIteration++) {
		resetSeed(tid);
		LG.start = std::chrono::system_clock::now();
		Graphe G(LG.nomGraphe);
		G.useCoordReel = useReel;

		LG.setupGrapheWithTypeFile(G,typeFile,fileGraph,fileSlots);
		printf("Tid: %d | Iter: %d Max: %d | %s | %s | %s | Slots: %lu | Nodes: %lu | TotalRun: %.1fs | Seed: %u | Param: %s\n",tid,LG.currentIteration,nbEssay,LG.nomGraphe.c_str(),methodePlacementName.c_str(),methodeAlgoName.c_str(),G._emplacements.size(),G._noeuds.size(),secondsTotalExec.count(),getSeed(tid),LG.getParamAsString().c_str());

		LG.placementGraphe(G); // Algorithme de placement initial

		LG.setupGrapheStruct(G); // Met en place les structures de cellules,triangulation,score ect...

        LG.heuristiqueGraphe(G); // Algorithme principal d'optimisation

        auto end = std::chrono::system_clock::now();
        secondsTotalExec = end - totalStart;

		LG.updateDataFromGraphe(G); // Récupération des données du graphes pour écriture dans le fichier csv
        LG.writeGrapheToFile(G,tid); // Sauvegarde le graphe au format json
		if (saveResult) { LG.writeResultToFile(); } // Sauvegarde les données dans un fichier csv
	}
}

#endif
