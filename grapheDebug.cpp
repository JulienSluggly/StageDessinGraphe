#include "graphe.hpp"
#include <iostream>

bool globalDebugVar = false;
bool globalDebugVar2 = false;
bool globalDebugVar3 = false;

void Graphe::afficherNoeudsReel(std::string nom) {
    tcout() << "-----------------------------------------------" << std::endl;
    tcout() << "Affichage DEBUG Noeuds: " << nomGraphe << " " << nom << std::endl;
    tcout() << "Nb Noeuds: " << _noeuds.size() << std::endl;
    for (int i = 0; i < _noeuds.size(); i++) {
        tcout() << "id: " << _noeuds[i].getId() << " xreel: " << _noeuds[i]._xreel << " yreel: " << _noeuds[i]._yreel << std::endl;
    }
    tcout() << "-----------------------------------------------" << std::endl;
}


void Graphe::afficherInfo(std::string nom) {
    tcout() << "-----------------------------------------------" << std::endl;
    tcout() << "Information sur le graphe: " << nomGraphe << " " << nom << std::endl;
    tcout() << "Nombre de noeud: " << _noeuds.size() << std::endl;
    tcout() << "Nombre d'aretes: " << _aretes.size() << std::endl;
    tcout() << "Max Voisin: " << maxVoisin << std::endl;
    tcout() << "-----------------------------------------------" << std::endl;
}

int Graphe::afficherAreteDouble(bool display, std::string nom) {
    int retour = 1;
    if (display) {
        tcout() << "-----------------------------------------------" << std::endl;
        tcout() << "Affichage DEBUG Arrete Double: " << nomGraphe << " " << nom << std::endl;
    }
    int nbAreteDouble = 0;
    for (int i = 0; i < _aretes.size(); i++) {
        int id1 = _aretes[i].getNoeud1()->getId();
        int id2 = _aretes[i].getNoeud2()->getId();
        for (int j = i + 1; j < _aretes.size(); j++) {
            int id12 = _aretes[j].getNoeud1()->getId();
            int id22 = _aretes[j].getNoeud2()->getId();
            if (((id1 == id12) && (id2 == id22)) || ((id1 == id22) && (id2 == id12))) {
                tcout() << "Arete double: " << i << " & " << j << " Noeud A1: " << id1 << " & " << id2 << " Noeud A2: " << id12 << " & " << id22 << std::endl;
                nbAreteDouble++;
            }
        }
    }
    if (nbAreteDouble == 0) {
        if (display) { tcout() << "Aucune" << std::endl; }
        retour=0;
    }
    else {
        if (!display) { tcout() << "-----------------------------------------------" << std::endl; }
    }
    if (display) { tcout() << "-----------------------------------------------" << std::endl; }
    return retour;
}

int Graphe::afficherNoeudSeul(bool display, std::string nom) {
    int retour = 10;
    if (display) {
        tcout() << "-----------------------------------------------" << std::endl;
        tcout() << "Affichage DEBUG Noeud Seul: " << nomGraphe << " " << nom << std::endl;
    }
    int nbNoeudSeul = 0;
    for (int i = 0; i < _noeuds.size(); i++) {
        if (_noeuds[i]._aretes.size() == 0) {
            tcout() << "Noeud Seul: " << i << std::endl;
            nbNoeudSeul++;
        }
    }
    if (nbNoeudSeul == 0) {
        if (display) { tcout() << "Aucun" << std::endl; }
        retour = 0;
    }
    else {
        if (!display) { tcout() << "-----------------------------------------------" << std::endl; }
    }
    if (display) { tcout() << "-----------------------------------------------" << std::endl; }
    return retour;
}

int Graphe::debugScoreGraphe(bool display, bool useArray, std::string nom) {
    int retour = 400;
    if (display) { 
        tcout() << "-----------------------------------------------" << std::endl;
        tcout() << "Affichage DEBUG Score Graphe: " << nomGraphe << " " << nom << std::endl;
    }
    bool scoreFaux = true;
    long scoreReel;
    if (useCoordReel) { scoreReel = getNbCroisementReelConst(); }

    if (nombreCroisement != scoreReel) {
        tcout() << "Croisement: " << nombreCroisement << " reel: " << scoreReel << std::endl;
        if (!display) { tcout() << "-----------------------------------------------" << std::endl; }
        scoreFaux = false;
    }
    if (scoreFaux) {
        if (display) { tcout() << "Score Correct" << std::endl; }
        retour = 0;
    }
    if (display) { tcout() << "-----------------------------------------------" << std::endl; }
    return retour;
}

int Graphe::debugDesyncAreteCell(bool display, std::string nom) {
    int retour = 4000;
    if (display) {
        tcout() << "-----------------------------------------------" << std::endl;
        tcout() << "Affichage DEBUG desync arete cellule: " << nomGraphe << " " << nom << std::endl;
    }
    long nbFail = 0;
    for (int i = 0; i < _aretes.size(); i++) {
        for (const int& id : _aretes[i].vecIdCellules) {
            if (!isInVector(grillePtr[id]->vecAreteId,i)) {
                tcout() << "Arete: " << i << " Cellule: " << id << std::endl;
                nbFail++;
            }
        }
    }
    for (int i=0;i<grillePtr.size();i++) {
        for (const int& id : grillePtr[i]->vecAreteId) {
            if (!isInVector(_aretes[id].vecIdCellules,i)) {
                tcout() << "Cellule: " << i << " Arete: " << id << std::endl;
                nbFail++;
            }
        }
    }
    if (nbFail == 0) {
        if (display) { tcout() << "Aucun" << std::endl; }
        retour = 0;
    }
    else {
        if (!display) { tcout() << "-----------------------------------------------" << std::endl; }
    }
    if (display) { tcout() << "-----------------------------------------------" << std::endl; }
    return retour;
}

int Graphe::debugOrdreIdVector(bool display, std::string nom) {
    int retour = 10000;
    if (display) {
        tcout() << "-----------------------------------------------" << std::endl;
        tcout() << "Debug Ordre Id Noeud Arete: " << nomGraphe << " " << nom << std::endl;
    }
    long nbWrongId = 0;
    for (int i = 0; i < _noeuds.size(); i++) {
        if (i!=_noeuds[i]._id) {
            tcout() << "i: " << i << " Noeud Id: " << _noeuds[i]._id << std::endl;
            nbWrongId++;
        }
    }
    for (int i=0;i<_aretes.size();i++) {
        if (i!=_aretes[i]._id) {
            tcout() << "i: " << i << " Arete Id: " << _aretes[i]._id << std::endl;
            nbWrongId++;
        }
    }
    if (nbWrongId == 0) {
        if (display) { tcout() << "Aucun" << std::endl; }
        retour = 0;
    }
    else {
        if (!display) { tcout() << "-----------------------------------------------" << std::endl; }
    }
    if (display) { tcout() << "-----------------------------------------------" << std::endl; }
    return retour;
}

int Graphe::debugSelfLoop(bool display, std::string nom) {
    int retour = 20000;
    if (display) {
        tcout() << "-----------------------------------------------" << std::endl;
        tcout() << "Debug Self Loop: " << nomGraphe << " " << nom << std::endl;
    }
    long nbLoop = 0;
    for (int i = 0; i < _noeuds.size(); i++) {
        for (const Noeud* voisin : _noeuds[i].voisins) {
            if (voisin->_id == _noeuds[i]._id) {
                tcout() << "Self Loop Node Id: " << voisin->_id << std::endl;
                nbLoop++;
            }
        }
    }
    if (nbLoop == 0) {
        if (display) { tcout() << "Aucun" << std::endl; }
        retour = 0;
    }
    else {
        if (!display) { tcout() << "-----------------------------------------------" << std::endl; }
    }
    if (display) { tcout() << "-----------------------------------------------" << std::endl; }
    return retour;
}

int Graphe::debugEverything(bool displayOther, bool displaySelf) {
#if defined(DEBUG_GRAPHE_PROGRESS)
    tcout() << "Debut debugging graphe\n";
#endif
    int total = 0;
    #if defined(DEBUG)
    if (displaySelf) { tcout() << "Debut debug arete double\n"; }
    total += afficherAreteDouble(displayOther);
    if (isNombreCroisementUpdated) {
        if (displaySelf) { tcout() << "Debut debug score graphe\n"; }
        total += debugScoreGraphe(displayOther);
    }
    if (grille.size() > 0) {
        if (displaySelf) { tcout() << "Debut debug desync arete cellule\n"; }
        total += debugDesyncAreteCell(displayOther);
    }
    if (displaySelf) { tcout() << "Debug Ordre Id Noeud Arete\n"; }
    total += debugOrdreIdVector(displayOther);
    if (displaySelf) { tcout() << "Debug Self Loop\n"; }
    total += debugSelfLoop(displayOther);
    if (displaySelf) { tcout() << "Debug fini\n"; }
    #endif
    return total;
}