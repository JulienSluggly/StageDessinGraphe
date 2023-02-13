#ifndef OGDFFUNC_HPP
#define OGDFFUNC_HPP

#include <ogdf/basic/GridLayout.h>
#include <ogdf/basic/Graph_d.h>
#include <ogdf/planarity/SubgraphPlanarizer.h>
#include <ogdf/planarity/EmbedderMaxFace.h>
#include <ogdf/planarity/PlanRep.h>
#include <ogdf/planarlayout/PlanarDrawLayout.h>
#include <ogdf/basic/simple_graph_alg.h>

void createOGDFGraphFromGraphe(Graphe &G, ogdf::GridLayout &ogdfGL, ogdf::Graph &ogdfG) {
	int nodeNumber = G._noeuds.size();
	ogdf::node* nodeTab = new  ogdf::node[nodeNumber];
	for (int i = 0; i < nodeNumber; i++) {
		nodeTab[i] = ogdfG.newNode();
		if (G.estPlace()) {
			ogdfGL.x(nodeTab[i]) = G._noeuds[i].getX();
			ogdfGL.y(nodeTab[i]) = G._noeuds[i].getY();
		}
		else {
			ogdfGL.x(nodeTab[i]) = 0;
			ogdfGL.y(nodeTab[i]) = 0;
		}
	}
	for (int i = 0; i < G._liens.size(); i++) {
		int id1 = G._liens[i].getNoeud1()->getId();
		int id2 = G._liens[i].getNoeud2()->getId();
		ogdfG.newEdge(nodeTab[id1], nodeTab[id2]);
	}
	delete[] nodeTab;
}

int crossingModule(const  ogdf::Graph& ogdfG) {
	ogdf::PlanRep pr(ogdfG);
	ogdf::SubgraphPlanarizer sp = ogdf::SubgraphPlanarizer();
	int crossingNumber;
	sp.call(pr,0,crossingNumber);
	return crossingNumber;
}

int planarizeMaxFace(ogdf::GridLayout& ogdfGL, ogdf::Graph& ogdfG) {
	if (crossingModule(ogdfG) != 0) {
		std::cout << "Skipping Planarization, crossing Number > 0" << std::endl;
		return 1;
	}
	ogdf::PlanarDrawLayout PL;
	ogdf::EmbedderModule* emb = new ogdf::EmbedderMaxFace();
	PL.separation(-19);
	PL.setEmbedder(emb);
	PL.callGrid(ogdfG, ogdfGL);
	return 0;
}

int ogdfPlacementAuPlusProche(Graphe& G) {
	ogdf::Graph ogdfG;
	ogdf::GridLayout ogdfGL{ ogdfG };
	createOGDFGraphFromGraphe(G, ogdfGL, ogdfG);
	if (planarizeMaxFace(ogdfGL, ogdfG) != 0) {
		return 1;
	}
	int maxX=0, maxY=0;
	for (int i = 0; i < G._emplacementsPossibles.size(); i++) {
		if (G._emplacementsPossibles[i].getX() > maxX) { maxX = G._emplacementsPossibles[i].getX();  }
		if (G._emplacementsPossibles[i].getY() > maxY) { maxY = G._emplacementsPossibles[i].getY();  }
	}
	int ogdfMaxX = 0, ogdfMaxY = 0;
	int nodeNumber = G._noeuds.size();
	ogdf::node* nodeTab = new  ogdf::node[nodeNumber];
	int i = 0;
	for (auto n : ogdfG.nodes) {
		if (ogdfGL.x(n) > ogdfMaxX) { ogdfMaxX = ogdfGL.x(n); }
		if (ogdfGL.y(n) > ogdfMaxY) { ogdfMaxY = ogdfGL.y(n); }
		nodeTab[i] = n;
		i++;
	}
	double ratio = (double)ogdfMaxX / (double)maxX;
	double ratio2 = (double)ogdfMaxY / (double)maxY;
	if (ratio2 > ratio) { ratio = ratio2; }
	if (ratio > 1) {
		for (int i = 0; i < G._emplacementsPossibles.size(); i++) {
			G._emplacementsPossibles[i]._x *= ratio;
			G._emplacementsPossibles[i]._y *= ratio;
		}
	}
	else {
		for (auto n : ogdfG.nodes) {
			ogdfGL.x(n) /= ratio;
			ogdfGL.y(n) /= ratio;
		}
	}
	for (int i=0;i<G._noeuds.size();i++) {
		int indexNoeud = -1, indexEmp = -1;
		double dist = 9999999;
		for (int j = 0; j < G._noeuds.size(); j++) {
			if (!G._noeuds[j].estPlace()) {
				for (int k = 0; k < G._emplacementsPossibles.size(); k++) {
					if (G._emplacementsPossibles[k].estDisponible()) {
						double dist2 = ((G._emplacementsPossibles[k].getX() - ogdfGL.x(nodeTab[j])) * (G._emplacementsPossibles[k].getX() - ogdfGL.x(nodeTab[j]))) + ((G._emplacementsPossibles[k].getY() - ogdfGL.y(nodeTab[j])) * (G._emplacementsPossibles[k].getY() - ogdfGL.y(nodeTab[j])));
						if (dist2 < dist) {
							dist = dist2;
							indexNoeud = j;
							indexEmp = k;
						}
					}
				}
			}
		}
		G._noeuds[indexNoeud].setEmplacement(&G._emplacementsPossibles[indexEmp]);
	}
	delete[] nodeTab;
	return 0;
}

int ogdfReverse(Graphe &G) {
	ogdf::Graph ogdfG;
	ogdf::GridLayout ogdfGL{ ogdfG };
	createOGDFGraphFromGraphe(G, ogdfGL, ogdfG);
	if (planarizeMaxFace(ogdfGL, ogdfG) != 0) {
		return 1;
	}
	G.clearNodeEmplacement();
	G._emplacementsPossibles.clear();
	G.gridHeight = 10;
	G.gridWidth = 10;
	int i = 0;
	for (auto n : ogdfG.nodes) {
		int x = ogdfGL.x(n);
		int y = ogdfGL.y(n);
		G._emplacementsPossibles.push_back(Emplacement(x,y,i));
		if (x > G.gridWidth) { G.gridWidth = x; }
		if (y > G.gridHeight) { G.gridHeight = y; }
		i++;
	}
	return 0;
}

void ogdfRun(Graphe &G) {
	ogdf::Graph ogdfG;
	ogdf::GridLayout ogdfGL{ ogdfG };
	std::cout << "Connexe: " << ogdf::isConnected(ogdfG) << std::endl;
	std::cout << "Biconnected: " << ogdf::isBiconnected(ogdfG) << std::endl;
	std::cout << "Graph genus: " << ogdfG.genus() << std::endl;

	createOGDFGraphFromGraphe(G, ogdfGL, ogdfG);
	std::cout << "Crossing Number: " << crossingModule(ogdfG) << std::endl;
}

#endif