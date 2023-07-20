#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include "aretes.hpp"


bool seCroisentReel(double px, double py, double qx, double qy, double rx, double ry, double sx, double sy, bool& isIllegal);

bool seCroisentReel(const Aretes &aretes1, const Aretes &aretes2, bool& isIllegal);

bool surSegmentReel(double sx, double sy, double tx, double ty, double cx, double cy);

bool surSegmentReel(const Aretes& lien, const Noeud& noeud);

bool seCroisentForce(double ax, double ay, double bx, double by, double cx, double cy, double dx, double dy, double& ix, double& iy);

//si c est à gauche de (s;t) renvoie 1, si c est à droite de (s;t) renvoie -1, si c,s et t sont alignés renvoie 0
int aGaucheIntReel(double sx, double sy, double tx, double ty, double cx, double cy);

#endif