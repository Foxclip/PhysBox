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
			std::vector<Point> points;
			points.push_back({-5, -5});
			points.push_back({-5,  5});
			points.push_back({ 5,  5});
			points.push_back({ 5, -5});
			points.push_back({ 0,  0});
			simulation.addPolygon(
				utils::randomBetween(0, mainWindow.getSize().x),
				utils::randomBetween(0, mainWindow.getSize().y),
				utils::randomBetween(0, 0),
				utils::randomBetween(0, 0),
				points,
				utils::randomHSVColor(100, 100)
			);
		}

		//simulation.addPlane(Plane::POS_TOP);
		//simulation.addPlane(Plane::POS_BOTTOM);
		//simulation.addPlane(Plane::POS_LEFT);
		//simulation.addPlane(Plane::POS_RIGHT);
		simulation.addTrack(10, 200, 50, -20, 20);

		simulation.runSimulation();

	}

	return 0;
}