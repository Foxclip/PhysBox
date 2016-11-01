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
#include <functional>
#include <libconfig.hh>
#include <iostream>
#include "lwindow.h"
#include "ltexture.h"
#include "globals.h"
#include "simobject.h"
#include "utils.h"

const double SECONDS_PER_FRAME = 1.0/60.0;

class Simulation {

public:

	bool collisionsEnabled = false;
	bool gravityRadialEnabled = false;
	bool gravityVerticalEnabled = false;
	bool backgroundFrictionEnabled = false;
	bool springsEnabled = false;

	CollisionType collisionType = COLLISION_TYPE_BOUNCE;
	double gravityVerticalForce = 0.05;
	double gravityRadialForce = 0.15;
	double springForce = 0.1;
	double springDamping = 0;
	double springDistance = 50;
	double springMaxDistance = springDistance * 1.25;
	double backgroundFrictionForce = 1;
	double cubicPixelMass = 0.001;
	//const double DEFAULT_DAMPING = 0.5;

	int springMaxConnections = 1024;

	const double SIMULATION_SPEED_BASE = 4;
	int simulationSpeedExponent = 0;

	std::vector<SimObject*> objects;

	Simulation(std::function<bool(Simulation*)> exitConditionFunction);
	~Simulation();
	double runSimulation();
	void resetSimulation();
	Ball* addBall(double x, double y, double radius, double speedX, double speedY, utils::Color color);
	void deleteAllObjects();
	void deleteObject(SimObject* object);
	void generateSystem(double centerX, double centerY, double centerRadius, double moonRadius, int moonCount, double gap);

private:

	double time = 0;
	bool pause = true;
	bool exitRequest = false;
	bool uiEnabled = true;
	std::function<bool(Simulation*)> exitContidionFunction;
	utils::Font bigFont;
	utils::Font smallFont;
	utils::Font smallerFont;
	int fpsCount = 0;
	int fps = 0;
	int lastFpsTime = 0;
	double simulationSpeed = pow(SIMULATION_SPEED_BASE, simulationSpeedExponent);

	bool initSDL();
	bool loadMedia();
	bool loadConfig();
	void close();
	void initWindow();
	void render();
	void drawSprings();
	void drawUIText();
	void handleEvents();
	void handleKeyboard(SDL_Event e);
	void processPhysics();
	void processCollisions();
	void deleteMarked();
	void processGravity();
	void processSprings();
	void processOther();
	void drawText(int x, int y, std::string str, utils::Color color, utils::Font& font);
	int getStringWidth(std::string, utils::Font& font);
	utils::Color getBoolColor(bool var);
	void updateFpsCount();
	void changeSimulationSpeed(int change);
	void checkExitCondition();

};