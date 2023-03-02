#ifndef GRAPHE_HPP
#define GRAPHE_HPP

#include <vector>
#include <set>
#include <string>
#include <utility>
#include "utilitaire.hpp"
#include "aretes.hpp"
#include "noeud.hpp"
#include "carte.h"
#include "cellule.hpp"
#include "personnel.hpp"

class Graphe {
public:
	std::vector<Emplacement> _emplacements;
	std::vector<Aretes> _aretes;
	std::vector<Noeud> _noeuds;

	std::set<Aretes*> areteIll;
	std::set<Aretes*> areteIllSelf;
	std::set<Aretes*> areteInter;

	Carte _c;

	std::vector<std::vector<Cellule>> grille;
	std::vector<Cellule*> grillePtr;

	int PENALITE_MAX = 1000;
	int PENALITE_MAX_SELF = 1001;
	int gridHeight = 10;
	int gridWidth = 10;

	long nombreCroisement = -1; // Attention cette variable n'est pas forcément à jour! Voir 'isNombreCroisementUpdated'
	bool isNombreCroisementUpdated = false; // Indique si la valeur dans 'nombreCroisement' est à jour.
	bool isNodeScoreUpdated = false; // Indique les les scores stockés dans les noeuds sont à jour.
	bool isIntersectionVectorUpdated = false; // Indique si les vecteurs d'intersections dans les edge sont à jour.
	bool isCarteSetUp = false; // Indique si la triangulation de delaunay sur le graphe a été faite.

	int maxVoisin = -1; // Nombre maximum de voisin d'un noeud dans le graphe. Pas forcément à jour.

	bool DEBUG_GRAPHE = false; // Affiche uniquement les informations importantes
	bool DEBUG_PROGRESS = false; // Affiche des informations pendant les itérations
	bool DEBUG_OPENGL = false; // Utile pour les affichage des intersections illégales dans openGL

	bool RECUIT_LIMIT_3600 = true; // Indique si on limite le temps d'un rerecuit a 3600 secondes.

	std::string nomGraphe = "Graphe";

	Graphe(){}

	Graphe(std::string nom);

	void afficherLiens(std::string nom = "");

	void afficherLiensEmp(std::string nom = "");

	void afficherNoeuds(std::string nom = "");

	void afficherEmplacement(std::string nom = "");

	void afficherInfo(std::string nom = "");

	// Affiche s'il y a un noeud n'ayant pas de voisin.
	int afficherNoeudSeul(bool display=true, std::string nom = "");

	// Indique s'il y a plusieurs aretes ayant la meme source et destination
	int afficherAreteDouble(bool display=true, std::string nom = "");

	// Indique s'il ya plusieurs noeud au meme emplacement
	int afficherNoeudDouble(bool display=true, std::string nom = "");

	// Indique s'il y a plusieurs emplacements aux meme coordonnées
	int afficherEmplacementDouble(bool display=true, std::string nom = "");

	// Indique si pour un noeud associé a un emplacement si l'inverse est aussi vrai. Puis pareil pour les emplacements.
	int debugDesyncNoeudEmplacement(bool display=true, std::string nom = "");

	// Calcule si le score d'un noeud est faux ou non.
	int debugScoreNoeud(bool display=true, std::string nom = "");

	// Calcule si il y a une incohérence dans les vecteurs d'intersections des cotés.
	int debugInterArrays(bool display=true, std::string nom = "");

	// Calcule si le score du graphe est le meme selon les différentes méthode que celui stocké.
	int debugScoreGraphe(bool display=true, bool useArray = false, std::string nom = "");

	// Affiche si un noeud n'est associé a aucun emplacement.
	int debugNoeudNonPlace(bool display=true, std::string nom = "");

	// Affiche le score calculé avec l'ancienne fonction d'intersection.
	int debugOldCroisement(bool display=true, std::string nom = "");

	// Indique si pour un emplacement associé a une cellule si la cellule contient cet emplacement. Puis pareil pour les cellules.
	int debugDesyncEmplacementCell(bool display=true, std::string nom = "");

	// Indique si pour un coté associé a une cellule si la cellule contient ce coté. Puis pareil pour les cellules.
	int debugDesyncAreteCell(bool display=true, std::string nom = "");

	// Appelle toutes les fonctions de debug et renvoie un entier en fonction de si un bug a été détecté.
	int debugEverything(bool displayOther=false, bool displaySelf=false);

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

	// Calcule le score d'intersection pour un graphe qui n'est pas forcement entierement place
	// Ne met pas a jour le nombre de croisement.
	long getNbCroisementGlouton();

	// Calcule le score d'intersection pour un noeud dans un graphe qui n'est pas forcement entierement place
	// Ne met pas a jour le nombre de croisement
	long getNbCroisementGloutonScore(int nodeId);

	// Selectionne deux noeud et retourne celui avec le score le plus faible. (le moin d'intersection)
	int selectionNoeudTournoiBinaire(bool isScoreUpdated=false);

	// Selectionne n noeuds et retourne celui avec le score le plus faible. (le moin d'intersection)
	int selectionNoeudTournoiMultiple(int n, bool isScoreUpdated=false);

	// Selectionne deux emplacements different entre eux et de celui du noeud, puis renvoie le plus proche du noeud.
	// Le noeud doit etre place.
	int selectionEmplacementTournoiBinaire(int nodeId);

	// Selectionne n emplacements different entre eux et de celui du noeud, puis renvoie le plus proche du noeud.
	// Le noeud doit etre place.
	int selectionEmplacementTournoiMultiple(int n, int nodeId);

	int selectionEmplacementTriangulation(int nodeId, int profondeur=1);

	// Effectue la selection du noeud en fonction de modeNoeud, 0=Aleatoire,1=TournoiBinaire,2=TournoiMultiple
	int selectionNoeud(int modeNoeud, int t, bool isScoreUpdated=false);

	// Effectue la selection de l'emplacement en fonction de modeEmplacement, 0=Aleatoire,1=TournoiBinaire,2=TournoiMultiple
	int selectionEmplacement(int modeEmplacement, int nodeId, int t, std::vector<double>& customParam, int iter);

	// Calcule le delay a appliquer lors du recuit si besoin.
	void calculDelaiRefroidissement(int& delay, std::vector<double>& customParam, int iter);

	// Calcule l'improve apres avoir simulé le déplacement du noeud nodeId vers le slot slotId.
	int calculImprove(int nodeId, int slotId, bool& swapped, int& idSwappedNode,Emplacement*& oldEmplacement,bool useGrille, bool useScore);

	// Modifie les parametres du rerecuit en fonction des customParam
	void applyRerecuitCustomParam(double& t,double& cool,double& coolt,double& seuil,std::vector<double> customParam={});

	// Effectue une sauvegarde du graphe en fonction de la méthode utilisée.
	void saveBestResultRecuit(std::vector<int>& bestResultVector, Graphe& bestResultGraphe, bool useScore, bool useGrille);

	// Charge la sauvegarde du graphe en fonction de la méthode utilisée.
	void loadBestResultRecuit(std::vector<int>& bestResultVector, Graphe& bestResultGraphe, long bestNbCroisement, bool useScore, bool useGrille);

	// Met a jour les variables importantes du graphe pour indiquer ce qui est stocké
	void updateGraphDataRecuit(bool useScore, bool useGrille);

	// Calcule le score du noeud en fonction de la méthode choisie.
	long calculScoreNodeMethode(int nodeId,int idSwappedNode, bool swapped, bool useGrille, bool useScore);

	// Lance l'algorithme de recuit simulé sur le graphe pour minimiser le nombre d'intersection
	// Met à jour la variable nombreCroisement du graphe.
	// delay est le nombre de tour auquel on reste à la même température, -1 pour le rendre dynamique en fonction de la taille du graphe.
	// modeNoeud et modeEMplacement sont le mode de sélection de noeud et d'emplacement, 0=Aléatoire, 1=TournoiBinaire, 2=TournoiMultiple
	void recuitSimule(double &timeBest, std::vector<double> customParam = {}, double cool = 0.99999, double t = 100.0, double seuil = 0.0001, int delay = 1, int modeNoeud = 0, int modeEmplacement = 2,bool useGrille=true,bool useScore=false);

	// Lance l'algorithme de recuit simulé sur le graphe pour minimiser le nombre d'intersection
	// Met à jour la variable nombreCroisement du graphe si elle etait a jour avant.
	void stepRecuitSimule(double& t, int& nbCrois, double cool = 0.99999, int modeNoeud = 0, int modeEmplacement = 0,std::vector<double> customParam = {});

	// Applique le recuit simulé plusieurs fois
	// Met a jour le nombre de croisement du graphe.
	void rerecuitSimule(double &timeBest, int &nombreRecuit, std::vector<double> customParam = {}, int iter = -1, double cool = 0.99999, double coolt = 0.99, double t = 100.0, double seuil = 0.0001, int delay = 1, int modeNoeud = 0, int modeEmplacement = 2,bool useGrille=true,bool useScore=false);

	// Applique l'algorithme meilleur deplacement sur le graphe.
	// On parcoure tout les noeuds et on teste chaque deplacement possible et on effectue le meilleur s'il ameliore le score. (O(n²*e))
	// Met a jour le nombre de croisement du graphe.
	void bestDeplacement();

	// Selectionne les emplacements disponibles a egale distance d'un point et en renvoie un aleatoirement.
	Emplacement* getEmplacementPlusProche(std::pair<int,int>& origin);

	// Selectionne les emplacements different de l'emplacement en parametre a egale distance de celui ci et en renvoie un aleatoirement.
	Emplacement* getEmplacementPlusProche(Emplacement* origin);

	// Retourne le centre de gravite des emplacements.
	std::pair<int,int> getCentreGravite();

	// Retourne le centre de gravite des noeuds place
	std::pair<int,int> getCentreGraviteNoeudPlaces();

	// Retourne le centre de gravite des voisins place d'un noeud.
	std::pair<int,int> getCentreGraviteVoisin(Noeud* noeud);

	// Renvoie l'id du meilleur emplacement disponible
	// Le noeud passé en argument ne doit pas être placé
	int getMeilleurEmplacement(Noeud& meilleurNoeud);

	// Renvoie l'id du meilleur emplacement disponible
	// Le noeud passé en argument ne doit pas être placé
	int getMeilleurEmplacementScore(Noeud& meilleurNoeud);

	// Renvoie le meilleur emplacement disponible
	int getMeilleurEmplacementGravite(Noeud* meilleurNoeud, std::pair<int,int>& gravite);

	// Ancien algorithme glouton non optimisé mais fonctionnel.
	void glouton();

	// Algorithme glouton
	void gloutonRevisite();

	void gloutonRevisiteGravite();

	void gloutonRevisiteVoisin();

	void completeBasicGlouton();

	void completeBasicGloutonScore(std::vector<int>& vecNode, int tailleMax);

	void completeBasicGloutonScoreGrille(std::vector<int>& vecNode, int tailleMax);

	void completePlacementAleatoire();

	void completePlacementAleatoireScore(std::vector<int>& vecNode, int tailleMax);

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

	// Vide les vecteurs du graphe et effectue une copie du contenu des vecteur du graphe en parametre
	void copyFromGrapheGenetique(Graphe& graphe);

	// Recupere un vecteur d'emplacement trié par x et y
	void getSortedEmpVecFromGraphe(std::vector<int>& sortedIdVec, Graphe& G);
	
	// Calcule le score du noeud en fonction de ses vecteur d'intersections
	long getScoreCroisementNodeFromArrays(int nodeIndex);

	// Calcule le score de l'enfant en fonction des aretes passées dans areteVec
	long getNodeScoreEnfant(Graphe& G, std::vector<int>& areteVec, int nodeIndex);

	// Calcule le score de l'enfant en fonction de la grille chez l'enfant
	long getNodeScoreEnfantGrille(Graphe& G, int nodeIndex);

	// Calcule le score du noeud en parametre.
	long getScoreCroisementNode(int nodeIndex);

	// Calcule le score du noeud nodeIndex sans ajouter le score produit par le noeud swapIndex.
	long getScoreCroisementNode(int nodeIndex, int swapIndex);

	// Calcule le score du noeud en parametre. Utilise la grille
	long getScoreCroisementNodeGrid(int nodeIndex);

	// Calcule le score du noeud nodeIndex sans ajouter le score produit par le noeud swapIndex. Utilise la grille
	long getScoreCroisementNodeGrid(int nodeIndex, int swapIndex);

	// Calcule le score du noeud en parametre. Utilise la grille
	long getScoreCroisementNodeGrid(std::vector<std::vector<int>>& vecId,int nodeIndex);

	// Calcule le score du noeud nodeIndex sans ajouter le score produit par le noeud swapIndex. Utilise la grille
	long getScoreCroisementNodeGrid(std::vector<std::vector<int>>& vecId, int nodeIndex, int swapIndex);

	// Met a jour les vecteurs de la grille et des aretes
	void applyNewAreteCelluleVec(std::vector<std::vector<int>>& vecId, int nodeIndex);

	void recalculateIllegalIntersections(int i);

	// Effectue le croisement entre deux parents,
	// Renvoie vrai si les deux parents ne sont pas identique
	// Ne met pas à jour le nombre de croisement d'un graphe
	bool croisementVoisinageFrom(Graphe& graphe1, Graphe& graphe2, bool useRand);

	// Effectue une seule etape dans l'algorithme de croisement, utile pour l'affichage opengl
	void stepCroisementVoisinageFrom(Graphe& graphe1, Graphe& graphe2, bool useRand, int &nbNoeudATraiter, int &currentGrapheNumber);

	// Indique aux aretes du noeuds que leur liste d'intersection n'est pas à jours.
	void changeUpdateValue(int idNode);

	// Recalcule les intersections d'une arete et met a jour sa liste d'intersection uniquement
	void recalculateInterArray(int idArray);

	// Met a jour le score du noeud et met a jour la liste d'intersection de ses aretes
	// Modifie le score des noeuds affectes par la liste
	void updateNodeScore(int idNode);

	// Effectue le croisement entre deux parents,
	// Renvoie vrai si les deux parents ne sont pas identique
	// Met a jour le nombre de croisement du graphe
	bool croisementVoisinageScore(Graphe& originalGraphe1, Graphe& originalGraphe2, bool useRand);

	// Effectue une seule etape dans l'algorithme de croisement, utile pour l'affichage opengl
	void stepCroisementVoisinageScore(Graphe& graphe1, Graphe& graphe2, bool useRand, int &nbNoeudATraiter, int &currentGrapheNumber);

	// Effectue le croisement entre deux parents,
	// Renvoie vrai si les deux parents ne sont pas identique
	// Met a jour le nombre de croisement du graphe
	bool croisementBestOfBoth(Graphe& originalGraphe1, Graphe& originalGraphe2, bool useRand);
	
	// Effectue un croisement entre deux parents en séparant le graphe en deux parties
	// Les emplacements sont triés par leur coord X
	// Ne met pas à jour la variable nombreCroisement du graphe
	bool croisementHalfParent(Graphe& graphe1, Graphe& graphe2, std::vector<int>& sortedEmpId, bool useRand);

	// Effectue un croisement entre deux parents en selectionnant un noeud aleatoirement de chaque parent en alternant
	// Ne met pas à jour la variable nombreCroisement du graphe
	bool croisementAleatoire(Graphe& graphe1, Graphe& graphe2, bool useRand);

	// Effectue le croisement entre deux parents,
	// On selectionne un noeud en alternant de parent, celui qui creer le moin d'intersection si le place chez l'enfant
	// Renvoie vrai si les deux parents ne sont pas identique
	// Ne met pas a jour le nombre de croisement du graphe et des noeuds
	bool croisementEnfantScore(Graphe& originalGraphe1, Graphe& originalGraphe2, bool useRand);
	
	bool croisementEnfantScoreGrille(Graphe& originalGraphe1, Graphe& originalGraphe2, bool useRand);
	
	// Nombre de noeuds du même ID placé aux mêmes emplacements.
	int nbNoeudEnCommun(const Graphe& graphe);

	// Renvoie un vecteur contenant les id des noeuds placé au meme endroit
	// Les deux graphes doivent etre place entierement
	std::vector<int> noeudEnCommun(Graphe &G);

	// Modifie le vecteur de noeuds en commun et le vecteur de noeud non commun en parametre
	void separateNodesInCommon(Graphe &G, std::vector<int>& commonNodeVec, std::vector<int>& otherNodeVec, std::vector<int>& indexNodeInOtherVec);

	long getNbCroisementConst() const;

	long getNbCroisementOldMethodConst() const;

	// Ne met pas a jour le nombre de croisement du graphe
	long getNbCroisementArray();

	// Renvoie le nombre de noeud non place
	int nbNoeudNonPlace();

	// Met a jour le score du graphe et de ses noeuds ainsi que les vecteurs d'intersections des aretes
	void initGraphAndNodeScoresAndCrossings();

	// Vide les sets contenant les intersections, utile uniquement pour l'affichage openGL
	void clearSetAreteInter();

	// Indique si les deux noeuds ont une aretes commune
	bool areVoisin(int nodeId1, int nodeId2);

	// Retourne l'id de l'arete qui relie deux points en parametre, -1 si aucune est trouvee
	// Utilise la liste d'arete stockee dans les nodes
	int getAreteFromTwoNodes(int nodeId1, int nodeId2);

	// Renvoie le score du noeud nodeId1 sans prendre en compte le score d'intersection avec le noeud nodeId2
	long getNodeLinkedScore(int nodeId1, int nodeId2);

	bool operator < (const Graphe& G) const {
		long n1, n2;
		if (isNombreCroisementUpdated) { n1 = nombreCroisement; } else { n1 = getNbCroisementConst(); }
		if (G.isNombreCroisementUpdated) { n2 = G.nombreCroisement; } else { n2 = G.getNbCroisementConst(); }
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

	void readFromGraphmlGraph(std::string input);

	// Sauvegarde des slots dans le fichier output
	void writeToJsonSlots(std::string output);

	void writeToJsonGraph(std::string output);

	// ----- CREATION D'UN Graph A PARTIR D'UN FICHIER JSON -----
	// Creer les emplacements a partir des anciennes coord
	// Les noeuds doivent avoir des coordonn�es
	std::vector<int> readFromJsonOldGraph(std::string input);

	// Applique un algorithme genetique sur une population, la moitié de la population est remplacée à chaque génération en croisant 2 parents appartenant aux 50% des meilleurs.
	// useRecuit indique si on applique un court recuit simulé à chaque nouvel enfant créé
	// useRand indique si l'on doit utiliser l'algorithme aléatoire plutôt que le glouton pour le replacage des noeuds lors du croisement
	// modeCroisement indique quel algorithme de croisement utiliser 0=Voisinage, 1=HalfParent, 2=Aléatoire, 3=VoisinageV2
	void grapheGenetique(double &timeBest, int &bestIteration, int &lastIteration, int population, int maxIteration, const std::string& nomGraphe, const std::string& nomSlot, bool useRecuit=false, bool useRand=false, int modeCroisement=0);

	// Creer la grille d'emplacement de taille gridHeight*gridWidth
	void generateGrid(int gridWidth=-1, int gridHeight=-1);

	// Creer une triangulation de delaunay des emplacements et peulple la carte _c
	void triangulationDelaunay();

	// Remplie la grille de cellules, nombre de ligne et nombre de colones
	void initGrille(int row=-1,int column=-1,bool decalleGrille=true);

	// Supprime la grille courante et la reinitialise
	void reinitGrille();

	// Vide la grille courante
	void clearGrille();

	// Enregistre les emplacements et les aretes dans la grille
	void registerSlotsAndEdgesInGrid();

	// Enregistre les aretes dans la grille
	void registerEdgesInGrid();

	// Enregistre avec alignements d'emplacements
	void registerSlotsAndEdgesInGridNoMove();

	// Renvoie un entier indiquant la direction de l'arete noeud1 vers noeud2.
	//0=R,1=TR,2=T,3=TL,4=L,5=BL,6=B,7=BR
	int getDirectionArete(int idArete);

	// Met a jour les cellules passée par l'arete areteId
	void recalcAreteCellule(int areteId);

	// Met a jour les aretes du noeud dans les cellules
	void recalcNodeCellule(int nodeId);

	// Ajoute les id dans les cellules et dans l'arete
	void initAreteCellule(int areteId);

	// Ajoute les id dans les cellules et dans les aretes du noeuds
	void initNodeCellule(int nodeId);

	// Calcule le vecteur de cellule sans le mettre a jour
	void calcAreteCelluleVec(std::vector<int>& vecInt,int areteId);

	// Calcule le vecteur de vecteur de cellule sans le mettre a jour
	void calculeNodeCelluleVec(std::vector<std::vector<int>>& vecVecInt, int nodeId);

	void setupGraphe(std::string fileGraphe, std::string fileSlot);

	void translateGrapheToOrigin();

};

#endif