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
int getStringWidth(const char* str, Font& font);
Color getBoolColor(bool var);
void updateFpsCount();
void generateObjects();
void deleteAllObjects();

bool gQuit = false;
SDL_Renderer* mainRenderer = NULL;
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
	if(!mainWindow.init()) {
		printf("Window creation error: %s\n", SDL_GetError());
		return false;
	}
	mainRenderer = mainWindow.createRenderer();
	if(mainRenderer == NULL) {
		printf("Renderer creation error: %s\n", SDL_GetError());
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

	mainWindow.maximize();
	//gWindow.setFullScreen(true);

	return true;
}

bool loadMedia() {
	bigFont.loadFont("arial.ttf", 28);
	smallFont.loadFont("arial.ttf", 15);
	return true;
}

void close() {

	deleteAllObjects();

	SDL_DestroyRenderer(mainRenderer);
	mainRenderer = NULL;
	TTF_Quit();
	SDL_Quit();

}

void init() {

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

	SDL_SetRenderDrawColor(mainRenderer, 0, 0, 0, 255);
	SDL_RenderClear(mainRenderer);

	for(int i = 0; i < (int)objects.size(); i++)
		objects.at(i)->render();

	drawText(0, 0, std::to_string(fps), { 255, 255, 0 }, smallFont);
	drawText(mainWindow.getWidth() - getStringWidth("Springs", smallFont), 0,
		"Springs", getBoolColor(springsEnabled), smallFont);
	if(pause)
		drawText((mainWindow.getWidth() - getStringWidth("PAUSE", bigFont)) / 2,
			(mainWindow.getHeight() - bigFont.getSize()) / 2, "PAUSE", { 255, 255, 0 }, bigFont);
	SDL_RenderPresent(mainRenderer);

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
		case SDLK_ESCAPE:
			gQuit = true; break;
		case SDLK_SPACE:
			pause = !pause; break;
		case SDLK_r:
			generateObjects(); break;
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

int getStringWidth(const char* str, Font& font) {
	int w;
	TTF_SizeText(font.getSDLFont(), str, &w, NULL);
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

	for(int i = 0; i < 10; i++)
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