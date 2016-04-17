#pragma once

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

void runSimulation();
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
void changeSimulationSpeed(int change);
void deleteAllObjects();