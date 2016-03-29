#include "object.h"
#include "globals.h"
#include <algorithm>

Object::Object() {
	x = 0;
	y = 0;
	speedX = 0;
	speedY = 0;
	damping = DAMPING;
}

void Object::move(double delta) {
	x += speedX * delta;
	y += speedY * delta;
}

void Object::render() {
	texture.render((int)x - texture.getWidth()/2, (int)y - texture.getHeight()/2);
}

void Object::calculateVerticalGravity(double delta) {
	speedY += gravityVerticalForce * delta;
}

void Object::calculateGravity(Object* anotherObject, double delta) {
	double distance = utils::distance(x, anotherObject->x, y, anotherObject->y);
	if(distance == 0)
		return;
	double force = gravityRadialForce * getMass() * anotherObject->getMass() / pow(distance, 2);
	double forceX = (anotherObject->x - x) / distance * force;
	double forceY = (anotherObject->y - y) / distance * force;
	speedX += forceX / getMass() * delta;
	speedY += forceY / getMass() * delta;
}

void Object::calculateSprings(Object* anotherObject, double delta) {
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

double Object::getMass() {
	return mass;
}

Ball::Ball(double x, double y, double radius, double speedX, double speedY, utils::Color color) {
	
	this->x = x;
	this->y = y;
	this->radius = radius;
	this->speedX = speedX;
	this->speedY = speedY;
	this->color = color;

	texture.createBlank(((int)radius)*2, ((int)radius)*2, SDL_TEXTUREACCESS_TARGET);
	renderToTexture();
	recalculateMass();

}

void Ball::move(double delta) {

	Object::move(delta);

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
			if((xCoord*xCoord + yCoord*yCoord) <= radius*radius)
				SDL_RenderDrawPoint(mainWindow.getRenderer(), (int)(xCoord + radius), (int)(yCoord + radius));
	SDL_SetRenderTarget(mainWindow.getRenderer(), NULL);
}

void Ball::recalculateMass() {
	mass = M_PI * pow(radius, 2);
}
