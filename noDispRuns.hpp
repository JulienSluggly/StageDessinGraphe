#ifndef NODISPRUN_HPP
#define NODISPRUN_HPP

#include <string>
#include "logCSV.hpp"
#include "personnel.hpp"

void readOldFiles(Graphe& G) {
	string nomFichierGraph = "auto21-6";
	string nomFichier = "D:/The World/Cours/M2S1/ProjetDessinGraphe/GitHub/ProjetDessinDeGraphe/automatique/" + nomFichierGraph + ".json";
	//string nomFichier = "D:/The World/Cours/M2S1/ProjetDessinGraphe/GitHub/ProjetDessinDeGraphe/2019/" + nomFichierGraph + ".json";
	std::vector<int> tmpvec = readFromJsonOldGraph(nomFichier, G);
	string nomFichierSlots = nomFichierGraph + "-slots.json";
	string nomFichierGrapheSave = "graph-" + nomFichierGraph + ".json";
	bool save = false;
	if (save) {
		bool savex2 = false;
		bool savex3 = false;
		if (savex2) {
			G.generateMoreEmplacement(2);
			nomFichierSlots = "2X-" + nomFichierSlots;
		}
		else if (savex3) {
			G.generateMoreEmplacement(3);
			nomFichierSlots = "3X-" + nomFichierSlots;
		}
		writeToJsonSlots(G, nomFichierSlots);
		//writeToJsonGraph(G, nomFichierGrapheSave);
	}
}

void allRunsSingleThread() {
	std::map<string, std::vector<string>> mapGraphSlots;
	std::vector<string> methodesPlacement = { "Aleatoire" };
	//std::vector<string> methodesPlacement = { "OGDF" };
	std::vector<string> methodesAlgo = { "Recuit Simule",  "Recuit Simule Score",  "Genetique Score",  "Genetique Score Recuit" };
	std::vector<bool> useScore = { false, true, true, true };
	for (int i = 1; i <= 12; i++) {
		mapGraphSlots.insert({ "graph-" + std::to_string(i) + "-input",{std::to_string(i) + "-input-slots"} });
	}
	int nbRuns = 1;
	std::cout << "Starting all run logs, nb run: " << nbRuns << std::endl;
	Graphe G;
	for (auto key : mapGraphSlots) {
		for (int i = 0; i < key.second.size(); i++) {
			std::cout << "-----------------------------------------" << std::endl;
			std::cout << "Graphe: " << key.first << " " << key.second[i] << std::endl;
			G.clearGraphe();
			string fileGraph = chemin + "exemple/Graphe/" + key.first + ".json";
			string fileSlots = chemin + "exemple/Slots/" + key.second[i] + ".json";
			readFromJsonGraph(G, fileGraph);
			readFromJsonSlots(G, fileSlots);
			string nomFichierLog = key.first;
			for (int j = 0; j < methodesPlacement.size(); j++) {
				std::cout << "--------------------------" << std::endl;
				for (int k = 0; k < methodesAlgo.size(); k++) {
					std::cout << "Placement: " << methodesPlacement[j] << " Algo: " << methodesAlgo[k] << std::endl;
					generateCSV(nbRuns, methodesPlacement[j], methodesAlgo[k], nomFichierLog, G, fileGraph, fileSlots, useScore[k]);
				}
			}
		}
	}
}

// Multithreadé
void allRunsLogged() {
	std::unordered_map<string, std::vector<string>> mapGraphSlots;
	//std::vector<string> methodesPlacement = { "Aleatoire", "Glouton", "Glouton Revisite", "Glouton Gravite", "Glouton Voisin", "OGDF"};
	std::vector<string> methodesPlacement = { "Aucun"};
	std::vector<string> methodesAlgo = { "Genetique No Recuit Random", "Genetique Recuit Random","Genetique No Recuit","Genetique Recuit"};
	//std::vector<int> runMethodesAlgo = { 5, 1, 5, 5, 5,5, 1, 5, 5,5, 1, 5, 5 };
	std::vector<int> runMethodesAlgo = { 5,5,5,5 };
	//std::vector<string> methodesAlgo;
	for (int i = 1; i <= 12; i++) {
		mapGraphSlots.insert({ "graph-" + std::to_string(i) + "-input",{std::to_string(i) + "-input-slots","2X-" + std::to_string(i) + "-input-slots","3X-" + std::to_string(i) + "-input-slots"} });
	}
	std::cout << "Starting all run logs." << std::endl;
	int nthreads, tid;
#pragma omp parallel private(tid)
	{
		Graphe G;
		int indexKey = 0;
		tid = ::omp_get_thread_num();
		nthreads = ::omp_get_num_threads();
		int chunk = std::ceil((double)mapGraphSlots.size() / (double)nthreads);
		if (tid == 0) {
			printf("Number of threads working on training data: %d\n", nthreads);
			printf("Chunk size: %d\n", chunk);
		}
		for (auto key : mapGraphSlots) {
			if (tid == (indexKey % nthreads)) {
				for (int i = 0; i < key.second.size(); i++) {
					if (tid == 0) std::cout << "-----------------------------------------" << std::endl;
					if (tid == 0) std::cout << "Graphe: " << key.first << " " << key.second[i] << std::endl;
					G.clearGraphe();
					string fileGraph = chemin + "exemple/Graphe/" + key.first + ".json";
					string fileSlots = chemin + "exemple/Slots/" + key.second[i] + ".json";
					readFromJsonGraph(G, fileGraph);
					readFromJsonSlots(G, fileSlots);
					string nomFichierLog = key.first;
					for (int j = 0; j < methodesPlacement.size(); j++) {
						if (tid == 0) std::cout << "--------------------------" << std::endl;
						if (tid == 0) std::cout << "Graphe: " << key.first << " Slots: " << key.second[i] << std::endl;
						if (tid == 0) std::cout << "Placement: " << methodesPlacement[j] << " Algo: Aucun" << std::endl;
						//generateCSV(1, methodesPlacement[j], "Aucun", nomFichierLog, G);
						for (int k = 0; k < methodesAlgo.size(); k++) {
							if (tid == 0) std::cout << "Placement: " << methodesPlacement[j] << " Algo: " << methodesAlgo[k] << std::endl;
							generateCSV(runMethodesAlgo[k], methodesPlacement[j], methodesAlgo[k], nomFichierLog, G, fileGraph, fileSlots);
						}
					}
				}
			}
			indexKey++;
		}
		std::cout << "Thread: " << tid << " done." << std::endl;
	}
}

// Multithreading sur un seul graphe pour differentes methodes
void specificGraphMulti(std::string fileGraph, std::string fileSlots, bool useSingleFile=false) {
	int nthreads, tid;
	int nbExec = 2;
	std::vector<string> methodesPlacement = { "Aleatoire", "Aucun" };
	std::vector<string> methodesAlgo = { "Recuit Simule", "Genetique Score" };
#pragma omp parallel private(tid)
	{
		tid = ::omp_get_thread_num();
		nthreads = ::omp_get_num_threads();
		if (tid < nbExec) {
			Graphe G;
			if (!useSingleFile) {
				readFromJsonGraph(G, fileGraph);
				readFromJsonSlots(G, fileSlots);
			}
			else { 
				readFromJsonGraphAndSlot(G,fileGraph);
			}
			if (tid == 0) {
				printf("Number max of threads working on training data: %d\n", nthreads);
			}
			string nomFichierLog = "Graph-" + std::to_string(tid);
			//generateCSV(1, methodesPlacement[tid], methodesAlgo[tid], nomFichierLog, G, fileGraph, fileSlots, true);
		}
		std::cout << "Thread: " << tid << " done." << std::endl;
	}
}

void performanceTest() {
	int nombreMax = 100;

	for (int n=1;n<nombreMax;n++) {

		// Vecteur d'id melange
		std::vector<int> shuffledIdVec;
		for (int i=0;i<5000*n;i++) {
			shuffledIdVec.push_back(i);
		}
		auto rng = std::default_random_engine {};
		std::shuffle(std::begin(shuffledIdVec), std::end(shuffledIdVec), rng);

		// Vecteur d'id melange
		std::vector<int> shuffledIdVec2;
		for (int i=0;i<5000*n;i++) {
			shuffledIdVec2.push_back(i);
		}
		std::shuffle(std::begin(shuffledIdVec2), std::end(shuffledIdVec2), rng);

		// Vecteur d'id genere au random
		std::vector<int> randomIdVec;
		for (int i=0;i<30000;i++) {
			randomIdVec.push_back(generateRand((5000*n)-1));
		}


		// Partie unordered set
		int nombreElem = n*50;
		std::cout << "Nombre element: " << nombreElem << std::endl;
		auto start = std::chrono::system_clock::now();
		std::unordered_set<int> nodeUSet;
		for (int i=0;i<nombreElem;i++) {
			nodeUSet.insert(shuffledIdVec[i]);
		}
		auto s1 = std::chrono::system_clock::now();
		for (int i=0;i<30000;i++) {
			nodeUSet.count(randomIdVec[i]);
		}
		auto s2 = std::chrono::system_clock::now();
		for (int i=0;i<nombreElem;i++) {
				nodeUSet.erase(shuffledIdVec2[i]);
		}
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> totalTime = end - start;
		std::chrono::duration<double> insertTime = s1-start;
		std::chrono::duration<double> findTime = s2-s1;
		std::chrono::duration<double> eraseTime = end-s2;
		std::cout << "Insert: " << insertTime.count() << "s.\n";
		std::cout << "Find: " << findTime.count() << "s.\n";
		std::cout << "Erase: " << eraseTime.count() << "s.\n";
		std::cout << "Total: " << totalTime.count() << "s.\n";
		std::cout << "-----------------------------------------------\n";

		string nomFichier = chemin + "/resultats/perfUSet.csv";
		std::ofstream resultats(nomFichier, std::ios_base::app);

		resultats << std::fixed;
		resultats << std::setprecision(7) << nombreElem << ","
			<< insertTime.count() << ","
			<< eraseTime.count() << ","
			<< findTime.count() << ","
			<< totalTime.count() << "\n";
		resultats.close();
	}
	// Partie Vector
	std::vector<int> nodeVec;
}

#endif