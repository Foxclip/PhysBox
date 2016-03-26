#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_thread.h>
#include <stdio.h>
#include <string>
#include <algorithm>
#include <sstream>
#include <vector>
#include "lwindow.h"
#include "globals.h"
#include "object.h"

bool initSDL();
bool loadMedia();
void close();
void init();
void mainLoop();
void render();
void handleEvents();
void handleKeyboard(SDL_Event e);
void processPhysics();

bool gQuit = false;
SDL_Renderer* gRenderer = NULL;
TTF_Font* gFont;
LWindow gWindow;
std::vector<Object*> objects;

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
	if(!gWindow.init()) {
		printf("Window creation error: %s\n", SDL_GetError());
		return false;
	}
	gRenderer = gWindow.createRenderer();
	if(gRenderer == NULL) {
		printf("Renderer creation error: %s\n", SDL_GetError());
		return false;
	}
	if(TTF_Init() == -1) {
		printf("TTF init error: %s\n", TTF_GetError());
		return false;
	}
	return true;
}

bool loadMedia() {
	gFont = TTF_OpenFont("arial.ttf", 28);
	if(!gFont) {
		printf("Font loading error: %s\n", TTF_GetError());
		return false;
	}
	return true;
}

void close() {
	SDL_DestroyRenderer(gRenderer);
	gRenderer = NULL;
	gFont = NULL;
	TTF_Quit();
	SDL_Quit();
}

void init() {
	objects.push_back(new Ball(50, 50, 30, 5, 5, { 255, 0, 0 }));
	objects.push_back(new Ball(150, 150, 20, 5, 0, { 0, 0, 255 }));
}

void mainLoop() {
	init();
	while(!gQuit) {
		handleEvents();
		processPhysics();
		render();
	}
}

void render() {
	if(gWindow.isMinimised())
		return;
	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	SDL_RenderClear(gRenderer);
	for(int i = 0; i < (int)objects.size(); i++)
		objects.at(i)->render();
	SDL_RenderPresent(gRenderer);
}

void handleEvents() {
	SDL_Event e;
	while(SDL_PollEvent(&e) != 0) {
		switch(e.type) {
		case SDL_QUIT:
			gQuit = true; break;
		case SDL_KEYDOWN:
			handleKeyboard(e); break;
		case SDL_WINDOWEVENT:
			gWindow.handleEvent(e); break;
		}
	}
}

void handleKeyboard(SDL_Event e) {
	switch(e.key.keysym.sym) {
	case SDLK_ESCAPE:
		gQuit = true; break;
	}
}

void processPhysics() {
	for(int i = 0; i < (int)objects.size(); i++)
		objects.at(i)->move();
}