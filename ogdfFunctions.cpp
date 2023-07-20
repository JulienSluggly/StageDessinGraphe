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
#include <ogdf/planarlayout/PlanarStraightLayout.h>
#include <ogdf/planarity/PlanarizationLayout.h>
#include <ogdf/orthogonal/OrthoLayout.h>

#include <ogdf/planarity/FixedEmbeddingInserter.h>
#include <ogdf/planarity/MultiEdgeApproxInserter.h>
#include <ogdf/planarity/VariableEmbeddingInserter.h>
#include <ogdf/planarity/VariableEmbeddingInserterBase.h>
#include <ogdf/planarity/VariableEmbeddingInserterDyn.h>
#include <ogdf/planarity/PlanarizerStarReinsertion.h>
#include <ogdf/planarity/PlanarizerMixedInsertion.h>
#include <ogdf/planarity/PlanarizerChordlessCycle.h>

#include <ogdf/energybased/MultilevelLayout.h>
#include <ogdf/energybased/multilevel_mixer/MatchingMerger.h>
#include <ogdf/energybased/multilevel_mixer/SolarMerger.h>
#include <ogdf/energybased/multilevel_mixer/SolarPlacer.h>

#include <ogdf/energybased/StressMinimization.h>
#include <ogdf/energybased/FastMultipoleEmbedder.h>
#include <ogdf/energybased/FMMMLayout.h>
#include <ogdf/energybased/GEMLayout.h>
#include <ogdf/energybased/PivotMDS.h>
#include <ogdf/energybased/SpringEmbedderKK.h>
#include <ogdf/energybased/SpringEmbedderGridVariant.h>
#include <ogdf/energybased/SpringEmbedderFRExact.h>

#include <ogdf/fileformats/GraphIO.h>

#include <ogdf/basic/LayoutStatistics.h>

#include "graphe.hpp"
#include "limits.h"
#include <fstream>
#include <filesystem>
#include <utility>

std::vector<ogdf::node> vecNoeudAOGDFNode;

void ogdfGetCoordsReel(Graphe& G, ogdf::Graph& ogdfG, ogdf::GraphAttributes& ogdfGA) {
	int i=0;
	for (auto n : ogdfG.nodes) {
		G._noeuds[i]._xreel = ogdfGA.x(n);
		G._noeuds[i]._yreel = ogdfGA.y(n);
		i++;
	}
}

void createGrapheFromOGDFGraphe(Graphe &G, ogdf::Graph &ogdfG) {
	for (ogdf::node n : ogdfG.nodes) {
		G._noeuds.push_back(Noeud(n->index()));
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

int ogdfNumberOfCrossings(ogdf::GraphAttributes& ogdfGA) {
	ogdf::ArrayBuffer<int> tmpA = ogdf::LayoutStatistics::numberOfCrossings(ogdfGA);
	int numberOfCrossings = 0;
	for (const int& number : tmpA) {
		numberOfCrossings += number;
	}
	return numberOfCrossings/2;
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
	bool isGraphePlace = G.estPlace();
	for (int i = 0; i < nodeNumber; i++) {
		nodeTab[i] = ogdfG.newNode();
		vecNoeudAOGDFNode.push_back(nodeTab[i]);
		if (G.useCoordReel) {
			if (isGraphePlace) {
				ogdfGL.x(nodeTab[i]) = G._noeuds[i]._xreel;
				ogdfGL.y(nodeTab[i]) = G._noeuds[i]._yreel;
			}
			else {
				ogdfGL.x(nodeTab[i]) = 0;
				ogdfGL.y(nodeTab[i]) = 0;
			}
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
		if (G.useCoordReel) {
			ogdfGA.x(nodeTab[i]) = G._noeuds[i]._xreel;
			ogdfGA.y(nodeTab[i]) = G._noeuds[i]._yreel;
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

void createOGDFGraphFromGraphe(Graphe &G, ogdf::Graph &ogdfG) {
	int nodeNumber = G._noeuds.size();
	ogdf::node* nodeTab = new  ogdf::node[nodeNumber];
	vecNoeudAOGDFNode.clear();
	for (int i = 0; i < nodeNumber; i++) {
		nodeTab[i] = ogdfG.newNode();
		vecNoeudAOGDFNode.push_back(nodeTab[i]);
		G._noeuds[i].ogdfId = nodeTab[i]->index();
	}
	for (int i = 0; i < G._aretes.size(); i++) {
		int id1 = G._aretes[i].getNoeud1()->getId();
		int id2 = G._aretes[i].getNoeud2()->getId();
		ogdfG.newEdge(nodeTab[id1], nodeTab[id2]);
	}
	delete[] nodeTab;
}

void ogdfReadFromGraph6(Graphe& G, std::string input) {
	ogdf::Graph ogdfG;
	std::istringstream strInput(input);
	ogdf::GraphIO::readGraph6(ogdfG,strInput);
	createGrapheFromOGDFGraphe(G,ogdfG);
}

void ogdfReadFromSparse6(Graphe& G, std::string input) {
	ogdf::Graph ogdfG;

	std::ifstream fichier(input);
	std::string ligne;
	std::getline(fichier, ligne);
	std::istringstream ligneStream(ligne);

	if (!ogdf::GraphIO::readSparse6(ogdfG,ligneStream)) { tcout() << "Erreur lecture\n"; }
	tcout() << ogdfG.nodes.size() << std::endl;
	createGrapheFromOGDFGraphe(G,ogdfG);
}

void ogdfReadFromFile(Graphe& G, std::string input) {
	ogdf::Graph ogdfG;



	std::istringstream strInput(input);
	if (!ogdf::GraphIO::read(ogdfG,strInput)) { tcout() << "Erreur lecture fichier\n"; }
	tcout() << ogdfG.nodes.size() << std::endl;
	createGrapheFromOGDFGraphe(G,ogdfG);
}

void ogdfWriteToGraph6(Graphe& G, std::string output) {
	ogdf::Graph ogdfG;
	createOGDFGraphFromGraphe(G,ogdfG);
	std::ofstream file(output);
	ogdf::GraphIO::writeGraph6(ogdfG,file);
	file.close();

    std::ifstream inputFile(output);
    std::string fileContents((std::istreambuf_iterator<char>(inputFile)),std::istreambuf_iterator<char>());
    inputFile.close();
	fileContents = fileContents.substr(10);

    std::ofstream outputFile(output);
    outputFile << fileContents;
    outputFile.close();
}

int getMaxNodeIdFromFile(std::string input) {
	std::ifstream infile(input);
	std::string line;
	int numeroLigne = 0;
	int nodeId;
	int maxNodeId = -1;
	while (std::getline(infile, line)) {
    	std::istringstream iss(line);
		if (numeroLigne != 0) {
			while (iss) {
				std::string subs;
				iss >> subs;
				if ((!subs.empty())&&(subs.find_first_not_of(' ') != std::string::npos)) {
					try {
						nodeId = stoi(subs)-1;
						if (nodeId > maxNodeId) { maxNodeId = nodeId; }
					}
					catch(...) { tcout() << "Error: " << subs << std::endl; }
				}
			}
		}
		numeroLigne++;
	}
	infile.close();
	return maxNodeId;
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
		tcout() << "Skipping Planarization, crossing Number > 0" << std::endl;
		return 1;
	}
	ogdf::PlanarDrawLayout PL;
	ogdf::EmbedderModule* emb = new ogdf::EmbedderMaxFace();
	PL.separation(-19);
	PL.setEmbedder(emb);
	PL.callGrid(ogdfG, ogdfGL);
	return 0;
}

int planarizeMaxFace(ogdf::GraphAttributes& ogdfGA, ogdf::Graph& ogdfG) {
	if (crossingModule(ogdfG) != 0) {
		tcout() << "Skipping Planarization, crossing Number > 0" << std::endl;
		return 1;
	}
	ogdf::PlanarDrawLayout PL;
	ogdf::EmbedderModule* emb = new ogdf::EmbedderMaxFace();
	PL.separation(-19);
	PL.setEmbedder(emb);
	PL.call(ogdfGA);
	return 0;
}

void ogdfReadQuickCrossToGraph(std::string input, Graphe& G) {
	ogdf::Graph ogdfG;
	ogdf::GraphAttributes ogdfGA{ ogdfG };
	std::ifstream infile(input);
	std::string line;
	int numeroLigne = 0;
	int firstNodeId, secondNodeId;
	int maxNodeId = getMaxNodeIdFromFile(input);
	int idArete = 0;
	G._noeuds.reserve((maxNodeId+1)*2);
	for (int i=0;i<=maxNodeId;i++) {
		G._noeuds.push_back(Noeud(i));
	}
	while (std::getline(infile, line)) {
    	std::istringstream iss(line);
		if (numeroLigne != 0) {
			while (iss) {
				std::string subs;
				iss >> subs;
				if ((!subs.empty())&&(subs.find_first_not_of(' ') != std::string::npos)) {
					try {
						firstNodeId = stoi(subs)-1;
						while (iss) {
							iss >> subs;
							if ((!subs.empty())&&(subs.find_first_not_of(' ') != std::string::npos)) {
								try {
									secondNodeId = stoi(subs)-1;
									G._aretes.push_back(Aretes(&G._noeuds[firstNodeId],&G._noeuds[secondNodeId],idArete));
									idArete++;
								}
								catch(...) { tcout() << "Error: " << subs << std::endl; }
							}
							iss >> subs;
						}
					}
					catch(...) { tcout() << "Error: " << subs << std::endl; }
				}
			}
		}
		numeroLigne++;
	}
	infile.close();
	tcout() << G._noeuds.size() << "n " << G._aretes.size() << "e\n";
	createOGDFGraphFromGraphe(G,ogdfG);
	tcout() << "Start Planarizing...\n";
	planarizeMaxFace(ogdfGA, ogdfG);
	tcout() << "Planarizing done.\n";
	int idNode = 0;
	for (auto n : ogdfG.nodes) {
		G._noeuds[idNode]._xreel = ogdfGA.x(n);
		G._noeuds[idNode]._yreel = ogdfGA.y(n);
		idNode++;
	}
}

void ogdfReadQuickCrossToGraphCrossings(std::string quickcrossInput, std::string graphInput, Graphe& G) {
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
	int maxX = G.gridWidth;
	int maxY = G.gridHeight;
	double ogdfMaxX = -100000, ogdfMaxY = -100000;
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
	if (G.useCoordReel) {
		ogdfGetCoordsReel(G,ogdfG,ogdfGA);
	}
	delete[] nodeTab;
}

// Planarize le graphe avec odgf et essaie de placer les noeuds au plus proche de ses emplacements possible par rapport au grpahe OGDF
int ogdfPlacementAuPlusProche(Graphe& G) {
	return 0;
}

// Planarize le graphe avec odgf et essaie de placer les noeuds au plus proche de ses emplacements possible par rapport au graphe OGDF
void ogdfPlacementAuPlusProcheStress(Graphe& G) {
}

// Remplace les emplacements du graphe par les emplacements trouvés par la planarizerMaxFace
int ogdfReverse(Graphe &G) {
	return 0;
}

void ogdfRescaleOgdfG(ogdf::Graph& ogdfG, ogdf::GraphAttributes& ogdfGA, int scale) {
	for (auto n : ogdfG.nodes) {
		ogdfGA.x(n) *= scale;
		ogdfGA.y(n) *= scale;
	}
}

void ogdfReverseAndPlace(Graphe &G, ogdf::GraphAttributes& ogdfGA, ogdf::Graph& ogdfG) {
	int i=0;
	for (auto n : ogdfG.nodes) {
		G._noeuds[i]._xreel = ogdfGA.x(n);
		G._noeuds[i]._yreel = ogdfGA.y(n);
		i++;
	}
}

void ogdfReverseNonPlanar(Graphe &G) {
}

void ogdfCrossingNumbers(std::vector<std::string> graphFiles) {
}

void ogdfStressMinimization(Graphe& G) {
	ogdf::Graph ogdfG;
	ogdf::GraphAttributes ogdfGA{ ogdfG };
	createOGDFGraphFromGraphe(G,ogdfGA,ogdfG);
	ogdf::StressMinimization sm;
	sm.call(ogdfGA);
	ogdfNumberOfCrossings(ogdfGA);
	ogdfTranslateOgdfGraphToOrigin(ogdfG,ogdfGA);
	ogdfReverseAndPlace(G,ogdfGA,ogdfG);
}

void ogdfFastMultipoleEmbedder(Graphe& G) {
	ogdf::Graph ogdfG;
	ogdf::GraphAttributes ogdfGA{ ogdfG };
	createOGDFGraphFromGraphe(G,ogdfGA,ogdfG);
	ogdf::FastMultipoleEmbedder fme;
	fme.call(ogdfGA);
	ogdfNumberOfCrossings(ogdfGA);
	ogdfTranslateOgdfGraphToOrigin(ogdfG,ogdfGA);
	ogdfReverseAndPlace(G,ogdfGA,ogdfG);
}

void ogdfFastMultipoleMultilevelEmbedder(Graphe& G) {
}

void ogdfFastMultipoleMultilevelEmbedderMinute(Graphe& G) {

}

void ogdfFastMultipoleMultilevelEmbedderReel(Graphe& G) {
	ogdf::Graph ogdfG;
	ogdf::GraphAttributes ogdfGA{ ogdfG };
	createOGDFGraphFromGraphe(G,ogdfGA,ogdfG);
	ogdf::FastMultipoleMultilevelEmbedder fmme;
	fmme.call(ogdfGA);
	int i=0;
	for (auto n : ogdfG.nodes) {
		G._noeuds[i]._xreel = ogdfGA.x(n);
		G._noeuds[i]._yreel = ogdfGA.y(n);
		i++;
	}
	G.isNombreCroisementUpdated = false;
}

void ogdfFastMultipoleMultilevelEmbedderReelMinute(Graphe& G) {
#if defined(DEBUG_GRAPHE)
	tcout() << "Debut placement OGDFFMMMM\n";
#endif
	auto start = std::chrono::system_clock::now();
	ogdf::Graph ogdfG;
	ogdf::GraphAttributes ogdfGA{ ogdfG };
	createOGDFGraphFromGraphe(G,ogdfGA,ogdfG);
	ogdf::FastMultipoleMultilevelEmbedder fmme;
	fmme.maxNumThreads(0);
	fmme.call(ogdfGA);
	std::vector<std::pair<double,double>> bestCoord(G._noeuds.size());
	int i=0;
	for (auto n : ogdfG.nodes) {
		bestCoord[i] = std::make_pair(ogdfGA.x(n),ogdfGA.y(n));
		i++;
	}
	int nbCrossing = ogdfNumberOfCrossings(ogdfGA);
	int bestNbCrossings = nbCrossing;
	int nbIter = 0;
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> secondsTotal = end - start;
	while (nbIter < 100 && secondsTotal.count() < 20) {
		fmme.call(ogdfGA);
		nbCrossing = ogdfNumberOfCrossings(ogdfGA);
		if (nbCrossing < bestNbCrossings) {
			bestNbCrossings = nbCrossing;
			i=0;
			for (auto n : ogdfG.nodes) {
				bestCoord[i] = std::make_pair(ogdfGA.x(n),ogdfGA.y(n));
				i++;
			}
		}
		nbIter++;
		end = std::chrono::system_clock::now();
		secondsTotal = end - start;
	}

	i=0;
	for (const std::pair<double,double>& coord : bestCoord) {
		G._noeuds[i]._xreel = coord.first;
		G._noeuds[i]._yreel = coord.second;
		i++;
	}
	G.isNombreCroisementUpdated = false;
#if defined(DEBUG_GRAPHE)
	tcout() << "Fin Placement OGDFFMMMM\n";
#endif
}

void ogdfMultilevelLayout(Graphe& G) {
	ogdf::Graph ogdfG;
	ogdf::GraphAttributes ogdfGA{ ogdfG };
	createOGDFGraphFromGraphe(G,ogdfGA,ogdfG);
	ogdf::MultilevelLayout mll;

	ogdf::FMMMLayout* fmmml = new ogdf::FMMMLayout();
	fmmml->forceModel(ogdf::FMMMOptions::ForceModel::Eades);
	
	mll.setLayout(new ogdf::StressMinimization());
	//mll.setMultilevelBuilder(new ogdf::MatchingMerger());
	mll.setMultilevelBuilder(new ogdf::SolarMerger());
	mll.setPlacer(new ogdf::SolarPlacer());
	mll.call(ogdfGA);
	//tcout() << ogdfNumberOfCrossings(ogdfGA) << " OGDF crossings.\n";
	ogdfPlacementAuPlusProche(ogdfGA,ogdfG,G);
	ogdf::GraphIO::write(ogdfGA, chemin + "/resultats/output-ERDiagram.svg", ogdf::GraphIO::drawSVG);
}

void ogdfFMMMLayout(Graphe& G) {
	ogdf::Graph ogdfG;
	ogdf::GraphAttributes ogdfGA{ ogdfG };
	createOGDFGraphFromGraphe(G,ogdfGA,ogdfG);
	ogdf::FMMMLayout fmmml;
	fmmml.qualityVersusSpeed(ogdf::FMMMOptions::QualityVsSpeed::GorgeousAndEfficient);
	fmmml.allowedPositions(ogdf::FMMMOptions::AllowedPositions::All);
	//fmmml.unitEdgeLength(22);
	fmmml.randSeed(9);
	fmmml.call(ogdfGA);
	//tcout() << ogdfNumberOfCrossings(ogdfGA) << " OGDF crossings.\n";
	ogdfPlacementAuPlusProche(ogdfGA,ogdfG,G);
	ogdf::GraphIO::write(ogdfGA, chemin + "/resultats/output-ERDiagram.svg", ogdf::GraphIO::drawSVG);
}

void ogdfGEMLayout(Graphe& G) {
	ogdf::Graph ogdfG;
	ogdf::GraphAttributes ogdfGA{ ogdfG };
	createOGDFGraphFromGraphe(G,ogdfGA,ogdfG);
	ogdf::GEMLayout geml;
	geml.call(ogdfGA);
	ogdfNumberOfCrossings(ogdfGA);
	ogdfTranslateOgdfGraphToOrigin(ogdfG,ogdfGA);
	ogdfReverseAndPlace(G,ogdfGA,ogdfG);
}

void ogdfPivotMDS(Graphe& G) {
	ogdf::Graph ogdfG;
	ogdf::GraphAttributes ogdfGA{ ogdfG };
	createOGDFGraphFromGraphe(G,ogdfGA,ogdfG);
	ogdf::PivotMDS pmds;
	pmds.call(ogdfGA);
	tcout() << ogdfNumberOfCrossings(ogdfGA) << std::endl;
	ogdfTranslateOgdfGraphToOrigin(ogdfG,ogdfGA);
	ogdfReverseAndPlace(G,ogdfGA,ogdfG);
}

void ogdfOther(Graphe& G) {
	ogdf::Graph ogdfG;
	ogdf::GraphAttributes ogdfGA{ ogdfG };
	createOGDFGraphFromGraphe(G,ogdfGA,ogdfG);
	ogdf::SpringEmbedderFRExact segv;
	segv.call(ogdfGA);
	ogdfNumberOfCrossings(ogdfGA);
	//ogdfTranslateOgdfGraphToOrigin(ogdfG,ogdfGA);
	//ogdfReverseAndPlace(G,ogdfGA,ogdfG);
}

void ogdfGutwenger(Graphe& G) {
	ogdf::Graph ogdfG;
	ogdf::GraphAttributes ogdfGA{ ogdfG };
	createOGDFGraphFromGraphe(G,ogdfGA,ogdfG);
	ogdf::PlanRep pr(ogdfGA);
	ogdf::SubgraphPlanarizer sp = ogdf::SubgraphPlanarizer();
	sp.permutations(20);
	ogdf::VariableEmbeddingInserter* veib = new ogdf::VariableEmbeddingInserter();
	veib->removeReinsert(ogdf::RemoveReinsertType::MostCrossed);
	sp.setInserter(veib);
	int crossingNumber;
	sp.call(pr,0,crossingNumber);
	printf("%d\n",crossingNumber);
	ogdfNumberOfCrossings(ogdfGA);
	ogdfTranslateOgdfGraphToOrigin(ogdfG,ogdfGA);
	ogdfReverseAndPlace(G,ogdfGA,ogdfG);
	ogdf::GraphIO::write(ogdfGA, chemin + "/resultats/output-ERDiagram.svg", ogdf::GraphIO::drawSVG);
}

void ogdfStarReinsertion(Graphe& G) {
	ogdf::Graph ogdfG;
	ogdf::GraphAttributes ogdfGA(ogdfG, ogdf::GraphAttributes::nodeGraphics | ogdf::GraphAttributes::edgeGraphics);
	ogdf::Layout lay(ogdfG);
	createOGDFGraphFromGraphe(G,ogdfGA,ogdfG);
	ogdf::PlanRep pr(ogdfGA);
	int crossingNumber;
	ogdf::PlanarizerStarReinsertion* psr = new ogdf::PlanarizerStarReinsertion();
	ogdf::PlanarizationGridLayout pgl;
	pgl.setCrossMin(psr);
	pgl.call(ogdfGA);
	//psr->call(pr,0,crossingNumber);
	tcout() << pgl.numberOfCrossings() << " crossings OGDF." << std::endl;
	ogdfGetCoordsReel(G,ogdfG,ogdfGA);
	ogdf::GraphIO::write(ogdfGA, chemin + "/resultats/output-ERDiagram.svg", ogdf::GraphIO::drawSVG);
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
    ogdfNumberOfCrossings(ogdfGA);
    ogdf::GraphIO::write(ogdfGA, chemin + "/svgOgdf/output.svg", ogdf::GraphIO::drawSVG);
    tcout() << "Nb Bends: " << ogdfTotalNumberOfBends(ogdfGA) << std::endl;
    ogdfGA.clearAllBends();
    ogdfNumberOfCrossings(ogdfGA);
    ogdf::GraphIO::write(ogdfGA, chemin + "/svgOgdf/outputNoBend.svg", ogdf::GraphIO::drawSVG);
}

// Fait différents test sur le graphe: connexité, biconnecté, genus, crossing number(SubGraphPlanarize)
void ogdfRun(Graphe &G) {
	ogdf::Graph ogdfG;
	ogdf::GridLayout ogdfGL{ ogdfG };
	createOGDFGraphFromGraphe(G, ogdfGL, ogdfG);
	tcout() << "Connexe: " << ogdf::isConnected(ogdfG) << std::endl;
	tcout() << "Biconnected: " << ogdf::isBiconnected(ogdfG) << std::endl;
	tcout() << "Graph genus: " << ogdfG.genus() << std::endl;
	tcout() << "Graph planarity: " << ogdf::isPlanar(ogdfG) << std::endl;

	tcout() << "Crossing Number: " << crossingModule(ogdfG) << std::endl;
}

void getGrapheMaxFace(Graphe& G) {
	ogdf::Graph ogdfG;
	createOGDFGraphFromGraphe(G,ogdfG);
	ogdf::ConstCombinatorialEmbedding cce(ogdfG);
	ogdf::face fa = cce.maximalFace();
	std::unordered_set<int> nodeId;
	for (auto elem : fa->entries) {
		nodeId.insert(elem->theNode()->index());
		nodeId.insert(elem->twinNode()->index());
	}
	for (const int& nodeIndex : nodeId) {
		std::cout << nodeIndex << std::endl;
	}
}

void sortByGenus() {
}

void testGenusFolder() {
}

#else

void ogdfReadFromMM(Graphe& G, std::istream& inStream) { printf("OGDF NOT INSTALLED.\n"); }
int ogdfPlacementAuPlusProche(Graphe& G) { printf("OGDF NOT INSTALLED.\n"); }
void ogdfPlacementAuPlusProcheStress(Graphe& G) { printf("OGDF NOT INSTALLED.\n"); }
int ogdfReverse(Graphe &G) { printf("OGDF NOT INSTALLED.\n"); }
void ogdfReverseNonPlanar(Graphe &G) { printf("OGDF NOT INSTALLED.\n"); }
void ogdfCrossingNumbers(std::vector<std::string> graphFiles) { printf("OGDF NOT INSTALLED.\n"); }
void ogdfStressMinimization(Graphe& G) { printf("OGDF NOT INSTALLED.\n"); }
void ogdfFastMultipoleEmbedder(Graphe& G) { printf("OGDF NOT INSTALLED.\n"); }
void ogdfFastMultipoleMultilevelEmbedder(Graphe& G) { printf("OGDF NOT INSTALLED.\n"); }
void ogdfFastMultipoleMultilevelEmbedderReel(Graphe& G) { printf("OGDF NOT INSTALLED.\n"); }
void ogdfFMMMLayout(Graphe& G) { printf("OGDF NOT INSTALLED.\n"); }
void ogdfGEMLayout(Graphe& G) { printf("OGDF NOT INSTALLED.\n"); }
void ogdfPivotMDS(Graphe& G) { printf("OGDF NOT INSTALLED.\n"); }
void ogdfOther(Graphe& G) { printf("OGDF NOT INSTALLED.\n"); }
void ogdfOtherTest(Graphe& G) { printf("OGDF NOT INSTALLED.\n"); }
void ogdfGutwenger(Graphe& G) { printf("OGDF NOT INSTALLED.\n"); }
void ogdfRun(Graphe &G) { printf("OGDF NOT INSTALLED.\n"); }
int getMaxNodeIdFromFile(std::string input) { printf("OGDF NOT INSTALLED.\n"); }

#endif