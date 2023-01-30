#ifndef ARETES_HPP
#define ARETES_HPP

#include "noeud.hpp"

class Aretes {
public:
	Noeud* _noeud1, *_noeud2;
	int _id;
	Aretes(Noeud* noeud1, Noeud* noeud2, int id) {
		_noeud1 = noeud1;
		_noeud2 = noeud2;
		_id = id;
		noeud1->_aretes.push_back(id);
		noeud2->_aretes.push_back(id);
		noeud1->connect(noeud2);
		noeud2->connect(noeud1);
	}

	Noeud* getNoeud1()  const { return _noeud1; }
	Noeud* getNoeud2()  const { return _noeud2; }

	void setNoeud1(Noeud* noeud1) { _noeud1 = noeud1; }
	void setNoeud2(Noeud* noeud2) { _noeud2 = noeud2; }

	bool contains(Noeud* noeud) const
	{
		return (_noeud1->getId() == noeud->getId() || _noeud2->getId() == noeud->getId());
	}

	bool contains(int id) {
		return (_noeud1->getId() == id || _noeud2->getId() == id);
	}

	// Renvoie le noeud qui n'est pas en commun avec l'arete passé en parametre
	Noeud* nodeNotInCommon(Aretes l) const {
		if (_noeud1->getId() == l.getNoeud1()->getId() || _noeud1->getId() == l.getNoeud2()->getId()) { return _noeud2; }
		return _noeud1;
	}

	bool estPlace() {
		return (_noeud1->getEmplacement() != nullptr && _noeud2->getEmplacement() != nullptr);
	}

};

#endif