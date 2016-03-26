#pragma once

#include "utils.h"
#include "settings.h"

class Object {

public:
	Object();
	virtual void move();
	virtual void render() = 0;
	void calculateVerticalGravity();

protected:
	double x, y;
	double speedX, speedY;
	double damping;
	Color color;

};

class Ball: public Object {
public:
	Ball(double x, double y, double radius, double speedX, double speedY, Color color);
	void move();
	void render();

private:
	double radius;

};