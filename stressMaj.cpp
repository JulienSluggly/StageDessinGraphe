#include "stressMaj.hpp"
#include <iostream>

void StressMajorization::initMatrices(Graphe& G, std::vector<std::vector<double>>& shortestPathMatrix, std::vector<std::vector<double>>& weightMatrix) {
    for (int i=0;i<G._noeuds.size();i++) {
        std::vector<double> tmpArrayPath;
        std::vector<double> tmpArrayWeight;
        for (int j=0;j<G._noeuds.size();j++) {
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

void StressMajorization::bfs_SPSS(Graphe& G, int nodeId, std::vector<double>& shortestPathArray, int edgeCosts) {
    std::vector<bool> mark(G._noeuds.size(),false);
    std::vector<int> bfs;
    bfs.push_back(nodeId);
    // mark s and set distance to itself 0
    mark[nodeId] = true;
    shortestPathArray[nodeId] = 0;
    int numero = 0;
    while (numero < bfs.size()) {
        int id = bfs[numero];
        double dist = shortestPathArray[id] + edgeCosts;
        for (Noeud* voisin : G._noeuds[id].voisins) {
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

void StressMajorization::bfs_SPAP(Graphe& G, std::vector<std::vector<double>>& shortestPathMatrix, int edgeCosts) {
    for (int i=0;i<G._noeuds.size();i++) {
        bfs_SPSS(G,i,shortestPathMatrix[i],edgeCosts);
    }
}

void StressMajorization::calcWeights(Graphe& G, std::vector<std::vector<double>>& shortestPathMatrix, std::vector<std::vector<double>>& weightMatrix) {
    for (int i=0;i<G._noeuds.size();i++) {
        for (int j=0;j<G._noeuds.size();j++) {
            if (i!=j) {
                // w_ij = d_ij^-2
                weightMatrix[i][j] = 1 / (shortestPathMatrix[i][j] * shortestPathMatrix[i][j]);
            }
        }
    }
}

void StressMajorization::nextIteration(Graphe& G, std::vector<std::vector<double>>& shortestPathMatrix, std::vector<std::vector<double>>& weightMatrix) {
    for (int nodeId=0;nodeId<G._noeuds.size();nodeId++) {
		double newXCoord = 0.0;
		double newYCoord = 0.0;
		double currXCoord = G._noeuds[nodeId].getX();
		double currYCoord = G._noeuds[nodeId].getY();
		double totalWeight = 0;
		for (int secondNodeId=0;secondNodeId<G._noeuds.size();secondNodeId++) {
			if (nodeId == secondNodeId) {
				continue;
			}
			// calculate euclidean distance between both points
			double xDiff = currXCoord - G._noeuds[secondNodeId].getX();
			double yDiff = currYCoord - G._noeuds[secondNodeId].getY();
			double euclideanDist = sqrt(xDiff * xDiff + yDiff * yDiff);
			// get the weight
			double weight = weightMatrix[nodeId][secondNodeId];
			// get the desired distance
			double desDistance = shortestPathMatrix[nodeId][secondNodeId];
			// reset the voted x coordinate
            double voteX = G._noeuds[secondNodeId].getX();
            if (euclideanDist != 0) {
                // calc the vote
                voteX += desDistance * (currXCoord - voteX) / euclideanDist;
            }
            // add the vote
            newXCoord += weight * voteX;
			// reset the voted y coordinate
            double voteY = G._noeuds[secondNodeId].getY();
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
        Emplacement* closestEmplacement;
        if (m_useGrille) {
            Emplacement* closestEmplacement = G.getClosestEmplacementFromPointGrid(currXCoord,currYCoord);
        }
        else {
            closestEmplacement = G.getClosestEmplacementFromPoint(currXCoord,currYCoord);
        }
        Emplacement* currentEmplacement = G._noeuds[nodeId].getEmplacement();
        if (closestEmplacement->_id != currentEmplacement->_id) {
            if (closestEmplacement->estDisponible()) {
                G._noeuds[nodeId].setEmplacement(closestEmplacement);
            }
            else {
                int idSwappedNode = closestEmplacement->_noeud->_id;
                G._noeuds[nodeId].swap(closestEmplacement);
            }
        }
	}
}

bool StressMajorization::finished(Graphe& G, int numberOfPerformedIterations, std::vector<double>& newX, std::vector<double>& newY, double prevStress, double curStress) {
	if (numberOfPerformedIterations == m_iterations) {
		return true;
	}
	return false;
}

void StressMajorization::minimizeStress(Graphe& G, std::vector<std::vector<double>>& shortestPathMatrix, std::vector<std::vector<double>>& weightMatrix) {
	int numberOfPerformedIterations = 0;

	double prevStress = std::numeric_limits<double>::max();
	double curStress = std::numeric_limits<double>::max();

	std::vector<double> newX;
	std::vector<double> newY;

    int i=0;
	do {
		nextIteration(G, shortestPathMatrix, weightMatrix);
        i++;
	} while (!finished(G, ++numberOfPerformedIterations, newX, newY, prevStress, curStress));
}

void StressMajorization::runAlgo(Graphe& G, std::vector<std::vector<double>>& shortestPathMatrix, std::vector<std::vector<double>>& weightMatrix) {
    //computeInitialLayout(G); // Pas obligatoire
    //replaceInfinityDistances(shortestPathMatrix,m_edgeCosts * sqrt((double)(G._noeuds.size()); // Pas utile si graphe connexe
    calcWeights(G,shortestPathMatrix,weightMatrix);
    minimizeStress(G,shortestPathMatrix,weightMatrix);
}

void StressMajorization::runAlgo(Graphe& G) {
    if (G.DEBUG_GRAPHE) std::cout << "Debut Stress Majorization\n";
    std::vector<std::vector<double>> shortestPathMatrix;
    std::vector<std::vector<double>> weightMatrix;
    initMatrices(G,shortestPathMatrix,weightMatrix);
    bfs_SPAP(G,shortestPathMatrix,m_edgeCosts);
    runAlgo(G,shortestPathMatrix,weightMatrix);
    if (G.DEBUG_GRAPHE) std::cout << "Fin Stress Majorization\n";
}