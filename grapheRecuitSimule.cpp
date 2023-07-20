#include "graphe.hpp"
#include <vector>
#include <chrono>
#include <iostream>
#include <climits>
#include <algorithm>
#include <omp.h>
#include <filesystem>
#include "intersection.hpp"

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
    int randomId=-1;
    for (int i=0;i<profondeur;i++) {
        if (i == profondeur - 1) {
            if (arrive->voisinsDelaunay->size() == 1) {
                if (arrive->voisinsDelaunay->at(0) == depart->_id) {
                    return arrive->_id;
                }
            }
            else {
                randomId = generateRand(arrive->voisinsDelaunay->size() - 1);
                while (arrive->voisinsDelaunay->at(randomId) == depart->_id) {
                    randomId = generateRand(arrive->voisinsDelaunay->size() - 1);
                }
            }
        }
        else {
            randomId = generateRand(arrive->voisinsDelaunay->size() - 1);
            while (arrive->voisinsDelaunay->at(randomId) == depart->_id) {
                randomId = generateRand(arrive->voisinsDelaunay->size() - 1);
            }
        }
        arrive = &_emplacements[arrive->voisinsDelaunay->at(randomId)];
    }
    return arrive->_id;
}

// Effectue la selection du noeud en fonction de modeNoeud, 0=Aleatoire,1=TournoiBinaire,2=TournoiMultiple
int Graphe::selectionNoeud(int modeNoeud, int t, bool isScoreUpdated) {
    int nodeId=-1;
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

// Effectue la selection de l'emplacement en fonction de modeEmplacement, 0=Aleatoire,1=TournoiBinaire,2=TournoiMultiple,3=Triangulation
int Graphe::selectionEmplacement(int modeEmplacement, int nodeId, int t, std::vector<std::vector<double>>& customParam, int iter) {
    int slotId = -1;
    switch (modeEmplacement) {
    case 0: { // Random
        slotId = generateRand(_emplacements.size() - 1); // Selection aléatoire d'un emplacement disponible (pas tres équiprobable)
        // on retire si on pioche le meme emplacement
        while (_noeuds[nodeId].getEmplacement()->getId() == slotId) {
            slotId = generateRand(_emplacements.size() - 1);
        }
        break;
    }
    case 1: { // Tournoi Binaire
        slotId = selectionEmplacementTournoiBinaire(nodeId);
        break;
    }
    case 2: { // Tournoi Multiple
        int nbTirage = (iter / 100000) + 3;
        if (customParam.size() > 0) {
            for (std::vector<double>& param : customParam) {
                if (param.size() > 0) {
                    if (param[0] == 0) {
                        if (param[1] == 0) {
                            nbTirage = param[2];
                        }
                        else if (param[1] == 1) {
                            nbTirage = ((100 - t) / param[2]) + 1;
                        }
                        else if (param[1] == 2) {
                            nbTirage = (iter / 100000) + param[2];
                        }
                        else if (param[1] == 3) {
                            nbTirage = (iter / param[2]) + 1;
                        }
                    }
                }
            }
        }
        slotId = selectionEmplacementTournoiMultiple(nbTirage, nodeId);
        break;
    }
    case 3: { // Voisin Triangulation
        int profondeur = (iter / 100000) + 10;
        if (customParam.size() > 0) {
            for (std::vector<double>& param : customParam) {
                if (param.size() > 0) {
                    if (param[0] == 7) {
                        profondeur = param[1];
                    }
                    else if (param[0] == 8) {
                        profondeur = (iter / 100000) + param[1];
                    }
                    else if (param[0] == 9) {
                        profondeur = ((1381545-iter) / 100000) + param[1];
                    }
                }
            }
        }
        slotId = selectionEmplacementTriangulation(nodeId,profondeur);
        break;
    }
    }
    return slotId;
}

// Calcule le delay a appliquer lors du recuit si besoin.
void Graphe::calculDelaiRefroidissement(int& delay, std::vector<std::vector<double>>& customParam, int iter) {
    if (customParam.size() > 0) {
        for (std::vector<double>& param : customParam) {
            if (param.size() > 0) {
                if (param[0] == 3) {
                    if (param[1] == 0) {
                        delay = param[2];
                    }
                    else if (param[1] == 1) {
                        delay = ceil((double)_noeuds.size() / param[2]) + 1;
                    }
                    else if (param[1] == 2) {
                        delay = (iter / 100000) + param[2];
                    }
                    else if (param[1] == 3) {
                        delay = ((1381545-iter) / 100000) + param[2];
                    }
                }
            }
        }
    }
    else if (delay == -1) {
        delay = std::min((int)ceil((double)_noeuds.size() / 20.0) + 1,5);
    }
}

long Graphe::calculScoreNodeMethode(int nodeId, int idSwappedNode, bool swapped, bool useGrille, bool useReel) {
    if (useGrille) {
        if (swapped) { return getScoreCroisementNodeGrid(nodeId, idSwappedNode); }
        else { return getScoreCroisementNodeGrid(nodeId);}
    }
    else { 
        if (swapped) { return getScoreCroisementNode(nodeId,idSwappedNode); }
        else { return getScoreCroisementNode(nodeId); }
    }
}

long Graphe::calculScoreNodeMethodeLimite(int nodeId, int idSwappedNode, bool swapped, bool useGrille, bool& makeMove, double limiteScore,bool useReel) {
    if (useGrille) {
        if (swapped) { return getScoreCroisementNodeGridLimite(nodeId, idSwappedNode,makeMove,limiteScore); }
        else { return getScoreCroisementNodeGridLimite(nodeId,makeMove,limiteScore);}
    }
    return -1;
}

long Graphe::calculImprove(int nodeId,int slotId,bool& swapped,int& idSwappedNode,bool useGrille) {
    long scoreNode;
    if (!_emplacements[slotId].estDisponible()) {
        swapped = true;
        idSwappedNode = _emplacements[slotId]._noeud->getId();
        scoreNode = calculScoreNodeMethode(nodeId,idSwappedNode,true,useGrille);
        scoreNode += calculScoreNodeMethode(idSwappedNode,-1,false,useGrille);
        _noeuds[nodeId].swap(&_emplacements[slotId]);
    }
    else {
        scoreNode = calculScoreNodeMethode(nodeId,-1,false,useGrille);
        _noeuds[nodeId].setEmplacement(&_emplacements[slotId]);
    }
    if (useGrille) { recalcNodeCellule(nodeId); }
    long newScoreNode;
    if (swapped) {
        if (useGrille) { recalcNodeCellule(idSwappedNode); }
        newScoreNode = calculScoreNodeMethode(nodeId,idSwappedNode,true,useGrille);
        newScoreNode += calculScoreNodeMethode(idSwappedNode,-1,false,useGrille);
    }
    else {
        newScoreNode = calculScoreNodeMethode(nodeId,-1,false,useGrille);
    }
    return newScoreNode - scoreNode;
}

long Graphe::calculImproveLimite(int nodeId,int slotId,bool& swapped,int& idSwappedNode,bool useGrille,bool& makeMove,double limiteImprove) {
    long scoreNode;
    if (!_emplacements[slotId].estDisponible()) {
        swapped = true;
        idSwappedNode = _emplacements[slotId]._noeud->getId();
        scoreNode = calculScoreNodeMethode(nodeId,idSwappedNode,true,useGrille,false);
        scoreNode += calculScoreNodeMethode(idSwappedNode,-1,false,useGrille,false);
        _noeuds[nodeId].swap(&_emplacements[slotId]);
    }
    else {
        scoreNode = calculScoreNodeMethode(nodeId,-1,false,useGrille,false);
        _noeuds[nodeId].setEmplacement(&_emplacements[slotId]);
    }
    if (useGrille) { recalcNodeCellule(nodeId); }
    long limiteScore = scoreNode + limiteImprove;
    if (limiteScore > 0) {
        long newScoreNode;
        if (swapped) {
            if (useGrille) { recalcNodeCellule(idSwappedNode); }
            newScoreNode = calculScoreNodeMethodeLimite(nodeId, idSwappedNode,swapped,useGrille,makeMove,limiteScore,useCoordReel);
            if (makeMove) { newScoreNode += calculScoreNodeMethodeLimite(idSwappedNode,-1,swapped,useGrille,makeMove,limiteScore-newScoreNode,useCoordReel); }
        }
        else {
            newScoreNode =  calculScoreNodeMethodeLimite(nodeId,-1,false,useGrille,makeMove,limiteScore,useCoordReel);
        }
        return newScoreNode - scoreNode;
    }
    return -1;
}

long Graphe::calculImproveTabou(int nodeId,int slotId,bool& swapped,int& idSwappedNode,bool useGrille,long bestImprove) {
    long scoreNode;
    if (!_emplacements[slotId].estDisponible()) {
        swapped = true;
        idSwappedNode = _emplacements[slotId]._noeud->getId();
        scoreNode = calculScoreNodeMethode(nodeId,idSwappedNode,true,useGrille,false);
        scoreNode += calculScoreNodeMethode(idSwappedNode,-1,false,useGrille,false);
        _noeuds[nodeId].swap(&_emplacements[slotId]);
    }
    else {
        scoreNode = calculScoreNodeMethode(nodeId,-1,false,useGrille,false);
        _noeuds[nodeId].setEmplacement(&_emplacements[slotId]);
    }
    if (useGrille) { 
        recalcNodeCellule(nodeId);
        if (swapped) { recalcNodeCellule(idSwappedNode); }
    }
    long limiteScore = scoreNode + bestImprove;
    if (limiteScore > 0) {
        long newScoreNode;
        if (swapped) {
            newScoreNode = getScoreCroisementNodeGridLimit(nodeId, idSwappedNode, limiteScore);
            if (newScoreNode < limiteScore) { newScoreNode += getScoreCroisementNodeGridLimit(idSwappedNode, limiteScore-newScoreNode); }
        }
        else {
            newScoreNode = getScoreCroisementNodeGridLimit(nodeId, limiteScore);
        }
        return newScoreNode - scoreNode;
    }
    return TRES_GRANDE_VALEUR;
}

void Graphe::applyRerecuitCustomParam(double& t,double& cool,double& coolt,double& seuil, bool adaptCool,std::vector<std::vector<double>>& customParam) {
    if (adaptCool) {
        if (_noeuds.size() <= 30) { cool = 0.9999945; }
        else if (_noeuds.size() <= 100) { cool = 0.999999; }
        else { cool = 0.99999945; }
    }
    if (customParam.size() > 0) {
        for (std::vector<double>& param : customParam) {
            if (param.size() > 0) {
                if (param[0] == 8) { t = param[1]; }
                else if (param[0] == 9) { cool = param[1]; }
                else if (param[0] == 13) { coolt = param[1]; }
                else if (param[0] == 14) { seuil = param[1]; }
            }
        }
    }
}

void Graphe::applyRecuitCustomParam(double& coeffImprove,std::vector<std::vector<double>>& customParam) {
    if (customParam.size() > 0) {
        for (std::vector<double>& param : customParam) {
            if (param.size() > 0) {
                if (param[0] == 12) { coeffImprove = param[1]; }
            }
        }
    }
}

void Graphe::saveBestResultRecuit(std::vector<int>& bestResultVector, Graphe& bestResultGraphe) {
    bestResultVector = saveCopy();
}

void Graphe::loadBestResultRecuit(std::vector<int>& bestResultVector, Graphe& bestResultGraphe, long bestNbCroisement) {
    nombreCroisement = bestNbCroisement;
    loadCopy(bestResultVector);
}

void Graphe::updateGraphDataRecuit() {
    isNombreCroisementUpdated = true;
}

void Graphe::stepRecuitSimule(double& t, int& nbCrois, double cool, int modeNoeud, int modeEmplacement, std::vector<std::vector<double>> customParam) {
    tcout() << "Not Yet Updated.\n";
}

void Graphe::setupNombreCroisement(long& nbCroisement, long& bestCroisement, long& debutCroisement) {
    if (isNombreCroisementUpdated) { nbCroisement = nombreCroisement; }
    nbCroisement = getNbCroisement();
    bestCroisement = nbCroisement;
    debutCroisement = nbCroisement;
}

// Lance l'algorithme de recuit simulé sur le graphe pour minimiser le nombre d'intersection
// Met à jour la variable nombreCroisement du graphe.
// modeNoeud et modeEmplacement sont le mode de sélection de noeud et d'emplacement, 0=Aléatoire, 1=TournoiBinaire, 2=TournoiMultiple, 3=Triangulation(Emplacement uniquement)
// Par defaut utilise la grille et le Tournoi Multiple sur les Emplacements.
long Graphe::recuitSimule(double &timeBest, std::chrono::time_point<std::chrono::system_clock> start, std::vector<std::vector<double>> customParam, double cool, double t, double seuil, int delay, int modeNoeud, int modeEmplacement, bool useGrille, int timeLimit) {
    auto bestEnd = start; auto end = start; // Timer pour le meilleur resultat trouvé et total
    std::vector<int> bestResultVector; Graphe bestResultGraphe; // Sauvegarde du meilleur graphe.
    saveBestResultRecuit(bestResultVector,bestResultGraphe);
    long nbCroisement, bestCroisement, debutCroisement;
    setupNombreCroisement(nbCroisement,bestCroisement,debutCroisement);
    calculDelaiRefroidissement(delay,customParam,0);
    #if defined(DEBUG_GRAPHE)
        tcout() << "Nb Croisement avant recuit: " << nbCroisement << std::endl;
    #endif
    int nodeId, slotId, idSwappedNode, improve;
    bool swapped;
    Emplacement* oldEmplacement;
    std::chrono::duration<double> secondsTotal = end - start;
    for (int iter = 0; t > seuil && nbCroisement > 0 && ((secondsTotal.count() < timeLimit)||(timeLimit == -1)); iter++) {
        calculDelaiRefroidissement(delay,customParam,iter); // Utile uniquement si customParam[0]==3 et customParam[1]==2 ou 3
        for (int del = 0; del < delay; del++) {
            nodeId = selectionNoeud(modeNoeud, t);
            slotId = selectionEmplacement(modeEmplacement, nodeId, t,customParam,iter);
            oldEmplacement = _noeuds[nodeId].getEmplacement();
            swapped = false;
            idSwappedNode = -1;
            improve = calculImprove(nodeId,slotId,swapped,idSwappedNode,useGrille);
            if (improve < 0) {
                nbCroisement += improve;
                if (nbCroisement < bestCroisement) {
                    bestCroisement = nbCroisement;
                    saveBestResultRecuit(bestResultVector,bestResultGraphe);
                    bestEnd = std::chrono::system_clock::now();
                    #if defined(DEBUG_GRAPHE_PROGRESS)
                        tcout() << "Meilleur Recuit: " << bestCroisement << " Iteration: " << iter << " t: " << t << std::endl;
                    #endif
                }
            }
            else {
                double randDouble = generateDoubleRand(1.0);
                if (randDouble >= exp(-improve / t)) {
                    if (swapped) {
                        _noeuds[nodeId].swap(oldEmplacement);
                        if (useGrille) { recalcNodeCellule(idSwappedNode); }
                    }
                    else {
                        _noeuds[nodeId].setEmplacement(oldEmplacement);
                    }
                    if (useGrille) { recalcNodeCellule(nodeId); }
                }
                else {
                    nbCroisement += improve;
                }
            }
        }
        t *= cool;
        end = std::chrono::system_clock::now();
        secondsTotal = end - start;
    }
    loadBestResultRecuit(bestResultVector,bestResultGraphe,bestCroisement);
    updateGraphDataRecuit();
    std::chrono::duration<double> secondsBest = bestEnd - start;
    timeBest = secondsBest.count();
    #if defined(DEBUG_GRAPHE)
        tcout() << "Meilleur resultat du recuit: " << bestCroisement << std::endl;
    #endif
    return debutCroisement - bestCroisement;
}

// Applique le recuit simulé plusieurs fois
// Met a jour le nombre de croisement du graphe.
void Graphe::rerecuitSimule(double &timeBest,int &nombreRecuit,std::chrono::time_point<std::chrono::system_clock> start,std::vector<std::vector<double>> customParam, int iter, double cool, double coolt, double t, double seuil, int delay, int modeNoeud, int modeEmplacement, bool useGrille, int timeLimit, bool firstWaveImp, bool adaptCool) {
    auto end = start;
    applyRerecuitCustomParam(t,cool,coolt,seuil,adaptCool,customParam);
    nombreRecuit= 0;
    int numberOfNoUpgrade = 0, maxIter = 10;
    #if defined(DEBUG_GRAPHE)
        if (iter == -1) { tcout() << "Starting Rerecuit Simule " << maxIter << " reheating no upgrade.\n"; }
        else { tcout() << "Starting Rerecuit Simule " << iter << " max iterations.\n"; }
    #endif
    if (iter != -1) { maxIter = iter; }
    long lastCroisement;
    if (isNombreCroisementUpdated) { lastCroisement = nombreCroisement; }
    else { lastCroisement = getNbCroisementConst(); }
    int i=1;
    double recuitTimeBest = -1.0;
    auto totalEnd = std::chrono::system_clock::now();
	std::chrono::duration<double> secondsTotalExec = totalEnd - start;
    double startingTemp = t;
    if (firstWaveImp) { t = 0.05; }
    while ((numberOfNoUpgrade < maxIter)&&((timeLimit == -1)||(secondsTotalExec.count() < timeLimit))) {
        if (useGrille) { if (i>1) { reinitGrille(); } }
        #if defined(DEBUG_GRAPHE)
            tcout() << "Starting Recuit Number: " << i << " t: " << t << " cool " << cool << " Crossings: " << lastCroisement << " NumNoUp: " << numberOfNoUpgrade << std::endl;
        #endif
        nombreRecuit++;
        long nbInterSuppr = recuitSimuleLimite(recuitTimeBest,start,customParam, cool, t, seuil, delay, modeNoeud, modeEmplacement, useGrille, timeLimit);
        t *= coolt;
        if ((firstWaveImp)&&(i==1)) { t = startingTemp; }
        if (iter != -1) { numberOfNoUpgrade++; }
        else {
            long newCroisement;
            if (isNombreCroisementUpdated) { newCroisement = nombreCroisement; }
            else { newCroisement = getNbCroisementConst(); }
            if (newCroisement == lastCroisement) {
                numberOfNoUpgrade++;
            }
            else {
                lastCroisement = newCroisement;
                numberOfNoUpgrade = 0;
                timeBest = recuitTimeBest;
            }
        }
        i++;
        totalEnd = std::chrono::system_clock::now();
        secondsTotalExec = totalEnd - start;
    }
    timeBest = recuitTimeBest;
}

long Graphe::recuitSimuleChallenge(double cool, double t, double seuil) {
    std::vector<int> bestResultVector; Graphe bestResultGraphe; // Sauvegarde du meilleur graphe.
    saveBestResultRecuit(bestResultVector,bestResultGraphe);
    long nbCroisement, bestCroisement, debutCroisement;
    setupNombreCroisement(nbCroisement,bestCroisement,debutCroisement);
    #if defined(DEBUG_GRAPHE)
        tcout() << "Nb Croisement avant recuit: " << nbCroisement << " cool: " << cool << std::endl;
    #endif
    int nodeId, slotId, idSwappedNode, improve;
    bool swapped;
    Emplacement* oldEmplacement;
    std::filesystem::path resultFolder = std::filesystem::current_path() / "resultats";
    auto start = std::chrono::system_clock::now();
    auto debutEcriture = start;
    std::chrono::duration<double> secondsTotal = debutEcriture - start;
    for (int iter = 0; t > seuil && nbCroisement > 0; iter++) {
        nodeId = generateRand(_noeuds.size() - 1);
        int nbTirage = (iter / 100000) + 3;
        slotId = selectionEmplacementTournoiMultiple(nbTirage, nodeId);
        oldEmplacement = _noeuds[nodeId].getEmplacement();
        swapped = false;
        idSwappedNode = -1;
        double randDouble = generateDoubleRand(1.0);
        double limiteImprove = -t*std::log(randDouble);
        bool makeMove = false;
        improve = calculImproveLimite(nodeId,slotId,swapped,idSwappedNode,true,makeMove,limiteImprove);
        if (makeMove) {
            nbCroisement += improve;
            if (nbCroisement < bestCroisement) {
                bestCroisement = nbCroisement;
                saveBestResultRecuit(bestResultVector,bestResultGraphe);
	            std::string outputName = resultFolder.string() + "/" + to_string(bestCroisement) + "-" + nomGraphe + ".json";
                if (isAbleToWriteFile) {
                    writeToJsonChallenge(outputName);
                    hasWrittenAFile = true;
                }
                #if defined(DEBUG_GRAPHE)
                    tcout() << "Meilleur Recuit: " << bestCroisement << " Iteration: " << iter << " t: " << t << std::endl;
                #endif
            }
        }
        else {
            if (swapped) {
                _noeuds[nodeId].swap(oldEmplacement);
                recalcNodeCellule(idSwappedNode);
            }
            else {
                _noeuds[nodeId].setEmplacement(oldEmplacement);
            }
            recalcNodeCellule(nodeId);
        }
        if (!isAbleToWriteFile) {
            debutEcriture = std::chrono::system_clock::now();
            secondsTotal = debutEcriture - start;
            if (secondsTotal.count() > 300) { isAbleToWriteFile = true; }
        }
        t *= cool;
    }
    loadBestResultRecuit(bestResultVector,bestResultGraphe,bestCroisement);
    updateGraphDataRecuit();
    reinitGrille();
    #if defined(DEBUG_GRAPHE)
        tcout() << "Meilleur resultat du recuit: " << bestCroisement << std::endl;
    #endif
    return debutCroisement - bestCroisement;
}

void Graphe::rerecuitSimuleChallenge(double coolt, double t, double seuil) {
    double cool;
    if (_noeuds.size() <= 30) { cool = 0.9999945; }
    else if (_noeuds.size() <= 100) { cool = 0.999999; }
    else { cool = 0.99999945; }
    int numberOfNoUpgrade = 0, maxIter = 20;
    long lastCroisement;
    if (isNombreCroisementUpdated) { lastCroisement = nombreCroisement; }
    else { lastCroisement = getNbCroisementConst(); }
    int i=1;
    double recuitTimeBest = -1.0;
    auto totalEnd = std::chrono::system_clock::now();
    double startingTemp = t;
    t = 0.05;
    std::filesystem::path resultFolder = std::filesystem::current_path() / "resultats";
    while (numberOfNoUpgrade < maxIter) {
        if (i>1) { reinitGrille(); isAbleToWriteFile = true; }
        if (isAbleToWriteFile && !hasWrittenAFile) { 
            std::string outputName = resultFolder.string() + "/" + to_string(nombreCroisement) + "-" + nomGraphe + ".json";
            writeToJsonChallenge(outputName);
            hasWrittenAFile = true;
        }
        #if defined(DEBUG_GRAPHE)
            tcout() << "Starting Recuit Number: " << i << " t: " << t << " cool " << cool << " Crossings: " << lastCroisement << " NumNoUp: " << numberOfNoUpgrade << std::endl;
        #endif
        long nbInterSuppr = recuitSimuleChallenge(cool, t, seuil);
        t *= coolt;
        if (i==1) { t = startingTemp; }
        if (nbInterSuppr<=0) {
            numberOfNoUpgrade++;
        }
        else {
            lastCroisement = lastCroisement-nbInterSuppr;
            numberOfNoUpgrade = 0;
        }
        i++;
    }
}

long Graphe::recuitSimuleLimite(double &timeBest, std::chrono::time_point<std::chrono::system_clock> start, std::vector<std::vector<double>> customParam, double cool, double t, double seuil, int delay, int modeNoeud, int modeEmplacement, bool useGrille, int timeLimit) {
    auto bestEnd = start; auto end = start; // Timer pour le meilleur resultat trouvé et total
    std::vector<int> bestResultVector; Graphe bestResultGraphe; // Sauvegarde du meilleur graphe.
    saveBestResultRecuit(bestResultVector,bestResultGraphe);
    long nbCroisement, bestCroisement, debutCroisement;
    setupNombreCroisement(nbCroisement,bestCroisement,debutCroisement);
    calculDelaiRefroidissement(delay,customParam,0);
    #if defined(DEBUG_GRAPHE)
        tcout() << "Nb Croisement avant recuit: " << nbCroisement << " cool: " << cool << std::endl;
    #endif
    int nodeId, slotId, idSwappedNode, improve;
    bool swapped;
    Emplacement* oldEmplacement;
    std::chrono::duration<double> secondsTotal = end - start;
    for (int iter = 0; t > seuil && nbCroisement > 0 && ((secondsTotal.count() < timeLimit)||(timeLimit == -1)); iter++) {
        calculDelaiRefroidissement(delay,customParam,iter); // Utile uniquement si customParam[0]==3 et customParam[1]==2 ou 3
        for (int del = 0; del < delay; del++) {
            nodeId = selectionNoeud(modeNoeud, t);
            slotId = selectionEmplacement(modeEmplacement, nodeId, t,customParam,iter);
            oldEmplacement = _noeuds[nodeId].getEmplacement();
            swapped = false;
            idSwappedNode = -1;
            double randDouble = generateDoubleRand(1.0);
            double limiteImprove = -t*std::log(randDouble);
            bool makeMove = false;
            improve = calculImproveLimite(nodeId,slotId,swapped,idSwappedNode,useGrille,makeMove,limiteImprove);
            if (makeMove) {
                nbCroisement += improve;
                if (nbCroisement < bestCroisement) {
                    bestCroisement = nbCroisement;
                    saveBestResultRecuit(bestResultVector,bestResultGraphe);
                    bestEnd = std::chrono::system_clock::now();
                    #if defined(DEBUG_GRAPHE_PROGRESS)
                        tcout() << "Meilleur Recuit: " << bestCroisement << " Iteration: " << iter << " t: " << t << std::endl;
                    #endif
                }
            }
            else {
                if (swapped) {
                    _noeuds[nodeId].swap(oldEmplacement);
                    if (useGrille) { recalcNodeCellule(idSwappedNode); }
                }
                else {
                    _noeuds[nodeId].setEmplacement(oldEmplacement);
                }
                if (useGrille) { recalcNodeCellule(nodeId); }
            }
        }
        t *= cool;
        end = std::chrono::system_clock::now();
        secondsTotal = end - start;
    }
    loadBestResultRecuit(bestResultVector,bestResultGraphe,bestCroisement);
    updateGraphDataRecuit();
    reinitGrille();
    std::chrono::duration<double> secondsBest = bestEnd - start;
    timeBest = secondsBest.count();
    #if defined(DEBUG_GRAPHE)
        tcout() << "Meilleur resultat du recuit: " << bestCroisement << std::endl;
    #endif
    return debutCroisement - bestCroisement;
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
    #if defined(DEBUG_GRAPHE)
        tcout() << "Meilleur resultat de l'algo meilleur deplacement: " << nbIntersection << std::endl;
    #endif
}

// Applique l'algorithme meilleur deplacement sur le graphe.
// On parcoure tout les noeuds et on teste chaque deplacement possible et on effectue le meilleur s'il ameliore le score. (O(n²*e))
// Met a jour le nombre de croisement du graphe.
void Graphe::bestDeplacementLimite() {
    long nbIntersection;
    if (isNombreCroisementUpdated) { nbIntersection = nombreCroisement; }
    else { nbIntersection = getNbCroisementGrid(); }
    tcout() << "Debut Best Deplacement Limite, nombre de croisement: " << nbIntersection << std::endl;
    int bestImprove = TRES_GRANDE_VALEUR;
    double improve;
    bool makeMove;
    int bestNodeId = -1, bestSlotId = -1, idSwappedNode = -1;
    bool swapped;
    int nbTour = 0;
    do {
        bestImprove = TRES_GRANDE_VALEUR;
        for (int nodeId = 0; nodeId < _noeuds.size(); nodeId++) {
            double limiteImprove = -1;
            Emplacement* oldEmplacement = _noeuds[nodeId].getEmplacement();
            for (int slotId = 0; slotId < _emplacements.size(); slotId++) {
                if (slotId != oldEmplacement->getId()) {
                    makeMove = false;
                    swapped = false;
                    idSwappedNode = -1;
                    improve = calculImproveLimite(nodeId,slotId,swapped,idSwappedNode,true,makeMove,limiteImprove);
                    moveNodeToSlot(nodeId,oldEmplacement->getId(),true);
                    if ((makeMove)&&(improve<bestImprove)) {
                        bestImprove = improve;
                        bestNodeId = nodeId;
                        bestSlotId = slotId;
                        limiteImprove = bestImprove - 1;
                        //tcout() << "Recherche Best Deplacement meilleur solution: " << bestImprove << " node: " << bestNodeId << " slot: " << bestSlotId << " tour: " << nbTour << std::endl;
                    }
                }
            }
        }
        if (bestImprove < 0) {
            moveNodeToSlot(bestNodeId,bestSlotId,true);
            nbIntersection += bestImprove;
            tcout() << "Deplacement " << nbTour << " croisements: " << nbIntersection << std::endl;
            std::filesystem::path resultFolder = std::filesystem::current_path() / "resultats";
            std::string outputName = resultFolder.string() + "/" + to_string(nombreCroisement) + "-" + nomGraphe + ".json";
            writeToJsonChallenge(outputName);
        }
        nbTour++;
    } while (bestImprove < TRES_GRANDE_VALEUR);
    nombreCroisement = nbIntersection;
    isNombreCroisementUpdated = true;
    #if defined(DEBUG_GRAPHE)
        tcout() << "Meilleur resultat de l'algo meilleur deplacement: " << nbIntersection << std::endl;
    #endif
}

void Graphe::rechercheTabou() {
    std::vector<int> bestResultVector = saveCopy();
    long nbCroisement, bestCroisement, bestImproveFound;
    nbCroisement = getNbCroisementGrid();
    #if defined(DEBUG_GRAPHE)
        tcout() << "Debut Recherche Tabou, nombre de croisement: " << nbCroisement << std::endl;
    #endif
    bestCroisement = nbCroisement;
    int nodeId, slotId, idSwappedNode, improve;
    int bestNodeId, bestSlotId;
    bool swapped;
    Emplacement* oldEmplacement;
    int nombreIteration = 1000;
    int nbTirageNoeud = _noeuds.size()/10;
    int nbTirageEmplacement = _emplacements.size()/10;
    int tabouTime = 20;
    std::vector<std::vector<int>> tabouVector;
    for (int i=0;i<_noeuds.size();i++) {
        std::vector<int> tmpTabou(_emplacements.size(),0);
        tabouVector.push_back(tmpTabou);
    }
    bool nonTabouFound;
    for (int iteration=0;iteration<nombreIteration;iteration++) {
        std::unordered_set<int> nodesId;
        std::unordered_set<int> slotsId;
        for (int i=0;i<nbTirageNoeud;i++) { nodesId.insert(generateRand(_noeuds.size() - 1)); }
        for (int i=0;i<nbTirageEmplacement;i++) { slotsId.insert(generateRand(_emplacements.size() - 1)); }
        bestImproveFound = TRES_GRANDE_VALEUR;
        nonTabouFound = false;
        for (const int& nodeId : nodesId) {
            oldEmplacement = _noeuds[nodeId].getEmplacement();
            for (const int& slotId : slotsId) {
                if (slotId != oldEmplacement->getId()) {
                    swapped = false;
                    idSwappedNode = -1;
                    improve = calculImproveTabou(nodeId,slotId,swapped,idSwappedNode,true,bestImproveFound);
                    moveNodeToSlot(nodeId,oldEmplacement->getId(),true);
                    if (improve < bestImproveFound) { 
                        bool isTabou = tabouVector[nodeId][slotId] >= iteration;
                        if ((swapped)&&(!isTabou)) { isTabou = tabouVector[idSwappedNode][oldEmplacement->getId()] >= iteration; }
                        if ((nbCroisement+bestImproveFound < bestCroisement)||(!isTabou)) {
                            bestImproveFound = improve;
                            bestNodeId = nodeId;
                            bestSlotId = slotId;
                            nonTabouFound = true;
                        }
                    }
                }
            }
        }
        if (nonTabouFound) {
            int oldSlotId = _emplacements[bestNodeId].getId();
            moveNodeToSlot(bestNodeId,bestSlotId,true);
            nbCroisement = nbCroisement + bestImproveFound;
            if (nbCroisement < bestCroisement) {
                bestCroisement = nbCroisement;
                bestResultVector = saveCopy();
                #if defined(DEBUG_GRAPHE_PROGRESS)
                    tcout() << "Recherche Tabou meilleur solution: " << bestCroisement << " iteration: " << iteration << std::endl;
                #endif
            }
            tabouVector[bestNodeId][oldSlotId] = iteration + tabouTime + generateRand(10) - 5;
        }
    }
    loadCopy(bestResultVector);
    nombreCroisement = bestCroisement;
    #if defined(DEBUG_GRAPHE)
        tcout() << "Fin Recherche Tabou, nombre de croisement: " << nombreCroisement << std::endl;
    #endif
    isNombreCroisementUpdated = true;
}

void Graphe::moveNodeToSlot(int nodeId, int slotId, bool useGrille) {
    if (!_emplacements[slotId].estDisponible()) {
        int idSwappedNode = _emplacements[slotId]._noeud->getId();
        _noeuds[nodeId].swap(&_emplacements[slotId]);
        if (useGrille) { recalcNodeCellule(idSwappedNode); }
    }
    else {
        _noeuds[nodeId].setEmplacement(&_emplacements[slotId]);
    }
    if (useGrille) { recalcNodeCellule(nodeId); }
}
