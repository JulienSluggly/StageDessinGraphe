#include "graphe.hpp"
#include "intersection.hpp"
#include "geometrie.hpp"
#include "triangulation.h"
#include <iostream>
#include <algorithm>

Graphe::Graphe(std::string nom) {
    nomGraphe = nom;
}

// Enleve tout les noeuds de leur emplacement.
void Graphe::clearNodeEmplacement() {
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

// Indique si le graphe contient une intersection illegale.
bool Graphe::hasIllegalCrossing() {
    for (int i = 0; i < _liens.size() - 1; ++i) {
        for (int j = i + 1; j < _liens.size(); ++j) {
            //Aretes aretes1 = _liens[i], aretes2 = _liens[j];
            if (!(_liens[i].contains(_liens[j].getNoeud1()) || _liens[i].contains(_liens[j].getNoeud2()))) {
                bool isIllegal = false;
                if (seCroisent(_liens[i], _liens[j],isIllegal)) {
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

// Indique s'il reste au moin un emplacement disponible
bool Graphe::emplacementRestant() {
    for (int i=0;i<_emplacementsPossibles.size();i++) {
        if (_emplacementsPossibles[i].estDisponible()) {
            return true;
        }
    }
    return false;
}

// Indique si tout les noeuds d'un graphe sont place
bool Graphe::estPlace() {
    for (int i=0;i<_noeuds.size();i++) {
        if (!_noeuds[i].estPlace()) return false;
    }
    return true;
}

// Renvoie le nombre de noeud non associé à un emplacement.
int Graphe::nbAPlace() {
    int nb = 0;
    for (int i=0; i<_noeuds.size();i++) {
        if (!_noeuds[i].estPlace()) nb++;
    }
    return nb;
}

// Ajoute (n-1)*_emplacements.size emplacements dans le graphe si possible
void Graphe::generateMoreEmplacement(int n) {
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
            _emplacementsPossibles.push_back(Emplacement(x,y,_emplacementsPossibles.size()));
        }
    }
}

// Creer la grille d'emplacement de taille gridHeight*gridWidth
void Graphe::generateGrid(int gridW, int gridH) {
    clearNodeEmplacement();
    _emplacementsPossibles.clear();
    if (gridW == -1 || gridH == -1) {
        gridW = _noeuds.size()*2;
        gridH = gridW;
    }
    for (int i=0;i<gridH;i++) {
        for (int j=0;j<gridW;j++) {
            _emplacementsPossibles.push_back(Emplacement(i,j,_emplacementsPossibles.size()));
        }
    }
    gridWidth = gridW;
    gridHeight = gridH;
}

// Ajoute _noeud.size() emplacement par défaut ou n emplacement 
// Si le nombre d'emplacement est laissé à -1, on génere dans une grille de taille _noeud.size()*_noeud.size()
void Graphe::generateEmplacements(int n) {
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
        _emplacementsPossibles.push_back(Emplacement(x,y,_emplacementsPossibles.size()));
    }

}

// Sauvegarde dans un vecteur les id des emplacements auquels les noeuds sont assignés
// Les noeuds doivent etre place
std::vector<int> Graphe::saveCopy() {
    std::vector<int> vectorId;
    for (int i = 0; i < _noeuds.size(); i++) {
        vectorId.push_back(_noeuds[i].getEmplacement()->getId());
    }
    return vectorId;
}

// Charge une copie en copiant les id dans le vecteur et en les affectant aux noeuds du graphe.
// La copie doit provenir d'un graphe genere du meme fichier de depart
void Graphe::loadCopy(std::vector<int> vectorId) {
    for (int i = 0; i < _noeuds.size(); i++) {
        _noeuds[i].clearEmplacement();
    }
    for (int i = 0; i < _noeuds.size(); i++) {
        _noeuds[i].setEmplacement(&_emplacementsPossibles[vectorId[i]]);
    }
}

// Vide tout les vecteurs du graphe
void Graphe::clearGraphe() {
    _noeuds.clear();
    _emplacementsPossibles.clear();
    _liens.clear();
    isNodeScoreUpdated = false;
    isNombreCroisementUpdated = false;
    isIntersectionVectorUpdated = false;
}

// Vide les vecteurs du graphe et effectue une copie du contenu des vecteur du graphe en parametre
void Graphe::copyFromGraphe(Graphe& graphe) {
    _emplacementsPossibles.clear();
    _noeuds.clear();
    _liens.clear();
    for (int i = 0; i < graphe._emplacementsPossibles.size(); ++i) {
        _emplacementsPossibles.push_back(Emplacement(graphe._emplacementsPossibles[i].getX(),graphe._emplacementsPossibles[i].getY(),i));
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
    gridHeight = graphe.gridHeight;
    gridWidth = graphe.gridWidth;
}

// Nombre de noeuds du même ID placé aux mêmes emplacements.
int Graphe::nbNoeudEnCommun(const Graphe& graphe) {
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
std::vector<int> Graphe::noeudEnCommun(Graphe &G) {
    std::vector<int> commonNodeVec;
    for (int i=0; i<_noeuds.size();i++) {
        if (_noeuds[i].getEmplacement()->_id == G._noeuds[i].getEmplacement()->_id) {
            commonNodeVec.push_back(i);
        }
    }
    return commonNodeVec;
}

// Modifie le vecteur de noeuds en commun et le vecteur de noeud non commun en parametre
void Graphe::separateNodesInCommon(Graphe &G, std::vector<int>& commonNodeVec, std::vector<int>& otherNodeVec, std::vector<int>& indexNodeInOtherVec) {
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

// Renvoie le nombre de noeud non place
int Graphe::nbNoeudNonPlace() {
    int total = _noeuds.size();
    for (int i=0;i<_noeuds.size();i++) {
        if (_noeuds[i].getEmplacement() != nullptr) {
            total--;
        }
    }
    return total;
}

// Vide les sets contenant les intersections, utile uniquement pour l'affichage openGL
void Graphe::clearSetAreteInter() {
    areteInter.clear();
    areteIll.clear();
    areteIllSelf.clear();
}

// Indique si les deux noeuds ont une aretes commune
bool Graphe::areVoisin(int nodeId1, int nodeId2) {
    for (const int& nodeAreteId : _noeuds[nodeId1]._aretes) {
        if (_liens[nodeAreteId].contains(nodeId2))
            return true;
    }
    return false;
}

// Retourne l'id de l'arete qui relie deux points en parametre, -1 si aucune est trouvee
// Utilise la liste d'arete stockee dans les nodes
int Graphe::getAreteFromTwoNodes(int nodeId1, int nodeId2) {
    for (const int& nodeAreteId : _noeuds[nodeId1]._aretes) {
        if (_liens[nodeAreteId].contains(nodeId2))
            return nodeAreteId;
    }
    return -1;
}

void Graphe::triangulationDelaunay() {
    std::cout << "Debut triangulation.\n";
    std::vector<Emplacement*> empPtrVec;
    for (int i=0;i<_emplacementsPossibles.size();i++) {
        empPtrVec.push_back(&_emplacementsPossibles[i]);
    }
    std::sort(empPtrVec.begin(), empPtrVec.end(), comparePtrEmplacementTri);
    triangulation(empPtrVec,_c);
    delaunay(_c);
    for (int i=0;i<_c.nbDemiCote();i+=2) {
        Emplacement* e1 = _c.demiCote(i)->sommet();
        Emplacement* e2 = _c.demiCote(i)->oppose()->sommet();
        e1->voisinsDelaunay.push_back(e2->_id);
        e2->voisinsDelaunay.push_back(e1->_id);
    }
    isCarteSetUp = true;
    std::cout << "Triangulation delaunay fini.\n";
}