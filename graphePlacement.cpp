#include "graphe.hpp"
#include "stressMaj.hpp"
#include "pivotMDS.hpp"
#include "intersection.hpp"
#include "ogdfFunctions.hpp"
#include <iostream>
#include <climits>

// Place les noeuds aleatoirement sur les emplacements disponibles.
// Ne tient pas a jour le score des noeuds ou du graphe.
void Graphe::placementAleatoire() {
    #if defined(DEBUG_GRAPHE_PROGRESS)
        tcout() << "Placement aleatoire" << std::endl;
    #endif
    for (int i = 0; i < _noeuds.size(); ++i) {
        int emplacementId = generateRand(_emplacements.size() - 1);
        while (!_emplacements[emplacementId].estDisponible()) {
            emplacementId = (emplacementId + 1) % _emplacements.size();
        }
        _noeuds[i].setEmplacement(&_emplacements[emplacementId]);
    }
    isNombreCroisementUpdated = false;
}

// Place les noeuds sur l'emplacement de meme ID
// Ne tient pas a jour le score des noeuds ou du graphe.
void Graphe::placementFixe() {
    #if defined(DEBUG_GRAPHE)
        tcout() << "Placement fixe" << std::endl;
    #endif
    for (int i=0;i<_noeuds.size();i++) {
        _noeuds[i].setEmplacement(&_emplacements[i]);
    }
    isNombreCroisementUpdated = false;
}

// Place un noeud aleatoirement sur un emplacement disponible.
// Ne tient pas a jour le score du noeud ou du graphe.
void Graphe::placementNoeudAleatoire(int idNoeud) {
    int emplacementId = generateRand(_emplacements.size() - 1);
    while (!_emplacements[emplacementId].estDisponible()) {
        emplacementId = (emplacementId + 1) % _emplacements.size();
    }
    _noeuds[idNoeud].setEmplacement(&_emplacements[emplacementId]);
    isNombreCroisementUpdated = false;
}

// Selectionne les emplacements disponibles a egale distance d'un point et en renvoie un aleatoirement.
Emplacement* Graphe::getEmplacementPlusProche(std::pair<int,int>& origin) {
    Emplacement* meilleurEmplacement = &_emplacements[generateRand(_emplacements.size() - 1)];
    while (!meilleurEmplacement->estDisponible()) {
        meilleurEmplacement = &_emplacements[(meilleurEmplacement->getId() + 1) % _emplacements.size()];
    }
    int meilleurDistance = meilleurEmplacement->distance(origin);
    int nbRencontre = 0;
    for (int i = 0; i < _emplacements.size(); ++i) {
        if (_emplacements[i].estDisponible() && i != meilleurEmplacement->getId()) {
            int distanceActuel = _emplacements[i].distance(origin);
            if (meilleurDistance > distanceActuel) {
                meilleurEmplacement = &_emplacements[i];
                meilleurDistance = distanceActuel;
                nbRencontre = 0;
            }
            else if (meilleurDistance == distanceActuel) {
                ++nbRencontre;
                if (generateRand(nbRencontre) == 1) {
                    meilleurEmplacement = &_emplacements[i];
                    meilleurDistance = distanceActuel;
                }
            }
        }

    }
    return meilleurEmplacement;
}

// Selectionne les emplacements different de l'emplacement en parametre a egale distance de celui ci et en renvoie un aleatoirement.
Emplacement* Graphe::getEmplacementPlusProche(Emplacement* origin) {
    Emplacement* meilleurEmplacement = &_emplacements[0];
    int meilleurDistance = meilleurEmplacement->distance(origin);
    int nbRencontre = 0;
    for (int i = 1; i < _emplacements.size(); ++i) {
        if (origin->getId() != _emplacements[i].getId()) {
            int distanceActuel = _emplacements[i].distance(origin);
            if (distanceActuel < meilleurDistance) {
                meilleurEmplacement = &_emplacements[i];
                meilleurDistance = distanceActuel;
                nbRencontre = 0;
            }
            else if (meilleurDistance == distanceActuel) {
                ++nbRencontre;
                if (generateRand(nbRencontre) == 1) {
                    meilleurEmplacement = &_emplacements[i];
                }
            }
        }
    }
    return meilleurEmplacement;
}

// Retourne le centre de gravite des emplacements.
std::pair<int,int> Graphe::getCentreGravite() {
    double totalX = 0.0, totalY = 0.0;
    for (int i=0;i<_emplacements.size();i++) {
        totalX += _emplacements[i].getX();
        totalY += _emplacements[i].getY();
    }
    return std::pair<int,int>(totalX / _emplacements.size(),totalY / _emplacements.size());
}

// Retourne le centre de gravite des noeuds place
std::pair<int,int> Graphe::getCentreGraviteNoeudPlaces() {
    double totalX = 0.0, totalY = 0.0;
    int nbIter = 0;
    for (int i=0;i<_noeuds.size();i++) {
        if (_noeuds[i].estPlace()){
            totalX += _noeuds[i].getX();
            totalY += _noeuds[i].getY();
            nbIter++;
        }
    }
    return std::pair<int,int>(totalX / nbIter,totalY / nbIter);
}

// Retourne le centre de gravite des voisins place d'un noeud.
std::pair<int,int> Graphe::getCentreGraviteVoisin(Noeud* noeud) {
    double totalX = 0.0, totalY = 0.0;
    int nbIter = 0;
    for (int i=0;i<noeud->voisins.size();i++) {
        if (noeud->voisins[i]->estPlace()) {
            totalX += noeud->voisins[i]->getX();
            totalY += noeud->voisins[i]->getY();
            nbIter++;
        }
    }
    return std::pair<int,int>(totalX / nbIter,totalY / nbIter);
}

void Graphe::stressMajorization(std::vector<std::vector<double>> customParam, int methode, bool useClosest) {
    if (!estPlace()) { placementAleatoire(); }
    for (int i=0;i<_noeuds.size();i++) {
        _noeuds[i].stressX = (double)_noeuds[i].getEmplacement()->getX();
        _noeuds[i].stressY = (double)_noeuds[i].getEmplacement()->getY();
    }
    if (_sm.G == nullptr) { _sm.G = this; }
    bool useGrille = _emplacements.size() >= _noeuds.size() * 2;
    #if defined(DEBUG_GRAPHE)
        if (useGrille) { tcout() << "Debut Stress Majorization avec grille.\n"; }
        else { tcout() << "Debut Stress Majorization sans grille.\n"; }
    #endif
    if (useGrille) {
        _sm.m_useGrille = true;
        initGrilleCarre();
        registerSlotsInGridNoMove();
    }
    else {
        _sm.m_useGrille = false;
    }
    _sm.m_useClosest = useClosest;
    if (customParam.size() > 0) {
        for (std::vector<double>& param : customParam) {
            if (param.size() > 0) {
                if (param[0] == 1) {
                    _sm.m_edgeCosts = param[1];
                }
                if (param[0] == 2) {
                    _sm.m_iterations = param[1];
                }
            }
        }
    }
    if (methode == 0) {
        _sm.runAlgo();
    }
    if (useGrille) { deleteGrille(); }
    #if defined(DEBUG_GRAPHE)
        tcout() << "Fin Stress Majorization\n";
    #endif
}

void Graphe::placementPivotMDS(std::vector<std::vector<double>> customParam, int edgeCost, int nbPivot) {
    if (_pmds.G == nullptr) { _pmds.G = this; }
    _pmds.m_edgeCosts = edgeCost;
    _pmds.m_numberOfPivots = nbPivot;
    bool useGrille = _emplacements.size() > _noeuds.size() * 5;
    if (useGrille) {
        initGrilleCarre();
        registerSlotsInGridNoMove();
        _pmds.m_useGrille = true;
    }
    else {
        _pmds.m_useGrille = false;
    }
    _pmds.runAlgo();
    if (useGrille) { deleteGrille(); }
}

void Graphe::placementFMME(bool minute) {
#if defined(OGDF_INSTALLED)
     if (minute) {
        ogdfFastMultipoleMultilevelEmbedderMinute(*this);
    }
    else {
        ogdfFastMultipoleMultilevelEmbedder(*this);
    }
#else
    placementAleatoire();
#endif
}