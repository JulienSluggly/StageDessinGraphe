#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
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

// Lecture des slots, noeuds et edges
void Graphe::readFromJsonChallenge(std::string input) {
	tcout() << "Fichier Graphe: " << input << std::endl;
	std::ifstream inp(input);
	json j;
	inp >> j;

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
	}

	int nodeNumber = static_cast<int>(j["nodes"].size());
	tcout() << "Nombre de noeud dans le json: " << nodeNumber << std::endl;
	_noeuds.reserve(nodeNumber*2);
	for (int i = 0; i < nodeNumber; i++) {
		_noeuds.push_back(Noeud(i));
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
