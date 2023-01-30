#ifndef GEOMETRIE_HPP
#define GEOMETRIE_HPP
#include <algorithm>

//renvoie 1,2,3 ou 4 si lpoint t est 1: en haut à droite, 2: en haut à gauche, 3: en bas à gauche, 4: en bas à droite du point s
//on considère s != t
int quadrant(int sx, int sy, int tx, int ty) {
	if (tx > sx) {
		if (ty >= sy) {
			return 1;
		}
		return 4;
	}
	if (ty > sy) {
		return 2;
	}
	return 3;
}

//s= source, t=target, c=comp=nouveau noeud à ajouter, renvoie vrai si c est à gauche de (s;t) faux sinon
bool aGauche(int sx, int sy, int tx, int ty, int cx, int cy) {
	return ((static_cast<long long>(tx) - sx) * (static_cast<long long>(cy) - sy) -
		(static_cast<long long>(ty) - sy) * (static_cast<long long>(cx) - sx)) > 0;
}
//si c est à gauche de (s;t) renvoie 1, si c est à droite de (s;t) renvoie -1, si c,s et t sont alignés renvoie 0
int aGaucheInt(int sx, int sy, int tx, int ty, int cx, int cy) {
	long long det = ((static_cast<long long>(tx) - sx) * (static_cast<long long>(cy) - sy) -
		(static_cast<long long>(ty) - sy) * (static_cast<long long>(cx) - sx));
	if (det > 0) return 1;
	if (det < 0) return -1;
	return 0;
}

// Renvoie vrai si c est dans le rectangle formé par st
bool dansRectangle(int sx,int sy,int tx,int ty,int cx,int cy) {
	return (cx <= std::max(sx, tx) && cx >= std::min(sx, tx) && cy <= std::max(sy, ty) && cy >= std::min(sy, ty));
}

bool dansRectangle(const Point& corner1, const Point& corner2, const Point& point)
{
	return dansRectangle(corner1.getX(), corner1.getY(), 
		corner2.getX(), corner2.getY(), point.getX(), point.getY());
}

bool dansRectangle(const Noeud &noeud1, const Noeud &noeud2, const Noeud &noeud)
{
	return dansRectangle(noeud1.getX(), noeud1.getY(),
		noeud2.getX(), noeud2.getY(), noeud.getX(), noeud.getY());
}

bool dansRectangle(const Aretes &aretes, const Noeud &noeud)
{
	return dansRectangle(aretes.getNoeud1()->getPosition(), 
		aretes.getNoeud2()->getPosition(), noeud.getPosition());
}

// Renvoie vrai si c est sur le segment st
bool surSegment(int sx, int sy, int tx, int ty, int cx, int cy) {
	return (dansRectangle(sx, sy, tx, ty, cx, cy) && (aGaucheInt(sx, sy, tx, ty, cx, cy) == 0));
}

bool surSegment(const Point& s, const Point& t, const Point& c)
{
	return surSegment(s.getX(), s.getY(), t.getX(), t.getY(), c.getX(), c.getY());
}

bool surSegment(const Aretes& lien, const Noeud& noeud)
{
	return surSegment(lien.getNoeud1()->getPosition(),
		lien.getNoeud2()->getPosition(), noeud.getPosition());
}


#endif