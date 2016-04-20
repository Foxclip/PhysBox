#include "simobject.h"
#include "globals.h"
#include <algorithm>

void SimObject::move(double delta) {
	x += velX * delta;
	y += velY * delta;
}

void SimObject::render() {
	texture.render((int)x - texture.getWidth()/2, (int)y - texture.getHeight()/2);
}

void SimObject::collide(SimObject * object1, SimObject * object2, double delta, CollisionType collisionType) {
		if(object1->getObjectType() == OBJECT_TYPE_BALL && object2->getObjectType() == OBJECT_TYPE_BALL)
			Ball::collideBalls((Ball*)object1, (Ball*)object2, delta, collisionType);
}

void SimObject::calculateBackgroudFriction(double delta, double backgroundFrictionForce) {
	double speed = sqrt(pow(velX, 2) + pow(velY, 2));
	if(speed == 0)
		return;
	double forceX = -velX / speed * backgroundFrictionForce;
	double forceY = -velY / speed * backgroundFrictionForce;
	double oldSpeedX = velX;
	double oldSpeedY = velY;
	velX += forceX / getMass() * delta;
	velY += forceY / getMass() * delta;
	if(velX * oldSpeedX < 0)
		velX = 0;
	if(velY * oldSpeedY < 0)
		velY = 0;
}

void SimObject::calculateVerticalGravity(double delta, double gravityVerticalForce) {
	velY += gravityVerticalForce * delta;
}

void SimObject::calculateGravity(SimObject* anotherObject, double delta, double gravityRadialForce) {
	double distance = utils::distance(x, anotherObject->x, y, anotherObject->y);
	if(distance == 0)
		return;
	double force = gravityRadialForce * getMass() * anotherObject->getMass() / pow(distance, 2);
	double forceX = (anotherObject->x - x) / distance * force;
	double forceY = (anotherObject->y - y) / distance * force;
	velX += forceX / getMass() * delta;
	velY += forceY / getMass() * delta;
}

void SimObject::calculateSprings(SimObject* anotherObject, double delta,
	double springMaxDistance, double springDistance, double springDamping, double springForce) {

	double distance = utils::distance(x, anotherObject->x, y, anotherObject->y);
	if(distance == 0) return;
	if(distance > springMaxDistance) {
		springConnections.erase(std::remove(springConnections.begin(), springConnections.end(), anotherObject), springConnections.end());
		anotherObject->incomingSpringConnectionsCount--;
		return;
	}
	double offset = distance - springDistance;
	double relativeSpeedX = anotherObject->velX - velX;
	double relativeSpeedY = anotherObject->velY - velY;
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
	velX += forceX / getMass() * delta;
	velY += forceY / getMass() * delta;

}

double SimObject::getMass() {
	return mass;
}

ObjectType SimObject::getObjectType() {
	return objectType;
}

Ball::Ball(double x, double y, double radius, double speedX, double speedY, utils::Color color) {
	
	this->x	= x;
	this->y	= y;
	this->radius = radius;
	this->velX = speedX;
	this->velY = speedY;
	this->color = color;
	objectType = OBJECT_TYPE_BALL;

	renderToTexture();
	recalculateMass();

}

void Ball::move(double delta) {

	SimObject::move(delta);

	if(x < radius) {
		x = radius;
		velX = -velX * damping;
	}
	if(x > mainWindow.getWidth() - radius) {
		x = mainWindow.getWidth() - radius;
		velX = -velX * damping;
	}
	if(y < radius) {
		y = radius;
		velY = -velY * damping;
	}
	if(y > mainWindow.getHeight() - radius) {
		y = mainWindow.getHeight() - radius;
		velY = -velY * damping;
	}

}

//TODO make faster
void Ball::renderToTexture() {
	texture.createBlank(((int)radius)*2, ((int)radius)*2, SDL_TEXTUREACCESS_TARGET);
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

void Ball::recalculateRadius() {
	radius = sqrt(mass / M_PI);
	renderToTexture();
}

void Ball::mergeBalls(Ball* ball1, Ball* ball2, double delta) {
	if(ball1->isMarkedForDeletion || ball2->isMarkedForDeletion) return;
	if(!((utils::distance(ball1->x, ball2->x, ball1->y, ball2->y) < ball1->radius + ball2->radius)
		|| checkCollision(ball1, ball2, delta))) return;
	Ball* big;
	Ball* small;
	if(ball1->mass >= ball2->mass) {
		big = ball1;
		small = ball2;
	} else {
		big = ball2;
		small = ball1;
	}
	double massCenterX = (big->mass * big->x + small->mass * small->x) / (big->mass + small->mass);
	double massCenterY = (big->mass * big->y + small->mass * small->y) / (big->mass + small->mass);
	big->x = massCenterX;
	big->y = massCenterY;
	big->velX = (big->mass * big->velX + small->mass * small->velX) / (big->mass + small->mass);
	big->velY = (big->mass * big->velY + small->mass * small->velY) / (big->mass + small->mass);
	big->color = {
		(unsigned char)((big->mass * big->color.red	  + small->mass * small->color.red  ) / (big->mass + small->mass)),
		(unsigned char)((big->mass * big->color.green + small->mass * small->color.green) / (big->mass + small->mass)),
		(unsigned char)((big->mass * big->color.blue  + small->mass * small->color.blue ) / (big->mass + small->mass))
	};
	big->mass = big->mass + small->mass;
	big->recalculateRadius();
	small->isMarkedForDeletion = true;
}

double Ball::partiallyElasticCollision(double v1, double v2, double m1, double m2, double restitution) {
	return (restitution*m2*(v2-v1)+m1*v1+m2*v2)/(m1+m2);
}

void Ball::collideBalls(Ball* b1, Ball* b2, double delta, CollisionType collisionType) {
	switch(collisionType) {
		case COLLISION_TYPE_BOUNCE: ballsBounce(b1, b2, delta); break;
		case COLLISION_TYPE_MERGE:  mergeBalls (b1, b2, delta);	break;
	}
}

void Ball::ballsBounce(Ball* b1, Ball* b2, double delta) {
	checkAndFixOverlap(b1, b2);
	if(checkCollision(b1, b2, delta))
		recalculateSpeedsAfterCollision(b1, b2);
}

bool Ball::checkCollision(Ball* b1, Ball* b2, double delta) {
	double collisionDistanceSquared = pow(b1->radius + b2->radius, 2);
	double distX = b2->x - b1->x;
	double distY = b2->y - b1->y;
	double relativeSpeedX = (b2->velX - b1->velX)*delta;
	double relativeSpeedY = (b2->velY - b1->velY)*delta;
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
	b1->x += t*b1->velX*delta;
	b1->y += t*b1->velY*delta;
	b2->x += t*b2->velX*delta;
	b2->y += t*b2->velY*delta;
	return true;
}

void Ball::recalculateSpeedsAfterCollision(Ball* b1, Ball* b2) {
	double b1Speed = sqrt(pow(b1->velX, 2) + pow(b1->velY, 2));
	double b2Speed = sqrt(pow(b2->velX, 2) + pow(b2->velY, 2));
	double collisionAngle = atan2(b2->x - b1->x, b1->y - b2->y) - M_PI/2;
	double b1SpeedAngle = atan2(b1->velX, -b1->velY) - M_PI/2;
	double b2SpeedAngle = atan2(b2->velX, -b2->velY) + M_PI/2;
	double b1SpeedXRot =  b1Speed * cos(b1SpeedAngle - collisionAngle);
	double b1SpeedYRot =  b1Speed * sin(b1SpeedAngle - collisionAngle);
	double b2SpeedXRot = -b2Speed * cos(b2SpeedAngle - collisionAngle);
	double b2SpeedYRot = -b2Speed * sin(b2SpeedAngle - collisionAngle);
	double b1NewSpeedXRot = partiallyElasticCollision(b1SpeedXRot,
		b2SpeedXRot, b1->getMass(), b2->getMass(), b1->damping*b2->damping);
	double b2NewSpeedXRot = partiallyElasticCollision(b2SpeedXRot,
		b1SpeedXRot, b2->getMass(), b1->getMass(), b1->damping*b2->damping);
	b1->velX = b1NewSpeedXRot * cos(collisionAngle) + b1SpeedYRot * cos(collisionAngle + M_PI/2);
	b1->velY = b1NewSpeedXRot * sin(collisionAngle) + b1SpeedYRot * sin(collisionAngle + M_PI/2);
	b2->velX = b2NewSpeedXRot * cos(collisionAngle) + b2SpeedYRot * cos(collisionAngle + M_PI/2);
	b2->velY = b2NewSpeedXRot * sin(collisionAngle) + b2SpeedYRot * sin(collisionAngle + M_PI/2);
}

void Ball::checkAndFixOverlap(Ball* b1, Ball* b2) {
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
