#pragma once

#include "utils.h"

class Object {

public:
	Object();
	virtual void move();
	virtual void render() = 0;
	Color color;

protected:
	double x, y;
	double speedX, speedY;

};

class Ball: public Object {
public:
	Ball(double x, double y, double radius, double speedX, double speedY, Color color);
	void move();
	void render();

private:
	double radius;

};