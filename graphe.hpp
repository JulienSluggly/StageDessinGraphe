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
#include "carte.h"
#include "cellule.hpp"
#include "personnel.hpp"
#include "stressMaj.hpp"
#include "pivotMDS.hpp"

// 0.0001
extern double epsilon;

class Graphe {
public:
	std::vector<Emplacement> _emplacements; // L'id d'un emplacement correspond à son index dans ce tableau
	std::vector<Aretes> _aretes; // L'id d'une arete correspond à son index dans ce tableau
	std::vector<Noeud> _noeuds; // L'id d'un noeud correspond à son index dans ce tableau

	// Contient l'id des noeuds de degré 0
	std::vector<int> _noeudsSeuls;

	Carte _c;
	StressMajorization _sm;
	PivotMDS _pmds;

	std::vector<std::vector<Cellule>> grille;
	std::vector<Cellule*> grillePtr; // Pointe sur les cellules de l'attribut 'grille'

	// Chaque case contient le pointeur sur le noeud qui n'est pas en commun de l'arete i. commonNodeEdges[i][j]
	// Si les aretes n'ont pas de noeud en commun, contient nullptr
	std::vector<std::vector<int>>* commonNodeEdges = nullptr;

	int PENALITE_MAX = 1000; // Penalite lors d'une intersection illégale, cette valeur est remplacée lors de l'appel a UpdatePenalite ou SetupGraphe
	int PENALITE_MAX_SELF = 1001; // Penalite lors d'une intersection illégale entre noeud voisins, cette valeur est remplacée lors de l'appel a UpdatePenalite ou SetupGraphe
	int gridHeight = 10;
	int gridWidth = 10;
	long TRES_GRANDE_VALEUR = 999999999; // 999999999

	long nombreCroisement = -1; // Attention cette variable n'est pas forcément à jour! Voir 'isNombreCroisementUpdated'
	bool isNombreCroisementUpdated = false; // Indique si la valeur dans 'nombreCroisement' est à jour.
	bool isCarteSetUp = false; // Indique si la triangulation de delaunay des emplacements sur le graphe a été faite.
	bool useCoordReel = false; // Indique si les noeuds ne sont pas attribués a des emplacements mais ont des coord réelles.

	int maxVoisin = -1; // Nombre maximum de voisin d'un noeud dans le graphe. Pas forcément à jour.
	double avgVoisin = -1; // Nombre moyen de voisin d'un noeud dans le graphe. Pas forcément à jour.

	bool grille_with_move = false; // Indique si on a initialisé la grille en déplacant le graphe.

	long nombreInter = -1; // Variable pas à jour
	long nombreInterIll = -1; // Variable pas à jour
	long nombreInterIllSelf = -1; // Variable pas à jour;

	int originalWidth = -1;
	int originalHeight = -1;

	bool areCoordMultiplied = false;

	int minXNode = -1, maxXNode = -1, minYNode = -1, maxYNode = -1; // Coord max des noeuds, pas forcément à jour.

	bool isAbleToWriteFile = false;
	bool hasWrittenAFile = false;

	std::string nomGraphe = "Graphe";

	Graphe();

	~Graphe();

	Graphe(std::string nom);

	void initGraphData();

	void afficherLiens(std::string nom = "");

	void afficherLiensEmp(std::string nom = "");

	void afficherNoeuds(std::string nom = "");

	void afficherEmplacement(std::string nom = "");

	void afficherInfo(std::string nom = "");

	// Affiche s'il y a un noeud n'ayant pas de voisin.
	int afficherNoeudSeul(bool display = true, std::string nom = "");

	// Indique s'il y a plusieurs aretes ayant la meme source et destination
	int afficherAreteDouble(bool display = true, std::string nom = "");

	// Indique s'il ya plusieurs noeud au meme emplacement
	int afficherNoeudDouble(bool display = true, std::string nom = "");

	// Indique s'il y a plusieurs emplacements aux meme coordonnées
	int afficherEmplacementDouble(bool display = true, std::string nom = "");

	// Indique si pour un noeud associé a un emplacement si l'inverse est aussi vrai. Puis pareil pour les emplacements.
	int debugDesyncNoeudEmplacement(bool display = true, std::string nom = "");

	// Calcule si le score du graphe est le meme selon les différentes méthode que celui stocké.
	int debugScoreGraphe(bool display = true, std::string nom = "");

	// Affiche si un noeud n'est associé a aucun emplacement.
	int debugNoeudNonPlace(bool display = true, std::string nom = "");

	// Indique si pour un emplacement associé a une cellule si la cellule contient cet emplacement. Puis pareil pour les cellules.
	int debugDesyncEmplacementCell(bool display = true, std::string nom = "");

	// Indique si pour un coté associé a une cellule si la cellule contient ce coté. Puis pareil pour les cellules.
	int debugDesyncAreteCell(bool display = true, std::string nom = "");

	// Indique pour les noeuds,aretes et emplacements si leur ordre dans le vecteur correspond bien a son ID.
	int debugOrdreIdVector(bool display = true, std::string nom = "");

	// Indique si un noeud a une arete vers lui meme
	int debugSelfLoop(bool display = true, std::string nom = "");

	// Appelle toutes les fonctions de debug et renvoie un entier en fonction de si un bug a été détecté.
	int debugEverything(bool displayOther = false, bool displaySelf = false);

	// Enleve tout les noeuds de leur emplacement.
	void clearNodeEmplacement();

	// Place les noeuds aleatoirement sur les emplacements disponibles.
	// Ne tient pas a jour le score des noeuds ou du graphe.
	void placementAleatoire();

	// Place les noeuds sur l'emplacement de meme ID
	// Ne tient pas a jour le score des noeuds ou du graphe.
	void placementFixe();

	// Place un noeud aleatoirement sur un emplacement disponible.
	// Ne tient pas a jour le score du noeud ou du graphe.
	void placementNoeudAleatoire(int idNoeud);

	// Indique si le graphe contient une intersection illegale.
	bool hasIllegalCrossing();

	// Calcule le score d'intersection du graphe et le met a jour.
	long getNbCroisement();

	// Calcule le score d'intersection du graphe et le met a jour.
	long getNbCroisementGrid();

	long getNbCroisementNoeudIsole(int nodeId);

	long getNbCroisementNoeudIsole(int nodeId, int swapIndex);

	long getNbCroisementNoeudIsoleGrid(int nodeId);

	long getNbCroisementNoeudIsoleGrid(int nodeId, int swapIndex);

	long getNbCroisementNoeudIsoleGridLimite(int nodeId, bool& depasse, double limiteScore);

	long getNbCroisementNoeudIsoleGridLimite(int nodeId, int swapIndex, bool& depasse, double limiteScore);

	// Selectionne deux noeud et retourne celui avec le score le plus faible. (le moin d'intersection)
	int selectionNoeudTournoiBinaire(bool isScoreUpdated = false);

	// Selectionne n noeuds et retourne celui avec le score le plus faible. (le moin d'intersection)
	int selectionNoeudTournoiMultiple(int n, bool isScoreUpdated = false);

	// Selectionne deux emplacements different entre eux et de celui du noeud, puis renvoie le plus proche du noeud.
	// Le noeud doit etre place.
	int selectionEmplacementTournoiBinaire(int nodeId);

	// Selectionne n emplacements different entre eux et de celui du noeud, puis renvoie le plus proche du noeud.
	// Le noeud doit etre place.
	int selectionEmplacementTournoiMultiple(int n, int nodeId);

	int selectionEmplacementTriangulation(int nodeId, int profondeur = 1);

	// Effectue la selection du noeud en fonction de modeNoeud, 0=Aleatoire,1=TournoiBinaire,2=TournoiMultiple
	int selectionNoeud(int modeNoeud, int t, bool isScoreUpdated = false);

	// Effectue la selection de l'emplacement en fonction de modeEmplacement, 0=Aleatoire,1=TournoiBinaire,2=TournoiMultiple
	int selectionEmplacement(int modeEmplacement, int nodeId, int t, std::vector<std::vector<double>>& customParam, int iter);

	// Calcule le delay a appliquer lors du recuit si besoin.
	void calculDelaiRefroidissement(int& delay, std::vector<std::vector<double>>& customParam, int iter);

	// Calcul les parametres a utiliser pour la selection des emplacements
	void setupSelectionEmplacement(int modeEmplacement, double t, double cool, double seuil, std::vector<std::vector<double>>& customParam);

	// Calcule l'improve apres avoir simulé le déplacement du noeud nodeId vers le slot slotId.
	long calculImprove(int nodeId, int slotId, bool& swapped, int& idSwappedNode, bool useGrille);
	long calculImproveLimite(int nodeId, int slotId, bool& swapped, int& idSwappedNode, bool useGrille, bool& makeMove, double limiteImprove);

	long calculImproveTabou(int nodeId, int slotId, bool& swapped, int& idSwappedNode, bool useGrille, long bestImprove);

	// Modifie les parametres du rerecuit en fonction des customParam
	void applyRerecuitCustomParam(double& t, double& cool, double& coolt, double& seuil, bool adaptCool, std::vector<std::vector<double>>& customParam);

	// Modifie les parametres du recuit en fonction des customParam
	void applyRecuitCustomParam(double& coeffImprove, std::vector<std::vector<double>>& customParam);

	// Effectue une sauvegarde du graphe en fonction de la méthode utilisée.
	void saveBestResultRecuit(std::vector<int>& bestResultVector, Graphe& bestResultGraphe);

	// Charge la sauvegarde du graphe en fonction de la méthode utilisée.
	void loadBestResultRecuit(std::vector<int>& bestResultVector, Graphe& bestResultGraphe, long bestNbCroisement);

	// Met a jour les variables importantes du graphe pour indiquer ce qui est stocké
	void updateGraphDataRecuit();

	// Calcule le score du noeud en fonction de la méthode choisie.
	long calculScoreNodeMethode(int nodeId, int idSwappedNode, bool swapped, bool useGrille, bool useReel = false);
	long calculScoreNodeMethodeLimite(int nodeId, int idSwappedNode, bool swapped, bool useGrille, bool& makeMove, double limiteScore, bool useReel = false);

	// Met a jour les valeurs de croisement au debut du recuit en appelant les fonctions adéquates.
	void setupNombreCroisement(long& nbCroisement, long& bestCroisement, long& debutCroisement);

	// Lance l'algorithme de recuit simulé sur le graphe pour minimiser le nombre d'intersection
	// Met à jour la variable nombreCroisement du graphe.
	// delay est le nombre de tour auquel on reste à la même température, -1 pour le rendre dynamique en fonction de la taille du graphe.
	// modeNoeud et modeEMplacement sont le mode de sélection de noeud et d'emplacement, 0=Aléatoire, 1=TournoiBinaire, 2=TournoiMultiple
	// Renvoie le nombre d'intersection supprimée
	long recuitSimule(double& timeBest, std::chrono::time_point<std::chrono::system_clock> start, std::vector<std::vector<double>> customParam = { {} }, double cool = 0.99999, double t = 100.0, double seuil = 0.0001, int delay = 1, int modeNoeud = 0, int modeEmplacement = 2, bool useGrille = true, int timeLimit = -1);
	long recuitSimuleLimite(double& timeBest, std::chrono::time_point<std::chrono::system_clock> start, std::vector<std::vector<double>> customParam = { {} }, double cool = 0.99999, double t = 100.0, double seuil = 0.0001, int delay = 1, int modeNoeud = 0, int modeEmplacement = 2, bool useGrille = true, int timeLimit = -1);

	// Lance l'algorithme de recuit simulé sur le graphe pour minimiser le nombre d'intersection
	// Met à jour la variable nombreCroisement du graphe si elle etait a jour avant.
	void stepRecuitSimule(double& t, int& nbCrois, double cool = 0.99999, int modeNoeud = 0, int modeEmplacement = 0, std::vector<std::vector<double>> customParam = { {} });

	// Applique le recuit simulé plusieurs fois
	// Met a jour le nombre de croisement du graphe.
	void rerecuitSimule(double& timeBest, int& nombreRecuit, std::chrono::time_point<std::chrono::system_clock> start, std::vector<std::vector<double>> customParam = { {} }, int iter = -1, double cool = 0.99999, double coolt = 0.99, double t = 100.0, double seuil = 0.0001, int delay = 1, int modeNoeud = 0, int modeEmplacement = 2, bool useGrille = true, int timeLimit = -1, bool firstWaveImp = false, bool adaptCool = false);

	long recuitSimuleChallenge(double cool = 0.99999, double t = 100.0, double seuil = 0.0001);
	void rerecuitSimuleChallenge(double coolt = 0.99, double t = 100.0, double seuil = 0.0001);

	// Applique l'algorithme meilleur deplacement sur le graphe.
	// On parcoure tout les noeuds et on teste chaque deplacement possible et on effectue le meilleur s'il ameliore le score. (O(n²*e))
	// Met a jour le nombre de croisement du graphe.
	void bestDeplacement();

	void bestDeplacementLimite();

	// Selectionne les emplacements disponibles a egale distance d'un point et en renvoie un aleatoirement.
	Emplacement* getEmplacementPlusProche(std::pair<int, int>& origin);

	// Selectionne les emplacements different de l'emplacement en parametre a egale distance de celui ci et en renvoie un aleatoirement.
	Emplacement* getEmplacementPlusProche(Emplacement* origin);

	// Retourne le centre de gravité des noeuds placés en flottant
	std::pair<double, double> getCentreGraviteDoubleNoeuds();

	// Retourne le centre de gravite des emplacements.
	std::pair<int, int> getCentreGravite();

	// Retourne le centre de gravite des noeuds place
	std::pair<int, int> getCentreGraviteNoeudPlaces();

	// Retourne le centre de gravite des voisins place d'un noeud.
	std::pair<int, int> getCentreGraviteVoisin(Noeud* noeud);

	// Renvoie l'id du meilleur emplacement disponible
	// Le noeud passé en argument ne doit pas être placé
	int getMeilleurEmplacement(Noeud& meilleurNoeud);

	// Renvoie l'id du meilleur emplacement disponible
	// Le noeud passé en argument ne doit pas être placé
	int getMeilleurEmplacementScore(Noeud& meilleurNoeud);

	// Renvoie le meilleur emplacement disponible
	int getMeilleurEmplacementGravite(Noeud* meilleurNoeud, std::pair<int, int>& gravite);

	// Indique s'il reste au moin un emplacement disponible
	bool emplacementRestant();

	// Indique si tout les noeuds d'un graphe sont place
	bool estPlace();

	// Renvoie le nombre de noeud non associé à un emplacement.
	int nbAPlace();

	// Ajoute (n-1)*_emplacements.size emplacements dans le graphe si possible
	void generateMoreEmplacement(int n);

	// Ajoute _noeud.size() emplacement par défaut ou n emplacement 
	// Si le nombre d'emplacement est laissé à -1, on génere dans une grille de taille _noeud.size()*_noeud.size()
	void generateEmplacements(int n = -1);

	// Sauvegarde dans un vecteur les id des emplacements auquels les noeuds sont assignés
	// Les noeuds doivent etre place
	std::vector<int> saveCopy();

	// Charge une copie en copiant les id dans le vecteur et en les affectant aux noeuds du graphe.
	// La copie doit provenir d'un graphe genere du meme fichier de depart
	void loadCopy(std::vector<int> vectorId);

	// Vide tout les vecteurs du graphe
	void clearGraphe();

	// Ne vide pas le vecteur de noeud, copie l'emplacement et le score
	void copyNodesFromGraphe(Graphe& graphe);

	// Vide les vecteurs du graphe et effectue une copie du contenu des vecteur du graphe en parametre
	void copyFromGraphe(Graphe& graphe);

	// Vide tout les vecteurs et effectue une copies de tout les objets dans le graphe
	void copyGrilleFromGraphe(Graphe& graphe);

	// Recupere un vecteur d'emplacement trié par x et y
	void getSortedEmpVecFromGraphe(std::vector<int>& sortedIdVec, Graphe& G);

	// Calcule le score du noeud en fonction de ses vecteur d'intersections
	long getScoreCroisementNodeFromArrays(int nodeIndex);

	// Calcule le score du noeud en parametre. N'utilise pas la grille.
	long getScoreCroisementNode(int nodeIndex);

	// Calcule le score du noeud nodeIndex sans ajouter le score produit par le noeud swapIndex.
	long getScoreCroisementNode(int nodeIndex, int swapIndex);

	// Calcule le score du noeud en parametre. Utilise la grille
	long getScoreCroisementNodeGrid(int nodeIndex);
	// Calcule le score du noeud nodeIndex sans ajouter le score produit par le noeud swapIndex. Utilise la grille
	long getScoreCroisementNodeGrid(int nodeIndex, int swapIndex);

	long getScoreCroisementNodeGridLimite(int nodeIndex, bool& makeMove, double limiteScore);
	long getScoreCroisementNodeGridLimite(int nodeIndex, int swapIndex, bool& makeMove, double limiteScore);

	long getScoreCroisementNodeGridLimit(int nodeIndex, long limitScore);

	long getScoreCroisementNodeGridLimit(int nodeIndex, int swapIndex, long limitScore);

	// Met a jour les vecteurs de la grille et des aretes
	void applyNewAreteCelluleVec(std::vector<std::vector<int>>& vecId, int nodeIndex);

	// Indique aux aretes du noeuds que leur liste d'intersection n'est pas à jours.
	void changeUpdateValue(int idNode);

	// Recalcule les intersections d'une arete et met a jour sa liste d'intersection uniquement
	void recalculateInterArray(int idArray);

	// Met a jour le score du noeud et met a jour la liste d'intersection de ses aretes
	// Modifie le score des noeuds affectes par la liste
	void updateNodeScore(int idNode);

	// Renvoie le score d'intersection, n'utilise pas la grille
	long getNbCroisementConst() const;

	// Renvoie le nombre d'intersection total sans malus, n'utilise pas la grille
	long getNbCroisementDiff();

	// Renvoie le nombre d'intersection total sans malus, utilise la grille
	long getNbCroisementDiffGrid();

	// Ne met pas a jour le nombre de croisement du graphe
	long getNbCroisementArray();

	// Renvoie le nombre de noeud non place
	int nbNoeudNonPlace();

	void initCompleteGraph(int n, bool setup = false);

	// Met a jour le score du graphe et de ses noeuds ainsi que les vecteurs d'intersections des aretes
	void initGraphAndNodeScoresAndCrossings();

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
		if (isNombreCroisementUpdated) { n1 = nombreCroisement; }
		else { n1 = getNbCroisementConst(); }
		if (G.isNombreCroisementUpdated) { n2 = G.nombreCroisement; }
		else { n2 = G.getNbCroisementConst(); }
		return (n1 < n2);
	}

	// ----- CREATION D'UN Graph A PARTIR D'UN FICHIER JSON -----
	// Precondition: Les id des noeuds sont ordonnees et commencent par 0
	void readFromJsonGraph(std::string input);

	// Lecture des slots
	// Precondition: Les id des slots sont ordonn�s et commencent par 0
	void readFromJsonSlots(std::string input);

	// ----- CREATION D'UN Graph A PARTIR D'UN FICHIER JSON -----
	// Precondition: Les id des noeuds sont ordonn�es et commencent par 0
	void readFromJsonGraphAndSlot(std::string input);

	void readFromJsonChallenge(std::string input);

	// Sauvegarde des slots dans le fichier output
	void writeToJsonSlots(std::string output);

	void writeToJsonGraph(std::string output);

	void writeToJsonGraphAndSlots(std::string output);

	void writeToJsonChallenge(std::string output);

	// Creer la grille d'emplacement de taille gridHeight*gridWidth
	void generateGrid(int gridWidth = -1, int gridHeight = -1);

	// Creer une triangulation de delaunay des emplacements et peulple la carte _c
	void triangulationDelaunay();

	// Remplie la grille de cellules, nombre de ligne et nombre de colones.
	// Les emplacements sont deplaces sur des entiers pairs ((coord*2+)2) et les cotés de la grilles sont sur des entiers impairs avec 1 comme min.
	// Cela a pour effet de n'avoir qu'une cellule associée a chaque emplacement.
	void initGrille(int row = -1, int column = -1);

	// Pareil que initGrilleNoMove, les cellules ont la meme largeur et hauteur
	void initGrilleCarre();

	// Remplie la grille de cellules, nombre de ligne et nombre de colones.
	// Chaque emplacement est associé a 1,2 ou 4 cellules.
	void initGrilleNoMove(int row = -1, int column = -1);

	// Supprime la grille courante et la reinitialise
	void reinitGrille();

	// Vide la grille courante
	void clearGrille();

	// Supprime la grille courante
	void deleteGrille();

	// Avec coord flottantes uniquement
	void registerNodesInGrid();

	// Avec coord flottantes uniquement
	void registerNodeInGrid(int nodeId);

	// Avec coord flottantes uniquement
	void registerNodesAndEdgesInGrid();

	// Enregistre les emplacements et les aretes dans la grille
	void registerSlotsAndEdgesInGrid();

	// Enregistre les aretes dans la grille
	void registerEdgesInGrid();

	// Enregistre avec alignements d'emplacements
	void registerSlotsInGrid();

	void registerIsolatedNodesInGrid();

	// Enregistre avec alignements d'emplacements
	void registerSlotsInGridNoMove();

	void registerEdgesInGridNoMove();

	// Effectue la lecture de fichier json pour le graphe et les emplacements
	void setupGraphe(std::string fileGraphe, std::string fileSlot);

	void translateGrapheToOrigin();

	// Multiplie les coordonnées de chaque emplacement par n. Ajuste gridWidth et gridHeight
	void scaleGraph(int n);

	Emplacement* getClosestEmplacementFromPoint(double x, double y, bool isFree = false);

	Emplacement* getClosestEmplacementFromPointGrid(double x, double y, bool isFree = false);

	// Renvoie le noeud le plus proche des coordonnées passées en parametres, utile pour openGL.
	int getClosestNodeFromPoint(double x, double y);

	// Renvoie le noeud en coordonnées réelles le plus proche des coordonnées passées en parametres, utile pour openGL.
	int getClosestNodeFromPointReel(double x, double y);

	// Appelle l'algorithme de stress majorization sur le graphe.
	void stressMajorization(std::vector<std::vector<double>> customParam = { {} }, int methode = 0, bool useClosest = false);

	// Effectue le deplacement d'un seul noeud avec l'algorithme de stressMajorization
	void stepStressMajorization(std::vector<std::vector<double>> customParam = { {} }, int edgeCost = 45);

	// Graphe connexe ou non
	bool isGrapheConnected();

	// Sauvegarde le degree max et moyen des noeuds dans les attributs du graphes: maxVoisin, avgVoisin.
	void calcMaxAndAverageDegree();

	// Implémentation du pivotMDS d'ogdf
	void placementPivotMDS(std::vector<std::vector<double>> customParam = { {} }, int edgeCost = 45, int nbPivot = 50);

	// Renvoie la distance moyenne des aretes de la triangulation de delaunay
	double distMoyDelaunayVoisins();

	// Renvoie la distance moyenne entre les emplacement apres n tirages aleatoires
	double distMoyNTirage(int n);

	// Affecte les valeurs a minXNode maxXNode minYNode maxYNode dans le graphe
	void getMinMaxFromNodes();

	// Renvoie la moyenne des longueurs des aretes du graphe
	double moyenneLongueurAretes();

	// Renvoie les identifiants des noeuds présents dans la plus grande composante connexe
	std::vector<int> plusGrandeComposanteConnexe();

	// Numerote les emplacement dans la grille avec un numero entre 0 et index.
	void generateActivationGrid(int gridWidth, int gridHeight, int index);

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

	// Heuristique de recherche tabou, modifie le graphe et garde la meilleure configuration trouvée
	void rechercheTabou();

	// Déplace le noeud nodeId à l'emplacement slotId, effectue un swap si pas disponible, met à jour la grille et le score.
	void moveNodeToSlot(int nodeId, int slotId, bool useGrille);

	// Algorithme FMME avec ou sans emplacement, minute indique si on garde le meilleur placement sur une minute
	void placementFMME(bool minute = false);

	// Modifie les valeurs de PENALITE_MAX et PENALITE_MAX_SELF par les valeurs pen1 et pen2, par défaut le nombre de noeud dans le graphe
	void updatePenalite(int pen1, int pen2);

	// Remplie le vecteur _noeudsSeuls du graphe
	void updateIsolatedNodes();

	// Ajoute des noeuds isolés dans le graphe jusqu'a en avoir autant que d'emplacements
	void fillWithSingleNodes();


private:
	// Recherche l'emplacement le plus proche en parcourant les cellules
	void searchInCellClosestEmplacement(double x, double y, int cellX, int cellY, int& closestEmpId, double& minDist, bool isFree);
	// Aggrandi le vecteur de recherche pour l'emplacement le plus proche lors du placement.
	void enlargeSearchVector(std::vector<std::pair<int, int>>& searchVector);
	// Renvoie un entier indiquant la direction de l'arete noeud1 vers noeud2.
	//0=Right,1=TopRight,2=Top,3=TopLeft,4=Left,5=BottomLeft,6=Bottom,7=BottomRight
	int getDirectionArete(int idArete);
	// Met a jour les cellules passée par l'arete areteId
	void recalcAreteCellule(int areteId);
	// Met a jour les aretes du noeud dans les cellules
	void recalcNodeCellule(int nodeId);
	// Met a jour les cellules dans le noeud uniquement
	void recalcSpecificNodeCell(int nodeId);
	// Met a jour les cellules et le noeud
	void recalcNoeudIsoleGrid(int nodeId);
	// Ajoute les id dans les cellules et dans l'arete
	void initAreteCellule(int areteId);
	// Ajoute les id dans les cellules et dans les aretes du noeuds
	void initNodeCellule(int nodeId);
	// Calcule le vecteur de cellule sans le mettre a jour
	void calcAreteCelluleVec(std::vector<int>& vecInt, int areteId);
	// Calcule le vecteur de vecteur de cellule sans le mettre a jour
	void calculeNodeCelluleVec(std::vector<std::vector<int>>& vecVecInt, int nodeId);
	void addCommonNodeVector(int nodeId); // Met a jour la matrice de voisinage pour ce noeud
	void removeCommonNodeVector(int nodeId); // Supprime les references de voisinage de ce noeud
};

bool compareGraphePtr(Graphe* g1, Graphe* g2);

#endif