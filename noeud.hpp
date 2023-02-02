#ifndef NOEUD_HPP
#define NOEUD_HPP

#include "emplacement.hpp"

class Noeud {
public:
	Emplacement* _emplacement = nullptr;
	std::vector<int> _aretes; // Contient les indices des aretes contenant ce noeud
	int _id;
	std::vector<Noeud*> voisins;
	Noeud(int id) {
		_id = id;
	}
	
	bool estPlace() const { return _emplacement != nullptr; }
	Emplacement* getEmplacement()  const { return _emplacement; }
	void connect(Noeud* noeud) { voisins.push_back(noeud); }
	std::vector<Noeud*> getVoisins() const { return voisins; }
	bool voisinsSontPlaces() const {
		for (Noeud* noeud : voisins)
		{
			if (!noeud->estPlace()) return false;
		}
		return true;
	}
	int getVoisinsPlaces() const {
		int nbVoisinPlaces = 0;
		for (Noeud* noeud : voisins)
		{
			if (noeud->estPlace()) ++nbVoisinPlaces;
		}
		return nbVoisinPlaces;
	}
	int getX()  const { return _emplacement->getX(); }
	int getY()  const { return _emplacement->getY(); }
	Point getPosition() const { return _emplacement->getPosition(); }
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

	void ecraseNoeud(Emplacement& emplacement) {
		if (_emplacement != nullptr) {
			_emplacement->removeNoeud();
		}
		if (emplacement._noeud != nullptr) {
			emplacement._noeud->clearEmplacement();
		}
		_emplacement = &emplacement;
		emplacement.setNoeud(this);
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