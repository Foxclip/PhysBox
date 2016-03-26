#include "object.h"
#include "globals.h"

Object::Object() {
	x = 0;
	y = 0;
	speedX = 0;
	speedY = 0;
}

void Object::move() {
	x += speedX;
	y += speedY;
}

Ball::Ball(double x, double y, double radius, double speedX, double speedY, Color color): Object() {
	this->x = x;
	this->y = y;
	this->radius = radius;
	this->speedX = speedX;
	this->speedY = speedY;
	this->color = color;
}

void Ball::move() {

	Object::move();

	if(x < radius) {
		x = radius;
		speedX = -speedX;
	}
	if(x > gWindow.getWidth() - radius) {
		x = gWindow.getWidth() - radius;
		speedX = -speedX;
	}
	if(y < radius) {
		y = radius;
		speedY = -speedY;
	}
	if(y > gWindow.getHeight() - radius) {
		y = gWindow.getHeight() - radius;
		speedY = -speedY;
	}

}

void Ball::render() {
	SDL_SetRenderDrawColor(gRenderer, color.red, color.green, color.blue, 255);
	for(int yCoord = -(int)radius; yCoord <= (int)radius; yCoord++)
		for(int xCoord = -(int)radius; xCoord <= (int)radius; xCoord++)
			if((xCoord*xCoord + yCoord*yCoord) <= radius*radius)
				SDL_RenderDrawPoint(gRenderer, (int)(x + xCoord), (int)(y + yCoord));
}
