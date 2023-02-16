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

class Graphe {
public:
	std::vector<Emplacement> _emplacementsPossibles;
	std::vector<Aretes> _liens;
	std::vector<Noeud> _noeuds;

	std::set<Aretes*> areteIll;
	std::set<Aretes*> areteIllSelf;
	std::set<Aretes*> areteInter;

	Carte _c;

	int PENALITE_MAX = 1000;
	int PENALITE_MAX_SELF = 1001;
	int gridHeight = 10;
	int gridWidth = 10;

	int compteurSwap = 0;
	int compteurDeplacement = 0;
	int compteurSwapE = 0;
	int compteurDeplacementE = 0;

	// Attention cette variable n'est pas forcément à jour!
	long nombreCroisement = -1;
	bool isNombreCroisementUpdated = false;
	bool isNodeScoreUpdated = false;
	bool isIntersectionVectorUpdated = false;
	bool isCarteSetUp = false;

	// Nombre maximum de voisin d'un noeud dans le graphe.
	int maxVoisin = -1;

	bool DEBUG_GRAPHE = true;
	bool DEBUG_PROGRESS = false;
	bool DEBUG_OPENGL = false;

	std::string nomGraphe = "Graphe";

	Graphe(){}

	Graphe(std::string nom);

	void afficherLiens(std::string nom = "");

	void afficherNoeuds(std::string nom = "");

	void afficherEmplacement(std::string nom = "");

	void afficherNoeudSeul(bool display=true, std::string nom = "");

	void afficherInfo(std::string nom = "");

	void afficherAreteDouble(bool display=true, std::string nom = "");

	void afficherNoeudDouble(bool display=true, std::string nom = "");

	void afficherEmplacementDouble(bool display=true, std::string nom = "");

	void debugDesyncNoeudEmplacement(bool display=true, std::string nom = "");

	void debugScoreNoeud(bool display=true, std::string nom = "");

	void debugInterArrays(bool display=true, std::string nom = "");

	void debugScoreGraphe(bool display=true, bool useArray = false, std::string nom = "");

	void debugNoeudNonPlace(bool display=true, std::string nom = "");

	void debugOldCroisement(bool display=true, std::string nom = "");

	void debugEverything(bool displayOther=false, bool displaySelf=false);

	// Fait une estimation du temps requis pour effectuer un recuit simule complet
	void tempsCalculRecuitSimule(double cool = 0.99999, double t = 100, int mode = 0);

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
	int selectionEmplacement(int modeEmplacement, int nodeId, int t, std::vector<double> = {}, int iter=-1);

	// Lance l'algorithme de recuit simulé sur le graphe pour minimiser le nombre d'intersection
	// Met à jour la variable nombreCroisement du graphe.
	// delay est le nombre de tour auquel on reste à la même température, -1 pour le rendre dynamique en fonction de la taille du graphe.
	// modeNoeud et modeEMplacement sont le mode de sélection de noeud et d'emplacement, 0=Aléatoire, 1=TournoiBinaire, 2=TournoiMultiple
	void recuitSimule(double &timeBest, double cool = 0.99999, double t = 100.0, double seuil = 0.0001, int delay = 1, int modeNoeud = 0, int modeEmplacement = 0);

	void recuitSimuleCustom(double &timeBest, double cool = 0.99999, double t= 100.0, double seuil = 0.0001, int delay=1, int modeNoeud=0, int modeEmplacement=0, std::vector<double> customParam={}) ;

	// Lance l'algorithme de recuit simulé sur le graphe pour minimiser le nombre d'intersection
	// Met à jour la variable nombreCroisement du graphe si elle etait a jour avant.
	void stepRecuitSimule(double& t, int& nbCrois, double cool = 0.99999, int modeNoeud = 0, int modeEmplacement = 0);

	// Applique le recuit simulé plusieurs fois
	// Met a jour le nombre de croisement du graphe.
	void rerecuitSimule(double &timeBest, int iter = -1, double cool = 0.99999, double coolt = 0.99, double t = 100.0, double seuil = 0.0001, int delay = 1, int modeNoeud = 0, int modeEmplacement = 0);

	void rerecuitSimuleCustom(double &timeBest, int iter=-1, double cool=0.99999, double coolt=0.99, double t=100.0, double seuil = 0.0001, int delay=1, int modeNoeud=0, int modeEmplacement=0,std::vector<double> customParam={});

	// Lance l'algorithme de recuit simulé sur le graphe pour minimiser le nombre d'intersection
	// Met à jour le score du graphe et des noeuds
	// delay est le nombre de tour auquel on reste à la même température, -1 pour le rendre dynamique en fonction de la taille du graphe.
	// modeNoeud et modeEMplacement sont le mode de sélection de noeud et d'emplacement, 0=Aléatoire, 1=TournoiBinaire, 2=TournoiMultiple
	void recuitSimuleScore(double &timeBest, double cool = 0.99999, double t = 100.0, double seuil = 0.0001, int delay = 1, int modeNoeud = 0, int modeEmplacement = 0);

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

	// Vide les vecteurs du graphe et effectue une copie du contenu des vecteur du graphe en parametre
	void copyFromGraphe(Graphe& graphe);

	// Calcule le score du noeud en fonction de ses vecteur d'intersections
	long getScoreCroisementNodeFromArrays(int nodeIndex);

	// Calcule le score de l'enfant en fonction des aretes passées dans areteVec
	long getNodeScoreEnfant(Graphe& G, std::vector<int>& areteVec, int nodeIndex);

	// Calcule le score du noeud en parametre.
	long getScoreCroisementNode(int nodeIndex);

	// Calcule le score du noeud nodeIndex sans ajouter le score produit par le noeud swapIndex.
	long getScoreCroisementNode(int nodeIndex, int swapIndex);

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

};

#endif