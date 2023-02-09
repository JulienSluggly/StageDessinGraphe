#ifndef POINT_HPP
#define POINT_HPP

#include <math.h>

class Point {
public:
	int _y;
	int _x;
	Point(int x, int y) {
		_x = x;
		_y = y;
	}
	Point(){}

	int getX() const { return _x; }
	int getY() const { return _y; }

	int distance(Point point) const
	{
		int xValue = _x - point.getX();
		int yValue = _y - point.getY();
		return sqrt(xValue * xValue + yValue * yValue);
	}

	void setX(int x) { _x = x; }
	void setY(int y) { _y = y; }

};

#endif