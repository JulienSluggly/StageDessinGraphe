#ifndef NOEUD_HPP
#define NOEUD_HPP

#include "emplacement.hpp"
#include <vector>

class Noeud {
public:
	Emplacement* _emplacement = nullptr;
	// Contient les indices des aretes contenant ce noeud
	std::vector<int> _aretes;
	int _id;
	std::vector<Noeud*> voisins;
	Noeud(int id) {
		_id = id;
	}
	// Score d'intersection du noeud
	// Attention ce score n'est pas toujours a jour!
	long score = -1;

	int ogdfId = -1;

	double stressX = -1;
	double stressY = -1;
	
	bool estPlace() const { return _emplacement != nullptr; }
	Emplacement* getEmplacement()  const { return _emplacement; }
	void connect(Noeud* noeud) { voisins.push_back(noeud); }
	std::vector<Noeud*> getVoisins() const { return voisins; }
	bool voisinsSontPlaces() const {
		for (int i=0;i<voisins.size();i++) {
			if (!voisins[i]->estPlace()) return false;
		}
		return true;
	}
	int getVoisinsPlaces() const {
		int nbVoisinPlaces = 0;
		for (int i=0;i<voisins.size();i++) {
			if (voisins[i]->estPlace()) ++nbVoisinPlaces;
		}
		return nbVoisinPlaces;
	}
	int getX()  const { return _emplacement->getX(); }
	int getY()  const { return _emplacement->getY(); }
	// Indique si deux noeuds ont les mêmes coordonnées
	bool compare(const Noeud* noeud) const {
		return ((getX() == noeud->getX()) && (getY() == noeud->getY()));
	}
	int getId() const { return _id; }

	void setEmplacement(Emplacement* emplacement)
	{
		if (_emplacement != nullptr) {
			_emplacement->removeNoeud();
		}
		_emplacement = emplacement;
		emplacement->setNoeud(this);
	}

	// Force le noeud à l'emplacement en enlevant l'ancien noeud s'il y en avait un.
	// Renvoie l'id de l'ancien noeud présent sur l'emplacement ou -1 s'il n'y en avait pas.
	int ecraseNoeud(Emplacement& emplacement) {
		int oldNodeId = -1;
		if (_emplacement != nullptr) {
			_emplacement->removeNoeud();
		}
		if (emplacement._noeud != nullptr) {
			oldNodeId = emplacement._noeud->getId();
			emplacement._noeud->clearEmplacement();
		}
		_emplacement = &emplacement;
		emplacement.setNoeud(this);
		return oldNodeId;
	}

	void clearEmplacement() {
		if (_emplacement != nullptr) {
			_emplacement->removeNoeud();
		}
		_emplacement = nullptr;
	}

	void swap(Emplacement* emp2) {
		Noeud* n2 = emp2->_noeud;
		Emplacement* emp1 = this->_emplacement;
		this->clearEmplacement();
		n2->clearEmplacement();
		this->setEmplacement(emp2);
		n2->setEmplacement(emp1);
	}

};


#endif