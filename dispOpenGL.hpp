#ifndef DISPOPENGL_HPP
#define DISPOPENGL_HPP

#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <stdio.h>
#include "jsonIO.hpp"
#include "EdgeMaps.hpp"
#include <random>

bool save_current = false;
bool make_copy = false;
bool apply_copy = false;
bool show_grid_size = true;
bool startRecuit = false;
bool affiche_score = false;
bool test_moyenne = false;
bool randomReset = false;
bool gloutonReset = false;
int currentZoom = 0;
bool showAllEdges = false;
bool show_nb_intersection = false;
bool moving = false;
bool move_current_left = false;
bool move_current_right = false;
int selectedNode = 0;
bool show_selected_emplacement = false;
int selectedEmplacement = 0;
int maxNodeIndex = 0;
int maxEmplacementIndex = 0;
bool make_swap = false;
bool show_noeud_illegal = false;
bool recalc_illegal = false;
bool debugNoeud = false;
bool debugArete = false;
bool debugEmplacement = false;
bool previsionRecuit = false;
bool stepRecuitSimule = false;
bool stepSetup = false;
double stepT;
int stepNbCrois;
std::vector<std::pair<int, int>> graphCopy;

void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

// Callback pour OpenGL
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS)
		switch (key) {
			// Fermer l'application
		case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
		case GLFW_KEY_R:
			show_grid_size = !show_grid_size;
			break;
			// GAUCHE,DROITE,HAUT,BAS deplace le point selectionne de 1 case dans la direction de la fleche
		case GLFW_KEY_LEFT:
			if (show_selected_emplacement) {
				if (selectedEmplacement > 0) {
					selectedEmplacement--;
				}
			}
			else {
				if (moving) {
					move_current_left = true;
				}
				else {
					if (selectedNode > 0) {
						selectedNode--;
					}
				}
			}
			break;
		case GLFW_KEY_RIGHT:
			if (show_selected_emplacement) {
				if (selectedEmplacement < maxEmplacementIndex) {
					selectedEmplacement++;
				}
			}
			else {
				if (moving) {
					move_current_right = true;
				}
				else {
					if (selectedNode < maxNodeIndex) {
						selectedNode++;
					}
				}
			}
			break;
		case GLFW_KEY_DOWN:
			break;
		case GLFW_KEY_UP:
			break;
		case GLFW_KEY_1:
			randomReset = true;
			break;
		case GLFW_KEY_2:
			gloutonReset = true;
			break;
		case GLFW_KEY_3:
			startRecuit = true;
			break;
		case GLFW_KEY_4:
			affiche_score = true;
			break;
		case GLFW_KEY_5:
			test_moyenne = true;
			break;
		case GLFW_KEY_KP_1:
			startRecuit = true;
			break;
		case GLFW_KEY_KP_9:
			save_current = true;
			break;
		case GLFW_KEY_F1:
			debugNoeud = true;
			break;
		case GLFW_KEY_F2:
			debugArete = true;
			break;
		case GLFW_KEY_F3:
			debugEmplacement = true;
			break;
		case GLFW_KEY_F4:
			previsionRecuit = true;
			break;
		case GLFW_KEY_F5:
			stepRecuitSimule = true;
			stepSetup = true;
			break;
		case GLFW_KEY_KP_ADD:
			//if (currentZoom >= 30)
			currentZoom = currentZoom - 30;
			break;
		case GLFW_KEY_KP_SUBTRACT:
			currentZoom = currentZoom + 30;
			break;
		case GLFW_KEY_N:
			show_nb_intersection = true;
			break;
		case GLFW_KEY_D:
			moving = !moving;
			break;
		case GLFW_KEY_E:
			show_selected_emplacement = !show_selected_emplacement;
			break;
		case GLFW_KEY_S:
			if (show_selected_emplacement) {
				make_swap = true;
			}
			break;
		case GLFW_KEY_I:
			show_noeud_illegal = !show_noeud_illegal;
			recalc_illegal = true;
			break;
		}
}

void dispOpenGL(Graphe& G, const int gridWidth, const int gridHeight, int maxX, int maxY) {

	// Chrono pour le temps d'exec, utilise pour le stockage de donnee pour la creation de graphiques, a supprimer lors de vrai tests
	auto start = std::chrono::system_clock::now();
	auto lastWritten = std::chrono::system_clock::now();
	// NB tour pour le stockage de donnee pour les graphiques, a supprimer lors de vrai executions
	unsigned long long totalTurn = 0;
	unsigned long long lastWrittenTurn = 0;
	maxNodeIndex = G._noeuds.size() - 1;
	maxEmplacementIndex = G._emplacementsPossibles.size() - 1;

	bool printRaccourcis = true;
	if (printRaccourcis) {
		std::cout << "-------------------------------------" << std::endl;
		std::cout << "RACCOURCIS CLAVIER POUR INTERRACTION OPENGL" << std::endl;
		std::cout << "Touche Escape/Echap: Ferme la fenetre OpenGl." << std::endl;
		std::cout << "Touche Gauche/Left: " << std::endl;
		std::cout << "     -Si en mode selection de noeud(par defaut): Selectionne le noeud precedent." << std::endl;
		std::cout << "     -Si en mode selection d'emplacement: Selectionne l'emplacement precedent." << std::endl;
		std::cout << "     -Si en mode deplacement: Deplace le noeud selectionne a un emplacement precedent le sien." << std::endl;
		std::cout << "Touche Droite/Right: Similaire a Gauche/Left mais pour le suivant." << std::endl;
		std::cout << "Touche 1/&: Reinitialise les noeuds du graphe et effectue un placement aleatoire." << std::endl;
		std::cout << "Touche 2/e: Reinitialise les noeuds du graphe et effectue un placement glouton." << std::endl;
		std::cout << "Touche 3/\": Appelle la fonction Recuit Simule sur le graphe." << std::endl;
		std::cout << "Touche 4/' Affiche le score du graphe, du noeud selectionne, et le score swap si l'emplacement selectionne n'est pas disponible." << std::endl;
		std::cout << "Touche 5/( Calcule la moyenne de score de 1000 placements aleatoire et de 1000 placements gloutons." << std::endl;
		std::cout << "Touche NumPad+/KeyPadAdd: Zoom." << std::endl;
		std::cout << "Touche NumPad-/KeyPadMinus: Dezoom." << std::endl;
		std::cout << "Touche N: Affiche le nombre de croisement dans le graphe." << std::endl;
		std::cout << "Touche D: Active/Desactive le mode deplacement." << std::endl;
		std::cout << "Touche E: Active/Desactive le mode selection d'emplacement." << std::endl;
		std::cout << "Touche S: Si le mode selection d'emplacement est actif, effectue un swap entre le noeud selectionne et l'emplacement selectionne." << std::endl;
		std::cout << "Touche I: Active/Desactive l'affichage de croisements(rouge) et croisements illegaux(violet)" << std::endl;
		std::cout << "-------------------------------------" << std::endl;
	}

	//fin ogdf
	if (!glfwInit())
		exit(EXIT_FAILURE);
	GLFWwindow* window = glfwCreateWindow(1820, 980, "Fenetre OpenGL", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glfwSetErrorCallback(error_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);
	int width, height;
	glLineWidth(3);
	while (!glfwWindowShouldClose(window)) {
		float ratio;
		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		if (show_grid_size) {
			glOrtho(-1 - currentZoom, static_cast<float>(gridWidth) + 1 + currentZoom, -1 - currentZoom, static_cast<float>(gridHeight) + 1 + currentZoom, 1.f, -1.f);
		}
		else {
			//glOrtho(-1, static_cast<float>(maxX) + 1, -1, static_cast<float>(maxY) + 1, 1.f, -1.f);
			glOrtho(-300, static_cast<float>(maxX) + 60, -300, static_cast<float>(maxY) + 60, 1.f, -1.f);
		}
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		if (save_current) {
			//writeToJson("currentSave.json", G, GL, gridWidth, gridHeight, maxBends);
			std::cout << "Finished saving" << std::endl;
			save_current = false;
		}
		else if (make_copy) {
			graphCopy.clear();
			//graphCopy = copyGraph(vectorNodeBends);
			make_copy = false;
		}
		else if (apply_copy) {
			//applyGraph(graphCopy);
			apply_copy = false;
		}
		else if (startRecuit) {
			std::cout << "Nb Croisement debut recuit: " << G.getNbCroisement() << std::endl;
			auto start = std::chrono::system_clock::now();
			G.recuitSimule();
			auto end = std::chrono::system_clock::now();
			std::chrono::duration<double> secondsTotal = end - start;
			std::cout << "Temps calcul: " << secondsTotal.count() << " secondes." << std::endl;
			std::cout << "Nb Croisement fin recuit: " << G.getNbCroisement() << std::endl;
			startRecuit = false;
		}
		else if (stepRecuitSimule) {
			if (stepSetup) {
				stepT = 100.0;
				stepNbCrois = G.getNbCroisement();
				stepSetup = false;
			}
			G.stepRecuitSimule(stepT, stepNbCrois);
			if (stepT <= 0.0001 || stepNbCrois == 0) {
				stepRecuitSimule = false;
				std::cout << stepT << " " << stepNbCrois << std::endl;
			}
		}
		else if (randomReset) {
			G.clearNodeEmplacement();
			G.placementAleatoire();
			randomReset = false;
		}
		else if (gloutonReset) {
			G.clearNodeEmplacement();
			G.gloutonRevisite();
			std::cout << "Nb Croisement: " << G.getNbCroisement() << std::endl;
			gloutonReset = false;
		}
		else if (affiche_score) {
			std::cout << "Selected Node: " << selectedNode << " Selected Emplacement: " << selectedEmplacement << std::endl;
			std::cout << "Nb Intersection: " << G.getNbCroisement() << std::endl;
			std::cout << "Selected node score: " << G.getScoreCroisementNode(selectedNode) << std::endl;
			if (show_selected_emplacement) {
				if (!G._emplacementsPossibles[selectedEmplacement].estDisponible()) {
					int swapId = G._emplacementsPossibles[selectedEmplacement]._noeud->getId();
					std::cout << "Selected emplacement score: " << G.getScoreCroisementNode(swapId) << std::endl;
					int score = G.getScoreCroisementNode(selectedNode);
					score += G.getScoreCroisementNode(swapId, selectedNode);
					std::cout << "Score before swap: " << score << std::endl;
					G._noeuds[selectedNode].swap(G._noeuds[swapId].getEmplacement());
					score = G.getScoreCroisementNode(selectedNode);
					score += G.getScoreCroisementNode(swapId, selectedNode);
					std::cout << "Score after swap: " << score << std::endl;
					G._noeuds[selectedNode].swap(G._noeuds[swapId].getEmplacement());
				}
			}
			affiche_score = false;
		}
		else if (test_moyenne) {
			int nb_essai = 1000;
			int somme1 = 0;
			int somme2 = 0;
			for (int i = 0; i < nb_essai; i++) {
				G.clearNodeEmplacement();
				G.placementAleatoire();
				somme1 += G.getNbCroisement();
				G.clearNodeEmplacement();
				G.glouton();
				somme2 += G.getNbCroisement();
			}
			std::cout << "Aleatoire: " << somme1 << " Glouton: " << somme2 << std::endl;
			test_moyenne = false;
		}
		else if (show_nb_intersection) {
			std::cout << "Nb Intersections: " << G.getNbCroisement() << std::endl;
			show_nb_intersection = false;
		}
		else if (move_current_left) {
			areteIll.clear();
			int numPos = G._noeuds[selectedNode].getEmplacement()->getId();
			for (int i = numPos - 1; i >= 0; i--) {
				if (G._emplacementsPossibles[i].estDisponible()) {
					G._noeuds[selectedNode].setEmplacement(&G._emplacementsPossibles[i]);
					break;
				}
			}
			move_current_left = false;
		}
		else if (move_current_right) {
			areteIll.clear();
			int numPos = G._noeuds[selectedNode].getEmplacement()->getId();
			for (int i = numPos + 1; i < G._emplacementsPossibles.size(); i++) {
				if (G._emplacementsPossibles[i].estDisponible()) {
					G._noeuds[selectedNode].setEmplacement(&G._emplacementsPossibles[i]);
					break;
				}
			}
			move_current_right = false;
		}
		else if (make_swap) {
			areteIll.clear();
			if (G._noeuds[selectedNode].getEmplacement()->getId() != selectedEmplacement) {
				int oldEmplacement = G._noeuds[selectedNode].getEmplacement()->getId();
				if (G._emplacementsPossibles[selectedEmplacement].estDisponible()) {
					G._noeuds[selectedNode].setEmplacement(&G._emplacementsPossibles[selectedEmplacement]);
				}
				else {
					G._noeuds[selectedNode].swap(&G._emplacementsPossibles[selectedEmplacement]);
				}
				selectedEmplacement = oldEmplacement;
			}
			make_swap = false;
		}
		else if (debugNoeud) {
			G.afficherNoeuds();
			debugNoeud = false;
		}
		else if (debugArete) {
			G.afficherLiens();
			debugArete = false;
		}
		else if (debugEmplacement) {
			G.afficherEmplacement();
			debugEmplacement = false;
		}
		else if (previsionRecuit) {
			G.tempsCalculRecuitSimule();
			previsionRecuit = false;
		}
		// affichage de la grille avec une marge de 1
		glColor3f(0.0f, 1.0f, 0.0f);
		glBegin(GL_LINE_STRIP);
		glVertex2d(-1, -1);
		glVertex2d(gridWidth + 1, -1);
		glVertex2d(gridWidth + 1, gridHeight + 1);
		glVertex2d(-1, gridHeight + 1);
		glVertex2d(-1, -1);
		glEnd();
		//afficher les edge
		//glLineWidth(1.0f);
		glColor3f(1.0f, 1.0f, 1.0f);
		for (int i = 0; i < G._liens.size(); i++) {
			glBegin(GL_LINE_STRIP);
			glVertex2d(G._liens[i].getNoeud1()->getX(), G._liens[i].getNoeud1()->getY());
			glVertex2d(G._liens[i].getNoeud2()->getX(), G._liens[i].getNoeud2()->getY());
			glEnd();
		}
		if (show_noeud_illegal && !recalc_illegal) {
			for (auto a : areteInter) {
				glColor3f(1.0f, 0.0f, 0.0f);
				glBegin(GL_LINE_STRIP);
				glVertex2d(a->getNoeud1()->getX(), a->getNoeud1()->getY());
				glVertex2d(a->getNoeud2()->getX(), a->getNoeud2()->getY());
				glEnd();
			}
			for (auto a : areteIll) {
				glColor3f(1.0f, 0.0f, 1.0f);
				glBegin(GL_LINE_STRIP);
				glVertex2d(a->getNoeud1()->getX(), a->getNoeud1()->getY());
				glVertex2d(a->getNoeud2()->getX(), a->getNoeud2()->getY());
				glEnd();
			}
		}
		//afficher les slots
		glPointSize(7);
		glBegin(GL_POINTS);
		glColor3f(0.0f, 1.0f, 0.0f);
		for (int i = 0; i < G._emplacementsPossibles.size(); i++) {
			glVertex2d(G._emplacementsPossibles[i].getX(), G._emplacementsPossibles[i].getY());
		}
		//afficher les nodes
		for (int i = 0; i < G._noeuds.size(); i++) {
			glColor3f(1.0f, 0.0f, 0.0f);
			if (selectedNode == i) {
				glColor3f(0.0f, 0.0f, 1.0f);
				if (moving) {
					glColor3f(0.0f, 0.5f, 0.0f);
				}
			}
			glVertex2d(G._noeuds[i].getX(), G._noeuds[i].getY());
		}
		if (show_selected_emplacement) {
			glColor3f(0.5f, 0.0f, 0.0f);
			glVertex2d(G._emplacementsPossibles[selectedEmplacement].getX(), G._emplacementsPossibles[selectedEmplacement].getY());
		}
		if (recalc_illegal) {
			glColor3f(0.0f, 0.0f, 0.5f);
			areteIll.clear();
			areteInter.clear();
			for (int i = 0; i < G._noeuds.size(); i++) {
				if (G.getScoreCroisementNode(i) >= 10000) {
					int idEmp = G._noeuds[i].getEmplacement()->getId();
					glVertex2d(G._emplacementsPossibles[idEmp].getX(), G._emplacementsPossibles[idEmp].getY());
				}
			}
			recalc_illegal = false;
		}
		glEnd();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
}

#endif