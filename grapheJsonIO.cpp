#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include "ogdfFunctions.hpp"
#include "graphe.hpp"
#include <unordered_map>
#include <set>
#include <map>

using nlohmann::json;

bool DEBUG_JSON = false;

// ----- CREATION D'UN Graph A PARTIR D'UN FICHIER JSON -----
// Precondition: Les id des noeuds sont ordonnees et commencent par 0
void Graphe::readFromJsonGraph(std::string input) {
	if (DEBUG_JSON) tcout() << "Fichier Graphe: " << input << std::endl;
	std::ifstream inp(input);
	json j;
	inp >> j;

	// Si le fichier ne contient pas de node
	if (j["nodes"] == nullptr) {
		exit(1);
	}
	int nodeNumber = static_cast<int>(j["nodes"].size());
	if (DEBUG_JSON) tcout() << "Nombre de noeud dans le json: " << nodeNumber << std::endl;
	_noeuds.reserve(nodeNumber*2);
	for (int i = 0; i < nodeNumber; i++) {
		_noeuds.push_back(Noeud(i));
	}
	int edgeNumber = static_cast<int>(j["edges"].size());
	if (DEBUG_JSON) tcout() << "Nombre d'arete dans le json: " << edgeNumber << std::endl;
	int id1, id2;
	_aretes.reserve(edgeNumber*2);
	for (int i = 0; i < edgeNumber; i++) {
		id1 = j["edges"][i]["source"];
		id2 = j["edges"][i]["target"];
		_aretes.push_back(Aretes(&_noeuds[id1], &_noeuds[id2],i));
	}
	updatePenalite(_aretes.size(),_aretes.size());
	updateIsolatedNodes();
}

void Graphe::readFromJsonGraphReel(std::string input) {
	if (DEBUG_JSON) tcout() << "Fichier Graphe: " << input << std::endl;
	std::ifstream inp(input);
	json j;
	inp >> j;

	// Si le fichier ne contient pas de node
	if (j["nodes"] == nullptr) {
		exit(1);
	}

	int nodeNumber = static_cast<int>(j["nodes"].size());
	if (DEBUG_JSON) tcout() << "Nombre de noeud dans le json: " << nodeNumber << std::endl;
	int id;
	_noeuds.reserve(nodeNumber*2);
	for (int i = 0; i < nodeNumber; i++) {
		_noeuds.push_back(Noeud(i));
		_noeuds[i]._xreel = j["nodes"][i]["x"];
		_noeuds[i]._yreel = j["nodes"][i]["y"];
	}

	int edgeNumber = static_cast<int>(j["edges"].size());
	if (DEBUG_JSON) tcout() << "Nombre d'arete dans le json: " << edgeNumber << std::endl;
	int id1, id2;
	_aretes.reserve(edgeNumber*2);
	for (int i = 0; i < edgeNumber; i++) {
		id1 = j["edges"][i]["source"];
		id2 = j["edges"][i]["target"];
		_aretes.push_back(Aretes(&_noeuds[id1], &_noeuds[id2],i));
	}
	updatePenalite(_aretes.size(),_aretes.size());
	updateIsolatedNodes();
}

void Graphe::readQuickCrossGraph(std::string input) {
	std::ifstream infile(input);
	std::string line;
	int numeroLigne = 0;
	int firstNodeId, secondNodeId;
	int maxNodeId = getMaxNodeIdFromFile(input);
	int idArete = 0;
	_noeuds.reserve((maxNodeId+1)*2);
	for (int i=0;i<=maxNodeId;i++) {
		_noeuds.push_back(Noeud(i));
	}
	while (std::getline(infile, line)) {
    	std::istringstream iss(line);
		if (numeroLigne != 0) {
			std::string subs;
			iss >> subs;
			firstNodeId = stoi(subs)-1;
			iss >> subs;
			secondNodeId = stoi(subs)-1;
			_aretes.push_back(Aretes(&_noeuds[firstNodeId],&_noeuds[secondNodeId],idArete));
			idArete++;
		}
		numeroLigne++;
	}
	infile.close();
}

void Graphe::readQuickCrossCoord(std::string input) {
	std::ifstream infile(input);
	std::string line;
	int numeroLigne = 0;
	while (std::getline(infile, line)) {
    	std::istringstream iss(line);
		std::string subs;
		iss >> subs;
		_noeuds[numeroLigne]._xreel = stod(subs);
		iss >> subs;
		_noeuds[numeroLigne]._yreel = stod(subs);
		numeroLigne++;
	}
	infile.close();
}

void Graphe::readQuickCrossGraphAndCoord(std::string fileArete, std::string fileCoord) {
	readQuickCrossGraph(fileArete);
	readQuickCrossCoord(fileCoord);
}

void Graphe::writeToGraphEdgeList(std::string output) {
	std::ofstream resultats(output, std::ios_base::app);
	for (int i=0;i<_noeuds.size();i++) {
		int nodeId = _noeuds[i]._id;
		for (int j=0;j<_noeuds[i].voisins.size();j++) {
			int voisinId = _noeuds[i].voisins[j]->_id;
			if (voisinId > nodeId) {
				resultats << nodeId+1 << " " << voisinId+1 << "\n";
			}
		}
	}
	resultats.close();
}

void Graphe::writeCoordsNodeReel(std::string output) {
	std::ofstream resultats(output, std::ios_base::app);
	for (int i=0;i<_noeuds.size();i++) {
		resultats << _noeuds[i]._xreel << " " << _noeuds[i]._yreel << "\n";
	}
	resultats.close();
}

void Graphe::writeToJsonGraphReel(std::string output) {
	json j;

	int nodeNumber = _noeuds.size();
	for (int i=0;i<nodeNumber;i++) {
		j["nodes"][i]["id"] = _noeuds[i].getId();
		j["nodes"][i]["x"] = _noeuds[i]._xreel;
		j["nodes"][i]["y"] = _noeuds[i]._yreel;
	}

	int edgeNumber = _aretes.size();
	for (int i = 0; i < edgeNumber; i++) {
		j["edges"][i]["source"] = _aretes[i].getNoeud1()->getId();
		j["edges"][i]["target"] = _aretes[i].getNoeud2()->getId();
	}

	std::ofstream o(output);
	o << std::setw(4) << j << std::endl;
}

void Graphe::readFromDimacsGraph(std::string input) {
	std::ifstream infile(input);
	std::string line;
	int numeroLigne = 0;
	int idArete = 0;
	bool unSurDeux = false;
	int idNoeud;
	while (std::getline(infile, line)) {
		idNoeud = numeroLigne - 1;
    	std::istringstream iss(line);
		int numeroMot = 0;
		std::string subs;
		iss >> subs;
		do {
			int idVoisin = std::stoi(subs);
			if (numeroLigne == 0) {
				if (numeroMot == 0) {
					for (int i=0;i<idVoisin;i++) {
						_noeuds.push_back(Noeud(i));
					}
				}
				else if (numeroMot == 2) {
					if (idVoisin == 1) { unSurDeux = true; }
				}
			}
			else {
				if ((!unSurDeux)||(numeroMot%2 == 0)) {
					idVoisin--;
					if (idVoisin > idNoeud) {
						_aretes.push_back(Aretes(&_noeuds[idNoeud],&_noeuds[idVoisin],idArete));
						idArete++;
					}
				}
			}
			iss >> subs;
			numeroMot++;
		} while (iss);
		numeroLigne++;
	}
	infile.close();
}

void Graphe::readFromDimacsGraphClean(std::string input) {
	// Lecture de la premiere ligne
	std::vector<int> changeIdNode;
	bool unSurDeux = false;
	int finalNoeud;
	std::ifstream fichier(input);
	std::string ligne;
	std::getline(fichier, ligne);
	std::istringstream ligneStream(ligne);
	std::string mot;
	ligneStream >> mot;
	int nombreNoeud = std::stoi(mot);
	for (int i=0;i<nombreNoeud;i++) {
		changeIdNode.push_back(i);
		finalNoeud = nombreNoeud;
	}
	ligneStream >> mot;
	ligneStream >> mot;
	if (std::stoi(mot) == 1) { unSurDeux = true; }
	fichier.close();

	int increment = -1;
	while (increment != 0) {
		increment = 0;
		std::ifstream infile1(input);
		std::string line1;
		int numeroLigne1 = 0;
		int idNoeud1;
		while (std::getline(infile1, line1)) {
			if (numeroLigne1 > 0) {
				idNoeud1 = numeroLigne1 - 1;
				if (changeIdNode[idNoeud1] != -1) {
					changeIdNode[idNoeud1] -= increment;
					std::istringstream iss(line1);
					int nombreVoisin = 0, nombreMot = 0;
					if ((iss)&&(iss.rdbuf()->in_avail() > 0)) {
						std::string subs;
						iss >> subs;
						do {
							if ((!unSurDeux)||(nombreMot%2==0)) {
								int idVoisin = std::stoi(subs);
								idVoisin--;
								if (changeIdNode[idVoisin] != -1) {
									nombreVoisin++;
								}
							}
							nombreMot++;
							iss >> subs;
						} while (iss);
					}
					if (nombreVoisin <= 1) {
						changeIdNode[idNoeud1] = -1;
						increment++;
						finalNoeud--;
					}
				}
			}
			numeroLigne1++;
		}
		infile1.close();
	}

	for (int i=0;i<finalNoeud;i++) {
		_noeuds.push_back(Noeud(i));
	}
	
	std::ifstream infile(input);
	std::string line;
	int numeroLigne = 0;
	int idArete = 0;
	while (std::getline(infile, line)) {
		if (numeroLigne > 0) {
			int idNoeud = numeroLigne - 1;
			int idNoeudReel = changeIdNode[idNoeud];
			if (idNoeudReel >= 0) {
				std::istringstream iss(line);
				int numeroMot = 0;
				std::string subs;
				iss >> subs;
				do {
					if ((!unSurDeux)||(numeroMot%2 == 0)) {
						int idVoisin = std::stoi(subs);
						idVoisin--;
						int idVoisinReel = changeIdNode[idVoisin];
						if ((idVoisinReel >= 0)&&(idVoisinReel > idNoeudReel)) {
							_aretes.push_back(Aretes(&_noeuds[idNoeudReel],&_noeuds[idVoisinReel],idArete));
							idArete++;
						}
					}
					numeroMot++;
					iss >> subs;
				} while (iss);
			}
		}
		numeroLigne++;
	}
	infile.close();
}

void Graphe::readFromCSVGraphReel(std::string input) {
	std::ifstream infile(input);
	std::string line;
	std::getline(infile,line);
	std::getline(infile,line);
	std::istringstream iss(line);
	std::string subs;
	iss >> subs;
	int nombreNoeud = stoi(subs);
	_noeuds.reserve(nombreNoeud * 2);
	int numeroLigne = 0;
	int idArete = 0;
	std::string deli = ",";
	while (std::getline(infile, line)) {
		std::istringstream iss(line);
		std::string tmpSub;
		std::string subs;
		std::string subs2;
		iss >> subs2;
		size_t pos = 0;
		std::string token;
		while ((pos = subs2.find(deli)) != std::string::npos) {
			subs = subs2.substr(0, pos);
			subs2.erase(0, pos + deli.length());
		}

		if (numeroLigne<nombreNoeud) {
			double coord1 = stod(subs);
			double coord2 = stod(subs2);
			_noeuds.push_back(Noeud(numeroLigne));
			_noeuds[numeroLigne]._xreel = coord1;
			_noeuds[numeroLigne]._yreel = coord2;
		}
		else {
			int id1 = stoi(subs);
			int id2 = stoi(subs2);
			_aretes.push_back(Aretes(&_noeuds[id1],&_noeuds[id2],idArete));
			idArete++;
		}
		numeroLigne++;
	}
	infile.close();
}

void Graphe::writeToCSVGraphReel(std::string output) {
	std::ofstream csvFile(output, std::ios_base::app);
	csvFile << "Has Coordinates\n";
	csvFile << _noeuds.size() << "\n";
	for (int i=0;i<_noeuds.size();i++) {
		csvFile << (int)_noeuds[i]._xreel << "," << (int)_noeuds[i]._yreel << "\n";
	}
	for (int i=0;i<_aretes.size();i++) {
		csvFile << _aretes[i].getNoeud1()->_id << "," << _aretes[i].getNoeud2()->_id << "\n";
	}
	csvFile.close();
}