#include "simulation.h"

LWindow mainWindow;

Simulation::Simulation() {
	initSDL();
	loadMedia();
	initWindow();
}

Simulation::~Simulation() {
	close();
}

void Simulation::runSimulation() {
	while(!gQuit) {
		handleEvents();
		processPhysics();
		render();
		updateFpsCount();
	}
}

bool Simulation::initSDL() {
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

bool Simulation::loadMedia() {
	bigFont.loadFont("arial.ttf", 28);
	smallFont.loadFont("arial.ttf", 15);
	return true;
}

void Simulation::close() {
	deleteAllObjects();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void Simulation::initWindow() {
	mainWindow.init();
	mainWindow.maximize();
	mainWindow.setFullScreen(true);
}

void Simulation::render() {

	if(mainWindow.isMinimised())
		return;

	SDL_SetRenderDrawColor(mainWindow.getRenderer(), 0, 0, 0, 255);
	SDL_RenderClear(mainWindow.getRenderer());

	if(springsEnabled) {
		SDL_SetRenderDrawColor(mainWindow.getRenderer(), 255, 255, 0, 255);
		for(SimObject* object1: objects) {
			for(SimObject* object2: objects) {
				if(std::find(object1->springConnections.begin(), object1->springConnections.end(),
					object2) != object1->springConnections.end()) {
					SDL_RenderDrawLine(mainWindow.getRenderer(), (int)object1->x, (int)object1->y, (int)object2->x, (int)object2->y);
				}
			}
		}
	}

	for(SimObject* object: objects)
		object->render();

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

void Simulation::handleEvents() {
	SDL_Event e;
	while(SDL_PollEvent(&e) != 0) {
		switch(e.type) {
			case SDL_QUIT:						gQuit = true;				break;
			case SDL_KEYUP: case SDL_KEYDOWN:	handleKeyboard(e);			break;
			case SDL_WINDOWEVENT:				mainWindow.handleEvent(e);	break;
		}
	}
}

void Simulation::handleKeyboard(SDL_Event e) {
	if(e.type == SDL_KEYDOWN) {
		switch(e.key.keysym.scancode) {
			case SDL_SCANCODE_ESCAPE:	gQuit = true;											break;
			case SDL_SCANCODE_SPACE:	pause = !pause;											break;
			case SDL_SCANCODE_1:		collisionsEnabled = !collisionsEnabled;					break;
			case SDL_SCANCODE_2:		gravityRadialEnabled = !gravityRadialEnabled;			break;
			case SDL_SCANCODE_3:		gravityVerticalEnabled = !gravityVerticalEnabled;		break;
			case SDL_SCANCODE_4:		backgroundFrictionEnabled = !backgroundFrictionEnabled; break;
			case SDL_SCANCODE_5:		springsEnabled = !springsEnabled;						break;
			case SDL_SCANCODE_KP_PLUS:	changeSimulationSpeed(1);								break;
			case SDL_SCANCODE_KP_MINUS: changeSimulationSpeed(-1);								break;
		}
	}
}

void Simulation::processPhysics() {
	if(pause) return;
	if(collisionsEnabled) {
		for(SimObject* object1: objects) {
			for(SimObject* object2: objects) {
				if(object1 == object2) continue;
				Ball::collide(object1, object2, simulationSpeed);
			}
		}
	}
	if(gravityVerticalEnabled) {
		for(SimObject* object: objects) {
			object->calculateVerticalGravity(simulationSpeed);
		}
	}
	if(gravityRadialEnabled) {
		for(SimObject* object1: objects) {
			for(SimObject* object2: objects) {
				if(object1 == object2) continue;
				object1->calculateGravity(object2, simulationSpeed);
			}
		}
	}
	if(springsEnabled) {
		for(SimObject* object1: objects) {
			if(object1->springConnections.size() >= springMaxConnections) continue;
			for(SimObject* object2: objects) {
				if(object1 == object2) continue;
				if(object1->springConnections.size() >= springMaxConnections) break;
				if(object2->incomingSpringConnectionsCount >= springMaxConnections) continue;
				if(std::find(object1->springConnections.begin(), object1->springConnections.end(),
					object2) != object1->springConnections.end()) continue;
				if(utils::distance(object1->x, object2->x, object1->y, object2->y) < springMaxDistance) {
					object1->springConnections.push_back(object2);
					object2->incomingSpringConnectionsCount++;
				}
			}
		}
		for(SimObject* object: objects) {
			for(int j = object->springConnections.size() - 1; j >= 0; j--) {
				object->calculateSprings(object->springConnections.at(j), simulationSpeed);
			}
		}
	}
	if(backgroundFrictionEnabled) {
		for(SimObject* object: objects) {
			object->calculateBackgroudFriction(simulationSpeed);
		}
	}
	for(SimObject* object: objects) {
		object->move(simulationSpeed);
	}
}

void Simulation::drawText(int x, int y, std::string str, utils::Color color, utils::Font& font) {
	LTexture textTexture;
	textTexture.loadFromRenderedText(str, { color.red, color.green, color.blue, 255 }, font.getSDLFont(), false);
	textTexture.render(x, y);
}

int Simulation::getStringWidth(std::string str, utils::Font& font) {
	int w;
	TTF_SizeText(font.getSDLFont(), str.c_str(), &w, NULL);
	return w;
}

utils::Color Simulation::getBoolColor(bool var) {
	if(var)
		return { 0, 255, 0 };
	else
		return { 255, 0, 0 };
}

void Simulation::updateFpsCount() {
	int currentTime = SDL_GetTicks();
	fpsCount++;
	if(currentTime - lastFpsTime > 1000) {
		fps = fpsCount;
		fpsCount = 0;
		lastFpsTime = currentTime;
	}
}

void Simulation::addBall(double x, double y, double radius, double speedX, double speedY, utils::Color color) {
	objects.push_back(new Ball(x, y, radius, speedX, speedY, color));
}

void Simulation::changeSimulationSpeed(int change) {
	simulationSpeedExponent += change;
	simulationSpeed = pow(SIMULATION_SPEED_BASE, simulationSpeedExponent);
}

void Simulation::deleteAllObjects() {
	for(SimObject* object: objects)
		delete object;
	objects.clear();
}