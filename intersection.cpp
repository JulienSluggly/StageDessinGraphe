#include "intersection.hpp"

using std::min;
using std::max;

//s= source, t=target, c=comp=nouveau noeud à ajouter, renvoie vrai si c est à gauche de (s;t) faux sinon
bool aGauche(int sx, int sy, int tx, int ty, int cx, int cy) {
	return ((static_cast<long long>(tx) - sx) * (static_cast<long long>(cy) - sy) -
		(static_cast<long long>(ty) - sy) * (static_cast<long long>(cx) - sx)) > 0;
}
//si 'c' est à gauche de (s;t) renvoie 1, si 'c' est à droite de (s;t) renvoie -1, si 'c','s' et 't' sont alignés renvoie 0
int aGaucheInt(int sx, int sy, int tx, int ty, int cx, int cy) {
	long long det = ((static_cast<long long>(tx) - sx) * (static_cast<long long>(cy) - sy) -
		(static_cast<long long>(ty) - sy) * (static_cast<long long>(cx) - sx));
	if (det > 0) return 1;
	if (det < 0) return -1;
	return 0;
}

int area2(int ax, int ay, int bx, int by, int cx, int cy) {
	return (bx-ax)*(cy-ay)-(cx-ax)*(by-ay);
}

bool left(int ax, int ay, int bx, int by, int cx, int cy) {
	return area2(ax,ay,bx,by,cx,cy) > 0;
}

bool leftOn(int ax,int ay, int bx, int by, int cx, int cy) {
	return area2(ax,ay,bx,by,cx,cy) >= 0;
}

bool collinear(int ax,int ay,int bx, int by, int cx, int cy) {
	return area2(ax,ay,bx,by,cx,cy) == 0;
}

bool xorBool(bool x, bool y) {
	return !x ^ !y;
}

bool intersectProp(int ax, int ay, int bx, int by, int cx, int cy,int dx,int dy) {
	if (collinear(ax,ay,bx,by,cx,cy)||collinear(ax,ay,bx,by,dx,dy)||collinear(cx,cy,dx,dy,ax,ay)||collinear(cx,cy,dx,dy,bx,by))
		return false;
	return xorBool(left(ax,ay,bx,by,cx,cy),left(ax,ay,bx,by,dx,dy))&&xorBool(left(cx,cy,dx,dy,ax,ay),left(cx,cy,dx,dy,bx,by));
}

bool between(int ax,int ay,int bx, int by, int cx, int cy) {
	if (!collinear(ax,ay,bx,by,cx,cy))
		return false;
	if (ax != bx)
		return ((ax <= cx)&&(cx <= bx))||((ax >= cx)&&(cx >= bx));
	else
		return ((ay <= cy)&&(cy <= by)||((ay >= cy)&&(cy >= by)));
}

bool seCroisent(int ax, int ay,int bx,int by,int cx,int cy,int dx,int dy, bool& isIllegal) {
	if (intersectProp(ax,ay,bx,by,cx,cy,dx,dy))
		return true;
	else if (between(ax,ay,bx,by,cx,cy)||between(ax,ay,bx,by,dx,dy)||between(cx,cy,dx,dy,ax,ay)||between(cx,cy,dx,dy,bx,by)) {
		isIllegal = true;
		return true;
	}
	return false;
}

bool seCroisent(Emplacement *p, Emplacement *q, Emplacement *r, Emplacement *s, bool& isIllegal) {
	return seCroisent(p->getX(), p->getY(), q->getX(), q->getY(), r->getX(), r->getY(), s->getX(), s->getY(),isIllegal);
}

bool seCroisent(const Aretes &aretes1, const Aretes &aretes2, bool& isIllegal) {
	return seCroisent(aretes1.getNoeud1()->getEmplacement(), aretes1.getNoeud2()->getEmplacement(), aretes2.getNoeud1()->getEmplacement(), aretes2.getNoeud2()->getEmplacement(),isIllegal);
}

//Pas sur du nom
double calculNormalisation(Emplacement *p1, Emplacement *p2)
{
	double y1 = p1->getY(), y2 = p2->getY();
	double x1 = p1->getX(), x2 = p2->getX();
	return (y2 - y1) / (x2 - x1);
}

bool sontAlignes(Emplacement* p1, Emplacement* p2, Emplacement* p3)
{
	return calculNormalisation(p1, p3) == calculNormalisation(p1, p2)
		&& calculNormalisation(p2, p3) == calculNormalisation(p1, p2);
	//Seconde ligne n�cessaire ?
}

// Renvoie vrai si c est sur le segment ab
bool surSegment(int ax, int ay, int bx, int by, int cx, int cy) {
	if (!collinear(ax,ay,bx,by,cx,cy))
		return false;
	if (ax != bx)
		return ((ax <= cx)&&(cx <= bx))||((ax >= cx)&&(cx >= bx));
	else
		return ((ay <= cy)&&(cy <= by)||((ay >= cy)&&(cy >= by)));
}

bool surSegment(Emplacement* s, Emplacement* t, Emplacement* c)
{
	return between(s->getX(), s->getY(), t->getX(), t->getY(), c->getX(), c->getY());
}

bool surSegment(const Aretes& lien, const Noeud& noeud)
{
	return surSegment(lien.getNoeud1()->getEmplacement(),
		lien.getNoeud2()->getEmplacement(), noeud.getEmplacement());
}

// compute the intersection point of two edges
bool seCroisentForce(double ax, double ay, double bx, double by, double cx, double cy, double dx, double dy, double& ix, double& iy) {
    double denominator = (bx - ax)*(dy - cy) - (by - ay)*(dx - cx);
    if (denominator == 0) {
        return false;
    }
    double t = ((cy - ay)*(dx - cx) - (cx - ax)*(dy - cy)) / denominator;
    double u = ((by - ay)*(dx - cx) - (bx - ax)*(dy - cy)) / denominator;
    if (t >= 0 && t <= 1 && u >= 0 && u <= 1) {
        ix = ax + t*(bx - ax);
        iy = ay + t*(by - ay);
        return true;
    } else {
        return false;
    }
}