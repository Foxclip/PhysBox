#pragma once

#include "utils.h"
#include "settings.h"
#include "ltexture.h"

class Object {

public:
	Object();
	virtual void move(double delta);
	void render();
	void calculateVerticalGravity();

protected:
	double x, y;
	double speedX, speedY;
	double damping;
	Color color;
	LTexture texture;
	virtual void renderToTexture() = 0;

};

class Ball: public Object {
public:
	Ball(double x, double y, double radius, double speedX, double speedY, Color color);
	void move(double delta);
	void renderToTexture();

private:
	double radius;

};