#ifndef NODISPRUN_HPP
#define NODISPRUN_HPP

#include <string>
#include <fstream>
#include "graphe.hpp"
#include "logCSV.hpp"
#include "personnel.hpp"
#include <filesystem>
#include <chrono>

void readOldFiles(Graphe& G) {
	std::string nomFichierGraph = "auto21-6";
	std::string nomFichier = "D:/The World/Cours/M2S1/ProjetDessinGraphe/GitHub/ProjetDessinDeGraphe/automatique/" + nomFichierGraph + ".json";
	//string nomFichier = "D:/The World/Cours/M2S1/ProjetDessinGraphe/GitHub/ProjetDessinDeGraphe/2019/" + nomFichierGraph + ".json";
	std::vector<int> tmpvec = G.readFromJsonOldGraph(nomFichier);
	std::string nomFichierSlots = nomFichierGraph + "-slots.json";
	std::string nomFichierGrapheSave = "graph-" + nomFichierGraph + ".json";
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
		G.writeToJsonSlots(nomFichierSlots);
		//writeToJsonGraph(G, nomFichierGrapheSave);
	}
}

void startRunsForAllSlots(std::pair<std::string, std::vector<std::string>>& pairGraphSlots, int nbRun, std::string methodePlacement, std::string methodeAlgo,std::vector<double> customParam, int tid) {
	for (int i = 0; i < pairGraphSlots.second.size(); i++) {
		generateCSV(nbRun, methodePlacement, methodeAlgo, pairGraphSlots.first, pairGraphSlots.first, pairGraphSlots.second[i],customParam,tid);
	}
}

void customRecuit() {
	fillLogsVector();
	std::string nomFichierGraph = "graph-10-input";
	std::vector<std::string> slotFiles = { "10-input-slots", "2X-10-input-slots", "3X-10-input-slots", "GRID" };
	int nthreads, tid;
#pragma omp parallel private(tid)
	{
		tid = ::omp_get_thread_num();
		nthreads = ::omp_get_num_threads();
		if (tid < slotFiles.size()) {
		//if (tid < 1) {
			if (tid == 0) {
				printf("Number of threads working on training data: %d\n", nthreads);
			}
			std::vector<std::vector<double>> totalRuns;
			totalRuns.push_back({8,3});
			totalRuns.push_back({8,4});
			totalRuns.push_back({8,5});
			totalRuns.push_back({8,6});
			totalRuns.push_back({8,7});
			totalRuns.push_back({8,8});
			totalRuns.push_back({8,9});
			totalRuns.push_back({8,10});
			totalRuns.push_back({8,11});
			totalRuns.push_back({8,12});
			totalRuns.push_back({8,13});
			totalRuns.push_back({8,14});
			totalRuns.push_back({8,15});
			for (int i=0;i<totalRuns.size();i++) {
				generateCSV(10,"Aleatoire","Recuit Simule Grille TRE Custom","graph-10-input",nomFichierGraph,slotFiles[tid],totalRuns[i],tid);
			}
		}
		printf("Thread: %d done.\n",tid);
	}
	printf("All Threads done.\n");
}

void customRecuitAllRuns() {
	fillLogsVector();
	std::vector<std::pair<std::string, std::vector<std::string>>> mapGraphSlots;
	for (int i = 1; i <= 12; i++) {
		mapGraphSlots.push_back({ "graph-" + std::to_string(i) + "-input",{std::to_string(i) + "-input-slots", "2X-" + std::to_string(i) + "-input-slots", "3X-" + std::to_string(i) + "-input-slots", "GRID"} });
	}
	int nthreads, tid;
#pragma omp parallel private(tid)
	{
		int indexKey = 0;
		tid = ::omp_get_thread_num();
		nthreads = ::omp_get_num_threads();
		if (tid == 0) {
			printf("Number of threads working on training data: %d\n", nthreads);
		}
		for (auto &key : mapGraphSlots) {
			if (tid == (indexKey % nthreads)) {
				for (int numSlot = 0; numSlot < key.second.size(); numSlot++) {
					std::vector<std::vector<double>> totalRuns;
					totalRuns.push_back({10,1});
					totalRuns.push_back({11,1});
					totalRuns.push_back({10,0.5});
					totalRuns.push_back({11,0.5});
					totalRuns.push_back({10,1.5});
					totalRuns.push_back({11,1.5});
					totalRuns.push_back({10,0.8});
					totalRuns.push_back({11,0.8});
					totalRuns.push_back({10,1.2});
					totalRuns.push_back({11,1.2});
					totalRuns.push_back({10,2});
					totalRuns.push_back({11,2});
					std::string nomFichierLog = key.first;
					for (int i=0;i<totalRuns.size();i++) {
						generateCSV(2,"Aleatoire","Recuit Simule Grille TME",nomFichierLog,key.first,key.second[numSlot],totalRuns[i],tid);
					}
				}
			}
			indexKey++;
		}
		printf("Thread: %d done.\n",tid);
	}
	printf("All Threads done.\n");
}

void allRunsSingleThread() {
	fillLogsVector();
	std::vector<std::pair<std::string, std::vector<std::string>>> mapGraphSlots;
	std::vector<std::string> methodesPlacement = { "Aleatoire" };
	std::vector<std::string> methodesAlgo = { "Rerecuit Simule TME" };
	for (int i = 1; i <= 10; i++) {
		mapGraphSlots.push_back({ "graph-" + std::to_string(i) + "-input",{std::to_string(i) + "-input-slots", "2X-" + std::to_string(i) + "-input-slots", "3X-" + std::to_string(i) + "-input-slots", "GRID"} });
	}
	int nbRuns = 5;
	std::cout << "Starting all run logs, nb run: " << nbRuns << std::endl;
	
	for (auto &key : mapGraphSlots) {
		for (int i = 0; i < key.second.size(); i++) {
			std::cout << "-----------------------------------------" << std::endl;
			std::cout << "Graphe: " << key.first << " " << key.second[i] << std::endl;
			for (int j = 0; j < methodesPlacement.size(); j++) {
				std::cout << "--------------------------" << std::endl;
				for (int k = 0; k < methodesAlgo.size(); k++) {
					std::cout << "Placement: " << methodesPlacement[j] << " Algo: " << methodesAlgo[k] << std::endl;
					generateCSV(nbRuns, methodesPlacement[j], methodesAlgo[k], key.first, key.first, key.second[i]);
				}
			}
		}
	}
}

// Multithreadé
void allRunsLogged() {
	fillLogsVector();
	std::unordered_map<std::string, std::vector<std::string>> mapGraphSlots;
	std::vector<std::string> methodesPlacement = { "Aleatoire"};
	std::vector<std::string> methodesAlgo = { "Rerecuit Simule Grille Best"};
	std::vector<int> runMethodesAlgo = { 1,1,1,1 };
	for (int i = 1; i <= 12; i++) {
		mapGraphSlots.insert({ "graph-" + std::to_string(i) + "-input",{std::to_string(i) + "-input-slots","2X-" + std::to_string(i) + "-input-slots","3X-" + std::to_string(i) + "-input-slots","GRID"} });
	}
	std::cout << "Starting all run logs." << std::endl;
	int nthreads, tid;
#pragma omp parallel private(tid)
	{
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
					for (int j = 0; j < methodesPlacement.size(); j++) {
						for (int k = 0; k < methodesAlgo.size(); k++) {
							generateCSV(runMethodesAlgo[k], methodesPlacement[j], methodesAlgo[k], key.first, key.first, key.second[i],{},tid);
						}
					}
				}
			}
			indexKey++;
		}
		printf("Thread: %d done.\n",tid);
	}
	printf("All Threads done.\n");
}

void allRunsBySlots() {
	fillLogsVector();
	std::vector<std::pair<std::string, std::vector<std::string>>> mapGraphSlots;
	for (int i = 1; i <= 12; i++) {
		mapGraphSlots.push_back({"graph-" + std::to_string(i) + "-input",{std::to_string(i) + "-input-slots","2X-" + std::to_string(i) + "-input-slots","3X-" + std::to_string(i) + "-input-slots","GRID"}});
	}
	std::cout << "Starting all run logs." << std::endl;
	int nthreads, tid;
#pragma omp parallel private(tid)
	{
		tid = ::omp_get_thread_num();
		nthreads = ::omp_get_num_threads();
		int chunk = std::ceil((double)mapGraphSlots.size() / (double)nthreads);
		if (tid == 0) {
			printf("Number of threads working on training data: %d\n", nthreads);
			printf("Chunk size: %d\n", chunk);
		}
		int indexKey = 0;
		std::vector<std::vector<double>> totalRuns;
		totalRuns.push_back({3,2});
		totalRuns.push_back({3,3});
		totalRuns.push_back({3,4});
		totalRuns.push_back({3,5});
		totalRuns.push_back({3,6});
		totalRuns.push_back({3,7});
		totalRuns.push_back({3,8});
		totalRuns.push_back({3,9});
		totalRuns.push_back({3,10});
		for (auto& key : mapGraphSlots) {
			if (tid == (indexKey % nthreads)) {
				for (int taille=0;taille<totalRuns.size();taille++) {
					startRunsForAllSlots(key,1,"Aleatoire","Rerecuit Simule Grille TME Custom",totalRuns[taille],tid);
					startRunsForAllSlots(key,1,"Aleatoire","Rerecuit Simule Grille TRE Custom",totalRuns[taille],tid);
				}
			}
			indexKey++;
		}
		std::vector<std::vector<double>> totalRuns2;
		totalRuns2.push_back({12,0});
		totalRuns2.push_back({12,1});
		indexKey = 0;
		for (auto& key : mapGraphSlots) {
			if (tid == (indexKey % nthreads)) {
				for (int taille=0;taille<totalRuns2.size();taille++) {
					startRunsForAllSlots(key,10,"Aleatoire","Rerecuit Simule Grille TME Custom",totalRuns2[taille],tid);
				}
			}
			indexKey++;
		}
		printf("Thread: %d done.\n",tid);
	}
	printf("All Threads done.\n");
}

// Multithreading sur un seul graphe pour differentes methodes
void specificGraphMulti(std::string fileGraph, std::string fileSlots, bool useSingleFile=false) {
	int nthreads, tid;
	int nbExec = 2;
	std::vector<std::string> methodesPlacement = { "Aleatoire", "Aucun" };
	std::vector<std::string> methodesAlgo = { "Recuit Simule", "Genetique Score" };
#pragma omp parallel private(tid)
	{
		tid = ::omp_get_thread_num();
		nthreads = ::omp_get_num_threads();
		if (tid < nbExec) {
			Graphe G;
			if (!useSingleFile) {
				G.readFromJsonGraph(fileGraph);
				G.readFromJsonSlots(fileSlots);
			}
			else { 
				G.readFromJsonGraphAndSlot(fileGraph);
			}
			if (tid == 0) {
				printf("Number max of threads working on training data: %d\n", nthreads);
			}
			std::string nomFichierLog = "Graph-" + std::to_string(tid);
			//generateCSV(1, methodesPlacement[tid], methodesAlgo[tid], nomFichierLog, G, fileGraph, fileSlots, true);
		}
		std::cout << "Thread: " << tid << " done." << std::endl;
	}
	printf("All Threads done.\n");
}

void performanceTest() {
	int nombreMax = 1000;

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

		std::string nomFichier = chemin + "/resultats/perfUSet.csv";
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

void performanceTest2() {
		int nombreElem = 5000;
		// Vecteur d'id melange
		std::vector<int> shuffledIdVec;
		for (int i=0;i<nombreElem;i++) {
			shuffledIdVec.push_back(i);
		}
		auto rng = std::default_random_engine {};
		std::shuffle(std::begin(shuffledIdVec), std::end(shuffledIdVec), rng);

		// Vecteur d'id melange
		std::vector<int> shuffledIdVec2;
		for (int i=0;i<nombreElem;i++) {
			shuffledIdVec2.push_back(i);
		}
		std::shuffle(std::begin(shuffledIdVec2), std::end(shuffledIdVec2), rng);

		// Vecteur d'id genere au random
		std::vector<int> randomIdVec;
		for (int i=0;i<30000;i++) {
			randomIdVec.push_back(generateRand((nombreElem*2)-1));
		}


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

		std::string nomFichier = chemin + "/resultats/perfUSet.csv";
		std::ofstream resultats(nomFichier, std::ios_base::app);

		resultats << std::fixed;
		resultats << std::setprecision(7) << nombreElem << ","
			<< insertTime.count() << ","
			<< eraseTime.count() << ","
			<< findTime.count() << ","
			<< totalTime.count() << "\n";
		resultats.close();
	// Partie Vector
	std::vector<int> nodeVec;
}

void testRomeGraphs() {
	std::string path = chemin + "rome/";
		int nthreads, tid;
#pragma omp parallel private(tid)
	{
		int i=0;
		tid = ::omp_get_thread_num();
		nthreads = ::omp_get_num_threads();
		if (tid == 0) {
			printf("Number of threads working on training data: %d\n", nthreads);
		}
		for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(path)) {
			if (i%nthreads == tid) {
				printf("Tid: %d | i: %d\n",tid,i);
				Graphe G;
				G.readFromGraphmlGraph(dirEntry.path());
				auto start = std::chrono::system_clock::now();
				double tempsBest = -1; int bestIteration = -1; int lastIteration = -1; int nombreRecuit=0; 
				G.generateGrid();
				G.placementAleatoire();
				G.initGrille();
				G.registerSlotsAndEdgesInGrid();
				G.recuitSimuleGrid(tempsBest,0.99999, 100.0,0.0001, 1, 0, 3);
				auto end = std::chrono::system_clock::now();
				std::chrono::duration<double> secondsTotal = end - start;
				std::string nomFichier = chemin + "resultatsRome/" + to_string(tid) + ".csv";
				std::ofstream resultats(nomFichier, std::ios_base::app);
				resultats << dirEntry.path() << "," << to_string(i) << "," << G.nombreCroisement << "," << tempsBest << "," << secondsTotal.count() << std::endl;
				resultats.close();
			}
			i++;
		}
		printf("Thread %d done.\n",tid);
	}

}

#endif