#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP

#include "emplacement.hpp"
#include "aretes.hpp"

//renvoie vrai si les segments [p,q] et [r,s] se croisent
bool seCroisent(int px, int py, int qx, int qy, int rx, int ry, int sx, int sy);

bool seCroisent(const Emplacement &p, const Emplacement &q, const Emplacement &r, const Emplacement &s);

bool seCroisent(const Aretes &aretes1, const Aretes &aretes2);

//Pas sur du nom
double calculNormalisation(const Emplacement &p1, const Emplacement &p2);

bool sontAlignes(const Emplacement& p1, const Emplacement& p2, const Emplacement& p3);

#endif