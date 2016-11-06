#pragma once

#include "utils.h"
#include "ltexture.h"
#include <vector>
#include <btBulletDynamicsCommon.h>

enum ObjectType {
	OBJECT_TYPE_BALL
};

enum CollisionType {
	COLLISION_TYPE_BOUNCE,
	COLLISION_TYPE_MERGE,
	COLLISION_TYPES_NUM
};

static double defaultRestitution;

class SimObject {

public:
	double isActive = true;
	std::vector<SimObject*> springConnections;
	int incomingSpringConnectionsCount = 0;
	bool isMarkedForDeletion = false;
	void addToRigidBodyWorld(btDynamicsWorld* world);
	double getX();
	double getY();
	double getVelX();
	double getVelY();
	void setX(double x);
	void setY(double y);
	void setVelX(double velX);
	void setVelY(double velY);
	void setRestitution(double restitution);
	void render(int offsetX, int offsetY);
	static double distanceBetween(SimObject* object1, SimObject* object2);
	void calculateGravity(SimObject* anotherObject, double delta, double gravityRadialForce);
	void calculateSprings(SimObject* anotherObject, double delta,
		double springMaxDistance, double springDistance, double springDamping, double springForce);
	double getMass();
	void setMass(double mass);
	ObjectType getObjectType();

protected:
	btRigidBody* rigidBody;
	utils::Color color;
	LTexture texture;
	ObjectType objectType;
	virtual void renderToTexture() = 0;

};

class Ball: public SimObject {
public:
	Ball(double x, double y, double radius, double speedX, double speedY, utils::Color color, bool isActive = true);
	void renderToTexture();
	void recalculateRadius();
	static void mergeBalls(Ball* ball1, Ball* ball2, double delta);

private:
	double radius;
	double calculateMass(double rad);
};

class Plane: public SimObject {
public:
	enum PlaneSide {
		POS_LEFT,
		POS_RIGHT,
		POS_TOP,
		POS_BOTTOM
	};
	Plane(PlaneSide side);

private:
	void renderToTexture();

};