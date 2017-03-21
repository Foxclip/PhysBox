#include "simulation.h"

sf::RenderWindow mainWindow;

Simulation::Simulation(std::function<bool(Simulation*)> exitConditionFunction) {
	this->exitContidionFunction = exitConditionFunction;
	initSFML();
	initBullet();
	loadConfig();
	loadMedia();
	this->exitContidionFunction = exitConditionFunction;
}

Simulation::~Simulation() {
	close();
}

double Simulation::runSimulation() {
	while(!exitRequest) {
		handleEvents();
		processPhysics();
		render();
		updateFpsCount();
		checkExitCondition();
	}
	return 0;
}

void Simulation::initSFML() {
	
	sf::ContextSettings settings;
	settings.antialiasingLevel = 8;
	mainWindow.create(sf::VideoMode::getFullscreenModes()[0], "SFML", sf::Style::Fullscreen, settings);
	mainWindow.setVerticalSyncEnabled(true);

	view.reset(sf::FloatRect(0, 0, mainWindow.getSize().x, mainWindow.getSize().y));
	mainWindow.setView(view);

}

void Simulation::initBullet() {
	btBroadphaseInterface* broadphase = new btDbvtBroadphase();
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->getSolverInfo().m_numIterations = 2000;
	dynamicsWorld->setGravity(btVector3(0, gravityVerticalForce, 0));
}

bool Simulation::loadMedia() {
	return font.loadFromFile("arial.ttf");
}

void Simulation::loadConfig() {

	libconfig::Config cfg;

    cfg.readFile("simulation_settings.cfg");
    collisionsEnabled			= cfg.lookup("collisionsEnabled");
    gravityRadialEnabled		= cfg.lookup("gravityRadialEnabled");
    gravityVerticalEnabled		= cfg.lookup("gravityVerticalEnabled");
    backgroundFrictionEnabled	= cfg.lookup("backgroundFrictionEnabled");
    springsEnabled				= cfg.lookup("springsEnabled");

	int _collisionType = cfg.lookup("collisionType");
	switch(_collisionType) {
		case 0:  collisionType = COLLISION_TYPE_BOUNCE;	break;
		case 1:	 collisionType = COLLISION_TYPE_MERGE;	break;
		default: collisionType = COLLISION_TYPE_BOUNCE;	break;
	}
	gravityVerticalForce	= cfg.lookup("gravityVerticalForce");
	gravityRadialForce		= cfg.lookup("gravityRadialForce");
	springForce				= cfg.lookup("springForce");
	springDamping			= cfg.lookup("springDamping");
	springDistance			= cfg.lookup("springDistance");
	springMaxDistance		= springDistance * 1.25;
	springMaxConnections	= cfg.lookup("springMaxConnections");
	backgroundFrictionForce	= cfg.lookup("backgroundFrictionForce");
	cubicPixelMass			= cfg.lookup("cubicPixelMass");
	bumpSpeed				= cfg.lookup("bumpSpeed");
	gravityIncrement		= cfg.lookup("gravityIncrement");
	defaultRestitution		= cfg.lookup("defaultRestitution");
	defaultFriction			= cfg.lookup("defaultFriction");

}

void Simulation::close() {
	deleteAllObjects();
}

void Simulation::render() {
	mainWindow.setView(view);
	mainWindow.clear(sf::Color::Black);
	drawSprings();
	for(SimObject* object: objects) {
		object->render();
	}
	mainWindow.setView(mainWindow.getDefaultView());
	if(uiEnabled) {
		drawUIText();
	}
	mainWindow.display();
	mainWindow.setView(view);
}

void Simulation::drawSprings() {
	if(!springsEnabled) return;
	for(SimObject* object1: objects) {
		for(SimObject* object2: objects) {
			if(std::find(object1->springConnections.begin(), object1->springConnections.end(),
				object2) != object1->springConnections.end()) {
				int opacity;
				double distance = SimObject::distanceBetween(object1, object2);
				if(distance > springMaxDistance) {
					opacity = 0;
				} else {
					double k = -255 / springMaxDistance;
					double b = -k * springMaxDistance;
					opacity = (int)(k * distance + b);
				}
				int Hue = (int)utils::mapRange(opacity, 0, 255, 120, 0);
				sf::Color springColor = utils::HSVtoRGB(Hue, 100, 100);
				sf::VertexArray lines(sf::LineStrip, 2);
				lines[0].position = sf::Vector2f(object1->getX(), object1->getY());
				lines[1].position = sf::Vector2f(object2->getX(), object2->getY());
				lines[0].color = sf::Color(springColor.r, springColor.g, springColor.b, opacity);
				lines[1].color = sf::Color(springColor.r, springColor.g, springColor.b, opacity);
				mainWindow.draw(lines);
			}
		}
	}
}

void Simulation::drawUIText() {

	textDrawOffset = 0;
	currentFontSize = FONT_SIZE_NORMAL;
	currentTextColor = {255, 255, 0};

	drawText(0, 0,				  WINDOW_SNAP_H_LEFT | WINDOW_SNAP_V_TOP, "fps: "  + std::to_string(fps));
	drawText(0, FONT_SIZE_NORMAL, WINDOW_SNAP_H_LEFT,					  "time: " + utils::toString(time, 1));
	std::string str = "Objects: " + std::to_string(objects.size());
	drawText(0, 0,				  WINDOW_SNAP_H_CENTER | WINDOW_SNAP_V_TOP, str);

	switch(collisionType) {
		case COLLISION_TYPE_BOUNCE: str = "bounce"; break;
		case COLLISION_TYPE_MERGE:  str = "merge";	break;
		default:					str = "?";		break;
	}
	drawOption("Collisions(" + str + ") (1)", &collisionsEnabled);
	drawOption("Gravity radial (2)", &gravityRadialEnabled);
	drawOption("Gravity vertical (3)", &gravityVerticalEnabled);
	drawOption("Background friction (4)", &backgroundFrictionEnabled);
	drawOption("Springs (5)", &springsEnabled);
	drawBlank();

	currentTextColor = {255, 255, 0};
	drawInfo("Simulation speed: " + std::to_string(simulationSpeed) + " (" + std::to_string((int)simulationSpeedExponent) + ")");
	drawBlank();

	drawText(300, 0, WINDOW_SNAP_V_TOP, "Zoom: " + utils::toString(mainWindow.getDefaultView().getSize().x / view.getSize().x, 10));

	currentFontSize = FONT_SIZE_SMALL;
	drawInfo("RadialG: ", &gravityRadialForce);
	drawInfo("VerticalG: ", &gravityVerticalForce);
	drawInfo("DefRest: ", &defaultRestitution);

	if(pause) {
		currentFontSize = FONT_SIZE_BIG;
		drawText(0, 0, WINDOW_SNAP_H_CENTER | WINDOW_SNAP_V_CENTER, "PAUSE");
	}

}

void Simulation::drawOption(std::string text, bool* option) {
	currentTextColor = getBoolColor(*option);
	drawText(0, textDrawOffset, WINDOW_SNAP_H_RIGHT, text);
	textDrawOffset += currentFontSize;
}

void Simulation::drawInfo(std::string text) {
	drawText(0, textDrawOffset, WINDOW_SNAP_H_RIGHT, text);
	textDrawOffset += currentFontSize;
}

void Simulation::drawInfo(std::string text, double* parameter) {
	drawInfo(text + std::to_string(*parameter));
}

void Simulation::drawBlank() {
	textDrawOffset += currentFontSize;
}


void Simulation::handleEvents() {
	sf::Event event;
	while(mainWindow.pollEvent(event)) {
		switch(event.type) {

			case sf::Event::Closed:					exit(EXIT_SUCCESS);			break;

			case sf::Event::KeyPressed:
			case sf::Event::KeyReleased:			handleKeyboard(event);		break;

			case sf::Event::MouseButtonPressed: 
			case sf::Event::MouseButtonReleased:
			case sf::Event::MouseMoved: 
			case sf::Event::MouseWheelScrolled:		handleMouse(event);			break;

		}
	}
}

void Simulation::handleKeyboard(sf::Event event) {
	if(event.type == sf::Event::KeyPressed) {
		switch(event.key.code) {
			case sf::Keyboard::Escape:		exit(EXIT_SUCCESS);										break;
			case sf::Keyboard::Space:		pause = !pause;											break;
			case sf::Keyboard::Num1:		collisionsEnabled = !collisionsEnabled;					break;
			case sf::Keyboard::Num2:		gravityRadialEnabled = !gravityRadialEnabled;			break;
			case sf::Keyboard::Num3:		gravityVerticalEnabled = !gravityVerticalEnabled;		break;
			case sf::Keyboard::Num4:		backgroundFrictionEnabled = !backgroundFrictionEnabled; break;
			case sf::Keyboard::Num5:		springsEnabled = !springsEnabled;						break;
			case sf::Keyboard::Add:			changeSimulationSpeed(1);								break;
			case sf::Keyboard::Subtract:	changeSimulationSpeed(-1);								break;
			case sf::Keyboard::F2:			uiEnabled = !uiEnabled;									break;
			case sf::Keyboard::C:			nextCollisionType();									break;
			case sf::Keyboard::Up:			bumpAll(0, -bumpSpeed);									break;
			case sf::Keyboard::Down:		bumpAll(0,  bumpSpeed);									break;
			case sf::Keyboard::Left:		bumpAll(-bumpSpeed, 0);									break;
			case sf::Keyboard::Right:		bumpAll( bumpSpeed, 0);									break;
			case sf::Keyboard::LBracket:	gravityRadialForce -= gravityIncrement;					break;
			case sf::Keyboard::RBracket:	gravityRadialForce += gravityIncrement;					break;
		}
	}
}

void Simulation::handleMouse(sf::Event event) {
	if(event.type == sf::Event::MouseButtonPressed) {
		if(event.mouseButton.button == sf::Mouse::Button::Middle) {
			mousePrev = mainWindow.mapPixelToCoords(sf::Mouse::getPosition(mainWindow), mainWindow.getDefaultView());
			isWheelDown = true;
		}
	}
	if(event.type == sf::Event::MouseButtonReleased) {
		if(event.mouseButton.button == sf::Mouse::Button::Middle) {
			isWheelDown = false;
		}
	}
	if(event.type == sf::Event::MouseMoved) {
		if(isWheelDown) {
			sf::Vector2f mousePos = mainWindow.mapPixelToCoords(sf::Mouse::getPosition(mainWindow), mainWindow.getDefaultView());
			sf::Vector2f delta(mousePrev.x - mousePos.x, mousePrev.y - mousePos.y);
			view.setCenter(view.getCenter() + delta*(view.getSize().x/mainWindow.getDefaultView().getSize().x));
			mousePrev = mousePos;
		}
	}
	if(event.type == sf::Event::MouseWheelScrolled) {
		view.zoom(pow(mouseWheelzoomFactor, -event.mouseWheelScroll.delta));
	}
}

void Simulation::processPhysics() {
	if(pause) return;
	deleteMarked();
	processGravity();
	processSprings();
	if(gravityVerticalEnabled) {
		dynamicsWorld->setGravity(btVector3(0, gravityVerticalForce, 0));
	} else {
		dynamicsWorld->setGravity(btVector3(0, 0, 0));
	}
	dynamicsWorld->stepSimulation(simulationSpeed * SECONDS_PER_FRAME, 1000);
	time += simulationSpeed * SECONDS_PER_FRAME;
}

void Simulation::deleteMarked() {
	int i = 0;
	while(i < (int)objects.size()) {
		if(objects.at(i)->isMarkedForDeletion) {
			deleteObject(objects.at(i));
		} else {
			i++;
		}
	}
}

void Simulation::processGravity() {
	if(gravityRadialEnabled) {
		for(SimObject* object1: objects) {
			for(SimObject* object2: objects) {
				if(object1 == object2) continue;
				object1->calculateGravity(object2, simulationSpeed, gravityRadialForce);
			}
		}
	}
}

void Simulation::processSprings() {
	if(springsEnabled) {
		for(SimObject* object1: objects) {
			if(object1->springConnections.size() >= springMaxConnections) continue;
			for(SimObject* object2: objects) {
				if(object1 == object2) continue;
				if(!object1->isActive && !object2->isActive) continue;
				if(object1->springConnections.size() >= springMaxConnections) break;
				if(object2->incomingSpringConnectionsCount >= springMaxConnections) continue;
				if(std::find(object1->springConnections.begin(), object1->springConnections.end(),
					object2) != object1->springConnections.end()) continue;
				if((springDistance > 0) && (SimObject::distanceBetween(object1, object2) < springDistance)) {
					object1->springConnections.push_back(object2);
					object2->incomingSpringConnectionsCount++;
				}
			}
		}
		for(SimObject* object: objects) {
			for(int j = object->springConnections.size() - 1; j >= 0; j--) {
				object->calculateSprings(object->springConnections.at(j),
					simulationSpeed, springMaxDistance, springDistance, springDamping, springForce);
			}
		}
	}
}

void Simulation::drawText(int x, int y, int snap, std::string str) {
	sf::Text text(str, font, currentFontSize);
	text.setFillColor(currentTextColor);
	if(snap == WINDOW_SNAP_OFF) {
		x = 0;
		y = 0;
	}
	if(snap & WINDOW_SNAP_H_LEFT) {
		x = 0;
	}
	if(snap & WINDOW_SNAP_H_CENTER) {
		x = mainWindow.getSize().x / 2 - text.getLocalBounds().width / 2;
	}
	if(snap & WINDOW_SNAP_H_RIGHT) {
		x = mainWindow.getSize().x - text.getLocalBounds().width;
	}
	if(snap & WINDOW_SNAP_V_TOP) {
		y = 0;
	}
	if(snap & WINDOW_SNAP_V_CENTER) {
		y = mainWindow.getSize().y / 2 - text.getLocalBounds().height / 2;
	}
	if(snap & WINDOW_SNAP_V_BOTTOM) {
		y = mainWindow.getSize().y - text.getLocalBounds().height;
	}
	text.setPosition(x, y);
	mainWindow.draw(text);
}

sf::Color Simulation::getBoolColor(bool var) {
	if(var)
		return { 0, 255, 0 };
	else
		return { 255, 0, 0 };
}

void Simulation::updateFpsCount() {
	int millisecondsPassed = clock.getElapsedTime().asMilliseconds();
	fpsCount++;
	if(millisecondsPassed > 1000) {
		fps = fpsCount;
		fpsCount = 0;
		clock.restart();
	}
}

Ball* Simulation::addBall(double x, double y, double radius, double speedX, double speedY, sf::Color color, bool isActive) {
	Ball* ball = new Ball(x, y, radius, speedX, speedY, color, isActive);
	objects.push_back(ball);
	ball->addToRigidBodyWorld(dynamicsWorld);
	return ball;
}

Polygon* Simulation::addPolygon(double x, double y, double speedX, double speedY, std::vector<Point> points, sf::Color color, bool isActive) {
	Polygon* polygon = new Polygon(x, y, speedX, speedY, points, color, isActive);
	objects.push_back(polygon);
	polygon->addToRigidBodyWorld(dynamicsWorld);
	return polygon;
}

void Simulation::addPolygonVehicle(double x, double y, double speedX, double speedY, std::vector<Point> points, std::vector<Wheel> wheels, sf::Color color) {
	Polygon* newPolygon = addPolygon(x, y, speedX, speedY, points, color);
	for(Wheel wheel: wheels) {
		Ball* newWheel = addBall(x + wheel.position.x, y + wheel.position.y, wheel.radius, speedX, speedY, color);
		addSpringConstraint(newPolygon, newWheel);
	}
}

Polygon* Simulation::addRandomPolygon(double x, double y, double speedX, double speedY, int vertexCount, double minLength, double maxLength, sf::Color color, bool isActive) {
	std::vector<Point> points = utils::generateRandomTriangleFan(vertexCount, minLength, maxLength);
	return addPolygon(x, y, speedX, speedY, points, color, isActive);
}

void Simulation::addTrack(int pointCount, double distanceBetweenPoints, double thickness, double bottomLimit, double topLimit) {
	Track* track = new Track(pointCount, distanceBetweenPoints, thickness, bottomLimit, topLimit);
	objects.push_back(track);
	track->addToRigidBodyWorld(dynamicsWorld);
}

void Simulation::addPlane(Plane::PlaneSide side) {
	Plane* plane = new Plane(side);
	planes.push_back(plane);
	plane->addToRigidBodyWorld(dynamicsWorld);
}

void Simulation::changeSimulationSpeed(int change) {
	simulationSpeedExponent += change;
	simulationSpeed = pow(SIMULATION_SPEED_BASE, simulationSpeedExponent);
}

void Simulation::nextCollisionType() {
	collisionType = (CollisionType)((collisionType+1) % COLLISION_TYPES_NUM);	
}

void Simulation::checkExitCondition() {
	if(exitContidionFunction(this))
		exitRequest = true;
}

void Simulation::bumpAll(double velX, double velY) {
	for(SimObject* obj: objects) {
		obj->setVelX(obj->getVelX() + velX);
		obj->setVelY(obj->getVelY() + velY);
	}
}

void Simulation::resetSimulation() {
	exitRequest = false;
	time = 0;
	deleteAllObjects();
}


void Simulation::deleteAllObjects() {
	for(SimObject* object: objects)
		delete object;
	objects.clear();
}

void Simulation::deleteObject(SimObject* object) {
	delete object;
	objects.erase(std::remove(objects.begin(), objects.end(), object), objects.end());
}

void Simulation::addSphericalConstraint(SimObject* object1, SimObject* object2) {
	btPoint2PointConstraint* constraint = new btPoint2PointConstraint(
		*object1->getRigidBody(), *object2->getRigidBody(), btVector3(object2->getX() - object1->getX(), object2->getY() - object1->getY(), 0), btVector3(0, 0, 0));
	dynamicsWorld->addConstraint(constraint, false);
}

void Simulation::addHingeConstraint(SimObject* object1, SimObject* object2) {
	btHingeConstraint* constraint = new btHingeConstraint(
		*object1->getRigidBody(), *object2->getRigidBody(),
		btVector3(object2->getX() - object1->getX(), object2->getY() - object1->getY(), 0), btVector3(0, 0, 0), btVector3(0, 0, 1), btVector3(0, 0, 1));
	constraint->enableAngularMotor(true, -10, 50);
	dynamicsWorld->addConstraint(constraint, true);
}

void Simulation::addSpringConstraint(SimObject* object1, SimObject* object2) {
	btTransform tr1, tr2;
	tr1.setIdentity();
	tr2.setIdentity();
	tr1.setOrigin(btVector3(object2->getX() - object1->getX(), object2->getY() - object1->getY(), 0));
	btGeneric6DofSpring2Constraint* constraint = new btGeneric6DofSpring2Constraint(*object1->getRigidBody(), *object2->getRigidBody(), tr1, tr2);
	constraint->enableSpring(0, true);
	constraint->setStiffness(0, 10000);
	constraint->setDamping(0, 50);
	constraint->enableSpring(1, true);
	constraint->setStiffness(1, 1000);
	constraint->setDamping(1, 5);
	constraint->setLinearLowerLimit(btVector3(-1000, -1000, 0));
	constraint->setLinearUpperLimit(btVector3(1000, 1000, 0));
	constraint->enableMotor(5, true);
	constraint->setMaxMotorForce(5, 100);
	constraint->setTargetVelocity(5, -10);
	dynamicsWorld->addConstraint(constraint, true);
}
