#include "simobject.h"
#include "globals.h"
#include <algorithm>

SimObject::SimObject() {
	x = 0;
	y = 0;
	speedX = 0;
	speedY = 0;
	damping = DAMPING;
}

void SimObject::move(double delta) {
	x += speedX * delta;
	y += speedY * delta;
}

void SimObject::render() {
	texture.render((int)x - texture.getWidth()/2, (int)y - texture.getHeight()/2);
}

void SimObject::calculateBackgroudFriction(double delta) {
	double speed = sqrt(pow(speedX, 2) + pow(speedY, 2));
	if(speed == 0)
		return;
	double forceX = -speedX / speed * backgroundFrictionForce;
	double forceY = -speedY / speed * backgroundFrictionForce;
	double oldSpeedX = speedX;
	double oldSpeedY = speedY;
	speedX += forceX / getMass() * delta;
	speedY += forceY / getMass() * delta;
	if(speedX * oldSpeedX < 0)
		speedX = 0;
	if(speedY * oldSpeedY < 0)
		speedY = 0;
}

void SimObject::calculateVerticalGravity(double delta) {
	speedY += gravityVerticalForce * delta;
}

void SimObject::calculateGravity(SimObject* anotherObject, double delta) {
	double distance = utils::distance(x, anotherObject->x, y, anotherObject->y);
	if(distance == 0)
		return;
	double force = gravityRadialForce * getMass() * anotherObject->getMass() / pow(distance, 2);
	double forceX = (anotherObject->x - x) / distance * force;
	double forceY = (anotherObject->y - y) / distance * force;
	speedX += forceX / getMass() * delta;
	speedY += forceY / getMass() * delta;
}

void SimObject::calculateSprings(SimObject* anotherObject, double delta) {
	double distance = utils::distance(x, anotherObject->x, y, anotherObject->y);
	if(distance == 0) return;
	if(distance > springMaxDistance) {
		springConnections.erase(std::remove(springConnections.begin(), springConnections.end(), anotherObject), springConnections.end());
		anotherObject->incomingSpringConnections--;
		return;
	}
	double offset = distance - springDistance;
	double relativeSpeedX = anotherObject->speedX - speedX;
	double relativeSpeedY = anotherObject->speedY - speedY;
	double relativeSpeed = sqrt(pow(relativeSpeedX, 2) + pow(relativeSpeedY, 2));
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
	double forceX = (anotherObject->x - x) / distance * force + dampingForceX;
	double forceY = (anotherObject->y - y) / distance * force + dampingForceY;
	speedX += forceX / getMass() * delta;
	speedY += forceY / getMass() * delta;
}

double SimObject::getMass() {
	return mass;
}

ObjectType SimObject::getObjectType() {
	return objectType;
}

Ball::Ball(double x, double y, double radius, double speedX, double speedY, utils::Color color) {
	
	this->x = x;
	this->y = y;
	this->radius = radius;
	this->speedX = speedX;
	this->speedY = speedY;
	this->color = color;
	objectType = OBJECT_TYPE_BALL;

	texture.createBlank(((int)radius)*2, ((int)radius)*2, SDL_TEXTUREACCESS_TARGET);
	renderToTexture();
	recalculateMass();

}

void Ball::move(double delta) {

	SimObject::move(delta);

	if(x < radius) {
		x = radius;
		speedX = -speedX * damping;
	}
	if(x > mainWindow.getWidth() - radius) {
		x = mainWindow.getWidth() - radius;
		speedX = -speedX * damping;
	}
	if(y < radius) {
		y = radius;
		speedY = -speedY * damping;
	}
	if(y > mainWindow.getHeight() - radius) {
		y = mainWindow.getHeight() - radius;
		speedY = -speedY * damping;
	}

}

void Ball::renderToTexture() {
	texture.setAsRenderTarget();
	SDL_SetTextureBlendMode(texture.getSDLTexture(), SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(mainWindow.getRenderer(), 0, 0, 0, 0);
	SDL_RenderClear(mainWindow.getRenderer());
	SDL_SetRenderDrawColor(mainWindow.getRenderer(), color.red, color.green, color.blue, 255);
	for(int yCoord = -(int)radius; yCoord <= (int)radius; yCoord++)
		for(int xCoord = -(int)radius; xCoord <= (int)radius; xCoord++)
			if(((xCoord+0.5)*(xCoord+0.5) + (yCoord+0.5)*(yCoord+0.5)) <= radius*radius)
				SDL_RenderDrawPoint(mainWindow.getRenderer(), (int)(xCoord + radius), (int)(yCoord + radius));
	SDL_SetRenderTarget(mainWindow.getRenderer(), NULL);
}

void Ball::recalculateMass() {
	mass = M_PI * pow(radius, 2);
}

double object_utils::partiallyElasticCollision(double v1, double v2, double m1, double m2, double restitution) {
	return (restitution*m2*(v2-v1)+m1*v1+m2*v2)/(m1+m2);
}

void object_utils::collide(SimObject* object1, SimObject* object2, double delta) {
	if(object1->getObjectType() == OBJECT_TYPE_BALL && object2->getObjectType() == OBJECT_TYPE_BALL)
		collideBalls((Ball*)object1, (Ball*)object2, delta);
}

void object_utils::collideBalls(Ball* b1, Ball* b2, double delta) {
	checkAndFixOverlap(b1, b2);
	if(checkCollision(b1, b2, delta))
		recalculateSpeedsAfterCollision(b1, b2);
}

bool object_utils::checkCollision(Ball* b1, Ball* b2, double delta) {
	double collisionDistanceSquared = pow(b1->radius + b2->radius, 2);
	double distX = b2->x - b1->x;
	double distY = b2->y - b1->y;
	double relativeSpeedX = (b2->speedX - b1->speedX)*delta;
	double relativeSpeedY = (b2->speedY - b1->speedY)*delta;
	double distanceSquared = pow(distX, 2) + pow(distY, 2);
	double distToCollisionSquared = distanceSquared - collisionDistanceSquared;
	double relativeSpeedSquared = pow(relativeSpeedX, 2) + pow(relativeSpeedY, 2);
	double d = 2*relativeSpeedX*distX + 2*relativeSpeedY*distY;
	double determinant = d*d - 4*distToCollisionSquared*relativeSpeedSquared;
	if(determinant <= 0) return false;
	double e = sqrt(determinant);
	double f = 2*relativeSpeedSquared;
	double t1 = -(-e + d)/f;
	double t2 = -(e + d)/f;
	double t = std::min(t1, t2);
	if(t < 0) return false;
	if(t > 1) return false;
	b1->x += t*b1->speedX*delta;
	b1->y += t*b1->speedY*delta;
	b2->x += t*b2->speedX*delta;
	b2->y += t*b2->speedY*delta;
	return true;
}

void object_utils::recalculateSpeedsAfterCollision(Ball* b1, Ball* b2) {
	double b1Speed = sqrt(pow(b1->speedX, 2) + pow(b1->speedY, 2));
	double b2Speed = sqrt(pow(b2->speedX, 2) + pow(b2->speedY, 2));
	double collisionAngle = atan2(b2->x - b1->x, b1->y - b2->y) - M_PI/2;
	double b1SpeedAngle = atan2(b1->speedX, -b1->speedY) - M_PI/2;
	double b2SpeedAngle = atan2(b2->speedX, -b2->speedY) + M_PI/2;
	double b1SpeedXRot = b1Speed * cos(b1SpeedAngle - collisionAngle);
	double b1SpeedYRot = b1Speed * sin(b1SpeedAngle - collisionAngle);
	double b2SpeedXRot = -b2Speed * cos(b2SpeedAngle - collisionAngle);
	double b2SpeedYRot = -b2Speed * sin(b2SpeedAngle - collisionAngle);
	double b1NewSpeedXRot = partiallyElasticCollision(b1SpeedXRot,
		b2SpeedXRot, b1->getMass(), b2->getMass(), b1->damping*b2->damping);
	double b2NewSpeedXRot = partiallyElasticCollision(b2SpeedXRot,
		b1SpeedXRot, b2->getMass(), b1->getMass(), b1->damping*b2->damping);
	b1->speedX = b1NewSpeedXRot * cos(collisionAngle) + b1SpeedYRot * cos(collisionAngle + M_PI/2);
	b1->speedY = b1NewSpeedXRot * sin(collisionAngle) + b1SpeedYRot * sin(collisionAngle + M_PI/2);
	b2->speedX = b2NewSpeedXRot * cos(collisionAngle) + b2SpeedYRot * cos(collisionAngle + M_PI/2);
	b2->speedY = b2NewSpeedXRot * sin(collisionAngle) + b2SpeedYRot * sin(collisionAngle + M_PI/2);
}

void object_utils::checkAndFixOverlap(Ball* b1, Ball* b2) {
	double distance = utils::distance(b1->x, b2->x, b1->y, b2->y);
	if(distance == 0) return;
	double overlap = b1->radius + b2->radius - distance;
	if(overlap <= 0) return;
	double k = overlap / distance;
	b1->x += (b1->x - b2->x)*k/2;
	b1->y += (b1->y - b2->y)*k/2;
	b2->x += (b2->x - b1->x)*k/2;
	b2->y += (b2->y - b1->y)*k/2;
}
