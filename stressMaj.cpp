#include "stressMaj.hpp"
#include "graphe.hpp"
#include <iostream>
#include <fstream>

void StressMajorization::initMatrices() {
    for (int i=0;i<G->_noeuds.size();i++) {
        std::vector<double> tmpArrayPath;
        std::vector<double> tmpArrayWeight;
        for (int j=0;j<G->_noeuds.size();j++) {
            if (i==j) {
                tmpArrayPath.push_back(0);
            }
            else {
                tmpArrayPath.push_back(std::numeric_limits<double>::infinity());
            }
            tmpArrayWeight.push_back(0);
        }
        shortestPathMatrix.push_back(tmpArrayPath);
        weightMatrix.push_back(tmpArrayWeight);
    }
}

void StressMajorization::bfs_SPSS(int nodeId, std::vector<double>& shortestPathArray, int edgeCosts) {
    std::vector<bool> mark(G->_noeuds.size(),false);
    std::vector<int> bfs;
    bfs.push_back(nodeId);
    // mark s and set distance to itself 0
    mark[nodeId] = true;
    shortestPathArray[nodeId] = 0;
    int numero = 0;
    while (numero < bfs.size()) {
        int id = bfs[numero];
        double dist = shortestPathArray[id] + edgeCosts;
        for (Noeud* voisin : G->_noeuds[id].voisins) {
            int voisinId = voisin->_id;
            if (!mark[voisinId]) {
                mark[voisinId] = true;
                bfs.push_back(voisinId);
                shortestPathArray[voisinId] = dist;
            }
        }
        numero++;
    }
}

void StressMajorization::bfs_SPAP(int edgeCosts) {
    for (int i=0;i<G->_noeuds.size();i++) {
        bfs_SPSS(i,shortestPathMatrix[i],edgeCosts);
    }
}

void StressMajorization::calcWeights() {
    for (int i=0;i<G->_noeuds.size();i++) {
        for (int j=0;j<G->_noeuds.size();j++) {
            if (i!=j) {
                // w_ij = d_ij^-2
                weightMatrix[i][j] = 1 / (shortestPathMatrix[i][j] * shortestPathMatrix[i][j]);
            }
        }
    }
}

bool StressMajorization::nextIteration() {
    double moyenneDist = 0.0;
    double dist = 0.0;
    int nombreDeplacement = 0;
    std::vector<int> distMatrix(11,0);
    bool converged = true;
    for (int nodeId=0;nodeId<G->_noeuds.size();nodeId++) {
		double newXCoord = 0.0;
		double newYCoord = 0.0;
		double currXCoord = G->_noeuds[nodeId].getX();
		double currYCoord = G->_noeuds[nodeId].getY();
		double totalWeight = 0;
		for (int secondNodeId=0;secondNodeId<G->_noeuds.size();secondNodeId++) {
			if (nodeId == secondNodeId) {
				continue;
			}
			// calculate euclidean distance between both points
			double xDiff = currXCoord - G->_noeuds[secondNodeId].getX();
			double yDiff = currYCoord - G->_noeuds[secondNodeId].getY();
			double euclideanDist = sqrt(xDiff * xDiff + yDiff * yDiff);
			// get the weight
			double weight = weightMatrix[nodeId][secondNodeId];
			// get the desired distance
			double desDistance = shortestPathMatrix[nodeId][secondNodeId];
			// reset the voted x coordinate
            double voteX = G->_noeuds[secondNodeId].getX();
            if (euclideanDist != 0) {
                // calc the vote
                voteX += desDistance * (currXCoord - voteX) / euclideanDist;
            }
            // add the vote
            newXCoord += weight * voteX;
			// reset the voted y coordinate
            double voteY = G->_noeuds[secondNodeId].getY();
            if (euclideanDist != 0) {
                // calc the vote
                voteY += desDistance * (currYCoord - voteY) / euclideanDist;
            }
            newYCoord += weight * voteY;
			// sum up the weights
			totalWeight += weight;
		}
		// update the positions
		if (totalWeight != 0) {
			currXCoord = newXCoord / totalWeight;
			currYCoord = newYCoord / totalWeight;
		}
        dist = sqrt((currXCoord - G->_noeuds[nodeId].getX()) * (currXCoord - G->_noeuds[nodeId].getX())  + (currYCoord - G->_noeuds[nodeId].getY()) * (currYCoord - G->_noeuds[nodeId].getY()));
        if (dist < 10) {
            distMatrix[floor(dist)]++;
        }
        else {
            distMatrix[distMatrix.size()-1]++;
        }
        moyenneDist += dist;
        Emplacement* closestEmplacement;
        if (m_useGrille) {
            closestEmplacement = G->getClosestEmplacementFromPointGrid(currXCoord,currYCoord);
        }
        else {
            closestEmplacement = G->getClosestEmplacementFromPoint(currXCoord,currYCoord);
        }
        Emplacement* currentEmplacement = G->_noeuds[nodeId].getEmplacement();
        if (closestEmplacement->_id != currentEmplacement->_id) {
            converged = false;
            nombreDeplacement++;
            if (closestEmplacement->estDisponible()) {
                G->_noeuds[nodeId].setEmplacement(closestEmplacement);
            }
            else {
                int idSwappedNode = closestEmplacement->_noeud->_id;
                G->_noeuds[nodeId].swap(closestEmplacement);
            }
        }
	}
    moyenneDist = moyenneDist / (double)G->_noeuds.size();
    std::string nomFichier = chemin + "/resultats/" + G->nomGraphe + "ST.csv";
	std::ofstream resultats(nomFichier, std::ios_base::app);
    resultats << std::fixed << moyenneDist << "," << nombreDeplacement << ",";
    for (int i=0;i<distMatrix.size();i++) {
        resultats << distMatrix[i] << ",";
    }
    resultats << "\n";
    resultats.close();
    return converged;
}

bool StressMajorization::finished(int numberOfPerformedIterations) {
	if (numberOfPerformedIterations == m_iterations) {
		return true;
	}
	return false;
}

void StressMajorization::minimizeStress() {
	int numberOfPerformedIterations = 0;

    bool converged;
    int i=0;
	do {
        //std::cout << i << std::endl;
		converged = nextIteration();
        i++;
	} while (!converged && !finished(++numberOfPerformedIterations));
}

void StressMajorization::replaceInfinityDistances(double newVal) {
    for (int i=0;i<G->_noeuds.size();i++) {
        for (int j=0;j<G->_noeuds.size();j++) {
            if (i != j) {
                if (isinf(shortestPathMatrix[i][j])) {
                    shortestPathMatrix[i][j] = newVal;
                }
            }
        }
    }
}

void StressMajorization::runAlgo() {
    initMatrices();
    bfs_SPAP(m_edgeCosts);
    //computeInitialLayout(G); // Pas obligatoire, aleatoire suffisant?
    if (!G->isGrapheConnected()) { replaceInfinityDistances(m_edgeCosts * sqrt((double)(G->_noeuds.size()))); }
    calcWeights();
    minimizeStress();
}

void StressMajorization::runStepAlgo() {
    initMatrices();
    bfs_SPAP(m_edgeCosts);
    //computeInitialLayout(G); // Pas obligatoire, aleatoire suffisant?
    if (!G->isGrapheConnected()) { replaceInfinityDistances(m_edgeCosts * sqrt((double)(G->_noeuds.size()))); }
    calcWeights();
    m_iterations = 1;
    minimizeStress();
}