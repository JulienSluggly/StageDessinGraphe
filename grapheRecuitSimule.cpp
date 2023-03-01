#include "graphe.hpp"
#include <vector>
#include <chrono>
#include <iostream>
#include <climits>
#include <algorithm>

// Selectionne deux noeud et retourne celui avec le score le plus faible. (le moin d'intersection)
int Graphe::selectionNoeudTournoiBinaire(bool isScoreUpdated) {
    int randomId = generateRand(_noeuds.size() - 1);
    int randomId2;
    do {
        randomId2 = generateRand(_noeuds.size() - 1);
    } while (randomId2 == randomId);
    if (isScoreUpdated) {
        if (_noeuds[randomId].score > _noeuds[randomId2].score)
            return randomId;
    }
    else {
    if (getScoreCroisementNode(randomId) > getScoreCroisementNode(randomId2))
        return randomId;
    }
    return randomId2;
}

// Selectionne n noeuds et retourne celui avec le score le plus faible. (le moin d'intersection)
int Graphe::selectionNoeudTournoiMultiple(int n, bool isScoreUpdated) {
    int randomId = generateRand(_noeuds.size() - 1);
    long scoreMeilleur;
    if (isScoreUpdated)
        scoreMeilleur = _noeuds[randomId].score;
    else
        scoreMeilleur = getScoreCroisementNode(randomId);
    if (n > 1) {
        for (int i = 0; i < n; i++) {
            int randomId2;
            do {
                randomId2 = generateRand(_noeuds.size() - 1);
            } while (randomId2 == randomId);
            long scoreId2;
            if (isScoreUpdated)
                scoreId2 = _noeuds[randomId2].score;
            else
                scoreId2 = getScoreCroisementNode(randomId2);
            if (scoreId2 > scoreMeilleur) {
                randomId = randomId2;
                scoreMeilleur = scoreId2;
            }
        }
    }
    return randomId;
}

// Selectionne deux emplacements different entre eux et de celui du noeud, puis renvoie le plus proche du noeud.
// Le noeud doit etre place.
int Graphe::selectionEmplacementTournoiBinaire(int nodeId) {
    int randomEmpId = generateRand(_emplacements.size() - 1); // Selection aléatoire d'un emplacement disponible (pas tres équiprobable)
    // on retire si on pioche le meme emplacement
    while (_noeuds[nodeId].getEmplacement()->getId() == randomEmpId) {
        randomEmpId = generateRand(_emplacements.size() - 1);
    }
    int randomEmpId2 = generateRand(_emplacements.size() - 1); // Selection aléatoire d'un emplacement disponible (pas tres équiprobable)
    // on retire si on pioche le meme emplacement
    while ((_noeuds[nodeId].getEmplacement()->getId() == randomEmpId2)||(randomEmpId2 == randomEmpId)) {
        randomEmpId2 = generateRand(_emplacements.size() - 1);
    }
    double dist1 = ((_emplacements[randomEmpId].getX() - _noeuds[nodeId].getX()) * (_emplacements[randomEmpId].getX() - _noeuds[nodeId].getX())) + ((_emplacements[randomEmpId].getY() - _noeuds[nodeId].getY()) * (_emplacements[randomEmpId].getY() - _noeuds[nodeId].getY()));
    double dist2 = ((_emplacements[randomEmpId2].getX() - _noeuds[nodeId].getX()) * (_emplacements[randomEmpId2].getX() - _noeuds[nodeId].getX())) + ((_emplacements[randomEmpId2].getY() - _noeuds[nodeId].getY()) * (_emplacements[randomEmpId2].getY() - _noeuds[nodeId].getY()));
    if (dist2 < dist1) {
        randomEmpId = randomEmpId2;
    }
    return randomEmpId;
}

// Selectionne n emplacements different entre eux et de celui du noeud, puis renvoie le plus proche du noeud.
// Le noeud doit etre place.
int Graphe::selectionEmplacementTournoiMultiple(int n, int nodeId) {
    int randomEmpId = generateRand(_emplacements.size() - 1); // Selection aléatoire d'un emplacement disponible (pas tres équiprobable)
    // on retire si on pioche le meme emplacement
    while (_noeuds[nodeId].getEmplacement()->getId() == randomEmpId) {
        randomEmpId = generateRand(_emplacements.size() - 1);
    }
    double dist;
    if (n > 1) {
        dist = ((_emplacements[randomEmpId].getX() - _noeuds[nodeId].getX()) * (_emplacements[randomEmpId].getX() - _noeuds[nodeId].getX())) + ((_emplacements[randomEmpId].getY() - _noeuds[nodeId].getY()) * (_emplacements[randomEmpId].getY() - _noeuds[nodeId].getY()));
    }
    for (int i = 1; i < n; i++) {
        int randomEmpId2 = generateRand(_emplacements.size() - 1); // Selection aléatoire d'un emplacement disponible (pas tres équiprobable)
        // on retire si on pioche le meme emplacement
        while ((_noeuds[nodeId].getEmplacement()->getId() == randomEmpId2)||(randomEmpId2 == randomEmpId)) {
            randomEmpId2 = generateRand(_emplacements.size() - 1);
        }
        double dist2 = ((_emplacements[randomEmpId2].getX() - _noeuds[nodeId].getX()) * (_emplacements[randomEmpId2].getX() - _noeuds[nodeId].getX())) + ((_emplacements[randomEmpId2].getY() - _noeuds[nodeId].getY()) * (_emplacements[randomEmpId2].getY() - _noeuds[nodeId].getY()));
        if (dist2 < dist) {
            randomEmpId = randomEmpId2;
            dist = dist2;
        }
    }
    return randomEmpId;
}

int Graphe::selectionEmplacementTriangulation(int nodeId, int profondeur) {
    Emplacement* depart = _noeuds[nodeId].getEmplacement();
    Emplacement* arrive = depart;
    int randomId;
    for (int i=0;i<profondeur;i++) {
        if (i == profondeur - 1) {
            if (arrive->voisinsDelaunay.size() == 1) {
                if (arrive->voisinsDelaunay[0] == depart->_id) {
                    return arrive->_id;
                }
            }
            else {
                randomId = generateRand(arrive->voisinsDelaunay.size() - 1);
                while (arrive->voisinsDelaunay[randomId] == depart->_id) {
                    randomId = generateRand(arrive->voisinsDelaunay.size() - 1);
                }
            }
        }
        else {
            randomId = generateRand(arrive->voisinsDelaunay.size() - 1);
            while (arrive->voisinsDelaunay[randomId] == depart->_id) {
                randomId = generateRand(arrive->voisinsDelaunay.size() - 1);
            }
        }
        arrive = &_emplacements[arrive->voisinsDelaunay[randomId]];
    }
    return arrive->_id;
}

// Effectue la selection du noeud en fonction de modeNoeud, 0=Aleatoire,1=TournoiBinaire,2=TournoiMultiple
int Graphe::selectionNoeud(int modeNoeud, int t, bool isScoreUpdated) {
    int nodeId;
    if (modeNoeud == 0)
        nodeId = generateRand(_noeuds.size() - 1);
    else if (modeNoeud == 1) {
        nodeId = selectionNoeudTournoiBinaire(isScoreUpdated);
    }
    else if (modeNoeud == 2) {
        int nbTirageNoeud = ((100 - t) / 15) + 1;
        nodeId = selectionNoeudTournoiMultiple(nbTirageNoeud,isScoreUpdated);
    }
    return nodeId;
}

// Effectue la selection de l'emplacement en fonction de modeEmplacement, 0=Aleatoire,1=TournoiBinaire,2=TournoiMultiple
int Graphe::selectionEmplacement(int modeEmplacement, int nodeId, int t, std::vector<double>& customParam, int iter) {
    int slotId;
    switch (modeEmplacement) {
    case 0: {
        slotId = generateRand(_emplacements.size() - 1); // Selection aléatoire d'un emplacement disponible (pas tres équiprobable)
        // on retire si on pioche le meme emplacement
        while (_noeuds[nodeId].getEmplacement()->getId() == slotId) {
            slotId = generateRand(_emplacements.size() - 1);
        }
        break;
    }
    case 1: {
        slotId = selectionEmplacementTournoiBinaire(nodeId);
        break;
    }
    case 2: {
        int nbTirage = ((100 - t) / 15) + 1;
        slotId = selectionEmplacementTournoiMultiple(nbTirage, nodeId);
        break;
    }
    case 3: {
        /*int nbTirage;
        if (customParam[0] == 0) {
            nbTirage == customParam[1];
        }
        else if (customParam[0] == 1) {
            nbTirage = ((100 - t) / customParam[1]) + 1;
        }
        else if (customParam[0] == 2) {
            nbTirage = (iter / 100000) + customParam[1];
        }*/
        int nbTirage = (iter / 100000) + 3;
        slotId = selectionEmplacementTournoiMultiple(nbTirage, nodeId);
        break;
    }
    case 4: {
        int profondeur = (iter / 100000) + 10;
        if (customParam.size() > 0) {
            if (customParam[0] == 7) {
                profondeur = customParam[1];
            }
            else if (customParam[0] == 8) {
                profondeur = (iter / 100000) + customParam[1];
            }
            else if (customParam[0] == 9) {
                profondeur = ((1381545-iter) / 100000) + customParam[1];
            }
        }
        slotId = selectionEmplacementTriangulation(nodeId,profondeur);
        break;
    }
    }
    return slotId;
}

// Calcule le delay a appliquer lors du recuit si besoin.
void Graphe::calculDelaiRefroidissement(int& delay, std::vector<double>& customParam, int iter) {
    if (customParam.size() > 2) {
        if (customParam[0] == 3) {
            if (customParam[1] == 0) {
                delay = customParam[2];
            }
            else if (customParam[1] == 1) {
                delay = ceil((double)_noeuds.size() / customParam[2]) + 1;
            }
            else if (customParam[1] == 2) {
                delay = (iter / 100000) + customParam[2];
            }
            else if (customParam[1] == 3) {
                delay = ((1381545-iter) / 100000) + customParam[2];
            }
        }
    }
    else if (delay == -1) {
        delay = std::min((int)ceil((double)_noeuds.size() / 20.0) + 1,5);
    }
}

long Graphe::calculScoreNodeMethode(int nodeId, int idSwappedNode, bool swapped, bool useGrille, bool useScore) {
    if (useGrille) {
        if (swapped) { return getScoreCroisementNodeGrid(nodeId, idSwappedNode); }
        else { return getScoreCroisementNodeGrid(nodeId);}
    }
    else if (useScore) {
        if (swapped) { return getNodeLinkedScore(nodeId, idSwappedNode); }
        else { return _noeuds[nodeId].score; }
    }
    else { 
        if (swapped) { return getScoreCroisementNode(nodeId,idSwappedNode); }
        else { return getScoreCroisementNode(nodeId); }
    }
}

int Graphe::calculImprove(int nodeId,int slotId,bool& swapped,int& idSwappedNode,Emplacement*& oldEmplacement,bool useGrille,bool useScore) {
    oldEmplacement = _noeuds[nodeId].getEmplacement();
    long scoreNode;
    if (!_emplacements[slotId].estDisponible()) {
        swapped = true;
        idSwappedNode = _emplacements[slotId]._noeud->getId();
        scoreNode = calculScoreNodeMethode(nodeId,idSwappedNode,true,useGrille,useScore);
        scoreNode += calculScoreNodeMethode(idSwappedNode,-1,false,useGrille,useScore);
        _noeuds[nodeId].swap(&_emplacements[slotId]);
        if (useScore) {
            changeUpdateValue(nodeId);
            changeUpdateValue(idSwappedNode);
            updateNodeScore(idSwappedNode);
        }
    }
    else {
        scoreNode = calculScoreNodeMethode(nodeId,-1,false,useGrille,useScore);
        _noeuds[nodeId].setEmplacement(&_emplacements[slotId]);
        if (useScore) { changeUpdateValue(nodeId); }
    }
    if (useScore) { updateNodeScore(nodeId); }
    if (useGrille) { recalcNodeCellule(nodeId); }
    long newScoreNode;
    if (swapped) {
        if (useGrille) { recalcNodeCellule(idSwappedNode); }
        newScoreNode = calculScoreNodeMethode(nodeId,idSwappedNode,true,useGrille,useScore);
        newScoreNode += calculScoreNodeMethode(idSwappedNode,-1,false,useGrille,useScore);
    }
    else {
        newScoreNode = calculScoreNodeMethode(nodeId,-1,false,useGrille,useScore);
    }
    return newScoreNode - scoreNode;
}

void Graphe::applyRerecuitCustomParam(double& t,double& cool,double& coolt,double& seuil,std::vector<double> customParam) {
    if (customParam[0] == 8) { t = customParam[1]; }
    else if (customParam[0] == 9) { cool = customParam[1]; }
    else if (customParam[0] == 10) { coolt = customParam[1]; }
    else if (customParam[0] == 11) { seuil = customParam[1]; }
}

void Graphe::saveBestResultRecuit(std::vector<int>& bestResultVector, Graphe& bestResultGraphe, bool useScore, bool useGrille) {
    if (useScore) { bestResultGraphe.copyFromGraphe(*this); }
    else if (useGrille) { bestResultVector = saveCopy(); }
    else { bestResultVector = saveCopy(); }
}

void Graphe::loadBestResultRecuit(std::vector<int>& bestResultVector, Graphe& bestResultGraphe, long bestNbCroisement, bool useScore, bool useGrille) {
    nombreCroisement = bestNbCroisement;
    if (useScore) { copyFromGraphe(bestResultGraphe); }
    else if (useGrille) { loadCopy(bestResultVector); }
    else { loadCopy(bestResultVector); }
}

void Graphe::updateGraphDataRecuit(bool useScore, bool useGrille) {
    isNombreCroisementUpdated = true;
    if (useScore) { isNodeScoreUpdated = true; }
    else { isNodeScoreUpdated = false; }
    if (useScore) { isIntersectionVectorUpdated = true; }
    else { isIntersectionVectorUpdated = false; }
}

void Graphe::stepRecuitSimule(double& t, int& nbCrois, double cool, int modeNoeud, int modeEmplacement, std::vector<double> customParam) {
    std::cout << "Not Yet Updated.\n";
}

// Lance l'algorithme de recuit simulé sur le graphe pour minimiser le nombre d'intersection
// Met à jour la variable nombreCroisement du graphe.
// delay est le nombre de tour auquel on reste à la même température, -1 pour le rendre dynamique en fonction de la taille du graphe.
// modeNoeud et modeEMplacement sont le mode de sélection de noeud et d'emplacement, 0=Aléatoire, 1=TournoiBinaire, 2=TournoiMultiple
void Graphe::recuitSimule(double &timeBest, std::vector<double> customParam, double cool, double t, double seuil, int delay, int modeNoeud, int modeEmplacement, bool useGrille, bool useScore) {
    auto start = std::chrono::system_clock::now(); auto end = start; // Timer pour le meilleur resultat trouvé
    std::vector<int> bestResultVector; Graphe bestResultGraphe; // Sauvegarde du meilleur graphe.
    saveBestResultRecuit(bestResultVector,bestResultGraphe,useScore,useGrille);
    long nbCroisement;
    if (isNombreCroisementUpdated) { nbCroisement = nombreCroisement; }
    else { nbCroisement = getNbCroisement(); }
    long bestCroisement = nbCroisement;
    calculDelaiRefroidissement(delay,customParam,0);
    if (DEBUG_GRAPHE) std::cout << "Nb Croisement avant recuit: " << nbCroisement << std::endl;
    int nodeId, slotId, idSwappedNode, improve;
    bool swapped;
    Emplacement* oldEmplacement;
    for (int iter = 0; t > seuil && nbCroisement > 0; iter++) {
        calculDelaiRefroidissement(delay,customParam,iter); // Utile uniquement si customParam[0]==3 et customParam[1]==2 ou 3
        for (int del = 0; del < delay; del++) {
            nodeId = selectionNoeud(modeNoeud, t);
            slotId = selectionEmplacement(modeEmplacement, nodeId, t,customParam,iter);
            swapped = false;
            idSwappedNode = -1;
            improve = calculImprove(nodeId,slotId,swapped,idSwappedNode,oldEmplacement,useGrille,useScore);
            if (improve < 0) {
                nbCroisement += improve;
                if (nbCroisement < bestCroisement) {
                    bestCroisement = nbCroisement;
                    saveBestResultRecuit(bestResultVector,bestResultGraphe,useScore,useGrille);
                    end = std::chrono::system_clock::now();
                    if (DEBUG_PROGRESS) std::cout << "Meilleur Recuit: " << bestCroisement << " Iteration: " << iter << " t: " << t << std::endl;
                }
            }
            else {
                double randDouble = generateDoubleRand(1.0);
                if (randDouble >= exp(-improve / t)) {
                    if (useScore) { changeUpdateValue(nodeId); }
                    if (swapped) {
                        if (useScore) { changeUpdateValue(idSwappedNode); }
                        _noeuds[nodeId].swap(oldEmplacement);
                        if (useScore) { updateNodeScore(idSwappedNode); }
                        if (useGrille) { recalcNodeCellule(idSwappedNode); }
                    }
                    else {
                        _noeuds[nodeId].setEmplacement(oldEmplacement);
                    }
                    if (useScore) { updateNodeScore(nodeId); }
                    if (useGrille) { recalcNodeCellule(nodeId); }
                }
                else {
                    nbCroisement += improve;
                }
            }
        }
        t *= cool;
    }
    loadBestResultRecuit(bestResultVector,bestResultGraphe,bestCroisement,useScore,useGrille);
    updateGraphDataRecuit(useScore,useGrille);
    std::chrono::duration<double> secondsTotal = end - start;
    timeBest = secondsTotal.count();
    if (DEBUG_GRAPHE) std::cout << "Meilleur resultat du recuit: " << bestCroisement << std::endl;
}

// Applique le recuit simulé plusieurs fois
// Met a jour le nombre de croisement du graphe.
void Graphe::rerecuitSimule(double &timeBest,int &nombreRecuit,std::vector<double> customParam, int iter, double cool, double coolt, double t, double seuil, int delay, int modeNoeud, int modeEmplacement, bool useGrille, bool useScore) {
    applyRerecuitCustomParam(t,cool,coolt,seuil,customParam);
    auto start = std::chrono::system_clock::now();
    auto end = start;
    if (DEBUG_GRAPHE) std::cout << "Starting Rerecuit " << iter << " iterations." << std::endl;
    int numberOfNoUpgrade = 0;
    int maxIter = 2;
    if (iter != -1) { maxIter = iter; }
    long lastCroisement;
    if (isNombreCroisementUpdated) { lastCroisement = nombreCroisement; }
    else { lastCroisement = getNbCroisementConst(); }
    int i=1;
    double recuitTimeBest;
    auto totalEnd = std::chrono::system_clock::now();
	std::chrono::duration<double> secondsTotalExec = totalEnd - start;
    while ((numberOfNoUpgrade < maxIter)&&(!RECUIT_LIMIT_3600||secondsTotalExec.count() < 3600)) {
        if (useGrille) { if (i>1) { reinitGrille(); } }
        if (DEBUG_GRAPHE) std::cout << "Starting Recuit Number: " << i << " t: " << t << " cool " << cool << " NumNoUp: " << numberOfNoUpgrade << std::endl;
        recuitSimule(recuitTimeBest,customParam, cool, t, seuil, delay, modeNoeud, modeEmplacement, useGrille, useScore);
        nombreRecuit++;
        t *= coolt;
        if (iter != -1) {
            numberOfNoUpgrade++;
        }
        else {
            long newCroisement;
            if (isNombreCroisementUpdated) { newCroisement = nombreCroisement; }
            else { newCroisement = getNbCroisementConst(); }
            if (newCroisement == lastCroisement) {
                numberOfNoUpgrade++;
            }
            else {
                lastCroisement = newCroisement;
                end = std::chrono::system_clock::now();
                numberOfNoUpgrade = 0;
            }
        }
        i++;
        totalEnd = std::chrono::system_clock::now();
        secondsTotalExec = totalEnd - start;
    }
    std::chrono::duration<double> secondsTotal = end - start;
    timeBest = secondsTotal.count();
}

// Applique l'algorithme meilleur deplacement sur le graphe.
// On parcoure tout les noeuds et on teste chaque deplacement possible et on effectue le meilleur s'il ameliore le score. (O(n²*e))
// Met a jour le nombre de croisement du graphe.
void Graphe::bestDeplacement() {
    long nbIntersection;
    if (isNombreCroisementUpdated) {
        nbIntersection = nombreCroisement;
    }
    else {
        nbIntersection = getNbCroisement();
    }
    int bestImprove = 0;
    int indexNoeud = -1, indexEmplacement = -1;
    do {
        bestImprove = 0;
        for (int i = 0; i < _noeuds.size(); i++) {
            for (int j = 0; j < _emplacements.size(); j++) {
                if (_noeuds[i].getEmplacement()->getId() != j) {
                    long score, newScore;
                    int swappedId;
                    bool swapped = false;
                    Emplacement* oldEmplacement = _noeuds[i].getEmplacement();
                    if (_emplacements[j].estDisponible()) {
                        score = getScoreCroisementNode(i);
                        _noeuds[i].setEmplacement(&_emplacements[j]);
                    }
                    else {
                        swappedId = _emplacements[j]._noeud->getId();
                        score = getScoreCroisementNode(i);
                        score += getScoreCroisementNode(swappedId, i);
                        _noeuds[i].swap(&_emplacements[j]);
                        swapped = true;
                    }
                    if (!swapped) {
                        newScore = getScoreCroisementNode(i);
                        _noeuds[i].setEmplacement(oldEmplacement);
                    }
                    else {
                        newScore = getScoreCroisementNode(i);
                        newScore += getScoreCroisementNode(swappedId, i);
                        _noeuds[i].swap(oldEmplacement);
                    }
                    int improve = newScore - score;
                    if (improve < bestImprove) {
                        bestImprove = improve;
                        indexNoeud = i;
                        indexEmplacement = j;
                    }
                }
            }
        }
        if (bestImprove < 0) {
            if (_emplacements[indexEmplacement].estDisponible()) {
                _noeuds[indexNoeud].setEmplacement(&_emplacements[indexEmplacement]);
            }
            else {
                _noeuds[indexNoeud].swap(&_emplacements[indexEmplacement]);
            }
            nbIntersection += bestImprove;
        }
    } while (bestImprove < 0);
    nombreCroisement = nbIntersection;
    isNombreCroisementUpdated = true;
    isNodeScoreUpdated = false;
    isIntersectionVectorUpdated = false;
    if (DEBUG_GRAPHE) std::cout << "Meilleur resultat de l'algo meilleur deplacement: " << nbIntersection << std::endl;
}