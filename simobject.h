#pragma once

#include "utils.h"
#include <vector>
#include <btBulletDynamicsCommon.h>

enum ObjectType {
	OBJECT_TYPE_BALL,
	OBJECT_TYPE_POLYGON,
	OBJECT_TYPE_TRACK
};

enum CollisionType {
	COLLISION_TYPE_BOUNCE,
	COLLISION_TYPE_MERGE,
	COLLISION_TYPES_NUM
};

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
	double getRotation();
	void setX(double x);
	void setY(double y);
	void setVelX(double velX);
	void setVelY(double velY);
	void setRestitution(double restitution);
	void setFriction(double friction);
	virtual void render() = 0;
	static double distanceBetween(SimObject* object1, SimObject* object2);
	void calculateGravity(SimObject* anotherObject, double delta, double gravityRadialForce);
	void calculateSprings(SimObject* anotherObject, double delta,
		double springMaxDistance, double springDistance, double springDamping, double springForce);
	double getMass();
	void setMass(double mass);
	ObjectType getObjectType();

protected:
	btRigidBody* rigidBody;
	sf::Color color;
	ObjectType objectType;

};

class Ball: public SimObject {
public:
	Ball(double x, double y, double radius, double speedX, double speedY, sf::Color color, bool isActive = true);
	void render();
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
	void render();

};

class Polygon: public SimObject {
public:
	Polygon(double x, double y, double speedX, double speedY, std::vector<Point> points, sf::Color color, bool isActive = true);
	void render();

private:
	sf::ConvexShape* renderShape;

};

class Track : public SimObject {
public:
	Track(int pointCount, double distanceBetweenPoints, double thickness, double bottomLimit, double topLimit);
	void render();

private:
	std::vector<sf::ConvexShape> renderSegments;

};