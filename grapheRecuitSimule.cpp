#include "graphe.hpp"
#include <vector>
#include <chrono>
#include <iostream>
#include <climits>

// Fait une estimation du temps requis pour effectuer un recuit simule complet
void Graphe::tempsCalculRecuitSimule(double cool, double t, int mode) {
    std::vector<int> bestResult = saveCopy();
    int nbCroisement = getNbCroisement();

    auto start = std::chrono::system_clock::now();
    // DEBUT RECUIT UNE ITERATION
    for (int iter = 0; iter < 10000 && nbCroisement > 0; iter++) {
        int randomId;
        if (mode == 0)
            randomId = generateRand(_noeuds.size() - 1); // Selection aléatoire du noeud
        else if (mode == 1) {
            randomId = selectionNoeudTournoiBinaire();
        }
        else if (mode == 2) {
            int nbTirageNoeud = ((100 - t) / 15) + 1;
            randomId = selectionNoeudTournoiMultiple(nbTirageNoeud);
        }
        int randomEmpId = generateRand(_emplacementsPossibles.size() - 1); // Selection aléatoire d'un emplacement disponible (pas tres équiprobable)
        // on retire si on pioche le meme emplacement
        while (_noeuds[randomId].getEmplacement()->getId() == randomEmpId) {
            randomEmpId = generateRand(_emplacementsPossibles.size() - 1);
        }
        int scoreNode;
        bool swapped = false;
        int idSwappedNode = -1;
        Emplacement* oldEmplacement = _noeuds[randomId].getEmplacement();
        if (!_emplacementsPossibles[randomEmpId].estDisponible()) {
            idSwappedNode = _emplacementsPossibles[randomEmpId]._noeud->getId();
            scoreNode = getScoreCroisementNode(randomId, idSwappedNode);
            scoreNode += getScoreCroisementNode(idSwappedNode);
            _noeuds[randomId].swap(&_emplacementsPossibles[randomEmpId]);
            swapped = true;
        }
        else {
            scoreNode = getScoreCroisementNode(randomId);
            _noeuds[randomId].setEmplacement(&_emplacementsPossibles[randomEmpId]);
        }
        int newScoreNode;
        if (swapped) {
            newScoreNode = getScoreCroisementNode(randomId, idSwappedNode);
            newScoreNode += getScoreCroisementNode(idSwappedNode);
        }
        else {
            newScoreNode = getScoreCroisementNode(randomId);
        }
        int improve = newScoreNode - scoreNode;
        if (improve < 0) {
            bestResult = saveCopy();
            nbCroisement += improve;
        }
        else {
            double randDouble = generateDoubleRand(1.0);
            if (randDouble >= exp(-improve / t)) {
                if (swapped) {
                    _noeuds[randomId].swap(oldEmplacement);
                }
                else {
                    _noeuds[randomId].setEmplacement(oldEmplacement);
                }
            }
            else {
                nbCroisement += improve;
            }
        }
        t *= cool;
    }
    // FIN RECUIT 100 ITERATIONS
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> secondsTotal = end - start;
    std::cout << "Prevision: " << secondsTotal.count() * 138.1545 << " secondes." << std::endl;
}

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
    int scoreMeilleur;
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
            int scoreId2;
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
    int randomEmpId = generateRand(_emplacementsPossibles.size() - 1); // Selection aléatoire d'un emplacement disponible (pas tres équiprobable)
    // on retire si on pioche le meme emplacement
    while (_noeuds[nodeId].getEmplacement()->getId() == randomEmpId) {
        randomEmpId = generateRand(_emplacementsPossibles.size() - 1);
    }
    int randomEmpId2 = generateRand(_emplacementsPossibles.size() - 1); // Selection aléatoire d'un emplacement disponible (pas tres équiprobable)
    // on retire si on pioche le meme emplacement
    while ((_noeuds[nodeId].getEmplacement()->getId() == randomEmpId2)||(randomEmpId2 == randomEmpId)) {
        randomEmpId2 = generateRand(_emplacementsPossibles.size() - 1);
    }
    double dist1 = ((_emplacementsPossibles[randomEmpId].getX() - _noeuds[nodeId].getX()) * (_emplacementsPossibles[randomEmpId].getX() - _noeuds[nodeId].getX())) + ((_emplacementsPossibles[randomEmpId].getY() - _noeuds[nodeId].getY()) * (_emplacementsPossibles[randomEmpId].getY() - _noeuds[nodeId].getY()));
    double dist2 = ((_emplacementsPossibles[randomEmpId2].getX() - _noeuds[nodeId].getX()) * (_emplacementsPossibles[randomEmpId2].getX() - _noeuds[nodeId].getX())) + ((_emplacementsPossibles[randomEmpId2].getY() - _noeuds[nodeId].getY()) * (_emplacementsPossibles[randomEmpId2].getY() - _noeuds[nodeId].getY()));
    if (dist2 < dist1) {
        randomEmpId = randomEmpId2;
    }
    return randomEmpId;
}

// Selectionne n emplacements different entre eux et de celui du noeud, puis renvoie le plus proche du noeud.
// Le noeud doit etre place.
int Graphe::selectionEmplacementTournoiMultiple(int n, int nodeId) {
    int randomEmpId = generateRand(_emplacementsPossibles.size() - 1); // Selection aléatoire d'un emplacement disponible (pas tres équiprobable)
    // on retire si on pioche le meme emplacement
    while (_noeuds[nodeId].getEmplacement()->getId() == randomEmpId) {
        randomEmpId = generateRand(_emplacementsPossibles.size() - 1);
    }
    double dist;
    if (n > 1) {
        dist = ((_emplacementsPossibles[randomEmpId].getX() - _noeuds[nodeId].getX()) * (_emplacementsPossibles[randomEmpId].getX() - _noeuds[nodeId].getX())) + ((_emplacementsPossibles[randomEmpId].getY() - _noeuds[nodeId].getY()) * (_emplacementsPossibles[randomEmpId].getY() - _noeuds[nodeId].getY()));
    }
    for (int i = 1; i < n; i++) {
        int randomEmpId2 = generateRand(_emplacementsPossibles.size() - 1); // Selection aléatoire d'un emplacement disponible (pas tres équiprobable)
        // on retire si on pioche le meme emplacement
        while ((_noeuds[nodeId].getEmplacement()->getId() == randomEmpId2)||(randomEmpId2 == randomEmpId)) {
            randomEmpId2 = generateRand(_emplacementsPossibles.size() - 1);
        }
        double dist2 = ((_emplacementsPossibles[randomEmpId2].getX() - _noeuds[nodeId].getX()) * (_emplacementsPossibles[randomEmpId2].getX() - _noeuds[nodeId].getX())) + ((_emplacementsPossibles[randomEmpId2].getY() - _noeuds[nodeId].getY()) * (_emplacementsPossibles[randomEmpId2].getY() - _noeuds[nodeId].getY()));
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
    for (int i=0;i<profondeur;i++) {
        if (arrive->voisinsDelaunay.size() == 1) {
            if (arrive->voisinsDelaunay[0] == depart->_id) {
                return arrive->_id;
            }
        }
        int randomId = generateRand(arrive->voisinsDelaunay.size() - 1);
        while (arrive->voisinsDelaunay[randomId] == depart->_id) {
            randomId = generateRand(arrive->voisinsDelaunay.size() - 1);
        }
        arrive = &_emplacementsPossibles[arrive->voisinsDelaunay[randomId]];
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
int Graphe::selectionEmplacement(int modeEmplacement, int nodeId, int t, std::vector<int> customParam, int iter) {
    int slotId;
    switch (modeEmplacement) {
    case 0: {
        slotId = generateRand(_emplacementsPossibles.size() - 1); // Selection aléatoire d'un emplacement disponible (pas tres équiprobable)
        // on retire si on pioche le meme emplacement
        while (_noeuds[nodeId].getEmplacement()->getId() == slotId) {
            slotId = generateRand(_emplacementsPossibles.size() - 1);
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
        slotId = selectionEmplacementTriangulation(nodeId);
        break;
    }
    }
    return slotId;
}

// Lance l'algorithme de recuit simulé sur le graphe pour minimiser le nombre d'intersection
// Met à jour la variable nombreCroisement du graphe.
// delay est le nombre de tour auquel on reste à la même température, -1 pour le rendre dynamique en fonction de la taille du graphe.
// modeNoeud et modeEMplacement sont le mode de sélection de noeud et d'emplacement, 0=Aléatoire, 1=TournoiBinaire, 2=TournoiMultiple
void Graphe::recuitSimule(double &timeBest, double cool, double t, int delay, int modeNoeud, int modeEmplacement) {
    auto start = std::chrono::system_clock::now();
    auto end = start;
    std::vector<int> bestResult = saveCopy();
    long nbCroisement;
    if (isNombreCroisementUpdated) {
        nbCroisement = nombreCroisement;
    }
    else {
        nbCroisement = getNbCroisement();
    }
    long bestCroisement = nbCroisement;
    if (delay == -1) { // -1 on calcule le delay en fonction de la taille du graphe
        delay = ceil((double)_noeuds.size() / 20.0) + 1;
    }
    if (DEBUG_GRAPHE) std::cout << "Nb Croisement avant recuit: " << nbCroisement << std::endl;
    for (int iter = 0; t > 0.0001 && nbCroisement > 0; iter++) {
        //std::cout << "Iter: " << iter << " t: " << t << " intersections: " << nbCroisement << std::endl;
        for (int del = 0; del < delay; del++) {
            int nodeId = selectionNoeud(modeNoeud, t);
            int slotId = selectionEmplacement(modeEmplacement, nodeId, t);
            int scoreNode;
            bool swapped = false;
            int idSwappedNode = -1;
            Emplacement* oldEmplacement = _noeuds[nodeId].getEmplacement();
            if (!_emplacementsPossibles[slotId].estDisponible()) {
                idSwappedNode = _emplacementsPossibles[slotId]._noeud->getId();
                scoreNode = getScoreCroisementNode(nodeId, idSwappedNode);
                scoreNode += getScoreCroisementNode(idSwappedNode);
                _noeuds[nodeId].swap(&_emplacementsPossibles[slotId]);
                swapped = true;
            }
            else {
                scoreNode = getScoreCroisementNode(nodeId);
                _noeuds[nodeId].setEmplacement(&_emplacementsPossibles[slotId]);
            }
            int newScoreNode;
            if (swapped) {
                newScoreNode = getScoreCroisementNode(nodeId, idSwappedNode);
                newScoreNode += getScoreCroisementNode(idSwappedNode);
            }
            else {
                newScoreNode = getScoreCroisementNode(nodeId);
            }
            int improve = newScoreNode - scoreNode;
            if (improve < 0) {
                nbCroisement += improve;
                if (nbCroisement < bestCroisement) {
                    if (swapped) { compteurSwapE++; } else { compteurDeplacementE++; }
                    bestCroisement = nbCroisement;
                    bestResult = saveCopy();
                    end = std::chrono::system_clock::now();
                    if (DEBUG_PROGRESS) std::cout << "Meilleur Recuit: " << bestCroisement << " Iteration: " << iter << " t: " << t << std::endl;
                }
            }
            else {
                double randDouble = generateDoubleRand(1.0);
                if (randDouble >= exp(-improve / t)) {
                    if (swapped) {
                        _noeuds[nodeId].swap(oldEmplacement);
                    }
                    else {
                        _noeuds[nodeId].setEmplacement(oldEmplacement);
                    }
                }
                else {
                    nbCroisement += improve;
                    if (swapped) { compteurSwapE++; } else { compteurDeplacementE++; }
                }
            }
            if (swapped) { compteurSwap++; } else { compteurDeplacement++; }
        }
        t *= cool;
    }
    loadCopy(bestResult);
    nombreCroisement = bestCroisement;
    isNombreCroisementUpdated = true;
    isNodeScoreUpdated = false;
    isIntersectionVectorUpdated = false;
    std::chrono::duration<double> secondsTotal = end - start;
    timeBest = secondsTotal.count();
    if (DEBUG_GRAPHE) std::cout << "Meilleur resultat du recuit: " << bestCroisement << std::endl;
    if (DEBUG_PROGRESS) std::cout << "Nombre de swap: " << compteurSwap << std::endl;
    if (DEBUG_PROGRESS) std::cout << "Nombre de deplacement: " << compteurDeplacement << std::endl;
    if (DEBUG_PROGRESS) std::cout << "Nombre de swap effectue: " << compteurSwapE << std::endl;
    if (DEBUG_PROGRESS) std::cout << "Nombre de deplacement effectue: " << compteurDeplacementE << std::endl;
}

// Lance l'algorithme de recuit simulé sur le graphe pour minimiser le nombre d'intersection
// Met à jour la variable nombreCroisement du graphe.
// delay est le nombre de tour auquel on reste à la même température, -1 pour le rendre dynamique en fonction de la taille du graphe.
// modeNoeud et modeEMplacement sont le mode de sélection de noeud et d'emplacement, 0=Aléatoire, 1=TournoiBinaire, 2=TournoiMultiple
void Graphe::recuitSimuleCustom(double &timeBest, double cool, double t, int delay, int modeNoeud, int modeEmplacement, std::vector<int> customParam) {
    auto start = std::chrono::system_clock::now();
    auto end = start;
    std::vector<int> bestResult = saveCopy();
    long nbCroisement;
    if (isNombreCroisementUpdated) {
        nbCroisement = nombreCroisement;
    }
    else {
        nbCroisement = getNbCroisement();
    }
    long bestCroisement = nbCroisement;
    if (delay == -1) { // -1 on calcule le delay en fonction de la taille du graphe
        if (customParam[0] == 3) {
            delay = customParam[1];
        }
        else if (customParam[0] == 4) {
            delay = ceil((double)_noeuds.size() / customParam[1]) + 1;
        }
        else {
            delay = ceil((double)_noeuds.size() / 20.0) + 1;
        }
    }
    if (DEBUG_GRAPHE) std::cout << "Nb Croisement avant recuit: " << nbCroisement << std::endl;
    for (int iter = 0; t > 0.0001 && nbCroisement > 0; iter++) {
        if (customParam[1] == 5) {
            delay = (iter / 100000) + customParam[1];
        }
        else if (customParam[1] == 6) {
            delay = ((1381545-iter) / 100000) + customParam[1];
        }
        //if (iter % 20000 == 0) {
        //    std::cout << "Iter: " << iter << " t: " << t << " intersections: " << nbCroisement << std::endl;
        //}
        for (int del = 0; del < delay; del++) {
            int nodeId = selectionNoeud(modeNoeud, t);
            int slotId = selectionEmplacement(modeEmplacement, nodeId, t,customParam,iter);
            int scoreNode;
            bool swapped = false;
            int idSwappedNode = -1;
            Emplacement* oldEmplacement = _noeuds[nodeId].getEmplacement();
            if (!_emplacementsPossibles[slotId].estDisponible()) {
                idSwappedNode = _emplacementsPossibles[slotId]._noeud->getId();
                scoreNode = getScoreCroisementNode(nodeId, idSwappedNode);
                scoreNode += getScoreCroisementNode(idSwappedNode);
                _noeuds[nodeId].swap(&_emplacementsPossibles[slotId]);
                swapped = true;
            }
            else {
                scoreNode = getScoreCroisementNode(nodeId);
                _noeuds[nodeId].setEmplacement(&_emplacementsPossibles[slotId]);
            }
            int newScoreNode;
            if (swapped) {
                newScoreNode = getScoreCroisementNode(nodeId, idSwappedNode);
                newScoreNode += getScoreCroisementNode(idSwappedNode);
            }
            else {
                newScoreNode = getScoreCroisementNode(nodeId);
            }
            int improve = newScoreNode - scoreNode;
            if (improve < 0) {
                nbCroisement += improve;
                if (nbCroisement < bestCroisement) {
                    if (swapped) { compteurSwapE++; } else { compteurDeplacementE++; }
                    bestCroisement = nbCroisement;
                    bestResult = saveCopy();
                    end = std::chrono::system_clock::now();
                    if (DEBUG_PROGRESS) std::cout << "Meilleur Recuit: " << bestCroisement << " Iteration: " << iter << " t: " << t << std::endl;
                }
            }
            else {
                double randDouble = generateDoubleRand(1.0);
                if (randDouble >= exp(-improve / t)) {
                    if (swapped) {
                        _noeuds[nodeId].swap(oldEmplacement);
                    }
                    else {
                        _noeuds[nodeId].setEmplacement(oldEmplacement);
                    }
                }
                else {
                    nbCroisement += improve;
                    if (swapped) { compteurSwapE++; } else { compteurDeplacementE++; }
                }
            }
            if (swapped) { compteurSwap++; } else { compteurDeplacement++; }
        }
        t *= cool;
    }
    loadCopy(bestResult);
    nombreCroisement = bestCroisement;
    isNombreCroisementUpdated = true;
    isNodeScoreUpdated = false;
    isIntersectionVectorUpdated = false;
    std::chrono::duration<double> secondsTotal = end - start;
    timeBest = secondsTotal.count();
    if (DEBUG_GRAPHE) std::cout << "Meilleur resultat du recuit: " << bestCroisement << std::endl;
    if (DEBUG_PROGRESS) std::cout << "Nombre de swap: " << compteurSwap << std::endl;
    if (DEBUG_PROGRESS) std::cout << "Nombre de deplacement: " << compteurDeplacement << std::endl;
    if (DEBUG_PROGRESS) std::cout << "Nombre de swap effectue: " << compteurSwapE << std::endl;
    if (DEBUG_PROGRESS) std::cout << "Nombre de deplacement effectue: " << compteurDeplacementE << std::endl;
}

// Lance l'algorithme de recuit simulé sur le graphe pour minimiser le nombre d'intersection
// Met à jour la variable nombreCroisement du graphe si elle etait a jour avant.
void Graphe::stepRecuitSimule(double& t, int& nbCrois, double cool, int modeNoeud, int modeEmplacement) {
    std::vector<int> bestResult = saveCopy();
    int nbCroisement = nbCrois;
    if (DEBUG_GRAPHE) std::cout << "Nb Croisement avant recuit: " << nbCroisement << std::endl;
    int randomId = selectionNoeud(modeNoeud, t);
    int randomEmpId = selectionEmplacement(modeEmplacement, randomId, t);
    int scoreNode;
    bool swapped = false;
    int idSwappedNode = -1;
    Emplacement* oldEmplacement = _noeuds[randomId].getEmplacement();
    if (!_emplacementsPossibles[randomEmpId].estDisponible()) {
        idSwappedNode = _emplacementsPossibles[randomEmpId]._noeud->getId();
        scoreNode = getScoreCroisementNode(randomId, idSwappedNode);
        scoreNode += getScoreCroisementNode(idSwappedNode);
        _noeuds[randomId].swap(&_emplacementsPossibles[randomEmpId]);
        swapped = true;
    }
    else {
        scoreNode = getScoreCroisementNode(randomId);
        _noeuds[randomId].setEmplacement(&_emplacementsPossibles[randomEmpId]);
    }
    int newScoreNode;
    if (swapped) {
        newScoreNode = getScoreCroisementNode(randomId, idSwappedNode);
        newScoreNode += getScoreCroisementNode(idSwappedNode);
    }
    else {
        newScoreNode = getScoreCroisementNode(randomId);
    }
    int improve = newScoreNode - scoreNode;
    if (improve < 0) {
        bestResult = saveCopy();
        nbCroisement += improve;
    }
    else {
        double randDouble = generateDoubleRand(1.0);
        if (randDouble >= exp(-improve / t)) {
            if (swapped) {
                _noeuds[randomId].swap(oldEmplacement);
            }
            else {
                _noeuds[randomId].setEmplacement(oldEmplacement);
            }
        }
        else {
            nbCroisement += improve;
        }
    }
    t *= cool;
    if (isNombreCroisementUpdated) {
        nombreCroisement = nbCroisement;
        isNombreCroisementUpdated = true;
    }
    loadCopy(bestResult);
}

// Applique le recuit simulé plusieurs fois
// Met a jour le nombre de croisement du graphe.
void Graphe::rerecuitSimule(double &timeBest, int iter, double cool, double coolt, double t, int delay, int modeNoeud, int modeEmplacement) {
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
    while (numberOfNoUpgrade < maxIter) {
        if (DEBUG_GRAPHE) std::cout << "Starting Recuit Number: " << i << " t: " << t << " cool " << cool << " NumNoUp: " << numberOfNoUpgrade << std::endl;
        recuitSimule(recuitTimeBest, cool, t, delay, modeNoeud, modeEmplacement);
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
    }
    std::chrono::duration<double> secondsTotal = end - start;
    timeBest = secondsTotal.count();
}

// Applique le recuit simulé plusieurs fois
// Met a jour le nombre de croisement du graphe.
void Graphe::rerecuitSimuleCustom(double &timeBest, int iter, double cool, double coolt, double t, int delay, int modeNoeud, int modeEmplacement, std::vector<int> customParam) {
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
    while (numberOfNoUpgrade < maxIter) {
        if (DEBUG_GRAPHE) std::cout << "Starting Recuit Number: " << i << " t: " << t << " cool " << cool << " NumNoUp: " << numberOfNoUpgrade << std::endl;
        recuitSimuleCustom(recuitTimeBest, cool, t, delay, modeNoeud, modeEmplacement,customParam);
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
    }
    std::chrono::duration<double> secondsTotal = end - start;
    timeBest = secondsTotal.count();
}

// Lance l'algorithme de recuit simulé sur le graphe pour minimiser le nombre d'intersection
// Met à jour le score du graphe et des noeuds
// delay est le nombre de tour auquel on reste à la même température, -1 pour le rendre dynamique en fonction de la taille du graphe.
// modeNoeud et modeEMplacement sont le mode de sélection de noeud et d'emplacement, 0=Aléatoire, 1=TournoiBinaire, 2=TournoiMultiple
void Graphe::recuitSimuleScore(double &timeBest, double cool, double t, int delay, int modeNoeud, int modeEmplacement) {
    auto start = std::chrono::system_clock::now();
    auto end = start;
    Graphe bestResult;
    bestResult.copyFromGraphe(*this);
    long nbCroisement = nombreCroisement;
    long bestCroisement = nbCroisement;
    if (delay == -1) {
        delay = ceil((double)_noeuds.size() / 20.0) + 1;
    }
    for (int iter = 0; t > 0.0001 && nbCroisement > 0; iter++) {
        for (int del = 0; del < delay; del++) {
            int nodeId = selectionNoeud(modeNoeud, t, true);
            int slotId = selectionEmplacement(modeEmplacement, nodeId, t);
            int scoreNode;
            bool swapped = false;
            int idSwappedNode = -1;
            Emplacement* oldEmplacement = _noeuds[nodeId].getEmplacement();
            if (!_emplacementsPossibles[slotId].estDisponible()) {
                idSwappedNode = _emplacementsPossibles[slotId]._noeud->getId();
                scoreNode = getNodeLinkedScore(nodeId, idSwappedNode);
                scoreNode += _noeuds[idSwappedNode].score;
                _noeuds[nodeId].swap(&_emplacementsPossibles[slotId]);
                swapped = true;
                changeUpdateValue(nodeId);
                changeUpdateValue(idSwappedNode);
                updateNodeScore(idSwappedNode);
            }
            else {
                scoreNode = _noeuds[nodeId].score;
                _noeuds[nodeId].setEmplacement(&_emplacementsPossibles[slotId]);
                changeUpdateValue(nodeId);
            }
            updateNodeScore(nodeId);
            int newScoreNode;
            if (swapped) {
                newScoreNode = getNodeLinkedScore(nodeId, idSwappedNode);
                newScoreNode += _noeuds[idSwappedNode].score;
            }
            else {
                newScoreNode = _noeuds[nodeId].score;
            }
            int improve = newScoreNode - scoreNode;
            if (improve < 0) {
                nbCroisement += improve;
                if (nbCroisement < bestCroisement) {
                    bestCroisement = nbCroisement;
                    bestResult.copyFromGraphe(*this);
                    end = std::chrono::system_clock::now();
                }
            }
            else {
                double randDouble = generateDoubleRand(1.0);
                if (randDouble >= exp(-improve / t)) {
                    changeUpdateValue(nodeId);
                    if (swapped) {
                        changeUpdateValue(idSwappedNode);
                        _noeuds[nodeId].swap(oldEmplacement);
                        updateNodeScore(idSwappedNode);
                    }
                    else {
                        _noeuds[nodeId].setEmplacement(oldEmplacement);
                    }
                    updateNodeScore(nodeId);
                }
                else {
                    nbCroisement += improve;
                }
            }
        }
        t *= cool;
    }
    copyFromGraphe(bestResult);
    isNombreCroisementUpdated = true;
    isNodeScoreUpdated = true;
    isIntersectionVectorUpdated = true;
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
            for (int j = 0; j < _emplacementsPossibles.size(); j++) {
                if (_noeuds[i].getEmplacement()->getId() != j) {
                    int score, newScore, swappedId;
                    bool swapped = false;
                    Emplacement* oldEmplacement = _noeuds[i].getEmplacement();
                    if (_emplacementsPossibles[j].estDisponible()) {
                        score = getScoreCroisementNode(i);
                        _noeuds[i].setEmplacement(&_emplacementsPossibles[j]);
                    }
                    else {
                        swappedId = _emplacementsPossibles[j]._noeud->getId();
                        score = getScoreCroisementNode(i);
                        score += getScoreCroisementNode(swappedId, i);
                        _noeuds[i].swap(&_emplacementsPossibles[j]);
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
            if (_emplacementsPossibles[indexEmplacement].estDisponible()) {
                _noeuds[indexNoeud].setEmplacement(&_emplacementsPossibles[indexEmplacement]);
            }
            else {
                _noeuds[indexNoeud].swap(&_emplacementsPossibles[indexEmplacement]);
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

