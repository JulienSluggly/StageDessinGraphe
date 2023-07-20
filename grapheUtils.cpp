#include "graphe.hpp"
#include "intersection.hpp"
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
    return false;
}

// Indique si tout les noeuds d'un graphe sont place
bool Graphe::estPlace() {
    if (useCoordReel) {
        for (int i=0;i<_noeuds.size();i++) {
            if ((_noeuds[i]._xreel == -12345)||(_noeuds[i]._yreel == -12345)) return false;
        }
    }
    return true;
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

void Graphe::reinitGrilleReel() {
    clearGrilleReel();
    registerNodesAndEdgesInGrid();
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
    grille_with_move = false;
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

void Graphe::registerNodesAndEdgesInGrid() {
    registerNodesInGrid();
    registerEdgesInGridReel();
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
        grillePtr[idCellule]->vecAreteId.push_back(i);
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
    int minSize = std::min(vecCellId.size(),_aretes[areteId].vecIdCellules.size());
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

void Graphe::recalcNodeCelluleReel(int nodeId) {
    recalcSpecificNodeCell(nodeId);
    for (const int& areteId : _noeuds[nodeId]._aretes) {
        recalcAreteCelluleReel(areteId);
    }
}

void Graphe::applyNewAreteCelluleVec(std::vector<std::vector<int>>& vecId, int nodeIndex) {
    for (int numArete=0;numArete<vecId.size();numArete++) {
        std::vector<int>& vecCellId = vecId[numArete];
        int areteId = _noeuds[nodeIndex]._aretes[numArete];
        int sameUntil;
        int minSize = std::min(vecCellId.size(),_aretes[areteId].vecIdCellules.size());
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

void Graphe::setupGrapheReel(std::string fileGraphe) {
    std::string pathGraph = fileGraphe;
    if (!containsString(fileGraphe,chemin)) {
        pathGraph = chemin + "exemple/Graphe/" + fileGraphe + ".json";
    }
    readFromJsonGraph(pathGraph);
    updateIsolatedNodes();
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
                if (param[0] == 10) { row = std::max(1,(int)(ceil(sqrt(_noeuds.size()))*param[1])); }
                else if (param[0] == 11) { row = std::max(1,(int)(ceil(sqrt(_aretes.size()))*param[1])); }
            }
        }
    }
    if (useCoordReel) { initGrilleReel(row,row); registerNodesAndEdgesInGrid(); }
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

    int higherIndex = std::max(firstNodeRandom,secondNodeRandom);
    int lowerIndex = std::min(firstNodeRandom,secondNodeRandom);
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

bool equalWithEpsilon(double a, double b, double eps) {
    return ((a+eps > b)&&(a-eps < b));
}

bool Graphe::checkNodeAlignementHorizontal(int nodeId) {
    if (_noeuds[nodeId].idCelluleVec->size() <= 1) return false;
    for (const int& celluleId : *_noeuds[nodeId].idCelluleVec) {
        double n1Y = grillePtr[celluleId]->getBottomLeftYReel();
        double n2Y = grillePtr[celluleId]->getTopLeftYReel();
        if ((!equalWithEpsilon(_noeuds[nodeId]._yreel,n1Y,epsilon))&&(!equalWithEpsilon(_noeuds[nodeId]._yreel,n2Y,epsilon))) return false;
    }
    return true;
}

bool Graphe::checkNodeAlignementVertical(int nodeId) {
    if (_noeuds[nodeId].idCelluleVec->size() <= 1) return false;
    for (const int& celluleId : *_noeuds[nodeId].idCelluleVec) {
        double n1X = grillePtr[celluleId]->getBottomLeftXReel();
        double n2X = grillePtr[celluleId]->getBottomRightXReel();
        if ((!equalWithEpsilon(_noeuds[nodeId]._xreel,n1X,epsilon))&&(!equalWithEpsilon(_noeuds[nodeId]._xreel,n2X,epsilon))) return false;
    }
    return true;
}