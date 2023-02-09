#ifndef GRAPHE_HPP
#define GRAPHE_HPP

#include <vector>
#include "aretes.hpp"
#include "point.hpp"
#include "intersection.hpp"
#include "emplacement.hpp"
#include "utilitaire.hpp"
#include <iostream>
#include <climits>

class Graphe {
public:
	std::vector<Emplacement> _emplacementsPossibles;
	std::vector<Aretes> _liens;
	std::vector<Noeud> _noeuds;

	std::set<Aretes*> areteIll;
	std::set<Aretes*> areteIllSelf;
	std::set<Aretes*> areteInter;

	int PENALITE_MAX = 1000;
	int PENALITE_MAX_SELF = 1001;
	int gridHeight = 10;
	int gridWidth = 10;

	// Attention cette variable n'est pas forcément à jour!
	long nombreCroisement = -1;
	bool isNombreCroisementUpdated = false;
	bool isNodeScoreUpdated = false;
	bool isIntersectionVectorUpdated = false;

	// Nombre maximum de voisin d'un noeud dans le graphe.
	int maxVoisin = -1;

	bool DEBUG_GRAPHE = false;
	bool DEBUG_OPENGL = true;

	std::string nomGraphe = "Graphe";

	Graphe()
	{}

	Graphe(std::string nom) {
		nomGraphe = nom;
	}

	void afficherLiens(std::string nom = "") {
		std::cout << "-----------------------------------------------" << std::endl;
		std::cout << "Affichage DEBUG Aretes: " << nomGraphe << " " << nom << std::endl;
		std::cout << "Nb Aretes: " << _liens.size() << std::endl;
		for (int i = 0; i < _liens.size(); i++) {
			std::cout << "id: " << _liens[i]._id << " idnode1: " << _liens[i].getNoeud1()->getId() << " idnode2: " << _liens[i].getNoeud2()->getId();
			std::cout << " x1: " << _liens[i].getNoeud1()->getX() << " y1: " << _liens[i].getNoeud1()->getY();
			std::cout << " x2: " << _liens[i].getNoeud2()->getX() << " y2: " << _liens[i].getNoeud2()->getY();
			std::cout << std::endl;
		}
		std::cout << "-----------------------------------------------" << std::endl;
	}

	void afficherNoeuds(std::string nom = "") {
		std::cout << "-----------------------------------------------" << std::endl;
		std::cout << "Affichage DEBUG Noeuds: " << nomGraphe << " " << nom << std::endl;
		std::cout << "Nb Noeuds: " << _noeuds.size() << std::endl;
		for (int i = 0; i < _noeuds.size(); i++) {
			std::cout << "id: " << _noeuds[i].getId() << " empid: ";
			if (_noeuds[i].getEmplacement() == nullptr) {
				std::cout << " aucun" << std::endl;
			}
			else {
				std::cout << _noeuds[i].getEmplacement()->_id << " x: " << _noeuds[i].getX() << " y: " << _noeuds[i].getY() << " score: " << _noeuds[i].score << std::endl;
			}
		}
		std::cout << "-----------------------------------------------" << std::endl;
	}

	void afficherEmplacement(std::string nom = "") {
		std::cout << "-----------------------------------------------" << std::endl;
		std::cout << "Affichage DEBUG Emplacements: " << nomGraphe << " " << nom << std::endl;
		std::cout << "Nb Emplacements: " << _emplacementsPossibles.size() << std::endl;
		for (int i = 0; i < _emplacementsPossibles.size(); i++) {
			std::cout << "id: " << _emplacementsPossibles[i]._id << " x: " << _emplacementsPossibles[i].getX() << " y: " << _emplacementsPossibles[i].getY() << " idnode: ";
			if (_emplacementsPossibles[i]._noeud == nullptr) {
				std::cout << "aucun: " << _emplacementsPossibles[i].estDisponible() << std::endl;
			}
			else {
				std::cout << _emplacementsPossibles[i]._noeud->getId() << std::endl;
			}
		}
		std::cout << "-----------------------------------------------" << std::endl;
	}

	void afficherNoeudSeul(bool display=true, std::string nom = "") {
		if (display) {
			std::cout << "-----------------------------------------------" << std::endl;
			std::cout << "Affichage DEBUG Noeud Seul: " << nomGraphe << " " << nom << std::endl;
		}
		int nbNoeudSeul = 0;
		for (int i = 0; i < _noeuds.size(); i++) {
			if (_noeuds[i]._aretes.size() == 0) {
				std::cout << "Noeud: " << i << std::endl;
				nbNoeudSeul++;
			}
		}
		if (nbNoeudSeul == 0) {
			if (display) { std::cout << "Aucun" << std::endl; }
		}
		else {
			if (!display) { std::cout << "-----------------------------------------------" << std::endl; }
		}
		if (display) { std::cout << "-----------------------------------------------" << std::endl; }
	}

	void afficherInfo(std::string nom = "") {
		std::cout << "-----------------------------------------------" << std::endl;
		std::cout << "Information sur le graphe: " << nomGraphe << " " << nom << std::endl;
		std::cout << "Nombre de noeud: " << _noeuds.size() << std::endl;
		std::cout << "Nombre d'emplacement: " << _emplacementsPossibles.size() << std::endl;
		std::cout << "Nombre d'aretes: " << _liens.size() << std::endl;
		std::cout << "-----------------------------------------------" << std::endl;
	}

	void afficherAreteDouble(bool display=true, std::string nom = "") {
		if (display) {
			std::cout << "-----------------------------------------------" << std::endl;
			std::cout << "Affichage DEBUG Arrete Double: " << nomGraphe << " " << nom << std::endl;
		}
		int nbAreteDouble = 0;
		for (int i = 0; i < _liens.size(); i++) {
			int id1 = _liens[i].getNoeud1()->getId();
			int id2 = _liens[i].getNoeud2()->getId();
			for (int j = i + 1; j < _liens.size(); j++) {
				int id12 = _liens[j].getNoeud1()->getId();
				int id22 = _liens[j].getNoeud2()->getId();
				if (((id1 == id12) && (id2 == id22)) || ((id1 == id22) && (id2 == id12))) {
					std::cout << "Arete: " << i << " & " << j << " Noeud A1: " << id1 << " & " << id2 << " Noeud A2: " << id12 << " & " << id22 << std::endl;
					nbAreteDouble++;
				}
			}
		}
		if (nbAreteDouble == 0) {
			if (display) { std::cout << "Aucune" << std::endl; }
		}
		else {
			if (!display) { std::cout << "-----------------------------------------------" << std::endl; }
		}
		if (display) { std::cout << "-----------------------------------------------" << std::endl; }
	}

	void afficherNoeudDouble(bool display=true, std::string nom = "") {
		if (display) {
			std::cout << "-----------------------------------------------" << std::endl;
			std::cout << "Affichage DEBUG Noeud Double: " << nomGraphe << " " << nom << std::endl;
		}
		int nbNoeudDouble = 0;
		for (int i = 0; i < _noeuds.size(); i++) {
			if (_noeuds[i].getEmplacement() != nullptr) {
				int id1 = _noeuds[i].getEmplacement()->getId();
				for (int j = i + 1; j < _noeuds.size(); j++) {
					if (_noeuds[j].getEmplacement() != nullptr) {
						int id2 = _noeuds[j].getEmplacement()->getId();
						if (id1 == id2) {
							std::cout << "Noeud: " << i << " & " << j << " Emplacement: " << id1 << std::endl;
							nbNoeudDouble++;
						}
					}
				}
			}
		}
		if (nbNoeudDouble == 0) {
			if (display) { std::cout << "Aucun" << std::endl; }
		}
		else {
			if (!display) { std::cout << "-----------------------------------------------" << std::endl; }
		}
		if (display) { std::cout << "-----------------------------------------------" << std::endl; }
	}

	void debugDesyncNoeudEmplacement(bool display=true, std::string nom = "") {
		if (display) { 
			std::cout << "-----------------------------------------------" << std::endl;
			std::cout << "Affichage DEBUG Sync Noeud Emplacement: " << nomGraphe << " " << nom << std::endl;
		}
		long nbWrongSync = 0;
		for (int i=0;i<_noeuds.size();i++) {
			if (_noeuds[i]._emplacement != nullptr) {
				if (_noeuds[i]._id != _emplacementsPossibles[_noeuds[i]._emplacement->_id]._noeud->_id) {
					std::cout << "Noeud: " << i << " NodeEmpId: " << _noeuds[i]._emplacement->_id << " EmpNodeId: " << _emplacementsPossibles[_noeuds[i]._emplacement->_id]._noeud->_id << std::endl;
					nbWrongSync++;
				}
			}
		}
		for (int i=0;i<_emplacementsPossibles.size();i++) {
			if (!_emplacementsPossibles[i].estDisponible()) {
				if (_emplacementsPossibles[i]._id != _noeuds[_emplacementsPossibles[i]._noeud->_id]._emplacement->_id) {
					std::cout << "Emplacement: " << i << " EmpNodeId: " << _emplacementsPossibles[_noeuds[i]._emplacement->_id]._noeud->_id << " NodeEmpId: " << _noeuds[i]._emplacement->_id << std::endl;
					nbWrongSync++;
				}
			}
		}
		if (nbWrongSync == 0) {
			if (display) { std::cout << "Aucun" << std::endl; }
		}
		else {
			if (!display) { std::cout << "-----------------------------------------------" << std::endl; }
		}
		if (display) { std::cout << "-----------------------------------------------" << std::endl; }
	}

	void debugScoreNoeud(bool display=true, std::string nom = "") {
		if (display) {
			std::cout << "-----------------------------------------------" << std::endl;
			std::cout << "Affichage DEBUG Score Noeud: " << nomGraphe << " " << nom << std::endl;
		}
		long nbWrongScore = 0;
		for (int i = 0; i < _noeuds.size(); i++) {
			if (_noeuds[i].getEmplacement() != nullptr) {
				long scoreReel = getScoreCroisementNode(i);
				long scoreArrays = getScoreCroisementNodeFromArrays(i);
				if ((scoreReel != _noeuds[i].score)||(scoreReel != scoreArrays)) {
					std::cout << "Noeud: " << i << " Score:" << _noeuds[i].score << " Score Reel: " << scoreReel << " Score Arrays: " << scoreArrays << std::endl;
					nbWrongScore++;
				}
			}
		}
		if (nbWrongScore == 0) {
			if (display) { std::cout << "Aucun" << std::endl; }
		}
		else {
			if (!display) { std::cout << "-----------------------------------------------" << std::endl; }
		}
		if (display) { std::cout << "-----------------------------------------------" << std::endl; }
	}

	void debugInterArrays(bool display=true, std::string nom = "") {
		if (display) {
			std::cout << "-----------------------------------------------" << std::endl;
			std::cout << "Affichage DEBUG Inter Arrays: " << nomGraphe << " " << nom << std::endl;
		}
		long nbWrongArray = 0;
		for (int i = 0; i < _liens.size(); i++) {
			for (const int& idOtherArray : _liens[i].intersections) {
				if (_liens[idOtherArray].intersections.count(i) == 0) {
					std::cout << "Inter: " << i << "&" << idOtherArray << std::endl;
					nbWrongArray++;
				}
				if (idOtherArray == i) {
					std::cout << "Inter: " << i << " self in array: " << idOtherArray << std::endl;
					nbWrongArray++;
				}
			}
			for (const int& idOtherArray : _liens[i].intersectionsIll) {
				if (_liens[idOtherArray].intersectionsIll.count(i) == 0) {
					std::cout << "InterIll: " << i << "&" << idOtherArray << std::endl;
					nbWrongArray++;
				}
				if (idOtherArray == i) {
					std::cout << "InterIll: " << i << " self in array: " << idOtherArray << std::endl;
					nbWrongArray++;
				}
			}
			for (const int& idOtherArray : _liens[i].intersectionsIllSelf) {
				if (_liens[idOtherArray].intersectionsIllSelf.count(i) == 0) {
					std::cout << "InterIllSelf: " << i << "&" << idOtherArray << std::endl;
					nbWrongArray++;
				}
				if (idOtherArray == i) {
					std::cout << "InterIllSelf: " << i << " self in array: " << idOtherArray << std::endl;
					nbWrongArray++;
				}
			}
		}
		if (nbWrongArray == 0) {
			if (display) { std::cout << "Aucun" << std::endl; }
		}
		else {
			if (!display) { std::cout << "-----------------------------------------------" << std::endl; }
		}
		if (display) { std::cout << "-----------------------------------------------" << std::endl; }
	}

	void debugScoreGraphe(bool display=true, bool useArray = false, std::string nom = "") {
		if (display) { 
			std::cout << "-----------------------------------------------" << std::endl;
			std::cout << "Affichage DEBUG Score Graphe: " << nomGraphe << " " << nom << std::endl;
		}
		bool scoreFaux = true;
		long scoreReel = getNbCroisementConst();
		if (useArray) {
			long scoreArray = getNbCroisementArray();
			if ((nombreCroisement != scoreReel)||(nombreCroisement != scoreArray)) {
				std::cout << "Croisement: " << nombreCroisement << " reel: " << scoreReel << " array: " << scoreArray << std::endl;
				if (!display) { std::cout << "-----------------------------------------------" << std::endl; }
				scoreFaux = false;
			}
		}
		else {
			if (nombreCroisement != scoreReel) {
				std::cout << "Croisement: " << nombreCroisement << " reel: " << scoreReel << std::endl;
				if (!display) { std::cout << "-----------------------------------------------" << std::endl; }
				scoreFaux = false;
			}
		}
		if (scoreFaux) {
			if (display) { std::cout << "Score Correct" << std::endl; }
		}
		if (display) { std::cout << "-----------------------------------------------" << std::endl; }
	}

	void debugNoeudNonPlace(bool display=true, std::string nom = "") {
		if (display) {
			std::cout << "-----------------------------------------------" << std::endl;
			std::cout << "Affichage DEBUG Noeud non place: " << nomGraphe << " " << nom << std::endl;
		}
		long nbFail = 0;
		for (int i = 0; i < _noeuds.size(); i++) {
			if (_noeuds[i].getEmplacement() == nullptr) {
				std::cout << "Noeud: " << i << std::endl;
				nbFail++;
			}
		}
		if (nbFail == 0) {
			if (display) { std::cout << "Aucun" << std::endl; }
		}
		else {
			if (!display) { std::cout << "-----------------------------------------------" << std::endl; }
		}
		if (display) { std::cout << "-----------------------------------------------" << std::endl; }
	}

	void debugEverything() {
		afficherAreteDouble(false);
		afficherNoeudDouble(false);
		afficherNoeudSeul(false);
		debugNoeudNonPlace(false);
		debugDesyncNoeudEmplacement(false);
		if (isNodeScoreUpdated) {
			debugScoreNoeud(false);
		}
		if (isNombreCroisementUpdated) {
			if (isIntersectionVectorUpdated)
				debugScoreGraphe(false,true);
			else
				debugScoreGraphe(false);
		}
		if (isIntersectionVectorUpdated) {
			debugInterArrays(false);
		}
	}

	// Fait une estimation du temps requis pour effectuer un recuit simule complet
	void tempsCalculRecuitSimule(double cool = 0.99999, double t = 100, int mode = 0) {
		std::vector<int> bestResult = saveCopy();
		int nbCroisement = getNbCroisement();

		auto start = std::chrono::system_clock::now();
		// DEBUT RECUIT UNE ITERATION
		for (int iter = 0; iter < 10000 && nbCroisement > 0; iter++) {
			int randomId;
			if (mode == 0)
				randomId = generateRand(_noeuds.size() - 1); // Selection aléatoire du noeud
			else if (mode == 1) {
				randomId = selectionNoeudTournoiBinaire();
			}
			else if (mode == 2) {
				int nbTirageNoeud = ((100 - t) / 15) + 1;
				randomId = selectionNoeudTournoiMultiple(nbTirageNoeud);
			}
			int randomEmpId = generateRand(_emplacementsPossibles.size() - 1); // Selection aléatoire d'un emplacement disponible (pas tres équiprobable)
			// on retire si on pioche le meme emplacement
			while (_noeuds[randomId].getEmplacement()->getId() == randomEmpId) {
				randomEmpId = generateRand(_emplacementsPossibles.size() - 1);
			}
			int scoreNode;
			bool swapped = false;
			int idSwappedNode = -1;
			Emplacement* oldEmplacement = _noeuds[randomId].getEmplacement();
			if (!_emplacementsPossibles[randomEmpId].estDisponible()) {
				idSwappedNode = _emplacementsPossibles[randomEmpId]._noeud->getId();
				scoreNode = getScoreCroisementNode(randomId, idSwappedNode);
				scoreNode += getScoreCroisementNode(idSwappedNode);
				_noeuds[randomId].swap(&_emplacementsPossibles[randomEmpId]);
				swapped = true;
			}
			else {
				scoreNode = getScoreCroisementNode(randomId);
				_noeuds[randomId].setEmplacement(&_emplacementsPossibles[randomEmpId]);
			}
			int newScoreNode;
			if (swapped) {
				newScoreNode = getScoreCroisementNode(randomId, idSwappedNode);
				newScoreNode += getScoreCroisementNode(idSwappedNode);
			}
			else {
				newScoreNode = getScoreCroisementNode(randomId);
			}
			int improve = newScoreNode - scoreNode;
			if (improve < 0) {
				bestResult = saveCopy();
				nbCroisement += improve;
			}
			else {
				double randDouble = generateDoubleRand(1.0);
				if (randDouble >= exp(-improve / t)) {
					if (swapped) {
						_noeuds[randomId].swap(oldEmplacement);
					}
					else {
						_noeuds[randomId].setEmplacement(oldEmplacement);
					}
				}
				else {
					nbCroisement += improve;
				}
			}
			t *= cool;
		}
		// FIN RECUIT 100 ITERATIONS
		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> secondsTotal = end - start;
		std::cout << "Prevision: " << secondsTotal.count() * 138.1545 << " secondes." << std::endl;
	}

	// Enleve tout les noeuds de leur emplacement.
	void clearNodeEmplacement() {
		for (int i = 0; i < _noeuds.size(); i++) {
			_noeuds[i].clearEmplacement();
			_noeuds[i].score = -1;
		}
		for (int i = 0;i < _liens.size();i++) {
			_liens[i].clearIntersectionsVector();
		}
		nombreCroisement = -1;
		isNombreCroisementUpdated = false;
		isNodeScoreUpdated = false;
		isIntersectionVectorUpdated = false;
	}

	// Place les noeuds aleatoirement sur les emplacements disponibles.
	// Ne tient pas a jour le score des noeuds ou du graphe.
	void placementAleatoire() {
		if (DEBUG_GRAPHE) std::cout << "Placement aleatoire" << std::endl;
		for (int i = 0; i < _noeuds.size(); ++i) {
			int emplacementId = generateRand(_emplacementsPossibles.size() - 1);
			while (!_emplacementsPossibles[emplacementId].estDisponible()) {
				emplacementId = (emplacementId + 1) % _emplacementsPossibles.size();
			}
			_noeuds[i].setEmplacement(&_emplacementsPossibles[emplacementId]);
		}
		isNombreCroisementUpdated = false;
		isNodeScoreUpdated = false;
		isIntersectionVectorUpdated = false;
	}

	// Place les noeuds sur l'emplacement de meme ID
	// Ne tient pas a jour le score des noeuds ou du graphe.
	void placementFixe() {
		if (DEBUG_GRAPHE) std::cout << "Placement fixe" << std::endl;
		for (int i=0;i<_noeuds.size();i++) {
			_noeuds[i].setEmplacement(&_emplacementsPossibles[i]);
		}
		isNombreCroisementUpdated = false;
		isNodeScoreUpdated = false;
		isIntersectionVectorUpdated = false;
	}

	// Place un noeud aleatoirement sur un emplacement disponible.
	// Ne tient pas a jour le score du noeud ou du graphe.
	void placementNoeudAleatoire(int idNoeud) {
		int emplacementId = generateRand(_emplacementsPossibles.size() - 1);
		while (!_emplacementsPossibles[emplacementId].estDisponible()) {
			emplacementId = (emplacementId + 1) % _emplacementsPossibles.size();
		}
		_noeuds[idNoeud].setEmplacement(&_emplacementsPossibles[emplacementId]);
		isNombreCroisementUpdated = false;
		isNodeScoreUpdated = false;
		isIntersectionVectorUpdated = false;
	}

	// Indique si le graphe contient une intersection illegale.
	bool hasIllegalCrossing() {
		for (int i = 0; i < _liens.size() - 1; ++i) {
			for (int j = i + 1; j < _liens.size(); ++j) {
				//Aretes aretes1 = _liens[i], aretes2 = _liens[j];
				if (!(_liens[i].contains(_liens[j].getNoeud1()) || _liens[i].contains(_liens[j].getNoeud2()))) {
					if (seCroisent(_liens[i], _liens[j])) {
						if (surSegment(_liens[i], *_liens[j].getNoeud1()) || surSegment(_liens[i], *_liens[j].getNoeud2())) {
							return true;
						}
						else if (surSegment(_liens[j], *_liens[i].getNoeud1()) || surSegment(_liens[j], *_liens[i].getNoeud2())) {
							return true;
						}
					}
				}
				else {
					Noeud* nodeNotInCommon = _liens[j].nodeNotInCommon(&_liens[i]);
					if (surSegment(_liens[i], *nodeNotInCommon)) {
						return true;
					}
					else {
						nodeNotInCommon = _liens[i].nodeNotInCommon(&_liens[j]);
						if (surSegment(_liens[j], *nodeNotInCommon)) {
							return true;
						}
					}
				}
			}
		}
		return false;
	}

	// Calcule le score d'intersection du graphe et le met a jour.
	long getNbCroisement() {
		long total = 0;
		for (int i = 0; i < _liens.size() - 1; ++i) {
			for (int j = i + 1; j < _liens.size(); ++j) {
				if (!(_liens[i].contains(_liens[j].getNoeud1()) || _liens[i].contains(_liens[j].getNoeud2()))) {
					if (seCroisent(_liens[i], _liens[j])) {
						if (surSegment(_liens[i], *_liens[j].getNoeud1()) || surSegment(_liens[i], *_liens[j].getNoeud2())) {
							total += PENALITE_MAX;
						}
						else if (surSegment(_liens[j], *_liens[i].getNoeud1()) || surSegment(_liens[j], *_liens[i].getNoeud2())) {
							total += PENALITE_MAX;
						}
						else {
							++total;
						}
					}
				}
				else {
					Noeud* nodeNotInCommon = _liens[j].nodeNotInCommon(&_liens[i]);
					if (surSegment(_liens[i], *nodeNotInCommon)) {
						total += PENALITE_MAX_SELF;
					}
					else {
						nodeNotInCommon = _liens[i].nodeNotInCommon(&_liens[j]);
						if (surSegment(_liens[j], *nodeNotInCommon)) {
							total += PENALITE_MAX_SELF;
						}
					}
				}
			}
		}
		nombreCroisement = total;
		isNombreCroisementUpdated = true;
		return total;
	}

	// Calcule le score d'intersection pour un graphe qui n'est pas forcement entierement place
	// Ne met pas a jour le nombre de croisement.
	long getNbCroisementGlouton() {
		long total = 0;
		for (int i = 0; i < _liens.size() - 1; ++i) {
			if (_liens[i].estPlace()) {
				for (int j = i + 1; j < _liens.size(); ++j) {
					if (_liens[j].estPlace()) {
						if (!(_liens[i].contains(_liens[j].getNoeud1()) || _liens[i].contains(_liens[j].getNoeud2()))) {
							if (seCroisent(_liens[i], _liens[j])) {
								if (surSegment(_liens[i], *_liens[j].getNoeud1()) || surSegment(_liens[i], *_liens[j].getNoeud2()))
								{
									total += PENALITE_MAX;
								}
								else if (surSegment(_liens[j], *_liens[i].getNoeud1()) || surSegment(_liens[j], *_liens[i].getNoeud2())) {
									total += PENALITE_MAX;
								}
								else {
									++total;
								}
							}
						}
						else {
							Noeud* nodeNotInCommon = _liens[j].nodeNotInCommon(&_liens[i]);
							if (surSegment(_liens[i], *nodeNotInCommon)) {
								total += PENALITE_MAX_SELF;
							}
							else {
								nodeNotInCommon = _liens[i].nodeNotInCommon(&_liens[j]);
								if (surSegment(_liens[j], *nodeNotInCommon)) {
									total += PENALITE_MAX_SELF;
								}
							}
						}
					}
				}
			}
		}
		return total;
	}

	// Calcule le score d'intersection pour un noeud dans un graphe qui n'est pas forcement entierement place
	// Ne met pas a jour le nombre de croisement
	long getNbCroisementGloutonScore(int nodeId) {
		long score = 0;
		std::vector<int> indexPasse;
		for (int i = 0; i < _noeuds[nodeId]._aretes.size(); ++i) {
			int index = _noeuds[nodeId]._aretes[i];
				if (_liens[index].estPlace()) {
				for (int j = 0; j < _liens.size(); ++j) {
					if (_liens[j].estPlace()) {
						if ((index != j) && (!isInVector(indexPasse, j))) {
							if (!(_liens[index].contains(_liens[j].getNoeud1()) || _liens[index].contains(_liens[j].getNoeud2()))) {
								if (seCroisent(_liens[index], _liens[j])) {
									if (surSegment(_liens[index], *_liens[j].getNoeud1()) || surSegment(_liens[index], *_liens[j].getNoeud2())) {
										score += PENALITE_MAX;
									}
									else if (surSegment(_liens[j], *_liens[index].getNoeud1()) || surSegment(_liens[j], *_liens[index].getNoeud2())) {
										score += PENALITE_MAX;
									}
									else {
										score++;
									}
								}
							}
							else {
								Noeud* nodeNotInCommon = _liens[j].nodeNotInCommon(&_liens[index]);
								if (surSegment(_liens[index], *nodeNotInCommon)) {
									score += PENALITE_MAX_SELF;
								}
								else {
									nodeNotInCommon = _liens[index].nodeNotInCommon(&_liens[j]);
									if (surSegment(_liens[j], *nodeNotInCommon)) {
										score += PENALITE_MAX_SELF;
									}
								}
							}
						}
					}
				}
				indexPasse.push_back(index);
			}
		}
		return score;
	}

	// Selectionne deux noeud et retourne celui avec le score le plus faible. (le moin d'intersection)
	int selectionNoeudTournoiBinaire(bool isScoreUpdated=false) {
		int randomId = generateRand(_noeuds.size() - 1);
		int randomId2;
		do {
			randomId2 = generateRand(_noeuds.size() - 1);
		} while (randomId2 == randomId);
		if (isScoreUpdated) {
			if (_noeuds[randomId].score > _noeuds[randomId2].score)
				return randomId;
		}
		else {
		if (getScoreCroisementNode(randomId) > getScoreCroisementNode(randomId2))
			return randomId;
		}
		return randomId2;
	}

	// Selectionne n noeuds et retourne celui avec le score le plus faible. (le moin d'intersection)
	int selectionNoeudTournoiMultiple(int n, bool isScoreUpdated=false) {
		int randomId = generateRand(_noeuds.size() - 1);
		int scoreMeilleur;
		if (isScoreUpdated)
			scoreMeilleur = _noeuds[randomId].score;
		else
			scoreMeilleur = getScoreCroisementNode(randomId);
		if (n > 1) {
			for (int i = 0; i < n; i++) {
				int randomId2;
				do {
					randomId2 = generateRand(_noeuds.size() - 1);
				} while (randomId2 == randomId);
				int scoreId2;
				if (isScoreUpdated)
					scoreId2 = _noeuds[randomId2].score;
				else
					scoreId2 = getScoreCroisementNode(randomId2);
				if (scoreId2 > scoreMeilleur) {
					randomId = randomId2;
					scoreMeilleur = scoreId2;
				}
			}
		}
		return randomId;
	}

	// Selectionne deux emplacements different entre eux et de celui du noeud, puis renvoie le plus proche du noeud.
	// Le noeud doit etre place.
	int selectionEmplacementTournoiBinaire(int nodeId) {
		int randomEmpId = generateRand(_emplacementsPossibles.size() - 1); // Selection aléatoire d'un emplacement disponible (pas tres équiprobable)
		// on retire si on pioche le meme emplacement
		while (_noeuds[nodeId].getEmplacement()->getId() == randomEmpId) {
			randomEmpId = generateRand(_emplacementsPossibles.size() - 1);
		}
		int randomEmpId2 = generateRand(_emplacementsPossibles.size() - 1); // Selection aléatoire d'un emplacement disponible (pas tres équiprobable)
		// on retire si on pioche le meme emplacement
		while ((_noeuds[nodeId].getEmplacement()->getId() == randomEmpId2)||(randomEmpId2 == randomEmpId)) {
			randomEmpId2 = generateRand(_emplacementsPossibles.size() - 1);
		}
		double dist1 = ((_emplacementsPossibles[randomEmpId].getX() - _noeuds[nodeId].getX()) * (_emplacementsPossibles[randomEmpId].getX() - _noeuds[nodeId].getX())) + ((_emplacementsPossibles[randomEmpId].getY() - _noeuds[nodeId].getY()) * (_emplacementsPossibles[randomEmpId].getY() - _noeuds[nodeId].getY()));
		double dist2 = ((_emplacementsPossibles[randomEmpId2].getX() - _noeuds[nodeId].getX()) * (_emplacementsPossibles[randomEmpId2].getX() - _noeuds[nodeId].getX())) + ((_emplacementsPossibles[randomEmpId2].getY() - _noeuds[nodeId].getY()) * (_emplacementsPossibles[randomEmpId2].getY() - _noeuds[nodeId].getY()));
		if (dist2 < dist1) {
			randomEmpId = randomEmpId2;
		}
		return randomEmpId;
	}

	// Selectionne n emplacements different entre eux et de celui du noeud, puis renvoie le plus proche du noeud.
	// Le noeud doit etre place.
	int selectionEmplacementTournoiMultiple(int n, int nodeId) {
		int randomEmpId = generateRand(_emplacementsPossibles.size() - 1); // Selection aléatoire d'un emplacement disponible (pas tres équiprobable)
		// on retire si on pioche le meme emplacement
		while (_noeuds[nodeId].getEmplacement()->getId() == randomEmpId) {
			randomEmpId = generateRand(_emplacementsPossibles.size() - 1);
		}
		double dist;
		if (n > 1) {
			dist = ((_emplacementsPossibles[randomEmpId].getX() - _noeuds[nodeId].getX()) * (_emplacementsPossibles[randomEmpId].getX() - _noeuds[nodeId].getX())) + ((_emplacementsPossibles[randomEmpId].getY() - _noeuds[nodeId].getY()) * (_emplacementsPossibles[randomEmpId].getY() - _noeuds[nodeId].getY()));
		}
		for (int i = 1; i < n; i++) {
			int randomEmpId2 = generateRand(_emplacementsPossibles.size() - 1); // Selection aléatoire d'un emplacement disponible (pas tres équiprobable)
			// on retire si on pioche le meme emplacement
			while ((_noeuds[nodeId].getEmplacement()->getId() == randomEmpId2)||(randomEmpId2 == randomEmpId)) {
				randomEmpId2 = generateRand(_emplacementsPossibles.size() - 1);
			}
			double dist2 = ((_emplacementsPossibles[randomEmpId2].getX() - _noeuds[nodeId].getX()) * (_emplacementsPossibles[randomEmpId2].getX() - _noeuds[nodeId].getX())) + ((_emplacementsPossibles[randomEmpId2].getY() - _noeuds[nodeId].getY()) * (_emplacementsPossibles[randomEmpId2].getY() - _noeuds[nodeId].getY()));
			if (dist2 < dist) {
				randomEmpId = randomEmpId2;
				dist = dist2;
			}
		}
		return randomEmpId;
	}

	// Effectue la selection du noeud en fonction de modeNoeud, 0=Aleatoire,1=TournoiBinaire,2=TournoiMultiple
	int selectionNoeud(int modeNoeud, int t, bool isScoreUpdated=false) {
		int nodeId;
		if (modeNoeud == 0)
			nodeId = generateRand(_noeuds.size() - 1);
		else if (modeNoeud == 1) {
			nodeId = selectionNoeudTournoiBinaire(isScoreUpdated);
		}
		else if (modeNoeud == 2) {
			int nbTirageNoeud = ((100 - t) / 15) + 1;
			nodeId = selectionNoeudTournoiMultiple(nbTirageNoeud,isScoreUpdated);
		}
		return nodeId;
	}

	// Effectue la selection de l'emplacement en fonction de modeEmplacement, 0=Aleatoire,1=TournoiBinaire,2=TournoiMultiple
	int selectionEmplacement(int modeEmplacement, int nodeId, int t) {
		int slotId;
		if (modeEmplacement == 0) {
			slotId = generateRand(_emplacementsPossibles.size() - 1); // Selection aléatoire d'un emplacement disponible (pas tres équiprobable)
			// on retire si on pioche le meme emplacement
			while (_noeuds[nodeId].getEmplacement()->getId() == slotId) {
				slotId = generateRand(_emplacementsPossibles.size() - 1);
			}
		}
		else if (modeEmplacement == 1) {
			slotId = selectionEmplacementTournoiBinaire(nodeId);
		}
		else if (modeEmplacement == 2) {
			int nbTirage = ((100 - t) / 15) + 1;
			slotId = selectionEmplacementTournoiMultiple(nbTirage, nodeId);
		}
		return slotId;
	}

	// Lance l'algorithme de recuit simulé sur le graphe pour minimiser le nombre d'intersection
	// Met à jour la variable nombreCroisement du graphe.
	// delay est le nombre de tour auquel on reste à la même température, -1 pour le rendre dynamique en fonction de la taille du graphe.
	// modeNoeud et modeEMplacement sont le mode de sélection de noeud et d'emplacement, 0=Aléatoire, 1=TournoiBinaire, 2=TournoiMultiple
	void recuitSimule(double &timeBest, double cool = 0.99999, double t = 100.0, int delay = 1, int modeNoeud = 0, int modeEmplacement = 0) {
		auto start = std::chrono::system_clock::now();
		auto end = start;
		std::vector<int> bestResult = saveCopy();
		long nbCroisement;
		if (isNombreCroisementUpdated) {
			nbCroisement = nombreCroisement;
		}
		else {
			nbCroisement = getNbCroisement();
		}
		long bestCroisement = nbCroisement;
		if (delay == -1) { // -1 on calcule le delay en fonction de la taille du graphe
			delay = ceil((double)_noeuds.size() / 20.0) + 1;
		}
		if (DEBUG_GRAPHE) std::cout << "Nb Croisement avant recuit: " << nbCroisement << std::endl;
		for (int iter = 0; t > 0.0001 && nbCroisement > 0; iter++) {
			//if (iter % 100000 == 0) {
				//std::cout << "Iter: " << iter << " t: " << t << " intersections: " << nbCroisement << std::endl;
			//}
			for (int del = 0; del < delay; del++) {
				int nodeId = selectionNoeud(modeNoeud, t);
				int slotId = selectionEmplacement(modeEmplacement, nodeId, t);
				int scoreNode;
				bool swapped = false;
				int idSwappedNode = -1;
				Emplacement* oldEmplacement = _noeuds[nodeId].getEmplacement();
				if (!_emplacementsPossibles[slotId].estDisponible()) {
					idSwappedNode = _emplacementsPossibles[slotId]._noeud->getId();
					scoreNode = getScoreCroisementNode(nodeId, idSwappedNode);
					scoreNode += getScoreCroisementNode(idSwappedNode);
					_noeuds[nodeId].swap(&_emplacementsPossibles[slotId]);
					swapped = true;
				}
				else {
					scoreNode = getScoreCroisementNode(nodeId);
					_noeuds[nodeId].setEmplacement(&_emplacementsPossibles[slotId]);
				}
				int newScoreNode;
				if (swapped) {
					newScoreNode = getScoreCroisementNode(nodeId, idSwappedNode);
					newScoreNode += getScoreCroisementNode(idSwappedNode);
				}
				else {
					newScoreNode = getScoreCroisementNode(nodeId);
				}
				int improve = newScoreNode - scoreNode;
				if (improve < 0) {
					nbCroisement += improve;
					if (nbCroisement < bestCroisement) {
						bestCroisement = nbCroisement;
						bestResult = saveCopy();
						end = std::chrono::system_clock::now();
					}
					if (DEBUG_GRAPHE) std::cout << "Graphe nnode: " << _noeuds.size() << " best score: " << nbCroisement << " iter: " << iter << " t: " << t << std::endl;
				}
				else {
					double randDouble = generateDoubleRand(1.0);
					if (randDouble >= exp(-improve / t)) {
						if (swapped) {
							_noeuds[nodeId].swap(oldEmplacement);
						}
						else {
							_noeuds[nodeId].setEmplacement(oldEmplacement);
						}
					}
					else {
						nbCroisement += improve;
					}
				}
			}
			t *= cool;
		}
		loadCopy(bestResult);
		nombreCroisement = bestCroisement;
		isNombreCroisementUpdated = true;
		isNodeScoreUpdated = false;
		isIntersectionVectorUpdated = false;
		std::chrono::duration<double> secondsTotal = end - start;
		timeBest = secondsTotal.count();
		if (DEBUG_GRAPHE) std::cout << "Meilleur resultat du recuit: " << bestCroisement << std::endl;
	}

	// Lance l'algorithme de recuit simulé sur le graphe pour minimiser le nombre d'intersection
	// Met à jour la variable nombreCroisement du graphe si elle etait a jour avant.
	void stepRecuitSimule(double& t, int& nbCrois, double cool = 0.99999, int modeNoeud = 0, int modeEmplacement = 0) {
		std::vector<int> bestResult = saveCopy();
		int nbCroisement = nbCrois;
		if (DEBUG_GRAPHE) std::cout << "Nb Croisement avant recuit: " << nbCroisement << std::endl;
		int randomId = selectionNoeud(modeNoeud, t);
		int randomEmpId = selectionEmplacement(modeEmplacement, randomId, t);
		int scoreNode;
		bool swapped = false;
		int idSwappedNode = -1;
		Emplacement* oldEmplacement = _noeuds[randomId].getEmplacement();
		if (!_emplacementsPossibles[randomEmpId].estDisponible()) {
			idSwappedNode = _emplacementsPossibles[randomEmpId]._noeud->getId();
			scoreNode = getScoreCroisementNode(randomId, idSwappedNode);
			scoreNode += getScoreCroisementNode(idSwappedNode);
			_noeuds[randomId].swap(&_emplacementsPossibles[randomEmpId]);
			swapped = true;
		}
		else {
			scoreNode = getScoreCroisementNode(randomId);
			_noeuds[randomId].setEmplacement(&_emplacementsPossibles[randomEmpId]);
		}
		int newScoreNode;
		if (swapped) {
			newScoreNode = getScoreCroisementNode(randomId, idSwappedNode);
			newScoreNode += getScoreCroisementNode(idSwappedNode);
		}
		else {
			newScoreNode = getScoreCroisementNode(randomId);
		}
		int improve = newScoreNode - scoreNode;
		if (improve < 0) {
			bestResult = saveCopy();
			nbCroisement += improve;
		}
		else {
			double randDouble = generateDoubleRand(1.0);
			if (randDouble >= exp(-improve / t)) {
				if (swapped) {
					_noeuds[randomId].swap(oldEmplacement);
				}
				else {
					_noeuds[randomId].setEmplacement(oldEmplacement);
				}
			}
			else {
				nbCroisement += improve;
			}
		}
		t *= cool;
		if (isNombreCroisementUpdated) {
			nombreCroisement = nbCroisement;
			isNombreCroisementUpdated = true;
		}
		loadCopy(bestResult);
	}

	// Applique le recuit simulé plusieurs fois
	// Met a jour le nombre de croisement du graphe.
	void rerecuitSimule(double &timeBest, int iter = 10, double cool = 0.99999, double coolt = 0.99, double t = 100.0, int delay = 1, int modeNoeud = 0, int modeEmplacement = 0) {
		if (DEBUG_GRAPHE) std::cout << "Starting Rerecuit " << iter << " iterations." << std::endl;
		for (int i = 0; i < iter; i++) {
			if (DEBUG_GRAPHE) std::cout << "Starting Recuit Number: " << i << " t: " << t << " cool " << cool << std::endl;
			recuitSimule(timeBest, cool, t, delay, modeNoeud, modeEmplacement);
			t *= coolt;
		}
	}

	// Lance l'algorithme de recuit simulé sur le graphe pour minimiser le nombre d'intersection
	// Met à jour le score du graphe et des noeuds
	// delay est le nombre de tour auquel on reste à la même température, -1 pour le rendre dynamique en fonction de la taille du graphe.
	// modeNoeud et modeEMplacement sont le mode de sélection de noeud et d'emplacement, 0=Aléatoire, 1=TournoiBinaire, 2=TournoiMultiple
	void recuitSimuleScore(double &timeBest, double cool = 0.99999, double t = 100.0, int delay = 1, int modeNoeud = 0, int modeEmplacement = 0) {
		auto start = std::chrono::system_clock::now();
		auto end = start;
		Graphe bestResult;
		bestResult.copyFromGraphe(*this);
		long nbCroisement = nombreCroisement;
		long bestCroisement = nbCroisement;
		if (delay == -1) {
			delay = ceil((double)_noeuds.size() / 20.0) + 1;
		}
		for (int iter = 0; t > 0.0001 && nbCroisement > 0; iter++) {
			for (int del = 0; del < delay; del++) {
				int nodeId = selectionNoeud(modeNoeud, t, true);
				int slotId = selectionEmplacement(modeEmplacement, nodeId, t);
				int scoreNode;
				bool swapped = false;
				int idSwappedNode = -1;
				Emplacement* oldEmplacement = _noeuds[nodeId].getEmplacement();
				if (!_emplacementsPossibles[slotId].estDisponible()) {
					idSwappedNode = _emplacementsPossibles[slotId]._noeud->getId();
					scoreNode = getNodeLinkedScore(nodeId, idSwappedNode);
					scoreNode += _noeuds[idSwappedNode].score;
					_noeuds[nodeId].swap(&_emplacementsPossibles[slotId]);
					swapped = true;
					changeUpdateValue(nodeId);
					changeUpdateValue(idSwappedNode);
					updateNodeScore(idSwappedNode);
				}
				else {
					scoreNode = _noeuds[nodeId].score;
					_noeuds[nodeId].setEmplacement(&_emplacementsPossibles[slotId]);
					changeUpdateValue(nodeId);
				}
				updateNodeScore(nodeId);
				int newScoreNode;
				if (swapped) {
					newScoreNode = getNodeLinkedScore(nodeId, idSwappedNode);
					newScoreNode += _noeuds[idSwappedNode].score;
				}
				else {
					newScoreNode = _noeuds[nodeId].score;
				}
				int improve = newScoreNode - scoreNode;
				if (improve < 0) {
					nbCroisement += improve;
					if (nbCroisement < bestCroisement) {
						bestCroisement = nbCroisement;
						bestResult.copyFromGraphe(*this);
						end = std::chrono::system_clock::now();
					}
				}
				else {
					double randDouble = generateDoubleRand(1.0);
					if (randDouble >= exp(-improve / t)) {
						changeUpdateValue(nodeId);
						if (swapped) {
							changeUpdateValue(idSwappedNode);
							_noeuds[nodeId].swap(oldEmplacement);
							updateNodeScore(idSwappedNode);
						}
						else {
							_noeuds[nodeId].setEmplacement(oldEmplacement);
						}
						updateNodeScore(nodeId);
					}
					else {
						nbCroisement += improve;
					}
				}
			}
			t *= cool;
		}
		copyFromGraphe(bestResult);
		isNombreCroisementUpdated = true;
		isNodeScoreUpdated = true;
		isIntersectionVectorUpdated = true;
		std::chrono::duration<double> secondsTotal = end - start;
		timeBest = secondsTotal.count();
	}

	// Applique l'algorithme meilleur deplacement sur le graphe.
	// On parcoure tout les noeuds et on teste chaque deplacement possible et on effectue le meilleur s'il ameliore le score. (O(n²*e))
	// Met a jour le nombre de croisement du graphe.
	void bestDeplacement() {
		long nbIntersection;
		if (isNombreCroisementUpdated) {
			nbIntersection = nombreCroisement;
		}
		else {
			nbIntersection = getNbCroisement();
		}
		int bestImprove = 0;
		int indexNoeud = -1, indexEmplacement = -1;
		do {
			bestImprove = 0;
			for (int i = 0; i < _noeuds.size(); i++) {
				for (int j = 0; j < _emplacementsPossibles.size(); j++) {
					if (_noeuds[i].getEmplacement()->getId() != j) {
						int score, newScore, swappedId;
						bool swapped = false;
						Emplacement* oldEmplacement = _noeuds[i].getEmplacement();
						if (_emplacementsPossibles[j].estDisponible()) {
							score = getScoreCroisementNode(i);
							_noeuds[i].setEmplacement(&_emplacementsPossibles[j]);
						}
						else {
							swappedId = _emplacementsPossibles[j]._noeud->getId();
							score = getScoreCroisementNode(i);
							score += getScoreCroisementNode(swappedId, i);
							_noeuds[i].swap(&_emplacementsPossibles[j]);
							swapped = true;
						}
						if (!swapped) {
							newScore = getScoreCroisementNode(i);
							_noeuds[i].setEmplacement(oldEmplacement);
						}
						else {
							newScore = getScoreCroisementNode(i);
							newScore += getScoreCroisementNode(swappedId, i);
							_noeuds[i].swap(oldEmplacement);
						}
						int improve = newScore - score;
						if (improve < bestImprove) {
							bestImprove = improve;
							indexNoeud = i;
							indexEmplacement = j;
						}
					}
				}
			}
			if (bestImprove < 0) {
				if (_emplacementsPossibles[indexEmplacement].estDisponible()) {
					_noeuds[indexNoeud].setEmplacement(&_emplacementsPossibles[indexEmplacement]);
				}
				else {
					_noeuds[indexNoeud].swap(&_emplacementsPossibles[indexEmplacement]);
				}
				nbIntersection += bestImprove;
			}
		} while (bestImprove < 0);
		nombreCroisement = nbIntersection;
		isNombreCroisementUpdated = true;
		isNodeScoreUpdated = false;
		isIntersectionVectorUpdated = false;
		if (DEBUG_GRAPHE) std::cout << "Meilleur resultat de l'algo meilleur deplacement: " << nbIntersection << std::endl;
	}

	// Selectionne les emplacements disponibles a egale distance d'un point et en renvoie un aleatoirement.
	Emplacement* getEmplacementPlusProche(const Point& origin) {
		Emplacement* meilleurEmplacement = &_emplacementsPossibles[generateRand(_emplacementsPossibles.size() - 1)];
		while (!meilleurEmplacement->estDisponible()) {
			meilleurEmplacement = &_emplacementsPossibles[(meilleurEmplacement->getId() + 1) % _emplacementsPossibles.size()];
		}
		int meilleurDistance = meilleurEmplacement->getPosition().distance(origin);
		int nbRencontre = 0;
		for (int i = 0; i < _emplacementsPossibles.size(); ++i) {
			if (_emplacementsPossibles[i].estDisponible() && i != meilleurEmplacement->getId()) {
				int distanceActuel = _emplacementsPossibles[i].getPosition().distance(origin);
				if (meilleurDistance > distanceActuel) {
					meilleurEmplacement = &_emplacementsPossibles[i];
					meilleurDistance = distanceActuel;
					nbRencontre = 0;
				}
				else if (meilleurDistance == distanceActuel) {
					++nbRencontre;
					if (generateRand(nbRencontre) == 1) {
						meilleurEmplacement = &_emplacementsPossibles[i];
						meilleurDistance = distanceActuel;
					}
				}
			}

		}
		return meilleurEmplacement;
	}

	// Selectionne les emplacements different de l'emplacement en parametre a egale distance de celui ci et en renvoie un aleatoirement.
	Emplacement* getEmplacementPlusProche(Emplacement* origin) {
		Emplacement* meilleurEmplacement = &_emplacementsPossibles[0];
		int meilleurDistance = meilleurEmplacement->getPosition().distance(origin->getPosition());
		int nbRencontre = 0;
		for (int i = 1; i < _emplacementsPossibles.size(); ++i) {
			if (origin->getId() != _emplacementsPossibles[i].getId()) {
				int distanceActuel = _emplacementsPossibles[i].getPosition().distance(origin->getPosition());
				if (distanceActuel < meilleurDistance) {
					meilleurEmplacement = &_emplacementsPossibles[i];
					meilleurDistance = distanceActuel;
					nbRencontre = 0;
				}
				else if (meilleurDistance == distanceActuel) {
					++nbRencontre;
					if (generateRand(nbRencontre) == 1) {
						meilleurEmplacement = &_emplacementsPossibles[i];
					}
				}
			}
		}
		return meilleurEmplacement;
	}

	// Retourne le centre de gravite des emplacements.
	Point getCentreGravite() {
		double totalX = 0.0, totalY = 0.0;
		for (int i=0;i<_emplacementsPossibles.size();i++) {
			totalX += _emplacementsPossibles[i].getX();
			totalY += _emplacementsPossibles[i].getY();
		}
		return Point(totalX / _emplacementsPossibles.size(),totalY / _emplacementsPossibles.size());
	}

	// Retourne le centre de gravite des noeuds place
	Point getCentreGraviteNoeudPlaces() {
		double totalX = 0.0, totalY = 0.0;
		for (int i=0;i<_noeuds.size();i++) {
			if (_noeuds[i].estPlace()){
				totalX += _noeuds[i].getX();
				totalY += _noeuds[i].getY();
			}
		}
		return Point(totalX / _emplacementsPossibles.size(),totalY / _emplacementsPossibles.size());
	}

	// Retourne le centre de gravite des voisins place d'un noeud.
	Point getCentreGraviteVoisin(Noeud* noeud) {
		double totalX = 0.0, totalY = 0.0;
		for (int i=0;i<noeud->voisins.size();i++) {
			if (noeud->voisins[i]->estPlace()) {
				totalX += noeud->voisins[i]->getX();
				totalY += noeud->voisins[i]->getY();
			}
		}
		return Point(totalX / _emplacementsPossibles.size(),totalY / _emplacementsPossibles.size());
	}

	// Renvoie l'id du meilleur emplacement disponible
	// Le noeud passé en argument ne doit pas être placé
	int getMeilleurEmplacement(Noeud& meilleurNoeud) {
		int nbRencontre = 0;
		long bestScore = INT_MAX;
		int bestId;
		for (int j = 0; j < _emplacementsPossibles.size(); j++) {
			if (_emplacementsPossibles[j].estDisponible()) {
				meilleurNoeud.setEmplacement(&_emplacementsPossibles[j]);
				long newScore = getNbCroisementGlouton();
				if (newScore < bestScore) {
					bestScore = newScore;
					bestId = j;
					nbRencontre = 1;
				}
				else if (newScore == bestScore) {
					++nbRencontre;
					if (generateRand(nbRencontre) == 1) {
						bestId = j;
					}
				}
			}
		}
		meilleurNoeud.clearEmplacement();
		return _emplacementsPossibles[bestId].getId();
	}

	// Renvoie l'id du meilleur emplacement disponible
	// Le noeud passé en argument ne doit pas être placé
	int getMeilleurEmplacementScore(Noeud& meilleurNoeud) {
		int nbRencontre = 0;
		long bestScore = -1;
		int bestId = -1;
		for (int j = 0; j < _emplacementsPossibles.size(); j++) {
			if (_emplacementsPossibles[j].estDisponible()) {
				if (bestId == -1) {
					bestId = j;
				}
				else {
					if (bestScore == -1) {
						meilleurNoeud.setEmplacement(&_emplacementsPossibles[bestId]);
						bestScore = getNbCroisementGloutonScore(meilleurNoeud._id);
					}
					meilleurNoeud.setEmplacement(&_emplacementsPossibles[j]);
					long newScore = getNbCroisementGloutonScore(meilleurNoeud._id);
					if (newScore < bestScore) {
						bestScore = newScore;
						bestId = j;
						nbRencontre = 1;
					}
					else if (newScore == bestScore) {
						++nbRencontre;
						if (generateRand(nbRencontre) == 1) {
							bestId = j;
						}
					}
				}
			}
		}
		meilleurNoeud.clearEmplacement();
		return bestId;
	}

	// Renvoie le meilleur emplacement disponible
	int getMeilleurEmplacementGravite(Noeud* meilleurNoeud, Point gravite) {
		Emplacement* oldEmplacement = meilleurNoeud->getEmplacement();
		int nbRencontre = 0;
		int bestId = -1;
		long bestScore = INT_MAX;
		long bestDistance;
		for (int i = 0; i < _emplacementsPossibles.size(); i++) {
			if (_emplacementsPossibles[i].estDisponible()) {
				meilleurNoeud->setEmplacement(&_emplacementsPossibles[i]);
				long newScore = getNbCroisementGlouton();
				if (newScore < bestScore) {
					bestScore = newScore;
					bestId = i;
					bestDistance = _emplacementsPossibles[bestId].getPosition().distance(gravite);
				}
				else if (newScore == bestScore) {
					long newDistance = _emplacementsPossibles[i].getPosition().distance(gravite);
					if (newDistance < bestDistance) {
						bestScore = newScore;
						bestId = i;
						bestDistance = newDistance;
					}
				}
			}
		}
		if (oldEmplacement == nullptr) {
			meilleurNoeud->clearEmplacement();
		}
		else {
			meilleurNoeud->setEmplacement(oldEmplacement);
		}
		return bestId;
	}

	// Ancien algorithme glouton non optimisé mais fonctionnel.
	void glouton() {
		for (int i = 0; i < _noeuds.size(); i++) {
			// Selection aléatoire du noeud
			int randomId = generateRand(_noeuds.size() - 1);
			while (_noeuds[randomId].getEmplacement() != nullptr) {
				randomId = (randomId + 1) % _noeuds.size();
			}
			long bestScore = INT_MAX;
			int bestEmpId = -1;
			for (int j = 0; j < _emplacementsPossibles.size(); j++) {
				if (_emplacementsPossibles[j].estDisponible()) {
					_noeuds[randomId].setEmplacement(&_emplacementsPossibles[j]);
					long newScore = getNbCroisementGlouton();
					if (newScore < bestScore) {
						bestScore = newScore;
						bestEmpId = j;
					}
				}
			}
			_noeuds[randomId].setEmplacement(&_emplacementsPossibles[bestEmpId]);
		}
		isNombreCroisementUpdated = false;
		isNodeScoreUpdated = false;
		isIntersectionVectorUpdated = false;
	}

	// Algorithme glouton
	void gloutonRevisite() {
		int idNoeud = 0;
		int nbRencontre = 0;
		for (int i = 1; i < _noeuds.size(); ++i) {
			if (!_noeuds[i].estPlace() &&
				_noeuds[i].getVoisins().size() > _noeuds[idNoeud].getVoisins().size())
			{
				idNoeud = i;
				nbRencontre = 1;
			}
			else if (!_noeuds[i].estPlace() &&
				_noeuds[i].getVoisins().size() == _noeuds[idNoeud].getVoisins().size())
			{
				++nbRencontre;
				int aleatoire = generateRand(nbRencontre);
				if (aleatoire == 1)
				{
					idNoeud = i;
				}
			}
		}

		Point centreGravite = getCentreGravite();
		_noeuds[idNoeud].setEmplacement(getEmplacementPlusProche(centreGravite));

		while (!estPlace())
		{
			Noeud* meilleurNoeud = nullptr;
			nbRencontre = 0;
			for (int i = 0; i < _noeuds.size(); ++i)
			{
				if (!_noeuds[i].estPlace())
				{
					Noeud* currentVoisin = &_noeuds[i];
					if (meilleurNoeud == nullptr)
					{
						meilleurNoeud = currentVoisin;
					}
					else if (meilleurNoeud->getVoisins().size() < currentVoisin->getVoisins().size())
					{
						meilleurNoeud = currentVoisin;
						nbRencontre = 0;
					}
					else if (meilleurNoeud->getVoisins().size() == currentVoisin->getVoisins().size())
					{
						++nbRencontre;
						int aleatoire = generateRand(nbRencontre);
						if (aleatoire == 1)
						{
							meilleurNoeud = currentVoisin;
						}
					}
				}

			}

			if (meilleurNoeud != nullptr)
			{
				int randomEmpId = generateRand(_emplacementsPossibles.size() - 1);
				while (!_emplacementsPossibles[randomEmpId].estDisponible()) {
					randomEmpId = (randomEmpId + 1) % _emplacementsPossibles.size();
				}
				meilleurNoeud->setEmplacement(&_emplacementsPossibles[randomEmpId]);
				long bestScore = getNbCroisementGlouton();
				int bestId = randomEmpId;
				int index = (randomEmpId + 1) % _emplacementsPossibles.size();
				if (emplacementRestant())
				{
					for (int j = 0; j < _emplacementsPossibles.size(); j++) {
						while (!_emplacementsPossibles[index].estDisponible()) {
							index = (index + 1) % _emplacementsPossibles.size();
						}
						meilleurNoeud->setEmplacement(&_emplacementsPossibles[index]);
						long newScore = getNbCroisementGlouton();
						if (newScore < bestScore) {
							bestScore = newScore;
							bestId = index;
						}
						else if (newScore == bestScore)
						{
							++nbRencontre;
							int aleatoire = generateRand(nbRencontre);
							if (aleatoire == 1)
							{
								bestScore = newScore;
								bestId = index;
							}
						}
					}
				}
				meilleurNoeud->setEmplacement(&_emplacementsPossibles[bestId]);
			}

		}
		isNombreCroisementUpdated = false;
		isNodeScoreUpdated = false;
		isIntersectionVectorUpdated = false;
	}

	void gloutonRevisiteGravite()
	{
		int idNoeud = 0;
		int nbRencontre = 0;
		for (int i = 1; i < _noeuds.size(); ++i)
		{
			if (!_noeuds[i].estPlace() &&
				_noeuds[i].getVoisins().size() > _noeuds[idNoeud].getVoisins().size())
			{
				idNoeud = i;
				nbRencontre = 1;
			}
			else if (!_noeuds[i].estPlace() &&
				_noeuds[i].getVoisins().size() == _noeuds[idNoeud].getVoisins().size())
			{
				++nbRencontre;
				int aleatoire = generateRand(nbRencontre);
				if (aleatoire == 1)
				{
					idNoeud = i;
				}
			}
		}

		Point centreGravite = getCentreGravite();
		_noeuds[idNoeud].setEmplacement(getEmplacementPlusProche(centreGravite));

		while (!estPlace())
		{
			Noeud* meilleurNoeud = nullptr;
			nbRencontre = 0;
			for (int i = 0; i < _noeuds.size(); ++i)
			{
				if (!_noeuds[i].estPlace())
				{
					Noeud* currentVoisin = &_noeuds[i];
					if (meilleurNoeud == nullptr)
					{
						meilleurNoeud = currentVoisin;
					}
					else if (meilleurNoeud->getVoisins().size() < currentVoisin->getVoisins().size())
					{
						meilleurNoeud = currentVoisin;
						nbRencontre = 0;
					}
					else if (meilleurNoeud->getVoisins().size() == currentVoisin->getVoisins().size())
					{
						++nbRencontre;
						int aleatoire = generateRand(nbRencontre);
						if (aleatoire == 1)
						{
							meilleurNoeud = currentVoisin;
						}
					}
				}

			}

			if (meilleurNoeud != nullptr)
			{
				centreGravite = getCentreGraviteNoeudPlaces();
				Emplacement* emplacement = getEmplacementPlusProche(centreGravite);
				meilleurNoeud->setEmplacement(emplacement);
			}

		}
		isNombreCroisementUpdated = false;
		isNodeScoreUpdated = false;
		isIntersectionVectorUpdated = false;
	}

	void gloutonRevisiteVoisin()
	{
		int idNoeud = 0;
		int nbRencontre = 0;
		for (int i = 1; i < _noeuds.size(); ++i)
		{
			if (!_noeuds[i].estPlace() &&
				_noeuds[i].getVoisins().size() > _noeuds[idNoeud].getVoisins().size())
			{
				idNoeud = i;
				nbRencontre = 1;
			}
			else if (!_noeuds[i].estPlace() &&
				_noeuds[i].getVoisins().size() == _noeuds[idNoeud].getVoisins().size())
			{
				++nbRencontre;
				int aleatoire = generateRand(nbRencontre);
				if (aleatoire == 1)
				{
					idNoeud = i;
				}
			}
		}

		Point centreGravite = getCentreGravite();
		_noeuds[idNoeud].setEmplacement(getEmplacementPlusProche(centreGravite));

		while (!estPlace())
		{
			Noeud* meilleurNoeud = nullptr;
			nbRencontre = 0;
			for (int i = 0; i < _noeuds.size(); ++i)
			{
				if (!_noeuds[i].estPlace())
				{
					Noeud* currentVoisin = &_noeuds[i];
					if (meilleurNoeud == nullptr)
					{
						meilleurNoeud = currentVoisin;
					}
					else if (meilleurNoeud->getVoisins().size() < currentVoisin->getVoisins().size())
					{
						meilleurNoeud = currentVoisin;
						nbRencontre = 0;
					}
					else if (meilleurNoeud->getVoisins().size() == currentVoisin->getVoisins().size())
					{
						++nbRencontre;
						int aleatoire = generateRand(nbRencontre);
						if (aleatoire == 1)
						{
							meilleurNoeud = currentVoisin;
						}
					}
				}

			}

			if (meilleurNoeud != nullptr)
			{
				centreGravite = getCentreGraviteVoisin(meilleurNoeud);
				Emplacement* emplacement = getEmplacementPlusProche(centreGravite);
				meilleurNoeud->setEmplacement(emplacement);
			}

		}
		isNombreCroisementUpdated = false;
		isNodeScoreUpdated = false;
		isIntersectionVectorUpdated = false;
	}

	void completeBasicGlouton() {
		int nbNode = nbAPlace();
		while (nbNode > 0) {
			Noeud* meilleurNoeud = nullptr;
			int nbRencontre = 0;
			for (int i = 0; i < _noeuds.size(); ++i) {
				if (!_noeuds[i].estPlace()) {
					if (meilleurNoeud == nullptr) {
						meilleurNoeud = &_noeuds[i];
					}
					else if (_noeuds[i].getVoisinsPlaces() > meilleurNoeud->getVoisinsPlaces()) {
						meilleurNoeud = &_noeuds[i];
						nbRencontre = 1;
					}
					else if (_noeuds[i].getVoisinsPlaces() == meilleurNoeud->getVoisinsPlaces()) {
						if (_noeuds[i].getVoisins().size() > meilleurNoeud->getVoisins().size()) {
							meilleurNoeud = &_noeuds[i];
							nbRencontre = 1;
						}
						else if (_noeuds[i].getVoisins().size() == meilleurNoeud->getVoisins().size()) {
							++nbRencontre;
							if (generateRand(nbRencontre) == 1) {
								meilleurNoeud = &_noeuds[i];
							}
						}
					}
				}
			}
			meilleurNoeud->setEmplacement(&_emplacementsPossibles[getMeilleurEmplacement(*meilleurNoeud)]);
			nbNode--;
		}
		isNombreCroisementUpdated = false;
		isNodeScoreUpdated = false;
		isIntersectionVectorUpdated = false;
	}

	void completeBasicGloutonScore(std::vector<int>& vecNode, int tailleMax) {
		std::vector<bool> marque;
		marque.resize(tailleMax);
		for (int i=0;i<tailleMax;i++) {
			int nbRencontre = 0;
			int bestNbVoisinsPlaces = -1;
			int bestNbVoisins = -1;
			int bestNodeId = -1;
			for (int j=0;j<tailleMax;j++) {
				if (!marque[j]) {
					int nodeNbVoisinsPlaces = _noeuds[vecNode[j]].getVoisinsPlaces();
					if (nodeNbVoisinsPlaces > bestNbVoisinsPlaces) {
						bestNbVoisinsPlaces = nodeNbVoisinsPlaces;
						bestNbVoisins = _noeuds[vecNode[j]].voisins.size();
						bestNodeId = j;
					}
					else if (nodeNbVoisinsPlaces == bestNbVoisinsPlaces) {
						int nodeNbVoisins = _noeuds[vecNode[j]].voisins.size();
						if (nodeNbVoisins > bestNbVoisins) {
							bestNbVoisinsPlaces = nodeNbVoisinsPlaces;
							bestNbVoisins = _noeuds[vecNode[j]].voisins.size();
							bestNodeId = j;
						}
						else if (nodeNbVoisins == bestNbVoisins) {
							nbRencontre++;
							if (generateRand(nbRencontre) == 1) {
								bestNbVoisinsPlaces = nodeNbVoisinsPlaces;
								bestNbVoisins = _noeuds[vecNode[j]].voisins.size();
								bestNodeId = j;
							}
						}
					}
				}
			}
			marque[bestNodeId] = true;
			int meilleurEmplacement = getMeilleurEmplacementScore(_noeuds[vecNode[bestNodeId]]);
			_noeuds[vecNode[bestNodeId]].setEmplacement(&_emplacementsPossibles[meilleurEmplacement]);
		}
		isNombreCroisementUpdated = false;
		isNodeScoreUpdated = false;
		isIntersectionVectorUpdated = false;
	}

	void completePlacementAleatoire()
	{
		if (DEBUG_GRAPHE) std::cout << "Placement aleatoire" << std::endl;
		for (int i = 0; i < _noeuds.size(); ++i)
		{
			if (_noeuds[i]._emplacement == nullptr) {
				int emplacementId = generateRand(_emplacementsPossibles.size() - 1);
				while (!_emplacementsPossibles[emplacementId].estDisponible()) {
					emplacementId = (emplacementId + 1) % _emplacementsPossibles.size();
				}
				_noeuds[i].setEmplacement(&_emplacementsPossibles[emplacementId]);
			}
		}
		isNombreCroisementUpdated = false;
		isNodeScoreUpdated = false;
		isIntersectionVectorUpdated = false;
	}

	void completePlacementAleatoireScore(std::vector<int>& vecNode, int tailleMax) {
		for (int i=0;i<tailleMax;i++) {
			int emplacementId = generateRand(_emplacementsPossibles.size() - 1);
			while (!_emplacementsPossibles[emplacementId].estDisponible()) {
				emplacementId = (emplacementId + 1) % _emplacementsPossibles.size();
			}
			_noeuds[vecNode[i]].setEmplacement(&_emplacementsPossibles[emplacementId]);
		}
		isNombreCroisementUpdated = false;
		isNodeScoreUpdated = false;
		isIntersectionVectorUpdated = false;
	}

	// Indique s'il reste au moin un emplacement disponible
	bool emplacementRestant() {
		for (int i=0;i<_emplacementsPossibles.size();i++) {
			if (_emplacementsPossibles[i].estDisponible()) {
				return true;
			}
		}
		return false;
	}

	// Indique si tout les noeuds d'un graphe sont place
	bool estPlace() {
		for (int i=0;i<_noeuds.size();i++) {
			if (!_noeuds[i].estPlace()) return false;
		}
		return true;
	}

	// Renvoie le nombre de noeud non associé à un emplacement.
	int nbAPlace() {
		int nb = 0;
		for (int i=0; i<_noeuds.size();i++) {
			if (!_noeuds[i].estPlace()) nb++;
		}
		return nb;
	}

	// Ajoute (n-1)*_emplacements.size emplacements dans le graphe si possible
	void generateMoreEmplacement(int n) {
		int nbTotal = gridWidth * gridHeight;
		if (n * _emplacementsPossibles.size() > nbTotal) {
			if (DEBUG_GRAPHE) std::cout << "Pas assez de place dans la grille. Grille: " << nbTotal << " " << n << " * emp: " << n * _emplacementsPossibles.size() << std::endl;
		}
		else {
			int nbAjout = (n - 1) * _emplacementsPossibles.size();
			std::vector<std::vector<bool>> marque;
			for (int i = 0; i <= gridHeight; i++) {
				std::vector<bool> tmpVec(gridWidth + 1);
				marque.push_back(tmpVec);
			}
			for (int i = 0; i < _emplacementsPossibles.size(); i++) {
				int x = _emplacementsPossibles[i].getX();
				int y = _emplacementsPossibles[i].getY();
				marque[y][x] = true;
			}
			int x, y;
			for (int i = 0; i < nbAjout; i++) {
				do {
					x = generateRand(gridWidth);
					y = generateRand(gridHeight);
				} while (marque[y][x]);
				marque[y][x] = true;
				_emplacementsPossibles.push_back(Emplacement(Point(x, y), _emplacementsPossibles.size()));
			}
		}
	}

	// Ajoute _noeud.size() emplacement par défaut ou n emplacement 
	// Si le nombre d'emplacement est laissé à -1, on génere dans une grille de taille _noeud.size()*_noeud.size()
	void generateEmplacements(int n = -1) {
		int nbEmplacement = n;
		if (nbEmplacement == -1) {
			gridWidth = _noeuds.size();
			gridHeight = _noeuds.size();
			nbEmplacement = _noeuds.size();
		}
		else {
			int nbTotal = gridWidth * gridHeight;
			if (n + _emplacementsPossibles.size() > nbTotal) {
				if (DEBUG_GRAPHE) std::cout << "Pas assez de place dans la grille. Grille: " << nbTotal << " " << n << " * emp: " << n * _emplacementsPossibles.size() << std::endl;
				return;
			}
		}

		std::vector<std::vector<bool>> marque;
		for (int i = 0; i <= gridHeight; i++) {
			std::vector<bool> tmpVec(gridWidth + 1);
			marque.push_back(tmpVec);
		}
		for (int i = 0; i < _emplacementsPossibles.size(); i++) {
			int x = _emplacementsPossibles[i].getX();
			int y = _emplacementsPossibles[i].getY();
			marque[y][x] = true;
		}
		int x, y;
		for (int i = 0; i < nbEmplacement; i++) {
			do {
				x = generateRand(gridWidth);
				y = generateRand(gridHeight);
			} while (marque[y][x]);
			marque[y][x] = true;
			_emplacementsPossibles.push_back(Emplacement(Point(x, y), _emplacementsPossibles.size()));
		}

	}

	// Sauvegarde dans un vecteur les id des emplacements auquels les noeuds sont assignés
	// Les noeuds doivent etre place
	std::vector<int> saveCopy() {
		std::vector<int> vectorId;
		for (int i = 0; i < _noeuds.size(); i++) {
			vectorId.push_back(_noeuds[i].getEmplacement()->getId());
		}
		return vectorId;
	}

	// Charge une copie en copiant les id dans le vecteur et en les affectant aux noeuds du graphe.
	// La copie doit provenir d'un graphe genere du meme fichier de depart
	void loadCopy(std::vector<int> vectorId) {
		for (int i = 0; i < _noeuds.size(); i++) {
			_noeuds[i].clearEmplacement();
		}
		for (int i = 0; i < _noeuds.size(); i++) {
			_noeuds[i].setEmplacement(&_emplacementsPossibles[vectorId[i]]);
		}
	}

	// Vide tout les vecteurs du graphe
	void clearGraphe() {
		_noeuds.clear();
		_emplacementsPossibles.clear();
		_liens.clear();
	}

	// Vide les vecteurs du graphe et effectue une copie du contenu des vecteur du graphe en parametre
	void copyFromGraphe(Graphe& graphe) {
		_emplacementsPossibles.clear();
		_noeuds.clear();
		_liens.clear();
		for (int i = 0; i < graphe._emplacementsPossibles.size(); ++i) {
			_emplacementsPossibles.push_back(Emplacement(Point(graphe._emplacementsPossibles[i].getX(), graphe._emplacementsPossibles[i].getY()), i));
		}
		for (int i = 0; i < graphe._noeuds.size(); ++i) {
			_noeuds.push_back(Noeud(i));
		}
		for (int i = 0; i < graphe._noeuds.size();i++) {
			_noeuds[i].score = graphe._noeuds[i].score;
		}

		for (int i = 0; i < graphe._liens.size(); ++i) {
			int id1 = graphe._liens[i].getNoeud1()->getId();
			int id2 = graphe._liens[i].getNoeud2()->getId();
			_liens.push_back(Aretes(&_noeuds[id1], &_noeuds[id2], i));
		}
		for (int i = 0; i < graphe._liens.size(); ++i) {
			_liens[i].intersections = graphe._liens[i].intersections;
			_liens[i].intersectionsIll = graphe._liens[i].intersectionsIll;
			_liens[i].intersectionsIllSelf = graphe._liens[i].intersectionsIllSelf;
			_liens[i].isUpdated = graphe._liens[i].isUpdated;
		}
		for (int i = 0; i < graphe._noeuds.size(); ++i) {
			if (graphe._noeuds[i].getEmplacement() != nullptr) {
				int idEmplacement = graphe._noeuds[i].getEmplacement()->getId();
				_noeuds[i].setEmplacement(&_emplacementsPossibles[idEmplacement]);
			}
		}
		nombreCroisement = graphe.nombreCroisement;
		isNombreCroisementUpdated = graphe.isNombreCroisementUpdated;
		isNodeScoreUpdated = graphe.isNodeScoreUpdated;
		isIntersectionVectorUpdated = graphe.isIntersectionVectorUpdated;
		maxVoisin = graphe.maxVoisin;
	}

	// Calcule le score du noeud en fonction de ses vecteur d'intersections
	long getScoreCroisementNodeFromArrays(int nodeIndex) {
		long score = 0;
		std::vector<int> passedIndex;
		for (int i=0;i<_noeuds[nodeIndex]._aretes.size();i++) {
			score += _liens[_noeuds[nodeIndex]._aretes[i]].intersections.size();
			score += _liens[_noeuds[nodeIndex]._aretes[i]].intersectionsIll.size() * PENALITE_MAX;
			for (const int& idArray : _liens[_noeuds[nodeIndex]._aretes[i]].intersectionsIllSelf) {
				if (!isInVector(passedIndex,idArray)) {
					score += PENALITE_MAX_SELF;
				}
			}
			passedIndex.push_back(_noeuds[nodeIndex]._aretes[i]);
		}
		return score;
	}

	// Calcule le score de l'enfant en fonction des aretes passées dans areteVec
	long getNodeScoreEnfant(Graphe& G, std::vector<int>& areteVec, int nodeIndex) {
		long score = 0;
		for (int i = 0; i < G._noeuds[nodeIndex]._aretes.size(); ++i) {
			int index = G._noeuds[nodeIndex]._aretes[i];
			if (G._liens[index].estPlace()) {
				for (int j = 0; j < areteVec.size(); ++j) {
					int index2 = areteVec[j];
					if (!(G._liens[index].contains(_liens[index2].getNoeud1()) || G._liens[index].contains(_liens[index2].getNoeud2()))) {
						if (seCroisent(G._liens[index], _liens[index2])) {
							if (surSegment(G._liens[index], *_liens[index2].getNoeud1()) || surSegment(G._liens[index], *_liens[index2].getNoeud2())) {
								score += PENALITE_MAX;
							}
							else if (surSegment(_liens[index2], *G._liens[index].getNoeud1()) || surSegment(_liens[index2], *G._liens[index].getNoeud2())) {
								score += PENALITE_MAX;
							}
							else {
								score++;
							}
						}
					}
					else {
						Noeud* nodeNotInCommon = _liens[index2].nodeNotInCommon(&G._liens[index]);
						if (surSegment(G._liens[index], *nodeNotInCommon)) {
							score += PENALITE_MAX_SELF;
						}
						else {
							nodeNotInCommon = G._liens[index].nodeNotInCommon(&_liens[index2]);
							if (surSegment(_liens[index2], *nodeNotInCommon)) {
								score += PENALITE_MAX_SELF;
							}
						}
					}
				}
			}
		}
		for (int i=0;i<G._noeuds[nodeIndex]._aretes.size()-1;i++) {
			int index = G._noeuds[nodeIndex]._aretes[i];
			if (G._liens[index].estPlace()) {
				for (int j=i+1;j<G._noeuds[nodeIndex]._aretes.size();j++) {
					int index2 = G._noeuds[nodeIndex]._aretes[j];
					if (G._liens[index2].estPlace()) {
						Noeud* nodeNotInCommon = G._liens[index2].nodeNotInCommon(&G._liens[index]);
						if (surSegment(G._liens[index], *nodeNotInCommon)) {
							score += PENALITE_MAX_SELF;
						}
						else {
							nodeNotInCommon = G._liens[index].nodeNotInCommon(&G._liens[index2]);
							if (surSegment(G._liens[index2], *nodeNotInCommon)) {
								score += PENALITE_MAX_SELF;
							}
						}
					}
				}
			}
		}
		return score;
	}

	// Calcule le score du noeud en parametre.
	long getScoreCroisementNode(int nodeIndex) {
		long score = 0;
		std::vector<int> indexPasse;
		for (int i = 0; i < _noeuds[nodeIndex]._aretes.size(); ++i) {
			int index = _noeuds[nodeIndex]._aretes[i];
			for (int j = 0; j < _liens.size(); ++j) {
				if ((index != j) && (!isInVector(indexPasse, j))) {
					if (!(_liens[index].contains(_liens[j].getNoeud1()) || _liens[index].contains(_liens[j].getNoeud2()))) {
						if (seCroisent(_liens[index], _liens[j])) {
							if (surSegment(_liens[index], *_liens[j].getNoeud1()) || surSegment(_liens[index], *_liens[j].getNoeud2())) {
								score += PENALITE_MAX;
								if (DEBUG_OPENGL) areteIll.insert(&_liens[index]);
							}
							else if (surSegment(_liens[j], *_liens[index].getNoeud1()) || surSegment(_liens[j], *_liens[index].getNoeud2())) {
								score += PENALITE_MAX;
								if (DEBUG_OPENGL) areteIll.insert(&_liens[index]);
							}
							else {
								score++;
								if (DEBUG_OPENGL) areteInter.insert(&_liens[index]);
							}
						}
					}
					else {
						Noeud* nodeNotInCommon = _liens[j].nodeNotInCommon(&_liens[index]);
						if (surSegment(_liens[index], *nodeNotInCommon)) {
							score += PENALITE_MAX_SELF;
							if (DEBUG_OPENGL) areteIllSelf.insert(&_liens[index]);
						}
						else {
							nodeNotInCommon = _liens[index].nodeNotInCommon(&_liens[j]);
							if (surSegment(_liens[j], *nodeNotInCommon)) {
								score += PENALITE_MAX_SELF;
								if (DEBUG_OPENGL) areteIllSelf.insert(&_liens[index]);
							}
						}
					}
				}
			}
			indexPasse.push_back(index);
		}
		return score;
	}

	// Calcule le score du noeud nodeIndex sans ajouter le score produit par le noeud swapIndex.
	int getScoreCroisementNode(int nodeIndex, int swapIndex) {
		long score = 0;
		std::vector<int> indexPasse;
		for (int i = 0; i < _noeuds[nodeIndex]._aretes.size(); ++i) {
			int index = _noeuds[nodeIndex]._aretes[i];
			if (!_liens[index].contains(swapIndex)) {
				for (int j = 0; j < _liens.size(); ++j) {
					if ((index != j) && (!isInVector(indexPasse, j))) {
						if (!_liens[j].contains(swapIndex)) {
							if (!(_liens[index].contains(_liens[j].getNoeud1()) || _liens[index].contains(_liens[j].getNoeud2()))) {
								if (seCroisent(_liens[index], _liens[j])) {
									if (surSegment(_liens[index], *_liens[j].getNoeud1()) || surSegment(_liens[index], *_liens[j].getNoeud2())) {
										score += PENALITE_MAX;
									}
									else if (surSegment(_liens[j], *_liens[index].getNoeud1()) || surSegment(_liens[j], *_liens[index].getNoeud2())) {
										score += PENALITE_MAX;
									}
									else {
										score++;
									}
								}
							}
							else {
								Noeud* nodeNotInCommon = _liens[j].nodeNotInCommon(&_liens[index]);
								if (surSegment(_liens[index], *nodeNotInCommon)) {
									score += PENALITE_MAX_SELF;
								}
								else {
									nodeNotInCommon = _liens[index].nodeNotInCommon(&_liens[j]);
									if (surSegment(_liens[j], *nodeNotInCommon)) {
										score += PENALITE_MAX_SELF;
									}
								}
							}
						}
					}
				}
			}
			indexPasse.push_back(index);
		}
		return score;
	}

	// Effectue le croisement entre deux parents,
	// Renvoie vrai si les deux parents ne sont pas identique
	// Ne met pas à jour le nombre de croisement d'un graphe
	bool croisementVoisinageFrom(Graphe& graphe1, Graphe& graphe2, bool useRand) {
		int nbNoeudATraiter = graphe1._noeuds.size() - graphe1.nbNoeudEnCommun(graphe2);
		//std::cout << "Nb noeud a traiter au debut: " << nbNoeudATraiter << "\n";
		if (nbNoeudATraiter == 0) {
			copyFromGraphe(graphe1);
			return false;
		}
		std::vector<int> saveGraphe1 = graphe1.saveCopy();
		std::vector<int> saveGraphe2 = graphe2.saveCopy();
		Graphe* currentGraphe, * otherGraphe;
		int currentGrapheNumber = generateRand(1);
		if (currentGrapheNumber == 0) { currentGraphe = &graphe1; otherGraphe = &graphe2; }
		else { currentGraphe = &graphe2; otherGraphe = &graphe1; }

		std::vector<int> nodeToRelocate;
		nodeToRelocate.resize(graphe1._noeuds.size());
		while (nbNoeudATraiter > 0) {
			int bestNodeId = -1;
			int meilleurScore;
			int nbRencontre = 0;
			int numberOfNodeToRelocate = 0;
			//Trouve le meilleur noeud du graphe en cours d'analyse
			for (int i = 0; i < _noeuds.size(); ++i) {
				if (!_noeuds[i].estPlace()) {
					int nodeScore = currentGraphe->getScoreCroisementNode(i);
					if (bestNodeId == -1 || nodeScore < meilleurScore) {
						bestNodeId = i;
						meilleurScore = nodeScore;
						nbRencontre = 1;
					}
					else if (meilleurScore == nodeScore) {
						++nbRencontre;
						if (generateRand(nbRencontre) == 1) {
							bestNodeId = i;
						}
					}
				}
			}
			int bestEmplacementId = currentGraphe->_noeuds[bestNodeId].getEmplacement()->getId();
			_noeuds[bestNodeId].setEmplacement(&_emplacementsPossibles[bestEmplacementId]);
			if (!graphe1._noeuds[bestNodeId].compare(&graphe2._noeuds[bestNodeId])) {
				--nbNoeudATraiter;
				int oldNodeId = otherGraphe->_noeuds[bestNodeId].ecraseNoeud(otherGraphe->_emplacementsPossibles[bestEmplacementId]);
				if (oldNodeId != -1) {
					if (!areVoisin(bestNodeId,oldNodeId)) {
						nodeToRelocate[numberOfNodeToRelocate] = oldNodeId;
						numberOfNodeToRelocate++;
					}
				}
			}
			//Place tout les voisins du point choisis
			for (int i=0;i<currentGraphe->_noeuds[bestNodeId].voisins.size();i++) {
				int idNodeVoisin = currentGraphe->_noeuds[bestNodeId].voisins[i]->getId();
				if (!_noeuds[idNodeVoisin].estPlace()) {
					if (currentGraphe->_noeuds[idNodeVoisin].getEmplacement() == nullptr) {
						bestEmplacementId = getMeilleurEmplacement(_noeuds[idNodeVoisin]);
					}
					else {
						bestEmplacementId = currentGraphe->_noeuds[idNodeVoisin].getEmplacement()->getId();
					}
					_noeuds[idNodeVoisin].setEmplacement(&_emplacementsPossibles[bestEmplacementId]);
					if ((!otherGraphe->_noeuds[idNodeVoisin].estPlace())||(!graphe1._noeuds[idNodeVoisin].compare(&graphe2._noeuds[idNodeVoisin]))) {
						--nbNoeudATraiter;
						int oldNodeId = otherGraphe->_noeuds[idNodeVoisin].ecraseNoeud(otherGraphe->_emplacementsPossibles[bestEmplacementId]);
						if (oldNodeId != -1) {
							if (!areVoisin(bestNodeId,oldNodeId)) {
								nodeToRelocate[numberOfNodeToRelocate] = oldNodeId;
								numberOfNodeToRelocate++;
							}
						}
					}
				}
			}
			if (useRand) {
				otherGraphe->completePlacementAleatoire();
			}
			else {
				otherGraphe->completeBasicGlouton();
			}
			//Si les lieux coincident les noeuds ne sont plus à traitercurrentGraphe->_noeuds[idNodeVoisin].ecraseNoeud(currentGraphe->_emplacementsPossibles[bestEmplacementId]);
			for (int i = 0; i < numberOfNodeToRelocate; ++i) {
				if (graphe1._noeuds[nodeToRelocate[i]].compare(&graphe2._noeuds[nodeToRelocate[i]])) {
					--nbNoeudATraiter;
				}
			}


			//Change le parent choisis 
			if (currentGrapheNumber == 0) {
				currentGraphe = &graphe2;
				otherGraphe = &graphe1;
				currentGrapheNumber = 1;
			}
			else {
				currentGraphe = &graphe1;
				otherGraphe = &graphe2;
				currentGrapheNumber = 0;
			}
		}
		_noeuds.swap(graphe1._noeuds);
		_liens.swap(graphe1._liens);
		_emplacementsPossibles.swap(graphe1._emplacementsPossibles);
		graphe1.loadCopy(saveGraphe1);
		graphe2.loadCopy(saveGraphe2);
		isNombreCroisementUpdated = false;
		isNodeScoreUpdated = false;
		isIntersectionVectorUpdated = false;
		return true;
	}

	// Effectue une seule etape dans l'algorithme de croisement, utile pour l'affichage opengl
	void stepCroisementVoisinageFrom(Graphe& graphe1, Graphe& graphe2, bool useRand, int &nbNoeudATraiter, int &currentGrapheNumber) {
		if (nbNoeudATraiter == 0) {
			copyFromGraphe(graphe1);
			return;
		}

		Graphe* currentGraphe, * otherGraphe;
		if (currentGrapheNumber == 0) { currentGraphe = &graphe1; otherGraphe = &graphe2; }
		else { currentGraphe = &graphe2; otherGraphe = &graphe1; }

		int bestNodeId = -1;
		int bestEmplacementId = -1;
		int meilleurScore;
		int nbRencontre = 0;
		int numberOfNodeToRelocate = 0;

		std::vector<int> nodeToRelocate;
		nodeToRelocate.resize(graphe1.maxVoisin+1);

		for (int i = 0; i < _noeuds.size(); ++i) {
			if (!_noeuds[i].estPlace()) {
				int nodeScore = currentGraphe->getScoreCroisementNode(i);
				if (bestNodeId == -1 || nodeScore < meilleurScore) {
					bestNodeId = i;
					meilleurScore = nodeScore;
					nbRencontre = 1;
				}
				else if (meilleurScore == nodeScore) {
					++nbRencontre;
					if (generateRand(nbRencontre) == 1) {
						bestNodeId = i;
					}
				}
			}
		}

		bestEmplacementId = currentGraphe->_noeuds[bestNodeId].getEmplacement()->getId();
		_noeuds[bestNodeId].setEmplacement(&_emplacementsPossibles[bestEmplacementId]);
		if (!graphe1._noeuds[bestNodeId].compare(&graphe2._noeuds[bestNodeId])) {
			--nbNoeudATraiter;
			int oldNodeId = otherGraphe->_noeuds[bestNodeId].ecraseNoeud(otherGraphe->_emplacementsPossibles[bestEmplacementId]);
			if (oldNodeId != -1) {
				if (!areVoisin(bestNodeId,oldNodeId)) {
					nodeToRelocate[numberOfNodeToRelocate] = oldNodeId;
					numberOfNodeToRelocate++;
				}
			}
		}
		//Place tout les voisins du point choisis
		for (int i=0;i<currentGraphe->_noeuds[bestNodeId].voisins.size();i++) {
			int idNodeVoisin = currentGraphe->_noeuds[bestNodeId].voisins[i]->getId();
			if (!_noeuds[idNodeVoisin].estPlace()) {
				if (currentGraphe->_noeuds[idNodeVoisin].getEmplacement() == nullptr) {
					bestEmplacementId = getMeilleurEmplacement(_noeuds[idNodeVoisin]);
				}
				else {
					bestEmplacementId = currentGraphe->_noeuds[idNodeVoisin].getEmplacement()->getId();
				}
				_noeuds[idNodeVoisin].setEmplacement(&_emplacementsPossibles[bestEmplacementId]);
				if ((!otherGraphe->_noeuds[idNodeVoisin].estPlace())||(!graphe1._noeuds[idNodeVoisin].compare(&graphe2._noeuds[idNodeVoisin]))) {
					--nbNoeudATraiter;
					int oldNodeId = otherGraphe->_noeuds[idNodeVoisin].ecraseNoeud(otherGraphe->_emplacementsPossibles[bestEmplacementId]);
					if (oldNodeId != -1) {
						if (!areVoisin(bestNodeId,oldNodeId)) {
							nodeToRelocate[numberOfNodeToRelocate] = oldNodeId;
							numberOfNodeToRelocate++;
						}
					}
				}
			}
		}

		if (useRand) {
			otherGraphe->completePlacementAleatoire();
		}
		else {
			otherGraphe->completeBasicGlouton();
		}
		//Si les lieux coincident les noeuds ne sont plus à traiter
		for (int i = 0; i < numberOfNodeToRelocate; ++i) {
			if (graphe1._noeuds[nodeToRelocate[i]].compare(&graphe2._noeuds[nodeToRelocate[i]])) {
				--nbNoeudATraiter;
			}
		}
		currentGrapheNumber = (currentGrapheNumber+1)%2;
		graphe1.isNombreCroisementUpdated = false;
		graphe2.isNombreCroisementUpdated = false;
		isNombreCroisementUpdated = false;
		if (nbNoeudATraiter == 0) {
			loadCopy(graphe1.saveCopy());
		}
	}

	// Indique aux aretes du noeuds que leur liste d'intersection n'est pas à jours.
	void changeUpdateValue(int idNode) {
		for (int i=0;i<_noeuds[idNode]._aretes.size();i++) {
			_liens[_noeuds[idNode]._aretes[i]].isUpdated = false;
		}
	}

	// Recalcule les intersections d'une arete et met a jour sa liste d'intersection uniquement
	void recalculateInterArray(int idArray) {
		_liens[idArray].clearIntersectionsVector();
		for (int i = 0; i < _liens.size(); ++i) {
			if (i != idArray) {
				if (!(_liens[idArray].contains(_liens[i].getNoeud1()) || _liens[idArray].contains(_liens[i].getNoeud2()))) {
					if (seCroisent(_liens[idArray], _liens[i])) {
						if (surSegment(_liens[idArray], *_liens[i].getNoeud1()) || surSegment(_liens[idArray], *_liens[i].getNoeud2())) {
							_liens[idArray].intersectionsIll.insert(i);
						}
						else if (surSegment(_liens[i], *_liens[idArray].getNoeud1()) || surSegment(_liens[i], *_liens[idArray].getNoeud2())) {
							_liens[idArray].intersectionsIll.insert(i);
						}
						else {
							_liens[idArray].intersections.insert(i);
						}
					}
				}
				else {
					Noeud* nodeNotInCommon = _liens[i].nodeNotInCommon(&_liens[idArray]);
					if (surSegment(_liens[idArray], *nodeNotInCommon)) {
						_liens[idArray].intersectionsIllSelf.insert(i);
					}
					else {
						nodeNotInCommon = _liens[idArray].nodeNotInCommon(&_liens[i]);
						if (surSegment(_liens[i], *nodeNotInCommon)) {
							_liens[idArray].intersectionsIllSelf.insert(i);
						}
					}
				}
			}
		}
	}

	// Met a jour le score du noeud et met a jour la liste d'intersection de ses aretes
	// Modifie le score des noeuds affectes par la liste
	void updateNodeScore(int idNode) {
		for (int i=0;i<_noeuds[idNode]._aretes.size();i++) {
			int idOtherNode = _liens[_noeuds[idNode]._aretes[i]].getNoeud1()->getId();
			if (idNode == idOtherNode) {
				idOtherNode = _liens[_noeuds[idNode]._aretes[i]].getNoeud2()->getId();
			}
			if (!_liens[_noeuds[idNode]._aretes[i]].isUpdated) {
				std::unordered_set<int> oldIntersections = _liens[_noeuds[idNode]._aretes[i]].intersections;
				std::unordered_set<int> oldIntersectionsIll = _liens[_noeuds[idNode]._aretes[i]].intersectionsIll;
				std::unordered_set<int> oldIntersectionsIllSelf = _liens[_noeuds[idNode]._aretes[i]].intersectionsIllSelf;
				recalculateInterArray(_noeuds[idNode]._aretes[i]);
				for (const int& elem: oldIntersections) {
					if (_liens[_noeuds[idNode]._aretes[i]].intersections.count(elem) == 0) {
						_liens[elem].getNoeud1()->score--;
						_liens[elem].getNoeud2()->score--;
						_liens[elem].intersections.erase(_noeuds[idNode]._aretes[i]);
						_noeuds[idOtherNode].score--;
						_noeuds[idNode].score--;
						nombreCroisement--;
					}
				}
				for (const int& elem: _liens[_noeuds[idNode]._aretes[i]].intersections) {
					if (oldIntersections.count(elem) == 0) {
						_liens[elem].getNoeud1()->score++;
						_liens[elem].getNoeud2()->score++;
						_liens[elem].intersections.insert(_noeuds[idNode]._aretes[i]);
						_noeuds[idOtherNode].score++;
						_noeuds[idNode].score++;
						nombreCroisement++;
					}
				}
				for (const int& elem: oldIntersectionsIll) {
					if (_liens[_noeuds[idNode]._aretes[i]].intersectionsIll.count(elem) == 0) {
						_liens[elem].getNoeud1()->score -= PENALITE_MAX;
						_liens[elem].getNoeud2()->score -= PENALITE_MAX;
						_liens[elem].intersectionsIll.erase(_noeuds[idNode]._aretes[i]);
						_noeuds[idOtherNode].score -= PENALITE_MAX;
						_noeuds[idNode].score -= PENALITE_MAX;
						nombreCroisement -= PENALITE_MAX;
					}
				}
				for (const int& elem: _liens[_noeuds[idNode]._aretes[i]].intersectionsIll) {
					if (oldIntersectionsIll.count(elem) == 0) {
						_liens[elem].getNoeud1()->score += PENALITE_MAX;
						_liens[elem].getNoeud2()->score += PENALITE_MAX;
						_liens[elem].intersectionsIll.insert(_noeuds[idNode]._aretes[i]);
						_noeuds[idOtherNode].score += PENALITE_MAX;
						_noeuds[idNode].score += PENALITE_MAX;
						nombreCroisement += PENALITE_MAX;
					}
				}
				for (const int& elem: oldIntersectionsIllSelf) {
					if (_liens[_noeuds[idNode]._aretes[i]].intersectionsIllSelf.count(elem) == 0) {
						_liens[elem].getNoeud1()->score -= PENALITE_MAX_SELF;
						_liens[elem].getNoeud2()->score -= PENALITE_MAX_SELF;
						_liens[elem].intersectionsIllSelf.erase(_noeuds[idNode]._aretes[i]);
						_liens[_noeuds[idNode]._aretes[i]].nodeNotInCommon(&_liens[elem])->score -= PENALITE_MAX_SELF;
						nombreCroisement -= PENALITE_MAX_SELF;
					}
				}
				for (const int& elem: _liens[_noeuds[idNode]._aretes[i]].intersectionsIllSelf) {
					if (oldIntersectionsIllSelf.count(elem) == 0) {
						_liens[elem].getNoeud1()->score += PENALITE_MAX_SELF;
						_liens[elem].getNoeud2()->score += PENALITE_MAX_SELF;
						_liens[elem].intersectionsIllSelf.insert(_noeuds[idNode]._aretes[i]);
						_liens[_noeuds[idNode]._aretes[i]].nodeNotInCommon(&_liens[elem])->score += PENALITE_MAX_SELF;
						nombreCroisement += PENALITE_MAX_SELF;
					}
				}
				_liens[_noeuds[idNode]._aretes[i]].isUpdated = true;
			}
		}
	}

	// Effectue le croisement entre deux parents,
	// Renvoie vrai si les deux parents ne sont pas identique
	// Met a jour le nombre de croisement du graphe
	bool croisementVoisinageScore(Graphe& originalGraphe1, Graphe& originalGraphe2, bool useRand) {
		int nbNoeudATraiter = originalGraphe1._noeuds.size() - originalGraphe1.nbNoeudEnCommun(originalGraphe2);
		if (nbNoeudATraiter == 0) {
			copyFromGraphe(originalGraphe1);
			return false;
		}
		Graphe graphe1, graphe2;
		graphe1.copyFromGraphe(originalGraphe1);
		graphe2.copyFromGraphe(originalGraphe2);
		Graphe* currentGraphe, * otherGraphe;
		int currentGrapheNumber = generateRand(1);
		if (currentGrapheNumber == 0) { currentGraphe = &graphe1; otherGraphe = &graphe2; }
		else { currentGraphe = &graphe2; otherGraphe = &graphe1; }

		std::vector<int> nodeToUpdate;
		std::vector<int> nodeToRelocate;
		nodeToUpdate.resize(originalGraphe1.maxVoisin+1);
		nodeToRelocate.resize(originalGraphe1.maxVoisin+1);
		while (nbNoeudATraiter > 0) {
			int bestNodeId = -1;
			int meilleurScore;
			int nbRencontre = 0;
			int numberOfNodeToUpdate=0;
			int numberOfNodeToRelocate = 0;

			//Trouve le meilleur noeud du graphe en cours d'analyse
			for (int i = 0; i < _noeuds.size(); ++i) {
				if (!_noeuds[i].estPlace()) {
					int nodeScore = currentGraphe->_noeuds[i].score;
					if (bestNodeId == -1 || nodeScore < meilleurScore) {
						bestNodeId = i;
						meilleurScore = nodeScore;
						nbRencontre = 1;
					}
					else if (meilleurScore == nodeScore) {
						++nbRencontre;
						if (generateRand(nbRencontre) == 1) {
							bestNodeId = i;
						}
					}
				}
			}

			int bestEmplacementId = currentGraphe->_noeuds[bestNodeId].getEmplacement()->getId();
			_noeuds[bestNodeId].setEmplacement(&_emplacementsPossibles[bestEmplacementId]);
			if (!graphe1._noeuds[bestNodeId].compare(&graphe2._noeuds[bestNodeId])) {
				--nbNoeudATraiter;
				int oldNodeId = otherGraphe->_noeuds[bestNodeId].ecraseNoeud(otherGraphe->_emplacementsPossibles[bestEmplacementId]);
				nodeToUpdate[numberOfNodeToUpdate] = bestNodeId;
				numberOfNodeToUpdate++;
				otherGraphe->changeUpdateValue(bestNodeId);
				if (oldNodeId != -1) {
					if (!areVoisin(bestNodeId,oldNodeId)) {
						nodeToRelocate[numberOfNodeToRelocate] = oldNodeId;
						numberOfNodeToRelocate++;
						otherGraphe->changeUpdateValue(oldNodeId);
					}
				}
			}
			//Place tout les voisins du point choisis
			for (int i=0;i<currentGraphe->_noeuds[bestNodeId].voisins.size();i++) {
				int idNodeVoisin = currentGraphe->_noeuds[bestNodeId].voisins[i]->getId();
				if (!_noeuds[idNodeVoisin].estPlace()) {
					if (currentGraphe->_noeuds[idNodeVoisin].getEmplacement() == nullptr) {
						bestEmplacementId = getMeilleurEmplacement(_noeuds[idNodeVoisin]);
					}
					else {
						bestEmplacementId = currentGraphe->_noeuds[idNodeVoisin].getEmplacement()->getId();
					}
					_noeuds[idNodeVoisin].setEmplacement(&_emplacementsPossibles[bestEmplacementId]);
					if ((!otherGraphe->_noeuds[idNodeVoisin].estPlace())||(!graphe1._noeuds[idNodeVoisin].compare(&graphe2._noeuds[idNodeVoisin]))) {
						--nbNoeudATraiter;
						int oldNodeId = otherGraphe->_noeuds[idNodeVoisin].ecraseNoeud(otherGraphe->_emplacementsPossibles[bestEmplacementId]);
						nodeToUpdate[numberOfNodeToUpdate] = idNodeVoisin;
						numberOfNodeToUpdate++;
						otherGraphe->changeUpdateValue(idNodeVoisin);
						if (oldNodeId != -1) {
							if (!areVoisin(bestNodeId,oldNodeId)) {
								nodeToRelocate[numberOfNodeToRelocate] = oldNodeId;
								numberOfNodeToRelocate++;
								otherGraphe->changeUpdateValue(oldNodeId);
							}
						}
					}
				}
			}
			if (useRand) {
				otherGraphe->completePlacementAleatoireScore(nodeToRelocate, numberOfNodeToRelocate);
			}
			else {
				otherGraphe->completeBasicGloutonScore(nodeToRelocate, numberOfNodeToRelocate);
			}
			for (int i=0;i<numberOfNodeToUpdate;i++) {
				otherGraphe->updateNodeScore(nodeToUpdate[i]);
			}
			//Si les lieux coincident les noeuds ne sont plus à traiter
			for (int i = 0; i < numberOfNodeToRelocate; ++i) {
				otherGraphe->updateNodeScore(nodeToRelocate[i]);
				if (graphe1._noeuds[nodeToRelocate[i]].compare(&graphe2._noeuds[nodeToRelocate[i]])) {
					--nbNoeudATraiter;
				}
			}

			//Change le parent choisis 
			if (currentGrapheNumber == 0) {
				currentGraphe = &graphe2;
				otherGraphe = &graphe1;
				currentGrapheNumber = 1;
			}
			else {
				currentGraphe = &graphe1;
				otherGraphe = &graphe2;
				currentGrapheNumber = 0;
			}
		}
		nombreCroisement = otherGraphe->nombreCroisement;
		_noeuds.swap(graphe1._noeuds);
		_liens.swap(graphe1._liens);
		_emplacementsPossibles.swap(graphe1._emplacementsPossibles);
		isNombreCroisementUpdated = true;
		isNodeScoreUpdated = true;
		isIntersectionVectorUpdated = true;
		return true;
	}

	// Effectue une seule etape dans l'algorithme de croisement, utile pour l'affichage opengl
	void stepCroisementVoisinageScore(Graphe& graphe1, Graphe& graphe2, bool useRand, int &nbNoeudATraiter, int &currentGrapheNumber) {
		if (nbNoeudATraiter == 0) {
			return;
		}
		Graphe* currentGraphe, * otherGraphe;
		if (currentGrapheNumber == 0) { currentGraphe = &graphe1; otherGraphe = &graphe2; }
		else { currentGraphe = &graphe2; otherGraphe = &graphe1; }

		std::vector<int> nodeToUpdate;
		std::vector<int> nodeToRelocate;
		nodeToUpdate.resize(graphe1.maxVoisin+1);
		nodeToRelocate.resize(graphe1.maxVoisin+1);

		int bestNodeId = -1;
		int meilleurScore;
		int nbRencontre = 0;
		int numberOfNodeToUpdate=0;
		int numberOfNodeToRelocate = 0;

		//Trouve le meilleur noeud du graphe en cours d'analyse
		for (int i = 0; i < _noeuds.size(); ++i) {
			if (!_noeuds[i].estPlace()) {
				int nodeScore = currentGraphe->_noeuds[i].score;
				if (bestNodeId == -1 || nodeScore < meilleurScore) {
					bestNodeId = i;
					meilleurScore = nodeScore;
					nbRencontre = 1;
				}
				else if (meilleurScore == nodeScore) {
					++nbRencontre;
					if (generateRand(nbRencontre) == 1) {
						bestNodeId = i;
					}
				}
			}
		}

		int bestEmplacementId = currentGraphe->_noeuds[bestNodeId].getEmplacement()->getId();
		_noeuds[bestNodeId].setEmplacement(&_emplacementsPossibles[bestEmplacementId]);
		if (!graphe1._noeuds[bestNodeId].compare(&graphe2._noeuds[bestNodeId])) {
			--nbNoeudATraiter;
			int oldNodeId = otherGraphe->_noeuds[bestNodeId].ecraseNoeud(otherGraphe->_emplacementsPossibles[bestEmplacementId]);
			nodeToUpdate[numberOfNodeToUpdate] = bestNodeId;
			numberOfNodeToUpdate++;
			otherGraphe->changeUpdateValue(bestNodeId);
			if (oldNodeId != -1) {
				if (!areVoisin(bestNodeId,oldNodeId)) {
					nodeToRelocate[numberOfNodeToRelocate] = oldNodeId;
					numberOfNodeToRelocate++;
					otherGraphe->changeUpdateValue(oldNodeId);
				}
			}
		}
		//Place tout les voisins du point choisis
		for (int i=0;i<currentGraphe->_noeuds[bestNodeId].voisins.size();i++) {
			int idNodeVoisin = currentGraphe->_noeuds[bestNodeId].voisins[i]->getId();
			if (!_noeuds[idNodeVoisin].estPlace()) {
				if (currentGraphe->_noeuds[idNodeVoisin].getEmplacement() == nullptr) {
					bestEmplacementId = getMeilleurEmplacement(_noeuds[idNodeVoisin]);
				}
				else {
					bestEmplacementId = currentGraphe->_noeuds[idNodeVoisin].getEmplacement()->getId();
				}
				_noeuds[idNodeVoisin].setEmplacement(&_emplacementsPossibles[bestEmplacementId]);
				if ((!otherGraphe->_noeuds[idNodeVoisin].estPlace())||(!graphe1._noeuds[idNodeVoisin].compare(&graphe2._noeuds[idNodeVoisin]))) {
					--nbNoeudATraiter;
					int oldNodeId = otherGraphe->_noeuds[idNodeVoisin].ecraseNoeud(otherGraphe->_emplacementsPossibles[bestEmplacementId]);
					nodeToUpdate[numberOfNodeToUpdate] = idNodeVoisin;
					numberOfNodeToUpdate++;
					otherGraphe->changeUpdateValue(idNodeVoisin);
					if (oldNodeId != -1) {
						if (!areVoisin(bestNodeId,oldNodeId)) {
							nodeToRelocate[numberOfNodeToRelocate] = oldNodeId;
							numberOfNodeToRelocate++;
							otherGraphe->changeUpdateValue(oldNodeId);
						}
					}
				}
			}
		}
		if (useRand) {
			otherGraphe->completePlacementAleatoireScore(nodeToRelocate, numberOfNodeToRelocate);
		}
		else {
			otherGraphe->completeBasicGloutonScore(nodeToRelocate, numberOfNodeToRelocate);
		}
		for (int i=0;i<nodeToUpdate.size();i++) {
			otherGraphe->updateNodeScore(nodeToUpdate[i]);
		}
		//Si les lieux coincident les noeuds ne sont plus à traiter
		for (int i = 0; i < numberOfNodeToRelocate; ++i) {
			otherGraphe->updateNodeScore(nodeToRelocate[i]);
			if (graphe1._noeuds[nodeToRelocate[i]].compare(&graphe2._noeuds[nodeToRelocate[i]])) {
				--nbNoeudATraiter;
			}
		}

		currentGrapheNumber = (currentGrapheNumber+1)%2;
		isNombreCroisementUpdated = true;
		if (nbNoeudATraiter == 0) {
			loadCopy(graphe1.saveCopy());
			nombreCroisement = otherGraphe->nombreCroisement;
		}
		std::cout << "NNT: " << nbNoeudATraiter << std::endl;
	}

	// Effectue le croisement entre deux parents,
	// Renvoie vrai si les deux parents ne sont pas identique
	// Met a jour le nombre de croisement du graphe
	bool croisementBestOfBoth(Graphe& originalGraphe1, Graphe& originalGraphe2, bool useRand) {
		int nbNoeudATraiter = originalGraphe1._noeuds.size() - originalGraphe1.nbNoeudEnCommun(originalGraphe2);
		if (nbNoeudATraiter == 0) {
			copyFromGraphe(originalGraphe1);
			return false;
		}
		Graphe graphe1, graphe2;
		graphe1.copyFromGraphe(originalGraphe1);
		graphe2.copyFromGraphe(originalGraphe2);
		Graphe* currentGraphe, * otherGraphe;

		std::vector<int> nodeToUpdate;
		std::vector<int> nodeToRelocate;
		nodeToUpdate.resize(originalGraphe1.maxVoisin+1);
		nodeToRelocate.resize(originalGraphe1.maxVoisin+1);
		while (nbNoeudATraiter > 0) {
			int bestNodeId = -1;
			int meilleurScore;
			int nbRencontre = 0;
			int numberOfNodeToUpdate=0;
			int numberOfNodeToRelocate = 0;

			//Trouve le meilleur noeud du graphe en cours d'analyse
			for (int i = 0; i < _noeuds.size(); ++i) {
				if (!_noeuds[i].estPlace()) {
					int nodeScore = graphe1._noeuds[i].score;
					if (bestNodeId == -1 || nodeScore < meilleurScore) {
						bestNodeId = i;
						meilleurScore = nodeScore;
						nbRencontre = 1;
						if (currentGraphe != &graphe1) { currentGraphe = &graphe1; otherGraphe = &graphe2; }
					}
					else if (meilleurScore == nodeScore) {
						++nbRencontre;
						if (generateRand(nbRencontre) == 1) {
							bestNodeId = i;
						}
					}
				}
			}
			for (int i = 0; i < _noeuds.size(); ++i) {
				if (!_noeuds[i].estPlace()) {
					int nodeScore = graphe2._noeuds[i].score;
					if (bestNodeId == -1 || nodeScore < meilleurScore) {
						bestNodeId = i;
						meilleurScore = nodeScore;
						nbRencontre = 1;
						if (currentGraphe != &graphe2) { currentGraphe = &graphe2; otherGraphe = &graphe1; }
					}
					else if (meilleurScore == nodeScore) {
						++nbRencontre;
						if (generateRand(nbRencontre) == 1) {
							bestNodeId = i;
						}
					}
				}
			}

			int bestEmplacementId = currentGraphe->_noeuds[bestNodeId].getEmplacement()->getId();
			_noeuds[bestNodeId].setEmplacement(&_emplacementsPossibles[bestEmplacementId]);
			if (!graphe1._noeuds[bestNodeId].compare(&graphe2._noeuds[bestNodeId])) {
				--nbNoeudATraiter;
				int oldNodeId = otherGraphe->_noeuds[bestNodeId].ecraseNoeud(otherGraphe->_emplacementsPossibles[bestEmplacementId]);
				nodeToUpdate[numberOfNodeToUpdate] = bestNodeId;
				numberOfNodeToUpdate++;
				otherGraphe->changeUpdateValue(bestNodeId);
				if (oldNodeId != -1) {
					if (!areVoisin(bestNodeId,oldNodeId)) {
						nodeToRelocate[numberOfNodeToRelocate] = oldNodeId;
						numberOfNodeToRelocate++;
						otherGraphe->changeUpdateValue(oldNodeId);
					}
				}
			}
			//Place tout les voisins du point choisis
			for (int i=0;i<currentGraphe->_noeuds[bestNodeId].voisins.size();i++) {
				int idNodeVoisin = currentGraphe->_noeuds[bestNodeId].voisins[i]->getId();
				if (!_noeuds[idNodeVoisin].estPlace()) {
					if (currentGraphe->_noeuds[idNodeVoisin].getEmplacement() == nullptr) {
						bestEmplacementId = getMeilleurEmplacement(_noeuds[idNodeVoisin]);
					}
					else {
						bestEmplacementId = currentGraphe->_noeuds[idNodeVoisin].getEmplacement()->getId();
					}
					_noeuds[idNodeVoisin].setEmplacement(&_emplacementsPossibles[bestEmplacementId]);
					if ((!otherGraphe->_noeuds[idNodeVoisin].estPlace())||(!graphe1._noeuds[idNodeVoisin].compare(&graphe2._noeuds[idNodeVoisin]))) {
						--nbNoeudATraiter;
						int oldNodeId = otherGraphe->_noeuds[idNodeVoisin].ecraseNoeud(otherGraphe->_emplacementsPossibles[bestEmplacementId]);
						nodeToUpdate[numberOfNodeToUpdate] = idNodeVoisin;
						numberOfNodeToUpdate++;
						otherGraphe->changeUpdateValue(idNodeVoisin);
						if (oldNodeId != -1) {
							if (!areVoisin(bestNodeId,oldNodeId)) {
								nodeToRelocate[numberOfNodeToRelocate] = oldNodeId;
								numberOfNodeToRelocate++;
								otherGraphe->changeUpdateValue(oldNodeId);
							}
						}
					}
				}
			}
			if (useRand) {
				otherGraphe->completePlacementAleatoireScore(nodeToRelocate, numberOfNodeToRelocate);
			}
			else {
				otherGraphe->completeBasicGloutonScore(nodeToRelocate, numberOfNodeToRelocate);
			}
			for (int i=0;i<numberOfNodeToUpdate;i++) {
				otherGraphe->updateNodeScore(nodeToUpdate[i]);
			}
			//Si les lieux coincident les noeuds ne sont plus à traiter
			for (int i = 0; i < numberOfNodeToRelocate; ++i) {
				otherGraphe->updateNodeScore(nodeToRelocate[i]);
				if (graphe1._noeuds[nodeToRelocate[i]].compare(&graphe2._noeuds[nodeToRelocate[i]])) {
					--nbNoeudATraiter;
				}
			}
		}
		nombreCroisement = otherGraphe->nombreCroisement;
		_noeuds.swap(graphe1._noeuds);
		_liens.swap(graphe1._liens);
		_emplacementsPossibles.swap(graphe1._emplacementsPossibles);
		isNombreCroisementUpdated = true;
		isNodeScoreUpdated = true;
		isIntersectionVectorUpdated = true;
		return true;
	}
	
	// Effectue un croisement entre deux parents en séparant le graphe en deux parties
	// Les emplacements sont triés par leur coord X
	// Ne met pas à jour la variable nombreCroisement du graphe
	bool croisementHalfParent(Graphe& graphe1, Graphe& graphe2, std::vector<int>& sortedEmpId, bool useRand) {
		int nbNoeudATraiter = graphe1._noeuds.size() - graphe1.nbNoeudEnCommun(graphe2);
		//std::cout << "Nb noeud a traiter au debut: " << nbNoeudATraiter << "\n";
		if (nbNoeudATraiter == 0) {
			copyFromGraphe(graphe1);
			return false;
		}
		std::vector<int> saveGraphe1 = graphe1.saveCopy();
		std::vector<int> saveGraphe2 = graphe2.saveCopy();
		int startVec2 = sortedEmpId.size() / 2;

		Graphe* currentGraphe, * otherGraphe;
		int currentGrapheNumber = generateRand(1);
		int startVec, endVec;
		if (currentGrapheNumber == 0) { currentGraphe = &graphe1; otherGraphe = &graphe2; startVec = 0; endVec = startVec2; }
		else { currentGraphe = &graphe2; otherGraphe = &graphe1; startVec = startVec2; endVec = sortedEmpId.size(); }
		while (nbNoeudATraiter > 0) {
			Noeud* meilleurNoeud = nullptr;
			int meilleurScore;
			int nbRencontre = 0;
			for (int i = startVec; i < endVec; ++i) { // Recherche du meilleur noeud du coté du graphe choisi
				if (currentGraphe->_emplacementsPossibles[sortedEmpId[i]]._noeud != nullptr) {
					int idNode = currentGraphe->_emplacementsPossibles[sortedEmpId[i]]._noeud->getId();
					if (!_noeuds[idNode].estPlace()) {
						int nodeScore = currentGraphe->getScoreCroisementNode(idNode);
						if (meilleurNoeud == nullptr || meilleurScore < nodeScore) {
							meilleurNoeud = &currentGraphe->_noeuds[idNode];
							meilleurScore = nodeScore;
							nbRencontre = 1;
						}
						else if (meilleurScore == nodeScore) {
							++nbRencontre;
							if (generateRand(nbRencontre) == 1) {
								meilleurNoeud = &currentGraphe->_noeuds[idNode];
							}
						}
					}
				}
			}

			if (meilleurNoeud != nullptr) {
				int meilleurEmplacement = meilleurNoeud->getEmplacement()->getId();
				_noeuds[meilleurNoeud->getId()].setEmplacement(&_emplacementsPossibles[meilleurEmplacement]);
				if (!graphe1._noeuds[meilleurNoeud->getId()].compare(&graphe2._noeuds[meilleurNoeud->getId()])) {
					--nbNoeudATraiter;
					otherGraphe->_noeuds[meilleurNoeud->getId()].ecraseNoeud(otherGraphe->_emplacementsPossibles[meilleurEmplacement]);
				}
				//Place tout les voisins du point choisis
				for (Noeud* noeudVoisin : meilleurNoeud->getVoisins()) {
					if (!_noeuds[noeudVoisin->getId()].estPlace()) {
						if (noeudVoisin->getEmplacement() == nullptr) {
							meilleurEmplacement = getMeilleurEmplacement(_noeuds[noeudVoisin->getId()]);
						}
						else {
							meilleurEmplacement = noeudVoisin->getEmplacement()->getId();
						}
						_noeuds[noeudVoisin->getId()].setEmplacement(&_emplacementsPossibles[meilleurEmplacement]);
						Noeud* noeudGraphe1 = &graphe1._noeuds[noeudVoisin->getId()];
						Noeud* noeudGraphe2 = &graphe2._noeuds[noeudVoisin->getId()];
						if (noeudGraphe1->estPlace() && noeudGraphe2->estPlace()) {
							if (!noeudGraphe1->compare(noeudGraphe2)) {
								--nbNoeudATraiter;
								otherGraphe->_noeuds[noeudVoisin->getId()].ecraseNoeud(graphe1._emplacementsPossibles[meilleurEmplacement]);
							}
						}
						else {
							--nbNoeudATraiter;
						}
					}
				}
				
				//Liste des noeuds à placer
				std::vector<int> noeudsAVerifier;
				for (int i = 0; i < _noeuds.size(); ++i) {
					if (!graphe1._noeuds[i].estPlace() || !graphe2._noeuds[i].estPlace()) {
						noeudsAVerifier.push_back(i);
					}
				}
				if (useRand) {
					otherGraphe->completePlacementAleatoire();
				}
				else {
					otherGraphe->completeBasicGlouton();
				}
				//Si les lieux coincident les noeuds ne sont plus à traiter
				for (int i = 0; i < noeudsAVerifier.size(); ++i) {
					if (graphe1._noeuds[noeudsAVerifier[i]].compare(&graphe2._noeuds[noeudsAVerifier[i]])) {
						--nbNoeudATraiter;
					}
				}
			}

			//Change le parent choisis 
			if (currentGrapheNumber == 0) {
				currentGraphe = &graphe2;
				otherGraphe = &graphe1;
				currentGrapheNumber = 1;
			}
			else {
				currentGraphe = &graphe1;
				otherGraphe = &graphe2;
				currentGrapheNumber = 0;
			}
		}

		_noeuds.swap(graphe1._noeuds);
		_liens.swap(graphe1._liens);
		_emplacementsPossibles.swap(graphe1._emplacementsPossibles);
		graphe1.loadCopy(saveGraphe1);
		graphe2.loadCopy(saveGraphe2);
		isNombreCroisementUpdated = false;
		isNodeScoreUpdated = false;
		isIntersectionVectorUpdated = false;
		return true;
	}

	// Effectue un croisement entre deux parents en selectionnant un noeud aleatoirement de chaque parent en alternant
	// Ne met pas à jour la variable nombreCroisement du graphe
	bool croisementAleatoire(Graphe& graphe1, Graphe& graphe2, bool useRand) {
		int nbNoeudATraiter = graphe1._noeuds.size() - graphe1.nbNoeudEnCommun(graphe2);
		if (nbNoeudATraiter == 0) {
			copyFromGraphe(graphe1);
			return false;
		}
		std::vector<int> saveGraphe1 = graphe1.saveCopy();
		std::vector<int> saveGraphe2 = graphe2.saveCopy();

		Graphe* currentGraphe, * otherGraphe;
		int currentGrapheNumber = generateRand(1);
		if (currentGrapheNumber == 0) { currentGraphe = &graphe1; otherGraphe = &graphe2; }
		else { currentGraphe = &graphe2; otherGraphe = &graphe1; }

		while (nbNoeudATraiter > 0) {
			int bestNodeId = generateRand(_noeuds.size() - 1);
			while (_noeuds[bestNodeId].estPlace()) {
				bestNodeId = generateRand(_noeuds.size() - 1);
			}

			int bestEmplacementId = currentGraphe->_noeuds[bestNodeId].getEmplacement()->getId();
			_noeuds[bestNodeId].setEmplacement(&_emplacementsPossibles[bestEmplacementId]);
			if (!graphe1._noeuds[bestNodeId].compare(&graphe2._noeuds[bestNodeId])) {
				--nbNoeudATraiter;
				int oldNodeId = otherGraphe->_noeuds[bestNodeId].ecraseNoeud(otherGraphe->_emplacementsPossibles[bestEmplacementId]);
				if (oldNodeId != -1) {
					if (useRand) {
						otherGraphe->completePlacementAleatoire();
					}
					else {
						otherGraphe->completeBasicGlouton();
					}		//std::cout << "Nb noeud a traiter au debut: " << nbNoeudATraiter << "\n";
					if (graphe1._noeuds[oldNodeId].compare(&graphe2._noeuds[oldNodeId])) {
						--nbNoeudATraiter;
					}
				}
			}

			//Change le parent choisis 
			if (currentGrapheNumber == 0) {
				currentGraphe = &graphe2;
				otherGraphe = &graphe1;
				currentGrapheNumber = 1;
			}
			else {
				currentGraphe = &graphe1;
				otherGraphe = &graphe2;
				currentGrapheNumber = 0;
			}
		}

		_noeuds.swap(graphe1._noeuds);
		_liens.swap(graphe1._liens);
		_emplacementsPossibles.swap(graphe1._emplacementsPossibles);
		graphe1.loadCopy(saveGraphe1);
		graphe2.loadCopy(saveGraphe2);
		isNombreCroisementUpdated = false;
		isNodeScoreUpdated = false;
		isIntersectionVectorUpdated = false;
		return true;
	}

	// Effectue le croisement entre deux parents,
	// On selectionne un noeud en alternant de parent, celui qui creer le moin d'intersection si le place chez l'enfant
	// Renvoie vrai si les deux parents ne sont pas identique
	// Ne met pas a jour le nombre de croisement du graphe et des noeuds
	bool croisementEnfantScore(Graphe& originalGraphe1, Graphe& originalGraphe2, bool useRand) {
		std::vector<int> commonNodeVec, otherNodeVec, indexNodeInOtherVec;
		isNombreCroisementUpdated = false;
		originalGraphe1.separateNodesInCommon(originalGraphe2, commonNodeVec, otherNodeVec, indexNodeInOtherVec);
		int nbNoeudATraiter = otherNodeVec.size();
		if (nbNoeudATraiter == 0) {
			copyFromGraphe(originalGraphe1);
			return false;
		}
		std::vector<int> activeEdges; // Contient les id des aretes actives chez l'enfant
		for (int i=0;i<commonNodeVec.size();i++) { // On place les noeuds communs et on active les aretes placées
			_noeuds[commonNodeVec[i]].setEmplacement(&_emplacementsPossibles[originalGraphe1._noeuds[commonNodeVec[i]].getEmplacement()->_id]);
			for (const int& areteId : _noeuds[commonNodeVec[i]]._aretes) {
				if (_liens[areteId].estPlace())
					activeEdges.push_back(areteId);
			}
		}
		Graphe graphe1, graphe2;
		graphe1.copyFromGraphe(originalGraphe1);
		graphe2.copyFromGraphe(originalGraphe2);
		Graphe* currentGraphe, * otherGraphe;
		int currentGrapheNumber = generateRand(1);
		if (currentGrapheNumber == 0) { currentGraphe = &graphe1; otherGraphe = &graphe2; }
		else { currentGraphe = &graphe2; otherGraphe = &graphe1; }

		std::vector<int> nodeToRelocate;
		while (nbNoeudATraiter > 0) {
			int bestNodeId = -1;
			long bestScore;
			int bestNbVoisinsPlace = -1;
			int bestNbVoisin = -1;
			int bestNodeIndexInVec = -1;
			int nbRencontre = 0;
			for (int i=0;i<nbNoeudATraiter;i++) {
				if (currentGraphe->_noeuds[otherNodeVec[i]].estPlace()) {
					if (_emplacementsPossibles[currentGraphe->_noeuds[otherNodeVec[i]].getEmplacement()->_id].estDisponible()) {
						int nodeNbVoisinsPlace = 0;
						for (const int& areteId : _noeuds[otherNodeVec[i]]._aretes) {
							if (_liens[areteId].unPlace()) { nodeNbVoisinsPlace++; }
						}
						int nodeNbVoisins = _noeuds[otherNodeVec[i]]._aretes.size();
						if (nodeNbVoisinsPlace > bestNbVoisinsPlace) {
							bestNodeId = otherNodeVec[i];
							bestNbVoisinsPlace = nodeNbVoisinsPlace;
							bestNbVoisin = nodeNbVoisins;
							bestScore = -1;
							bestNodeIndexInVec = i;
							nbRencontre = 0;
						}
						else if (nodeNbVoisinsPlace == bestNbVoisinsPlace) {
							if (bestScore == -1) {
								bestScore = getNodeScoreEnfant(*currentGraphe, activeEdges,bestNodeId);
							}
							long nodeScore = getNodeScoreEnfant(*currentGraphe, activeEdges,otherNodeVec[i]);
							if (nodeScore < bestScore) {
								bestNodeId = otherNodeVec[i];
								bestNbVoisinsPlace = nodeNbVoisinsPlace;
								bestNbVoisin = nodeNbVoisins;
								bestScore = nodeScore;
								bestNodeIndexInVec = i;
								nbRencontre = 0;
							}
							else if (nodeScore == bestScore) {
								if (nodeNbVoisins > bestNbVoisin) {
									bestNodeId = otherNodeVec[i];
									bestNbVoisinsPlace = nodeNbVoisinsPlace;
									bestNbVoisin = nodeNbVoisins;
									bestScore = nodeScore;
									bestNodeIndexInVec = i;
									nbRencontre = 0;
								}
							}
						}
					}
				}
			}
			if (bestNodeId != -1) {
				int bestEmplacementId = currentGraphe->_noeuds[bestNodeId].getEmplacement()->getId();
				_noeuds[bestNodeId].setEmplacement(&_emplacementsPossibles[bestEmplacementId]);
				for (const int& areteId : _noeuds[bestNodeId]._aretes) {
					if (_liens[areteId].estPlace())
						activeEdges.push_back(areteId);
				}
				--nbNoeudATraiter;
				otherNodeVec[bestNodeIndexInVec] = otherNodeVec[nbNoeudATraiter];
				indexNodeInOtherVec[otherNodeVec[bestNodeIndexInVec]] = bestNodeIndexInVec;
				int oldNodeId = otherGraphe->_noeuds[bestNodeId].ecraseNoeud(otherGraphe->_emplacementsPossibles[bestEmplacementId]);
				if (oldNodeId != -1) {
					if (!areVoisin(bestNodeId,oldNodeId)) {
						if (!currentGraphe->_noeuds[oldNodeId].estPlace()) {
							nbNoeudATraiter--;
							nodeToRelocate.push_back(oldNodeId);
							otherNodeVec[indexNodeInOtherVec[oldNodeId]] = otherNodeVec[nbNoeudATraiter];
							indexNodeInOtherVec[otherNodeVec[nbNoeudATraiter]] = indexNodeInOtherVec[oldNodeId];
						}
					}
				}
				//Place tout les voisins du point choisis
				for (int i=0;i<currentGraphe->_noeuds[bestNodeId].voisins.size();i++) {
					int idNodeVoisin = currentGraphe->_noeuds[bestNodeId].voisins[i]->getId();
					if (!_noeuds[idNodeVoisin].estPlace()) {
						if (currentGraphe->_noeuds[idNodeVoisin].estPlace()) {
							bestEmplacementId = currentGraphe->_noeuds[idNodeVoisin].getEmplacement()->getId();
							_noeuds[idNodeVoisin].setEmplacement(&_emplacementsPossibles[bestEmplacementId]);
							for (const int& areteId : _noeuds[idNodeVoisin]._aretes) {
								if (_liens[areteId].estPlace())
									activeEdges.push_back(areteId);
							}
							--nbNoeudATraiter;
							otherNodeVec[indexNodeInOtherVec[idNodeVoisin]] = otherNodeVec[nbNoeudATraiter];
							indexNodeInOtherVec[otherNodeVec[nbNoeudATraiter]] = indexNodeInOtherVec[idNodeVoisin];
							int oldNodeId = otherGraphe->_noeuds[idNodeVoisin].ecraseNoeud(otherGraphe->_emplacementsPossibles[bestEmplacementId]);
							if (oldNodeId != -1) {
								if (!currentGraphe->_noeuds[oldNodeId].estPlace()) {
									nodeToRelocate.push_back(oldNodeId);
									nbNoeudATraiter--;
									otherNodeVec[indexNodeInOtherVec[oldNodeId]] = otherNodeVec[nbNoeudATraiter];
									indexNodeInOtherVec[otherNodeVec[nbNoeudATraiter]] = indexNodeInOtherVec[oldNodeId];
								}
							}
						}
						else {
							if (!otherGraphe->_noeuds[idNodeVoisin].estPlace()) {
								if (std::find(nodeToRelocate.begin(),nodeToRelocate.end(),idNodeVoisin) == nodeToRelocate.end()) {
									nodeToRelocate.push_back(idNodeVoisin);
									nbNoeudATraiter--;
									otherNodeVec[indexNodeInOtherVec[idNodeVoisin]] = otherNodeVec[nbNoeudATraiter];
									indexNodeInOtherVec[otherNodeVec[nbNoeudATraiter]] = indexNodeInOtherVec[idNodeVoisin];
								}
							}
						}
					}
				}
			}
			//Change le parent choisis 
			if (currentGrapheNumber == 0) {
				currentGraphe = &graphe2;
				otherGraphe = &graphe1;
				currentGrapheNumber = 1;
			}
			else {
				currentGraphe = &graphe1;
				otherGraphe = &graphe2;
				currentGrapheNumber = 0;
			}
		}
		int tailleMax = nodeToRelocate.size();
		completeBasicGloutonScore(nodeToRelocate,tailleMax);
		isNombreCroisementUpdated = false;
		isNodeScoreUpdated = false;
		isIntersectionVectorUpdated = false;
		return true;
	}
	
	// Nombre de noeuds du même ID placé aux mêmes emplacements.
	int nbNoeudEnCommun(const Graphe& graphe) {
		int total = 0;
		for (int i = 0; i < _noeuds.size(); ++i) {
			if (_noeuds[i].compare(&graphe._noeuds[i])) {
				++total;
			}
		}
		return total;
	}

	// Renvoie un vecteur contenant les id des noeuds placé au meme endroit
	// Les deux graphes doivent etre place entierement
	std::vector<int> noeudEnCommun(Graphe &G) {
		std::vector<int> commonNodeVec;
		for (int i=0; i<_noeuds.size();i++) {
			if (_noeuds[i].getEmplacement()->_id == G._noeuds[i].getEmplacement()->_id) {
				commonNodeVec.push_back(i);
			}
		}
		return commonNodeVec;
	}

	// Modifie le vecteur de noeuds en commun et le vecteur de noeud non commun en parametre
	void separateNodesInCommon(Graphe &G, std::vector<int>& commonNodeVec, std::vector<int>& otherNodeVec, std::vector<int>& indexNodeInOtherVec) {
		for (int i=0; i<_noeuds.size();i++) {
			if (_noeuds[i].getEmplacement()->_id == G._noeuds[i].getEmplacement()->_id) {
				commonNodeVec.push_back(i);
				indexNodeInOtherVec.push_back(-1);
			}
			else {
				indexNodeInOtherVec.push_back(otherNodeVec.size());
				otherNodeVec.push_back(i);
			}
		}
	}

	long getNbCroisementConst() const
	{
		long total = 0;
		for (int i = 0; i < _liens.size() - 1; ++i)
		{
			for (int j = i + 1; j < _liens.size(); ++j)
			{
				//Aretes aretes1 = _liens[i], aretes2 = _liens[j];
				if (!(_liens[i].contains(_liens[j].getNoeud1())
					|| _liens[i].contains(_liens[j].getNoeud2())))
				{
					if (seCroisent(_liens[i], _liens[j]))
					{
						if (surSegment(_liens[i], *_liens[j].getNoeud1()) || surSegment(_liens[i], *_liens[j].getNoeud2()))
						{
							total += PENALITE_MAX;
						}
						else if (surSegment(_liens[j], *_liens[i].getNoeud1()) || surSegment(_liens[j], *_liens[i].getNoeud2())) {
							total += PENALITE_MAX;
						}
						else {
							++total;
						}
					}
				}
				else {
					Noeud* nodeNotInCommon = _liens[j].nodeNotInCommon(&_liens[i]);
					if (surSegment(_liens[i], *nodeNotInCommon))
					{
						total += PENALITE_MAX_SELF;
					}
					else {
						nodeNotInCommon = _liens[i].nodeNotInCommon(&_liens[j]);
						if (surSegment(_liens[j], *nodeNotInCommon)) {
							total += PENALITE_MAX_SELF;
						}
					}
				}
			}
		}
		return total;
	}

	// Ne met pas a jour le nombre de croisement du graphe
	long getNbCroisementArray() {
		long total = 0;
		for (int i=0;i<_liens.size();i++) {
			total += _liens[i].intersections.size();
			total += _liens[i].intersectionsIll.size() * PENALITE_MAX;
			total += _liens[i].intersectionsIllSelf.size() * PENALITE_MAX_SELF;
		}
		return total/2;
	}

	// Renvoie le nombre de noeud non place
	int nbNoeudNonPlace() {
		int total = _noeuds.size();
		for (int i=0;i<_noeuds.size();i++) {
			if (_noeuds[i].getEmplacement() != nullptr) {
				total--;
			}
		}
		return total;
	}

	// Met a jour le score du graphe et de ses noeuds ainsi que les vecteurs d'intersections des aretes
	void initGraphAndNodeScoresAndCrossings() {
		long total = 0;
		for (int i=0;i<_liens.size();i++) {
			_liens[i].clearIntersectionsVector();
			_liens[i].isUpdated = true;
		}
		for (int i = 0; i < _liens.size() - 1; ++i) {
			for (int j = i + 1; j < _liens.size(); ++j) {
				if (!(_liens[i].contains(_liens[j].getNoeud1()) || _liens[i].contains(_liens[j].getNoeud2()))) {
					if (seCroisent(_liens[i], _liens[j])) {
						if (surSegment(_liens[i], *_liens[j].getNoeud1()) || surSegment(_liens[i], *_liens[j].getNoeud2())) {
							total += PENALITE_MAX;
							_liens[i].intersectionsIll.insert(j);
							_liens[j].intersectionsIll.insert(i);
						}
						else if (surSegment(_liens[j], *_liens[i].getNoeud1()) || surSegment(_liens[j], *_liens[i].getNoeud2())) {
							total += PENALITE_MAX;
							_liens[i].intersectionsIll.insert(j);
							_liens[j].intersectionsIll.insert(i);
						}
						else {
							++total;
							_liens[i].intersections.insert(j);
							_liens[j].intersections.insert(i);
						}
					}
				}
				else {
					Noeud* nodeNotInCommon = _liens[j].nodeNotInCommon(&_liens[i]);
					if (surSegment(_liens[i], *nodeNotInCommon)) {
						total += PENALITE_MAX_SELF;
						_liens[i].intersectionsIllSelf.insert(j);
						_liens[j].intersectionsIllSelf.insert(i);
					}
					else {
						nodeNotInCommon = _liens[i].nodeNotInCommon(&_liens[j]);
						if (surSegment(_liens[j], *nodeNotInCommon)) {
							total += PENALITE_MAX_SELF;
							_liens[i].intersectionsIllSelf.insert(j);
							_liens[j].intersectionsIllSelf.insert(i);
						}
					}
				}
			}
		}
		nombreCroisement = total;
		isNombreCroisementUpdated = true;
		isNodeScoreUpdated = true;
		isIntersectionVectorUpdated = true;
		for (int i=0;i<_noeuds.size();i++) {
			_noeuds[i].score = getScoreCroisementNodeFromArrays(i);
			int nombreVoisin = _noeuds[i]._aretes.size();
			if (nombreVoisin > maxVoisin) {
				maxVoisin = nombreVoisin;
			}
		}
		//std::cout << "Max Voisin dans le graphe: " << maxVoisin << std::endl;
	}

	// Vide les sets contenant les intersections, utile uniquement pour l'affichage openGL
	void clearSetAreteInter() {
		areteInter.clear();
		areteIll.clear();
		areteIllSelf.clear();
	}

	// Indique si les deux noeuds ont une aretes commune
	bool areVoisin(int nodeId1, int nodeId2) {
		for (const int& nodeAreteId : _noeuds[nodeId1]._aretes) {
			if (_liens[nodeAreteId].contains(nodeId2))
				return true;
		}
		return false;
	}

	// Retourne l'id de l'arete qui relie deux points en parametre, -1 si aucune est trouvee
	// Utilise la liste d'arete stockee dans les nodes
	int getAreteFromTwoNodes(int nodeId1, int nodeId2) {
		for (const int& nodeAreteId : _noeuds[nodeId1]._aretes) {
			if (_liens[nodeAreteId].contains(nodeId2))
				return nodeAreteId;
		}
		return -1;
	}

	// Renvoie le score du noeud nodeId1 sans prendre en compte le score d'intersection avec le noeud nodeId2
	long getNodeLinkedScore(int nodeId1, int nodeId2) {
		long score = _noeuds[nodeId1].score;
		for (const int& node1AreteId : _noeuds[nodeId1]._aretes) {
			for (const int& node2AreteId : _noeuds[nodeId2]._aretes) {
				if (_liens[node1AreteId].intersections.count(node2AreteId) > 0) {
					score--;
				}
				if (_liens[node1AreteId].intersectionsIll.count(node2AreteId) > 0) {
					score -= PENALITE_MAX;
				}
				if (_liens[node1AreteId].intersectionsIllSelf.count(node2AreteId) == 0) {
					score -= PENALITE_MAX_SELF;
				}
			}
		}
		return score;
	}

	bool operator < (const Graphe& G) const {
		long n1, n2;
		if (isNombreCroisementUpdated) { n1 = nombreCroisement; } else { n1 = getNbCroisementConst(); }
		if (G.isNombreCroisementUpdated) { n2 = G.nombreCroisement; } else { n2 = G.getNbCroisementConst(); }
		return (n1 < n2);
	}

};

#endif