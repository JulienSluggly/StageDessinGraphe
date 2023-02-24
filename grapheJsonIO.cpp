#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include "pugixml.hpp"
#include "graphe.hpp"

using nlohmann::json;

bool DEBUG_JSON = false;

// ----- CREATION D'UN Graph A PARTIR D'UN FICHIER JSON -----
// Precondition: Les id des noeuds sont ordonnees et commencent par 0
void Graphe::readFromJsonGraph(std::string input) {
	if (DEBUG_JSON) std::cout << "Fichier Graphe: " << input << std::endl;
	std::ifstream inp(input);
	json j;
	inp >> j;

	// Si le fichier ne contient pas de node
	if (j["nodes"] == nullptr) {
		exit(1);
	}
	int nodeNumber = static_cast<int>(j["nodes"].size());
	if (DEBUG_JSON) std::cout << "Nombre de noeud dans le json: " << nodeNumber << std::endl;
	for (int i = 0; i < nodeNumber; i++) {
		_noeuds.push_back(Noeud(i));
	}
	int edgeNumber = static_cast<int>(j["edges"].size());
	if (DEBUG_JSON) std::cout << "Nombre d'arete dans le json: " << edgeNumber << std::endl;
	int id1, id2;
	for (int i = 0; i < edgeNumber; i++) {
		id1 = j["edges"][i]["source"];
		id2 = j["edges"][i]["target"];
		_liens.push_back(Aretes(&_noeuds[id1], &_noeuds[id2],i));
	}
}

// Lecture des slots
// Precondition: Les id des slots sont ordonn�s et commencent par 0
void Graphe::readFromJsonSlots(std::string input) {
	if (DEBUG_JSON) std::cout << "Fichier Slot: " << input << std::endl;
	std::ifstream inp(input);
	json j;
	inp >> j;

	// Si le fichier ne contient pas de slots
	if (j["slots"] == nullptr) {
		exit(1);
	}

	int slotsNumber = static_cast<int>(j["slots"].size());
	if (DEBUG_JSON) std::cout << "Nombre de slots dans le fichier json: " << slotsNumber << std::endl;
	int x, y;
	for (int i = 0; i < slotsNumber; i++) {
		x = j["slots"][i]["x"];
		y = j["slots"][i]["y"];
		_emplacementsPossibles.push_back(Emplacement(x,y,i));
		if (x > gridWidth) { gridWidth = x; }
		if (y > gridHeight) { gridHeight = y; }
	}
}

// ----- CREATION D'UN Graph A PARTIR D'UN FICHIER JSON -----
// Precondition: Les id des noeuds sont ordonn�es et commencent par 0
void Graphe::readFromJsonGraphAndSlot(std::string input) {
	if (DEBUG_JSON) std::cout << "Fichier Graphe: " << input << std::endl;
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
	if (DEBUG_JSON) std::cout << "Nombre de slots dans le fichier json: " << slotsNumber << std::endl;
	int x, y;
	for (int i = 0; i < slotsNumber; i++) {
		x = j["slots"][i]["x"];
		y = j["slots"][i]["y"];
		_emplacementsPossibles.push_back(Emplacement(x,y,i));
		if (x > gridWidth) { gridWidth = x; }
		if (y > gridHeight) { gridHeight = y; }
	}

	int nodeNumber = static_cast<int>(j["nodes"].size());
	if (DEBUG_JSON) std::cout << "Nombre de noeud dans le json: " << nodeNumber << std::endl;
	int id;
	for (int i = 0; i < nodeNumber; i++) {
		_noeuds.push_back(Noeud(i));
	}
	for (int i=0;i<nodeNumber;i++) {
		id = j["nodes"][i]["id_slot"];
		if (id != -1) {
			_noeuds[i].setEmplacement(&_emplacementsPossibles[id]);
		}
	}
	int edgeNumber = static_cast<int>(j["edges"].size());
	if (DEBUG_JSON) std::cout << "Nombre d'arete dans le json: " << edgeNumber << std::endl;
	int id1, id2;
	for (int i = 0; i < edgeNumber; i++) {
		id1 = j["edges"][i]["source"];
		id2 = j["edges"][i]["target"];
		_liens.push_back(Aretes(&_noeuds[id1], &_noeuds[id2],i));
	}
}

// Sauvegarde des slots dans le fichier output
void Graphe::writeToJsonSlots(std::string output) {
	json j;
	j["width"] = gridWidth;
	j["height"] = gridHeight;

	int slotsNumber = _emplacementsPossibles.size();
	for (int i = 0; i < slotsNumber; i++) {
		j["slots"][i]["id"] = i;
		j["slots"][i]["x"] = _emplacementsPossibles[i].getX();
		j["slots"][i]["y"] = _emplacementsPossibles[i].getY();
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

	int edgeNumber = _liens.size();
	for (int i = 0; i < edgeNumber; i++) {
		j["edges"][i]["source"] = _liens[i].getNoeud1()->getId();
		j["edges"][i]["target"] = _liens[i].getNoeud2()->getId();
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
	if (DEBUG_JSON) std::cout << "gridWidth: " << gridWidth << " gridHeight: " << gridHeight << std::endl;

	if (j["nodes"] == nullptr) {
		exit(1);
	}

	bool areNodePlaced = true;
	if ((j["nodes"][0]["x"] == j["nodes"][1]["x"])&&(j["nodes"][0]["y"] == j["nodes"][0]["y"])) {
		areNodePlaced = false;
	}

	int nodeNumber = static_cast<int>(j["nodes"].size());
	int x, y;
	for (int i = 0; i < nodeNumber; i++) {
		_noeuds.push_back(Noeud(i));
		x = j["nodes"][i]["x"];
		y = j["nodes"][i]["y"];
		if (areNodePlaced) { _emplacementsPossibles.push_back(Emplacement(x,y,i)); }
	}

	int edgeNumber = static_cast<int>(j["edges"].size());
	int id1, id2;
	std::vector<int> tmpVec;
	int nbArete = 0;
	for (int i = 0, index = nodeNumber; i < edgeNumber; i++) {
		id1 = j["edges"][i]["source"];
		tmpVec.push_back(id1);
		if (j["edges"][i]["bends"] != nullptr) {
			int bendsNumber = static_cast<int>(j["edges"][i]["bends"].size());
			for (int k=0; k < bendsNumber; k++, index++) {
				x = j["edges"][i]["bends"][k]["x"];
				y = j["edges"][i]["bends"][k]["y"];
				id2 = index;
				tmpVec.push_back(id2);
				tmpVec.push_back(id2);
				_noeuds.push_back(Noeud(index));
				if (areNodePlaced) { _emplacementsPossibles.push_back(Emplacement(x,y,index)); }
				nbArete++;
				id1 = id2;
			}
		}
		id2 = j["edges"][i]["target"];
		tmpVec.push_back(id2);
	}
	for (int i = 0; i < tmpVec.size(); i += 2) {
		_liens.push_back(Aretes(&_noeuds[tmpVec[i]], &_noeuds[tmpVec[i + 1]], i / 2));
	}
	return tmpVec;
}

void Graphe::readFromGraphmlGraph(std::string input) {
	pugi::xml_document doc;
	if (!doc.load_file(input.c_str())) { std::cout << "Fichier inexistant.\n"; exit(-1); }

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
		_liens.push_back(Aretes(&_noeuds[id1], &_noeuds[id2],i));
    }
}