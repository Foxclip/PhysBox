#pragma once

#include "utils.h"
#include "settings.h"
#include "ltexture.h"
#include <vector>

enum ObjectType {
	OBJECT_TYPE_BALL
};

class Object {

public:
	double x, y;
	double speedX, speedY;
	double damping;
	std::vector<Object*> springConnections;
	int incomingSpringConnections = 0;
	Object();
	virtual void move(double delta);
	void render();
	void calculateBackgroudFriction(double delta);
	void calculateVerticalGravity(double delta);
	void calculateGravity(Object* anotherObject, double delta);
	void calculateSprings(Object* anotherObject, double delta);
	double getMass();
	ObjectType getObjectType();

protected:
	double mass;
	utils::Color color;
	LTexture texture;
	ObjectType objectType;
	virtual void renderToTexture() = 0;
	virtual void recalculateMass() = 0;

};

class Ball: public Object {
public:
	double radius;
	Ball(double x, double y, double radius, double speedX, double speedY, utils::Color color);
	void move(double delta);
	void renderToTexture();
	void recalculateMass();

};

namespace object_utils {

	double partiallyElasticCollision(double v1, double v2, double m1, double m2, double restitution);
	void collide(Object* object1, Object* object2, double delta);
	void collideBalls(Ball* b1, Ball* b2, double delta);
	bool checkCollision(Ball* b1, Ball* b2, double delta);
	void recalculateSpeedsAfterCollision(Ball* b1, Ball* b2);
	void checkAndFixOverlap(Ball* b1, Ball* b2);

}