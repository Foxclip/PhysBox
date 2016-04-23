#pragma once

#include "utils.h"
#include "ltexture.h"
#include <vector>

enum ObjectType {
	OBJECT_TYPE_BALL
};

enum CollisionType {
	COLLISION_TYPE_BOUNCE,
	COLLISION_TYPE_MERGE,
	COLLISION_TYPES_NUM
};

const double DEFAULT_DAMPING = 0.5;

class SimObject {

public:
	double x = 0, y = 0;
	double velX = 0, velY = 0;
	double damping = DEFAULT_DAMPING;
	std::vector<SimObject*> springConnections;
	int incomingSpringConnectionsCount = 0;
	bool isMarkedForDeletion = false;
	virtual void move(double delta);
	void render();
	static void collide(SimObject* object1, SimObject* object2, double delta, CollisionType collisionType);
	void calculateBackgroudFriction(double delta, double backgroundFrictionForce);
	void calculateVerticalGravity(double delta, double gravityVerticalForce);
	void calculateGravity(SimObject* anotherObject, double delta, double gravityRadialForce);
	void calculateSprings(SimObject* anotherObject, double delta,
		double springMaxDistance, double springDistance, double springDamping, double springForce);
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

class Ball: public SimObject {
public:
	Ball(double x, double y, double radius, double speedX, double speedY, utils::Color color);
	void move(double delta);
	void renderToTexture();
	void recalculateMass();
	void recalculateRadius();
	static void mergeBalls(Ball* ball1, Ball* ball2, double delta);
	static double partiallyElasticCollision(double v1, double v2, double m1, double m2, double restitution);
	static void collideBalls(Ball* b1, Ball* b2, double delta, CollisionType collisionType);
	static void ballsBounce(Ball* b1, Ball* b2, double delta);
	static bool checkCollision(Ball* b1, Ball* b2, double delta);
	static void recalculateSpeedsAfterCollision(Ball* b1, Ball* b2);
	static void checkAndFixOverlap(Ball* b1, Ball* b2);

private:
	double radius;

};