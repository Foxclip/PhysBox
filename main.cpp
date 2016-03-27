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
void drawText(int x, int y, std::string str, Color color, Font& font);
int getStringWidth(std::string, Font& font);
Color getBoolColor(bool var);
void updateFpsCount();
void generateObjects();
void deleteAllObjects();

bool gQuit = false;
Font bigFont;
Font smallFont;
LWindow mainWindow;
std::vector<Object*> objects;
int fpsCount = 0;
int fps = 0;
int lastFpsTime = 0;

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
	SDL_Quit();

}

void init() {

	mainWindow.init();
	mainWindow.maximize();
	mainWindow.setFullScreen(true);

	srand(SDL_GetTicks());

	//objects.push_back(new Ball(50, 50, 30, 5, 5, { 255, 0, 0 }));
	//objects.push_back(new Ball(150, 150, 20, 5, 0, { 0, 0, 255 }));

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

	for(int i = 0; i < (int)objects.size(); i++)
		objects.at(i)->render();

	drawText(0, 0, std::to_string(fps), { 255, 255, 0 }, smallFont);

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
		switch(e.key.keysym.sym) {
		case SDLK_ESCAPE: gQuit = true; break;
		case SDLK_SPACE: pause = !pause; break;
		case SDLK_r: generateObjects(); break;
		case SDLK_1: collisionsEnabled = !collisionsEnabled; break;
		case SDLK_2: gravityRadialEnabled = !gravityRadialEnabled; break;
		case SDLK_3: gravityVerticalEnabled = !gravityVerticalEnabled; break;
		case SDLK_4: backgroundFrictionEnabled = !backgroundFrictionEnabled; break;
		case SDLK_5: springsEnabled = !springsEnabled; break;
		}
	}
}

void processPhysics() {
	if(pause) return;
	if(gravityVerticalEnabled)
		for(int i = 0; i < (int)objects.size(); i++)
			objects.at(i)->calculateVerticalGravity();
	for(int i = 0; i < (int)objects.size(); i++)
		objects.at(i)->move();
}

void drawText(int x, int y, std::string str, Color color, Font& font) {
	LTexture textTexture;
	textTexture.loadFromRenderedText(str, { color.red, color.green, color.blue, 255 }, font.getSDLFont(), false);
	textTexture.render(x, y);
}

int getStringWidth(std::string str, Font& font) {
	int w;
	TTF_SizeText(font.getSDLFont(), str.c_str(), &w, NULL);
	return w;
}

Color getBoolColor(bool var) {
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

	
	drawText((mainWindow.getWidth() - getStringWidth("GENERATING...", bigFont)) / 2,
		(mainWindow.getHeight() - bigFont.getSize()) / 2, "GENERATING...", { 255, 255, 0 }, bigFont);
	SDL_RenderPresent(mainWindow.getRenderer());

	for(int i = 0; i < 1000; i++)
		objects.push_back(new Ball(
			randomBetween(0, mainWindow.getWidth()),
			randomBetween(0, mainWindow.getHeight()),
			10,
			randomBetween(-10, 10),
			randomBetween(-10, 10),
			randomColor()
			));

}

void deleteAllObjects() {
	for(int i = 0; i < (int)objects.size(); i++)
		delete objects.at(i);
	objects.clear();
}