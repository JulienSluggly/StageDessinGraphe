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
    for (int i = 0; i < _aretes.size() - 1; ++i) {
        for (int j = i + 1; j < _aretes.size(); ++j) {
            //Aretes aretes1 = _aretes[i], aretes2 = _aretes[j];
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
    if (n * _emplacements.size() > nbTotal) {
        if (DEBUG_GRAPHE) std::cout << "Pas assez de place dans la grille. Grille: " << nbTotal << " " << n << " * emp: " << n * _emplacements.size() << std::endl;
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
        gridWidth = _noeuds.size();
        gridHeight = _noeuds.size();
        nbEmplacement = _noeuds.size();
    }
    else {
        int nbTotal = gridWidth * gridHeight;
        if (n + _emplacements.size() > nbTotal) {
            if (DEBUG_GRAPHE) std::cout << "Pas assez de place dans la grille. Grille: " << nbTotal << " " << n << " * emp: " << n * _emplacements.size() << std::endl;
            return;
        }
    }

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
    if (DEBUG_GRAPHE) std::cout << "Debut triangulation.\n";
    std::vector<Emplacement*> empPtrVec;
    for (int i=0;i<_emplacements.size();i++) {
        empPtrVec.push_back(&_emplacements[i]);
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
    if (DEBUG_GRAPHE) std::cout << "Triangulation delaunay fini.\n";
}

void Graphe::reinitGrille() {
    clearGrille();
    registerEdgesInGrid();
}

void Graphe::clearGrille() {
    for (int i=0;i<grillePtr.size();i++) {
        grillePtr[i]->vecAreteId.clear();
    }
    for (int i=0;i<_aretes.size();i++) {
        _aretes[i].vecIdCellules.clear();
    }
}

void Graphe::initGrille(int row,int column,bool decalleGrille) {
    if (decalleGrille) {
        for (int i=0;i<_emplacements.size();i++) {
            _emplacements[i]._x = (_emplacements[i]._x*2)+2;
            _emplacements[i]._y = (_emplacements[i]._y*2)+2;
        }
        gridWidth = (gridWidth * 2) + 2;
        gridHeight = (gridHeight * 2) + 2;
    }
    if (row == -1) { row = (int)ceil(sqrt(_aretes.size())*1.5); }
    if (column == -1) { column = row; }
    int sizeColumn = ceil((double)gridWidth / (double)column);
    if (sizeColumn % 2 == 1) { sizeColumn++; }
    int sizeRow = ceil((double)gridHeight / (double)row);
    if (sizeRow % 2 == 1) { sizeRow++; }
    for (int y=0;y<row;y++) {
        std::vector<Cellule> tmpVec;
        for (int x=0;x<column;x++) {
            int id = y*sizeColumn + x;
            int x1 = x*sizeColumn + 1;
            int x2 = x1+sizeColumn;
            int y1 = y*sizeRow + 1;
            int y2 = y1+sizeRow;
            tmpVec.push_back(Cellule(id,x,y,x1,y2,x2,y1));
        }
        grille.push_back(tmpVec);
    }

    for (int i=0;i<grille.size();i++) {
        for (int j=0;j<grille[0].size();j++) {
            grillePtr.push_back(&grille[i][j]);
        }
    }

}

// Pas utilisé
void Graphe::registerSlotsAndEdgesInGridNoMove() {
    int sizeColumn = grille[0][0].getBottomRightX() - grille[0][0].getBottomLeftX();
    int sizeRow = grille[0][0].getTopLeftY() - grille[0][0].getBottomLeftY();
    for (int i=0;i < _emplacements.size();i++) {
        int x = _emplacements[i].getX();
        int y = _emplacements[i].getY();
        double dnumX = (double)x/(double)sizeColumn;
        double dnumY = (double)y/(double)sizeRow;
        int numX = floor(dnumX);
        int numY = floor(dnumY);

        double epsilon = 0.00001;
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
                        //_emplacements[i].vecIdCellules.push_back(id);
                        grille[numY2][numX].vecEmplacementId.push_back(i);
                    }
                    if ((numX2 >= 0)&&(numX2 < grille[0].size())) { // Ajout numY2 et numX2
                        id = numY2 * grille[0].size() + numX2;
                        //_emplacements[i].vecIdCellules.push_back(id);
                        grille[numY2][numX2].vecEmplacementId.push_back(i);
                    }
                }
            }
            if ((numX2 >= 0)&&(numX2 < grille[0].size())&&numY < grille.size()) { // Ajout numY et numX2
                id = numY * grille[0].size() + numX2;
                //_emplacements[i].vecIdCellules.push_back(id);
                grille[numY][numX2].vecEmplacementId.push_back(i);
            }
        }
        else if (numY2 != numY) { // Cas juste aligné sur segment horizontal de la grille
            if ((numY2 >= 0)&&(numY2 < grille.size())&&numX < grille[0].size()) { // Ajout numY2 & numX
                id = numY2 * grille[0].size() + numX;
                //_emplacements[i].vecIdCellules.push_back(id);
                grille[numY2][numX].vecEmplacementId.push_back(i);
            }
        }
        // Ajout numY & numX
        if (numY < grille.size() && numX < grille[0].size()) {
            id = numY * grille[0].size() + numX;
            //_emplacements[i].vecIdCellules.push_back(id);
            grille[numY][numX].vecEmplacementId.push_back(i);
        }
    }

    for (int i=0;i<_aretes.size();i++) {
        int direction = getDirectionArete(i);

        // A faire si besoin
    }
}

void Graphe::registerSlotsAndEdgesInGrid() {
    if (DEBUG_GRAPHE) std::cout << "Remplissage de la grille.\n";
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
    }
    registerEdgesInGrid();
    if (DEBUG_GRAPHE) std::cout << "Fin remplissage de la grille.\n";
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
            int idCellX = idCell % grille[0].size();
            int idCellY = idCell / grille[0].size();
            _aretes[i].vecIdCellules.push_back(idCell);
            grille[idCellY][idCellX].vecAreteId.push_back(i); 
            while (idCell != idCellArrive) {
            switch(direction) {
                case 0:
                    idCellX++;
                    idCell++;
                    break;
                case 1: {
                    int cellX = grillePtr[idCell]->getTopRightX();
                    int cellY = grillePtr[idCell]->getTopRightY();
                    int alignement = aGaucheInt(n1X,n1Y,n2X,n2Y,cellX,cellY);
                    if (alignement == 1) {
                        idCellX++;
                        idCell++;
                    }
                    else if (alignement == -1) {
                        idCellY++;
                        idCell += nombreColonne;
                    }
                    else {
                        _aretes[i].vecIdCellules.push_back(idCell+nombreColonne);
                        grille[idCellY+1][idCellX].vecAreteId.push_back(i);
                        _aretes[i].vecIdCellules.push_back(idCell+1);
                        grille[idCellY][idCellX+1].vecAreteId.push_back(i);
                        idCell = idCell + nombreColonne + 1;
                        idCellX++;
                        idCellY++;
                    }
                    break;
                }
                case 2:
                    idCellY++;
                    idCell += nombreColonne;
                    break;
                case 3: {
                    int cellX = grillePtr[idCell]->getTopLeftX();
                    int cellY = grillePtr[idCell]->getTopLeftY();
                    int alignement = aGaucheInt(n1X,n1Y,n2X,n2Y,cellX,cellY);
                    if (alignement == 1) {
                        idCellY++;
                        idCell += nombreColonne;
                    }
                    else if (alignement == -1) {
                        idCellX--;
                        idCell--;
                    }
                    else {
                        _aretes[i].vecIdCellules.push_back(idCell+nombreColonne);
                        grille[idCellY+1][idCellX].vecAreteId.push_back(i);
                        _aretes[i].vecIdCellules.push_back(idCell-1);
                        grille[idCellY][idCellX-1].vecAreteId.push_back(i);
                        idCell = idCell + nombreColonne - 1;
                        idCellX--;
                        idCellY++;
                    }
                    break;
                }
                case 4:
                    idCellX--;
                    idCell--;
                    break;
                case 5: {
                    int cellX = grillePtr[idCell]->getBottomLeftX();
                    int cellY = grillePtr[idCell]->getBottomLeftY();
                    int alignement = aGaucheInt(n1X,n1Y,n2X,n2Y,cellX,cellY);
                    if (alignement == 1) {
                        idCellX--;
                        idCell--;
                    }
                    else if (alignement == -1) {
                        idCellY--;
                        idCell -= nombreColonne;
                    }
                    else {
                        _aretes[i].vecIdCellules.push_back(idCell-nombreColonne);
                        grille[idCellY-1][idCellX].vecAreteId.push_back(i);
                        _aretes[i].vecIdCellules.push_back(idCell-1);
                        grille[idCellY][idCellX-1].vecAreteId.push_back(i);
                        idCell = idCell - nombreColonne - 1;
                        idCellX--;
                        idCellY--;
                    }
                    break;
                }
                case 6:
                    idCellY--;
                    idCell -= grille[0].size();
                    break;
                case 7: {
                    int cellX = grillePtr[idCell]->getBottomRightX();
                    int cellY = grillePtr[idCell]->getBottomRightY();
                    int alignement = aGaucheInt(n1X,n1Y,n2X,n2Y,cellX,cellY);
                    if (alignement == 1) {
                        idCellY--;
                        idCell -= nombreColonne;
                    }
                    else if (alignement == -1) {
                        idCellX++;
                        idCell++;
                    }
                    else {
                        _aretes[i].vecIdCellules.push_back(idCell-nombreColonne);
                        grille[idCellY-1][idCellX].vecAreteId.push_back(i);
                        _aretes[i].vecIdCellules.push_back(idCell+1);
                        grille[idCellY][idCellX+1].vecAreteId.push_back(i);
                        idCell = idCell - nombreColonne + 1;
                        idCellX++;
                        idCellY--;
                    }
                    break;
                }
            }
            _aretes[i].vecIdCellules.push_back(idCell);
            grille[idCellY][idCellX].vecAreteId.push_back(i);
        }
        }
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
        removeFromVector(grillePtr[_aretes[areteId].vecIdCellules[i]]->vecAreteId,areteId);
    }
    for (int i=sameUntil;i<vecCellId.size();i++) {
        if (!isInVector(grillePtr[vecCellId[i]]->vecAreteId,areteId)) {
            grillePtr[vecCellId[i]]->vecAreteId.push_back(areteId);
        }
    }
    _aretes[areteId].vecIdCellules.swap(vecCellId);
}

void Graphe::recalcNodeCellule(int nodeId) {
    for (const int& areteId : _noeuds[nodeId]._aretes) {
        recalcAreteCellule(areteId);
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

void Graphe::setupGraphe(std::string fileGraphe, std::string fileSlot) {
    std::string pathGraph = chemin + "exemple/Graphe/" + fileGraphe + ".json";
    std::string pathSlot;
    readFromJsonGraph(pathGraph);
    if ((fileSlot != "GRID")&&(fileSlot != "Grid")) {
        pathSlot = chemin + "exemple/Slots/" + fileSlot + ".json";
        readFromJsonSlots(pathSlot);
    }
    else {
        generateGrid();
    }
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
    double minX, minY;
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