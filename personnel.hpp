#ifndef PERSO_HPP
#define PERSO_HPP
#include <string>

#define OGDF_INSTALLED
#define LINUX_OS

#define DEBUG // Regarde s'il y a un probleme dans la structure du graphe
#define DEBUG_GRAPHE // Affiche uniquement les informations importantes
//#define DEBUG_GRAPHE_PROGRESS // Affiche des informations pendant les itérations
//#define DEBUG_GRAPHE_RECUIT_PROGRESS // Affiche le score au debut et fin de chaque vague de recuit

extern std::string chemin;
extern std::string cheminGitHub;
extern std::string machine;
extern std::string cheminOutputGraphs;

#endif