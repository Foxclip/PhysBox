#include "simobject.h"
#include "globals.h"
#include <algorithm>


void SimObject::addToRigidBodyWorld(btDynamicsWorld* world) {
	world->addRigidBody(rigidBody);
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


void SimObject::render(int offsetX, int offsetY) {
	texture.render((int)getX() - texture.getWidth()/2 + offsetX, (int)getY() - texture.getHeight()/2 + offsetY);
}

double SimObject::distanceBetween(SimObject* object1, SimObject* object2) {
		double deltaX = object1->getX() - object2->getX();
		double deltaY = object1->getY() - object2->getY();
		return sqrt(deltaX*deltaX + deltaY*deltaY);
}

void SimObject::calculateBackgroundFriction(double delta, double backgroundFrictionForce) {
	double velX = getVelX();
	double velY = getVelY();
	double speed = sqrt(velX*velX + velY*velY);
	if(speed == 0) return;
	double forceX = -velX / speed * backgroundFrictionForce;
	double forceY = -velY / speed * backgroundFrictionForce;
	double oldSpeedX = velX;
	double oldSpeedY = velY;
	velX += forceX / getMass() * delta;
	velY += forceY / getMass() * delta;
	if(velX * oldSpeedX < 0) {
		velX = 0;
	}
	if(velY * oldSpeedY < 0) {
		velY = 0;
	}
	setVelX(velX);
	setVelY(velY);
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

Ball::Ball(double x, double y, double radius, double speedX, double speedY, utils::Color color, bool isActive) {
	
	btCollisionShape* shape = new btSphereShape(radius);
	btDefaultMotionState* mState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(x, y, 0)));
	double mass;
	if(isActive) {
		mass = calculateMass(radius);
	} else {
		mass = 0;
	}
	btVector3 inertia(0, 0, 0);
	shape->calculateLocalInertia(mass, inertia);
	btRigidBody::btRigidBodyConstructionInfo ci(mass, mState, shape, inertia);
	rigidBody = new btRigidBody(ci);
	rigidBody->setActivationState(DISABLE_DEACTIVATION);
	rigidBody->setRestitution(1);

	setVelX(speedX);
	setVelY(speedY);

	this->radius = radius;
	this->color = color;
	this->isActive = isActive;
	objectType = OBJECT_TYPE_BALL;

	renderToTexture();

}

//TODO make faster
void Ball::renderToTexture() {
	texture.createBlank(((int)radius)*2, ((int)radius)*2, SDL_TEXTUREACCESS_TARGET);
	texture.setAsRenderTarget();
	SDL_SetTextureBlendMode(texture.getSDLTexture(), SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(mainWindow.getRenderer(), 0, 0, 0, 0);
	SDL_RenderClear(mainWindow.getRenderer());
	SDL_SetRenderDrawColor(mainWindow.getRenderer(), color.red, color.green, color.blue, 255);
	for(int yCoord = -(int)radius; yCoord <= (int)radius; yCoord++) {
		for(int xCoord = -(int)radius; xCoord <= (int)radius; xCoord++) {
			double xCoordShifted = xCoord + 0.5;
			double yCoordShifted = yCoord + 0.5;
			if((xCoordShifted*xCoordShifted + yCoordShifted*yCoordShifted) <= radius*radius) {
				SDL_RenderDrawPoint(mainWindow.getRenderer(), (int)(xCoord + radius), (int)(yCoord + radius));
			}
		}
	}
	SDL_SetRenderTarget(mainWindow.getRenderer(), NULL);
}

double Ball::calculateMass(double rad) {
	return M_PI * rad*rad;
}

void Ball::recalculateRadius() {
	radius = sqrt(getMass() / M_PI);
	renderToTexture();
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
		(unsigned char)((big->getMass() * big->color.red   + small->getMass() * small->color.red  ) / (big->getMass() + small->getMass())),
		(unsigned char)((big->getMass() * big->color.green + small->getMass() * small->color.green) / (big->getMass() + small->getMass())),
		(unsigned char)((big->getMass() * big->color.blue  + small->getMass() * small->color.blue ) / (big->getMass() + small->getMass()))
	};
	big->setMass(big->getMass() + small->getMass());
	big->recalculateRadius();
	small->isMarkedForDeletion = true;
}
