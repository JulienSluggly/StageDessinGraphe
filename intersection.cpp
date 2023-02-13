#include "intersection.hpp"
#include "geometrie.hpp"

using std::min;
using std::max;

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

	//R est a gauche, droite ou align� a [P;Q]
	int ag1 = aGaucheInt(px, py, qx, qy, rx, ry);
	//S est a gauche, droite ou align� a [P;Q]
	int ag2 = aGaucheInt(px, py, qx, qy, sx, sy);
	//P est a gauche, droite ou align� a [R;S]
	int ag3 = aGaucheInt(rx, ry, sx, sy, px, py);
	//Q est a gauche, droite ou align� a [R;S]
	int ag4 = aGaucheInt(rx, ry, sx, sy, qx, qy);

	//R et S sont du meme cote par rapport a PQ
	//OU P et Q sont du meme cote par rapport a RS
	if (ag1 * ag2 == 1 || ag3 * ag4 == 1) {
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
}

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
