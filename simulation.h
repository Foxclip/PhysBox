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
#include "simobject.h"
#include "utils.h"

class Simulation {

public:

	Simulation();
	~Simulation();
	void runSimulation();
	void addBall(double x, double y, double radius, double speedX, double speedY, utils::Color color);
	void deleteAllObjects();

private:

	bool gQuit = false;
	utils::Font bigFont;
	utils::Font smallFont;
	std::vector<SimObject*> objects;
	int fpsCount = 0;
	int fps = 0;
	int lastFpsTime = 0;
	double simulationSpeed = pow(SIMULATION_SPEED_BASE, simulationSpeedExponent);

	bool initSDL();
	bool loadMedia();
	void close();
	void initWindow();
	void render();
	void handleEvents();
	void handleKeyboard(SDL_Event e);
	void processPhysics();
	void drawText(int x, int y, std::string str, utils::Color color, utils::Font& font);
	int getStringWidth(std::string, utils::Font& font);
	utils::Color getBoolColor(bool var);
	void updateFpsCount();
	void changeSimulationSpeed(int change);

};