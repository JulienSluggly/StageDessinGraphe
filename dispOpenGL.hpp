#ifndef DISPOPENGL_HPP
#define DISPOPENGL_HPP

#include "personnel.hpp"

#if defined(OPENGL_INSTALLED)

#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <stdio.h>
#include <random>
#include "graphe.hpp"
#include "ogdfFunctions.hpp"

bool useReel = false;
bool singleKeyPress = false;
int keyPressFunctionNum = -1;

double windowWidth = 1750.0, windowHeight=900.0;
int gridWidth, gridHeight, initialGridWidth, initialGridHeight;

bool repeatInfinitely = false;
bool display_genetic = false;
bool isGeneticSetUp = false;
bool show_triangulation = false;
bool show_cells = false;
double currentZoom = 0.0;
bool moving = false;
int selectedNode = 0;
bool show_selected_node = false;
bool show_selected_cell = false;
int selectedCellX = 0, selectedCellY=0;
int maxCellX = 0, maxCellY = 0;
int maxNodeIndex = 0;
bool showIllegal = false;
bool showEdges = true;
bool showNodes = true;
bool stepSetup = false;
bool isCursorDraging = false, dragModeOn = false;
double dragOriginX, dragOriginY;
double stepT;
int stepNbCrois;
std::vector<int> graphCopy;
// Parents pour affichage genetique
Graphe parent1("parent1"), parent2("parent2"), enfant("enfant");
int nbNoeudATraiter, currentGrapheNumber=0;
double initialClicX, initialClicY;
double sensiDrag;
double clicX=0.0, clicY=0.0;
double cursorPosX, cursorPosY;
double initialMargeXDebut = 1, initialMargeXFin = 1, initialMargeYDebut = 1, initialMargeYFin = 1;
double margeXDebut, margeXFin, margeYDebut, margeYFin;
double orthoStartX, orthoStartY, orthoEndX, orthoEndY;
double originalOrthoStartX, originalOrthoStartY, originalOrthoEndX, originalOrthoEndY;
std::vector<std::vector<double>> historiqueOpenGL;

void updateOrtho() {
	orthoStartX = -margeXDebut - (gridWidth * currentZoom);
	orthoEndX = gridWidth + margeXFin +(gridWidth * currentZoom);
	orthoStartY = -margeYDebut - (gridHeight * currentZoom);
	orthoEndY = gridHeight + margeYFin + (gridHeight * currentZoom);
}

void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void window_size_callback(GLFWwindow* window, int width, int height) {
	windowWidth = width; windowHeight = height;
}

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	cursorPosX = xpos;
	cursorPosY = ypos;
}


static void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if ((int)yoffset == -1) { // Dezoom
		currentZoom += 0.01;
	}
	else if ((int)yoffset == 1) { // Zoom
		currentZoom -= 0.01;
	}
	updateOrtho();
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	glfwGetCursorPos(window, &initialClicX, &initialClicY);
	clicY = windowHeight-initialClicY;
	clicX = (orthoEndX-orthoStartX)*(initialClicX/windowWidth)+(orthoStartX-originalOrthoStartX)-1;
	clicY = (orthoEndY-orthoStartY)*(clicY/windowHeight)+(orthoStartY-originalOrthoStartY)-1;
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (dragModeOn) {
			isCursorDraging = true;
			dragOriginX = initialClicX; dragOriginY = initialClicY;
		}
		else {
			if (show_cells) {
				// TO DO IF NEEDED
			}
			else if (show_selected_node) {
				keyPressFunctionNum = 26; singleKeyPress = true;
			}
		}
    }
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		isCursorDraging = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		if (show_selected_node) {
			keyPressFunctionNum = 29; singleKeyPress = true;
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
		dragModeOn = !dragModeOn;
	}
}

// Callback pour OpenGL
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS)
		switch (key) {
			// Fermer l'application
		case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
		case GLFW_KEY_R: // Reset Zoom
			currentZoom = 0.0;
			gridWidth = initialGridWidth;
			gridHeight = initialGridHeight;
			margeXDebut = initialMargeXDebut, margeXFin = initialMargeXFin, margeYDebut = initialMargeYDebut, margeYFin = initialMargeYFin;
			orthoStartX = originalOrthoStartX; orthoEndX = originalOrthoEndX; orthoStartY = originalOrthoStartY; orthoEndY = originalOrthoEndY;
			sensiDrag = 10.0;
			break;
			// GAUCHE,DROITE,HAUT,BAS deplace le point selectionne de 1 case dans la direction de la fleche
		case GLFW_KEY_LEFT:
			if (show_cells) {
				if (selectedCellX > 0) {
					selectedCellX--;
					show_selected_cell = true;
				}
			}
			else {
				if (moving) {
					keyPressFunctionNum = 9; singleKeyPress = true;
				}
				else {
					if (selectedNode > 0) {
						selectedNode--;
						show_selected_node = true;
					}
				}
			}
			break;
		case GLFW_KEY_RIGHT:
			if (show_cells) {
				if (selectedCellX < maxCellX) {
					selectedCellX++;
					show_selected_cell = true;
				}
			}
			else {
				if (moving) {
					keyPressFunctionNum = 10; singleKeyPress = true;
				}
				else {
					if (selectedNode < maxNodeIndex) {
						selectedNode++;
						show_selected_node = true;
					}
				}
			}
			break;
		case GLFW_KEY_DOWN:
			if (show_cells) {
				if (selectedCellY > 0) {
					selectedCellY--;
					show_selected_cell = true;
				}
			}
			break;
		case GLFW_KEY_UP:
			if (show_cells) {
				if (selectedCellY < maxCellY) {
					selectedCellY++;
					show_selected_cell = true;
				}
			}
			break;
		case GLFW_KEY_1:
			keyPressFunctionNum = 5; singleKeyPress = true;
			break;
		case GLFW_KEY_2:
			keyPressFunctionNum = 6; singleKeyPress = true;
			break;
		case GLFW_KEY_3: // Recuit Simule
			keyPressFunctionNum = 3; singleKeyPress = true;
			break;
		case GLFW_KEY_4:
			keyPressFunctionNum = 7; singleKeyPress = true;
			break;
		case GLFW_KEY_5: // Step Stress Majorization
			keyPressFunctionNum = 20; singleKeyPress = true;
			break;
		case GLFW_KEY_8:
			break;
		case GLFW_KEY_9:
			showEdges = !showEdges;
			break;
		case GLFW_KEY_0:
			showNodes = !showNodes;
			break;
		case GLFW_KEY_KP_1: // Copy Graph
			keyPressFunctionNum = 1; singleKeyPress = true;
			break;
		case GLFW_KEY_KP_2: // Load Graph
			keyPressFunctionNum = 2; singleKeyPress = true;
			break;
		case GLFW_KEY_KP_7: // Rotate Graph +5°
			keyPressFunctionNum = 24; singleKeyPress = true;
			break;
		case GLFW_KEY_KP_4: // Rotate Graphe -5°
			keyPressFunctionNum = 25; singleKeyPress = true;
			break;
		case GLFW_KEY_KP_9: // Save Graph
			keyPressFunctionNum = 0; singleKeyPress = true;
			break;
		case GLFW_KEY_F1:
			keyPressFunctionNum = 13; singleKeyPress = true;
			break;
		case GLFW_KEY_F2: // Recuit Low Temp
			keyPressFunctionNum = 28; singleKeyPress = true;
			break;
		case GLFW_KEY_F3:
			keyPressFunctionNum = 12; singleKeyPress = true;
			break;
		case GLFW_KEY_F4:
			keyPressFunctionNum = 14; singleKeyPress = true;
			break;
		case GLFW_KEY_F5:
			keyPressFunctionNum = 4; singleKeyPress = true;
			stepSetup = true;
			break;
		case GLFW_KEY_F6:
			display_genetic = !display_genetic;
			if (display_genetic) {
				keyPressFunctionNum = 15; singleKeyPress = true;
			}
			else {
				gridHeight = initialGridHeight;
				gridWidth = initialGridWidth;
			}
			break;
		case GLFW_KEY_F7:
			keyPressFunctionNum = 16; singleKeyPress = true;
			break;
		case GLFW_KEY_F8:
			keyPressFunctionNum = 19; singleKeyPress = true;
			break;
		case GLFW_KEY_F9: // Print current seed
			keyPressFunctionNum = 21; singleKeyPress = true;
			break;
		case GLFW_KEY_F10:
			break;
		case GLFW_KEY_F11: // ogdfGMMM
			keyPressFunctionNum = 32; singleKeyPress = true;
			break;
		case GLFW_KEY_F12: // Repeat Infinitely
			repeatInfinitely = !repeatInfinitely;
			if (repeatInfinitely) { printf("Mode Repeat: ON\n"); }
			else { printf("Mode Repeat: OFF\n"); }
			break;
		case GLFW_KEY_KP_ADD:
			sensiDrag = sensiDrag / 2;
			tcout() << sensiDrag << std::endl;
			break;
		case GLFW_KEY_KP_SUBTRACT:
			sensiDrag *= 2;
			tcout() << sensiDrag << std::endl;
			break;
		case GLFW_KEY_N:
			keyPressFunctionNum = 8; singleKeyPress = true;
			break;
		case GLFW_KEY_D:
			moving = !moving;
			break;
		case GLFW_KEY_E:
			break;
		case GLFW_KEY_G:
			show_cells = !show_cells;
			break;
		case GLFW_KEY_S:
			keyPressFunctionNum = 11; singleKeyPress = true;
			break;
		case GLFW_KEY_I:
			showIllegal = !showIllegal;
			keyPressFunctionNum = 18; singleKeyPress = true;
			break;
		case GLFW_KEY_T:
			show_triangulation = !show_triangulation;
			keyPressFunctionNum = 17; singleKeyPress = true;
			break;
		case GLFW_KEY_PAGE_UP: // Ajoute 5 au m_edgecost du SM
			keyPressFunctionNum = 22; singleKeyPress = true;
			break;
		case GLFW_KEY_PAGE_DOWN: // Enleve 5 au m_edgecost du SM
			keyPressFunctionNum = 23; singleKeyPress = true;
			break;
		case GLFW_KEY_BACKSPACE: // CTRL + Z
			keyPressFunctionNum = 31; singleKeyPress = true;
			break;
		case GLFW_KEY_KP_DECIMAL:
			dragModeOn = !dragModeOn;
			break;
		}
}

void openGLShowNodesReel(Graphe& G) {
	if (showNodes) {
		if (!display_genetic) {
			glPointSize(7);
			glBegin(GL_POINTS);
			glColor3f(1.0f, 0.0f, 0.0f);
			for (int i = 0; i < G._noeuds.size(); i++) {
				glVertex2d(G._noeuds[i]._xreel, G._noeuds[i]._yreel);
			}
			glEnd();
		}
	}
}

void openGLShowEdgesReel(Graphe& G) {
	if (showEdges) {
		glLineWidth(3.0f);
		if (!display_genetic) {
			glColor3f(1.0f, 1.0f, 1.0f);
			for (int i = 0; i < G._aretes.size(); i++) {
				glBegin(GL_LINE_STRIP);
				glVertex2d(G._aretes[i].getNoeud1()->_xreel, G._aretes[i].getNoeud1()->_yreel);
				glVertex2d(G._aretes[i].getNoeud2()->_xreel, G._aretes[i].getNoeud2()->_yreel);
				glEnd();
			}
			if (showIllegal) {
				for (auto &a : G.areteInter) {
					glColor3f(0.75f, 0.0f, 0.0f);
					glBegin(GL_LINE_STRIP);
					glVertex2d(a->getNoeud1()->_xreel, a->getNoeud1()->_yreel);
					glVertex2d(a->getNoeud2()->_xreel, a->getNoeud2()->_yreel);
					glEnd();
				}
				for (auto &a : G.areteIll) {
					glColor3f(1.0f, 0.0f, 1.0f);
					glBegin(GL_LINE_STRIP);
					glVertex2d(a->getNoeud1()->_xreel, a->getNoeud1()->_yreel);
					glVertex2d(a->getNoeud2()->_xreel, a->getNoeud2()->_yreel);
					glEnd();
				}
				for (auto &a : G.areteIllSelf) {
					glColor3f(0.3f, 0.2f, 0.5f);
					glBegin(GL_LINE_STRIP);
					glVertex2d(a->getNoeud1()->_xreel, a->getNoeud1()->_yreel);
					glVertex2d(a->getNoeud2()->_xreel, a->getNoeud2()->_yreel);
					glEnd();
				}
			}
		}
	}
}

void openGLShowCellsReel(Graphe& G) {
	if (show_cells) {
		glLineWidth(2.0f);
		glColor3f(0.8f, 0.7f, 0.6f);
		for (int i=0;i<G.grille.size();i++) {
			for (int j=0;j<G.grille[i].size();j++) {
				double x1 = G.grille[i][j].getBottomRightXReel();
				double y1 = G.grille[i][j].getBottomRightYReel();
				double x2 = G.grille[i][j].getBottomLeftXReel();
				double y2 = G.grille[i][j].getTopRightYReel();
				glBegin(GL_LINE_STRIP);
				glVertex2d(x1, y1);
				glVertex2d(x2, y1);
				glEnd();
				glBegin(GL_LINE_STRIP);
				glVertex2d(x1, y1);
				glVertex2d(x1, y2);
				glEnd();
				if (j == 0) {
					glBegin(GL_LINE_STRIP);
					glVertex2d(x2, y1);
					glVertex2d(x2, y2);
					glEnd();
				}
				if (i == G.grille.size()-1) {
					glBegin(GL_LINE_STRIP);
					glVertex2d(x2, y2);
					glVertex2d(x1, y2);
					glEnd();
				}
			}
		}
		if (show_selected_cell) {
			double x1 = G.grille[selectedCellY][selectedCellX].getTopLeftXReel();
			double y1 = G.grille[selectedCellY][selectedCellX].getTopLeftYReel();
			double x2 = G.grille[selectedCellY][selectedCellX].getBottomRightXReel();
			double y2 = G.grille[selectedCellY][selectedCellX].getBottomRightYReel();
			glBegin(GL_QUADS);
			glVertex2f(x1, y1);
			glVertex2f(x2, y1);
			glVertex2f(x2, y2);
			glVertex2f(x1, y2);
			glEnd();
		}
	}
}

void openGLShowSelectedReel(Graphe& G) {
	glPointSize(7);
	glBegin(GL_POINTS);
	if (show_selected_node) {
		glColor3f(0.0f, 0.0f, 1.0f);
		if (moving) {
			glColor3f(0.0f, 0.5f, 0.0f);
		}
		glVertex2d(G._noeuds[selectedNode]._xreel, G._noeuds[selectedNode]._yreel);
	}
	glEnd();
}

void openGLShowGrid() {
	// affichage de la grille avec une marge de 1
	glLineWidth(2.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_LINE_STRIP);
	glVertex2d(-1, -1);
	glVertex2d(gridWidth + 1, -1);
	glVertex2d(gridWidth + 1, gridHeight + 1);
	glVertex2d(-1, gridHeight + 1);
	glVertex2d(-1, -1);
	glEnd();
}

void openGLPrintRaccourcis(bool printRaccourcis) {
	if (printRaccourcis) {
		tcout() << "-------------------------------------" << std::endl;
		tcout() << "RACCOURCIS CLAVIER POUR INTERRACTION OPENGL" << std::endl;
		tcout() << "Touche Escape/Echap: Ferme la fenetre OpenGl." << std::endl;
		tcout() << "Touche Gauche/Left: " << std::endl;
		tcout() << "     -Si en mode selection de noeud(par defaut): Selectionne le noeud precedent." << std::endl;
		tcout() << "Touche Droite/Right: Similaire a Gauche/Left mais pour le suivant." << std::endl;
		tcout() << "Touche 1/&: Reinitialise les noeuds du graphe et effectue un placement aleatoire." << std::endl;
		tcout() << "Touche 2/e: Reinitialise les noeuds du graphe et effectue un placement glouton." << std::endl;
		tcout() << "Touche 3/\": Appelle la fonction Recuit Simule sur le graphe." << std::endl;
		tcout() << "Touche 5/( Calcule la moyenne de score de 1000 placements aleatoire et de 1000 placements gloutons." << std::endl;
		tcout() << "Touche NumPad+/KeyPadAdd: Zoom." << std::endl;
		tcout() << "Touche NumPad-/KeyPadMinus: Dezoom." << std::endl;
		tcout() << "Touche N: Affiche le nombre de croisement dans le graphe." << std::endl;
		tcout() << "Touche D: Active/Desactive le mode deplacement." << std::endl;
		tcout() << "Touche I: Active/Desactive l'affichage de croisements(rouge) et croisements illegaux(violet)" << std::endl;
		tcout() << "-------------------------------------" << std::endl;
	}
}

void openGLKeyPressFunction(Graphe& G) {
	if (singleKeyPress) {
		//tcout() << "KeyPress: " << keyPressFunctionNum << std::endl;
		bool recalcIllegal = false;
		switch(keyPressFunctionNum) {
		case 0: {// Save Graphe to JSON
			break;
		}
		case 1: {// Copie le graphe
			break;
		}
		case 2: {// Load copy
			break;
		}
		case 3: {// Recuit simule (KEY: 3")
			if (useReel) {
				tcout() << "Nb Croisement debut recuit: " << G.getNbCroisementReelConst() << std::endl;
				auto start = std::chrono::system_clock::now();
				double timeBest;
				bool useGrille = G.grillePtr.size() > 0;
				G.recuitSimuleReel(timeBest,start,{},0.99999,100.0,0.0001,1,0,2,useGrille,false);
				auto end = std::chrono::system_clock::now();
				std::chrono::duration<double> secondsTotal = end - start;
				tcout() << "Temps calcul: " << secondsTotal.count() << " secondes." << std::endl;
				tcout() << "Temps Meilleur: " << timeBest << " secondes.\n";
				tcout() << "Nb Croisement fin recuit: " << G.getNbCroisementReelConst() << std::endl;
				if (G.grillePtr.size() > 0) { G.reinitGrilleReel(); }
			}
			historiqueOpenGL.clear();
			break;
		}
		case 4: {// Step Recuit Simule
			break;
		}
		case 5: {// Placement aleatoire (KEY: 1&)
			G.placementAleatoireReel();
			if (G.grillePtr.size() > 0 ) { G.reinitGrilleReel(); }
			if (showIllegal) recalcIllegal = true;
			historiqueOpenGL.clear();
			break;
		}
		case 6: {// Placement Glouton Revisite
			break;
		}
		case 7: {// Affiche score (KEY: 4')
			G.getNbCroisementDiffReel();
			tcout() << "Total Inter: " << G.nombreInter + G.nombreInterIll + G.nombreInterIllSelf << " normales: " << G.nombreInter << " illegales: " << G.nombreInterIll << " self: " << G.nombreInterIllSelf << std::endl;
			break;
		}
		case 8: {
			break;
		}
		case 9: {
			break;
		}
		case 10: {
			break;
		}
		case 11: {
			break;
		}
		case 12: {// Debug All Info (KEY: F3)
			if (useReel) {
				G.afficherNoeudsReel();
			}
			break;
		}
		case 13: {// Debug Specific Info
			G.afficherInfo();
			break;
		}
		case 14: {
			break;
		}
		case 15: {
			break;
		}
		case 16: {
			break;
		}
		case 17: {
			break;
		}
		case 18: {// Recalc Illegal (KEY: I)
			recalcIllegal = true;
			break;
		}
		case 19: {// Affiche le contenu de la cellule (KEY:F8)
			//G.afficherLiens();
			if (show_selected_node) {
				tcout() << "Selected Node: " << selectedNode;
				if (selectedNode >= 0) {
					if (G.useCoordReel) {
						tcout() << " Coord: X: " << G._noeuds[selectedNode]._xreel << " Y: " << G._noeuds[selectedNode]._yreel << std::endl;
					}
				}
			}
			if (show_cells && show_selected_cell) {
				tcout() << "Selected Cellule: " << selectedCellY << " " << selectedCellX << " id: " << selectedCellY * G.grille[0].size() + selectedCellX << std::endl;
				for (const int& id : G.grille[selectedCellY][selectedCellX].vecAreteId) {
					tcout() << "	Arete: " << id << " N1: " << G._aretes[id].getNoeud1()->_id << " N2: " << G._aretes[id].getNoeud2()->_id << std::endl;
				}
			}
			break;
		}
		case 20: {
			break;
		}
		case 21: {// Print current seed
			tcout() << "Current Seed: "; 
			if (!isSeedFixe()) { tcout() << "R-"; }
			else { tcout() << "NR-"; }
			tcout() << getSeed(0) << "\n";
			break;
		}
		case 22: {
			break;
		}
		case 23: {
			break;
		}
		case 24: {
			break;
		}
		case 25: {
			break;
		}
		case 26: {//Selection node (clic gauche avec modeNode)
			if (useReel) { 
				selectedNode = G.getClosestNodeFromPointReel(clicX,clicY); 
				tcout() << "Selected Node Id: " << G._noeuds[selectedNode]._id << " X: " << G._noeuds[selectedNode]._xreel << " Y: " << G._noeuds[selectedNode]._yreel << " Degre: " << G._noeuds[selectedNode].voisins.size() << " " << G._noeuds[selectedNode].voisinString() << std::endl;
			}
			break;
		}
		case 27: {
			break;
		}
		case 28: {// Recuit simule a basse temperature (KEY: F2)
			if (useReel) {
				tcout() << "Nb Croisement debut recuit: " << G.getNbCroisementReelConst() << std::endl;
				auto start = std::chrono::system_clock::now();
				double timeBest;
				bool useGrille = G.grillePtr.size() > 0;
				G.recuitSimuleReel(timeBest,start,{},0.99999,0.01,0.0001,1,0,2,useGrille,false);
				auto end = std::chrono::system_clock::now();
				std::chrono::duration<double> secondsTotal = end - start;
				tcout() << "Temps calcul: " << secondsTotal.count() << " secondes." << std::endl;
				tcout() << "Temps Meilleur: " << timeBest << " secondes.\n";
				tcout() << "Nb Croisement fin recuit: " << G.getNbCroisementReelConst() << std::endl;
				if (G.grillePtr.size() > 0) { G.reinitGrilleReel(); }
			}
			if (showIllegal) recalcIllegal = true;
			break;
		}
		case 29: {//Force Selected Emplacement, (UseReel) Change selected node coord (Right Click)
			historiqueOpenGL.push_back({0.0,(double)selectedNode,G._noeuds[selectedNode]._xreel,G._noeuds[selectedNode]._yreel});
			G._noeuds[selectedNode]._xreel = clicX;
			G._noeuds[selectedNode]._yreel = clicY;
			if (showIllegal) recalcIllegal = true;
			break;
		}
		case 30: {
			break;
		}
		case 31: {//CTRL + Z (KEY: Backspace)
			int lastIndex = historiqueOpenGL.size()-1;
			if (lastIndex >= 0) {
				if ((int)historiqueOpenGL[lastIndex][0] == 0) {
					int nodeId = (int)historiqueOpenGL[lastIndex][1];
					G._noeuds[nodeId]._xreel = historiqueOpenGL[lastIndex][2];
					G._noeuds[nodeId]._yreel = historiqueOpenGL[lastIndex][3];
					historiqueOpenGL.pop_back();
					recalcIllegal = true;
				}
			}
			break;
		}
		case 32: {//ogdfFMMM (KEY: F11)
#if defined(OGDF_INSTALLED)
			ogdfFastMultipoleMultilevelEmbedderReel(G); G.translateGrapheToOriginReel(-1);
			if (G.grillePtr.size() > 0 ) { G.reinitGrilleReel(); }
			historiqueOpenGL.clear();
#endif
			break;
		}
		default:{
			tcout() << "No function found.\n";
		}
		}
		if (recalcIllegal) {
			G.clearSetAreteInter();
			if (useReel) { G.recalculateIllegalIntersectionsReel(); }
		}
		if (!repeatInfinitely) {
			singleKeyPress = false;
			keyPressFunctionNum = -1;
		}
	}
	if (isCursorDraging) {
		double diffX = (cursorPosX - dragOriginX)/(sensiDrag);
		double diffY = (cursorPosY - dragOriginY)/(sensiDrag);
		margeXDebut += diffX;
		margeXFin -= diffX;
		margeYDebut -= diffY;
		margeYFin += diffY;
		dragOriginX = cursorPosX;
		dragOriginY = cursorPosY;
		updateOrtho();
	}
}

void openGLShowEverything(Graphe& G) {
	openGLShowGrid();
	openGLShowCellsReel(G);
	openGLShowEdgesReel(G);
	openGLShowNodesReel(G);
	openGLShowSelectedReel(G);
}

void openGLDisplay() {
	glOrtho(orthoStartX,orthoEndX,orthoStartY,orthoEndY,1.f,-1.f);
}

void openGLInitGlobalVariables(Graphe& G,bool useReelCoord) {
	margeXDebut = initialMargeXDebut, margeXFin = initialMargeXFin, margeYDebut = initialMargeYDebut, margeYFin = initialMargeYFin;
	useReel = useReelCoord;
	maxNodeIndex = G._noeuds.size() - 1;
	maxCellY = G.grille.size()-1;
	if (maxCellY > 0) {
		maxCellX = G.grille[0].size()-1;
	}
	updateOrtho();
	originalOrthoStartX = orthoStartX; originalOrthoStartY = orthoStartY; originalOrthoEndX = orthoEndX; originalOrthoEndY = orthoEndY;
	sensiDrag = 10.0/(gridWidth/280.0);
}

void dispOpenGL(Graphe& G, int w, int h, bool useReelCoord=false) {
	gridWidth = w; gridHeight = h; initialGridWidth = gridWidth; initialGridHeight = gridHeight;

	openGLInitGlobalVariables(G,useReelCoord);
	openGLPrintRaccourcis(false);

	if (!glfwInit()) { exit(EXIT_FAILURE); }
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Fenetre OpenGL", NULL, NULL);
	if (!window) { glfwTerminate(); exit(EXIT_FAILURE); }
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glfwSetErrorCallback(error_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window,mouse_scroll_callback);
	glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwMakeContextCurrent(window);
	int width, height;
	while (!glfwWindowShouldClose(window)) {
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);
		glLoadIdentity();

		openGLDisplay();

		openGLKeyPressFunction(G);

		openGLShowEverything(G);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
}

#else
	class Graphe;
	void dispOpenGL(Graphe& G, int w, int h, int mx, int my) {
		tcout() << "OPENGL NOT INSTALLED.\n";
		return;
	}
#endif

#endif