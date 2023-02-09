#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include "point.hpp"
#include "aretes.hpp"

//renvoie vrai si les segments [p,q] et [r,s] se croisent
bool seCroisent(int px, int py, int qx, int qy, int rx, int ry, int sx, int sy);

bool seCroisent(const Point &p, const Point &q, const Point &r, const Point &s);

bool seCroisent(const Aretes &aretes1, const Aretes &aretes2);

//Pas sur du nom
double calculNormalisation(const Point &p1, const Point &p2);

bool sontAlignes(const Point& p1, const Point& p2, const Point& p3);

#endif