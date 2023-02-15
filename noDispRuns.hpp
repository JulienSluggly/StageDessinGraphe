#ifndef NODISPRUN_HPP
#define NODISPRUN_HPP

#include <string>
#include <fstream>
#include "graphe.hpp"
#include "logCSV.hpp"
#include "personnel.hpp"

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

void customRecuit() {
	fillLogsVector();
	std::string nomFichierGraph = "graph-10-input";
	std::string fileGraph = chemin + "exemple/Graphe/" + nomFichierGraph + ".json";
	std::vector<std::string> slotFiles = { "10-input-slots", "2X-10-input-slots", "3X-10-input-slots", "GRID" };
	for (auto &fileSlot : slotFiles) {
		Graphe G;
		G.readFromJsonGraph(fileGraph);
		std::string slotFileUsed;
		if (fileSlot != "GRID") {
			slotFileUsed = chemin + "exemple/Slots/" + fileSlot + ".json";
			G.readFromJsonSlots(slotFileUsed);
		}
		else {
			slotFileUsed = "GRID";
			G.generateGrid();
		}
		std::vector<std::vector<int>> totalRuns;
		totalRuns.push_back({3,2});
		totalRuns.push_back({3,3});
		totalRuns.push_back({3,4});
		totalRuns.push_back({3,5});
		totalRuns.push_back({3,6});
		totalRuns.push_back({3,7});
		totalRuns.push_back({3,8});
		totalRuns.push_back({3,9});
		totalRuns.push_back({3,10});

		totalRuns.push_back({4,10});
		totalRuns.push_back({4,15});
		totalRuns.push_back({4,20});
		totalRuns.push_back({4,25});
		totalRuns.push_back({4,30});
		totalRuns.push_back({4,35});
		totalRuns.push_back({4,40});
		totalRuns.push_back({4,45});
		totalRuns.push_back({4,50});

		totalRuns.push_back({5,1});
		totalRuns.push_back({5,2});
		totalRuns.push_back({5,3});
		totalRuns.push_back({5,4});
		totalRuns.push_back({5,5});

		totalRuns.push_back({6,1});
		totalRuns.push_back({6,2});
		totalRuns.push_back({6,3});
		totalRuns.push_back({6,4});
		totalRuns.push_back({6,5});
		for (int i=0;i<totalRuns.size();i++) {
			generateCSV(10,"Aleatoire","Recuit Simule Delay TME Custom","graph-10-input",G,"",slotFileUsed,totalRuns[i]);
		}
		for (int i=0;i<totalRuns.size();i++) {
			generateCSV(10,"Aleatoire","Rerecuit Simule Delay TME Custom","graph-10-input",G,"",slotFileUsed,totalRuns[i]);
		}
		totalRuns.clear();
		totalRuns.push_back({7,1});
		totalRuns.push_back({7,2});
		totalRuns.push_back({7,3});
		totalRuns.push_back({7,4});
		totalRuns.push_back({7,5});
		totalRuns.push_back({7,6});
		totalRuns.push_back({7,7});
		totalRuns.push_back({7,8});
		totalRuns.push_back({7,9});
		totalRuns.push_back({7,10});

		totalRuns.push_back({8,1});
		totalRuns.push_back({8,2});
		totalRuns.push_back({8,3});
		totalRuns.push_back({8,4});
		totalRuns.push_back({8,5});

		totalRuns.push_back({9,1});
		totalRuns.push_back({9,2});
		totalRuns.push_back({9,3});
		totalRuns.push_back({9,4});
		totalRuns.push_back({9,5});
		for (int i=0;i<totalRuns.size();i++) {
			generateCSV(10,"Aleatoire","Recuit Simule TRE Custom","graph-10-input",G,"",slotFileUsed,totalRuns[i]);
		}
		for (int i=0;i<totalRuns.size();i++) {
			generateCSV(10,"Aleatoire","Rerecuit Simule TRE Custom","graph-10-input",G,"",slotFileUsed,totalRuns[i]);
		}
	}
}

void allRunsSingleThread() {
	fillLogsVector();
	std::vector<std::pair<std::string, std::vector<std::string>>> mapGraphSlots;
	std::vector<std::string> methodesPlacement = { "Aleatoire" };
	//std::vector<string> methodesPlacement = { "OGDF" };
	std::vector<std::string> methodesAlgo = { "Rerecuit Simule TME", "Recuit Simule Delay TME", "Rerecuit Simule Delay TME", "Recuit Simule TRE", "Rerecuit Simule TRE" };
	for (int i = 1; i <= 10; i++) {
		//mapGraphSlots.push_back({ "graph-" + std::to_string(i) + "-input",{std::to_string(i) + "-input-slots"} });
		mapGraphSlots.push_back({ "graph-" + std::to_string(i) + "-input",{std::to_string(i) + "-input-slots", "2X-" + std::to_string(i) + "-input-slots", "3X-" + std::to_string(i) + "-input-slots", "GRID"} });
	}
	int nbRuns = 10;
	std::cout << "Starting all run logs, nb run: " << nbRuns << std::endl;
	
	for (auto &key : mapGraphSlots) {
		for (int i = 0; i < key.second.size(); i++) {
			std::cout << "-----------------------------------------" << std::endl;
			std::cout << "Graphe: " << key.first << " " << key.second[i] << std::endl;
			Graphe G;
			std::string fileGraph = chemin + "exemple/Graphe/" + key.first + ".json";
			G.readFromJsonGraph(fileGraph);
			std::string fileSlots;
			if (key.second[i] != "GRID") {
				fileSlots = chemin + "exemple/Slots/" + key.second[i] + ".json";
				G.readFromJsonSlots(fileSlots);
			}
			else {
				fileSlots = "GRID";
				G.generateGrid();
			}
			std::string nomFichierLog = key.first;
			for (int j = 0; j < methodesPlacement.size(); j++) {
				std::cout << "--------------------------" << std::endl;
				for (int k = 0; k < methodesAlgo.size(); k++) {
					std::cout << "Placement: " << methodesPlacement[j] << " Algo: " << methodesAlgo[k] << std::endl;
					generateCSV(nbRuns, methodesPlacement[j], methodesAlgo[k], nomFichierLog, G, fileGraph, fileSlots);
				}
			}
		}
	}
}

// Multithreadé
void allRunsLogged() {
	std::unordered_map<std::string, std::vector<std::string>> mapGraphSlots;
	//std::vector<string> methodesPlacement = { "Aleatoire", "Glouton", "Glouton Revisite", "Glouton Gravite", "Glouton Voisin", "OGDF"};
	std::vector<std::string> methodesPlacement = { "Aucun"};
	std::vector<std::string> methodesAlgo = { "Genetique No Recuit Random", "Genetique Recuit Random","Genetique No Recuit","Genetique Recuit"};
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
					std::string fileGraph = chemin + "exemple/Graphe/" + key.first + ".json";
					std::string fileSlots = chemin + "exemple/Slots/" + key.second[i] + ".json";
					G.readFromJsonGraph(fileGraph);
					G.readFromJsonSlots(fileSlots);
					std::string nomFichierLog = key.first;
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

#endif