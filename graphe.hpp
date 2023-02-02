#ifndef GRAPHE_HPP
#define GRAPHE_HPP

#include <vector>
#include "aretes.hpp"
#include "point.hpp"
#include "intersection.hpp"
#include "emplacement.hpp"
#include "utilitaire.hpp"
#include "EdgeMaps.hpp"
#include <iostream>
#include <climits>

class Graphe {
public:
	std::vector<Emplacement> _emplacementsPossibles;
	std::vector<Aretes> _liens;
	std::vector<Noeud> _noeuds;
	int PENALITE_MAX = 10000;
	int gridHeight = 10;
	int gridWidth = 10;

	// Attention cette variable n'est pas forcément à jour!
	long nombreCroisement = -1;

	bool DEBUG_GRAPHE = false;
	bool DEBUG_OPENGL = true;

	Graphe()
	{}

	void afficherLiens() {
		std::cout << "-----------------------------------------------" << std::endl;
		std::cout << "Affichage DEBUG Aretes:" << std::endl;
		std::cout << "Nb Aretes: " << _liens.size() << std::endl;
		for (int i = 0; i < _liens.size(); i++) {
			std::cout << "id: " << _liens[i]._id << " idnode1: " << _liens[i].getNoeud1()->getId() << " idnode2: " << _liens[i].getNoeud2()->getId();
			std::cout << " x1: " << _liens[i].getNoeud1()->getX() << " y1: " << _liens[i].getNoeud1()->getY();
			std::cout << " x2: " << _liens[i].getNoeud2()->getX() << " y2: " << _liens[i].getNoeud2()->getY();
			std::cout << std::endl;
		}
		std::cout << "-----------------------------------------------" << std::endl;
	}

	void afficherNoeuds() {
		std::cout << "-----------------------------------------------" << std::endl;
		std::cout << "Affichage DEBUG Noeuds:" << std::endl;
		std::cout << "Nb Noeuds: " << _noeuds.size() << std::endl;
		for (int i = 0; i < _noeuds.size(); i++) {
			std::cout << "id: " << _noeuds[i].getId() << " empid: ";
			if (_noeuds[i].getEmplacement() == nullptr) {
				std::cout << " aucun" << std::endl;
			}
			else {
				std::cout << _noeuds[i].getEmplacement()->_id << " x: " << _noeuds[i].getX() << " y: " << _noeuds[i].getY() << std::endl;
			}
		}
		std::cout << "-----------------------------------------------" << std::endl;
	}

	void afficherEmplacement() {
		std::cout << "-----------------------------------------------" << std::endl;
		std::cout << "Affichage DEBUG Emplacements:" << std::endl;
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

	void afficherNoeudSeul() {
		std::cout << "-----------------------------------------------" << std::endl;
		std::cout << "Affichage DEBUG Noeud Seul:" << std::endl;
		int nbNoeudSeul = 0;
		for (int i = 0; i < _noeuds.size(); i++) {
			if (_noeuds[i]._aretes.size() == 0) {
				std::cout << "Noeud: " << i << std::endl;
				nbNoeudSeul++;
			}
		}
		if (nbNoeudSeul == 0) {
			std::cout << "Aucun" << std::endl;
		}
		std::cout << "-----------------------------------------------" << std::endl;
	}

	void afficherInfo() {
		std::cout << "-----------------------------------------------" << std::endl;
		std::cout << "Information sur le graphe:" << std::endl;
		std::cout << "Nombre de noeud: " << _noeuds.size() << std::endl;
		std::cout << "Nombre d'emplacement: " << _emplacementsPossibles.size() << std::endl;
		std::cout << "Nombre d'aretes: " << _liens.size() << std::endl;
		std::cout << "-----------------------------------------------" << std::endl;
	}

	void afficherAreteDouble() {
		std::cout << "-----------------------------------------------" << std::endl;
		std::cout << "Affichage DEBUG Arrete Double:" << std::endl;
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
			std::cout << "Aucune" << std::endl;
		}
		std::cout << "-----------------------------------------------" << std::endl;
	}

	void afficherNoeudDouble() {
		std::cout << "-----------------------------------------------" << std::endl;
		std::cout << "Affichage DEBUG Noeud Double:" << std::endl;
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
			std::cout << "Aucun" << std::endl;
		}
		std::cout << "-----------------------------------------------" << std::endl;
	}

	// Effectue 100 déplacements de recuit simulé! 
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

	void clearNodeEmplacement() {
		for (int i = 0; i < _noeuds.size(); i++) {
			_noeuds[i].clearEmplacement();
		}
	}

	void placementAleatoire()
	{
		if (DEBUG_GRAPHE) std::cout << "Placement aleatoire" << std::endl;
		for (int i = 0; i < _noeuds.size(); ++i)
		{
			int emplacementId = generateRand(_emplacementsPossibles.size() - 1);
			while (!_emplacementsPossibles[emplacementId].estDisponible()) {
				emplacementId = (emplacementId + 1) % _emplacementsPossibles.size();
			}
			_noeuds[i].setEmplacement(&_emplacementsPossibles[emplacementId]);
		}
	}

	std::vector<Noeud> noeudsNonPlaces()
	{
		std::vector<Noeud> noeudsNonPlaces;
		for (Noeud noeud : _noeuds)
		{
			noeudsNonPlaces.push_back(noeud);
		}
		return noeudsNonPlaces;
	}

	void placementNoeudAleatoire(int idNoeud)
	{
		int emplacementId = generateRand(_emplacementsPossibles.size() - 1);
		while (!_emplacementsPossibles[emplacementId].estDisponible()) {
			emplacementId = (emplacementId + 1) % _emplacementsPossibles.size();
		}
		_noeuds[idNoeud].setEmplacement(&_emplacementsPossibles[emplacementId]);
	}

	bool hasIllegalCrossing() {
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
							return true;
						}
						else if (surSegment(_liens[j], *_liens[i].getNoeud1()) || surSegment(_liens[j], *_liens[i].getNoeud2())) {
							return true;
						}
					}
				}
				else {
					Noeud* nodeNotInCommon = _liens[j].nodeNotInCommon(_liens[i]);
					if (surSegment(_liens[i], *nodeNotInCommon))
					{
						return true;
					}
					else {
						nodeNotInCommon = _liens[i].nodeNotInCommon(_liens[j]);
						if (surSegment(_liens[j], *nodeNotInCommon)) {
							return true;
						}
					}
				}
			}
		}
		return false;
	}

	long getNbCroisement() {
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
					Noeud* nodeNotInCommon = _liens[j].nodeNotInCommon(_liens[i]);
					if (surSegment(_liens[i], *nodeNotInCommon))
					{
						total += PENALITE_MAX;
					}
					else {
						nodeNotInCommon = _liens[i].nodeNotInCommon(_liens[j]);
						if (surSegment(_liens[j], *nodeNotInCommon)) {
							total += PENALITE_MAX;
						}
					}
				}
			}
		}
		nombreCroisement = total;
		return total;
	}

	long getNbCroisementGlouton() {
		int debug = 0;
		long total = 0;
		for (int i = 0; i < _liens.size() - 1; ++i) {
			if (_liens[i].estPlace()) {
				for (int j = i + 1; j < _liens.size(); ++j) {
					if (_liens[j].estPlace()) {
						//Aretes aretes1 = _liens[i], aretes2 = _liens[j];
						if (!(_liens[i].contains(_liens[j].getNoeud1())
							|| _liens[i].contains(_liens[j].getNoeud2())))
						{
							if (seCroisent(_liens[i], _liens[j]))
							{
								if (surSegment(_liens[i], *_liens[j].getNoeud1())
									|| surSegment(_liens[i], *_liens[j].getNoeud2()))
								{
									total += PENALITE_MAX;
								}
								else {
									++total;
								}
							}
						}
						else {
							Noeud* nodeNotInCommon = _liens[j].nodeNotInCommon(_liens[i]);
							if (surSegment(_liens[i], *nodeNotInCommon))
							{
								total += PENALITE_MAX;
							}
						}
					}
				}
			}
		}
		return total;
	}

	int selectionNoeudTournoiBinaire() {
		int randomId = generateRand(_noeuds.size() - 1);
		int randomId2;
		do {
			randomId2 = generateRand(_noeuds.size() - 1);
		} while (randomId2 == randomId);
		if (getScoreCroisementNode(randomId) > getScoreCroisementNode(randomId2))
			return randomId;
		return randomId2;
	}

	int selectionNoeudTournoiMultiple(int n) {
		int randomId = generateRand(_noeuds.size() - 1);
		int scoreMeilleur = getScoreCroisementNode(randomId);
		if (n > 1) {
			for (int i = 0; i < n; i++) {
				int randomId2;
				do {
					randomId2 = generateRand(_noeuds.size() - 1);
				} while (randomId2 == randomId);
				int scoreId2 = getScoreCroisementNode(randomId2);
				if (scoreId2 > scoreMeilleur) {
					randomId = randomId2;
					scoreMeilleur = scoreId2;
				}
			}
		}
		return randomId;
	}

	int selectionEmplacementTournoiBinaire(int nodeId) {
		int randomEmpId = generateRand(_emplacementsPossibles.size() - 1); // Selection aléatoire d'un emplacement disponible (pas tres équiprobable)
		// on retire si on pioche le meme emplacement
		while (_noeuds[nodeId].getEmplacement()->getId() == randomEmpId) {
			randomEmpId = generateRand(_emplacementsPossibles.size() - 1);
		}
		int randomEmpId2 = generateRand(_emplacementsPossibles.size() - 1); // Selection aléatoire d'un emplacement disponible (pas tres équiprobable)
		// on retire si on pioche le meme emplacement
		while (_noeuds[nodeId].getEmplacement()->getId() == randomEmpId2) {
			randomEmpId2 = generateRand(_emplacementsPossibles.size() - 1);
		}
		double dist1 = ((_emplacementsPossibles[randomEmpId].getX() - _noeuds[nodeId].getX()) * (_emplacementsPossibles[randomEmpId].getX() - _noeuds[nodeId].getX())) + ((_emplacementsPossibles[randomEmpId].getY() - _noeuds[nodeId].getY()) * (_emplacementsPossibles[randomEmpId].getY() - _noeuds[nodeId].getY()));
		double dist2 = ((_emplacementsPossibles[randomEmpId2].getX() - _noeuds[nodeId].getX()) * (_emplacementsPossibles[randomEmpId2].getX() - _noeuds[nodeId].getX())) + ((_emplacementsPossibles[randomEmpId2].getY() - _noeuds[nodeId].getY()) * (_emplacementsPossibles[randomEmpId2].getY() - _noeuds[nodeId].getY()));
		if (dist2 < dist1) {
			randomEmpId = randomEmpId2;
		}
		return randomEmpId;
	}

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
			while (_noeuds[nodeId].getEmplacement()->getId() == randomEmpId2) {
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

	int selectionNoeud(int modeNoeud, int t) {
		int nodeId;
		if (modeNoeud == 0)
			nodeId = generateRand(_noeuds.size() - 1);
		else if (modeNoeud == 1) {
			nodeId = selectionNoeudTournoiBinaire();
		}
		else if (modeNoeud == 2) {
			int nbTirageNoeud = ((100 - t) / 15) + 1;
			nodeId = selectionNoeudTournoiMultiple(nbTirageNoeud);
		}
		return nodeId;
	}

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
	void recuitSimule(double cool = 0.99999, double t = 100.0, int delay = 1, int modeNoeud = 0, int modeEmplacement = 0) {
		std::vector<int> bestResult = saveCopy();
		long nbCroisement = getNbCroisement();
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
		if (DEBUG_GRAPHE) std::cout << "Meilleur resultat du recuit: " << bestCroisement << std::endl;
	}

	// Lance l'algorithme de recuit simulé sur le graphe pour minimiser le nombre d'intersection
	void stepRecuitSimule(double& t, int& nbCrois, double cool = 0.99999, int mode = 0) {
		std::vector<int> bestResult = saveCopy();
		int nbCroisement = nbCrois;
		if (DEBUG_GRAPHE) std::cout << "Nb Croisement avant recuit: " << nbCroisement << std::endl;
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
		loadCopy(bestResult);
	}

	// Lance l'algorithme de recuit simulé sur le graphe pour minimiser le nombre d'intersection
	// Utilise un delay pour rester plus longtemps à la meme température
	void recuitSimuleDelay(double cool = 0.99999, double t = 100, int delay = 10, int mode = 0) {
		std::vector<int> bestResult = saveCopy();
		int nbCroisement = getNbCroisement();
		if (DEBUG_GRAPHE) std::cout << "Nb Croisement avant recuit delay: " << nbCroisement << std::endl;
		for (int iter = 0; t > 0.0001 && nbCroisement > 0; iter++) {
			//if (iter % 100000 == 0) {
			//	std::cout << "Iter: " << iter << " t: " << t << " intersections: " << newNbIntersection << std::endl;
			//}
			for (int del = 0; del < delay; del++) {
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
			}
			t *= cool;
		}
		loadCopy(bestResult);
		if (DEBUG_GRAPHE) std::cout << "Meilleur resultat du recuit delay: " << nbCroisement << std::endl;
	}

	// Lance l'algorithme de recuit simulé sur le graphe pour minimiser le nombre d'intersection
	// Tire 2 emplacements aléatoires et prend le plus proche
	void recuitSimuleTournoiBinaire(double cool = 0.99999, double t = 100, int mode = 0) {
		std::vector<int> bestResult = saveCopy();
		int nbCroisement = getNbCroisement();
		if (DEBUG_GRAPHE) std::cout << "Nb Croisement avant recuit: " << nbCroisement << std::endl;
		for (int iter = 0; t > 0.0001 && nbCroisement > 0; iter++) {
			//if (iter % 100000 == 0) {
			//	std::cout << "Iter: " << iter << " t: " << t << " intersections: " << newNbIntersection << std::endl;
			//}
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
			int randomEmpId2 = generateRand(_emplacementsPossibles.size() - 1); // Selection aléatoire d'un emplacement disponible (pas tres équiprobable)
			// on retire si on pioche le meme emplacement
			while (_noeuds[randomId].getEmplacement()->getId() == randomEmpId2) {
				randomEmpId2 = generateRand(_emplacementsPossibles.size() - 1);
			}
			double dist1 = ((_emplacementsPossibles[randomEmpId].getX() - _noeuds[randomId].getX()) * (_emplacementsPossibles[randomEmpId].getX() - _noeuds[randomId].getX())) + ((_emplacementsPossibles[randomEmpId].getY() - _noeuds[randomId].getY()) * (_emplacementsPossibles[randomEmpId].getY() - _noeuds[randomId].getY()));
			double dist2 = ((_emplacementsPossibles[randomEmpId2].getX() - _noeuds[randomId].getX()) * (_emplacementsPossibles[randomEmpId2].getX() - _noeuds[randomId].getX())) + ((_emplacementsPossibles[randomEmpId2].getY() - _noeuds[randomId].getY()) * (_emplacementsPossibles[randomEmpId2].getY() - _noeuds[randomId].getY()));
			if (dist2 < dist1) {
				randomEmpId = randomEmpId2;
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
		loadCopy(bestResult);
		if (DEBUG_GRAPHE) std::cout << "Meilleur resultat du recuit: " << nbCroisement << std::endl;
	}

	// Lance l'algorithme de recuit simulé sur le graphe pour minimiser le nombre d'intersection
	// Tire plusieurs emplacements aléatoires et prend le plus proche, augmente le tirage avec le temps
	void recuitSimuleTournoiMultiple(double cool = 0.99999, double t = 100, int mode = 0) {
		std::vector<int> bestResult = saveCopy();
		int nbCroisement = getNbCroisement();
		if (DEBUG_GRAPHE) std::cout << "Nb Croisement avant recuit: " << nbCroisement << std::endl;
		for (int iter = 0; t > 0.0001 && nbCroisement > 0; iter++) {
			//if (iter % 100000 == 0) {
			//	std::cout << "Iter: " << iter << " t: " << t << " intersections: " << newNbIntersection << std::endl;
			//}
			int nbTirage = ((100 - t) / 15) + 1;
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
			double dist;
			if (nbTirage > 1) {
				dist = ((_emplacementsPossibles[randomEmpId].getX() - _noeuds[randomId].getX()) * (_emplacementsPossibles[randomEmpId].getX() - _noeuds[randomId].getX())) + ((_emplacementsPossibles[randomEmpId].getY() - _noeuds[randomId].getY()) * (_emplacementsPossibles[randomEmpId].getY() - _noeuds[randomId].getY()));
			}
			for (int i = 1; i < nbTirage; i++) {
				int randomEmpId2 = generateRand(_emplacementsPossibles.size() - 1); // Selection aléatoire d'un emplacement disponible (pas tres équiprobable)
				// on retire si on pioche le meme emplacement
				while (_noeuds[randomId].getEmplacement()->getId() == randomEmpId2) {
					randomEmpId2 = generateRand(_emplacementsPossibles.size() - 1);
				}
				double dist2 = ((_emplacementsPossibles[randomEmpId2].getX() - _noeuds[randomId].getX()) * (_emplacementsPossibles[randomEmpId2].getX() - _noeuds[randomId].getX())) + ((_emplacementsPossibles[randomEmpId2].getY() - _noeuds[randomId].getY()) * (_emplacementsPossibles[randomEmpId2].getY() - _noeuds[randomId].getY()));
				if (dist2 < dist) {
					randomEmpId = randomEmpId2;
					dist = dist2;
				}
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
		loadCopy(bestResult);
		if (DEBUG_GRAPHE) std::cout << "Meilleur resultat du recuit: " << nbCroisement << std::endl;
	}

	void rerecuitSimule(int iter = 10) {
		if (DEBUG_GRAPHE) std::cout << "Starting Rerecuit " << iter << " iterations." << std::endl;
		double cool = 0.99999, t = 100, coolt = 0.99;
		for (int i = 0; i < iter; i++) {
			if (DEBUG_GRAPHE) std::cout << "Starting Recuit Number: " << i << " t: " << t << " cool " << cool << std::endl;
			recuitSimule(cool, t);
			t *= coolt;
		}
	}

	void bestDeplacement() {
		long nbIntersection = getNbCroisement();
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
						int NbInterReelNew = getNbCroisement();
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
						if (NbInterReelNew != nbIntersection + improve) {
							return;
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
		if (DEBUG_GRAPHE) std::cout << "Meilleur resultat de l'algo meilleur deplacement: " << nbIntersection << std::endl;
	}

	Emplacement* getEmplacementPlusProche(const Point& origin)
	{
		Emplacement* meilleurEmplacement = &_emplacementsPossibles[generateRand(_emplacementsPossibles.size() - 1)];
		while (!meilleurEmplacement->estDisponible()) {
			meilleurEmplacement = &_emplacementsPossibles[(meilleurEmplacement->getId() + 1) % _emplacementsPossibles.size()];
		}
		int meilleurDistance = meilleurEmplacement->getPosition().distance(origin);
		int nbRencontre = 0;
		for (int i = 0; i < _emplacementsPossibles.size(); ++i)
		{
			if (_emplacementsPossibles[i].estDisponible() && i != meilleurEmplacement->getId())
			{
				int distanceActuel = _emplacementsPossibles[i].getPosition().distance(origin);
				if (meilleurDistance > distanceActuel)
				{
					meilleurEmplacement = &_emplacementsPossibles[i];
					meilleurDistance = distanceActuel;
					nbRencontre = 0;
				}
				else if (meilleurDistance == distanceActuel)
				{
					++nbRencontre;
					int aleatoire = generateRand(nbRencontre);
					if (aleatoire == 1)
					{
						meilleurEmplacement = &_emplacementsPossibles[i];
						meilleurDistance = distanceActuel;
					}
				}
			}

		}

		return meilleurEmplacement;
	}

	Emplacement* getEmplacementPlusProche(Emplacement* origin) {
		Emplacement* meilleurEmplacement = &_emplacementsPossibles[0];
		int meilleurDistance = meilleurEmplacement->getPosition()
			.distance(origin->getPosition());
		int nbRencontre = 0;
		for (int i = 1; i < _emplacementsPossibles.size(); ++i)
		{
			int distanceActuel = _emplacementsPossibles[i].getPosition()
				.distance(origin->getPosition());
			if (origin->getId() != _emplacementsPossibles[i].getId() && meilleurDistance > distanceActuel)
			{
				meilleurEmplacement = &_emplacementsPossibles[i];
				meilleurDistance = distanceActuel;
				nbRencontre = 0;
			}
			else if (meilleurDistance == distanceActuel)
			{
				++nbRencontre;
				int aleatoire = generateRand(nbRencontre);
				if (aleatoire == 1)
				{
					meilleurEmplacement = &_emplacementsPossibles[i];
					meilleurDistance = distanceActuel;
				}
			}
		}

		return meilleurEmplacement;
	}

	Point getCentreGravite()
	{
		double totalX = 0.0, totalY = 0.0;
		for (Emplacement emplacement : _emplacementsPossibles)
		{
			totalX += emplacement.getX();
			totalY += emplacement.getY();
		}

		return Point(
			totalX / _emplacementsPossibles.size(),
			totalY / _emplacementsPossibles.size()
		);
	}

	Point getCentreGraviteNoeudPlaces()
	{
		double totalX = 0.0, totalY = 0.0;
		for (Noeud noeud : _noeuds)
		{
			if (noeud.estPlace())
			{
				totalX += noeud.getX();
				totalY += noeud.getY();
			}
		}

		return Point(
			totalX / _emplacementsPossibles.size(),
			totalY / _emplacementsPossibles.size()
		);
	}

	Point getCentreGraviteVoisin(Noeud* noeud)
	{
		double totalX = 0.0, totalY = 0.0;
		for (Noeud* noeudParcouru : noeud->getVoisins())
		{
			if (noeudParcouru->estPlace())
			{
				totalX += noeudParcouru->getX();
				totalY += noeudParcouru->getY();
			}
		}

		return Point(
			totalX / _emplacementsPossibles.size(),
			totalY / _emplacementsPossibles.size()
		);
	}

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

	Emplacement* getMeilleurEmplacementGravite(Noeud* meilleurNoeud, Point gravite)
	{
		Emplacement* oldEmplacement = meilleurNoeud->getEmplacement();
		int nbRencontre = 0;
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
				else if (newScore == bestScore &&
					_emplacementsPossibles[bestId].getPosition().distance(gravite) >
					_emplacementsPossibles[index].getPosition().distance(gravite))
				{
					bestScore = newScore;
					bestId = index;
				}
			}
		}
		if (oldEmplacement == nullptr)
		{
			meilleurNoeud->clearEmplacement();
		}
		else
		{
			meilleurNoeud->setEmplacement(oldEmplacement);
		}
		return &_emplacementsPossibles[bestId];
	}

	void glouton() {
		for (int i = 0; i < _noeuds.size(); i++) {
			// Selection aléatoire du noeud
			int randomId = generateRand(_noeuds.size() - 1);
			while (_noeuds[randomId].getEmplacement() != nullptr) {
				randomId = (randomId + 1) % _noeuds.size();
			}
			long bestScore = 99999999999999999;
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
	}

	void gloutonRevisite()
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
	}

	bool emplacementRestant() {
		for (int i=0;i<_emplacementsPossibles.size();i++) {
			if (_emplacementsPossibles[i].estDisponible()) {
				return true;
			}
		}
		return false;
	}

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

	std::vector<int> saveCopy() {
		std::vector<int> vectorId;
		for (int i = 0; i < _noeuds.size(); i++) {
			vectorId.push_back(_noeuds[i].getEmplacement()->getId());
		}
		return vectorId;
	}

	void loadCopy(std::vector<int> vectorId) {
		for (int i = 0; i < _noeuds.size(); i++) {
			_noeuds[i].clearEmplacement();
		}
		for (int i = 0; i < _noeuds.size(); i++) {
			_noeuds[i].setEmplacement(&_emplacementsPossibles[vectorId[i]]);
		}
	}

	void clearGraphe() {
		_noeuds.clear();
		_emplacementsPossibles.clear();
		_liens.clear();
	}

	bool isInVector(std::vector<int>& vectorInt, int x) {
		for (int i = 0; i < vectorInt.size(); i++) {
			if (vectorInt[i] == x) return true;
		}
		return false;
	}

	void copyFromGraphe(Graphe& graphe)
	{
		_emplacementsPossibles.clear();
		_noeuds.clear();
		_liens.clear();
		for (int i = 0; i < graphe._emplacementsPossibles.size(); ++i)
		{
			_emplacementsPossibles.push_back(
				Emplacement(
					Point(graphe._emplacementsPossibles[i].getX(), graphe._emplacementsPossibles[i].getY())
					, i)
			);
		}
		for (int i = 0; i < graphe._noeuds.size(); ++i)
		{
			_noeuds.push_back(Noeud(i));
		}

		for (int i = 0; i < graphe._liens.size(); ++i)
		{
			int id1 = graphe._liens[i].getNoeud1()->getId();
			int id2 = graphe._liens[i].getNoeud2()->getId();
			_liens.push_back(Aretes(&_noeuds[id1], &_noeuds[id2], i));
		}
		for (int i = 0; i < graphe._noeuds.size(); ++i)
		{
			if (graphe._noeuds[i].getEmplacement() != nullptr) {
				int idEmplacement = graphe._noeuds[i].getEmplacement()->getId();
				_noeuds[i].setEmplacement(&_emplacementsPossibles[idEmplacement]);
			}
		}
	}

	int getScoreCroisementNode(int nodeIndex) {
		long score = 0;
		std::vector<int> indexPasse;
		for (int i = 0; i < _noeuds[nodeIndex]._aretes.size(); ++i) {
			int index = _noeuds[nodeIndex]._aretes[i];
			for (int j = 0; j < _liens.size(); ++j) {
				if ((_liens[index]._id != _liens[j]._id) && (!isInVector(indexPasse, j))) {
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
						Noeud* nodeNotInCommon = _liens[j].nodeNotInCommon(_liens[index]);
						if (surSegment(_liens[index], *nodeNotInCommon)) {
							score += PENALITE_MAX;
							if (DEBUG_OPENGL) areteIll.insert(&_liens[index]);
						}
						else {
							nodeNotInCommon = _liens[index].nodeNotInCommon(_liens[j]);
							if (surSegment(_liens[j], *nodeNotInCommon)) {
								score += PENALITE_MAX;
								if (DEBUG_OPENGL) areteIll.insert(&_liens[index]);
							}
						}
					}
				}
			}
			indexPasse.push_back(index);
		}
		return score;
	}

	int getScoreCroisementNode(int nodeIndex, int swapIndex) {
		long score = 0;
		std::vector<int> indexPasse;
		for (int i = 0; i < _noeuds[nodeIndex]._aretes.size(); ++i) {
			int index = _noeuds[nodeIndex]._aretes[i];
			if (!_liens[index].contains(swapIndex)) {
				for (int j = 0; j < _liens.size(); ++j) {
					if ((_liens[index]._id != _liens[j]._id) && (!isInVector(indexPasse, j))) {
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
								Noeud* nodeNotInCommon = _liens[j].nodeNotInCommon(_liens[index]);
								if (surSegment(_liens[index], *nodeNotInCommon)) {
									score += PENALITE_MAX;
								}
								else {
									nodeNotInCommon = _liens[index].nodeNotInCommon(_liens[j]);
									if (surSegment(_liens[j], *nodeNotInCommon)) {
										score += PENALITE_MAX;
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
		std::vector<int> saveGraphe1 = graphe1.saveCopy();
		std::vector<int> saveGraphe2 = graphe2.saveCopy();
		int nbNoeudATraiter = graphe1._noeuds.size() - graphe1.nbNoeudEnCommun(graphe2);
		//std::cout << "Nb noeud a traiter au debut: " << nbNoeudATraiter << "\n";
		if (nbNoeudATraiter == 0) {
			copyFromGraphe(graphe1);
			return false;
		}

		Graphe* currentGraphe, * otherGraphe;
		int currentGrapheNumber = generateRand(1) + 1;
		if (currentGrapheNumber == 1) { currentGraphe = &graphe1; otherGraphe = &graphe2; }
		else { currentGraphe = &graphe2; otherGraphe = &graphe1; }

		while (nbNoeudATraiter > 0) {
			int bestNodeId = -1;
			int meilleurScore;
			int nbRencontre = 0;
			//std::cout << "Nb noeud a traiter encore: " << nbNoeudATraiter << "\n";

			//Trouve le meilleur noeud du graphe en cours d'analyse
			for (int i = 0; i < _noeuds.size(); ++i) {
				if (!_noeuds[i].estPlace()) {
					int nodeScore = currentGraphe->getScoreCroisementNode(i);
					if (bestNodeId == -1 || nodeScore < meilleurScore) {
						bestNodeId = currentGraphe->_noeuds[i].getId();
						meilleurScore = nodeScore;
						nbRencontre = 1;
					}
					else if (meilleurScore == nodeScore) {
						++nbRencontre;
						if (generateRand(nbRencontre) == 1) {
							bestNodeId = currentGraphe->_noeuds[i].getId();
						}
					}
				}
			}

			int bestEmplacementId = currentGraphe->_noeuds[bestNodeId].getEmplacement()->getId();
			_noeuds[bestNodeId].setEmplacement(&_emplacementsPossibles[bestEmplacementId]);
			if (!graphe1._noeuds[bestNodeId].compare(&graphe2._noeuds[bestNodeId])) {
				--nbNoeudATraiter;
				otherGraphe->_noeuds[bestNodeId].ecraseNoeud(otherGraphe->_emplacementsPossibles[bestEmplacementId]);
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
					if (graphe1._noeuds[idNodeVoisin].estPlace() && graphe2._noeuds[idNodeVoisin].estPlace()) {
						if (!graphe1._noeuds[idNodeVoisin].compare(&graphe2._noeuds[idNodeVoisin])) {
							--nbNoeudATraiter;
						}
					}
					else {
						--nbNoeudATraiter;
					}
					currentGraphe->_noeuds[idNodeVoisin].ecraseNoeud(currentGraphe->_emplacementsPossibles[bestEmplacementId]);
					otherGraphe->_noeuds[idNodeVoisin].ecraseNoeud(otherGraphe->_emplacementsPossibles[bestEmplacementId]);
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
				graphe1.completePlacementAleatoire();
				graphe2.completePlacementAleatoire();
			}
			else {
				graphe1.completeBasicGlouton();
				graphe2.completeBasicGlouton();
			}
			//Si les lieux coincident les noeuds ne sont plus à traiter
			for (int i = 0; i < noeudsAVerifier.size(); ++i) {
				if (graphe1._noeuds[noeudsAVerifier[i]].compare(&graphe2._noeuds[noeudsAVerifier[i]])) {
					--nbNoeudATraiter;
				}
			}


			//Change le parent choisis 
			if (currentGrapheNumber == 1) {
				currentGraphe = &graphe2;
				otherGraphe = &graphe1;
				currentGrapheNumber = 0;
			}
			else {
				currentGraphe = &graphe1;
				otherGraphe = &graphe2;
				currentGrapheNumber = 1;
			}
		}

		_noeuds.swap(graphe1._noeuds);
		_liens.swap(graphe1._liens);
		_emplacementsPossibles.swap(graphe1._emplacementsPossibles);
		graphe1.loadCopy(saveGraphe1);
		graphe2.loadCopy(saveGraphe2);
		return true;
	}

	void stepCroisementVoisinageFrom(Graphe& graphe1, Graphe& graphe2, bool useRand, int &nbNoeudATraiter, int &currentGrapheNumber) {
		if (nbNoeudATraiter == 0) {
			return;
		}

		Graphe* currentGraphe, * otherGraphe;
		if (currentGrapheNumber == 1) { currentGraphe = &graphe1; otherGraphe = &graphe2; }
		else { currentGraphe = &graphe2; otherGraphe = &graphe1; }

		int bestNodeId = -1;
		int bestEmplacementId = -1;
		int meilleurScore;
		int nbRencontre = 0;

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
						i;
					}
				}
			}
		}
		std::cout << "NUMERO 0:\n";
		graphe1.afficherNoeudDouble();
		graphe2.afficherNoeudDouble();

		bestEmplacementId = currentGraphe->_noeuds[bestNodeId].getEmplacement()->getId();
		_noeuds[bestNodeId].setEmplacement(&_emplacementsPossibles[bestEmplacementId]);
		if (!graphe1._noeuds[bestNodeId].compare(&graphe2._noeuds[bestNodeId])) {
			--nbNoeudATraiter;
			otherGraphe->_noeuds[bestNodeId].ecraseNoeud(otherGraphe->_emplacementsPossibles[bestEmplacementId]);
		}
		std::cout << "NUMERO 1:\n";
		graphe1.afficherNoeudDouble();
		graphe2.afficherNoeudDouble();
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
				if (graphe1._noeuds[idNodeVoisin].estPlace() && graphe2._noeuds[idNodeVoisin].estPlace()) {
					if (!graphe1._noeuds[idNodeVoisin].compare(&graphe2._noeuds[idNodeVoisin])) {
						--nbNoeudATraiter;
					}
				}
				else {
					--nbNoeudATraiter;
				}
				currentGraphe->_noeuds[idNodeVoisin].ecraseNoeud(currentGraphe->_emplacementsPossibles[bestEmplacementId]);
				otherGraphe->_noeuds[idNodeVoisin].ecraseNoeud(otherGraphe->_emplacementsPossibles[bestEmplacementId]);
			}
		}
		std::cout << "NUMERO 2:\n";
		graphe1.afficherNoeudDouble();
		graphe2.afficherNoeudDouble();

		//Liste des noeuds à placer
		std::vector<int> noeudsAVerifier;
		for (int i = 0; i < _noeuds.size(); ++i) {
			if (!graphe1._noeuds[i].estPlace() || !graphe2._noeuds[i].estPlace()) {
				noeudsAVerifier.push_back(i);
			}
		}
		if (useRand) {
			graphe1.completePlacementAleatoire();
			graphe2.completePlacementAleatoire();
		}
		else {
			graphe1.completeBasicGlouton();
			graphe2.completeBasicGlouton();
		}
		std::cout << "NUMERO 3:\n";
		graphe1.afficherNoeudDouble();
		graphe2.afficherNoeudDouble();
		//Si les lieux coincident les noeuds ne sont plus à traiter
		for (int i = 0; i < noeudsAVerifier.size(); ++i) {
			if (graphe1._noeuds[noeudsAVerifier[i]].compare(&graphe2._noeuds[noeudsAVerifier[i]])) {
				--nbNoeudATraiter;
			}
		}
		currentGrapheNumber = (currentGrapheNumber+1)%2;
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
		int startVec2 = sortedEmpId.size() / 2;

		Graphe* currentGraphe, * otherGraphe;
		int currentGrapheNumber = generateRand(1) + 1;
		int startVec, endVec;
		if (currentGrapheNumber == 1) { currentGraphe = &graphe1; otherGraphe = &graphe2; startVec = 0; endVec = startVec2; }
		else { currentGraphe = &graphe2; otherGraphe = &graphe1; startVec = startVec2; endVec = sortedEmpId.size(); }
		while (nbNoeudATraiter > 0) {
			Noeud* meilleurNoeud = nullptr;
			int meilleurScore;
			int nbRencontre = 0;
			for (int i = startVec; i < endVec; ++i) { // Recherche du meilleur noeud du coté du graphe choisi
				std::cout << "NNT: " << nbNoeudATraiter << " i: " << i << " Current: " << currentGrapheNumber << std::endl;
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
							}
						}
						else {
							--nbNoeudATraiter;
						}
						graphe1._noeuds[noeudVoisin->getId()].ecraseNoeud(graphe1._emplacementsPossibles[meilleurEmplacement]);
						graphe2._noeuds[noeudVoisin->getId()].ecraseNoeud(graphe2._emplacementsPossibles[meilleurEmplacement]);
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
					graphe1.completePlacementAleatoire();
					graphe2.completePlacementAleatoire();
				}
				else {
					graphe1.completeBasicGlouton();
					graphe2.completeBasicGlouton();
				}
				//Si les lieux coincident les noeuds ne sont plus à traiter
				for (int i = 0; i < noeudsAVerifier.size(); ++i) {
					if (graphe1._noeuds[noeudsAVerifier[i]].compare(&graphe2._noeuds[noeudsAVerifier[i]])) {
						--nbNoeudATraiter;
					}
				}
			}

			//Change le parent choisis 
			if (currentGrapheNumber == 1) {
				currentGraphe = &graphe2;
				otherGraphe = &graphe1;
				currentGrapheNumber = 0;
			}
			else {
				currentGraphe = &graphe1;
				otherGraphe = &graphe2;
				currentGrapheNumber = 1;
			}
		}

		_noeuds.swap(graphe1._noeuds);
		_liens.swap(graphe1._liens);
		_emplacementsPossibles.swap(graphe1._emplacementsPossibles);
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

	void croisementAleatoireFrom(Graphe* graphe1, Graphe* graphe2)
	{
		std::vector<int> noeudNonTraite;
		for (int noeud = 0; noeud < _noeuds.size(); ++noeud)
		{
			int random = generateRand(1) + 1;
			Graphe* currentGraphe;
			if (random == 1)
			{
				currentGraphe = graphe1;
			}
			else
			{
				currentGraphe = graphe2;
			}
			int idEmplacement = currentGraphe->_noeuds[noeud].getEmplacement()->getId();
			if (_emplacementsPossibles[idEmplacement].estDisponible())
			{
				_noeuds[noeud].setEmplacement(&_emplacementsPossibles[noeud]);
			}
			else
			{
				noeudNonTraite.push_back(noeud);
			}
		}

		for (int noeud : noeudNonTraite)
		{
			placementNoeudAleatoire(noeud);
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
					Noeud* nodeNotInCommon = _liens[j].nodeNotInCommon(_liens[i]);
					if (surSegment(_liens[i], *nodeNotInCommon))
					{
						total += PENALITE_MAX;
					}
					else {
						nodeNotInCommon = _liens[i].nodeNotInCommon(_liens[j]);
						if (surSegment(_liens[j], *nodeNotInCommon)) {
							total += PENALITE_MAX;
						}
					}
				}
			}
		}
		return total;
	}

	int nbNoeudNonPlace() {
		int total = _noeuds.size();
		for (int i=0;i<_noeuds.size();i++) {
			if (_noeuds[i].getEmplacement() != nullptr) {
				total--;
			}
		}
		return total;
	}

	bool operator < (const Graphe& G) const {
		//return (getNbCroisementConst() < G.getNbCroisementConst());
		return (nombreCroisement < G.nombreCroisement);
	}

};

#endif