#include "graphe.hpp"
#include <vector>
#include <chrono>
#include <iostream>
#include <climits>
#include <algorithm>
#include "intersection.hpp"

// Effectue la selection du noeud en fonction de modeNoeud, 0=Aleatoire,1=TournoiBinaire,2=TournoiMultiple
int Graphe::selectionNoeud(int modeNoeud, int t, bool isScoreUpdated) {
    int nodeId=-1;
    if (modeNoeud == 0)
        nodeId = generateRand(_noeuds.size() - 1);
    return nodeId;
}

std::pair<double,double> Graphe::selectionEmplacementReel(int modeEmplacement, int nodeId, int t, std::vector<std::vector<double>>& customParam, int iter) {
    std::pair<double,double> randCoord;
    switch (modeEmplacement) {
    case 0: {
        tirageCoordReel(randCoord);
        return randCoord;
    }
    case 1: {
        std::pair<double,double> nodeCoord({_noeuds[nodeId]._xreel,_noeuds[nodeId]._yreel});
        tirageCoordReel(randCoord);
        double closestDistance = distanceReel(randCoord,nodeCoord);
        std::pair<double,double> randCoordSecond;
        tirageCoordReel(randCoordSecond);
        if (closestDistance >= distanceReel(randCoordSecond,nodeCoord)) { return randCoordSecond; }
        return randCoord;
    }
    case 2: {
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
        std::pair<double,double> nodeCoord({_noeuds[nodeId]._xreel,_noeuds[nodeId]._yreel});
        tirageCoordReel(randCoord);
        double closestDistance = distanceReel(randCoord,nodeCoord);
        std::pair<double,double> randCoordSecond;
        for (int tir=0;tir<nbTirage-1;tir++) {
            tirageCoordReel(randCoordSecond);
            double distance = distanceReel(randCoordSecond,nodeCoord);
            if (closestDistance >= distance) {
                randCoord = randCoordSecond;
                closestDistance = distance;
            }
        }
        return randCoord;
        break;
    }
    case 4: { // Boite autour du noeud
        double sizeX = boiteXSizeDepart - (diffXBoiteIter * iter);
        double sizeY = boiteYSizeDepart - (diffYBoiteIter * iter);
        randCoord.first =  std::min(std::max(_noeuds[nodeId]._xreel + generateDoubleRand(sizeX*2)-sizeX,0.0),(double)gridWidth);
        randCoord.second =  std::min(std::max(_noeuds[nodeId]._yreel + generateDoubleRand(sizeY*2)-sizeY,0.0),(double)gridHeight);
    }
    }
    return randCoord;
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

void Graphe::setupSelectionEmplacement(int modeEmplacement, double t, double cool, double seuil, std::vector<std::vector<double>>& customParam) {
    if (modeEmplacement == 4) {
        std::pair<double,double> sizeGraphe = sizeOfGraphe();
        int nbIterationRecuit = nombreIterationRecuit(t, cool, seuil) ;
        double coeffDepart = 0.75, coeffArrivee = 0.25;
        if (customParam.size() > 0) {
            for (std::vector<double>& param : customParam) {
                if (param.size() > 0) {
                    if (param[0] == 6) {
                        coeffDepart = param[1];
                    }
                    else if (param[0] == 7) {
                        coeffArrivee = param[1];
                    }
                }
            }
        }
        boiteXSizeDepart = sizeGraphe.first*coeffDepart;
        boiteYSizeDepart = sizeGraphe.second*coeffDepart;
        diffXBoiteIter = (boiteXSizeDepart - coeffArrivee*(boiteXSizeDepart/10.0))/(double)nbIterationRecuit;
        diffYBoiteIter = (boiteYSizeDepart - coeffArrivee*(boiteYSizeDepart/10.0))/(double)nbIterationRecuit;
    }
}

long Graphe::calculScoreNodeMethode(int nodeId, int idSwappedNode, bool swapped, bool useGrille, bool useScore, bool useReel) {
    if (useGrille) { return getScoreCroisementNodeGridReel(nodeId); }
    else if (useScore) { return _noeuds[nodeId].score; }
    else { return getScoreCroisementNodeReel(nodeId); }
}

long Graphe::calculScoreNodeMethodeLimite(int nodeId, int idSwappedNode, bool swapped, bool useGrille, bool& makeMove, double limiteScore,bool useReel) {
    if (useGrille) { return getScoreCroisementNodeGridReelLimite(nodeId,makeMove,limiteScore); }
    //else { return getScoreCroisementNodeReel(nodeId); }
    return -1;
}

long Graphe::calculImproveReel(int nodeId,std::pair<double,double>& randCoord, bool useGrille,bool useScore) {
    long scoreNode = calculScoreNodeMethode(nodeId,-1,false,useGrille,useScore,true);
    _noeuds[nodeId].setCoordReel(randCoord);
    if (useGrille) { recalcNodeCelluleReel(nodeId); }
    long newScoreNode = calculScoreNodeMethode(nodeId,-1,false,useGrille,useScore,true);
    return newScoreNode - scoreNode;
}

long Graphe::calculImproveReelLimite(int nodeId,std::pair<double,double>& randCoord,bool useGrille,bool& makeMove,double limiteImprove) {
    long scoreNode = calculScoreNodeMethode(nodeId,-1,false,useGrille,false,true);
    _noeuds[nodeId].setCoordReel(randCoord);
    if (useGrille) { recalcNodeCelluleReel(nodeId); }
    long limiteScore = scoreNode + limiteImprove;
    if (limiteScore > 0) {
        long newScoreNode =  calculScoreNodeMethodeLimite(nodeId,-1,false,useGrille,makeMove,limiteScore,useCoordReel);
        return newScoreNode - scoreNode;
    }
    return -1;
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

void Graphe::saveBestResultRecuitReel(std::vector<std::pair<double,double>>& bestResultVector) {
    bestResultVector.clear();
    for (int i=0;i<_noeuds.size();i++) {
        bestResultVector.push_back({_noeuds[i]._xreel,_noeuds[i]._yreel});
    }
}

void Graphe::loadBestResultRecuitReel(std::vector<std::pair<double,double>>& bestResultVector, long bestNbCroisement) {
    nombreCroisement = bestNbCroisement;
    for (int i=0;i<_noeuds.size();i++) {
        _noeuds[i].setCoordReel(bestResultVector[i]);
    }
}

void Graphe::updateGraphDataRecuit(bool useScore, bool useGrille) {
    isNombreCroisementUpdated = true;
}

void Graphe::setupNombreCroisement(long& nbCroisement, long& bestCroisement, long& debutCroisement) {
    if (isNombreCroisementUpdated) { nbCroisement = nombreCroisement; }
    else { nbCroisement = getNbCroisementReel(); }
    bestCroisement = nbCroisement;
    debutCroisement = nbCroisement;
}

// Lance l'algorithme de recuit simulé sur le graphe pour minimiser le nombre d'intersection
// Met à jour la variable nombreCroisement du graphe.
// modeNoeud et modeEMplacement sont le mode de sélection de noeud et d'emplacement, 0=Aléatoire, 1=TournoiBinaire, 2=TournoiMultiple, 3=Triangulation(Emplacement uniquement)
// Par defaut utilise la grille et le Tournoi Multiple sur les Emplacements.
long Graphe::recuitSimuleReel(double &timeBest, std::chrono::time_point<std::chrono::system_clock> start, std::vector<std::vector<double>> customParam, double cool, double t, double seuil, int delay, int modeNoeud, int modeEmplacement, bool useGrille, bool useScore, int timeLimit) {
    auto bestEnd = start; auto end = start; // Timer pour le meilleur resultat trouvé et total
    std::vector<std::pair<double,double>> bestResultVector;
    saveBestResultRecuitReel(bestResultVector);
    long nbCroisement, bestCroisement, debutCroisement;
    setupNombreCroisement(nbCroisement,bestCroisement,debutCroisement);
    double coeffImprove = 1.0;
    applyRecuitCustomParam(coeffImprove,customParam);
    calculDelaiRefroidissement(delay,customParam,0);
    setupSelectionEmplacement(modeEmplacement,t,cool,seuil,customParam);
    #if defined(DEBUG_GRAPHE)
        tcout() << "Debut Recuit Simule Reel.\n";
    #endif
    int nodeId, improve;
    std::pair<double,double> randCoord;
    std::chrono::duration<double> secondsTotal = end - start;
    for (int iter = 0; t > seuil && nbCroisement > 0 && ((secondsTotal.count() < timeLimit)||(timeLimit == -1)); iter++) {
        calculDelaiRefroidissement(delay,customParam,iter); // Utile uniquement si customParam[0]==3 et customParam[1]==2 ou 3
        for (int del = 0; del < delay; del++) {
            nodeId = selectionNoeud(modeNoeud, t);
            std::pair<double,double> oldCoord({_noeuds[nodeId]._xreel,_noeuds[nodeId]._yreel});
            randCoord = selectionEmplacementReel(modeEmplacement, nodeId, t,customParam,iter);
            //double TMPDISTANCE = distanceReelSqrt(randCoord,oldCoord);
            //recuitDistanceAll.push_back(TMPDISTANCE);
            improve = calculImproveReel(nodeId,randCoord,useGrille,useScore);
            if (improve <= 0) {
                nbCroisement += improve;
                //recuitDistanceUpgrade.push_back(make_pair(iter,TMPDISTANCE));
                if (nbCroisement < bestCroisement) {
                    //recuitDistanceUpgradeGlobal.push_back(make_pair(iter,TMPDISTANCE));
                    bestCroisement = nbCroisement;
                    saveBestResultRecuitReel(bestResultVector);
                    bestEnd = std::chrono::system_clock::now();
                    
                    //std::chrono::duration<double> tmpBestTime = bestEnd - start;
                    //recuitScoreTemps.push_back(make_pair(bestCroisement,tmpBestTime.count()));
                    
                    #if defined(DEBUG_GRAPHE_PROGRESS)
                        tcout() << "Meilleur Recuit: " << bestCroisement << " Iteration: " << iter << " t: " << t << std::endl;
                    #endif
                }
            }
            else {
                double randDouble = generateDoubleRand(1.0);
                //double TMPDISTANCE = exp(-improve / t);
                //recuitDistanceUpgrade.push_back(make_pair(iter,TMPDISTANCE));
                double valImprove = exp(-improve / t) * coeffImprove;
                if (randDouble >= valImprove) {
                    _noeuds[nodeId].setCoordReel(oldCoord);
                    if (useGrille) { recalcNodeCelluleReel(nodeId); }
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
    loadBestResultRecuitReel(bestResultVector,bestCroisement);
    updateGraphDataRecuit(useScore,useGrille);
    std::chrono::duration<double> secondsBest = bestEnd - start;
    timeBest = secondsBest.count();
    #if defined(DEBUG_GRAPHE)
        tcout() << "Fin Vague Recuit Simule Reel.\n";
    #endif
    return debutCroisement - bestCroisement;
}

// Applique le recuit simulé plusieurs fois
// Met a jour le nombre de croisement du graphe.
void Graphe::rerecuitSimuleReel(double &timeBest,int &nombreRecuit,std::chrono::time_point<std::chrono::system_clock> start,std::vector<std::vector<double>> customParam, int iter, double cool, double coolt, double t, double seuil, int delay, int modeNoeud, int modeEmplacement, bool useGrille, bool useScore, int timeLimit, bool firstWaveImp, bool adaptCool) {
    auto end = start;
    applyRerecuitCustomParam(t,cool,coolt,seuil,adaptCool,customParam);
    nombreRecuit= 0;
    int numberOfNoUpgrade = 0, maxIter = 10;
    #if defined(DEBUG_GRAPHE)
        if (iter == -1) { tcout() << "Starting Rerecuit Simule Reel " << maxIter << " reheating no upgrade.\n"; }
        else { tcout() << "Starting Rerecuit Simule Reel " << iter << " max iterations.\n"; }
    #endif
    if (iter != -1) { maxIter = iter; }
    long lastCroisement;
    if (isNombreCroisementUpdated) { lastCroisement = nombreCroisement; }
    else { lastCroisement = getNbCroisementReelConst(); }
    int i=1;
    double recuitTimeBest = -1.0;
    auto totalEnd = std::chrono::system_clock::now();
	std::chrono::duration<double> secondsTotalExec = totalEnd - start;
    double startingTemp = t;
    if (firstWaveImp) { t = 0.05; }
    while ((numberOfNoUpgrade < maxIter)&&((timeLimit == -1)||(secondsTotalExec.count() < timeLimit))) {
        if (useGrille) { if (i>1) { reinitGrilleReel(); } }
        #if defined(DEBUG_GRAPHE)
            tcout() << "Starting Recuit Number: " << i << " t: " << t << " cool " << cool << " Crossings: " << lastCroisement << " NumNoUp: " << numberOfNoUpgrade << std::endl;
        #endif
        nombreRecuit++;
        long nbInterSuppr = recuitSimuleReelLimite(recuitTimeBest,start,customParam, cool, t, seuil, delay, modeNoeud, modeEmplacement, useGrille, useScore,timeLimit);
        t *= coolt;
        if ((firstWaveImp)&&(i==1)) { t = startingTemp; }
        if (iter != -1) { numberOfNoUpgrade++; }
        else {
            long newCroisement;
            if (isNombreCroisementUpdated) { newCroisement = nombreCroisement; }
            else { newCroisement = getNbCroisementReelConst(); }
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
}

long Graphe::recuitSimuleReelLimite(double &timeBest, std::chrono::time_point<std::chrono::system_clock> start, std::vector<std::vector<double>> customParam, double cool, double t, double seuil, int delay, int modeNoeud, int modeEmplacement, bool useGrille, bool useScore, int timeLimit) {
    auto bestEnd = start; auto end = start; // Timer pour le meilleur resultat trouvé et total
    std::vector<std::pair<double,double>> bestResultVector;
    saveBestResultRecuitReel(bestResultVector);
    long nbCroisement, bestCroisement, debutCroisement;
    setupNombreCroisement(nbCroisement,bestCroisement,debutCroisement);
    calculDelaiRefroidissement(delay,customParam,0);
    setupSelectionEmplacement(modeEmplacement,t,cool,seuil,customParam);
    #if defined(DEBUG_GRAPHE_RECUIT_PROGRESS)
        tcout() << "Nb Croisement avant recuit: " << nbCroisement << std::endl;
    #endif
    int nodeId, improve;
    std::pair<double,double> randCoord;
    std::pair<double,double> oldCoord;
    std::chrono::duration<double> secondsTotal = end - start;
    double randDouble, limiteImprove;
    bool makeMove;
    for (int iter = 0; t > seuil && nbCroisement > 0 && ((secondsTotal.count() < timeLimit)||(timeLimit == -1)); iter++) {
        nodeId = selectionNoeud(modeNoeud, t);
        oldCoord.first = _noeuds[nodeId]._xreel;
        oldCoord.second = _noeuds[nodeId]._yreel;
        randCoord = selectionEmplacementReel(modeEmplacement, nodeId, t,customParam,iter);
        randDouble = generateDoubleRandRecuitImprove();
        limiteImprove = -t*std::log(randDouble);
        makeMove = false;
        improve = calculImproveReelLimite(nodeId,randCoord,useGrille,makeMove,limiteImprove);
        if (makeMove) {
            nbCroisement += improve;
            if (nbCroisement < bestCroisement) {
                bestCroisement = nbCroisement;
                saveBestResultRecuitReel(bestResultVector);
                bestEnd = std::chrono::system_clock::now();
                #if defined(DEBUG_GRAPHE_PROGRESS)
                    tcout() << "Meilleur Recuit: " << bestCroisement << " Iteration: " << iter << " t: " << t << std::endl;
                #endif
            }
        }
        else {
            _noeuds[nodeId].setCoordReel(oldCoord);
            if (useGrille) { recalcNodeCelluleReel(nodeId); }
        }
        t *= cool;
        end = std::chrono::system_clock::now();
        secondsTotal = end - start;
    }
    loadBestResultRecuitReel(bestResultVector,bestCroisement);
    updateGraphDataRecuit(useScore,useGrille);
    std::chrono::duration<double> secondsBest = bestEnd - start;
    timeBest = secondsBest.count();
    #if defined(DEBUG_GRAPHE_RECUIT_PROGRESS)
        tcout() << "Meilleur resultat du recuit: " << bestCroisement << std::endl;
    #endif
    return debutCroisement - bestCroisement;
}