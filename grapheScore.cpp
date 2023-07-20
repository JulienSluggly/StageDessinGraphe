#include "graphe.hpp"
#include "intersection.hpp"
#include <iostream>
#include <limits.h>

void Graphe::recalculateIllegalIntersectionsReel() {
    for (int i = 0; i < _aretes.size() - 1; ++i) {
        for (int j = i + 1; j < _aretes.size(); ++j) {
            if ((*commonNodeEdges)[i][j] == -1) {
                bool isIllegal = false;
                if (seCroisentReel(_aretes[i], _aretes[j],isIllegal)) {
                    if (isIllegal) {
                        areteIll.insert(&_aretes[i]);
                        areteIll.insert(&_aretes[j]);
                    }
                    else {
                        areteInter.insert(&_aretes[i]);
                        areteInter.insert(&_aretes[j]);
                    }
                }
            }
            else {
                if (surSegmentReel(_aretes[i], _noeuds[(*commonNodeEdges)[j][i]])) {
                    areteIllSelf.insert(&_aretes[i]);
                    areteIllSelf.insert(&_aretes[j]);
                }
                else if (surSegmentReel(_aretes[j], _noeuds[(*commonNodeEdges)[i][j]])) { 
                    areteIllSelf.insert(&_aretes[i]);
                    areteIllSelf.insert(&_aretes[j]);
                }
            }
        }
    }
}

// Calcule le score d'intersection du graphe et le met a jour.
long Graphe::getNbCroisementReel() {
    long total = 0;
    for (int i = 0; i < _aretes.size(); ++i) {
        for (int j = i + 1; j < _aretes.size(); ++j) {
            if ((*commonNodeEdges)[i][j] == -1) {
                bool isIllegal = false;
                if (seCroisentReel(_aretes[i], _aretes[j],isIllegal)) {
                    if (isIllegal) { total += PENALITE_MAX; }
                    else { ++total; }
                }
            }
            else {
                if (surSegmentReel(_aretes[i], _noeuds[(*commonNodeEdges)[j][i]])) { total += PENALITE_MAX_SELF; }
                else if (surSegmentReel(_aretes[j], _noeuds[(*commonNodeEdges)[i][j]])) { total += PENALITE_MAX_SELF; }
            }
        }
        for (int k=0;k<_noeudsSeuls.size();k++) {
            if (surSegmentReel(_aretes[i],_noeuds[_noeudsSeuls[k]])) { total += PENALITE_MAX; }
        }
    }
    nombreCroisement = total;
    isNombreCroisementUpdated = true;
    return total;
}

// Calcule le score du noeud en parametre.
long Graphe::getScoreCroisementNodeReel(int nodeIndex) {
    long score = 0;
    std::vector<bool> indexPasse(_aretes.size(),false);
    for (const int& index : _noeuds[nodeIndex]._aretes) {
        for (int j = 0; j < _aretes.size(); ++j) {
            if ((index != j) && (!indexPasse[j])) {
                if ((*commonNodeEdges)[index][j] == -1) {
                    bool isIllegal = false;
                    if (seCroisentReel(_aretes[index], _aretes[j],isIllegal)) {
                        if (isIllegal) { score += PENALITE_MAX; }
                        else { score++; }
                    }
                }
                else {
                    if (surSegmentReel(_aretes[index], _noeuds[(*commonNodeEdges)[j][index]])) { score += PENALITE_MAX_SELF; }
                    else if (surSegmentReel(_aretes[j], _noeuds[(*commonNodeEdges)[index][j]])) { score += PENALITE_MAX_SELF; }
                }
            }
        }
        indexPasse[index] = true;
    }
    return score;
}


// Calcule le score du noeud en parametre.
long Graphe::getScoreCroisementNodeGridReel(int nodeIndex) {
    long score = 0;
    std::vector<bool> indexPasse(_aretes.size(),false);
    std::vector<bool> indexPasseCellule(_aretes.size(),false);
    for (const int& index : _noeuds[nodeIndex]._aretes) {
        if (index != _noeuds[nodeIndex]._aretes[0]) { indexPasseCellule.assign(indexPasseCellule.size(), false); }
        for (int j = 0; j < _aretes[index].vecIdCellules.size(); ++j) {
            std::vector<int>& vecId = grillePtr[_aretes[index].vecIdCellules[j]]->vecAreteId;
            for (const int& index2 : vecId) {
                if ((index != index2) && (!indexPasse[index2]) && (!indexPasseCellule[index2])) {
                    if ((*commonNodeEdges)[index][index2] == -1) {
                        bool isIllegal = false;
                        if (seCroisentReel(_aretes[index], _aretes[index2],isIllegal)) {
                            if (isIllegal) { score += PENALITE_MAX; }
                            else { score++; }
                        }
                    }
                    else {
                        if (surSegmentReel(_aretes[index], _noeuds[(*commonNodeEdges)[index2][index]])) { score += PENALITE_MAX_SELF; }
                        else if (surSegmentReel(_aretes[index2], _noeuds[(*commonNodeEdges)[index][index2]])) { score += PENALITE_MAX_SELF; }
                    }
                    indexPasseCellule[index2] = true;
                }
            }
        }
        indexPasse[index] = true;
    }
    return score;
}

// Calcule le score du noeud en parametre.
long Graphe::getScoreCroisementNodeGridReelLimite(int nodeIndex, bool& makeMove, double limiteScore) {
    long score = 0;
    std::vector<bool> indexPasse(_aretes.size(),false);
    std::vector<bool> indexPasseCellule(_aretes.size(),false);
    for (const int& index : _noeuds[nodeIndex]._aretes) {
        if (index != _noeuds[nodeIndex]._aretes[0]) { indexPasseCellule.assign(indexPasseCellule.size(), false); }
        for (int j = 0; j < _aretes[index].vecIdCellules.size(); ++j) {
            std::vector<int>& vecId = grillePtr[_aretes[index].vecIdCellules[j]]->vecAreteId;
            for (const int& index2 : vecId) {
                if ((index != index2) && (!indexPasse[index2]) && (!indexPasseCellule[index2])) {
                    if ((*commonNodeEdges)[index][index2] == -1) {
                        bool isIllegal = false;
                        if (seCroisentReel(_aretes[index], _aretes[index2],isIllegal)) {
                            if (isIllegal) { score += PENALITE_MAX; }
                            else { score++; }
                        }
                    }
                    else {
                        if (surSegmentReel(_aretes[index], _noeuds[(*commonNodeEdges)[index2][index]])) { score += PENALITE_MAX_SELF; }
                        else if (surSegmentReel(_aretes[index2], _noeuds[(*commonNodeEdges)[index][index2]])) { score += PENALITE_MAX_SELF; }
                    }
                    if (score > limiteScore) { return TRES_GRANDE_VALEUR; }
                    indexPasseCellule[index2] = true;
                }
            }
        }
        indexPasse[index] = true;
    }
    makeMove = true;
    return score;
}

// Ne modifie pas le score
long Graphe::getNbCroisementReelConst() const {
    long total = 0;
    for (int i = 0; i < _aretes.size() - 1; ++i) {
        for (int j = i + 1; j < _aretes.size(); ++j) {
            if ((*commonNodeEdges)[i][j] == -1) {
                bool isIllegal = false;
                if (seCroisentReel(_aretes[i], _aretes[j],isIllegal)) {
                    if (isIllegal) { total += PENALITE_MAX; }
                    else { ++total; }
                }
            }
            else {
                if (surSegmentReel(_aretes[i], _noeuds[(*commonNodeEdges)[j][i]])) { total += PENALITE_MAX_SELF; }
                else if (surSegmentReel(_aretes[j], _noeuds[(*commonNodeEdges)[i][j]])) { total += PENALITE_MAX_SELF; }
            }
        }
    }
    return total;
}

// Ne modifie pas le score
long Graphe::getNbCroisementDiffReel() {
    nombreInter = 0;
    nombreInterIll = 0;
    nombreInterIllSelf = 0;
    for (int i = 0; i < _aretes.size() - 1; ++i) {
        for (int j = i + 1; j < _aretes.size(); ++j) {
            if ((*commonNodeEdges)[i][j] == -1) {
                bool isIllegal = false;
                if (seCroisentReel(_aretes[i], _aretes[j],isIllegal)) {
                    if (isIllegal) { nombreInterIll++; }
                    else { nombreInter++; }
                }
            }
            else {
                if (surSegmentReel(_aretes[i], _noeuds[(*commonNodeEdges)[j][i]])) { nombreInterIllSelf++; }
                else if (surSegmentReel(_aretes[j], _noeuds[(*commonNodeEdges)[i][j]])) { nombreInterIllSelf++; }
            }
        }
    }
    return nombreInter+nombreInterIll+nombreInterIllSelf;
}

long Graphe::getNbCroisementGridReel() {
    long score = 0;
    std::vector<bool> indexPasse(_aretes.size(),false);
    std::vector<bool> indexPasseCellule(_aretes.size(),false);
    for (int index = 0; index < _aretes.size() - 1; ++index) {
        if (index != 0) { indexPasseCellule.assign(indexPasseCellule.size(), false); }
        for (int j = 0; j < _aretes[index].vecIdCellules.size(); ++j) {
            std::vector<int>& vecId = grillePtr[_aretes[index].vecIdCellules[j]]->vecAreteId;
            for (const int& index2 : vecId) {
                if ((index != index2) && (!indexPasse[index2]) && (!indexPasseCellule[index2])) {
                    if ((*commonNodeEdges)[index][index2] == -1) {
                        bool isIllegal = false;
                        if (seCroisentReel(_aretes[index], _aretes[index2],isIllegal)) {
                            if (isIllegal) { score += PENALITE_MAX; }
                            else { score++; }
                        }
                    }
                    else {
                        if (surSegmentReel(_aretes[index], _noeuds[(*commonNodeEdges)[index2][index]])) { score += PENALITE_MAX_SELF; }
                        else if (surSegmentReel(_aretes[index2], _noeuds[(*commonNodeEdges)[index][index2]])) { score += PENALITE_MAX_SELF; }
                    }
                    indexPasseCellule[index2] = true;
                }
            }
        }
        indexPasse[index] = true;
    }
    nombreCroisement = score;
    isNombreCroisementUpdated = true;
    return score;
}