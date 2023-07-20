#include "intersection.hpp"

using std::min;
using std::max;

double Depsilon = 0.0001;

bool xorBool(bool x, bool y) {
	return !x ^ !y;
}

double area2Reel(double ax, double ay, double bx, double by, double cx, double cy) {
	return (bx-ax)*(cy-ay)-(cx-ax)*(by-ay);
}

bool leftReel(double ax, double ay, double bx, double by, double cx, double cy) {
	return area2Reel(ax,ay,bx,by,cx,cy) > 0;
}

bool collinearReel(double ax,double ay,double bx, double by, double cx, double cy) {
	return area2Reel(ax,ay,bx,by,cx,cy) == 0;
}

bool intersectPropReel(double ax, double ay, double bx, double by, double cx, double cy,double dx,double dy) {
	if (collinearReel(ax,ay,bx,by,cx,cy)||collinearReel(ax,ay,bx,by,dx,dy)||collinearReel(cx,cy,dx,dy,ax,ay)||collinearReel(cx,cy,dx,dy,bx,by))
		return false;
	return xorBool(leftReel(ax,ay,bx,by,cx,cy),leftReel(ax,ay,bx,by,dx,dy))&&xorBool(leftReel(cx,cy,dx,dy,ax,ay),leftReel(cx,cy,dx,dy,bx,by));
}

bool betweenReel(double ax,double ay,double bx, double by, double cx, double cy) {
	if (!collinearReel(ax,ay,bx,by,cx,cy))
		return false;
	if (ax != bx)
		return ((ax <= cx)&&(cx <= bx))||((ax >= cx)&&(cx >= bx));
	else
		return ((ay <= cy)&&(cy <= by)||((ay >= cy)&&(cy >= by)));
}

bool seCroisentReel(double ax, double ay,double bx,double by,double cx,double cy,double dx,double dy, bool& isIllegal) {
	if (intersectPropReel(ax,ay,bx,by,cx,cy,dx,dy))
		return true;
	else if (betweenReel(ax,ay,bx,by,cx,cy)||betweenReel(ax,ay,bx,by,dx,dy)||betweenReel(cx,cy,dx,dy,ax,ay)||betweenReel(cx,cy,dx,dy,bx,by)) {
		isIllegal = true;
		return true;
	}
	return false;
}

bool seCroisentReel(const Aretes &aretes1, const Aretes &aretes2, bool& isIllegal) {
	return seCroisentReel(aretes1.getNoeud1()->_xreel,aretes1.getNoeud1()->_yreel, aretes1.getNoeud2()->_xreel,aretes1.getNoeud2()->_yreel, aretes2.getNoeud1()->_xreel,aretes2.getNoeud1()->_yreel, aretes2.getNoeud2()->_xreel,aretes2.getNoeud2()->_yreel,isIllegal);
}

// Renvoie vrai si c est sur le segment ab
bool surSegmentReel(double ax, double ay, double bx, double by, double cx, double cy) {
	if (!collinearReel(ax,ay,bx,by,cx,cy))
		return false;
	if (ax != bx)
		return ((ax <= cx)&&(cx <= bx))||((ax >= cx)&&(cx >= bx));
	else
		return ((ay <= cy)&&(cy <= by)||((ay >= cy)&&(cy >= by)));
}

bool surSegmentReel(const Aretes& lien, const Noeud& noeud) {
	return surSegmentReel(lien.getNoeud1()->_xreel,lien.getNoeud1()->_yreel,lien.getNoeud2()->_xreel,lien.getNoeud2()->_yreel, noeud._xreel,noeud._yreel);
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

//si c est à gauche de (s;t) renvoie 1, si c est à droite de (s;t) renvoie -1, si c,s et t sont alignés renvoie 0
int aGaucheIntReel(double sx, double sy, double tx, double ty, double cx, double cy) {
	double det = ((tx - sx) * (cy - sy) - (ty - sy) * (cx - sx));
	if (det > Depsilon) return 1;
	if (det < -Depsilon) return -1;
	return 0;
}