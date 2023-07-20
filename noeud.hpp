#ifndef NOEUD_HPP
#define NOEUD_HPP

#include <vector>
#include <iostream>
#include <sstream>

class Noeud {
public:
	std::vector<int> _aretes; // Contient les indices des aretes contenant ce noeud
	int _id; // Id du noeud dans le tableau du graphe, unique et >= 0
	std::vector<Noeud*> voisins; // Voisins directs relié par une arete
	Noeud(int id) { _id = id; }
	
	// Attention ce score n'est pas toujours a jour!
	long score = -1; // Score d'intersection du noeud

	int ogdfId = -1; // Id du noeud dans ogdf, utile uniquement dans l'algo ogdf.

	double _xreel = -12345;
	double _yreel = -12345;
	std::vector<int>* idCelluleVec = nullptr;
	
	int areteCommune(Noeud* noeudVoisin) {
		for (const int& idArete1 : _aretes) {
			for (const int& idArete2 : noeudVoisin->_aretes) {
				if (idArete1 == idArete2) { return idArete1; }
			}
		}
		return -1;
	}
	void connect(Noeud* noeud) { voisins.push_back(noeud); }
	std::vector<Noeud*> getVoisins() const { return voisins; }
	bool estVoisin(Noeud* n1) {
		for (const int& idArete1 : _aretes) {
			for (const int& idArete2 : n1->_aretes) {
				if (idArete1 == idArete2) { return true; }
			}
		}
		return false;
	}
	bool estIsole() { return (voisins.size() == 0); }
	int getId() const { return _id; }

	void setCoordReel(std::pair<double,double>& coord) {
		_xreel = coord.first;
		_yreel = coord.second;
	}

	std::string voisinString() {
		std::stringstream paramStream;
		paramStream << "{";
		for (int i=0;i<voisins.size();i++) {
			paramStream << voisins[i]->_id;
			if (i<voisins.size()-1) { paramStream << " "; }
		}
		paramStream << "}";
		return paramStream.str();
	}

};


#endif