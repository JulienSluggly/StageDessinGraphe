#include "graphe.hpp"
#include "intersection.hpp"
#include "geometrie.hpp"

// Calcule le score d'intersection du graphe et le met a jour.
long Graphe::getNbCroisement() {
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
long Graphe::getNbCroisementGlouton() {
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
long Graphe::getNbCroisementGloutonScore(int nodeId) {
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

// Calcule le score du noeud en fonction de ses vecteur d'intersections
long Graphe::getScoreCroisementNodeFromArrays(int nodeIndex) {
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
long Graphe::getNodeScoreEnfant(Graphe& G, std::vector<int>& areteVec, int nodeIndex) {
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
long Graphe::getScoreCroisementNode(int nodeIndex) {
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
int Graphe::getScoreCroisementNode(int nodeIndex, int swapIndex) {
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

// Indique aux aretes du noeuds que leur liste d'intersection n'est pas à jours.
void Graphe::changeUpdateValue(int idNode) {
    for (int i=0;i<_noeuds[idNode]._aretes.size();i++) {
        _liens[_noeuds[idNode]._aretes[i]].isUpdated = false;
    }
}

// Recalcule les intersections d'une arete et met a jour sa liste d'intersection uniquement
void Graphe::recalculateInterArray(int idArray) {
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
void Graphe::updateNodeScore(int idNode) {
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

// Ne modifie pas le score
long Graphe::getNbCroisementConst() const {
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
long Graphe::getNbCroisementArray() {
    long total = 0;
    for (int i=0;i<_liens.size();i++) {
        total += _liens[i].intersections.size();
        total += _liens[i].intersectionsIll.size() * PENALITE_MAX;
        total += _liens[i].intersectionsIllSelf.size() * PENALITE_MAX_SELF;
    }
    return total/2;
}

// Met a jour le score du graphe et de ses noeuds ainsi que les vecteurs d'intersections des aretes
void Graphe::initGraphAndNodeScoresAndCrossings() {
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

// Renvoie le score du noeud nodeId1 sans prendre en compte le score d'intersection avec le noeud nodeId2
long Graphe::getNodeLinkedScore(int nodeId1, int nodeId2) {
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
