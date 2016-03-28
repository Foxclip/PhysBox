#include "object.h"
#include "globals.h"

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

void Object::calculateVerticalGravity() {
	speedY += gravityVerticalForce;
}

Ball::Ball(double x, double y, double radius, double speedX, double speedY, Color color) {
	
	this->x = x;
	this->y = y;
	this->radius = radius;
	this->speedX = speedX;
	this->speedY = speedY;
	this->color = color;

	texture.createBlank(((int)radius)*2, ((int)radius)*2, SDL_TEXTUREACCESS_TARGET);
	renderToTexture();

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