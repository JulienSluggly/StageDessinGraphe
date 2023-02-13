#include "intersection.hpp"
#include "geometrie.hpp"

using std::min;
using std::max;

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

bool seCroisent(int ax, int ay,int bx,int by,int cx,int cy,int dx,int dy) {
	if (intersectProp(ax,ay,bx,by,cx,cy,dx,dy))
		return true;
	else if (between(ax,ay,bx,by,cx,cy)||between(ax,ay,bx,by,dx,dy)||between(cx,cy,dx,dy,ax,ay)||between(cx,cy,dx,dy,bx,by))
		return true;
	return false;
}

/*
//renvoie vrai si les segments [p,q] et [r,s] se croisent
bool seCroisent(int px, int py, int qx, int qy, int rx, int ry, int sx, int sy) {

	//[P;Q] est a l'ouest de [R;S]
	if (max(px, qx) < min(rx, sx)) {
		return false;
	}
	//[P;Q] est a l'est de [R;S]
	if (min(px, qx) > max(rx, sx)) {
		return false;
	}
	//[P;Q] est au sud de [R;S]
	if (max(py, qy) < min(ry, sy)) {
		return false;
	}
	//[P;Q] est au sud de [R;S]
	if (min(py, qy) > max(ry, sy)) {
		return false;
	}

	if (aGaucheInt(px, py, qx, qy, rx, ry) * aGaucheInt(px, py, qx, qy, sx, sy) == 1 || aGaucheInt(rx, ry, sx, sy, px, py) * aGaucheInt(rx, ry, sx, sy, qx, qy) == 1) {
		return false;
	}
	//Il restetrois cas, SOIT
	//R et S sont de cotes opposes par rapport a PQ ET P et Q sont de cotes opposes par rapport a RS
	//SOIT
	//les quatre points sont alignes
	//SOIT
	//il ne reste plus que les cas ou un produit est � 0 et l'autre � -1
	//trois points sont alignes
	//deux points sont de cotes opposes par rapport a un segment
	//les segments se croisent forcement
	return true;
}*/

bool seCroisent(Emplacement *p, Emplacement *q, Emplacement *r, Emplacement *s)
{
	return seCroisent(p->getX(), p->getY(), q->getX(), q->getY(), r->getX(), r->getY(), s->getX(), s->getY());
}

bool seCroisent(const Aretes &aretes1, const Aretes &aretes2)
{
	return seCroisent(aretes1.getNoeud1()->getEmplacement(), aretes1.getNoeud2()->getEmplacement(), aretes2.getNoeud1()->getEmplacement(), aretes2.getNoeud2()->getEmplacement());
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
