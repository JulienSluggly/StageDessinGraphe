#ifndef GRAPHE_HPP
#define GRAPHE_HPP

#include <vector>
#include <set>
#include <string>
#include <utility>
#include <chrono>
#include "utilitaire.hpp"
#include "aretes.hpp"
#include "noeud.hpp"
#include "cellule.hpp"
#include "personnel.hpp"

extern bool globalDebugVar; // Utile pour le debug uniquement
extern bool globalDebugVar2; // Utile pour le debug uniquement
extern bool globalDebugVar3; // Utile pour le debug uniquement

// 0.0001
extern double epsilon;

class Graphe {
public:
	std::vector<Aretes> _aretes; // L'id d'une arete correspond à son index dans ce tableau
	std::vector<Noeud> _noeuds; // L'id d'un noeud correspond à son index dans ce tableau

	// Contient l'id des noeuds de degré 0
	std::vector<int> _noeudsSeuls;

	// Contient les aretes qui ont des intersections pour l'affichage OpenGL
	std::set<Aretes*> areteIll;
	std::set<Aretes*> areteIllSelf;
	std::set<Aretes*> areteInter;

	std::vector<std::vector<Cellule>> grille;
	std::vector<Cellule*> grillePtr; // Pointe sur les cellules de l'attribut 'grille'

	// Chaque case contient le pointeur sur le noeud qui n'est pas en commun de l'arete i. commonNodeEdges[i][j]
	// Si les aretes n'ont pas de noeud en commun, contient nullptr
	std::vector<std::vector<int>>* commonNodeEdges = nullptr;

	std::vector<std::vector<std::pair<int,int>>> activationGrid;

	int PENALITE_MAX = 1000; // Penalite lors d'une intersection illégale, cette valeur est remplacée lors de l'appel a UpdatePenalite ou SetupGraphe
	int PENALITE_MAX_SELF = 1001; // Penalite lors d'une intersection illégale entre noeud voisins, cette valeur est remplacée lors de l'appel a UpdatePenalite ou SetupGraphe
	int gridHeight = 10;
	int gridWidth = 10;
	long TRES_GRANDE_VALEUR = 999999999; // 999999999

	long nombreCroisement = -1; // Attention cette variable n'est pas forcément à jour! Voir 'isNombreCroisementUpdated'
	bool isNombreCroisementUpdated = false; // Indique si la valeur dans 'nombreCroisement' est à jour.
	bool useCoordReel = true; // Indique si les noeuds ne sont pas attribués a des emplacements mais ont des coord réelles.

	int maxVoisin = -1; // Nombre maximum de voisin d'un noeud dans le graphe. Pas forcément à jour.
	double avgVoisin = -1; // Nombre moyen de voisin d'un noeud dans le graphe. Pas forcément à jour.

	bool grille_with_move = false; // Indique si on a initialisé la grille en déplacant le graphe.

	long nombreInter = -1; // Variable pas à jour
	long nombreInterIll = -1; // Variable pas à jour
	long nombreInterIllSelf = -1; // Variable pas à jour;

	int minXNode=-1,maxXNode=-1,minYNode=-1,maxYNode=-1; // Coord max des noeuds, pas forcément à jour.

	// Valeur utilisés pour le tirage de coordonnées dans une boite autour de chaque noeud.
	double boiteXSizeDepart=-1.0, boiteYSizeDepart=-1.0, diffXBoiteIter=-1.0, diffYBoiteIter=-1.0;

	bool debugVar = false; // Debug print only, pas utile

	std::vector<int> maxFace; // Pas utilisé

	std::string nomGraphe = "Graphe";

	Graphe();

	~Graphe();

	Graphe(std::string nom);

	void initGraphData();

	void afficherNoeudsReel(std::string nom = "");

	void afficherInfo(std::string nom = "");

	// Affiche s'il y a un noeud n'ayant pas de voisin.
	int afficherNoeudSeul(bool display=true, std::string nom = "");

	// Indique s'il y a plusieurs aretes ayant la meme source et destination
	int afficherAreteDouble(bool display=true, std::string nom = "");

	// Calcule si il y a une incohérence dans les vecteurs d'intersections des cotés.
	int debugInterArrays(bool display=true, std::string nom = "");

	// Calcule si le score du graphe est le meme selon les différentes méthode que celui stocké.
	int debugScoreGraphe(bool display=true, bool useArray = false, std::string nom = "");

	// Indique si pour un coté associé a une cellule si la cellule contient ce coté. Puis pareil pour les cellules.
	int debugDesyncAreteCell(bool display=true, std::string nom = "");

	// Indique pour les noeuds,aretes et emplacements si leur ordre dans le vecteur correspond bien a son ID.
	int debugOrdreIdVector(bool display=true, std::string nom="");

	// Indique si un noeud a une arete vers lui meme
	int debugSelfLoop(bool display=true, std::string nom="");

	// Appelle toutes les fonctions de debug et renvoie un entier en fonction de si un bug a été détecté.
	int debugEverything(bool displayOther=false, bool displaySelf=false);

	// Place les noeuds aleatoirement en coordonnée reel dans la grille
	// Ne tient pas a jour le score des noeuds ou du graphe.
	void placementAleatoireReel();

	// Indique si le graphe contient une intersection illegale.
	bool hasIllegalCrossing();

	long getNbCroisementGridReel();

	// Calcule le score d'intersection du graphe en coordonnée flottantes et le met a jour.
	long getNbCroisementReel();

	// Effectue la selection du noeud en fonction de modeNoeud, 0=Aleatoire,1=TournoiBinaire,2=TournoiMultiple
	int selectionNoeud(int modeNoeud, int t, bool isScoreUpdated=false);

	// Effectue la selection de l'emplacement en fonction de modeEmplacement, 0=Aleatoire,1=TournoiBinaire,2=TournoiMultiple
	std::pair<double,double> selectionEmplacementReel(int modeEmplacement, int nodeId, int t, std::vector<std::vector<double>>& customParam, int iter);

	// Calcule le delay a appliquer lors du recuit si besoin.
	void calculDelaiRefroidissement(int& delay, std::vector<std::vector<double>>& customParam, int iter);

	// Calcul les parametres a utiliser pour la selection des emplacements
	void setupSelectionEmplacement(int modeEmplacement, double t, double cool, double seuil, std::vector<std::vector<double>>& customParam);

	// Renvoie la taille maximale en largeur et en hauteur du graphe.
	std::pair<double,double> sizeOfGraphe();

	// Calcule l'improve apres avoir simulé le déplacement du noeud nodeId vers le slot slotId.
	long calculImproveReel(int nodeId, std::pair<double,double>& randCoord,bool useGrille, bool useScore);
	long calculImproveReelLimite(int nodeId,std::pair<double,double>& randCoord,bool useGrille,bool& makeMove,double limiteImprove);

	// Modifie les parametres du rerecuit en fonction des customParam
	void applyRerecuitCustomParam(double& t,double& cool,double& coolt,double& seuil,bool adaptCool,std::vector<std::vector<double>>& customParam);

	// Modifie les parametres du recuit en fonction des customParam
	void applyRecuitCustomParam(double& coeffImprove,std::vector<std::vector<double>>& customParam);

	// Effectue une sauvegarde des coordonnées flottantes des noeuds en graphe
	void saveBestResultRecuitReel(std::vector<std::pair<double,double>>& bestResultVector);

	// Charge la sauvegarde du graphe en coordonnées flottantes
	void loadBestResultRecuitReel(std::vector<std::pair<double,double>>& bestResultVector, long bestNbCroisement);

	// Met a jour les variables importantes du graphe pour indiquer ce qui est stocké
	void updateGraphDataRecuit(bool useScore, bool useGrille);

	// Calcule le score du noeud en fonction de la méthode choisie.
	long calculScoreNodeMethode(int nodeId,int idSwappedNode, bool swapped, bool useGrille, bool useScore, bool useReel=false);
	long calculScoreNodeMethodeLimite(int nodeId,int idSwappedNode, bool swapped, bool useGrille, bool& makeMove, double limiteScore,bool useReel=false);

	// Met a jour le vecteur des cycles de vagues du recuit.
	// Incrémente la derniere valeur du tableau si meme cycle, sinon pushback 1 du signe opposé à la dernière case.
	void updateRecuitCycleVague(bool upgrade);

	// Met a jour les valeurs de croisement au debut du recuit en appelant les fonctions adéquates.
	void setupNombreCroisement(long& nbCroisement, long& bestCroisement, long& debutCroisement);

	// Lance l'algorithme de recuit simulé sur le graphe pour minimiser le nombre d'intersection en coordonnée flottantes
	// Met à jour la variable nombreCroisement du graphe.
	// delay est le nombre de tour auquel on reste à la même température, -1 pour le rendre dynamique en fonction de la taille du graphe.
	// modeNoeud et modeEMplacement sont le mode de sélection de noeud et d'emplacement, 0=Aléatoire, 1=TournoiBinaire, 2=TournoiMultiple
	// Renvoie le nombre d'intersection supprimé
	long recuitSimuleReel(double &timeBest, std::chrono::time_point<std::chrono::system_clock> start, std::vector<std::vector<double>> customParam = {{}}, double cool = 0.99999, double t = 100.0, double seuil = 0.0001, int delay = 1, int modeNoeud = 0, int modeEmplacement = 2,bool useGrille=true,bool useScore=false, int timeLimit=-1);

	long recuitSimuleReelLimite(double &timeBest, std::chrono::time_point<std::chrono::system_clock> start, std::vector<std::vector<double>> customParam = {{}}, double cool = 0.99999, double t = 100.0, double seuil = 0.0001, int delay = 1, int modeNoeud = 0, int modeEmplacement = 2,bool useGrille=true,bool useScore=false, int timeLimit=-1);

	// Applique le recuit simulé en coordonnée flottantes plusieurs fois
	// Met a jour le nombre de croisement du graphe.
	void rerecuitSimuleReel(double &timeBest, int &nombreRecuit, std::chrono::time_point<std::chrono::system_clock> start, std::vector<std::vector<double>> customParam = {{}}, int iter = -1, double cool = 0.99999, double coolt = 0.99, double t = 100.0, double seuil = 0.0001, int delay = 1, int modeNoeud = 0, int modeEmplacement = 2,bool useGrille=true,bool useScore=false, int timeLimit=-1, bool firstWaveImp=false, bool adaptCool=false);

	double distanceReel(std::pair<double,double>& randCoord,std::pair<double,double>& nodeCoord);

	double distanceReelSqrt(std::pair<double,double>& randCoord,std::pair<double,double>& nodeCoord);

	// Fait un tirage de coordonne flottantes et les enregistre dans coord
	void tirageCoordReel(std::pair<double,double>& coord);

	// Indique si tout les noeuds d'un graphe sont place
	bool estPlace();

	// Vide tout les vecteurs du graphe
	void clearGraphe();

	// Vide tout les vecteurs et effectue une copies de tout les objets dans le graphe
	void copyGrilleFromGraphe(Graphe& graphe);

	// Calcule le score du noeud en parametre en coordonnée flottante. N'utilise pas la grille.
	long getScoreCroisementNodeReel(int nodeIndex);

	// Calcule le score du noeud en parametre en coordonnée flottantes. Utilise la grille
	long getScoreCroisementNodeGridReel(int nodeIndex);

	long getScoreCroisementNodeGridReelLimite(int nodeIndex, bool& makeMove, double limiteScore);

	// Met a jour les vecteurs de la grille et des aretes
	void applyNewAreteCelluleVec(std::vector<std::vector<int>>& vecId, int nodeIndex);

	// Utile pour l'affichage openGL
	void recalculateIllegalIntersectionsReel();

	// Renvoie le score d'intersection en coordonnées flottatnes, n'utilise pas la grille
	long getNbCroisementReelConst() const;

	// Renvoie le nombre d'intersection total sans malus, n'utilise pas la grille, utilise les coordonnées flottantes
	long getNbCroisementDiffReel();

	void initCompleteGraph(int n, bool setup=false);

	// Vide les sets contenant les intersections, utile uniquement pour l'affichage openGL
	void clearSetAreteInter();

	// Indique si les deux noeuds ont une aretes commune
	bool areVoisin(int nodeId1, int nodeId2);

	// Retourne l'id de l'arete qui relie deux points en parametre, -1 si aucune est trouvee
	// Utilise la liste d'arete stockee dans les nodes
	int getAreteFromTwoNodes(int nodeId1, int nodeId2);

	// Utile pour l'algorithme genetique dans la fonction sort du vecteur de population
	bool operator < (const Graphe& G) const {
		long n1, n2;
		if (useCoordReel) {
			if (isNombreCroisementUpdated) { n1 = nombreCroisement; } else { n1 = getNbCroisementReelConst(); }
			if (G.isNombreCroisementUpdated) { n2 = G.nombreCroisement; } else { n2 = G.getNbCroisementReelConst(); }
		}
		return (n1 < n2);
	}

	void readFromGraphmlGraph(std::string input);

	void readFromJsonGraph(std::string input);

	void readFromDimacsGraph(std::string input);

	void readFromDimacsGraphClean(std::string input);

	void readFromJsonGraphReel(std::string input);

	void readFromCSVGraphReel(std::string input);

	void writeToCSVGraphReel(std::string output);

	void readQuickCrossGraphAndCoord(std::string fileArete, std::string fileCoord);

	void readQuickCrossGraph(std::string fileArete);

	void readQuickCrossCoord(std::string input);

	void writeToJsonGraphReel(std::string output);

	void writeToJsonComposanteConnexe(std::string output, std::vector<int> composante);

	void writeToJsonCleanGraphe(std::string output);

	void writeToGraphEdgeList(std::string output);

	void writeCoordsNodeReel(std::string output);

	// Creer la grille d'emplacement de taille gridHeight*gridWidth
	void generateGrid(int gridWidth=-1, int gridHeight=-1);

	void initGrilleReel(int row=-1,int column=-1);

	// Supprime la grille courante et la reinitialise
	void reinitGrilleReel();

	// Vide la grille courante
	void clearGrilleReel();

	// Supprime la grille courante
	void deleteGrille();

	// Enregistre les aretes dans la grille en coord flottantes
	void registerEdgesInGridReel();

	// Enregistre les aretes dans la grille en coord flottantes
	void registerEdgeInGridReel(int areteId);

	// Avec coord flottantes uniquement
	void registerNodesInGrid();

	// Avec coord flottantes uniquement
	void registerNodeInGrid(int nodeId);

	// Avec coord flottantes uniquement
	void registerNodesAndEdgesInGrid();

	// Effectue la lecture de fichier json pour le graphe
	void setupGrapheReel(std::string fileGraphe);

	void translateGrapheToOriginReel(double marge=0);

	// Renvoie le noeud le plus proche des coordonnées passées en parametres, utile pour openGL.
	int getClosestNodeFromPoint(double x, double y);

	// Renvoie le noeud en coordonnées réelles le plus proche des coordonnées passées en parametres, utile pour openGL.
	int getClosestNodeFromPointReel(double x, double y);

	// Graphe connexe ou non
	bool isGrapheConnected();

	// Sauvegarde le degree max et moyen des noeuds dans les attributs du graphes: maxVoisin, avgVoisin.
	void calcMaxAndAverageDegree();

	// Renvoie les identifiants des noeuds présents dans la plus grande composante connexe
	std::vector<int> plusGrandeComposanteConnexe();

	// Numerote les emplacement dans la grille avec un numero entre 0 et index.
	void generateActivationGrid(int gridWidth,int gridHeight,int index);

	// Ajoute les emplacement dont l'index d'activation est egal a l'index en parametre
	void activateSlotsGrid(int index);

	// Ajoute les emplacement dont l'index d'activation est inferieur ou egal a l'index en parametre
	void activateSlotsGridUntil(int index);

	// Initialise la matrice des aretes de noeuds communs
	void fillCommonNodeVectors();

	void printCommonMatrix();

	// Initialise la grille en fonction de si on utilise des réel ou non.
	// Enregistre les objets nécéssaires dans la grille.
	// Applique les parametre custom si besoin
	void setupGridAndRegistration(std::vector<std::vector<double>> customParam);

	// nbNoeud * degre doit être pair.
	// Génere un graphe dont tout les noeuds sont de degrés d.
	void generateKRegular(int nbNoeud, int degre);

	// Algorithme FMME avec ou sans emplacement, minute indique si on garde le meilleur placement sur une minute
	void placementFMME(bool minute=false);

	// Modifie les valeurs de PENALITE_MAX et PENALITE_MAX_SELF par les valeurs pen1 et pen2, par défaut le nombre de noeud dans le graphe
	void updatePenalite(int pen1, int pen2);

	// Remplie le vecteur _noeudsSeuls du graphe
	void updateIsolatedNodes();
	
private:
	// Renvoie un entier indiquant la direction de l'arete noeud1 vers noeud2.
	//0=R,1=TR,2=T,3=TL,4=L,5=BL,6=B,7=BR
	int getDirectionAreteReel(int idArete);
	// Met a jour les cellules passée par l'arete areteId
	void recalcAreteCelluleReel(int areteId);
	// Met a jour les aretes du noeud dans les cellules
	void recalcNodeCelluleReel(int nodeId);
	// Met a jour les cellules dans le noeud uniquement
	void recalcSpecificNodeCell(int nodeId);
	// Ajoute les id dans les cellules et dans l'arete
	void initAreteCellule(int areteId);
	// Ajoute les id dans les cellules et dans les aretes du noeuds
	void initNodeCellule(int nodeId);
	// Calcule le vecteur de cellule sans le mettre a jour
	void calcAreteCelluleVec(std::vector<int>& vecInt,int areteId);
	void addCommonNodeVector(int nodeId); // Met a jour la matrice de voisinage pour ce noeud
	void removeCommonNodeVector(int nodeId); // Supprime les references de voisinage de ce noeud
	bool checkNodeAlignementHorizontal(int nodeId); // Renvoie vrai si le noeud est placé sur la longeur d'une grille a epsilon pret
	bool checkNodeAlignementVertical(int nodeId); // Renvoie vrai si le noeud est placé sur la hauteur d'une grille a epsilon pret
};

bool compareGraphePtr(Graphe* g1, Graphe* g2);

#endif