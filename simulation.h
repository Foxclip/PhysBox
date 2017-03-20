#pragma once

#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <sstream>
#include <vector>
#include <functional>
#include <libconfig.hh>
#include <btBulletDynamicsCommon.h>
#include <iostream>
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
	double gravityVerticalForce = 100.0;
	double gravityRadialForce = 0.15;
	double springForce = 0.1;
	double springDamping = 0;
	double springDistance = 50;
	double springMaxDistance = springDistance * 1.25;
	double backgroundFrictionForce = 1;
	double cubicPixelMass = 0.001;

	double bumpSpeed = 1;
	double gravityIncrement = 0.1;

	int springMaxConnections = 1024;

	const double SIMULATION_SPEED_BASE = 4;
	int simulationSpeedExponent = 0;
	double mouseWheelzoomFactor = 1.1;

	bool isWheelDown = false;
	sf::Vector2f mousePrev;

	std::vector<SimObject*> objects;
	std::vector<Plane*> planes;

	Simulation(std::function<bool(Simulation*)> exitConditionFunction);
	~Simulation();
	double runSimulation();
	void resetSimulation();
	void addBall(double x, double y, double radius, double speedX, double speedY, sf::Color color, bool isActive = true);
	void addPolygon(double x, double y, double speedX, double speedY, std::vector<Point> points, sf::Color color, bool isActive = true);
	void addRandomPolygon(double x, double y, double speedX, double speedY, int vertexCount, double minLength, double maxLength, sf::Color color, bool isActive = true);
	void addTrack(int pointCount, double distanceBetweenPoints, double thickness, double bottomLimit, double topLimit);
	void addPlane(Plane::PlaneSide side);
	void deleteAllObjects();
	void deleteObject(SimObject* object);

private:

	sf::View view;
	double time = 0;
	sf::Clock clock;
	bool pause = true;
	bool exitRequest = false;
	bool uiEnabled = true;
	std::function<bool(Simulation*)> exitContidionFunction;
	int fpsCount = 0;
	int fps = 0;
	int lastFpsTime = 0;
	double simulationSpeed = pow(SIMULATION_SPEED_BASE, simulationSpeedExponent);
	int textDrawOffset = 0;
	enum WindowSnap {
		WINDOW_SNAP_OFF = 0,
		WINDOW_SNAP_H_LEFT = 1,
		WINDOW_SNAP_H_RIGHT = 2,
		WINDOW_SNAP_V_TOP = 4,
		WINDOW_SNAP_V_BOTTOM = 8,
		WINDOW_SNAP_H_CENTER = 16,
		WINDOW_SNAP_V_CENTER = 32
	};
	sf::Font font;
	enum FontSize {
		FONT_SIZE_SMALL = 10,
		FONT_SIZE_NORMAL = 15,
		FONT_SIZE_BIG = 28
	};
	FontSize currentFontSize = FONT_SIZE_NORMAL;
	sf::Color currentTextColor = sf::Color::Yellow;

	void initSFML();
	void initBullet();
	bool loadMedia();
	void loadConfig();
	void close();
	void render();
	void drawSprings();
	void drawUIText();
	void drawOption(std::string text, bool* option);
	void drawInfo(std::string text);
	void drawInfo(std::string text, double* parameter);
	void drawBlank();
	void handleEvents();
	void handleKeyboard(sf::Event e);
	void handleMouse(sf::Event e);
	void processPhysics();
	void deleteMarked();
	void processGravity();
	void processSprings();
	void drawText(int x, int y, int snap, std::string str);
	sf::Color getBoolColor(bool var);
	void updateFpsCount();
	void changeSimulationSpeed(int change);
	void nextCollisionType();
	void checkExitCondition();
	void bumpAll(double velX, double velY);

};