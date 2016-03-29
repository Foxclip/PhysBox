#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_thread.h>
#include <stdio.h>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <sstream>
#include <vector>
#include "lwindow.h"
#include "ltexture.h"
#include "globals.h"
#include "object.h"
#include "utils.h"

bool initSDL();
bool loadMedia();
void close();
void init();
void mainLoop();
void render();
void handleEvents();
void handleKeyboard(SDL_Event e);
void processPhysics();
void drawText(int x, int y, std::string str, utils::Color color, utils::Font& font);
int getStringWidth(std::string, utils::Font& font);
utils::Color getBoolColor(bool var);
void updateFpsCount();
void generateObjects();
void initializeSprings();
void changeSimulationSpeed(int change);
void deleteAllObjects();

bool gQuit = false;
utils::Font bigFont;
utils::Font smallFont;
LWindow mainWindow;
std::vector<Object*> objects;
int fpsCount = 0;
int fps = 0;
int lastFpsTime = 0;
double simulationSpeed = pow(SIMULATION_SPEED_BASE, simulationSpeedExponent);

int main(int argc, char* args[]) {
	if(!initSDL()) {
		printf("Failed to init\n");
	} else if(!loadMedia()) {
		printf("Failed to load media\n");
	} else {
		mainLoop();
	}
	close();
	return 0;
}

bool initSDL() {
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
		printf("Init error: %s\n", SDL_GetError());
		return false;
	}
	int imgFlags = IMG_INIT_PNG;
	if(!(IMG_Init(imgFlags) & imgFlags)) {
		printf("SDL image init error: %s\n", IMG_GetError());
		return false;
	}
	if(TTF_Init() == -1) {
		printf("TTF init error: %s\n", TTF_GetError());
		return false;
	}
	return true;
}

bool loadMedia() {
	bigFont.loadFont("arial.ttf", 28);
	smallFont.loadFont("arial.ttf", 15);
	return true;
}

void close() {

	deleteAllObjects();

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();

}

void init() {

	mainWindow.init();
	mainWindow.maximize();
	//mainWindow.setFullScreen(true);

	srand(SDL_GetTicks());

	generateObjects();

}

void mainLoop() {

	init();

	while(!gQuit) {

		handleEvents();
		processPhysics();
		render();

		updateFpsCount();

	}
}

void render() {

	if(mainWindow.isMinimised())
		return;

	SDL_SetRenderDrawColor(mainWindow.getRenderer(), 0, 0, 0, 255);
	SDL_RenderClear(mainWindow.getRenderer());

	SDL_SetRenderDrawColor(mainWindow.getRenderer(), 255, 255, 0, 255);
	for(int i = 0; i < (int)objects.size(); i++) {
		Object* object1 = objects.at(i);
		for(int j = 0; j < (int)objects.size(); j++) {
			Object* object2 = objects.at(j);
			if(std::find(object1->springConnections.begin(), object1->springConnections.end(),
				object2) != object1->springConnections.end()) {
				SDL_RenderDrawLine(mainWindow.getRenderer(), (int)object1->x, (int)object1->y, (int)object2->x, (int)object2->y);
			}
		}
	}

	for(int i = 0; i < (int)objects.size(); i++)
		objects.at(i)->render();

	drawText(0, 0, "fps: " + std::to_string(fps), { 255, 255, 0 }, smallFont);

	drawText(mainWindow.getWidth() - getStringWidth("Collisions (1)", smallFont), 0,
		"Collisions (1)", getBoolColor(collisionsEnabled), smallFont);
	drawText(mainWindow.getWidth() - getStringWidth("Gravity radial (2)", smallFont), smallFont.getSize(),
		"Gravity radial (2)", getBoolColor(gravityRadialEnabled), smallFont);
	drawText(mainWindow.getWidth() - getStringWidth("Gravity vertical (3)", smallFont), smallFont.getSize()*2,
		"Gravity vertical (3)", getBoolColor(gravityVerticalEnabled), smallFont);
	drawText(mainWindow.getWidth() - getStringWidth("Background friction (4)", smallFont), smallFont.getSize()*3,
		"Background friction (4)", getBoolColor(backgroundFrictionEnabled), smallFont);
	drawText(mainWindow.getWidth() - getStringWidth("Springs (5)", smallFont), smallFont.getSize()*4,
		"Springs (5)", getBoolColor(springsEnabled), smallFont);

	std::string str = "Simulation speed: " + std::to_string(simulationSpeed) +
		" (" + std::to_string((int)simulationSpeedExponent) + ")";
	drawText(mainWindow.getWidth() - getStringWidth(str, smallFont), smallFont.getSize()*7, str, { 255, 255, 0 }, smallFont);

	if(pause)
		drawText((mainWindow.getWidth() - getStringWidth("PAUSE", bigFont)) / 2,
			(mainWindow.getHeight() - bigFont.getSize()) / 2, "PAUSE", { 255, 255, 0 }, bigFont);

	SDL_RenderPresent(mainWindow.getRenderer());

}

void handleEvents() {
	SDL_Event e;
	while(SDL_PollEvent(&e) != 0) {
		switch(e.type) {
		case SDL_QUIT:
			gQuit = true; break;
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			handleKeyboard(e); break;
		case SDL_WINDOWEVENT:
			mainWindow.handleEvent(e); break;
		}
	}
}

void handleKeyboard(SDL_Event e) {
	if(e.type == SDL_KEYDOWN) {
		switch(e.key.keysym.scancode) {
			case SDL_SCANCODE_ESCAPE: gQuit = true; break;
			case SDL_SCANCODE_SPACE: pause = !pause; break;
			case SDL_SCANCODE_R: generateObjects(); break;
			case SDL_SCANCODE_1: collisionsEnabled = !collisionsEnabled; break;
			case SDL_SCANCODE_2: gravityRadialEnabled = !gravityRadialEnabled; break;
			case SDL_SCANCODE_3: gravityVerticalEnabled = !gravityVerticalEnabled; break;
			case SDL_SCANCODE_4: backgroundFrictionEnabled = !backgroundFrictionEnabled; break;
			case SDL_SCANCODE_5: springsEnabled = !springsEnabled; break;
			case SDL_SCANCODE_KP_PLUS: changeSimulationSpeed(1); break;
			case SDL_SCANCODE_KP_MINUS: changeSimulationSpeed(-1); break;
		}
	}
}

void processPhysics() {
	if(pause) return;
	if(gravityVerticalEnabled) {
		for(int i = 0; i < (int)objects.size(); i++) {
			objects.at(i)->calculateVerticalGravity(simulationSpeed);
		}
	}
	if(gravityRadialEnabled) {
		for(int i = 0; i < (int)objects.size(); i++) {
			for(int j = 0; j < (int)objects.size(); j++) {
				if(i == j) continue;
				objects.at(i)->calculateGravity(objects.at(j), simulationSpeed);
			}
		}
	}
	if(springsEnabled) {
		for(int i = 0; i < (int)objects.size(); i++) {
			Object* object1 = objects.at(i);
			if(object1->springConnections.size() >= springMaxConnections) continue;
			for(int j = 0; j < (int)objects.size(); j++) {
				Object* object2 = objects.at(j);
				if(i == j) continue;
				if(object1->springConnections.size() >= springMaxConnections) break;
				if(object2->incomingSpringConnections >= springMaxConnections) continue;
				if(std::find(object1->springConnections.begin(), object1->springConnections.end(),
					object2) != object1->springConnections.end()) continue;
				if(utils::distance(object1->x, object2->x, object1->y, object2->y) < springMaxDistance) {
					object1->springConnections.push_back(object2);
					object2->incomingSpringConnections++;
				}
			}
		}
		for(int i = 0; i < (int)objects.size(); i++) {
			Object* object = objects.at(i);
			for(int j = object->springConnections.size() - 1; j >= 0; j--) {
				object->calculateSprings(object->springConnections.at(j), simulationSpeed);
			}
		}
	}
	for(int i = 0; i < (int)objects.size(); i++) {
		objects.at(i)->move(simulationSpeed);
	}
}

void drawText(int x, int y, std::string str, utils::Color color, utils::Font& font) {
	LTexture textTexture;
	textTexture.loadFromRenderedText(str, { color.red, color.green, color.blue, 255 }, font.getSDLFont(), false);
	textTexture.render(x, y);
}

int getStringWidth(std::string str, utils::Font& font) {
	int w;
	TTF_SizeText(font.getSDLFont(), str.c_str(), &w, NULL);
	return w;
}

utils::Color getBoolColor(bool var) {
	if(var)
		return { 0, 255, 0 };
	else
		return { 255, 0, 0 };
}

void updateFpsCount() {
	int currentTime = SDL_GetTicks();
	fpsCount++;
	if(currentTime - lastFpsTime > 1000) {
		fps = fpsCount;
		fpsCount = 0;
		lastFpsTime = currentTime;
	}
}

void generateObjects() {

	deleteAllObjects();

	SDL_SetRenderDrawColor(mainWindow.getRenderer(), 0, 0, 0, 255);
	SDL_RenderClear(mainWindow.getRenderer());
	drawText((mainWindow.getWidth() - getStringWidth("GENERATING...", bigFont)) / 2,
		(mainWindow.getHeight() - bigFont.getSize()) / 2, "GENERATING...", { 255, 255, 0 }, bigFont);
	SDL_RenderPresent(mainWindow.getRenderer());

	for(int i = 0; i < 100; i++)
		objects.push_back(new Ball(
			utils::randomBetween(0, mainWindow.getWidth()),
			utils::randomBetween(0, mainWindow.getHeight()),
			10,
			utils::randomBetween(-1, 1),
			utils::randomBetween(-1, 1),
			utils::randomColor()
			));

	//initializeSprings();

	//objects.push_back(new Ball(800, 400, 50, 0, 0, { 255, 0, 0 }));
	//objects.push_back(new Ball(800, 200, 10, 1, 0, { 0, 0, 255 }));

}

void initializeSprings() {
	for(int i = 0; i < (int)objects.size(); i++) {
		Object* object1 = objects.at(i);
		for(int j = 0; j < (int)objects.size(); j++) {
			Object* object2 = objects.at(j);
			if(object1 == object2) continue;
			double distance = utils::distance(object1->x, object2->x, object1->y, object2->y);
			if(distance <= springInitialDistance) {
				object1->springConnections.push_back(object2);
			}
		}
	}
}

void changeSimulationSpeed(int change) {
	simulationSpeedExponent += change;
	simulationSpeed = pow(SIMULATION_SPEED_BASE, simulationSpeedExponent);
}

void deleteAllObjects() {
	for(int i = 0; i < (int)objects.size(); i++)
		delete objects.at(i);
	objects.clear();
}