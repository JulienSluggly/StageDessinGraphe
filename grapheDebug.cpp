#include "graphe.hpp"
#include <iostream>

void Graphe::afficherLiens(std::string nom) {
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

void Graphe::afficherLiensEmp(std::string nom) {
    std::cout << "-----------------------------------------------" << std::endl;
    std::cout << "Affichage DEBUG Aretes: " << nomGraphe << " " << nom << std::endl;
    std::cout << "Nb Aretes: " << _liens.size() << std::endl;
    for (int i = 0; i < _liens.size(); i++) {
        std::cout << "id: " << _liens[i]._id << " idemp1: " << _liens[i].getNoeud1()->getEmplacement()->_id << " idemp2: " << _liens[i].getNoeud2()->getEmplacement()->_id;
        std::cout << std::endl;
    }
    std::cout << "-----------------------------------------------" << std::endl;
}

void Graphe::afficherNoeuds(std::string nom) {
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

void Graphe::afficherEmplacement(std::string nom) {
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

void Graphe::afficherNoeudSeul(bool display, std::string nom) {
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

void Graphe::afficherInfo(std::string nom) {
    std::cout << "-----------------------------------------------" << std::endl;
    std::cout << "Information sur le graphe: " << nomGraphe << " " << nom << std::endl;
    std::cout << "Nombre de noeud: " << _noeuds.size() << std::endl;
    std::cout << "Nombre d'emplacement: " << _emplacementsPossibles.size() << std::endl;
    std::cout << "Nombre d'aretes: " << _liens.size() << std::endl;
    std::cout << "Max Voisin: " << maxVoisin << std::endl;
    std::cout << "-----------------------------------------------" << std::endl;
}

void Graphe::afficherAreteDouble(bool display, std::string nom) {
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

void Graphe::afficherNoeudDouble(bool display, std::string nom) {
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

void Graphe::afficherEmplacementDouble(bool display, std::string nom) {
    if (display) {
        std::cout << "-----------------------------------------------" << std::endl;
        std::cout << "Affichage DEBUG Emplacement Double: " << nomGraphe << " " << nom << std::endl;
    }
    int nbEmplacementDouble = 0;
    for (int i = 0; i < _emplacementsPossibles.size(); i++) {
        for (int j = i + 1; j < _emplacementsPossibles.size(); j++) {
            if ((_emplacementsPossibles[i].getX() == _emplacementsPossibles[j].getX())&&(_emplacementsPossibles[i].getY() == _emplacementsPossibles[j].getY())) {
                std::cout << "Emplacement: " << i << " & " << j << std::endl;
                nbEmplacementDouble++;
            }
        }
    }
    if (nbEmplacementDouble == 0) {
        if (display) { std::cout << "Aucun" << std::endl; }
    }
    else {
        if (!display) { std::cout << "-----------------------------------------------" << std::endl; }
    }
    if (display) { std::cout << "-----------------------------------------------" << std::endl; }
}

void Graphe::debugDesyncNoeudEmplacement(bool display, std::string nom) {
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

void Graphe::debugScoreNoeud(bool display, std::string nom) {
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

void Graphe::debugInterArrays(bool display, std::string nom) {
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

void Graphe::debugScoreGraphe(bool display, bool useArray, std::string nom) {
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

void Graphe::debugNoeudNonPlace(bool display, std::string nom) {
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

void Graphe::debugOldCroisement(bool display, std::string nom) {
    if (display) { 
        std::cout << "-----------------------------------------------" << std::endl;
        std::cout << "Affichage DEBUG Score Methode Graphe: " << nomGraphe << " " << nom << std::endl;
    }
    bool scoreFaux = true;
    long scoreActuel = getNbCroisementConst();
    long scoreOld = getNbCroisementOldMethodConst();
    if (scoreActuel != scoreOld) {
        std::cout << "Actuel: " << scoreActuel << " old: " << scoreOld << std::endl;
        if (!display) { std::cout << "-----------------------------------------------" << std::endl; }
        scoreFaux = false;
    }
    if (scoreFaux) {
        if (display) { std::cout << "Score Correct" << std::endl; }
    }
    if (display) { std::cout << "-----------------------------------------------" << std::endl; }
}

void Graphe::debugDesyncEmplacementCell(bool display, std::string nom) {
    if (display) {
        std::cout << "-----------------------------------------------" << std::endl;
        std::cout << "Affichage DEBUG desync emplacement cellule: " << nomGraphe << " " << nom << std::endl;
    }
    long nbFail = 0;
    for (int i = 0; i < _emplacementsPossibles.size(); i++) {
        for (const int& id : _emplacementsPossibles[i].vecIdCellules) {
            if (!isInVector(grillePtr[id]->vecEmplacementId,i)) {
                std::cout << "Emplacement: " << i << " Cellule: " << id << std::endl;
                nbFail++;
            }
        }
    }
    for (int i=0;i<grillePtr.size();i++) {
        for (const int& id : grillePtr[i]->vecEmplacementId) {
            if (!isInVector(_emplacementsPossibles[id].vecIdCellules,i)) {
                std::cout << "Cellule: " << i << " Emplacement: " << id << std::endl;
                nbFail++;
            }
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

void Graphe::debugDesyncAreteCell(bool display, std::string nom) {
    if (display) {
        std::cout << "-----------------------------------------------" << std::endl;
        std::cout << "Affichage DEBUG desync emplacement cellule: " << nomGraphe << " " << nom << std::endl;
    }
    long nbFail = 0;
    for (int i = 0; i < _liens.size(); i++) {
        for (const int& id : _liens[i].vecIdCellules) {
            if (!isInVector(grillePtr[id]->vecAreteId,i)) {
                std::cout << "Arete: " << i << " Cellule: " << id << std::endl;
                nbFail++;
            }
        }
    }
    for (int i=0;i<grillePtr.size();i++) {
        for (const int& id : grillePtr[i]->vecAreteId) {
            if (!isInVector(_liens[id].vecIdCellules,i)) {
                std::cout << "Cellule: " << i << " Arete: " << id << std::endl;
                nbFail++;
            }
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

void Graphe::debugEverything(bool displayOther, bool displaySelf) {
    if (displaySelf) { std::cout << "Debut debug arete double\n"; }
    afficherAreteDouble(displayOther);
    if (displaySelf) { std::cout << "Debut debug noeud double\n"; }
    afficherNoeudDouble(displayOther);
    if (_emplacementsPossibles.size() < ((_noeuds.size()*_noeuds.size())/2)) {
        if (displaySelf) { std::cout << "Debut debug emplacement double\n"; }
        afficherEmplacementDouble(displayOther);
    }
    if (displaySelf) { std::cout << "Debut debug noeud seul\n"; }
    afficherNoeudSeul(displayOther);
    if (displaySelf) { std::cout << "Debut debug noeud non place\n"; }
    debugNoeudNonPlace(displayOther);
    if (displaySelf) { std::cout << "Debut debug desync noeud emplacement\n"; }
    debugDesyncNoeudEmplacement(displayOther);
    if (displaySelf) { std::cout << "Debut debug methode intersection\n"; }
    debugOldCroisement(displayOther);
    if (isNodeScoreUpdated) {
        if (displaySelf) { std::cout << "Debut debug score noeud\n"; }
        debugScoreNoeud(displayOther);
    }
    if (isNombreCroisementUpdated) {
        if (isIntersectionVectorUpdated) {
            if (displaySelf) { std::cout << "Debut debug score graphe avec array\n"; }
            debugScoreGraphe(displayOther,true);
        }
        else {
            if (displaySelf) { std::cout << "Debut debug score graphe\n"; }
            debugScoreGraphe(displayOther);
        }
    }
    if (isIntersectionVectorUpdated) {
        if (displaySelf) { std::cout << "Debut debug inter array\n"; }
        debugInterArrays(displayOther);
    }
    if (grille.size() > 0) {
        if (displaySelf) { std::cout << "Debut debug desync emplacement cellule\n"; }
        debugDesyncEmplacementCell(displayOther);
        if (displaySelf) { std::cout << "Debut debug desync arete cellule\n"; }
        debugDesyncAreteCell(displayOther);
    }
    if (displaySelf) { std::cout << "Debug fini\n"; }
}