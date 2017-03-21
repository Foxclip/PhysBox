#include "simobject.h"
#include "globals.h"
#include <algorithm>
#include <iostream>

double defaultRestitution;
double defaultFriction;

void SimObject::addToRigidBodyWorld(btDynamicsWorld* world) {
	rigidBody->setLinearFactor(btVector3(1, 1, 0));
	rigidBody->setAngularFactor(btVector3(0, 0, 1));
	world->addRigidBody(rigidBody);
}

btRigidBody* SimObject::getRigidBody() {
	return rigidBody;
}

double SimObject::getX() {
	btTransform t;
	rigidBody->getMotionState()->getWorldTransform(t);
	return t.getOrigin().getX();
}

double SimObject::getY() {
	btTransform t;
	rigidBody->getMotionState()->getWorldTransform(t);
	return t.getOrigin().getY();
}

double SimObject::getVelX() {
	return rigidBody->getLinearVelocity().x();
}

double SimObject::getVelY() {
	return rigidBody->getLinearVelocity().y();
}

double SimObject::getRotation() {
	btTransform t;
	rigidBody->getMotionState()->getWorldTransform(t);
	btQuaternion quat = t.getRotation();
	return utils::quaternionToEulerZ(quat.x(), quat.y(), quat.z(), quat.w());
}

void SimObject::setX(double x) {
	btTransform t;
	rigidBody->getMotionState()->getWorldTransform(t);
	t.setOrigin(btVector3(x, getY(), 0));
	rigidBody->getMotionState()->setWorldTransform(t);
}

void SimObject::setY(double y) {
	btTransform t;
	rigidBody->getMotionState()->getWorldTransform(t);
	t.setOrigin(btVector3(getX(), y, 0));
	rigidBody->getMotionState()->setWorldTransform(t);
}

void SimObject::setVelX(double velX) {
	rigidBody->setLinearVelocity(btVector3(velX, getVelY(), 0));
}

void SimObject::setVelY(double velY) {
	rigidBody->setLinearVelocity(btVector3(getVelX(), velY, 0));
}

void SimObject::setRestitution(double restitution) {
	rigidBody->setRestitution(restitution);
}

void SimObject::setFriction(double friction) {
	rigidBody->setFriction(friction);
}

double SimObject::distanceBetween(SimObject* object1, SimObject* object2) {
		double deltaX = object1->getX() - object2->getX();
		double deltaY = object1->getY() - object2->getY();
		return sqrt(deltaX*deltaX + deltaY*deltaY);
}

void SimObject::calculateGravity(SimObject* anotherObject, double delta, double gravityRadialForce) {
	double distance = distanceBetween(this, anotherObject);
	if(distance == 0) return;
	double force = gravityRadialForce * getMass() * anotherObject->getMass() / (distance*distance);
	double forceX = (anotherObject->getX() - getX()) / distance * force;
	double forceY = (anotherObject->getY() - getY()) / distance * force;
	double velX = getVelX();
	double velY = getVelY();
	velX += forceX / getMass() * delta;
	velY += forceY / getMass() * delta;
	setVelX(velX);
	setVelY(velY);
}

void SimObject::calculateSprings(SimObject* anotherObject, double delta,
	double springMaxDistance, double springDistance, double springDamping, double springForce) {

	double distance = distanceBetween(this, anotherObject);
	if(distance == 0) return;
	if(springMaxDistance > 0 && distance > springMaxDistance) {
		springConnections.erase(std::remove(springConnections.begin(), springConnections.end(), anotherObject), springConnections.end());
		anotherObject->incomingSpringConnectionsCount--;
		return;
	}
	double offset = distance - springDistance;
	double relativeSpeedX = anotherObject->getVelX() - getVelX();
	double relativeSpeedY = anotherObject->getVelY() - getVelY();
	//TODO remake damping
	double relativeSpeed = sqrt(relativeSpeedX*relativeSpeedX + relativeSpeedY*relativeSpeedY);
	double dampingForce = relativeSpeed * springDamping;
	double dampingForceX, dampingForceY;
	if(relativeSpeed != 0) {
		dampingForceX = relativeSpeedX / relativeSpeed * dampingForce;
		dampingForceY = relativeSpeedY / relativeSpeed * dampingForce;
	} else {
		dampingForceX = 0;
		dampingForceY = 0;
	}
	double force;
	force = offset * springForce - dampingForce;
	double forceX = (anotherObject->getX() - getX()) / distance * force + dampingForceX;
	double forceY = (anotherObject->getY() - getY()) / distance * force + dampingForceY;
	double velX = getVelX();
	double velY = getVelY();
	velX += forceX / getMass() * delta;
	velY += forceY / getMass() * delta;
	setVelX(velX);
	setVelY(velY);
}

double SimObject::getMass() {
	return 1.0 / rigidBody->getInvMass();
}

void SimObject::setMass(double mass) {
	btVector3 inertia;
	rigidBody->getCollisionShape()->calculateLocalInertia(mass, inertia);
	rigidBody->setMassProps(mass, inertia);
}

ObjectType SimObject::getObjectType() {
	return objectType;
}

Ball::Ball(double x, double y, double radius, double speedX, double speedY, sf::Color color, bool isActive) {
	
	btCollisionShape* shape = new btSphereShape(radius);
	btDefaultMotionState* mState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(x, y, 0)));
	double mass;
	if(isActive) {
		mass = 1;
	} else {
		mass = 0;
	}
	btVector3 inertia(0, 0, 0);
	shape->calculateLocalInertia(mass, inertia);
	btRigidBody::btRigidBodyConstructionInfo ci(mass, mState, shape, inertia);
	rigidBody = new btRigidBody(ci);
	rigidBody->setActivationState(DISABLE_DEACTIVATION);
	rigidBody->setRestitution(defaultRestitution);
	rigidBody->setDamping(0, 0);
	rigidBody->setFriction(defaultFriction);

	setVelX(speedX);
	setVelY(speedY);

	this->radius = radius;
	this->color = color;
	this->isActive = isActive;
	objectType = OBJECT_TYPE_BALL;

}

void Ball::render() {
	sf::CircleShape circle(radius);
	circle.setOrigin(radius, radius);
	circle.setFillColor(color);
	circle.setPosition(getX(), getY());
	circle.setRotation(getRotation() * 180 / PI - 90);
	mainWindow.draw(circle);
}

double Ball::calculateMass(double rad) {
	return PI * rad*rad;
}

void Ball::recalculateRadius() {
	radius = sqrt(getMass() / PI);
}

void Ball::mergeBalls(Ball* ball1, Ball* ball2, double delta) {
	if(ball1->isMarkedForDeletion || ball2->isMarkedForDeletion) return;
	if(!((distanceBetween(ball1, ball2) < ball1->radius + ball2->radius))) return;
	Ball* big;
	Ball* small;
	if(ball1->getMass() >= ball2->getMass()) {
		big = ball1;
		small = ball2;
	} else {
		big = ball2;
		small = ball1;
	}
	double massCenterX = (big->getMass() * big->getX() + small->getMass() * small->getX()) / (big->getMass() + small->getMass());
	double massCenterY = (big->getMass() * big->getY() + small->getMass() * small->getY()) / (big->getMass() + small->getMass());
	big->setX(massCenterX);
	big->setY(massCenterY);
	big->setVelX((big->getMass() * big->getVelX() + small->getMass() * small->getVelX()) / (big->getMass() + small->getMass()));
	big->setVelY((big->getMass() * big->getVelY() + small->getMass() * small->getVelY()) / (big->getMass() + small->getMass()));
	big->color = {
		(unsigned char)((big->getMass() * big->color.r + small->getMass() * small->color.r) / (big->getMass() + small->getMass())),
		(unsigned char)((big->getMass() * big->color.g + small->getMass() * small->color.g) / (big->getMass() + small->getMass())),
		(unsigned char)((big->getMass() * big->color.b + small->getMass() * small->color.b) / (big->getMass() + small->getMass()))
	};
	big->setMass(big->getMass() + small->getMass());
	big->recalculateRadius();
	small->isMarkedForDeletion = true;
}

Plane::Plane(PlaneSide side) {
	btVector3 rot;
	btVector3 pos;
	switch(side) {
		case Plane::POS_LEFT:	 rot = btVector3( 1,  0,  0); pos = btVector3(0,					  0,					  0); break;
		case Plane::POS_RIGHT:	 rot = btVector3(-1,  0,  0); pos = btVector3(mainWindow.getSize().x, 0,					  0); break;
		case Plane::POS_TOP:	 rot = btVector3( 0,  1,  0); pos = btVector3(0,					  0,					  0); break;
		case Plane::POS_BOTTOM:	 rot = btVector3( 0, -1,  0); pos = btVector3(0,					  mainWindow.getSize().y, 0); break;
	}
	btCollisionShape* shape = new btStaticPlaneShape(rot, 1);
	btDefaultMotionState* mState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), pos));
	btRigidBody::btRigidBodyConstructionInfo ci(0, mState, shape, btVector3(0, 0, 0));
	rigidBody = new btRigidBody(ci);
	rigidBody->setActivationState(DISABLE_DEACTIVATION);
	rigidBody->setRestitution(defaultRestitution);
	rigidBody->setFriction(defaultFriction);
}

void Plane::render() {
}

Polygon::Polygon() {
}

Polygon::Polygon(double x, double y, double speedX, double speedY, std::vector<Point> points, sf::Color color, bool isActive) {

	btCompoundShape* shape = new btCompoundShape();
	for(int i = 0; i < points.size(); i++) {
		btConvexHullShape* convex = new btConvexHullShape();
		convex->addPoint(btVector3(0, 0, -10));
		convex->addPoint(btVector3(0, 0,  10));
		convex->addPoint(btVector3(points[i].x, points[i].y, -10));
		convex->addPoint(btVector3(points[i].x, points[i].y,  10));
		if(i < points.size() - 1) {
			convex->addPoint(btVector3(points[i + 1].x, points[i + 1].y, -10));
			convex->addPoint(btVector3(points[i + 1].x, points[i + 1].y,  10));
		} else {
			convex->addPoint(btVector3(points[0].x, points[0].y, -10));
			convex->addPoint(btVector3(points[0].x, points[0].y,  10));
		}
		btTransform transform;
		transform.setIdentity();
		shape->addChildShape(transform, convex);
	}

	btDefaultMotionState* mState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(x, y, 0)));
	double mass;
	if(isActive) {
		mass = 1;
	} else {
		mass = 0;
	}
	btVector3 inertia(0, 0, 0);
	shape->calculateLocalInertia(mass, inertia);
	btRigidBody::btRigidBodyConstructionInfo ci(mass, mState, shape, inertia);
	rigidBody = new btRigidBody(ci);
	rigidBody->setActivationState(DISABLE_DEACTIVATION);
	rigidBody->setRestitution(defaultRestitution);
	rigidBody->setDamping(0, 0);
	rigidBody->setFriction(defaultFriction);

	setVelX(speedX);
	setVelY(speedY);

	this->color = color;
	this->isActive = isActive;
	objectType = OBJECT_TYPE_POLYGON;

	renderShape = new sf::ConvexShape();
	renderShape->setPointCount(points.size());
	for(int i = 0; i < points.size(); i++) {
		renderShape->setPoint(i, sf::Vector2f(points[i].x, points[i].y));
	}
	renderShape->setPosition(sf::Vector2f(x, y));
	renderShape->setFillColor(color);

}

void Polygon::render() {
	renderShape->setPosition(getX(), getY());
	renderShape->setRotation(getRotation() * 180 / PI - 90);
	mainWindow.draw(*renderShape);
}

Track::Track(int pointCount, double distanceBetweenPoints, double thickness, double bottomLimit, double topLimit) {

	std::vector<Point> trackPoints;
	for(int i = 0; i < pointCount; i++) {
		trackPoints.push_back({
			i * distanceBetweenPoints,
			utils::randomBetween(bottomLimit, topLimit)
		});
	}

	btCompoundShape* shape = new btCompoundShape();

	for(int i = 1; i < pointCount; i++) {

		btConvexHullShape* convex = new btConvexHullShape();
		convex->addPoint(btVector3(0,						 0,													 -10));
		convex->addPoint(btVector3(0,						 0,													  10));
		convex->addPoint(btVector3(-distanceBetweenPoints,	 trackPoints[i - 1].y - trackPoints[i].y,			 -10));
		convex->addPoint(btVector3(-distanceBetweenPoints,	 trackPoints[i - 1].y - trackPoints[i].y,			  10));
		convex->addPoint(btVector3(-distanceBetweenPoints,	 trackPoints[i - 1].y - trackPoints[i].y + thickness,-10));
		convex->addPoint(btVector3(-distanceBetweenPoints,	 trackPoints[i - 1].y - trackPoints[i].y + thickness, 10));
		convex->addPoint(btVector3(0,						 thickness,											 -10));
		convex->addPoint(btVector3(0,						 thickness,											  10));
		btTransform transform;
		transform.setIdentity();
		transform.setOrigin(btVector3(trackPoints[i].x, trackPoints[i].y, 0));
		shape->addChildShape(transform, convex);

		sf::ConvexShape segment;
		segment.setPointCount(4);
		segment.setPoint(0, sf::Vector2f(0,						 0));
		segment.setPoint(1, sf::Vector2f(-distanceBetweenPoints, trackPoints[i - 1].y - trackPoints[i].y));
		segment.setPoint(2, sf::Vector2f(-distanceBetweenPoints, trackPoints[i - 1].y - trackPoints[i].y + thickness));
		segment.setPoint(3, sf::Vector2f(0,						 thickness));
		segment.setPosition(sf::Vector2f(trackPoints[i].x, trackPoints[i].y));
		segment.setFillColor(utils::randomColor());
		renderSegments.push_back(segment);

	}

	btDefaultMotionState* mState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 1000, 0)));
	double mass = 0;
	btVector3 inertia(0, 0, 0);
	shape->calculateLocalInertia(mass, inertia);
	btRigidBody::btRigidBodyConstructionInfo ci(mass, mState, shape, inertia);
	rigidBody = new btRigidBody(ci);
	rigidBody->setActivationState(DISABLE_DEACTIVATION);
	rigidBody->setRestitution(defaultRestitution);
	rigidBody->setDamping(0, 0);
	rigidBody->setFriction(defaultFriction);

	objectType = OBJECT_TYPE_TRACK;

}

void Track::render() {
	for(sf::ConvexShape segment : renderSegments) {
		segment.setPosition(segment.getPosition().x + getX(), segment.getPosition().y + getY());
		segment.setRotation(getRotation() * 180 / PI - 90);
		mainWindow.draw(segment);
	}
}

PolygonVehicle::PolygonVehicle(double x, double y, double speedX, double speedY, std::vector<Point> points, std::vector<Wheel> wheels, sf::Color color) {

	Polygon::Polygon(x, y, speedX, speedY, points, color, true);
	objectType = OBJECT_TYPE_POLYGONVEHICLE;

}