#include "ogdfFunctions.hpp"
#include "personnel.hpp"

#if defined(OGDF_INSTALLED)

#include <ogdf/basic/GridLayout.h>
#include <ogdf/basic/Graph_d.h>
#include <ogdf/planarity/SubgraphPlanarizer.h>
#include <ogdf/planarity/EmbedderMaxFace.h>
#include <ogdf/planarity/PlanRep.h>
#include <ogdf/planarlayout/PlanarDrawLayout.h>
#include <ogdf/basic/simple_graph_alg.h>

#include <ogdf/planarity/PlanarizationGridLayout.h>
#include <ogdf/planarity/PlanarizationLayout.h>
#include <ogdf/orthogonal/OrthoLayout.h>

#include <ogdf/planarity/FixedEmbeddingInserter.h>
#include <ogdf/planarity/MultiEdgeApproxInserter.h>
#include <ogdf/planarity/VariableEmbeddingInserter.h>
#include <ogdf/planarity/VariableEmbeddingInserterDyn.h>
#include <ogdf/planarity/PlanarizerStarReinsertion.h>
#include <ogdf/planarity/PlanarizerMixedInsertion.h>
#include <ogdf/planarity/PlanarizerChordlessCycle.h>

#include <ogdf/energybased/StressMinimization.h>
#include <ogdf/energybased/FastMultipoleEmbedder.h>
#include <ogdf/energybased/FMMMLayout.h>
#include <ogdf/energybased/GEMLayout.h>
#include <ogdf/energybased/PivotMDS.h>
#include <ogdf/energybased/SpringEmbedderKK.h>

#include <ogdf/fileformats/GraphIO.h>

#include <ogdf/basic/LayoutStatistics.h>

#include "emplacement.hpp"
#include "graphe.hpp"
#include "limits.h"
#include <fstream>

std::vector<ogdf::node> vecNoeudAOGDFNode;

void createGrapheFromOGDFGraphe(Graphe &G, ogdf::Graph &ogdfG) {
	for (int i=0;i<ogdfG.nodes.size();i++) {
		G._noeuds.push_back(Noeud(i));
	}
	std::set<std::pair<int,int>> aretesComposante;
	for (ogdf::node n : ogdfG.nodes) {
		ogdf::adjEntry adj = n->firstAdj();
		if (adj != nullptr) {
			ogdf::adjEntry nextAdj = adj;
			do {
				ogdf::node voisin = nextAdj->twinNode();
				int idNoeud = n->index();
				int idVoisin = voisin->index();
				int idNoeud1, idNoeud2;
				if (idNoeud < idVoisin) { idNoeud1 = idNoeud; idNoeud2 = idVoisin; }
				else { idNoeud1 = idVoisin; idNoeud2 = idNoeud; }
				aretesComposante.insert({idNoeud1,idNoeud2});
				nextAdj = nextAdj->succ();
			} while ((nextAdj != adj)&&(nextAdj != nullptr));
		}
	}
	int numeroArete = 0;
	for (const std::pair<int,int>& arete : aretesComposante) {
		G._aretes.push_back(Aretes(&G._noeuds[arete.first],&G._noeuds[arete.second],numeroArete));
		numeroArete++;
	}
}

void ogdfPrintNumberOfCrossings(ogdf::GraphAttributes& ogdfGA) {
	ogdf::ArrayBuffer<int> tmpA = ogdf::LayoutStatistics::numberOfCrossings(ogdfGA);
	int numberOfCrossings = 0;
	for (const int& number : tmpA) {
		numberOfCrossings += number;
	}
	std::cout << "OGDF Crossings: " << numberOfCrossings/2 << std::endl;
}

int ogdfTotalNumberOfBends(ogdf::GraphAttributes& ogdfGA) {
    int total = 0;
    const ogdf::Graph& ogdfG = ogdfGA.constGraph();
    ogdf::edge e = ogdfG.firstEdge();
    while (e != nullptr) {
        const ogdf::DPolyline& bends = ogdfGA.bends(e);
        total += bends.size(); 
        e = e->succ();
    }
    return total;
}

void ogdfReadFromMM(Graphe& G, std::istream& inStream) {
	ogdf::Graph ogdfG;
	ogdf::GraphIO::readMatrixMarket(ogdfG, inStream);
	createGrapheFromOGDFGraphe(G,ogdfG);
}

// Creer un graphe ogdf a partir d'un graphe placé ou non.
void createOGDFGraphFromGraphe(Graphe &G, ogdf::GridLayout &ogdfGL, ogdf::Graph &ogdfG) {
	vecNoeudAOGDFNode.clear();
	int nodeNumber = G._noeuds.size();
	ogdf::node* nodeTab = new  ogdf::node[nodeNumber];
	for (int i = 0; i < nodeNumber; i++) {
		nodeTab[i] = ogdfG.newNode();
		vecNoeudAOGDFNode.push_back(nodeTab[i]);
		if (G.estPlace()) {
			ogdfGL.x(nodeTab[i]) = G._noeuds[i].getX();
			ogdfGL.y(nodeTab[i]) = G._noeuds[i].getY();
		}
		else {
			ogdfGL.x(nodeTab[i]) = 0;
			ogdfGL.y(nodeTab[i]) = 0;
		}
		G._noeuds[i].ogdfId = nodeTab[i]->index();
	}
	for (int i = 0; i < G._aretes.size(); i++) {
		int id1 = G._aretes[i].getNoeud1()->getId();
		int id2 = G._aretes[i].getNoeud2()->getId();
		ogdfG.newEdge(nodeTab[id1], nodeTab[id2]);
	}
	delete[] nodeTab;
}

// Creer un graphe ogdf a partir d'un graphe placé ou non.
void createOGDFGraphFromGraphe(Graphe &G, ogdf::GraphAttributes &ogdfGA, ogdf::Graph &ogdfG) {
	int nodeNumber = G._noeuds.size();
	ogdf::node* nodeTab = new  ogdf::node[nodeNumber];
	vecNoeudAOGDFNode.clear();
	for (int i = 0; i < nodeNumber; i++) {
		nodeTab[i] = ogdfG.newNode();
		vecNoeudAOGDFNode.push_back(nodeTab[i]);
		if (G.estPlace()) {
			ogdfGA.x(nodeTab[i]) = G._noeuds[i].getX();
			ogdfGA.y(nodeTab[i]) = G._noeuds[i].getY();
		}
		else {
			ogdfGA.x(nodeTab[i]) = 0;
			ogdfGA.y(nodeTab[i]) = 0;
		}
		G._noeuds[i].ogdfId = nodeTab[i]->index();
	}
	for (int i = 0; i < G._aretes.size(); i++) {
		int id1 = G._aretes[i].getNoeud1()->getId();
		int id2 = G._aretes[i].getNoeud2()->getId();
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

void ogdfTranslateOgdfGraphToOrigin(ogdf::Graph& ogdfG, ogdf::GraphAttributes& ogdfGA) {
	double minX, minY;
	int i=0;
	for (auto n : ogdfG.nodes) {
		if (i==0) {
			minX = ogdfGA.x(n);
			minY = ogdfGA.y(n);
		}
		else {
			if (ogdfGA.x(n) < minX) { minX = ogdfGA.x(n); }
			if (ogdfGA.y(n) < minY) { minY = ogdfGA.y(n); }
		}
		i++;
	}
	for (auto n : ogdfG.nodes) {
		ogdfGA.x(n) -= minX;
		ogdfGA.y(n) -= minY;
	}
}

// ogdfGA doit contenir le placement du graphe ogdf, on fait ensuite une translation puis scaling et on place au plus proche
void ogdfPlacementAuPlusProche(ogdf::GraphAttributes& ogdfGA, ogdf::Graph& ogdfG, Graphe& G) {
	ogdfTranslateOgdfGraphToOrigin(ogdfG,ogdfGA);
	int maxX=0, maxY=0;
	for (int i = 0; i < G._emplacements.size(); i++) {
		if (G._emplacements[i].getX() > maxX) { maxX = G._emplacements[i].getX();  }
		if (G._emplacements[i].getY() > maxY) { maxY = G._emplacements[i].getY();  }
	}
	int ogdfMaxX = 0, ogdfMaxY = 0;
	int nodeNumber = G._noeuds.size();
	ogdf::node* nodeTab = new  ogdf::node[nodeNumber];
	int i = 0;
	for (auto n : ogdfG.nodes) {
		if (ogdfGA.x(n) > ogdfMaxX) { ogdfMaxX = ogdfGA.x(n); }
		if (ogdfGA.y(n) > ogdfMaxY) { ogdfMaxY = ogdfGA.y(n); }
		nodeTab[i] = n;
		i++;
	}
	double ratio = (double)ogdfMaxX / (double)maxX;
	double ratio2 = (double)ogdfMaxY / (double)maxY;
	for (auto n : ogdfG.nodes) {
		ogdfGA.x(n) = ogdfGA.x(n) / ratio;
		ogdfGA.y(n) = ogdfGA.y(n) / ratio2;
	}
	G.clearNodeEmplacement();
	for (int i=0;i<G._noeuds.size();i++) {
		double x = ogdfGA.x(nodeTab[i]);
		double y = ogdfGA.y(nodeTab[i]);
		Emplacement* closestEmplacement;
        if (G.grillePtr.size() > 0) {
            closestEmplacement = G.getClosestEmplacementFromPointGrid(x,y,true);
        }
        else {
            closestEmplacement = G.getClosestEmplacementFromPoint(x,y,true);
        }
		G._noeuds[i].setEmplacement(closestEmplacement);
	}
	delete[] nodeTab;
}

// Planarize le graphe avec odgf et essaie de placer les noeuds au plus proche de ses emplacements possible par rapport au grpahe OGDF
int ogdfPlacementAuPlusProche(Graphe& G) {
	ogdf::Graph ogdfG;
	ogdf::GridLayout ogdfGL{ ogdfG };
	createOGDFGraphFromGraphe(G, ogdfGL, ogdfG);
	if (planarizeMaxFace(ogdfGL, ogdfG) != 0) {
		return 1;
	}
	int maxX=0, maxY=0;
	for (int i = 0; i < G._emplacements.size(); i++) {
		if (G._emplacements[i].getX() > maxX) { maxX = G._emplacements[i].getX();  }
		if (G._emplacements[i].getY() > maxY) { maxY = G._emplacements[i].getY();  }
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
		for (int i = 0; i < G._emplacements.size(); i++) {
			G._emplacements[i]._x *= ratio;
			G._emplacements[i]._y *= ratio;
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
				for (int k = 0; k < G._emplacements.size(); k++) {
					if (G._emplacements[k].estDisponible()) {
						double dist2 = ((G._emplacements[k].getX() - ogdfGL.x(nodeTab[j])) * (G._emplacements[k].getX() - ogdfGL.x(nodeTab[j]))) + ((G._emplacements[k].getY() - ogdfGL.y(nodeTab[j])) * (G._emplacements[k].getY() - ogdfGL.y(nodeTab[j])));
						if (dist2 < dist) {
							dist = dist2;
							indexNoeud = j;
							indexEmp = k;
						}
					}
				}
			}
		}
		G._noeuds[indexNoeud].setEmplacement(&G._emplacements[indexEmp]);
	}
	delete[] nodeTab;
	return 0;
}

// Planarize le graphe avec odgf et essaie de placer les noeuds au plus proche de ses emplacements possible par rapport au graphe OGDF
void ogdfPlacementAuPlusProcheStress(Graphe& G) {
	ogdf::Graph ogdfG;
	ogdf::GraphAttributes ogdfGA{ ogdfG };
	createOGDFGraphFromGraphe(G,ogdfGA,ogdfG);
	ogdf::StressMinimization sm;
	sm.call(ogdfGA);
	ogdfTranslateOgdfGraphToOrigin(ogdfG,ogdfGA);
	int maxX=0, maxY=0;
	for (int i = 0; i < G._emplacements.size(); i++) {
		if (G._emplacements[i].getX() > maxX) { maxX = G._emplacements[i].getX();  }
		if (G._emplacements[i].getY() > maxY) { maxY = G._emplacements[i].getY();  }
	}
	int ogdfMaxX = 0, ogdfMaxY = 0;
	int nodeNumber = G._noeuds.size();
	ogdf::node* nodeTab = new  ogdf::node[nodeNumber];
	int i = 0;
	for (auto n : ogdfG.nodes) {
		if (ogdfGA.x(n) > ogdfMaxX) { ogdfMaxX = ogdfGA.x(n); }
		if (ogdfGA.y(n) > ogdfMaxY) { ogdfMaxY = ogdfGA.y(n); }
		nodeTab[i] = n;
		i++;
	}
	double ratio = (double)ogdfMaxX / (double)maxX;
	double ratio2 = (double)ogdfMaxY / (double)maxY;
	if (ratio2 > ratio) { ratio = ratio2; }
	if (ratio > 1) {
		for (int i = 0; i < G._emplacements.size(); i++) {
			G._emplacements[i]._x *= ratio;
			G._emplacements[i]._y *= ratio;
		}
		G.gridWidth *= ratio;
		G.gridHeight *= ratio;
	}
	else {
		for (auto n : ogdfG.nodes) {
			ogdfGA.x(n) /= ratio;
			ogdfGA.y(n) /= ratio;
		}
	}
	G.clearNodeEmplacement();
	for (int i=0;i<G._noeuds.size();i++) {
		int indexNoeud = -1, indexEmp = -1;
		long dist = LONG_MAX;
		for (int j = 0; j < G._noeuds.size(); j++) {
			if (!G._noeuds[j].estPlace()) {
				for (int k = 0; k < G._emplacements.size(); k++) {
					if (G._emplacements[k].estDisponible()) {
						long long dist2 = ((G._emplacements[k].getX() - ogdfGA.x(nodeTab[j])) * (G._emplacements[k].getX() - ogdfGA.x(nodeTab[j]))) + ((G._emplacements[k].getY() - ogdfGA.y(nodeTab[j])) * (G._emplacements[k].getY() - ogdfGA.y(nodeTab[j])));
						if (dist2 < dist) {
							dist = dist2;
							indexNoeud = j;
							indexEmp = k;
						}
					}
				}
			}
		}
		G._noeuds[indexNoeud].setEmplacement(&G._emplacements[indexEmp]);
	}
	delete[] nodeTab;
}

// Remplace les emplacements du graphe par les emplacements trouvés par la planarizerMaxFace
int ogdfReverse(Graphe &G) {
	ogdf::Graph ogdfG;
	ogdf::GridLayout ogdfGL{ ogdfG };
	createOGDFGraphFromGraphe(G, ogdfGL, ogdfG);
	if (planarizeMaxFace(ogdfGL, ogdfG) != 0) {
		return 1;
	}
	G.clearNodeEmplacement();
	G._emplacements.clear();
	G.gridHeight = 10;
	G.gridWidth = 10;
	int i = 0;
	for (auto n : ogdfG.nodes) {
		int x = ogdfGL.x(n);
		int y = ogdfGL.y(n);
		G._emplacements.push_back(Emplacement(x,y,i));
		if (x > G.gridWidth) { G.gridWidth = x; }
		if (y > G.gridHeight) { G.gridHeight = y; }
		i++;
	}
	for (int i=0;i<G._noeuds.size();i++) {
		G._noeuds[i].setEmplacement(&G._emplacements[i]);
	}
	return 0;
}

void ogdfRescaleOgdfG(ogdf::Graph& ogdfG, ogdf::GraphAttributes& ogdfGA, int scale) {
	for (auto n : ogdfG.nodes) {
		ogdfGA.x(n) *= scale;
		ogdfGA.y(n) *= scale;
	}
}

void ogdfReverseAndPlace(Graphe &G, ogdf::GraphAttributes& ogdfGA, ogdf::Graph& ogdfG) {
	G.clearNodeEmplacement();
	G._emplacements.clear();
	G.gridHeight = 10;
	G.gridWidth = 10;
	int i = 0;
	for (auto n : ogdfG.nodes) {
		int x = ogdfGA.x(n);
		int y = ogdfGA.y(n);
		G._emplacements.push_back(Emplacement(x,y,i));
		if (x > G.gridWidth) { G.gridWidth = x; }
		if (y > G.gridHeight) { G.gridHeight = y; }
		i++;
	}
	for (int i=0;i<G._noeuds.size();i++) {
		G._noeuds[i].setEmplacement(&G._emplacements[i]);
	}
}

void ogdfReverseNonPlanar(Graphe &G) {
	G.clearNodeEmplacement();
	G._emplacements.clear();
	ogdf::Graph ogdfG;
	ogdf::GraphAttributes ogdfGA{ ogdfG };
	createOGDFGraphFromGraphe(G, ogdfGA, ogdfG);
	
	ogdf::PlanarizationGridLayout pl;

	ogdf::SubgraphPlanarizer *crossMin = new ogdf::SubgraphPlanarizer;

	pl.setCrossMin(crossMin);

	pl.call(ogdfGA);

	std::cout << "OGDF number of crossings: " << pl.numberOfCrossings() << std::endl;

	int i=0;
	for (ogdf::node& n : ogdfG.nodes) {
		int x = ogdfGA.x(n);
		int y = ogdfGA.y(n);
		G._emplacements.push_back(Emplacement(x,y,i));
		if (x > G.gridWidth) { G.gridWidth = x; }
		if (y > G.gridHeight) { G.gridHeight = y; }
		i++;
	}
	for (int i=0;i<G._noeuds.size();i++) {
		G._noeuds[i].setEmplacement(&G._emplacements[i]);
	}
	ogdf::GraphIO::write(ogdfGA, chemin + "/resultats/output-ERDiagram.svg", ogdf::GraphIO::drawSVG);
}

void ogdfCrossingNumbers(std::vector<std::string> graphFiles) {
	for (std::string &nomFichierGraph : graphFiles) {
		std::string fileGraph = chemin + "exemple/Graphe/" + nomFichierGraph + ".json";
		Graphe G;
		G.readFromJsonGraph(fileGraph);
		ogdf::Graph ogdfG;
		ogdf::GridLayout ogdfGL{ ogdfG };
		createOGDFGraphFromGraphe(G, ogdfGL, ogdfG);
		ogdf::PlanRep pr(ogdfG);
		std::string nomFichier = chemin + "/resultats/" + nomFichierGraph + ".csv";
		std::ofstream resultats(nomFichier, std::ios_base::app);
		int crossingNumber;

		/*
		ogdf::SubgraphPlanarizer sp = ogdf::SubgraphPlanarizer();
		sp.call(pr,0,crossingNumber);
		resultats << crossingNumber << std::endl;
		sp.setInserter(new ogdf::FixedEmbeddingInserter);
		sp.call(pr,0,crossingNumber);
		resultats << crossingNumber << std::endl;
		sp.setInserter(new ogdf::MultiEdgeApproxInserter);
		sp.call(pr,0,crossingNumber);
		resultats << crossingNumber << std::endl;
		sp.setInserter(new ogdf::VariableEmbeddingInserter);
		sp.call(pr,0,crossingNumber);
		resultats << crossingNumber << std::endl;
		sp.setInserter(new ogdf::VariableEmbeddingInserterDyn);
		sp.call(pr,0,crossingNumber);
		resultats << crossingNumber << std::endl;

		ogdf::PlanarizerChordlessCycle pcc = ogdf::PlanarizerChordlessCycle();
		pcc.call(pr,0,crossingNumber);
		resultats << crossingNumber << std::endl;

		ogdf::PlanarizerMixedInsertion pmi = ogdf::PlanarizerMixedInsertion();
		pmi.call(pr,0,crossingNumber);
		resultats << crossingNumber << std::endl;
		pmi.nodeSelectionMethod(ogdf::PlanarizerMixedInsertion::NodeSelectionMethod::Random);
		pmi.call(pr,0,crossingNumber);
		resultats << crossingNumber << std::endl;
		pmi.nodeSelectionMethod(ogdf::PlanarizerMixedInsertion::NodeSelectionMethod::HigherDegree);
		pmi.call(pr,0,crossingNumber);
		resultats << crossingNumber << std::endl;
		pmi.nodeSelectionMethod(ogdf::PlanarizerMixedInsertion::NodeSelectionMethod::LowerDegree);
		pmi.call(pr,0,crossingNumber);
		resultats << crossingNumber << std::endl;
		pmi.nodeSelectionMethod(ogdf::PlanarizerMixedInsertion::NodeSelectionMethod::HigherNonPlanarDegree);
		pmi.call(pr,0,crossingNumber);
		resultats << crossingNumber << std::endl;
		pmi.nodeSelectionMethod(ogdf::PlanarizerMixedInsertion::NodeSelectionMethod::LowerNonPlanarDegree);
		pmi.call(pr,0,crossingNumber);
		resultats << crossingNumber << std::endl;
		pmi.nodeSelectionMethod(ogdf::PlanarizerMixedInsertion::NodeSelectionMethod::BothEndpoints);
		pmi.call(pr,0,crossingNumber);
		resultats << crossingNumber << std::endl;
		*/
		ogdf::PlanarizerStarReinsertion psr = ogdf::PlanarizerStarReinsertion();
		psr.call(pr,0,crossingNumber);
		resultats << crossingNumber << std::endl;

		resultats.close();
	}
}

void ogdfStressMinimization(Graphe& G) {
	ogdf::Graph ogdfG;
	ogdf::GraphAttributes ogdfGA{ ogdfG };
	createOGDFGraphFromGraphe(G,ogdfGA,ogdfG);
	ogdf::StressMinimization sm;
	sm.call(ogdfGA);
	ogdfPrintNumberOfCrossings(ogdfGA);
	ogdfTranslateOgdfGraphToOrigin(ogdfG,ogdfGA);
	ogdfReverseAndPlace(G,ogdfGA,ogdfG);
}

void ogdfFastMultipoleEmbedder(Graphe& G) {
	ogdf::Graph ogdfG;
	ogdf::GraphAttributes ogdfGA{ ogdfG };
	createOGDFGraphFromGraphe(G,ogdfGA,ogdfG);
	ogdf::FastMultipoleEmbedder fme;
	fme.call(ogdfGA);
	ogdfPrintNumberOfCrossings(ogdfGA);
	ogdfTranslateOgdfGraphToOrigin(ogdfG,ogdfGA);
	ogdfReverseAndPlace(G,ogdfGA,ogdfG);
}

void ogdfFastMultipoleMultilevelEmbedder(Graphe& G) {
	G.initGrilleCarre();
    G.registerSlotsInGridNoMove();
	ogdf::Graph ogdfG;
	ogdf::GraphAttributes ogdfGA{ ogdfG };
	createOGDFGraphFromGraphe(G,ogdfGA,ogdfG);
	ogdf::FastMultipoleMultilevelEmbedder fmme;
	fmme.call(ogdfGA);
	ogdfPlacementAuPlusProche(ogdfGA,ogdfG,G);
	G.deleteGrille();
}

void ogdfFMMMLayout(Graphe& G) {
	ogdf::Graph ogdfG;
	ogdf::GraphAttributes ogdfGA{ ogdfG };
	createOGDFGraphFromGraphe(G,ogdfGA,ogdfG);
	ogdf::FMMMLayout fmmml;
	fmmml.unitEdgeLength(22);
	fmmml.call(ogdfGA);
	//ogdfPrintNumberOfCrossings(ogdfGA);
	ogdfPlacementAuPlusProche(ogdfGA,ogdfG,G);
	ogdf::GraphIO::write(ogdfGA, chemin + "/resultats/output-ERDiagram.svg", ogdf::GraphIO::drawSVG);
}

void ogdfGEMLayout(Graphe& G) {
	ogdf::Graph ogdfG;
	ogdf::GraphAttributes ogdfGA{ ogdfG };
	createOGDFGraphFromGraphe(G,ogdfGA,ogdfG);
	ogdf::GEMLayout geml;
	geml.call(ogdfGA);
	ogdfPrintNumberOfCrossings(ogdfGA);
	ogdfTranslateOgdfGraphToOrigin(ogdfG,ogdfGA);
	ogdfReverseAndPlace(G,ogdfGA,ogdfG);
}

void ogdfPivotMDS(Graphe& G) {
	ogdf::Graph ogdfG;
	ogdf::GraphAttributes ogdfGA{ ogdfG };
	createOGDFGraphFromGraphe(G,ogdfGA,ogdfG);
	ogdf::PivotMDS pmds;
	pmds.call(ogdfGA);
	ogdfPrintNumberOfCrossings(ogdfGA);
	ogdfTranslateOgdfGraphToOrigin(ogdfG,ogdfGA);
	ogdfReverseAndPlace(G,ogdfGA,ogdfG);
}

void ogdfOther(Graphe& G) {
	ogdf::Graph ogdfG;
	ogdf::GraphAttributes ogdfGA{ ogdfG };
	createOGDFGraphFromGraphe(G,ogdfGA,ogdfG);
	ogdf::SpringEmbedderKK segv;
	segv.call(ogdfGA);
	ogdfPrintNumberOfCrossings(ogdfGA);
	//ogdfTranslateOgdfGraphToOrigin(ogdfG,ogdfGA);
	//ogdfReverseAndPlace(G,ogdfGA,ogdfG);
}


void ogdfOtherTest(Graphe& G) {
	ogdf::Graph ogdfG;
	ogdf::GraphAttributes ogdfGA{ ogdfG };
	createOGDFGraphFromGraphe(G,ogdfGA,ogdfG);
    ogdf::PlanarizationLayout pl;
    ogdf::OrthoLayout* ol = new ogdf::OrthoLayout();
    ol->bendBound(1);
    //pl.setPlanarLayouter(ol);
    pl.setCrossMin(new ogdf::SubgraphPlanarizer());

    pl.call(ogdfGA);
    ogdfPrintNumberOfCrossings(ogdfGA);
    ogdf::GraphIO::write(ogdfGA, chemin + "/svgOgdf/output.svg", ogdf::GraphIO::drawSVG);
    std::cout << "Nb Bends: " << ogdfTotalNumberOfBends(ogdfGA) << std::endl;
    ogdfGA.clearAllBends();
    ogdfPrintNumberOfCrossings(ogdfGA);
    ogdf::GraphIO::write(ogdfGA, chemin + "/svgOgdf/outputNoBend.svg", ogdf::GraphIO::drawSVG);
}

// Fait différents test sur le graphe: connexité, biconnecté, genus, crossing number(SubGraphPlanarize)
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