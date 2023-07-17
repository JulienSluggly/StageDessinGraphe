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

// Lecture des slots
// Precondition: Les id des slots sont ordonn�s et commencent par 0
void Graphe::readFromJsonSlots(std::string input) {
	if (DEBUG_JSON) tcout() << "Fichier Slot: " << input << std::endl;
	std::ifstream inp(input);
	json j;
	inp >> j;

	// Si le fichier ne contient pas de slots
	if (j["slots"] == nullptr) {
		exit(1);
	}

	int slotsNumber = static_cast<int>(j["slots"].size());
	if (DEBUG_JSON) tcout() << "Nombre de slots dans le fichier json: " << slotsNumber << std::endl;
	int x, y;
	_emplacements.reserve(slotsNumber*2);
	for (int i = 0; i < slotsNumber; i++) {
		x = j["slots"][i]["x"];
		y = j["slots"][i]["y"];
		_emplacements.push_back(Emplacement(x,y,i));
		if (x > gridWidth) { gridWidth = x; }
		if (y > gridHeight) { gridHeight = y; }
	}
}

struct PairHash {
    template <typename T1, typename T2>
    std::size_t operator()(const std::pair<T1, T2>& pair) const {
        auto hash1 = std::hash<T1>{}(pair.first);
        auto hash2 = std::hash<T2>{}(pair.second);
        return hash1 ^ hash2;
    }
};

// Lecture des slots, noeuds et edges
void Graphe::readFromJsonChallenge(std::string input) {
	tcout() << "Fichier Graphe: " << input << std::endl;
	std::ifstream inp(input);
	json j;
	inp >> j;


	std::unordered_map<std::pair<int,int>,Emplacement*,PairHash> mapCoordEmplacement;

	int slotsNumber = static_cast<int>(j["points"].size());
	tcout() << "Nombre de slots dans le fichier json: " << slotsNumber << std::endl;
	int x, y;
	_emplacements.reserve(slotsNumber*2);
	for (int i = 0; i < slotsNumber; i++) {
		x = j["points"][i]["x"];
		y = j["points"][i]["y"];
		_emplacements.push_back(Emplacement(x,y,i));
		if (x > gridWidth) { gridWidth = x; }
		if (y > gridHeight) { gridHeight = y; }
		mapCoordEmplacement.insert(make_pair(make_pair(x,y),&_emplacements[i]));
	}

	int nodeNumber = static_cast<int>(j["nodes"].size());
	tcout() << "Nombre de noeud dans le json: " << nodeNumber << std::endl;
	_noeuds.reserve(nodeNumber*2);
	for (int i = 0; i < nodeNumber; i++) {
		_noeuds.push_back(Noeud(i));
		x = j["nodes"][i]["x"];
		y = j["nodes"][i]["y"];
		std::pair<int,int> coords(x,y);
		auto elemMap = mapCoordEmplacement.find(coords);
		if (elemMap != mapCoordEmplacement.end()) {
			_noeuds[i].setEmplacement(elemMap->second);
		}
	}
	int edgeNumber = static_cast<int>(j["edges"].size());
	tcout() << "Nombre d'arete dans le json: " << edgeNumber << std::endl;
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

// ----- CREATION D'UN Graph A PARTIR D'UN FICHIER JSON -----
// Precondition: Les id des noeuds sont ordonn�es et commencent par 0
void Graphe::readFromJsonGraphAndSlot(std::string input) {
	if (DEBUG_JSON) tcout() << "Fichier Graphe: " << input << std::endl;
	std::ifstream inp(input);
	json j;
	inp >> j;

	// Si le fichier ne contient pas de node
	if (j["nodes"] == nullptr) {
		exit(1);
	}
	// Si le fichier ne contient pas de slots
	if (j["slots"] == nullptr) {
		exit(1);
	}

	int slotsNumber = static_cast<int>(j["slots"].size());
	if (DEBUG_JSON) tcout() << "Nombre de slots dans le fichier json: " << slotsNumber << std::endl;
	int x, y;
	_emplacements.reserve(slotsNumber*2);
	for (int i = 0; i < slotsNumber; i++) {
		x = j["slots"][i]["x"];
		y = j["slots"][i]["y"];
		_emplacements.push_back(Emplacement(x,y,i));
		if (x > gridWidth) { gridWidth = x; }
		if (y > gridHeight) { gridHeight = y; }
	}

	int nodeNumber = static_cast<int>(j["nodes"].size());
	if (DEBUG_JSON) tcout() << "Nombre de noeud dans le json: " << nodeNumber << std::endl;
	int id;
	_noeuds.reserve(nodeNumber*2);
	for (int i = 0; i < nodeNumber; i++) {
		_noeuds.push_back(Noeud(i));
	}
	for (int i=0;i<nodeNumber;i++) {
		id = j["nodes"][i]["id_slot"];
		if (id != -1) {
			_noeuds[i].setEmplacement(&_emplacements[id]);
		}
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

// Sauvegarde des slots dans le fichier output
void Graphe::writeToJsonSlots(std::string output) {
	json j;
	j["width"] = gridWidth;
	j["height"] = gridHeight;

	int slotsNumber = _emplacements.size();
	for (int i = 0; i < slotsNumber; i++) {
		j["slots"][i]["id"] = i;
		j["slots"][i]["x"] = _emplacements[i].getX();
		j["slots"][i]["y"] = _emplacements[i].getY();
	}

	std::ofstream o(output);
	o << std::setw(4) << j << std::endl;
}

void Graphe::writeToJsonGraph(std::string output) {
	json j;

	int nodeNumber = _noeuds.size();
	for (int i = 0; i < nodeNumber; i++) {
		j["nodes"][i]["id"] = _noeuds[i].getId();
		if (_noeuds[i].getEmplacement() != nullptr)
			j["nodes"][i]["id_slot"] = _noeuds[i].getEmplacement()->getId();
		else
			j["nodes"][i]["id_slot"] = -1;
	}

	int edgeNumber = _aretes.size();
	for (int i = 0; i < edgeNumber; i++) {
		j["edges"][i]["source"] = _aretes[i].getNoeud1()->getId();
		j["edges"][i]["target"] = _aretes[i].getNoeud2()->getId();
	}

	std::ofstream o(output);
	o << std::setw(4) << j << std::endl;
}

void Graphe::writeToJsonGraphAndSlots(std::string output) {
	json j;

	int slotsNumber = _emplacements.size();
	for (int i = 0; i < slotsNumber; i++) {
		j["slots"][i]["id"] = i;
		j["slots"][i]["x"] = _emplacements[i].getX();
		j["slots"][i]["y"] = _emplacements[i].getY();
	}

	int nodeNumber = _noeuds.size();
	for (int i = 0; i < nodeNumber; i++) {
		j["nodes"][i]["id"] = _noeuds[i].getId();
		if (_noeuds[i].getEmplacement() != nullptr)
			j["nodes"][i]["id_slot"] = _noeuds[i].getEmplacement()->getId();
		else
			j["nodes"][i]["id_slot"] = -1;
	}

	int edgeNumber = _aretes.size();
	for (int i = 0; i < edgeNumber; i++) {
		j["edges"][i]["source"] = _aretes[i].getNoeud1()->getId();
		j["edges"][i]["target"] = _aretes[i].getNoeud2()->getId();
	}

	std::ofstream o(output);
	o << std::setw(4) << j << std::endl;
}

void Graphe::writeToJsonChallenge(std::string output) {
	json j;

	int slotsNumber = _emplacements.size();
	for (int i = 0; i < slotsNumber; i++) {
		j["points"][i]["id"] = i;
		j["points"][i]["x"] = _emplacements[i].getX();
		j["points"][i]["y"] = _emplacements[i].getY();
	}

	int nodeNumber = _noeuds.size();
	for (int i = 0; i < nodeNumber; i++) {
		j["nodes"][i]["id"] = _noeuds[i].getId();
		if (_noeuds[i].getEmplacement() != nullptr) {
			j["nodes"][i]["x"] = _noeuds[i].getEmplacement()->getX();
			j["nodes"][i]["y"] = _noeuds[i].getEmplacement()->getY();
		}
		else {
			j["nodes"][i]["x"] = 0;
			j["nodes"][i]["y"] = 0;
		}
	}

	int edgeNumber = _aretes.size();
	for (int i = 0; i < edgeNumber; i++) {
		j["edges"][i]["source"] = _aretes[i].getNoeud1()->getId();
		j["edges"][i]["target"] = _aretes[i].getNoeud2()->getId();
	}

	std::ofstream o(output);
	o << std::setw(4) << j << std::endl;
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

void Graphe::writeToJsonComposanteConnexe(std::string output, std::vector<int> composante) {
	json j;
	std::sort(composante.begin(),composante.end());
	std::unordered_map<int,int> mapNewIndex;
	for (int i=0;i<composante.size();i++) {
		mapNewIndex.insert({composante[i],i});
	}
	std::set<std::pair<int,int>> aretesComposante;
	for (int i=0;i<composante.size();i++) {
		j["nodes"][i]["id"] = i;
		if (_noeuds[composante[i]].getEmplacement() != nullptr) { j["nodes"][i]["id_slot"] = _noeuds[composante[i]].getEmplacement()->getId(); }
		else { j["nodes"][i]["id_slot"] = -1; }
		for (const int& idArete : _noeuds[composante[i]]._aretes) {
			int idNoeud1 = mapNewIndex[_aretes[idArete]._noeud1->_id];
			int idNoeud2 = mapNewIndex[_aretes[idArete]._noeud2->_id];
			aretesComposante.insert({idNoeud1,idNoeud2});
		}
	}
	int numeroArete = 0;
	for (const std::pair<int,int>& arete : aretesComposante) {
		j["edges"][numeroArete]["source"] = arete.first;
		j["edges"][numeroArete]["target"] = arete.second;
		numeroArete++;
	}

	std::ofstream o(output);
	o << std::setw(4) << j << std::endl;
}

// ----- CREATION D'UN Graph A PARTIR D'UN FICHIER JSON -----
// Creer les emplacements a partir des anciennes coord
// Les noeuds doivent avoir des coordonn�es
std::vector<int> Graphe::readFromJsonOldGraph(std::string input) {
	std::ifstream inp(input);
	json j;
	inp >> j;
	gridWidth = j["width"];
	gridHeight = j["height"];
	if (DEBUG_JSON) tcout() << "gridWidth: " << gridWidth << " gridHeight: " << gridHeight << std::endl;

	if (j["nodes"] == nullptr) {
		exit(1);
	}

	bool areNodePlaced = true;
	if ((j["nodes"][0]["x"] == j["nodes"][1]["x"])&&(j["nodes"][0]["y"] == j["nodes"][0]["y"])) {
		areNodePlaced = false;
	}

	int nodeNumber = static_cast<int>(j["nodes"].size());
	int x, y;
	_noeuds.reserve(nodeNumber*2);
	_emplacements.reserve(nodeNumber*2);
	for (int i = 0; i < nodeNumber; i++) {
		_noeuds.push_back(Noeud(i));
		x = j["nodes"][i]["x"];
		y = j["nodes"][i]["y"];
		if (areNodePlaced) { _emplacements.push_back(Emplacement(x,y,i)); }
	}

	int edgeNumber = static_cast<int>(j["edges"].size());
	int id1, id2;
	std::vector<int> tmpVec;
	int nbArete = 0;
	_aretes.reserve(edgeNumber*2);
	for (int i = 0; i < edgeNumber; i++) {
		id1 = j["edges"][i]["source"];
		tmpVec.push_back(id1);
		id2 = j["edges"][i]["target"];
		tmpVec.push_back(id2);
	}
	for (int i = 0; i < tmpVec.size(); i += 2) {
		_aretes.push_back(Aretes(&_noeuds[tmpVec[i]], &_noeuds[tmpVec[i + 1]], i / 2));
	}
	return tmpVec;
}

#if defined(PUGIXML_INSTALLED)
#include "pugixml.hpp"

void Graphe::readFromGraphmlGraph(std::string input) {
	pugi::xml_document doc;
	if (!doc.load_file(input.c_str())) { tcout() << "Fichier inexistant.\n"; exit(-1); }

	pugi::xml_node data = doc.child("graphml").child("graph");
    // tag::basic[]
	int i=0;
    for (pugi::xml_node tool = data.child("node"); tool; tool = tool.next_sibling("node"), i++) {
        _noeuds.push_back(Noeud(i));
    }
	i=0;
	for (pugi::xml_node tool = data.child("edge"); tool; tool = tool.next_sibling("edge"), i++) {
        std::string idSourceString = tool.attribute("source").value();
        std::string idTargetString = tool.attribute("target").value();
		int id1 = std::stoi(idSourceString.substr(1));
		int id2 = std::stoi(idTargetString.substr(1));
		_aretes.push_back(Aretes(&_noeuds[id1], &_noeuds[id2],i));
    }
	updatePenalite(_aretes.size(),_aretes.size());
	updateIsolatedNodes();
}
#else
void Graphe::readFromGraphmlGraph(std::string input) {
	tcout() << "PUGIXML NOT INSTALLED.\n";
	exit(1);
}
#endif

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

void Graphe::writeToJsonCleanGraphe(std::string output) {
	json j;
	Graphe* G = this;
	if (!isGrapheConnected()) {
		std::vector<int> composante = plusGrandeComposanteConnexe();
		std::sort(composante.begin(),composante.end());
		std::unordered_map<int,int> mapNewIndex;
		G = new Graphe();
		for (int i=0;i<composante.size();i++) {
			mapNewIndex.insert({composante[i],i});
			G->_noeuds.push_back(Noeud(i));
		}
		std::set<std::pair<int,int>> aretesComposante;
		for (int i=0;i<composante.size();i++) {
			for (const int& idArete : _noeuds[composante[i]]._aretes) {
				int idNoeud = mapNewIndex[_aretes[idArete]._noeud1->_id];
				int idVoisin = mapNewIndex[_aretes[idArete]._noeud2->_id];
				if (idNoeud != idVoisin) {
					int idNoeud1, idNoeud2;
					if (idNoeud < idVoisin) { idNoeud1 = idNoeud; idNoeud2 = idVoisin; }
					else { idNoeud1 = idVoisin; idNoeud2 = idNoeud; }
					aretesComposante.insert({idNoeud1,idNoeud2});
				}
			}
		}
		int numeroArete = 0;
		for (const std::pair<int,int>& arete : aretesComposante) {
			G->_aretes.push_back(Aretes(&G->_noeuds[arete.first],&G->_noeuds[arete.second],numeroArete));
			numeroArete++;
		}
	}

	std::map<int,std::set<int>> mapNoeudVoisins;
	for (int i=0;i<G->_noeuds.size();i++) {
		std::set<int> voisins;
		for (const Noeud* voisinNoeud : G->_noeuds[i].voisins) {
			if (G->_noeuds[i]._id != voisinNoeud->_id) {
				voisins.insert(voisinNoeud->_id);
			}
		}
		mapNoeudVoisins.insert({i,voisins});
	}
	bool suppression = true;
	std::vector<int> toDelete;
	while (suppression) {
		toDelete.clear();
		suppression = false;
		for (auto const& [idNoeud, voisins] : mapNoeudVoisins) {
			if (voisins.size() <= 1) {
				suppression = true;
				for (const int& idVoisin : voisins) {
					mapNoeudVoisins[idVoisin].erase(idNoeud);
				}
				toDelete.push_back(idNoeud);
			}
		}
		for (const int& idNoeud : toDelete) {
			mapNoeudVoisins.erase(idNoeud);
		}
	}

	std::unordered_map<int,int> mapNewIndex;
	int i=0;
	for (auto const& [idNoeud, voisins] : mapNoeudVoisins) {
		mapNewIndex.insert({idNoeud,i});
		i++;
	}
	std::set<std::pair<int,int>> aretesComposante;
	i=0;
	for (auto const& [tmpIdNoeud, voisins] : mapNoeudVoisins) {
		int idNoeud = mapNewIndex[tmpIdNoeud];
		j["nodes"][i]["id"] = i;
		if (G->_noeuds[idNoeud].getEmplacement() != nullptr) { j["nodes"][i]["id_slot"] = _noeuds[idNoeud].getEmplacement()->getId(); }
		else { j["nodes"][i]["id_slot"] = -1; }
		for (const int& tmpIdVoisin : voisins) {
			int idVoisin = mapNewIndex[tmpIdVoisin];
			int idNoeud1, idNoeud2;
			if (idNoeud < idVoisin) { idNoeud1 = idNoeud; idNoeud2 = idVoisin; }
			else { idNoeud1 = idVoisin; idNoeud2 = idNoeud; }
			aretesComposante.insert({idNoeud1,idNoeud2});
		}
		i++;
	}
	int numeroArete = 0;
	for (const std::pair<int,int>& arete : aretesComposante) {
		j["edges"][numeroArete]["source"] = arete.first;
		j["edges"][numeroArete]["target"] = arete.second;
		numeroArete++;
	}
	std::ofstream o(output);
	o << std::setw(4) << j << std::endl;
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