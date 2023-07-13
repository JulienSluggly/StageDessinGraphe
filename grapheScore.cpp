#include "graphe.hpp"
#include "intersection.hpp"
#include <iostream>
#include <limits.h>

// Calcule le score d'intersection du graphe et le met a jour.
long Graphe::getNbCroisement() {
    long total = 0;
    for (int i = 0; i < _aretes.size(); ++i) {
        for (int j = i + 1; j < _aretes.size(); ++j) {
            if ((*commonNodeEdges)[i][j] == -1) {
                bool isIllegal = false;
                if (seCroisent(_aretes[i], _aretes[j],isIllegal)) {
                    if (isIllegal) { total += PENALITE_MAX; }
                    else { ++total; }
                }
            }
            else {
                if (surSegment(_aretes[i], _noeuds[(*commonNodeEdges)[j][i]])) { total += PENALITE_MAX_SELF; }
                else if (surSegment(_aretes[j], _noeuds[(*commonNodeEdges)[i][j]])) { total += PENALITE_MAX_SELF; }
            }
        }
        for (int k=0;k<_noeudsSeuls.size();k++) {
            if (surSegment(_aretes[i],_noeuds[_noeudsSeuls[k]])) { total += PENALITE_MAX; }
        }
    }
    nombreCroisement = total;
    isNombreCroisementUpdated = true;
    return total;
}

long Graphe::getNbCroisementNoeudIsole(int nodeId) {
    long total = 0;
    for (int i=0;i<_aretes.size();i++) {
        if (surSegment(_aretes[i], _noeuds[nodeId])) { total += PENALITE_MAX; }
    }
    return total;
}

long Graphe::getNbCroisementNoeudIsole(int nodeId, int swapIndex) {
    long total = 0;
    for (int i=0;i<_aretes.size();i++) {
        if (!_aretes[i].contains(swapIndex)) {
            if (surSegment(_aretes[i], _noeuds[nodeId])) { total += PENALITE_MAX; }
        }
    }
    return total;
}

long Graphe::getNbCroisementNoeudIsoleGrid(int nodeId) {
    long total = 0;
    std::vector<bool> indexPasseCellule(_aretes.size(),false);
    for (const int indexCellule : *_noeuds[nodeId].idCelluleVec) {
        for (const int index : grillePtr[indexCellule]->vecAreteId) {
            if (!indexPasseCellule[index]) {
                if (surSegment(_aretes[index],_noeuds[nodeId])) { total += PENALITE_MAX; }
                indexPasseCellule[index] = true;
            }
        }
    }
    return total;
}

long Graphe::getNbCroisementNoeudIsoleGrid(int nodeId, int swapIndex) {
    long total = 0;
    std::vector<bool> indexPasseCellule(_aretes.size(),false);
    for (const int indexCellule : *_noeuds[nodeId].idCelluleVec) {
        for (const int index : grillePtr[indexCellule]->vecAreteId) {
            if ((!indexPasseCellule[index])&&(!_aretes[index].contains(swapIndex))) {
                if (surSegment(_aretes[index],_noeuds[nodeId])) { total += PENALITE_MAX; }
                indexPasseCellule[index] = true;
            }
        }
    }
    return total;
}

long Graphe::getNbCroisementNoeudIsoleGridLimite(int nodeId, bool& depasse, double limiteScore) {
    long total = 0;
    std::vector<bool> indexPasseCellule(_aretes.size(),false);
    for (const int indexCellule : *_noeuds[nodeId].idCelluleVec) {
        for (const int index : grillePtr[indexCellule]->vecAreteId) {
            if (!indexPasseCellule[index]) {
                if (surSegment(_aretes[index],_noeuds[nodeId])) {
                    total += PENALITE_MAX;
                    if (total > limiteScore) { 
                        depasse = true;
                        return TRES_GRANDE_VALEUR;
                    }
                }
                indexPasseCellule[index] = true;
            }
        }
    }
    return total;
}

long Graphe::getNbCroisementNoeudIsoleGridLimite(int nodeId, int swapIndex,bool& depasse, double limiteScore) {
    long total = 0;
    std::vector<bool> indexPasseCellule(_aretes.size(),false);
    for (const int indexCellule : *_noeuds[nodeId].idCelluleVec) {
        for (const int index : grillePtr[indexCellule]->vecAreteId) {
            if ((!indexPasseCellule[index])&&(!_aretes[index].contains(swapIndex))) {
                if (surSegment(_aretes[index],_noeuds[nodeId])) { 
                    total += PENALITE_MAX;
                    if (total > limiteScore) { 
                        depasse = true;
                        return TRES_GRANDE_VALEUR;
                    }
                }
                indexPasseCellule[index] = true;
            }
        }
    }
    return total;
}

// Calcule le score du noeud en parametre.
long Graphe::getScoreCroisementNode(int nodeIndex) {
    long score = 0;
    std::vector<bool> indexPasse(_aretes.size(),false);
    for (const int& index : _noeuds[nodeIndex]._aretes) {
        for (int j = 0; j < _aretes.size(); ++j) {
            if ((index != j) && (!indexPasse[j])) {
                if ((*commonNodeEdges)[index][j] == -1) {
                    bool isIllegal = false;
                    if (seCroisent(_aretes[index], _aretes[j],isIllegal)) {
                        if (isIllegal) { score += PENALITE_MAX; }
                        else { score++; }
                    }
                }
                else {
                    if (surSegment(_aretes[index], _noeuds[(*commonNodeEdges)[j][index]])) { score += PENALITE_MAX_SELF; }
                    else if (surSegment(_aretes[j], _noeuds[(*commonNodeEdges)[index][j]])) { score += PENALITE_MAX_SELF; }
                }
            }
        }
        indexPasse[index] = true;
        for (int k=0;k<_noeudsSeuls.size();k++) {
            if (surSegment(_aretes[index],_noeuds[_noeudsSeuls[k]])) { score += PENALITE_MAX; }
        }
    }
    if (_noeuds[nodeIndex].estIsole()) { score += getNbCroisementNoeudIsole(nodeIndex); }
    return score;
}

// Calcule le score du noeud nodeIndex sans ajouter le score produit par le noeud swapIndex.
long Graphe::getScoreCroisementNode(int nodeIndex, int swapIndex) {
    long score = 0;
    std::vector<bool> indexPasse(_aretes.size(),false);
    for (const int& index : _noeuds[nodeIndex]._aretes) {
        if (!_aretes[index].contains(swapIndex)) {
            for (int j = 0; j < _aretes.size(); ++j) {
                if ((index != j) && (!indexPasse[j])) {
                    if (!_aretes[j].contains(swapIndex)) {
                        if ((*commonNodeEdges)[index][j] == -1) {
                            bool isIllegal = false;
                            if (seCroisent(_aretes[index], _aretes[j],isIllegal)) {
                                if (isIllegal) { score += PENALITE_MAX; }
                                else { score++; }
                            }
                        }
                        else {
                            if (surSegment(_aretes[index], _noeuds[(*commonNodeEdges)[j][index]])) { score += PENALITE_MAX_SELF; }
                            else if (surSegment(_aretes[j], _noeuds[(*commonNodeEdges)[index][j]])) { score += PENALITE_MAX_SELF; }
                        }
                    }
                }
            }
            for (int k=0;k<_noeudsSeuls.size();k++) {
                if (_noeudsSeuls[k] != swapIndex) {
                    if (surSegment(_aretes[index],_noeuds[_noeudsSeuls[k]])) { score += PENALITE_MAX; }
                }
            }
        }
        indexPasse[index] = true;
    }
    if (_noeuds[nodeIndex].estIsole()) { score += getNbCroisementNoeudIsole(nodeIndex,swapIndex); }
    return score;
}

// Calcule le score du noeud en parametre.
long Graphe::getScoreCroisementNodeGrid(int nodeIndex) {
    long score = 0;
    std::vector<bool> indexPasse(_aretes.size(),false);
    std::vector<bool> indexPasseCellule(_aretes.size(),false);
    std::vector<bool>* indexPasseIsole = nullptr;
    if (_noeudsSeuls.size() > 0) { indexPasseIsole = new std::vector<bool>(_noeuds.size(),false); }
    for (const int& index : _noeuds[nodeIndex]._aretes) {
        if (index != _noeuds[nodeIndex]._aretes[0]) { 
            indexPasseCellule.assign(indexPasseCellule.size(), false);
            if (indexPasseIsole != nullptr) { indexPasseIsole->assign(indexPasseIsole->size(),false); }
        }
        for (int j = 0; j < _aretes[index].vecIdCellules.size(); ++j) {
            std::vector<int>& vecId = grillePtr[_aretes[index].vecIdCellules[j]]->vecAreteId;
            for (const int& index2 : vecId) {
                if ((index != index2) && (!indexPasse[index2]) && (!indexPasseCellule[index2])) {
                    if ((*commonNodeEdges)[index][index2] == -1) {
                        bool isIllegal = false;
                        if (seCroisent(_aretes[index], _aretes[index2],isIllegal)) {
                            if (isIllegal) { score += PENALITE_MAX; }
                            else { score++; }
                        }
                    }
                    else {
                        if (surSegment(_aretes[index], _noeuds[(*commonNodeEdges)[index2][index]])) { score += PENALITE_MAX_SELF; }
                        else if (surSegment(_aretes[index2], _noeuds[(*commonNodeEdges)[index][index2]])) { score += PENALITE_MAX_SELF; }
                    }
                    indexPasseCellule[index2] = true;
                }
            }
            std::vector<int>& vecNodeIsole = grillePtr[_aretes[index].vecIdCellules[j]]->vecSingleNodeId;
            if (vecNodeIsole.size() > 0) {
                for (const int indexIsole : vecNodeIsole) {
                    if (!(*indexPasseIsole)[indexIsole]) {
                        if (surSegment(_aretes[index], _noeuds[indexIsole])) { score += PENALITE_MAX; }
                        (*indexPasseIsole)[indexIsole] = true;
                    }
                }
            }
        }
        indexPasse[index] = true;
    }
    if (_noeuds[nodeIndex].estIsole()) { score += getNbCroisementNoeudIsoleGrid(nodeIndex); }
    return score;
}

// Calcule le score du noeud nodeIndex sans ajouter le score produit par le noeud swapIndex.
long Graphe::getScoreCroisementNodeGrid(int nodeIndex, int swapIndex) {
    long score = 0;
    std::vector<bool> indexPasse(_aretes.size(),false);
    std::vector<bool> indexPasseCellule(_aretes.size(),false);
    std::vector<bool>* indexPasseIsole = nullptr;
    if (_noeudsSeuls.size() > 0) { indexPasseIsole = new std::vector<bool>(_noeuds.size(),false); }
    for (const int& index : _noeuds[nodeIndex]._aretes) {
        if (!_aretes[index].contains(swapIndex)) {
            if (index != _noeuds[nodeIndex]._aretes[0]) { 
                indexPasseCellule.assign(indexPasseCellule.size(), false);
                if (indexPasseIsole != nullptr) { indexPasseIsole->assign(indexPasseIsole->size(),false); }
            }
            for (int j = 0; j < _aretes[index].vecIdCellules.size(); ++j) {
                std::vector<int>& vecId = grillePtr[_aretes[index].vecIdCellules[j]]->vecAreteId;
                for (const int& index2 : vecId) {
                    if ((index != index2) && (!indexPasse[index2]) && (!indexPasseCellule[index2])) {
                        if (!_aretes[index2].contains(swapIndex)) {
                            if ((*commonNodeEdges)[index][index2] == -1) {
                                bool isIllegal = false;
                                if (seCroisent(_aretes[index], _aretes[index2],isIllegal)) {
                                    if (isIllegal) { score += PENALITE_MAX; }
                                    else { score++; }
                                }
                            }
                            else {
                                if (surSegment(_aretes[index], _noeuds[(*commonNodeEdges)[index2][index]])) { score += PENALITE_MAX_SELF; }
                                else if (surSegment(_aretes[index2], _noeuds[(*commonNodeEdges)[index][index2]])) { score += PENALITE_MAX_SELF; }
                            }
                            indexPasseCellule[index2] = true;
                        }
                    }
                }
                std::vector<int>& vecNodeIsole = grillePtr[_aretes[index].vecIdCellules[j]]->vecSingleNodeId;
                if (vecNodeIsole.size() > 0) {
                    for (const int indexIsole : vecNodeIsole) {
                        if ((!(*indexPasseIsole)[indexIsole])&&(indexIsole != swapIndex)) {
                            if (surSegment(_aretes[index], _noeuds[indexIsole])) { score += PENALITE_MAX; }
                            (*indexPasseIsole)[indexIsole] = true;
                        }
                    }
                }
            }
        }
        indexPasse[index] = true;
    }
    if (_noeuds[nodeIndex].estIsole()) { score += getNbCroisementNoeudIsoleGrid(nodeIndex,swapIndex); }
    return score;
}

// Calcule le score du noeud en parametre.
long Graphe::getScoreCroisementNodeGridLimite(int nodeIndex, bool& makeMove, double limiteScore) {
    long score = 0;
    std::vector<bool> indexPasse(_aretes.size(),false);
    std::vector<bool> indexPasseCellule(_aretes.size(),false);
    std::vector<bool>* indexPasseIsole = nullptr;
    if (_noeudsSeuls.size() > 0) { indexPasseIsole = new std::vector<bool>(_noeuds.size(),false); }
    for (const int& index : _noeuds[nodeIndex]._aretes) {
        if (index != _noeuds[nodeIndex]._aretes[0]) { 
            indexPasseCellule.assign(indexPasseCellule.size(), false);
            if (indexPasseIsole != nullptr) { indexPasseIsole->assign(indexPasseIsole->size(),false); }
        }
        for (int j = 0; j < _aretes[index].vecIdCellules.size(); ++j) {
            std::vector<int>& vecId = grillePtr[_aretes[index].vecIdCellules[j]]->vecAreteId;
            for (const int& index2 : vecId) {
                if ((index != index2) && (!indexPasse[index2]) && (!indexPasseCellule[index2])) {
                    if ((*commonNodeEdges)[index][index2] == -1) {
                        bool isIllegal = false;
                        if (seCroisent(_aretes[index], _aretes[index2],isIllegal)) {
                            if (isIllegal) { score += PENALITE_MAX; }
                            else { score++; }
                        }
                    }
                    else {
                        if (surSegment(_aretes[index], _noeuds[(*commonNodeEdges)[index2][index]])) { score += PENALITE_MAX_SELF; }
                        else if (surSegment(_aretes[index2], _noeuds[(*commonNodeEdges)[index][index2]])) { score += PENALITE_MAX_SELF; }
                    }
                    if (score > limiteScore) {
                        makeMove = false;
                        return TRES_GRANDE_VALEUR;
                    }
                    indexPasseCellule[index2] = true;
                }
            }
            std::vector<int>& vecNodeIsole = grillePtr[_aretes[index].vecIdCellules[j]]->vecSingleNodeId;
            if (vecNodeIsole.size() > 0) {
                for (const int indexIsole : vecNodeIsole) {
                    if (!(*indexPasseIsole)[indexIsole]) {
                        if (surSegment(_aretes[index], _noeuds[indexIsole])) { 
                            score += PENALITE_MAX;
                            if (score > limiteScore) {
                                makeMove = false;
                                return TRES_GRANDE_VALEUR;
                            }
                        }
                        (*indexPasseIsole)[indexIsole] = true;
                    }
                }
            }
        }
        indexPasse[index] = true;
    }
    if (_noeuds[nodeIndex].estIsole()) { 
        bool depasse = false;
        score += getNbCroisementNoeudIsoleGridLimite(nodeIndex,depasse,limiteScore);
        if (depasse) { 
            makeMove = false;
            return TRES_GRANDE_VALEUR;
        }
    }
    makeMove = true;
    return score;
}

long Graphe::getScoreCroisementNodeGridLimite(int nodeIndex, int swapIndex, bool& makeMove, double limiteScore) {
    long score = 0;
    std::vector<bool> indexPasse(_aretes.size(),false);
    std::vector<bool> indexPasseCellule(_aretes.size(),false);
    std::vector<bool>* indexPasseIsole = nullptr;
    if (_noeudsSeuls.size() > 0) { indexPasseIsole = new std::vector<bool>(_noeuds.size(),false); }
    for (const int& index : _noeuds[nodeIndex]._aretes) {
        if (!_aretes[index].contains(swapIndex)) {
            if (index != _noeuds[nodeIndex]._aretes[0]) { 
                indexPasseCellule.assign(indexPasseCellule.size(), false);
                if (indexPasseIsole != nullptr) { indexPasseIsole->assign(indexPasseIsole->size(),false); }
            }
            for (int j = 0; j < _aretes[index].vecIdCellules.size(); ++j) {
                std::vector<int>& vecId = grillePtr[_aretes[index].vecIdCellules[j]]->vecAreteId;
                for (const int& index2 : vecId) {
                    if ((index != index2) && (!indexPasse[index2]) && (!indexPasseCellule[index2])) {
                        if (!_aretes[index2].contains(swapIndex)) {
                            if ((*commonNodeEdges)[index][index2] == -1) {
                                bool isIllegal = false;
                                if (seCroisent(_aretes[index], _aretes[index2],isIllegal)) {
                                    if (isIllegal) { score += PENALITE_MAX; }
                                    else { score++; }
                                }
                            }
                            else {
                                if (surSegment(_aretes[index], _noeuds[(*commonNodeEdges)[index2][index]])) { score += PENALITE_MAX_SELF; }
                                else if (surSegment(_aretes[index2], _noeuds[(*commonNodeEdges)[index][index2]])) { score += PENALITE_MAX_SELF; }
                            }
                            if (score > limiteScore) {
                                makeMove = false;
                                return TRES_GRANDE_VALEUR;
                            }
                            indexPasseCellule[index2] = true;
                        }
                    }
                }
                std::vector<int>& vecNodeIsole = grillePtr[_aretes[index].vecIdCellules[j]]->vecSingleNodeId;
                if (vecNodeIsole.size() > 0) {
                    for (const int indexIsole : vecNodeIsole) {
                        if ((!(*indexPasseIsole)[indexIsole])&&(indexIsole != swapIndex)) {
                            if (surSegment(_aretes[index], _noeuds[indexIsole])) { 
                                score += PENALITE_MAX;
                                if (score > limiteScore) {
                                    makeMove = false;
                                    return TRES_GRANDE_VALEUR;
                                }
                            }
                            (*indexPasseIsole)[indexIsole] = true;
                        }
                    }
                }
            }
        }
        indexPasse[index] = true;
    }
    if (_noeuds[nodeIndex].estIsole()) { 
        bool depasse = false;
        score += getNbCroisementNoeudIsoleGridLimite(nodeIndex,swapIndex,depasse,limiteScore);
        if (depasse) { 
            makeMove = false;
            return TRES_GRANDE_VALEUR;
        }
    }
    makeMove = true;
    return score;
}

long Graphe::getScoreCroisementNodeGridLimit(int nodeIndex, long limiteScore) {
    long score = 0;
    std::vector<bool> indexPasse(_aretes.size(),false);
    std::vector<bool> indexPasseCellule(_aretes.size(),false);
    std::vector<bool>* indexPasseIsole = nullptr;
    if (_noeudsSeuls.size() > 0) { indexPasseIsole = new std::vector<bool>(_noeuds.size(),false); }
    for (const int& index : _noeuds[nodeIndex]._aretes) {
        if (index != _noeuds[nodeIndex]._aretes[0]) { 
            indexPasseCellule.assign(indexPasseCellule.size(), false);
            if (indexPasseIsole != nullptr) { indexPasseIsole->assign(indexPasseIsole->size(),false); }
        }
        for (int j = 0; j < _aretes[index].vecIdCellules.size(); ++j) {
            std::vector<int>& vecId = grillePtr[_aretes[index].vecIdCellules[j]]->vecAreteId;
            for (const int& index2 : vecId) {
                if ((index != index2) && (!indexPasse[index2]) && (!indexPasseCellule[index2])) {
                    if ((*commonNodeEdges)[index][index2] == -1) {
                        bool isIllegal = false;
                        if (seCroisent(_aretes[index], _aretes[index2],isIllegal)) {
                            if (isIllegal) { score += PENALITE_MAX; }
                            else { score++; }
                        }
                    }
                    else {
                        if (surSegment(_aretes[index], _noeuds[(*commonNodeEdges)[index2][index]])) { score += PENALITE_MAX_SELF; }
                        else if (surSegment(_aretes[index2], _noeuds[(*commonNodeEdges)[index][index2]])) { score += PENALITE_MAX_SELF; }
                    }
                    indexPasseCellule[index2] = true;
                    if (score > limiteScore) { return TRES_GRANDE_VALEUR; }
                }
            }
            std::vector<int>& vecNodeIsole = grillePtr[_aretes[index].vecIdCellules[j]]->vecSingleNodeId;
            if (vecNodeIsole.size() > 0) {
                for (const int indexIsole : vecNodeIsole) {
                    if (!(*indexPasseIsole)[indexIsole]) {
                        if (surSegment(_aretes[index], _noeuds[indexIsole])) { 
                            score += PENALITE_MAX;
                            if (score > limiteScore) { return TRES_GRANDE_VALEUR; }
                        }
                        (*indexPasseIsole)[indexIsole] = true;
                    }
                }
            }
        }
        indexPasse[index] = true;
    }
    if (_noeuds[nodeIndex].estIsole()) { 
        bool depasse = false;
        score += getNbCroisementNoeudIsoleGridLimite(nodeIndex,depasse,limiteScore);
        if (depasse) {  return TRES_GRANDE_VALEUR; }
    }
    return score;
}

long Graphe::getScoreCroisementNodeGridLimit(int nodeIndex, int swapIndex, long limiteScore) {
    long score = 0;
    std::vector<bool> indexPasse(_aretes.size(),false);
    std::vector<bool> indexPasseCellule(_aretes.size(),false);
    std::vector<bool>* indexPasseIsole = nullptr;
    if (_noeudsSeuls.size() > 0) { indexPasseIsole = new std::vector<bool>(_noeuds.size(),false); }
    for (const int& index : _noeuds[nodeIndex]._aretes) {
        if (!_aretes[index].contains(swapIndex)) {
            if (index != _noeuds[nodeIndex]._aretes[0]) {
                indexPasseCellule.assign(indexPasseCellule.size(), false);
                if (indexPasseIsole != nullptr) { indexPasseIsole->assign(indexPasseIsole->size(),false); }
            }
            for (int j = 0; j < _aretes[index].vecIdCellules.size(); ++j) {
                std::vector<int>& vecId = grillePtr[_aretes[index].vecIdCellules[j]]->vecAreteId;
                for (const int& index2 : vecId) {
                    if ((index != index2) && (!indexPasse[index2]) && (!indexPasseCellule[index2])) {
                        if (!_aretes[index2].contains(swapIndex)) {
                            if ((*commonNodeEdges)[index][index2] == -1) {
                                bool isIllegal = false;
                                if (seCroisent(_aretes[index], _aretes[index2],isIllegal)) {
                                    if (isIllegal) { score += PENALITE_MAX; }
                                    else { score++; }
                                }
                            }
                            else {
                                if (surSegment(_aretes[index], _noeuds[(*commonNodeEdges)[index2][index]])) { score += PENALITE_MAX_SELF; }
                                else if (surSegment(_aretes[index2], _noeuds[(*commonNodeEdges)[index][index2]])) { score += PENALITE_MAX_SELF; }
                            }
                            indexPasseCellule[index2] = true;
                            if (score > limiteScore) { return TRES_GRANDE_VALEUR; }
                        }
                    }
                }
                std::vector<int>& vecNodeIsole = grillePtr[_aretes[index].vecIdCellules[j]]->vecSingleNodeId;
                if (vecNodeIsole.size() > 0) {
                    for (const int indexIsole : vecNodeIsole) {
                        if ((!(*indexPasseIsole)[indexIsole])&&(indexIsole != swapIndex)) {
                            if (surSegment(_aretes[index], _noeuds[indexIsole])) { 
                                score += PENALITE_MAX;
                                if (score > limiteScore) { return TRES_GRANDE_VALEUR; }
                            }
                            (*indexPasseIsole)[indexIsole] = true;
                        }
                    }
                }
            }
        }
        indexPasse[index] = true;
    }
    if (_noeuds[nodeIndex].estIsole()) { 
        bool depasse = false;
        score += getNbCroisementNoeudIsoleGridLimite(nodeIndex,swapIndex,depasse,limiteScore);
        if (depasse) {  return TRES_GRANDE_VALEUR; }
    }
    return score;
}

// Ne modifie pas le score
long Graphe::getNbCroisementConst() const {
    long total = 0;
    for (int i = 0; i < _aretes.size(); ++i) {
        for (int j = i + 1; j < _aretes.size(); ++j) {
            if ((*commonNodeEdges)[i][j] == -1) {
                bool isIllegal = false;
                if (seCroisent(_aretes[i], _aretes[j],isIllegal)) {
                    if (isIllegal) { total += PENALITE_MAX; }
                    else { ++total; }
                }
            }
            else {
                if (surSegment(_aretes[i], _noeuds[(*commonNodeEdges)[j][i]])) { total += PENALITE_MAX_SELF; }
                else if (surSegment(_aretes[j], _noeuds[(*commonNodeEdges)[i][j]])) { total += PENALITE_MAX_SELF; }
            }
        }
        for (int k=0;k<_noeudsSeuls.size();k++) {
            if (surSegment(_aretes[i],_noeuds[_noeudsSeuls[k]])) { total += PENALITE_MAX; }
        }
    }
    return total;
}

// Ne modifie pas le score
long Graphe::getNbCroisementDiff() {
    nombreInter = 0;
    nombreInterIll = 0;
    nombreInterIllSelf = 0;
    for (int i = 0; i < _aretes.size(); ++i) {
        for (int j = i + 1; j < _aretes.size(); ++j) {
            if ((*commonNodeEdges)[i][j] == -1) {
                bool isIllegal = false;
                if (seCroisent(_aretes[i], _aretes[j],isIllegal)) {
                    if (isIllegal) { nombreInterIll++; }
                    else { nombreInter++; }
                }
            }
            else {
                if (surSegment(_aretes[i], _noeuds[(*commonNodeEdges)[j][i]])) { nombreInterIllSelf++; }
                else if (surSegment(_aretes[j], _noeuds[(*commonNodeEdges)[i][j]])) {nombreInterIllSelf++; }
            }
        }
        for (int k=0;k<_noeudsSeuls.size();k++) {
            if (surSegment(_aretes[i],_noeuds[_noeudsSeuls[k]])) { nombreInterIll++; }
        }
    }
    return nombreInter+nombreInterIll+nombreInterIllSelf;
}

long Graphe::getNbCroisementDiffGrid() {
    nombreInter = 0;
    nombreInterIll = 0;
    nombreInterIllSelf = 0;
    std::vector<bool> indexPasse(_aretes.size(),false);
    std::vector<bool> indexPasseCellule(_aretes.size(),false);
    std::vector<bool>* indexPasseIsole = nullptr;
    if (_noeudsSeuls.size() > 0) { indexPasseIsole = new std::vector<bool>(_noeuds.size(),false); }
    for (int index = 0; index < _aretes.size(); ++index) {
        if (index != 0) { 
            indexPasseCellule.assign(indexPasseCellule.size(), false);
            if (indexPasseIsole != nullptr) { indexPasseIsole->assign(indexPasseIsole->size(),false); }
        }
        for (int j = 0; j < _aretes[index].vecIdCellules.size(); ++j) {
            std::vector<int>& vecId = grillePtr[_aretes[index].vecIdCellules[j]]->vecAreteId;
            for (const int& index2 : vecId) {
                if ((index != index2) && (!indexPasse[index2]) && (!indexPasseCellule[index2])) {
                    if ((*commonNodeEdges)[index][index2] == -1) {
                        bool isIllegal = false;
                        if (seCroisent(_aretes[index], _aretes[index2],isIllegal)) {
                            if (isIllegal) { nombreInterIll++; }
                            else { nombreInter++; }
                        }
                    }
                    else {
                        if (surSegment(_aretes[index], _noeuds[(*commonNodeEdges)[index2][index]])) { nombreInterIllSelf++; }
                        else if (surSegment(_aretes[index2], _noeuds[(*commonNodeEdges)[index][index2]])) { nombreInterIllSelf++; }
                    }
                    indexPasseCellule[index2] = true;
                }
            }
            std::vector<int>& vecNodeIsole = grillePtr[_aretes[index].vecIdCellules[j]]->vecSingleNodeId;
            if (vecNodeIsole.size() > 0) {
                for (const int indexIsole : vecNodeIsole) {
                    if (!(*indexPasseIsole)[indexIsole]) {
                        if (surSegment(_aretes[index], _noeuds[indexIsole])) { nombreInterIll++; }
                        (*indexPasseIsole)[indexIsole] = true;
                    }
                }
            }
        }
        indexPasse[index] = true;
    }
    return nombreInter+nombreInterIll+nombreInterIllSelf;
}

long Graphe::getNbCroisementGrid() {
    long score = 0;
    std::vector<bool> indexPasse(_aretes.size(),false);
    std::vector<bool> indexPasseCellule(_aretes.size(),false);
    std::vector<bool>* indexPasseIsole = nullptr;
    if (_noeudsSeuls.size() > 0) { indexPasseIsole = new std::vector<bool>(_noeuds.size(),false); }
    for (int index = 0; index < _aretes.size(); ++index) {
        if (index != 0) {
            indexPasseCellule.assign(indexPasseCellule.size(), false);
            if (indexPasseIsole != nullptr) { indexPasseIsole->assign(indexPasseIsole->size(),false); }
        }
        for (int j = 0; j < _aretes[index].vecIdCellules.size(); ++j) {
            std::vector<int>& vecId = grillePtr[_aretes[index].vecIdCellules[j]]->vecAreteId;
            for (const int& index2 : vecId) {
                if ((index != index2) && (!indexPasse[index2]) && (!indexPasseCellule[index2])) {
                    if ((*commonNodeEdges)[index][index2] == -1) {
                        bool isIllegal = false;
                        if (seCroisent(_aretes[index], _aretes[index2],isIllegal)) {
                            if (isIllegal) { score += PENALITE_MAX; }
                            else { score++; }
                        }
                    }
                    else {
                        if (surSegment(_aretes[index], _noeuds[(*commonNodeEdges)[index2][index]])) { score += PENALITE_MAX_SELF; }
                        else if (surSegment(_aretes[index2], _noeuds[(*commonNodeEdges)[index][index2]])) { score += PENALITE_MAX_SELF; }
                    }
                    indexPasseCellule[index2] = true;
                }
            }
            std::vector<int>& vecNodeIsole = grillePtr[_aretes[index].vecIdCellules[j]]->vecSingleNodeId;
            if (vecNodeIsole.size() > 0) {
                for (const int indexIsole : vecNodeIsole) {
                    if (!(*indexPasseIsole)[indexIsole]) {
                        if (surSegment(_aretes[index], _noeuds[indexIsole])) { score += PENALITE_MAX; }
                        (*indexPasseIsole)[indexIsole] = true;
                    }
                }
            }
        }
        indexPasse[index] = true;
    }
    nombreCroisement = score;
    isNombreCroisementUpdated = true;
    return score;
}
