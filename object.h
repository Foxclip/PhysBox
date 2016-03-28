#pragma once

#include "utils.h"
#include "settings.h"
#include "ltexture.h"

class Object {

public:
	Object();
	virtual void move(double delta);
	void render();
	void calculateVerticalGravity(double delta);
	void calculateGravity(Object* anotherObject, double delta);
	double getMass();

protected:
	double x, y;
	double speedX, speedY;
	double mass;
	double damping;
	utils::Color color;
	LTexture texture;
	virtual void renderToTexture() = 0;
	virtual void recalculateMass() = 0;

};

class Ball: public Object {
public:
	Ball(double x, double y, double radius, double speedX, double speedY, utils::Color color);
	void move(double delta);
	void renderToTexture();
	void recalculateMass();

private:
	double radius;

};