#ifndef EMPLACEMENT_HPP
#define EMPLACEMENT_HPP

#include "point.hpp"

class Noeud;

class Emplacement {
public:
	Point _position;
	Noeud* _noeud = nullptr;
	int _id;
	bool _estDisponible = true;

	Emplacement(Point position, int id) {
		_position = position;
		_id = id;
	}

	int getId() const { return _id; }
	Point getPosition() const { return _position; }
	int getX() const { return _position.getX(); }
	int getY() const { return _position.getY(); }
	bool estDisponible() { return _estDisponible; }

	// NE PAS APPELER CETTE FONCTION
	void setNoeud(Noeud* noeud)
	{
		_noeud = noeud;
		_estDisponible = false;
	}
	void removeNoeud()
	{
		_noeud = nullptr;
		_estDisponible = true;
	}

};

#endif