#include "graphe.hpp"
#include "intersection.hpp"
#include "geometrie.hpp"
#include "triangulation.h"
#include <iostream>
#include <algorithm>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

double epsilon = 0.0001;

Graphe::Graphe() {
    initGraphData();
}

Graphe::~Graphe() {
    if (commonNodeEdges != nullptr) { delete commonNodeEdges; }
}
 
Graphe::Graphe(std::string nom) {
    nomGraphe = nom;
    initGraphData();
}

void Graphe::initGraphData() {
    mutexNoeud = new std::mutex();
    mutexAretes = new std::mutex();
    mutexEmplacements = new std::mutex();
}

bool compareGraphePtr(Graphe* g1, Graphe* g2) {
    long n1, n2;
    if (g1->useCoordReel) {
        if (g1->isNombreCroisementUpdated) { n1 = g1->nombreCroisement; } else { n1 = g1->getNbCroisementReelConst(); }
        if (g2->isNombreCroisementUpdated) { n2 = g2->nombreCroisement; } else { n2 = g2->getNbCroisementReelConst(); }
    }
    else {
        if (g1->isNombreCroisementUpdated) { n1 = g1->nombreCroisement; } else { n1 = g1->getNbCroisementConst(); }
        if (g2->isNombreCroisementUpdated) { n2 = g2->nombreCroisement; } else { n2 = g2->getNbCroisementConst(); }
    }
    return (n1 < n2);
}

// Enleve tout les noeuds de leur emplacement.
void Graphe::clearNodeEmplacement() {
    for (int i = 0; i < _noeuds.size(); i++) {
        _noeuds[i].clearEmplacement();
        _noeuds[i].score = -1;
    }
    for (int i = 0;i < _aretes.size();i++) {
        _aretes[i].clearIntersectionsVector();
        _aretes[i].vecIdCellules.clear();
    }
    nombreCroisement = -1;
    isNombreCroisementUpdated = false;
    isNodeScoreUpdated = false;
    isIntersectionVectorUpdated = false;
}

// Indique si le graphe contient une intersection illegale.
bool Graphe::hasIllegalCrossing() {
    if(useCoordReel) {
        for (int i = 0; i < _aretes.size() - 1; ++i) {
            for (int j = i + 1; j < _aretes.size(); ++j) {
                if (!(_aretes[i].contains(_aretes[j].getNoeud1()) || _aretes[i].contains(_aretes[j].getNoeud2()))) {
                    bool isIllegal = false;
                    if (seCroisentReel(_aretes[i], _aretes[j],isIllegal)) {
                        if (surSegmentReel(_aretes[i], *_aretes[j].getNoeud1()) || surSegmentReel(_aretes[i], *_aretes[j].getNoeud2())) {
                            return true;
                        }
                        else if (surSegmentReel(_aretes[j], *_aretes[i].getNoeud1()) || surSegmentReel(_aretes[j], *_aretes[i].getNoeud2())) {
                            return true;
                        }
                    }
                }
                else {
                    Noeud* nodeNotInCommon = _aretes[j].nodeNotInCommon(&_aretes[i]);
                    if (surSegmentReel(_aretes[i], *nodeNotInCommon)) {
                        return true;
                    }
                    else {
                        nodeNotInCommon = _aretes[i].nodeNotInCommon(&_aretes[j]);
                        if (surSegmentReel(_aretes[j], *nodeNotInCommon)) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    else {
        for (int i = 0; i < _aretes.size() - 1; ++i) {
            for (int j = i + 1; j < _aretes.size(); ++j) {
                if (!(_aretes[i].contains(_aretes[j].getNoeud1()) || _aretes[i].contains(_aretes[j].getNoeud2()))) {
                    bool isIllegal = false;
                    if (seCroisent(_aretes[i], _aretes[j],isIllegal)) {
                        if (surSegment(_aretes[i], *_aretes[j].getNoeud1()) || surSegment(_aretes[i], *_aretes[j].getNoeud2())) {
                            return true;
                        }
                        else if (surSegment(_aretes[j], *_aretes[i].getNoeud1()) || surSegment(_aretes[j], *_aretes[i].getNoeud2())) {
                            return true;
                        }
                    }
                }
                else {
                    Noeud* nodeNotInCommon = _aretes[j].nodeNotInCommon(&_aretes[i]);
                    if (surSegment(_aretes[i], *nodeNotInCommon)) {
                        return true;
                    }
                    else {
                        nodeNotInCommon = _aretes[i].nodeNotInCommon(&_aretes[j]);
                        if (surSegment(_aretes[j], *nodeNotInCommon)) {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

// Indique s'il reste au moin un emplacement disponible
bool Graphe::emplacementRestant() {
    for (int i=0;i<_emplacements.size();i++) {
        if (_emplacements[i].estDisponible()) {
            return true;
        }
    }
    return false;
}

// Indique si tout les noeuds d'un graphe sont place
bool Graphe::estPlace() {
    if (useCoordReel) {
        for (int i=0;i<_noeuds.size();i++) {
            if ((_noeuds[i]._xreel == -12345)||(_noeuds[i]._yreel == -12345)) return false;
        }
    }
    else {
        for (int i=0;i<_noeuds.size();i++) {
            if (!_noeuds[i].estPlace()) return false;
        }
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
    if (n * _emplacements.size() > nbTotal) {
        #if defined(DEBUG_GRAPHE)
            tcout() << "Pas assez de place dans la grille. Grille: " << nbTotal << " " << n << " * emp: " << n * _emplacements.size() << std::endl;
        #endif
    }
    else {
        int nbAjout = (n - 1) * _emplacements.size();
        std::vector<std::vector<bool>> marque;
        for (int i = 0; i <= gridHeight; i++) {
            std::vector<bool> tmpVec(gridWidth + 1);
            marque.push_back(tmpVec);
        }
        for (int i = 0; i < _emplacements.size(); i++) {
            int x = _emplacements[i].getX();
            int y = _emplacements[i].getY();
            marque[y][x] = true;
        }
        int x, y;
        for (int i = 0; i < nbAjout; i++) {
            do {
                x = generateRand(gridWidth);
                y = generateRand(gridHeight);
            } while (marque[y][x]);
            marque[y][x] = true;
            _emplacements.push_back(Emplacement(x,y,_emplacements.size()));
        }
    }
}

// Creer la grille d'emplacement de taille gridHeight*gridWidth
void Graphe::generateGrid(int gridW, int gridH) {
    clearNodeEmplacement();
    _emplacements.clear();
    if (gridW == -1 || gridH == -1) {
        gridW = _noeuds.size()*2;
        gridH = gridW;
    }
    for (int i=0;i<gridH;i++) {
        for (int j=0;j<gridW;j++) {
            _emplacements.push_back(Emplacement(i,j,_emplacements.size()));
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
        gridWidth = std::min((int)_noeuds.size()*2,6000);
        gridHeight = gridWidth;
        nbEmplacement = _noeuds.size();
    }
    nbEmplacement = std::min(nbEmplacement,(gridWidth*gridHeight)-(int)_emplacements.size());
    std::vector<std::vector<bool>> marque;
    for (int i = 0; i <= gridHeight; i++) {
        std::vector<bool> tmpVec(gridWidth + 1);
        marque.push_back(tmpVec);
    }
    for (int i = 0; i < _emplacements.size(); i++) {
        int x = _emplacements[i]._x;
        int y = _emplacements[i]._y;
        marque[y][x] = true;
    }
    int x, y;
    for (int i = 0; i < nbEmplacement; i++) {
        do {
            x = generateRand(gridWidth);
            y = generateRand(gridHeight);
        } while (marque[y][x]);
        marque[y][x] = true;
        _emplacements.push_back(Emplacement(x,y,_emplacements.size()));
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
        _noeuds[i].setEmplacement(&_emplacements[vectorId[i]]);
    }
}

// Vide tout les vecteurs du graphe
void Graphe::clearGraphe() {
    _noeuds.clear();
    _emplacements.clear();
    _aretes.clear();
    grille.clear();
    isNodeScoreUpdated = false;
    isNombreCroisementUpdated = false;
    isIntersectionVectorUpdated = false;
    isCarteSetUp = false;
    nombreCroisement = -1;
    maxVoisin = -1;
}

// Vide les vecteurs du graphe et effectue une copie du contenu des vecteur du graphe en parametre
void Graphe::copyFromGraphe(Graphe& graphe) {
    _emplacements.clear();
    _noeuds.clear();
    _aretes.clear();
    _emplacements.reserve(graphe._emplacements.size()*2);
    _noeuds.reserve(graphe._noeuds.size()*2);
    _aretes.reserve(graphe._aretes.size()*2);
    for (int i = 0; i < graphe._emplacements.size(); ++i) {
        _emplacements.push_back(Emplacement(graphe._emplacements[i].getX(),graphe._emplacements[i].getY(),i));
    }
    for (int i = 0; i < graphe._noeuds.size(); ++i) {
        _noeuds.push_back(Noeud(i));
    }
    for (int i = 0; i < graphe._noeuds.size();i++) {
        _noeuds[i].score = graphe._noeuds[i].score;
    }

    for (int i = 0; i < graphe._aretes.size(); ++i) {
        int id1 = graphe._aretes[i].getNoeud1()->getId();
        int id2 = graphe._aretes[i].getNoeud2()->getId();
        _aretes.push_back(Aretes(&_noeuds[id1], &_noeuds[id2], i));
    }
    for (int i = 0; i < graphe._aretes.size(); ++i) {
        _aretes[i].intersections = graphe._aretes[i].intersections;
        _aretes[i].intersectionsIll = graphe._aretes[i].intersectionsIll;
        _aretes[i].intersectionsIllSelf = graphe._aretes[i].intersectionsIllSelf;
        _aretes[i].isUpdated = graphe._aretes[i].isUpdated;
    }
    for (int i = 0; i < graphe._noeuds.size(); ++i) {
        if (graphe._noeuds[i].getEmplacement() != nullptr) {
            int idEmplacement = graphe._noeuds[i].getEmplacement()->getId();
            _noeuds[i].setEmplacement(&_emplacements[idEmplacement]);
        }
        _noeuds[i]._xreel = graphe._noeuds[i]._xreel;
        _noeuds[i]._yreel = graphe._noeuds[i]._yreel;
    }
    nombreCroisement = graphe.nombreCroisement;
    isNombreCroisementUpdated = graphe.isNombreCroisementUpdated;
    isNodeScoreUpdated = graphe.isNodeScoreUpdated;
    isIntersectionVectorUpdated = graphe.isIntersectionVectorUpdated;
    maxVoisin = graphe.maxVoisin;
    gridHeight = graphe.gridHeight;
    gridWidth = graphe.gridWidth;
}

void Graphe::copyNodesFromGraphe(Graphe& graphe) {
    clearNodeEmplacement();
    for (int i = 0; i < graphe._noeuds.size();i++) {
        _noeuds[i].score = graphe._noeuds[i].score;
    }
    for (int i = 0; i < graphe._noeuds.size(); ++i) {
        if (graphe._noeuds[i].getEmplacement() != nullptr) {
            int idEmplacement = graphe._noeuds[i].getEmplacement()->getId();
            _noeuds[i].setEmplacement(&_emplacements[idEmplacement]);
        }
    }
    nombreCroisement = graphe.nombreCroisement;
    isNombreCroisementUpdated = graphe.isNombreCroisementUpdated;
    isNodeScoreUpdated = graphe.isNodeScoreUpdated;
}

void Graphe::copyGrilleFromGraphe(Graphe& graphe) {
    clearGrille();
    for (int i=0;i<graphe.grillePtr.size();i++) {
        grillePtr[i]->vecAreteId = graphe.grillePtr[i]->vecAreteId;
    }
    for (int i=0;i<graphe._aretes.size();i++) {
        _aretes[i].vecIdCellules = graphe._aretes[i].vecIdCellules;
    }
}

void Graphe::copyFromGrapheGenetique(Graphe& graphe) {
    _emplacements.clear();
    _noeuds.clear();
    _aretes.clear();
    _emplacements.reserve(graphe._emplacements.size()*2);
    _noeuds.reserve(graphe._noeuds.size()*2);
    _aretes.reserve(graphe._aretes.size()*2);
    for (int i = 0; i < graphe._emplacements.size(); ++i) {
        _emplacements.push_back(Emplacement(graphe._emplacements[i].getX(),graphe._emplacements[i].getY(),i));
    }
    for (int i = 0; i < graphe._noeuds.size(); ++i) {
        _noeuds.push_back(Noeud(i));
    }
    for (int i = 0; i < graphe._aretes.size(); ++i) {
        int id1 = graphe._aretes[i].getNoeud1()->getId();
        int id2 = graphe._aretes[i].getNoeud2()->getId();
        _aretes.push_back(Aretes(&_noeuds[id1], &_noeuds[id2], i));
    }
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
        if (_aretes[nodeAreteId].contains(nodeId2))
            return true;
    }
    return false;
}

// Retourne l'id de l'arete qui relie deux points en parametre, -1 si aucune est trouvee
// Utilise la liste d'arete stockee dans les nodes
int Graphe::getAreteFromTwoNodes(int nodeId1, int nodeId2) {
    for (const int& nodeAreteId : _noeuds[nodeId1]._aretes) {
        if (_aretes[nodeAreteId].contains(nodeId2))
            return nodeAreteId;
    }
    return -1;
}

void Graphe::triangulationDelaunay() {
    #if defined(DEBUG_GRAPHE)
        tcout() << "Debut triangulation.\n";
    #endif
    std::vector<Emplacement*> empPtrVec;
    for (int i=0;i<_emplacements.size();i++) {
        empPtrVec.push_back(&_emplacements[i]);
        if (_emplacements[i].voisinsDelaunay == nullptr) {
            _emplacements[i].voisinsDelaunay = new std::vector<int>();
        }
        else {
            _emplacements[i].voisinsDelaunay->clear();
        }
    }
    std::sort(empPtrVec.begin(), empPtrVec.end(), comparePtrEmplacementTri);
    triangulation(empPtrVec,_c);
    delaunay(_c);
    for (int i=0;i<_c.nbDemiCote();i+=2) {
        Emplacement* e1 = _c.demiCote(i)->sommet();
        Emplacement* e2 = _c.demiCote(i)->oppose()->sommet();
        e1->voisinsDelaunay->push_back(e2->_id);
        e2->voisinsDelaunay->push_back(e1->_id);
    }
    isCarteSetUp = true;
    #if defined(DEBUG_GRAPHE)
        tcout() << "Triangulation delaunay fini.\n";
    #endif
}

void Graphe::reinitGrille() {
    clearGrille();
    registerEdgesInGrid();
}

void Graphe::reinitGrilleReel() {
    clearGrilleReel();
    registerNodesAndEdgesInGrid();
}

void Graphe::clearGrille() {
    for (int i=0;i<grillePtr.size();i++) {
        grillePtr[i]->vecAreteId.clear();
        grillePtr[i]->containAreteId.assign(grillePtr[i]->containAreteId.size(), -1);
    }
    for (int i=0;i<_aretes.size();i++) {
        _aretes[i].vecIdCellules.clear();
    }
}

void Graphe::clearGrilleReel() {
    for (int i=0;i<grillePtr.size();i++) {
        grillePtr[i]->vecAreteId.clear();
        grillePtr[i]->containAreteId.assign(grillePtr[i]->containAreteId.size(), -1);
    }
    for (int i=0;i<_aretes.size();i++) {
        _aretes[i].vecIdCellules.clear();
    }
    for (int i=0;i<_noeuds.size();i++) {
        _noeuds[i].idCelluleVec->clear();
    }
}

void Graphe::deleteGrille() {
    grillePtr.clear();
    if (grille.size() > 0) {
        for (int i=0;i<grille[0].size();i++) {
            grille[i].clear();
        }
    }
    grille.clear();
    for (int i=0;i<_emplacements.size();i++) {
        if (_emplacements[i].idCelluleVec != nullptr) {
            delete _emplacements[i].idCelluleVec;
            _emplacements[i].idCelluleVec = nullptr;
        }
        _emplacements[i].idCellule = -1;
    }
    for (int i=0;i<_aretes.size();i++) {
        _aretes[i].vecIdCellules.clear();
    }
    for (int i=0;i<_noeuds.size();i++) {
        if (_noeuds[i].idCelluleVec != nullptr) { 
            _noeuds[i].idCelluleVec->clear();
            delete _noeuds[i].idCelluleVec;
            _noeuds[i].idCelluleVec = nullptr;
        }
    }
    if (grille_with_move) {
        for (int i=0;i<_emplacements.size();i++) {
            _emplacements[i]._x = (_emplacements[i]._x-2)/2;
            _emplacements[i]._y = (_emplacements[i]._y-2)/2;
        }
        gridWidth = (gridWidth-2)/2;
        gridHeight = (gridHeight-2)/2;
    }
    grille_with_move = false;
}

void Graphe::initGrille(int row,int column) {
    deleteGrille();
    grille_with_move = true;
    for (int i=0;i<_emplacements.size();i++) {
        _emplacements[i]._x = (_emplacements[i]._x*2)+2;
        _emplacements[i]._y = (_emplacements[i]._y*2)+2;
    }
    gridWidth = (gridWidth * 2) + 2;
    gridHeight = (gridHeight * 2) + 2;
    if (row == -1) { row = (int)ceil(sqrt(_aretes.size())*1.5); }
    if (column == -1) { column = row; }
    int sizeColumn = ceil((double)gridWidth / (double)column);
    if (sizeColumn % 2 == 1) { sizeColumn++; }
    int sizeRow = ceil((double)gridHeight / (double)row);
    if (sizeRow % 2 == 1) { sizeRow++; }
    for (int y=0;y<row;y++) {
        std::vector<Cellule> tmpVec;
        for (int x=0;x<column;x++) {
            int id = y*column + x;
            int x1 = x*sizeColumn + 1;
            int x2 = x1+sizeColumn;
            int y1 = y*sizeRow + 1;
            int y2 = y1+sizeRow;
            tmpVec.push_back(Cellule(id,x,y,x1,y2,x2,y1,_aretes.size(),_noeuds.size()));
        }
        grille.push_back(tmpVec);
    }

    for (int i=0;i<grille.size();i++) {
        for (int j=0;j<grille[0].size();j++) {
            grillePtr.push_back(&grille[i][j]);
        }
    }

}

void Graphe::initGrilleNoMove(int row,int column) {
    deleteGrille();
    if (row == -1) { row = (int)ceil(sqrt(_aretes.size())*1.5); }
    if (column == -1) { column = row; }
    int sizeColumn = ceil((double)gridWidth / (double)column);
    int sizeRow = ceil((double)gridHeight / (double)row);
    for (int y=0;y<row;y++) {
        std::vector<Cellule> tmpVec;
        for (int x=0;x<column;x++) {
            int id = y*column + x;
            int x1 = x*sizeColumn;
            int x2 = x1+sizeColumn;
            int y1 = y*sizeRow;
            int y2 = y1+sizeRow;
            tmpVec.push_back(Cellule(id,x,y,x1,y2,x2,y1,_aretes.size(),_noeuds.size()));
        }
        grille.push_back(tmpVec);
    }

    for (int i=0;i<grille.size();i++) {
        for (int j=0;j<grille[0].size();j++) {
            grillePtr.push_back(&grille[i][j]);
        }
    }
}

void Graphe::initGrilleReel(int row,int column) {
    deleteGrille();
    if (row == -1) { row = (int)ceil(sqrt(_aretes.size())*1.5); }
    if (column == -1) { column = row; }
    double sizeColumn = (double)gridWidth / (double)column;
    double sizeRow = (double)gridHeight / (double)row;
    for (int y=0;y<row;y++) {
        std::vector<Cellule> tmpVec;
        for (int x=0;x<column;x++) {
            int id = y*column + x;
            double x1 = (double)x*sizeColumn;
            double x2 = (double)x1+sizeColumn;
            double y1 = (double)y*sizeRow;
            double y2 = (double)y1+sizeRow;
            tmpVec.push_back(Cellule(id,x,y,x1,y2,x2,y1,_aretes.size(),_noeuds.size()));
        }
        grille.push_back(tmpVec);
    }

    for (int i=0;i<grille.size();i++) {
        for (int j=0;j<grille[0].size();j++) {
            grillePtr.push_back(&grille[i][j]);
        }
    }
}

void Graphe::initGrilleCarre() {
    deleteGrille();
    int row = (int)ceil(sqrt(_aretes.size())*1.5);
    int column = row;
    int sizeColumn = ceil((double)gridWidth / (double)row);
    int sizeRow = ceil((double)gridHeight / (double)row);
    int sizeCell = min(sizeColumn,sizeRow);
    if (sizeCell == sizeRow) { column = ceil((double)gridWidth/(double)sizeCell); }
    else { row = ceil((double)gridHeight/(double)sizeCell); }
    for (int y=0;y<row;y++) {
        std::vector<Cellule> tmpVec;
        for (int x=0;x<column;x++) {
            int id = y*column + x;
            int x1 = x*sizeCell;
            int x2 = x1+sizeCell;
            int y1 = y*sizeCell;
            int y2 = y1+sizeCell;
            tmpVec.push_back(Cellule(id,x,y,x1,y2,x2,y1,_aretes.size(),_noeuds.size()));
        }
        grille.push_back(tmpVec);
    }
    for (int i=0;i<grille.size();i++) {
        for (int j=0;j<grille[0].size();j++) {
            grillePtr.push_back(&grille[i][j]);
        }
    }
}

// Appeller cette fonction uniquement si initGrilleNoMove ou initGrilleCarre a été appellé précédement.
void Graphe::registerSlotsInGridNoMove() {
    int sizeColumn = grille[0][0].getBottomRightX() - grille[0][0].getBottomLeftX();
    int sizeRow = grille[0][0].getTopLeftY() - grille[0][0].getBottomLeftY();
    for (int i=0;i < _emplacements.size();i++) {
        if (_emplacements[i].idCelluleVec == nullptr) {
            _emplacements[i].idCelluleVec = new std::vector<int>();
        }
        int x = _emplacements[i].getX();
        int y = _emplacements[i].getY();
        double dnumX = (double)x/(double)sizeColumn;
        double dnumY = (double)y/(double)sizeRow;
        int numX = floor(dnumX);
        int numY = floor(dnumY);

        int numX2 = floor(dnumX + epsilon);
        int numY2 = floor(dnumY + epsilon);
        if (numX2 == numX) { numX2 = floor(dnumX - epsilon); }
        if (numY2 == numY) { numY2 = floor(dnumY - epsilon); }
        int id;
        if (numX2 != numX) { // Cas alligné sur segment vertical de la grille
            if (numY2 != numY) { // Cas aligné sur intersection de la grille
                if ((numY2 >= 0)&&(numY2 < grille.size())) { // Ajout numY2 & numX
                    if (numX < grille[0].size()) {
                        id = numY2 * grille[0].size() + numX;
                        _emplacements[i].idCelluleVec->push_back(id);
                        grille[numY2][numX].vecEmplacementId.push_back(i);
                    }
                    if ((numX2 >= 0)&&(numX2 < grille[0].size())) { // Ajout numY2 et numX2
                        id = numY2 * grille[0].size() + numX2;
                        _emplacements[i].idCelluleVec->push_back(id);
                        grille[numY2][numX2].vecEmplacementId.push_back(i);
                    }
                }
            }
            if ((numX2 >= 0)&&(numX2 < grille[0].size())&&numY < grille.size()) { // Ajout numY et numX2
                id = numY * grille[0].size() + numX2;
                _emplacements[i].idCelluleVec->push_back(id);
                grille[numY][numX2].vecEmplacementId.push_back(i);
            }
        }
        else if (numY2 != numY) { // Cas juste aligné sur segment horizontal de la grille
            if ((numY2 >= 0)&&(numY2 < grille.size())&&numX < grille[0].size()) { // Ajout numY2 & numX
                id = numY2 * grille[0].size() + numX;
                _emplacements[i].idCelluleVec->push_back(id);
                grille[numY2][numX].vecEmplacementId.push_back(i);
            }
        }
        // Ajout numY & numX
        if (numY < grille.size() && numX < grille[0].size()) {
            id = numY * grille[0].size() + numX;
            _emplacements[i].idCelluleVec->push_back(id);
            grille[numY][numX].vecEmplacementId.push_back(i);
        }
    }
}

void Graphe::registerEdgesInGridNoMove() {
    int nombreColonne = grille[0].size();
    for (int i=0;i<_aretes.size();i++) {
        int n1X = _aretes[i].getNoeud1()->getX();
        int n1Y = _aretes[i].getNoeud1()->getY();
        int n2X = _aretes[i].getNoeud2()->getX();
        int n2Y = _aretes[i].getNoeud2()->getY();
        int direction = getDirectionArete(i);
        std::vector<int>* vecCellDepart = _aretes[i].getNoeud1()->getEmplacement()->idCelluleVec;
        int idCellX, idCellY;
        for (const int& tmpIdCell : *vecCellDepart) {
            _aretes[i].vecIdCellules.push_back(tmpIdCell);
        }
        int idCell = _aretes[i].getNoeud1()->getEmplacement()->idCelluleVec->at(0);
        std::vector<int>* vecCellArrive = _aretes[i].getNoeud2()->getEmplacement()->idCelluleVec;
        while (!isInVector(*vecCellArrive,idCell)) {
            switch(direction) {
                case 0:
                    idCell++;
                    break;
                case 1: {
                    double cellX = grillePtr[idCell]->getTopRightX();
                    double cellY = grillePtr[idCell]->getTopRightY();
                    int alignement = aGaucheInt(n1X,n1Y,n2X,n2Y,cellX,cellY);
                    if (alignement == 1) {
                        idCell++;
                    }
                    else if (alignement == -1) {
                        idCell += nombreColonne;
                    }
                    else {
                        _aretes[i].vecIdCellules.push_back(idCell+nombreColonne);
                        _aretes[i].vecIdCellules.push_back(idCell+1);
                        idCell = idCell + nombreColonne + 1;
                    }
                    break;
                }
                case 2:
                    idCell += nombreColonne;
                    break;
                case 3: {
                    double cellX = grillePtr[idCell]->getTopLeftX();
                    double cellY = grillePtr[idCell]->getTopLeftY();
                    int alignement = aGaucheInt(n1X,n1Y,n2X,n2Y,cellX,cellY);
                    if (alignement == 1) {
                        idCell += nombreColonne;
                    }
                    else if (alignement == -1) {
                        idCell--;
                    }
                    else {
                        _aretes[i].vecIdCellules.push_back(idCell+nombreColonne);
                        _aretes[i].vecIdCellules.push_back(idCell-1);
                        idCell = idCell + nombreColonne - 1;

                    }
                    break;
                }
                case 4:
                    idCell--;
                    break;
                case 5: {
                    double cellX = grillePtr[idCell]->getBottomLeftX();
                    double cellY = grillePtr[idCell]->getBottomLeftY();
                    int alignement = aGaucheInt(n1X,n1Y,n2X,n2Y,cellX,cellY);
                    if (alignement == 1) {
                        idCell--;
                    }
                    else if (alignement == -1) {
                        idCell -= nombreColonne;
                    }
                    else {
                        _aretes[i].vecIdCellules.push_back(idCell-nombreColonne);
                        _aretes[i].vecIdCellules.push_back(idCell-1);
                        idCell = idCell - nombreColonne - 1;
                    }
                    break;
                }
                case 6:
                    idCell -= grille[0].size();
                    break;
                case 7: {
                    double cellX = grillePtr[idCell]->getBottomRightX();
                    double cellY = grillePtr[idCell]->getBottomRightY();
                    int alignement = aGaucheInt(n1X,n1Y,n2X,n2Y,cellX,cellY);
                    if (alignement == 1) {
                        idCell -= nombreColonne;
                    }
                    else if (alignement == -1) {
                        idCell++;
                    }
                    else {
                        _aretes[i].vecIdCellules.push_back(idCell-nombreColonne);
                        _aretes[i].vecIdCellules.push_back(idCell+1);
                        idCell = idCell - nombreColonne + 1;
                    }
                    break;
                }
            }
            _aretes[i].vecIdCellules.push_back(idCell);
        }
        for (const int& tmpIdCell : *vecCellArrive) {
            _aretes[i].vecIdCellules.push_back(tmpIdCell);
        }
        std::unordered_set<int> tmpUSet(_aretes[i].vecIdCellules.begin(), _aretes[i].vecIdCellules.end());
        _aretes[i].vecIdCellules.assign(tmpUSet.begin(), tmpUSet.end());
        for (const int& idCellule : _aretes[i].vecIdCellules) {
            grillePtr[idCellule]->containAreteId[i] = grillePtr[idCellule]->vecAreteId.size();
            grillePtr[idCellule]->vecAreteId.push_back(i);
        }
    }
}

void Graphe::registerSlotsInGrid() {
    int sizeColumn = grille[0][0].getBottomRightX() - grille[0][0].getBottomLeftX();
    int sizeRow = grille[0][0].getTopLeftY() - grille[0][0].getBottomLeftY();
    for (int i=0;i < _emplacements.size();i++) {
        int x = _emplacements[i].getX()-1;
        int y = _emplacements[i].getY()-1;
        double dnumX = (double)x/(double)sizeColumn;
        double dnumY = (double)y/(double)sizeRow;
        int numX = floor(dnumX);
        int numY = floor(dnumY);
        int id = numY * grille[0].size() + numX;
        _emplacements[i].idCellule = id;
        grille[numY][numX].vecEmplacementId.push_back(i);
        if (_emplacements[i]._noeud != nullptr) {
            if (_emplacements[i]._noeud->estIsole()) {
                grille[numY][numX].vecSingleNodeId.push_back(_emplacements[i]._noeud->_id);
                grille[numY][numX].containSingleNodeId[_emplacements[i]._noeud->_id] = grille[numY][numX].vecSingleNodeId.size() - 1;
            }
        }
    }
}

void Graphe::registerSlotsAndEdgesInGrid() {
    #if defined(DEBUG_GRAPHE_PROGRESS)
        tcout() << "Remplissage des cellules.\n";
    #endif
    registerSlotsInGrid();
    registerEdgesInGrid();
}

void Graphe::registerEdgesInGrid() {
    int nombreColonne = grille[0].size();
    for (int i=0;i<_aretes.size();i++) {
        if (_aretes[i].estPlace()) {
            int n1X = _aretes[i].getNoeud1()->getX();
            int n1Y = _aretes[i].getNoeud1()->getY();
            int n2X = _aretes[i].getNoeud2()->getX();
            int n2Y = _aretes[i].getNoeud2()->getY();
            int direction = getDirectionArete(i);
            int idCell = _aretes[i].getNoeud1()->getEmplacement()->idCellule;
            int idCellArrive = _aretes[i].getNoeud2()->getEmplacement()->idCellule;
            _aretes[i].vecIdCellules.push_back(idCell);
            while (idCell != idCellArrive) {
                switch(direction) {
                    case 0:
                        idCell++;
                        break;
                    case 1: {
                        int cellX = grillePtr[idCell]->getTopRightX();
                        int cellY = grillePtr[idCell]->getTopRightY();
                        int alignement = aGaucheInt(n1X,n1Y,n2X,n2Y,cellX,cellY);
                        if (alignement == 1) {
                            idCell++;
                        }
                        else if (alignement == -1) {
                            idCell += nombreColonne;
                        }
                        else {
                            _aretes[i].vecIdCellules.push_back(idCell+nombreColonne);
                            _aretes[i].vecIdCellules.push_back(idCell+1);
                            idCell = idCell + nombreColonne + 1;
                        }
                        break;
                    }
                    case 2:
                        idCell += nombreColonne;
                        break;
                    case 3: {
                        int cellX = grillePtr[idCell]->getTopLeftX();
                        int cellY = grillePtr[idCell]->getTopLeftY();
                        int alignement = aGaucheInt(n1X,n1Y,n2X,n2Y,cellX,cellY);
                        if (alignement == 1) {
                            idCell += nombreColonne;
                        }
                        else if (alignement == -1) {
                            idCell--;
                        }
                        else {
                            _aretes[i].vecIdCellules.push_back(idCell+nombreColonne);
                            _aretes[i].vecIdCellules.push_back(idCell-1);
                            idCell = idCell + nombreColonne - 1;
                        }
                        break;
                    }
                    case 4:
                        idCell--;
                        break;
                    case 5: {
                        int cellX = grillePtr[idCell]->getBottomLeftX();
                        int cellY = grillePtr[idCell]->getBottomLeftY();
                        int alignement = aGaucheInt(n1X,n1Y,n2X,n2Y,cellX,cellY);
                        if (alignement == 1) {
                            idCell--;
                        }
                        else if (alignement == -1) {
                            idCell -= nombreColonne;
                        }
                        else {
                            _aretes[i].vecIdCellules.push_back(idCell-nombreColonne);
                            _aretes[i].vecIdCellules.push_back(idCell-1);
                            idCell = idCell - nombreColonne - 1;
                        }
                        break;
                    }
                    case 6:
                        idCell -= grille[0].size();
                        break;
                    case 7: {
                        int cellX = grillePtr[idCell]->getBottomRightX();
                        int cellY = grillePtr[idCell]->getBottomRightY();
                        int alignement = aGaucheInt(n1X,n1Y,n2X,n2Y,cellX,cellY);
                        if (alignement == 1) {
                            idCell -= nombreColonne;
                        }
                        else if (alignement == -1) {
                            idCell++;
                        }
                        else {
                            _aretes[i].vecIdCellules.push_back(idCell-nombreColonne);
                            _aretes[i].vecIdCellules.push_back(idCell+1);
                            idCell = idCell - nombreColonne + 1;
                        }
                        break;
                    }
                }
                _aretes[i].vecIdCellules.push_back(idCell);
            }
            for (const int& idCellule : _aretes[i].vecIdCellules) {
                grillePtr[idCellule]->containAreteId[i] = grillePtr[idCellule]->vecAreteId.size();
                grillePtr[idCellule]->vecAreteId.push_back(i);
            }
        }
    }
}

void Graphe::registerNodesAndEdgesInGrid() {
    registerNodesInGrid();
    registerEdgesInGridReel();
}

void Graphe::registerIsolatedNodesInGrid() {
    for (int i=0;i < _noeudsSeuls.size();i++) {
        int nodeId = _noeudsSeuls[i];
        if (_noeuds[nodeId].idCelluleVec == nullptr) {
            _noeuds[nodeId].idCelluleVec = new std::vector<int>();
        }
        if (_noeuds[nodeId].getEmplacement()->idCelluleVec != nullptr) {
            *_noeuds[nodeId].idCelluleVec = *_noeuds[nodeId].getEmplacement()->idCelluleVec;
        }
        else {
            _noeuds[nodeId].idCelluleVec->push_back(_noeuds[nodeId].getEmplacement()->idCellule);
        }
        for (const int cellId : (*_noeuds[nodeId].idCelluleVec)) {
            int numero = grillePtr[cellId]->vecSingleNodeId.size();
            grillePtr[cellId]->vecSingleNodeId.push_back(nodeId);
            grillePtr[cellId]->containSingleNodeId[nodeId] = numero;
        }
    }
}

void Graphe::registerNodesInGrid() {
    double sizeColumn = grille[0][0].getBottomRightXReel() - grille[0][0].getBottomLeftXReel();
    double sizeRow = grille[0][0].getTopLeftYReel() - grille[0][0].getBottomLeftYReel();
    for (int i=0;i < _noeuds.size();i++) {
        if (_noeuds[i].idCelluleVec == nullptr) {
            _noeuds[i].idCelluleVec = new std::vector<int>();
        }
        double x = _noeuds[i]._xreel;
        double y = _noeuds[i]._yreel;
        double dnumX = x/sizeColumn;
        double dnumY = y/sizeRow;
        int numX = floor(dnumX);
        int numY = floor(dnumY);

        int numX2 = floor(dnumX + epsilon);
        int numY2 = floor(dnumY + epsilon);
        if (numX2 == numX) { numX2 = floor(dnumX - epsilon); }
        if (numY2 == numY) { numY2 = floor(dnumY - epsilon); }
        int id;
        if (numX2 != numX) { // Cas alligné sur segment vertical de la grille
            if (numY2 != numY) { // Cas aligné sur intersection de la grille
                if ((numY2 >= 0)&&(numY2 < grille.size())) { // Ajout numY2 & numX
                    if (numX < grille[0].size()) {
                        id = numY2 * grille[0].size() + numX;
                        _noeuds[i].idCelluleVec->push_back(id);
                    }
                    if ((numX2 >= 0)&&(numX2 < grille[0].size())) { // Ajout numY2 et numX2
                        id = numY2 * grille[0].size() + numX2;
                        _noeuds[i].idCelluleVec->push_back(id);
                    }
                }
            }
            if ((numX2 >= 0)&&(numX2 < grille[0].size())&&numY < grille.size()) { // Ajout numY et numX2
                id = numY * grille[0].size() + numX2;
                _noeuds[i].idCelluleVec->push_back(id);
            }
        }
        else if (numY2 != numY) { // Cas juste aligné sur segment horizontal de la grille
            if ((numY2 >= 0)&&(numY2 < grille.size())&&numX < grille[0].size()) { // Ajout numY2 & numX
                id = numY2 * grille[0].size() + numX;
                _noeuds[i].idCelluleVec->push_back(id);
            }
        }
        // Ajout numY & numX
        if (numY < grille.size() && numX < grille[0].size()) {
            id = numY * grille[0].size() + numX;
            _noeuds[i].idCelluleVec->push_back(id);
        }
    }
}

void Graphe::registerNodeInGrid(int i) {
    double sizeColumn = grille[0][0].getBottomRightXReel() - grille[0][0].getBottomLeftXReel();
    double sizeRow = grille[0][0].getTopLeftYReel() - grille[0][0].getBottomLeftYReel();
    if (_noeuds[i].idCelluleVec == nullptr) {
        _noeuds[i].idCelluleVec = new std::vector<int>();
    }
    double x = _noeuds[i]._xreel;
    double y = _noeuds[i]._yreel;
    double dnumX = x/sizeColumn;
    double dnumY = y/sizeRow;
    int numX = floor(dnumX);
    int numY = floor(dnumY);

    int numX2 = floor(dnumX + epsilon);
    int numY2 = floor(dnumY + epsilon);
    if (numX2 == numX) { numX2 = floor(dnumX - epsilon); }
    if (numY2 == numY) { numY2 = floor(dnumY - epsilon); }
    int id;
    if (numX2 != numX) { // Cas alligné sur segment vertical de la grille
        if (numY2 != numY) { // Cas aligné sur intersection de la grille
            if ((numY2 >= 0)&&(numY2 < grille.size())) { // Ajout numY2 & numX
                if (numX < grille[0].size()) {
                    id = numY2 * grille[0].size() + numX;
                    _noeuds[i].idCelluleVec->push_back(id);
                }
                if ((numX2 >= 0)&&(numX2 < grille[0].size())) { // Ajout numY2 et numX2
                    id = numY2 * grille[0].size() + numX2;
                    _noeuds[i].idCelluleVec->push_back(id);
                }
            }
        }
        if ((numX2 >= 0)&&(numX2 < grille[0].size())&&numY < grille.size()) { // Ajout numY et numX2
            id = numY * grille[0].size() + numX2;
            _noeuds[i].idCelluleVec->push_back(id);
        }
    }
    else if (numY2 != numY) { // Cas juste aligné sur segment horizontal de la grille
        if ((numY2 >= 0)&&(numY2 < grille.size())&&numX < grille[0].size()) { // Ajout numY2 & numX
            id = numY2 * grille[0].size() + numX;
            _noeuds[i].idCelluleVec->push_back(id);
        }
    }
    // Ajout numY & numX
    if (numY < grille.size() && numX < grille[0].size()) {
        id = numY * grille[0].size() + numX;
        _noeuds[i].idCelluleVec->push_back(id);
    }
}

void Graphe::recalcSpecificNodeCell(int nodeId) {
    double sizeColumn = grille[0][0].getBottomRightXReel() - grille[0][0].getBottomLeftXReel();
    double sizeRow = grille[0][0].getTopLeftYReel() - grille[0][0].getBottomLeftYReel();
    _noeuds[nodeId].idCelluleVec->clear();
    double x = _noeuds[nodeId]._xreel;
    double y = _noeuds[nodeId]._yreel;
    double dnumX = x/sizeColumn;
    double dnumY = y/sizeRow;
    int numX = floor(dnumX);
    int numY = floor(dnumY);

    int numX2 = floor(dnumX + epsilon);
    int numY2 = floor(dnumY + epsilon);
    if (numX2 == numX) { numX2 = floor(dnumX - epsilon); }
    if (numY2 == numY) { numY2 = floor(dnumY - epsilon); }
    int id;
    if (numX2 != numX) { // Cas alligné sur segment vertical de la grille
        if (numY2 != numY) { // Cas aligné sur intersection de la grille
            if ((numY2 >= 0)&&(numY2 < grille.size())) { // Ajout numY2 & numX
                if (numX < grille[0].size()) {
                    id = numY2 * grille[0].size() + numX;
                    _noeuds[nodeId].idCelluleVec->push_back(id);
                }
                if ((numX2 >= 0)&&(numX2 < grille[0].size())) { // Ajout numY2 et numX2
                    id = numY2 * grille[0].size() + numX2;
                    _noeuds[nodeId].idCelluleVec->push_back(id);
                }
            }
        }
        if ((numX2 >= 0)&&(numX2 < grille[0].size())&&numY < grille.size()) { // Ajout numY et numX2
            id = numY * grille[0].size() + numX2;
            _noeuds[nodeId].idCelluleVec->push_back(id);
        }
    }
    else if (numY2 != numY) { // Cas juste aligné sur segment horizontal de la grille
        if ((numY2 >= 0)&&(numY2 < grille.size())&&numX < grille[0].size()) { // Ajout numY2 & numX
            id = numY2 * grille[0].size() + numX;
            _noeuds[nodeId].idCelluleVec->push_back(id);
        }
    }
    // Ajout numY & numX
    if (numY < grille.size() && numX < grille[0].size()) {
        id = numY * grille[0].size() + numX;
        _noeuds[nodeId].idCelluleVec->push_back(id);
    }
}

void Graphe::registerEdgesInGridReel() {
    int nombreColonne = grille[0].size();
    for (int i=0;i<_aretes.size();i++) {
        double n1X = _aretes[i].getNoeud1()->_xreel;
        double n1Y = _aretes[i].getNoeud1()->_yreel;
        double n2X = _aretes[i].getNoeud2()->_xreel;
        double n2Y = _aretes[i].getNoeud2()->_yreel;
        int direction = getDirectionAreteReel(i);
        std::vector<int>* vecCellDepart = _aretes[i].getNoeud1()->idCelluleVec;
        for (const int& tmpIdCell : *vecCellDepart) {
            _aretes[i].vecIdCellules.push_back(tmpIdCell);
        }
        int idCell = _aretes[i].getNoeud1()->idCelluleVec->at(0);
        std::vector<int>* vecCellArrive = _aretes[i].getNoeud2()->idCelluleVec;
        while (!isInVector(*vecCellArrive,idCell)) {
            switch(direction) {
                case 0:
                    idCell++;
                    break;
                case 1: {
                    double cellX = grillePtr[idCell]->getTopRightXReel();
                    double cellY = grillePtr[idCell]->getTopRightYReel();
                    int alignement = aGaucheIntReel(n1X,n1Y,n2X,n2Y,cellX,cellY);
                    if (alignement == 1) {
                        idCell++;
                    }
                    else if (alignement == -1) {
                        idCell += nombreColonne;
                    }
                    else {
                        _aretes[i].vecIdCellules.push_back(idCell+nombreColonne);
                        _aretes[i].vecIdCellules.push_back(idCell+1);
                        idCell = idCell + nombreColonne + 1;
                    }
                    break;
                }
                case 2:
                    idCell += nombreColonne;
                    break;
                case 3: {
                    double cellX = grillePtr[idCell]->getTopLeftXReel();
                    double cellY = grillePtr[idCell]->getTopLeftYReel();
                    int alignement = aGaucheIntReel(n1X,n1Y,n2X,n2Y,cellX,cellY);
                    if (alignement == 1) {
                        idCell += nombreColonne;
                    }
                    else if (alignement == -1) {
                        idCell--;
                    }
                    else {
                        _aretes[i].vecIdCellules.push_back(idCell+nombreColonne);
                        _aretes[i].vecIdCellules.push_back(idCell-1);
                        idCell = idCell + nombreColonne - 1;

                    }
                    break;
                }
                case 4:
                    idCell--;
                    break;
                case 5: {
                    double cellX = grillePtr[idCell]->getBottomLeftXReel();
                    double cellY = grillePtr[idCell]->getBottomLeftYReel();
                    int alignement = aGaucheIntReel(n1X,n1Y,n2X,n2Y,cellX,cellY);
                    if (alignement == 1) {
                        idCell--;
                    }
                    else if (alignement == -1) {
                        idCell -= nombreColonne;
                    }
                    else {
                        _aretes[i].vecIdCellules.push_back(idCell-nombreColonne);
                        _aretes[i].vecIdCellules.push_back(idCell-1);
                        idCell = idCell - nombreColonne - 1;
                    }
                    break;
                }
                case 6:
                    idCell -= grille[0].size();
                    break;
                case 7: {
                    double cellX = grillePtr[idCell]->getBottomRightXReel();
                    double cellY = grillePtr[idCell]->getBottomRightYReel();
                    int alignement = aGaucheIntReel(n1X,n1Y,n2X,n2Y,cellX,cellY);
                    if (alignement == 1) {
                        idCell -= nombreColonne;
                    }
                    else if (alignement == -1) {
                        idCell++;
                    }
                    else {
                        _aretes[i].vecIdCellules.push_back(idCell-nombreColonne);
                        _aretes[i].vecIdCellules.push_back(idCell+1);
                        idCell = idCell - nombreColonne + 1;
                    }
                    break;
                }
            }
            _aretes[i].vecIdCellules.push_back(idCell);
        }
        for (const int& tmpIdCell : *vecCellArrive) {
            _aretes[i].vecIdCellules.push_back(tmpIdCell);
        }
        std::unordered_set<int> tmpUSet(_aretes[i].vecIdCellules.begin(), _aretes[i].vecIdCellules.end());
        _aretes[i].vecIdCellules.assign(tmpUSet.begin(), tmpUSet.end());
        for (const int& idCellule : _aretes[i].vecIdCellules) {
            grillePtr[idCellule]->containAreteId[i] = grillePtr[idCellule]->vecAreteId.size();
            grillePtr[idCellule]->vecAreteId.push_back(i);
        }
    }
}

// Enregistre les aretes dans la grille en coord flottantes
void Graphe::registerEdgeInGridReel(int i) {
    int nombreColonne = grille[0].size();
    double n1X = _aretes[i].getNoeud1()->_xreel;
    double n1Y = _aretes[i].getNoeud1()->_yreel;
    double n2X = _aretes[i].getNoeud2()->_xreel;
    double n2Y = _aretes[i].getNoeud2()->_yreel;
    int direction = getDirectionAreteReel(i);
    std::vector<int>* vecCellDepart = _aretes[i].getNoeud1()->idCelluleVec;
    int idCellX, idCellY;
    for (const int& tmpIdCell : *vecCellDepart) {
        _aretes[i].vecIdCellules.push_back(tmpIdCell);
    }
    int idCell = _aretes[i].getNoeud1()->idCelluleVec->at(0);
    std::vector<int>* vecCellArrive = _aretes[i].getNoeud2()->idCelluleVec;
    while (!isInVector(*vecCellArrive,idCell)) {
        switch(direction) {
            case 0:
                idCell++;
                break;
            case 1: {
                double cellX = grillePtr[idCell]->getTopRightXReel();
                double cellY = grillePtr[idCell]->getTopRightYReel();
                int alignement = aGaucheIntReel(n1X,n1Y,n2X,n2Y,cellX,cellY);
                if (alignement == 1) {
                    idCell++;
                }
                else if (alignement == -1) {
                    idCell += nombreColonne;
                }
                else {
                    _aretes[i].vecIdCellules.push_back(idCell+nombreColonne);
                    _aretes[i].vecIdCellules.push_back(idCell+1);
                    idCell = idCell + nombreColonne + 1;
                }
                break;
            }
            case 2:
                idCell += nombreColonne;
                break;
            case 3: {
                double cellX = grillePtr[idCell]->getTopLeftXReel();
                double cellY = grillePtr[idCell]->getTopLeftYReel();
                int alignement = aGaucheIntReel(n1X,n1Y,n2X,n2Y,cellX,cellY);
                if (alignement == 1) {
                    idCell += nombreColonne;
                }
                else if (alignement == -1) {
                    idCell--;
                }
                else {
                    _aretes[i].vecIdCellules.push_back(idCell+nombreColonne);
                    _aretes[i].vecIdCellules.push_back(idCell-1);
                    idCell = idCell + nombreColonne - 1;

                }
                break;
            }
            case 4:
                idCell--;
                break;
            case 5: {
                double cellX = grillePtr[idCell]->getBottomLeftXReel();
                double cellY = grillePtr[idCell]->getBottomLeftYReel();
                int alignement = aGaucheIntReel(n1X,n1Y,n2X,n2Y,cellX,cellY);
                if (alignement == 1) {
                    idCell--;
                }
                else if (alignement == -1) {
                    idCell -= nombreColonne;
                }
                else {
                    _aretes[i].vecIdCellules.push_back(idCell-nombreColonne);
                    _aretes[i].vecIdCellules.push_back(idCell-1);
                    idCell = idCell - nombreColonne - 1;
                }
                break;
            }
            case 6:
                idCell -= grille[0].size();
                break;
            case 7: {
                double cellX = grillePtr[idCell]->getBottomRightXReel();
                double cellY = grillePtr[idCell]->getBottomRightYReel();
                int alignement = aGaucheIntReel(n1X,n1Y,n2X,n2Y,cellX,cellY);
                if (alignement == 1) {
                    idCell -= nombreColonne;
                }
                else if (alignement == -1) {
                    idCell++;
                }
                else {
                    _aretes[i].vecIdCellules.push_back(idCell-nombreColonne);
                    _aretes[i].vecIdCellules.push_back(idCell+1);
                    idCell = idCell - nombreColonne + 1;
                }
                break;
            }
        }
        _aretes[i].vecIdCellules.push_back(idCell);
    }
    for (const int& tmpIdCell : *vecCellArrive) {
        _aretes[i].vecIdCellules.push_back(tmpIdCell);
    }
    std::unordered_set<int> tmpUSet(_aretes[i].vecIdCellules.begin(), _aretes[i].vecIdCellules.end());
    _aretes[i].vecIdCellules.assign(tmpUSet.begin(), tmpUSet.end());
    for (const int& idCellule : _aretes[i].vecIdCellules) {
        grillePtr[idCellule]->mutexArete->lock();
        grillePtr[idCellule]->vecAreteId.push_back(i);
        grillePtr[idCellule]->mutexArete->unlock();
    }
}

int Graphe::getDirectionArete(int idArete) {
    int x1 = _aretes[idArete].getNoeud1()->getX();
    int y1 = _aretes[idArete].getNoeud1()->getY();

    int x2 = _aretes[idArete].getNoeud2()->getX();
    int y2 = _aretes[idArete].getNoeud2()->getY();
    
    if (x2 > x1) { // Dirigé vers la droite
        if (y2 > y1) { // Dirigé en haut à droite
            return 1;
        }
        else if (y2 < y1) { // Dirigé en bas à droite
            return 7;
        }
        return 0; // Dirigé à droite
    }
    else if (x2 < x1) { // Dirigé vers la gauche
        if (y2 > y1) {   // Dirigé en haut à gauche
            return 3;
        }
        else if (y2 < y1) { // Dirigé en bas à gauche
            return 5;
        }
        return 4; // Dirigé à gauche
    }
    else { // Dirigé verticalement
        if (y2 > y1) { // Dirigé vers le haut
            return 2;
        }
        return 6; // Dirigé vers le bas
    }
}

int Graphe::getDirectionAreteReel(int idArete) {
    double x1 = _aretes[idArete].getNoeud1()->_xreel;
    double y1 = _aretes[idArete].getNoeud1()->_yreel;
    double x2 = _aretes[idArete].getNoeud2()->_xreel;
    double y2 = _aretes[idArete].getNoeud2()->_yreel;

    if ((x2 > x1 + epsilon)&&(x2 - epsilon > x1)) { // Dirigé vers la droite
        if ((y2 > y1 + epsilon)&&(y2 - epsilon > y1)) { // Dirigé en haut à droite
            return 1;
        }
        else if ((y2 < y1 - epsilon)&&(y2 + epsilon < y1)) { // Dirigé en bas à droite
            return 7;
        }
        return 0; // Dirigé à droite
    }
    else if ((x2 < x1 - epsilon)&&(x2 + epsilon < x1)) { // Dirigé vers la gauche
        if ((y2 > y1 + epsilon)&&(y2 - epsilon > y1)) {   // Dirigé en haut à gauche
            return 3;
        }
        else if ((y2 < y1 - epsilon)&&(y2 + epsilon < y1)) { // Dirigé en bas à gauche
            return 5;
        }
        return 4; // Dirigé à gauche
    }
    else { // Dirigé verticalement
        if ((y2 > y1 + epsilon)&&(y2 - epsilon > y1)) { // Dirigé vers le haut
            return 2;
        }
        return 6; // Dirigé vers le bas
    }
}

void Graphe::recalcAreteCellule(int areteId) {
    std::vector<int> vecCellId;
    int n1X = _aretes[areteId].getNoeud1()->getX();
    int n1Y = _aretes[areteId].getNoeud1()->getY();
    int n2X = _aretes[areteId].getNoeud2()->getX();
    int n2Y = _aretes[areteId].getNoeud2()->getY();
    int direction = getDirectionArete(areteId);
    int idCell = _aretes[areteId].getNoeud1()->getEmplacement()->idCellule;
    int idCellArrive = _aretes[areteId].getNoeud2()->getEmplacement()->idCellule;
    vecCellId.push_back(idCell);
    int nombreColonne = grille[0].size();
    while (idCell != idCellArrive) {
        switch(direction) {
            case 0:
                idCell++;
                break;
            case 1: {
                int cellX = grillePtr[idCell]->getTopRightX();
                int cellY = grillePtr[idCell]->getTopRightY();
                int alignement = aGaucheInt(n1X,n1Y,n2X,n2Y,cellX,cellY);
                if (alignement == 1) {
                    idCell++;
                }
                else if (alignement == -1) {
                    idCell += nombreColonne;
                }
                else {
                    vecCellId.push_back(idCell+nombreColonne);
                    vecCellId.push_back(idCell+1);
                    idCell = idCell + nombreColonne + 1;
                }
                break;
            }
            case 2:
                idCell += nombreColonne;
                break;
            case 3: {
                int cellX = grillePtr[idCell]->getTopLeftX();
                int cellY = grillePtr[idCell]->getTopLeftY();
                int alignement = aGaucheInt(n1X,n1Y,n2X,n2Y,cellX,cellY);
                if (alignement == 1) {
                    idCell += nombreColonne;
                }
                else if (alignement == -1) {
                    idCell--;
                }
                else {
                    vecCellId.push_back(idCell+nombreColonne);
                    vecCellId.push_back(idCell-1);
                    idCell = idCell + nombreColonne - 1;
                }
                break;
            }
            case 4:
                idCell--;
                break;
            case 5: {
                int cellX = grillePtr[idCell]->getBottomLeftX();
                int cellY = grillePtr[idCell]->getBottomLeftY();
                int alignement = aGaucheInt(n1X,n1Y,n2X,n2Y,cellX,cellY);
                if (alignement == 1) {
                    idCell--;
                }
                else if (alignement == -1) {
                    idCell -= nombreColonne;
                }
                else {
                    vecCellId.push_back(idCell-nombreColonne);
                    vecCellId.push_back(idCell-1);
                    idCell = idCell - nombreColonne - 1;
                }
                break;
            }
            case 6:
                idCell -= grille[0].size();
                break;
            case 7: {
                int cellX = grillePtr[idCell]->getBottomRightX();
                int cellY = grillePtr[idCell]->getBottomRightY();
                int alignement = aGaucheInt(n1X,n1Y,n2X,n2Y,cellX,cellY);
                if (alignement == 1) {
                    idCell -= nombreColonne;
                }
                else if (alignement == -1) {
                    idCell++;
                }
                else {
                    vecCellId.push_back(idCell-nombreColonne);
                    vecCellId.push_back(idCell+1);
                    idCell = idCell - nombreColonne + 1;
                }
                break;
            }
        }
        vecCellId.push_back(idCell);
    }
    int sameUntil=0;
    int minSize = min(vecCellId.size(),_aretes[areteId].vecIdCellules.size());
    for (;((sameUntil<minSize)&&(vecCellId[sameUntil]==_aretes[areteId].vecIdCellules[sameUntil]));sameUntil++);
    for (int i=sameUntil;i<_aretes[areteId].vecIdCellules.size();i++) {
        int indexCell = _aretes[areteId].vecIdCellules[i];
        int lastElem = grillePtr[indexCell]->vecAreteId.size() - 1;
        int lastElemId = grillePtr[indexCell]->vecAreteId[lastElem];
        if (lastElemId != areteId) {
            grillePtr[indexCell]->containAreteId[lastElemId] = grillePtr[indexCell]->containAreteId[areteId];
            grillePtr[indexCell]->vecAreteId[grillePtr[indexCell]->containAreteId[areteId]] = lastElemId;
        }
        grillePtr[indexCell]->containAreteId[areteId] = -1;
        grillePtr[indexCell]->vecAreteId.pop_back();
    }
    for (int i=sameUntil;i<vecCellId.size();i++) {
        if (grillePtr[vecCellId[i]]->containAreteId[areteId] == -1) {
            grillePtr[vecCellId[i]]->vecAreteId.push_back(areteId);
            grillePtr[vecCellId[i]]->containAreteId[areteId] = grillePtr[vecCellId[i]]->vecAreteId.size() - 1;
        }
    }
    _aretes[areteId].vecIdCellules.swap(vecCellId);
}

void Graphe::recalcAreteCelluleReel(int areteId) {
    std::vector<int> vecCellId;
    std::vector<bool> vecCellAlreadyPassed(grillePtr.size(),false);
    double n1X = _aretes[areteId].getNoeud1()->_xreel;
    double n1Y = _aretes[areteId].getNoeud1()->_yreel;
    double n2X = _aretes[areteId].getNoeud2()->_xreel;
    double n2Y = _aretes[areteId].getNoeud2()->_yreel;
    int direction = getDirectionAreteReel(areteId);
    int startNodeId = _aretes[areteId]._noeud1->_id;
    for (const int& tmpIdCell : *_noeuds[startNodeId].idCelluleVec) {
        vecCellId.push_back(tmpIdCell);
        vecCellAlreadyPassed[tmpIdCell] = true;
    }
    int idCell = vecCellId[0];
    bool aligneHorizontal = false, aligneVertical = false;
    if ((direction == 0)||(direction == 4)) { aligneHorizontal = checkNodeAlignementHorizontal(startNodeId); }
    else if ((direction == 2)||(direction == 6)) { aligneVertical = checkNodeAlignementVertical(startNodeId); }
    if (aligneHorizontal||aligneVertical) {
        for (const int& tmpId : vecCellId) {
            if (tmpId < idCell) { idCell = tmpId; }
        }
    }
    std::vector<int>* vecArrive = _aretes[areteId].getNoeud2()->idCelluleVec;
    int nombreColonne = grille[0].size();
    bool endNotFound = true;
    for (const int& arriveNums : *vecArrive) { 
        if (arriveNums == idCell) { endNotFound = false; break; } 
    }
    while(endNotFound) {
        switch(direction) {
            case 0:
                idCell++;
                if (aligneHorizontal) {
                    int idCell1 = idCell+nombreColonne;
                    if (!vecCellAlreadyPassed[idCell1]) { vecCellId.push_back(idCell1); vecCellAlreadyPassed[idCell1] = true; }
                    for (const int& arriveNums : *vecArrive) { if (arriveNums == idCell1) { endNotFound = false; break; } }
                }
                break;
            case 1: {
                double cellX = grillePtr[idCell]->getTopRightXReel();
                double cellY = grillePtr[idCell]->getTopRightYReel();
                int alignement = aGaucheIntReel(n1X,n1Y,n2X,n2Y,cellX,cellY);
                if (alignement == 1) {
                    idCell++;
                }
                else if (alignement == -1) {
                    idCell += nombreColonne;
                }
                else {
                    int idCell1 = idCell+nombreColonne;
                    int idCell2 = idCell+1;
                    if (!vecCellAlreadyPassed[idCell1]) { vecCellId.push_back(idCell1); vecCellAlreadyPassed[idCell1] = true; }
                    if (!vecCellAlreadyPassed[idCell2]) { vecCellId.push_back(idCell2); vecCellAlreadyPassed[idCell2] = true; }
                    idCell = idCell + nombreColonne + 1;
                }
                break;
            }
            case 2:
                idCell += nombreColonne;
                if (aligneVertical) {
                    int idCell1 = idCell+1;
                    if (!vecCellAlreadyPassed[idCell1]) { vecCellId.push_back(idCell1); vecCellAlreadyPassed[idCell1] = true; }
                    for (const int& arriveNums : *vecArrive) { if (arriveNums == idCell1) { endNotFound = false; break; } }
                }
                break;
            case 3: {
                double cellX = grillePtr[idCell]->getTopLeftXReel();
                double cellY = grillePtr[idCell]->getTopLeftYReel();
                int alignement = aGaucheIntReel(n1X,n1Y,n2X,n2Y,cellX,cellY);
                if (alignement == 1) {
                    idCell += nombreColonne;
                }
                else if (alignement == -1) {
                    idCell--;
                }
                else {
                    int idCell1 = idCell+nombreColonne;
                    int idCell2 = idCell-1;
                    if (!vecCellAlreadyPassed[idCell1]) { vecCellId.push_back(idCell1); vecCellAlreadyPassed[idCell1] = true; }
                    if (!vecCellAlreadyPassed[idCell2]) { vecCellId.push_back(idCell2); vecCellAlreadyPassed[idCell2] = true; }
                    idCell = idCell + nombreColonne - 1;
                }
                break;
            }
            case 4:
                idCell--;
                if (aligneHorizontal) {
                    int idCell1 = idCell+nombreColonne;
                    if (!vecCellAlreadyPassed[idCell1]) { vecCellId.push_back(idCell1); vecCellAlreadyPassed[idCell1] = true; }
                    for (const int& arriveNums : *vecArrive) { if (arriveNums == idCell1) { endNotFound = false; break; } }
                }
                break;
            case 5: {
                double cellX = grillePtr[idCell]->getBottomLeftXReel();
                double cellY = grillePtr[idCell]->getBottomLeftYReel();
                int alignement = aGaucheIntReel(n1X,n1Y,n2X,n2Y,cellX,cellY);
                if (alignement == 1) {
                    idCell--;
                }
                else if (alignement == -1) {
                    idCell -= nombreColonne;
                }
                else {
                    int idCell1 = idCell-nombreColonne;
                    int idCell2 = idCell-1;
                    if (!vecCellAlreadyPassed[idCell1]) { vecCellId.push_back(idCell1); vecCellAlreadyPassed[idCell1] = true; }
                    if (!vecCellAlreadyPassed[idCell2]) { vecCellId.push_back(idCell2); vecCellAlreadyPassed[idCell2] = true; }
                    idCell = idCell - nombreColonne - 1;
                }
                break;
            }
            case 6:
                idCell -= nombreColonne;
                if (aligneVertical) {
                    int idCell1 = idCell+1;
                    if (!vecCellAlreadyPassed[idCell1]) { vecCellId.push_back(idCell1); vecCellAlreadyPassed[idCell1] = true; }
                    for (const int& arriveNums : *vecArrive) { if (arriveNums == idCell1) { endNotFound = false; break; } }
                }
                break;
            case 7: {
                double cellX = grillePtr[idCell]->getBottomRightXReel();
                double cellY = grillePtr[idCell]->getBottomRightYReel();
                int alignement = aGaucheIntReel(n1X,n1Y,n2X,n2Y,cellX,cellY);
                if (alignement == 1) {
                    idCell -= nombreColonne;
                }
                else if (alignement == -1) {
                    idCell++;
                }
                else {
                    int idCell1 = idCell-nombreColonne;
                    int idCell2 = idCell+1;
                    if (!vecCellAlreadyPassed[idCell1]) { vecCellId.push_back(idCell1); vecCellAlreadyPassed[idCell1] = true; }
                    if (!vecCellAlreadyPassed[idCell2]) { vecCellId.push_back(idCell2); vecCellAlreadyPassed[idCell2] = true; }
                    idCell = idCell - nombreColonne + 1;
                }
                break;
            }
        }
        if (!vecCellAlreadyPassed[idCell]) { vecCellId.push_back(idCell); vecCellAlreadyPassed[idCell] = true; }
        for (const int& arriveNums : *vecArrive) { if (arriveNums == idCell) { endNotFound = false; break; } }
    }
    for (const int& tmpIdCell : *vecArrive) {
        if (!vecCellAlreadyPassed[tmpIdCell]) { vecCellId.push_back(tmpIdCell); vecCellAlreadyPassed[tmpIdCell] = true; }
    }
    int sameUntil=0;
    int minSize = min(vecCellId.size(),_aretes[areteId].vecIdCellules.size());
    for (;((sameUntil<minSize)&&(vecCellId[sameUntil]==_aretes[areteId].vecIdCellules[sameUntil]));sameUntil++);
    for (int i=sameUntil;i<_aretes[areteId].vecIdCellules.size();i++) {
        int indexCell = _aretes[areteId].vecIdCellules[i];
        int lastElem = grillePtr[indexCell]->vecAreteId.size() - 1;
        int lastElemId = grillePtr[indexCell]->vecAreteId[lastElem];
        if (lastElemId != areteId) {
            grillePtr[indexCell]->containAreteId[lastElemId] = grillePtr[indexCell]->containAreteId[areteId];
            grillePtr[indexCell]->vecAreteId[grillePtr[indexCell]->containAreteId[areteId]] = lastElemId;
        }
        grillePtr[indexCell]->containAreteId[areteId] = -1;
        grillePtr[indexCell]->vecAreteId.pop_back();
    }
    for (int i=sameUntil;i<vecCellId.size();i++) {
        if (grillePtr[vecCellId[i]]->containAreteId[areteId] == -1) {
            grillePtr[vecCellId[i]]->vecAreteId.push_back(areteId);
            grillePtr[vecCellId[i]]->containAreteId[areteId] = grillePtr[vecCellId[i]]->vecAreteId.size() - 1;
        }
    }
    _aretes[areteId].vecIdCellules.swap(vecCellId);
}

void Graphe::recalcNoeudIsoleGrid(int nodeId) {
    for (const int cellId : (*_noeuds[nodeId].idCelluleVec)) {
        int numero = grillePtr[cellId]->containSingleNodeId[nodeId];
        int lastNumero = grillePtr[cellId]->vecSingleNodeId.size() - 1;
        grillePtr[cellId]->containSingleNodeId[nodeId] = -1;
        grillePtr[cellId]->vecSingleNodeId[numero] = grillePtr[cellId]->vecSingleNodeId[lastNumero];
        grillePtr[cellId]->vecSingleNodeId.pop_back();
        int nodeIdRemplace = grillePtr[cellId]->vecSingleNodeId[numero];
        grillePtr[cellId]->containSingleNodeId[nodeIdRemplace] = numero;
    }
    _noeuds[nodeId].idCelluleVec->clear();
    if (_noeuds[nodeId].getEmplacement()->idCelluleVec != nullptr) {
        *_noeuds[nodeId].idCelluleVec = *_noeuds[nodeId].getEmplacement()->idCelluleVec;
    }
    else {
        _noeuds[nodeId].idCelluleVec->push_back(_noeuds[nodeId].getEmplacement()->idCellule);
    }
    for (const int cellId : (*_noeuds[nodeId].idCelluleVec)) {
        int numero = grillePtr[cellId]->vecSingleNodeId.size();
        grillePtr[cellId]->vecSingleNodeId.push_back(nodeId);
        grillePtr[cellId]->containSingleNodeId[nodeId] = numero;
    }
}

void Graphe::recalcNodeCellule(int nodeId) {
    if (_noeuds[nodeId].estIsole()) { recalcNoeudIsoleGrid(nodeId); }
    else {
        for (const int& areteId : _noeuds[nodeId]._aretes) {
            if (_aretes[areteId].estPlace()) {
                recalcAreteCellule(areteId);
            }
        }
    }
}

void Graphe::recalcNodeCelluleReel(int nodeId) {
    recalcSpecificNodeCell(nodeId);
    for (const int& areteId : _noeuds[nodeId]._aretes) {
        recalcAreteCelluleReel(areteId);
    }
}

void Graphe::initAreteCellule(int areteId) {
    int n1X = _aretes[areteId].getNoeud1()->getX();
    int n1Y = _aretes[areteId].getNoeud1()->getY();
    int n2X = _aretes[areteId].getNoeud2()->getX();
    int n2Y = _aretes[areteId].getNoeud2()->getY();
    int direction = getDirectionArete(areteId);
    int idCell = _aretes[areteId].getNoeud1()->getEmplacement()->idCellule;
    int idCellArrive = _aretes[areteId].getNoeud2()->getEmplacement()->idCellule;
    _aretes[areteId].vecIdCellules.push_back(idCell);
    int nombreColonne = grille[0].size();
    while (idCell != idCellArrive) {
        switch(direction) {
            case 0:
                idCell++;
                break;
            case 1: {
                int cellX = grillePtr[idCell]->getTopRightX();
                int cellY = grillePtr[idCell]->getTopRightY();
                int alignement = aGaucheInt(n1X,n1Y,n2X,n2Y,cellX,cellY);
                if (alignement == 1) {
                    idCell++;
                }
                else if (alignement == -1) {
                    idCell += nombreColonne;
                }
                else {
                    _aretes[areteId].vecIdCellules.push_back(idCell+nombreColonne);
                    _aretes[areteId].vecIdCellules.push_back(idCell+1);
                    idCell = idCell + nombreColonne + 1;
                }
                break;
            }
            case 2:
                idCell += nombreColonne;
                break;
            case 3: {
                int cellX = grillePtr[idCell]->getTopLeftX();
                int cellY = grillePtr[idCell]->getTopLeftY();
                int alignement = aGaucheInt(n1X,n1Y,n2X,n2Y,cellX,cellY);
                if (alignement == 1) {
                    idCell += nombreColonne;
                }
                else if (alignement == -1) {
                    idCell--;
                }
                else {
                    _aretes[areteId].vecIdCellules.push_back(idCell+nombreColonne);
                    _aretes[areteId].vecIdCellules.push_back(idCell-1);
                    idCell = idCell + nombreColonne - 1;
                }
                break;
            }
            case 4:
                idCell--;
                break;
            case 5: {
                int cellX = grillePtr[idCell]->getBottomLeftX();
                int cellY = grillePtr[idCell]->getBottomLeftY();
                int alignement = aGaucheInt(n1X,n1Y,n2X,n2Y,cellX,cellY);
                if (alignement == 1) {
                    idCell--;
                }
                else if (alignement == -1) {
                    idCell -= nombreColonne;
                }
                else {
                    _aretes[areteId].vecIdCellules.push_back(idCell-nombreColonne);
                    _aretes[areteId].vecIdCellules.push_back(idCell-1);
                    idCell = idCell - nombreColonne - 1;
                }
                break;
            }
            case 6:
                idCell -= grille[0].size();
                break;
            case 7: {
                int cellX = grillePtr[idCell]->getBottomRightX();
                int cellY = grillePtr[idCell]->getBottomRightY();
                int alignement = aGaucheInt(n1X,n1Y,n2X,n2Y,cellX,cellY);
                if (alignement == 1) {
                    idCell -= nombreColonne;
                }
                else if (alignement == -1) {
                    idCell++;
                }
                else {
                    _aretes[areteId].vecIdCellules.push_back(idCell-nombreColonne);
                    _aretes[areteId].vecIdCellules.push_back(idCell+1);
                    idCell = idCell - nombreColonne + 1;
                }
                break;
            }
        }
        _aretes[areteId].vecIdCellules.push_back(idCell);
    }
    for (int i=0;i<_aretes[areteId].vecIdCellules.size();i++) {
        grillePtr[_aretes[areteId].vecIdCellules[i]]->vecAreteId.push_back(areteId);
    }
}

void Graphe::initNodeCellule(int nodeId) {
    for (const int& areteId : _noeuds[nodeId]._aretes) {
        if (_aretes[areteId].estPlace()) {
            initAreteCellule(areteId);
        }
    }
}

void Graphe::calcAreteCelluleVec(std::vector<int>& vecCellId, int areteId) {
    int n1X = _aretes[areteId].getNoeud1()->getX();
    int n1Y = _aretes[areteId].getNoeud1()->getY();
    int n2X = _aretes[areteId].getNoeud2()->getX();
    int n2Y = _aretes[areteId].getNoeud2()->getY();
    int direction = getDirectionArete(areteId);
    int idCell = _aretes[areteId].getNoeud1()->getEmplacement()->idCellule;
    int idCellArrive = _aretes[areteId].getNoeud2()->getEmplacement()->idCellule;
    vecCellId.push_back(idCell);
    int nombreColonne = grille[0].size();
    while (idCell != idCellArrive) {
        switch(direction) {
            case 0:
                idCell++;
                break;
            case 1: {
                int cellX = grillePtr[idCell]->getTopRightX();
                int cellY = grillePtr[idCell]->getTopRightY();
                int alignement = aGaucheInt(n1X,n1Y,n2X,n2Y,cellX,cellY);
                if (alignement == 1) {
                    idCell++;
                }
                else if (alignement == -1) {
                    idCell += nombreColonne;
                }
                else {
                    vecCellId.push_back(idCell+nombreColonne);
                    vecCellId.push_back(idCell+1);
                    idCell = idCell + nombreColonne + 1;
                }
                break;
            }
            case 2:
                idCell += nombreColonne;
                break;
            case 3: {
                int cellX = grillePtr[idCell]->getTopLeftX();
                int cellY = grillePtr[idCell]->getTopLeftY();
                int alignement = aGaucheInt(n1X,n1Y,n2X,n2Y,cellX,cellY);
                if (alignement == 1) {
                    idCell += nombreColonne;
                }
                else if (alignement == -1) {
                    idCell--;
                }
                else {
                    vecCellId.push_back(idCell+nombreColonne);
                    vecCellId.push_back(idCell-1);
                    idCell = idCell + nombreColonne - 1;
                }
                break;
            }
            case 4:
                idCell--;
                break;
            case 5: {
                int cellX = grillePtr[idCell]->getBottomLeftX();
                int cellY = grillePtr[idCell]->getBottomLeftY();
                int alignement = aGaucheInt(n1X,n1Y,n2X,n2Y,cellX,cellY);
                if (alignement == 1) {
                    idCell--;
                }
                else if (alignement == -1) {
                    idCell -= nombreColonne;
                }
                else {
                    vecCellId.push_back(idCell-nombreColonne);
                    vecCellId.push_back(idCell-1);
                    idCell = idCell - nombreColonne - 1;
                }
                break;
            }
            case 6:
                idCell -= grille[0].size();
                break;
            case 7: {
                int cellX = grillePtr[idCell]->getBottomRightX();
                int cellY = grillePtr[idCell]->getBottomRightY();
                int alignement = aGaucheInt(n1X,n1Y,n2X,n2Y,cellX,cellY);
                if (alignement == 1) {
                    idCell -= nombreColonne;
                }
                else if (alignement == -1) {
                    idCell++;
                }
                else {
                    vecCellId.push_back(idCell-nombreColonne);
                    vecCellId.push_back(idCell+1);
                    idCell = idCell - nombreColonne + 1;
                }
                break;
            }
        }
        vecCellId.push_back(idCell);
    }
}

void Graphe::calculeNodeCelluleVec(std::vector<std::vector<int>>& vecVecInt, int nodeId) {
    for (int i=0;i<_noeuds[nodeId]._aretes.size();i++) {
        std::vector<int> tmpVec;
        calcAreteCelluleVec(tmpVec,_noeuds[nodeId]._aretes[i]);
        vecVecInt.push_back(tmpVec);
    }
}

void Graphe::applyNewAreteCelluleVec(std::vector<std::vector<int>>& vecId, int nodeIndex) {
    for (int numArete=0;numArete<vecId.size();numArete++) {
        std::vector<int>& vecCellId = vecId[numArete];
        int areteId = _noeuds[nodeIndex]._aretes[numArete];
        int sameUntil;
        int minSize = min(vecCellId.size(),_aretes[areteId].vecIdCellules.size());
        for (sameUntil=0;((sameUntil<minSize)&&(vecCellId[sameUntil]==_aretes[areteId].vecIdCellules[sameUntil]));sameUntil++);
        for (int i=sameUntil;i<_aretes[areteId].vecIdCellules.size();i++) {
            removeFromVector(grillePtr[_aretes[areteId].vecIdCellules[i]]->vecAreteId,areteId);
        }
        for (int i=sameUntil;i<vecCellId.size();i++) {
            if (!isInVector(grillePtr[vecCellId[i]]->vecAreteId,areteId)) {
                grillePtr[vecCellId[i]]->vecAreteId.push_back(areteId);
            }
        }
        _aretes[areteId].vecIdCellules.swap(vecCellId);
    }
}

void Graphe::updatePenalite(int pen1, int pen2) {
    PENALITE_MAX = pen1;
    PENALITE_MAX_SELF = pen2;
}

void Graphe::setupGraphe(std::string fileGraphe, std::string fileSlot) {
    std::string pathGraph = fileGraphe;
    if (!containsString(fileGraphe,chemin)) {
        pathGraph = chemin + "exemple/Graphe/" + fileGraphe + ".json";
    }
    readFromJsonGraph(pathGraph);
    if (!useCoordReel) {
        std::string pathSlot;
        if (!containsString(fileSlot,"Grid")) {
            pathSlot = chemin + "exemple/Slots/" + fileSlot + ".json";
            readFromJsonSlots(pathSlot);
        }
        else {
            int nbNoeud = std::min((int)_noeuds.size()*2,6000);
            generateGrid(nbNoeud,nbNoeud);
        }
        if (_noeuds.size() > _emplacements.size()) {
            tcout() << "Pas assez d'emplacement.\n";
            exit(3);
        }
    }
    updatePenalite(_noeuds.size(),_noeuds.size());
    updateIsolatedNodes();
}

void Graphe::setupGrapheReel(std::string fileGraphe) {
    std::string pathGraph = fileGraphe;
    if (!containsString(fileGraphe,chemin)) {
        pathGraph = chemin + "exemple/Graphe/" + fileGraphe + ".json";
    }
    readFromJsonGraph(pathGraph);
    updateIsolatedNodes();
}

void Graphe::getSortedEmpVecFromGraphe(std::vector<int>& sortedIdVec, Graphe& G) {
    std::vector<Emplacement*> empPtrVec;
    empPtrVec.resize(G._emplacements.size());
    for (int i = 0; i < G._emplacements.size(); i++) {
        empPtrVec[i] = &G._emplacements[i];
    }
    // Tri des emplacements pas la coord X
    std::sort(empPtrVec.begin(), empPtrVec.end(), comparePtrEmplacement);
    sortedIdVec.resize(empPtrVec.size());
    for (int i=0;i<empPtrVec.size();i++) {
        sortedIdVec[i] = empPtrVec[i]->getId();
    }
}

void Graphe::translateGrapheToOrigin() {
    double minX=0.0, minY=0.0;
    for (int i=0;i<_emplacements.size();i++) {
        if (i==0) {
            minX = _emplacements[i].getX();
            minY = _emplacements[i].getY();
        }
        else {
            if (_emplacements[i].getX() < minX) minX = _emplacements[i].getX();
            if (_emplacements[i].getY() < minY) minY = _emplacements[i].getY();
        }
    }
    for (int i=0;i<_emplacements.size();i++) {
        _emplacements[i]._x -= minX;
        _emplacements[i]._y -= minY;
    }
    gridWidth -= minX;
    gridHeight -= minY;
}

void Graphe::translateGrapheToOriginReel(double marge) {
    double minX, minY;
    double maxX, maxY;
    bool dynMarge = false;
    if (marge == -1) {
        marge = 0; dynMarge = true;
    }
    minX = _noeuds[0]._xreel;
    minY = _noeuds[0]._yreel;
    maxX = _noeuds[0]._xreel;
    maxY = _noeuds[0]._yreel;
    for (int i=1;i<_noeuds.size();i++) {
        if (_noeuds[i]._xreel < minX) { minX = _noeuds[i]._xreel; }
        else if (_noeuds[i]._xreel > maxX) { maxX = _noeuds[i]._xreel; }

        if (_noeuds[i]._yreel < minY) { minY = _noeuds[i]._yreel; }
        else if (_noeuds[i]._yreel > maxY) { maxY = _noeuds[i]._yreel; }
    }
    for (int i=0;i<_noeuds.size();i++) {
        _noeuds[i]._xreel = _noeuds[i]._xreel - minX + marge;
        _noeuds[i]._yreel = _noeuds[i]._yreel - minY + marge;
    }
    gridWidth = maxX-minX + 2*marge;
    gridHeight = maxY-minY + 2*marge;
    if (dynMarge) {
        double margeX = (double)gridWidth;
        double margeY = (double)gridHeight;
        for (int i=0;i<_noeuds.size();i++) {
            _noeuds[i]._xreel += margeX;
            _noeuds[i]._yreel += margeY;
        }
        gridWidth = gridWidth+(2*margeX);
        gridHeight = gridHeight+(2*margeY);
    }
    initCoordReelDistribution((double)gridWidth,(double)gridHeight);
}

void Graphe::scaleGraph(int n) {
    for (int i=0;i<_emplacements.size();i++) {
        _emplacements[i]._x *= n;
        _emplacements[i]._y *= n;
    }
    gridHeight *= n;
    gridWidth *= n;
}

Emplacement* Graphe::getClosestEmplacementFromPoint(double x, double y, bool isFree) {
    double minDist = std::numeric_limits<double>::max();
    int empId = -1;
    for (int i=0;i<_emplacements.size();i++) {
        if (!isFree||_emplacements[i].estDisponible()) {
            double xDiff = x - (double)_emplacements[i].getX();
            double yDiff = y - (double)_emplacements[i].getY();
            double newDist = xDiff * xDiff + yDiff * yDiff;
            if (newDist < minDist) {
                minDist = newDist;
                empId = i;
            }
        }
    }
    return &_emplacements[empId];
}

int Graphe::getClosestNodeFromPoint(double x, double y) {
    int nodeId = 0;
    double minDist = std::numeric_limits<double>::max();
    for (int i=0;i<_noeuds.size();i++) {
        if (_noeuds[i].estPlace()) {
            double xDiff = x - (double)_noeuds[i].getEmplacement()->getX();
            double yDiff = y - (double)_noeuds[i].getEmplacement()->getY();
            double newDist = xDiff * xDiff + yDiff * yDiff;
            if (newDist < minDist) {
                minDist = newDist;
                nodeId = i;
            }
        }
    }
    return nodeId;
}

int Graphe::getClosestNodeFromPointReel(double x, double y) {
    int nodeId = 0;
    double minDist = std::numeric_limits<double>::max();
    for (int i=0;i<_noeuds.size();i++) {
        double xDiff = x - _noeuds[i]._xreel;
        double yDiff = y - _noeuds[i]._yreel;
        double newDist = xDiff * xDiff + yDiff * yDiff;
        if (newDist < minDist) {
            minDist = newDist;
            nodeId = i;
        }
    }
    return nodeId;
}

void Graphe::searchInCellClosestEmplacement(double x, double y,int cellX,int cellY,int& closestEmpId,double& minDist, bool isFree) {
    for (int& empId : grille[cellY][cellX].vecEmplacementId) {
        if (!isFree||_emplacements[empId].estDisponible()) {
            double xDiff = x - (double)_emplacements[empId].getX();
            double yDiff = y - (double)_emplacements[empId].getY();
            double newDist = xDiff * xDiff + yDiff * yDiff;
            if (newDist < minDist) {
                minDist = newDist;
                closestEmpId = empId;
            }
        }
    }
}

// Vide le vecteur et ajoute les id des cellules autour des précédentes.
// La premiere case du vecteur est la cellule en bas a gauche de la zone, la derniere et la cellule en haut a droite.
void Graphe::enlargeSearchVector(std::vector<std::pair<int,int>>& searchVector) {
    std::pair<int,int> firstCellCoord = searchVector[0];
    std::pair<int,int> lastCellCoord = searchVector[searchVector.size()-1];
    int startX = std::max(firstCellCoord.first-1,0);
    int endX = std::min(lastCellCoord.first+1,(int)grille[0].size()-1);
    int startY = std::max(firstCellCoord.second-1,0);
    int endY = std::min(lastCellCoord.second+1,(int)grille.size()-1);
    searchVector.clear();
    for (int x=startX;x<=endX;x++) {
        searchVector.push_back(make_pair(x,startY));
    }
    for (int y=startY;y<endY;y++) {
        searchVector.push_back(make_pair(startX,y));
        searchVector.push_back(make_pair(endX,y));
    }
    for (int x=startX;x<=endX;x++) {
        searchVector.push_back(make_pair(x,endY));
    }
}

Emplacement* Graphe::getClosestEmplacementFromPointGrid(double x, double y, bool isFree) {
    int sizeColumn = grille[0][0].getBottomRightX() - grille[0][0].getBottomLeftX();
    int sizeRow = grille[0][0].getTopLeftY() - grille[0][0].getBottomLeftY();
    double dnumX = x/(double)sizeColumn;
    double dnumY = y/(double)sizeRow;
    int numX = round(dnumX);
    if (numX == grille[0].size()) { numX--; }
    int numY = round(dnumY);
    if (numY == grille.size()) { numY--; }
    int startX = std::max(numX-1,0);
    startX = std::min(startX,(int)grille[0].size()-1);
    int endX = std::min(numX+1,(int)grille[0].size()-1);
    endX = std::max(endX,0);
    int startY = std::max(numY-1,0);
    startY = std::min(startY,(int)grille.size()-1);
    int endY = std::min(numY+1,(int)grille.size()-1);
    endY = std::max(endY,0);
    std::vector<std::pair<int,int>> searchVector;
    for (int i=startY;i<=endY;i++) {
        for (int j=startX;j<=endX;j++) {
            searchVector.push_back(make_pair(j,i));
        }
    }
    double minDist = std::numeric_limits<double>::max();
    int closestEmpId = -1;
    while (closestEmpId == -1) {
        for (std::pair<int,int>& cellCoord : searchVector) {
            searchInCellClosestEmplacement(x,y,cellCoord.first,cellCoord.second,closestEmpId,minDist,isFree);
        }
        if (closestEmpId == -1) {
            enlargeSearchVector(searchVector);
        }
    }
    return &_emplacements[closestEmpId];
}

bool Graphe::isGrapheConnected() {
    if (_noeuds.size() == 0) { return false; }
    std::vector<bool> visitedNode(_noeuds.size(),false);
    int totalVisited = 0;
    std::vector<int> nodeIdVec(1,0);
    for (int i=0;i<nodeIdVec.size();i++) {
        int nodeId = nodeIdVec[i];
        if (!visitedNode[nodeId]) {
            visitedNode[nodeId] = true;
            totalVisited++;
            for (Noeud* n : _noeuds[nodeId].voisins) {
                if (!visitedNode[n->_id]) { nodeIdVec.push_back(n->_id); }
            }
        }
    }
    return (totalVisited == _noeuds.size());
}

void Graphe::calcMaxAndAverageDegree() {
    maxVoisin = 0;
    avgVoisin = 0.0;
    for (int i=0;i<_noeuds.size();i++) {
        int nombreVoisin = _noeuds[i]._aretes.size();
        if (nombreVoisin > maxVoisin) {
            maxVoisin = nombreVoisin;
        }
        avgVoisin += nombreVoisin;
    }
    avgVoisin = avgVoisin / (double)_noeuds.size();
}

void Graphe::rotateNode(double angle, int nodeId, double centerX,double centerY) {
    double angleRadian = angle * (M_PI/180.0); 
    double s = sin(angleRadian);
    double c = cos(angleRadian);

    // translate point back to origin:
    _noeuds[nodeId]._xreel -= centerX;
    _noeuds[nodeId]._yreel -= centerY;

    // rotate point
    double xnew = _noeuds[nodeId]._xreel * c - _noeuds[nodeId]._yreel * s;
    double ynew = _noeuds[nodeId]._xreel * s + _noeuds[nodeId]._yreel * c;

    // translate point back:
    _noeuds[nodeId]._xreel = xnew + centerX;
    _noeuds[nodeId]._yreel = ynew + centerY;
}

void Graphe::rotateGraph(double angle) {
    std::pair<double,double> centreGravite = getCentreGraviteDoubleNoeuds();
    if (useCoordReel) {
        for (int i=0;i<_noeuds.size();i++) {
            rotateNode(angle,i,centreGravite.first,centreGravite.second);
        }
    }
    else {
        for (int i=0;i<_noeuds.size();i++) {
            _noeuds[i]._xreel = _noeuds[i].getEmplacement()->getX();
            _noeuds[i]._yreel = _noeuds[i].getEmplacement()->getY();
        }
        clearNodeEmplacement();
        for (int i=0;i<_noeuds.size();i++) {
            rotateNode(angle,i,centreGravite.first,centreGravite.second);
        }
        for (int i=0;i<_noeuds.size();i++) {
            Emplacement* currentEmp;
            if (grillePtr.size() > 0) {
                currentEmp = getClosestEmplacementFromPointGrid(_noeuds[i]._xreel,_noeuds[i]._yreel,true);
            }
            else {
                currentEmp = getClosestEmplacementFromPoint(_noeuds[i]._xreel,_noeuds[i]._yreel,true);
            }
            _noeuds[i].setEmplacement(currentEmp);
        }
    }
}

double Graphe::distMoyDelaunayVoisins() {
    if (!isCarteSetUp) {
        triangulationDelaunay();
    }
    double moyenne = 0.0;
    int totalArete = 0;
    for (int i=0;i<_emplacements.size()-1;i++) {
        for (int j=0;j<_emplacements[i].voisinsDelaunay->size();j++) {
            const int& indexVoisin = _emplacements[i].voisinsDelaunay->at(j);
            if (indexVoisin > i) {
                double xDiff = _emplacements[i].getX() - _emplacements[indexVoisin].getX();
                double yDiff = _emplacements[i].getY() - _emplacements[indexVoisin].getY();
                moyenne += sqrt(xDiff * xDiff + yDiff * yDiff);
                totalArete++;
            }
        }
    }
    return moyenne/(double)totalArete;
}

double Graphe::distMoyNTirage(int n) {
    double moyenne = 0.0;
    for (int i=0;i<n;i++) {
        int empId1 = generateRand(_emplacements.size() - 1);
        int empId2 = generateRand(_emplacements.size() - 1);
        while (empId2 == empId1) {
            empId2 = generateRand(_emplacements.size() - 1);
        }
        double xDiff = _emplacements[empId1].getX() - _emplacements[empId2].getX();
        double yDiff = _emplacements[empId1].getY() - _emplacements[empId2].getY();
        moyenne += sqrt(xDiff * xDiff + yDiff * yDiff);
    }
    return moyenne/(double)n;
}

void Graphe::getMinMaxFromNodes() {
    minXNode=_noeuds[0].getEmplacement()->getX();
    maxXNode=_noeuds[0].getEmplacement()->getX();
    minYNode=_noeuds[0].getEmplacement()->getY();
    maxYNode=_noeuds[0].getEmplacement()->getY();
    for (int i=1;i<_noeuds.size();i++) {
        int x = _noeuds[i].getEmplacement()->getX();
        int y = _noeuds[i].getEmplacement()->getY();
        if (x < minXNode) { minXNode = x; }
        else if (x > maxXNode) { maxXNode = x; }
        if (y < minYNode) { minYNode = y; }
        else if (y > maxYNode) { maxYNode = y; }
    }
}

double Graphe::moyenneLongueurAretes() {
    double moyenne = 0.0;
    for (int i=0;i<_aretes.size();i++) {
        double xDiff = _aretes[i]._noeud1->getEmplacement()->getX() - _aretes[i]._noeud2->getEmplacement()->getX();
        double yDiff = _aretes[i]._noeud1->getEmplacement()->getY() - _aretes[i]._noeud2->getEmplacement()->getY();
        moyenne += sqrt(xDiff * xDiff + yDiff * yDiff);
    }
    return moyenne/(double)_aretes.size();
}

double Graphe::moyenneLongueurAretesReel() {
    double moyenne = 0.0;
    for (int i=0;i<_aretes.size();i++) {
        double xDiff = _aretes[i]._noeud1->stressX - _aretes[i]._noeud2->stressX;
        double yDiff = _aretes[i]._noeud1->stressY - _aretes[i]._noeud2->stressY;
        moyenne += sqrt(xDiff * xDiff + yDiff * yDiff);
    }
    return moyenne/(double)_aretes.size();
}

void Graphe::supprimerArete(int idArete) {
    int idNoeud1 = _aretes[idArete]._noeud1->_id;
    int idNoeud2 = _aretes[idArete]._noeud2->_id;
    // On enleve l'arete de idNoeud1
    for (int i=0;i<_noeuds[idNoeud1]._aretes.size();i++) {
        if (_noeuds[idNoeud1]._aretes[i] == idArete) {
            _noeuds[idNoeud1]._aretes[i] = _noeuds[idNoeud1]._aretes[_noeuds[idNoeud1]._aretes.size()-1];
            _noeuds[idNoeud1]._aretes.pop_back();
            break;
        }
    }
    // On enleve l'arete de idNoeud2
    for (int i=0;i<_noeuds[idNoeud2]._aretes.size();i++) {
        if (_noeuds[idNoeud2]._aretes[i] == idArete) {
            _noeuds[idNoeud2]._aretes[i] = _noeuds[idNoeud2]._aretes[_noeuds[idNoeud2]._aretes.size()-1];
            _noeuds[idNoeud2]._aretes.pop_back();
            break;
        }
    }
    // On met a jour les voisins de idNoeud1
    for (int i=0;i<_noeuds[idNoeud1].voisins.size();i++) {
        int idVoisin = _noeuds[idNoeud1].voisins[i]->_id;
        if (idVoisin == idNoeud2) {
            _noeuds[idNoeud1].voisins[i] = _noeuds[idNoeud1].voisins[_noeuds[idNoeud1].voisins.size()-1];
            _noeuds[idNoeud1].voisins.pop_back();
            break;
        }
    }
    // On met a jour les voisins de idNoeud2
    for (int i=0;i<_noeuds[idNoeud2].voisins.size();i++) {
        int idVoisin = _noeuds[idNoeud2].voisins[i]->_id;
        if (idVoisin == idNoeud1) {
            _noeuds[idNoeud2].voisins[i] = _noeuds[idNoeud2].voisins[_noeuds[idNoeud2].voisins.size()-1];
            _noeuds[idNoeud2].voisins.pop_back();
            break;
        }
    }
    // On met a jour le tableau en remplacant l'arete par la derniere
    int idLastArete = _aretes.size()-1;
    if (idArete != idLastArete) {
        int lastIdNoeud1 = _aretes[idLastArete]._noeud1->_id;
        int lastIdNoeud2 = _aretes[idLastArete]._noeud2->_id;
        for (int i=0;i<_noeuds[lastIdNoeud1]._aretes.size();i++) {
            if (_noeuds[lastIdNoeud1]._aretes[i] == idLastArete) {
                _noeuds[lastIdNoeud1]._aretes[i] = idArete;
                break;
            }
        }
        for (int i=0;i<_noeuds[lastIdNoeud2]._aretes.size();i++) {
            if (_noeuds[lastIdNoeud2]._aretes[i] == idLastArete) {
                _noeuds[lastIdNoeud2]._aretes[i] = idArete;
                break;
            }
        }
        _aretes[idArete] = _aretes[idLastArete];
        _aretes[idArete]._id = idArete;
    }
    _aretes.pop_back();
}

void Graphe::supprimerNoeud(int idNoeud) {
    // Suppression des aretes du noeuds
    for (const int& idArete : _noeuds[idNoeud]._aretes) {
        supprimerArete(idArete);
    }
    // Mise a jour du dernier noeud du tableau
    int idLastNoeud = _noeuds.size() - 1;
    if (idNoeud != idLastNoeud) {
        // A FINIR ICI (maj addresse dans aretes et id dans voisins)
        _noeuds[idNoeud] = _noeuds[idLastNoeud];
        _noeuds[idNoeud]._id = idNoeud;
    }
    _noeuds.pop_back();
}

std::vector<int> Graphe::plusGrandeComposanteConnexe() {
    int totalVisited = 0;
    int taillePlusGrande = 0;
    int totalNoeuds = _noeuds.size();
    int indexMeilleur = -1;
    std::vector<std::unordered_set<int>> composantesConnexes;
    std::vector<bool> visitedNode(totalNoeuds,false);
    while (taillePlusGrande < totalNoeuds-totalVisited) {
        int idDebut;
        for (int i=0;i<visitedNode.size();i++) {
            if (visitedNode[i] == false) {
                idDebut = i;
                break;
            }
        }
        std::unordered_set<int> tmpComposante;
        std::vector<int> nodeIdVec(1,idDebut);
        tmpComposante.insert(idDebut);
        for (int i=0;i<nodeIdVec.size();i++) {
            int nodeId = nodeIdVec[i];
            if (!visitedNode[nodeId]) {
                visitedNode[nodeId] = true;
                totalVisited++;
                for (Noeud* n : _noeuds[nodeId].voisins) {
                    if (!visitedNode[n->_id]) { nodeIdVec.push_back(n->_id); tmpComposante.insert(n->_id); }
                }
            }
        }
        if (tmpComposante.size() > taillePlusGrande) { taillePlusGrande = tmpComposante.size(); indexMeilleur = composantesConnexes.size(); }
        composantesConnexes.push_back(tmpComposante);
    }
    std::vector<int> plusGrandeComposante;
    for (const int& idNoeud : composantesConnexes[indexMeilleur]) {
        plusGrandeComposante.push_back(idNoeud);
    }
    return plusGrandeComposante;
}

void Graphe::generateActivationGrid(int gWidth,int gHeight,int etape) {
    gridWidth = gWidth;
    gridHeight = gHeight;
    activationGrid.clear();
    for (int i=0;i<=etape;i++) {
        std::vector<std::pair<int,int>> tmpVec;
        activationGrid.push_back(tmpVec);
    }
    for (int y=0;y<gHeight;y++) {
        for (int x=0;x<gWidth;x++) {
            int randNum = generateRand(etape);
            std::pair<int,int> tmpPair(x,y);
            activationGrid[randNum].push_back(tmpPair);
        }
    }
}

// Ajoute les emplacement dont l'index d'activation est egal a l'index en parametre
void Graphe::activateSlotsGrid(int index) {
    int indexEmplacement = _emplacements.size();
    for (const std::pair<int,int>& coord : activationGrid[index]) {
        _emplacements.push_back(Emplacement(coord.first,coord.second,indexEmplacement));
        indexEmplacement++;
    }
}

// Ajoute les emplacement dont l'index d'activation est inferieur ou egal a l'index en parametre
void Graphe::activateSlotsGridUntil(int index) {
    for (int i=0;i<=index;i++) {
        activateSlotsGrid(i);
    }
}

// Retourne la distance sans la racine
double Graphe::distanceReel(std::pair<double,double>& randCoord,std::pair<double,double>& nodeCoord) {
    double xDiff = randCoord.first - nodeCoord.first;
    double yDiff = randCoord.second - nodeCoord.second;
    return xDiff * xDiff + yDiff * yDiff;
}

// Retourne la distance sans la racine
double Graphe::distanceReelSqrt(std::pair<double,double>& randCoord,std::pair<double,double>& nodeCoord) {
    double xDiff = randCoord.first - nodeCoord.first;
    double yDiff = randCoord.second - nodeCoord.second;
    return sqrt(xDiff * xDiff + yDiff * yDiff);
}

void Graphe::initCompleteGraph(int n, bool setup) {
    for (int i=0;i<n;i++) {
        _noeuds.push_back(Noeud(i));
    }
    int nbArete = 0;
    for (int i=0;i<_noeuds.size()-1;i++) {
        for (int j=i+1;j<_noeuds.size();j++) {
            _aretes.push_back(Aretes(&_noeuds[i],&_noeuds[j],nbArete));
            nbArete++;
        }
    }
    if (setup) {
        int nbNoeud = std::min((int)_noeuds.size()*2,6000);
	    generateGrid(nbNoeud,nbNoeud);
    }
}

std::pair<double,double> Graphe::sizeOfGraphe() {
    if (useCoordReel) {
        double minX = _noeuds[0]._xreel;
        double maxX = minX;
        double minY = _noeuds[0]._yreel;
        double maxY = minY;
        for (int i=1;i<_noeuds.size();i++) {
            if (_noeuds[i]._xreel < minX) { minX = _noeuds[i]._xreel; }
            else if (_noeuds[i]._xreel > maxX) { maxX = _noeuds[i]._xreel; }
            if (_noeuds[i]._yreel < minY) { minY = _noeuds[i]._yreel; }
            else if (_noeuds[i]._yreel > maxY) { maxY = _noeuds[i]._yreel; }
        }
        double sizeX = maxX - minX;
        double sizeY = maxY - minY;
        return std::make_pair(sizeX,sizeY);
    }
    return std::make_pair(-1.0,-1.0);
}

int Graphe::creationNoeudTemporaire(int nodeId, std::pair<double,double>& coord) {
    int idTmpNode = _noeuds.size();
    _noeuds.push_back(Noeud(idTmpNode));
    _noeuds[idTmpNode]._xreel = coord.first;
    _noeuds[idTmpNode]._yreel = coord.second;
    int areteIdNew = _aretes.size();
    for (int i=0;i<_noeuds[nodeId].voisins.size();i++) {
        int voisinId = _noeuds[nodeId].voisins[i]->_id;
        _aretes.push_back(Aretes(&_noeuds[idTmpNode],&_noeuds[voisinId],areteIdNew));
        _aretes[areteIdNew].typeArrete = 2;
        areteIdNew++;
    }
    for (const int& areteId : _noeuds[nodeId]._aretes) {
        _aretes[areteId].typeArrete = 1;
    }
    bool useGrille = grillePtr.size() > 0;
    if (useGrille) { 
        _noeuds[idTmpNode].idCelluleVec = new std::vector<int>();
        registerNodeInGrid(idTmpNode);
        for (const int& areteId : _noeuds[idTmpNode]._aretes) {
            registerEdgeInGridReel(areteId);
        }
    }
    return idTmpNode;
}

int Graphe::creationNoeudTemporaireThread(int nodeId, std::pair<double,double>& coord, int tid) {
    mutexNoeud->lock();
    int idTmpNode = _noeuds.size();
    _noeuds.push_back(Noeud(idTmpNode));
    mutexNoeud->unlock();
    _noeuds[idTmpNode]._xreel = coord.first;
    _noeuds[idTmpNode]._yreel = coord.second;
    mutexAretes->lock();
    int areteIdNew = _aretes.size();
    for (int i=0;i<_noeuds[nodeId].voisins.size();i++) {
        int voisinId = _noeuds[nodeId].voisins[i]->_id;
        _aretes.push_back(Aretes(&_noeuds[idTmpNode],&_noeuds[voisinId],areteIdNew));
        _aretes[areteIdNew].typeArrete = tid;
        areteIdNew++;
    }
    mutexAretes->unlock();
    for (const int& areteId : _noeuds[nodeId]._aretes) {
        _aretes[areteId].typeArrete = -1;
    }
    bool useGrille = grillePtr.size() > 0;
    if (useGrille) { 
        _noeuds[idTmpNode].idCelluleVec = new std::vector<int>();
        registerNodeInGrid(idTmpNode);
        for (const int& areteId : _noeuds[idTmpNode]._aretes) {
            registerEdgeInGridReel(areteId);
        }
    }
    return idTmpNode;
}

void Graphe::supprimerNoeudTemporaire(int copyNodeId) {
    int idNodeSuppr = _noeuds.size()-1;
    bool useGrille = grillePtr.size() > 0;
    if (useGrille) {
        for (const int& areteId : _noeuds[idNodeSuppr]._aretes) {
            for (const int& celluleId : _aretes[areteId].vecIdCellules) {
                grillePtr[celluleId]->vecAreteId.pop_back();
            }
        }
        free(_noeuds[idNodeSuppr].idCelluleVec);
    }
    for (int i=0;i<_noeuds[idNodeSuppr].voisins.size();i++) {
        int idVoisin = _noeuds[idNodeSuppr].voisins[i]->_id;
        _noeuds[idVoisin].voisins.pop_back();
        _noeuds[idVoisin]._aretes.pop_back();
        _aretes.pop_back();
    }
    _noeuds.pop_back();
    for (const int& areteId : _noeuds[copyNodeId]._aretes) {
        _aretes[areteId].typeArrete = 0;
    }
}

void Graphe::supprimerNoeudTemporaireThread(int copyNodeId, int tid) {
    int idNodeSuppr = _noeuds.size()-tid;
    bool useGrille = grillePtr.size() > 0;
    if (useGrille) {
        for (const int& areteId : _noeuds[idNodeSuppr]._aretes) {
            for (const int& celluleId : _aretes[areteId].vecIdCellules) {
                grillePtr[celluleId]->mutexArete->lock();
                grillePtr[celluleId]->vecAreteId.pop_back();
                grillePtr[celluleId]->mutexArete->unlock();
            }
        }
        free(_noeuds[idNodeSuppr].idCelluleVec);
    }
    for (int i=0;i<_noeuds[idNodeSuppr].voisins.size();i++) {
        int idVoisin = _noeuds[idNodeSuppr].voisins[i]->_id;
        _noeuds[idVoisin].mutexAreteVoisin->lock();
        _noeuds[idVoisin].voisins.pop_back();
        _noeuds[idVoisin]._aretes.pop_back();
        _noeuds[idVoisin].mutexAreteVoisin->unlock();
    }
    for (const int& areteId : _noeuds[copyNodeId]._aretes) {
        _aretes[areteId].typeArrete = 0;
    }
}

void Graphe::replaceNoeudTemporaire(int nodeId) {
    int lastNodeIndex = _noeuds.size() - 1;
    _noeuds[nodeId]._xreel = _noeuds[lastNodeIndex]._xreel;
    _noeuds[nodeId]._yreel = _noeuds[lastNodeIndex]._yreel;
    supprimerNoeudTemporaire(nodeId);
    recalcNodeCelluleReel(nodeId);
}

void Graphe::resizeVectorTemporaire(int nodeId, int nbNodeTemporaire) {
    _noeuds.erase(_noeuds.end() - nbNodeTemporaire, _noeuds.end());
    _aretes.erase(_aretes.end() - (_noeuds[nodeId]._aretes.size()*nbNodeTemporaire), _aretes.end());
}

void Graphe::fillCommonNodeVectors() {
    commonNodeEdges = new std::vector<std::vector<int>>();
    commonNodeEdges->reserve(_aretes.size());
    for (int i=0;i<_aretes.size();i++) {
        std::vector<int> tmpVec(_aretes.size(),-1);
        commonNodeEdges->push_back(tmpVec);
    }
    for (int i=0;i<_noeuds.size();i++) {
        for (int j=0;j<(int)_noeuds[i].voisins.size()-1;j++) {
            Noeud* n1 = _noeuds[i].voisins[j];
            int index1 = _noeuds[i]._aretes[j];
            for (int k=j+1;k<_noeuds[i].voisins.size();k++) {
                Noeud* n2 = _noeuds[i].voisins[k];
                int index2 = _noeuds[i]._aretes[k];
                (*commonNodeEdges)[index1][index2] = n1->getId();
                (*commonNodeEdges)[index2][index1] = n2->getId();
            }
        }
    }
}

void Graphe::fillCommonNodeVectorsGenetique(std::vector<std::vector<int>>& commonNodeEdgesGenetique) {
    commonNodeEdgesGenetique.reserve(_aretes.size());
    for (int i=0;i<_aretes.size();i++) {
        std::vector<int> tmpVec(_aretes.size(),-1);
        tmpVec.reserve(_aretes.size());
        commonNodeEdgesGenetique.push_back(tmpVec);
    }
    for (int i=0;i<_noeuds.size();i++) {
        for (int j=0;j<_noeuds[i].voisins.size()-1;j++) {
            Noeud* n1 = _noeuds[i].voisins[j];
            int index1 = _noeuds[i]._aretes[j];
            for (int k=j+1;k<_noeuds[i].voisins.size();k++) {
                Noeud* n2 = _noeuds[i].voisins[k];
                int index2 = _noeuds[i]._aretes[k];
                commonNodeEdgesGenetique[index1][index2] = n1->getId();
                commonNodeEdgesGenetique[index2][index1] = n2->getId();
            }
        }
    }
}

void Graphe::addCommonNodeVector(int nodeId) {
    for (int j=0;j<_noeuds[nodeId].voisins.size()-1;j++) {
        Noeud* n1 = _noeuds[nodeId].voisins[j];
        int index1 = _noeuds[nodeId]._aretes[j];
        for (int k=j+1;k<_noeuds[nodeId].voisins.size();k++) {
            Noeud* n2 = _noeuds[nodeId].voisins[k];
            int index2 = _noeuds[nodeId]._aretes[k];
            (*commonNodeEdges)[index1][index2] = n1->getId();
            (*commonNodeEdges)[index2][index1] = n2->getId();
        }
    }
}

void Graphe::removeCommonNodeVector(int nodeId) {
    for (const int& index1 : _noeuds[nodeId]._aretes) {
        for (const int& index2 : _noeuds[nodeId]._aretes) {
            (*commonNodeEdges)[index1][index2] = -1;
            (*commonNodeEdges)[index2][index1] = -1;
        }
    }
}

void Graphe::printCommonMatrix() {
    for (int i=0;i<commonNodeEdges->size();i++) {
        for (int j=0;j<commonNodeEdges->size();j++) {
            if ((*commonNodeEdges)[i][j] == -1) { tcout() << "X "; }
            else { tcout() << (*commonNodeEdges)[i][j] << " "; }
        }
        tcout() << std::endl;
    }
}

void Graphe::setupGridAndRegistration(std::vector<std::vector<double>> customParam) {
#if defined(DEBUG_GRAPHE_PROGRESS)
    tcout() << "Setup Cellules.\n";
#endif
    int row = (int)ceil(sqrt(_aretes.size())*1.5);
    if (customParam.size() > 0) {
        for (std::vector<double>& param : customParam) {
            if (param.size() > 0) {
                if (param[0] == 10) { row = max(1,(int)(ceil(sqrt(_noeuds.size()))*param[1])); }
                else if (param[0] == 11) { row = max(1,(int)(ceil(sqrt(_aretes.size()))*param[1])); }
            }
        }
    }
    if (useCoordReel) { initGrilleReel(row,row); registerNodesAndEdgesInGrid(); }
    else { 
        initGrille(row,row); 
        registerSlotsAndEdgesInGrid(); 
        registerIsolatedNodesInGrid();
    }
}

void resetKRegular(std::vector<std::vector<int>>& voisinage, std::vector<int>& values, int valuesSize) {
    printf("Reset kregular\n");
    for (std::vector<int>& noeudVoisinage : voisinage) {
        noeudVoisinage.clear();
    }
    values.clear();
    values.resize(valuesSize);
    for (int i=0;i<valuesSize;i++) {
        values[i] = i;
    }
}

bool isEdgeSuitable(int firstNodeId, int secondNodeId, std::vector<int>& values, std::vector<std::vector<int>>& voisinage) {
    if (firstNodeId==secondNodeId) { return false; }

    for (const int& voisinId : voisinage[firstNodeId]) {
        if (voisinId==secondNodeId) { return false; }
    }

    for (const int& voisinId : voisinage[secondNodeId]) {
        if (voisinId==firstNodeId) { return false; }
    }
    return true;
}

bool checkSuitablePair(std::vector<int>& values, std::vector<std::vector<int>>& voisinage, int nbNoeud) {
    for (int i=0;i<values.size()-1;i++) {
        for (int j=i+1;j<values.size();j++) {
            int firstNodeId = values[i] % nbNoeud;
            int secondNodeId = values[j] % nbNoeud;
            if (isEdgeSuitable(firstNodeId,secondNodeId,values,voisinage)) { return true; }
        }
    }
    return false;
}

void connectIfSuitable(int firstNodeRandom, int secondNodeRandom, std::vector<int>& values, std::vector<std::vector<int>>& voisinage, int nbNoeud) {
    int firstNodeId = values[firstNodeRandom] % nbNoeud;
    int secondNodeId = values[secondNodeRandom] % nbNoeud;
    if (!isEdgeSuitable(firstNodeId,secondNodeId,values,voisinage)) { return; }

    voisinage[firstNodeId].push_back(secondNodeId);
    voisinage[secondNodeId].push_back(firstNodeId);

    int higherIndex = max(firstNodeRandom,secondNodeRandom);
    int lowerIndex = min(firstNodeRandom,secondNodeRandom);
    values[higherIndex] = values[values.size()-1];
    values.pop_back();
    values[lowerIndex] = values[values.size()-1];
    values.pop_back();
}

void Graphe::updateIsolatedNodes() {
    _noeudsSeuls.clear();
    for (int i=0;i<_noeuds.size();i++) {
        if (_noeuds[i].voisins.size() == 0) { _noeudsSeuls.push_back(i); }
    }
}

void Graphe::generateKRegular(int nbNoeud, int degre) {
    int valueSize = nbNoeud*degre;
    if (valueSize%2 == 1) {
        tcout() << "NbNoeud * degre doit être pair.\n";
        exit(3);
    }
    _noeuds.reserve(nbNoeud*2);
    for (int i=0;i<nbNoeud;i++) {
        _noeuds.push_back(Noeud(i));
    }
    _aretes.reserve(valueSize);

    std::vector<std::vector<int>> voisinage;
    voisinage.resize(nbNoeud);
    for (int i=0;i<nbNoeud;i++) {
        voisinage[i].reserve(degre);
    }

    std::vector<int> values(valueSize);
    for (int i=0;i<valueSize;i++) {
        values[i]=i;
    }
    bool kregular_found = false;
    while (!kregular_found) {
        int firstNodeRandom = generateRand(values.size()-1);
        int secondNodeRandom = generateRand(values.size()-1);
        connectIfSuitable(firstNodeRandom,secondNodeRandom,values,voisinage,nbNoeud);
        if (values.size() == 0) { kregular_found = true; }
        else if (!checkSuitablePair(values,voisinage,nbNoeud)) { resetKRegular(voisinage,values,valueSize); }
    }

    int nbArete = 0;
    for (int nodeId=0;nodeId<nbNoeud;nodeId++) {
        for (const int& voisinId : voisinage[nodeId]) {
            if (voisinId < nodeId) {
                _aretes.push_back(Aretes(&_noeuds[nodeId],&_noeuds[voisinId],nbArete));
                nbArete++;
            }
        }
    }

}

void Graphe::fillWithSingleNodes() {
    int nombreNoeud = _noeuds.size();
    int nombreEmplacement = _emplacements.size();
    while (nombreNoeud < nombreEmplacement) {
        _noeuds.push_back(Noeud(nombreNoeud));
        nombreNoeud++;
    }
    updateIsolatedNodes();
    updatePenalite(_noeuds.size(),_noeuds.size());
}

bool equalWithEpsilon(double a, double b, double eps) {
    return ((a+eps > b)&&(a-eps < b));
}

bool Graphe::checkNodeAlignementHorizontal(int nodeId) {
    if (_noeuds[nodeId].idCelluleVec->size() <= 1) return false;
    for (const int& celluleId : *_noeuds[nodeId].idCelluleVec) {
        double n1Y = grillePtr[celluleId]->getBottomLeftY();
        double n2Y = grillePtr[celluleId]->getTopLeftY();
        if ((!equalWithEpsilon(_noeuds[nodeId]._yreel,n1Y,epsilon))&&(!equalWithEpsilon(_noeuds[nodeId]._yreel,n2Y,epsilon))) return false;
    }
    return true;
}

bool Graphe::checkNodeAlignementVertical(int nodeId) {
    if (_noeuds[nodeId].idCelluleVec->size() <= 1) return false;
    for (const int& celluleId : *_noeuds[nodeId].idCelluleVec) {
        double n1X = grillePtr[celluleId]->getBottomLeftX();
        double n2X = grillePtr[celluleId]->getBottomRightX();
        if ((!equalWithEpsilon(_noeuds[nodeId]._xreel,n1X,epsilon))&&(!equalWithEpsilon(_noeuds[nodeId]._xreel,n2X,epsilon))) return false;
    }
    return true;
}