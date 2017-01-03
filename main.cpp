#include "simulation.h"

int main(int argc, char* args[]) {

	Simulation simulation([](Simulation* sim) {
		return false;
	});
	libconfig::Config cfg;
	cfg.readFile("startup.cfg");
	int numberOfObjects = cfg.lookup("numberOfObjects");
	double radius = cfg.lookup("radius");
	while(true) {
		simulation.resetSimulation();
		for(int i = 0; i < numberOfObjects; i++) {
			simulation.addBall(
								utils::randomBetween(0, mainWindow.getSize().x),
								utils::randomBetween(0, mainWindow.getSize().y),
								radius,
								utils::randomBetween(0, 0),
								utils::randomBetween(0, 0),
								utils::randomHSVColor(100, 100)
							  );
		}

		std::vector<Point> points;
		points.push_back({-5, -5});
		points.push_back({-5,  5});
		points.push_back({ 5,  5});
		points.push_back({ 5, -5});
		simulation.addPolygon(500, 500, 0, 0, points, sf::Color::White, true);

		simulation.addPlane(Plane::POS_TOP);
		simulation.addPlane(Plane::POS_BOTTOM);
		simulation.addPlane(Plane::POS_LEFT);
		simulation.addPlane(Plane::POS_RIGHT);

		simulation.runSimulation();

	}

	return 0;
}