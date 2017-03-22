#include "simulation.h"

int main(int argc, char* args[]) {

	Simulation simulation([](Simulation* sim) {
		return false;
	});
	libconfig::Config cfg;
	cfg.readFile("startup.cfg");
	int numberOfObjects = cfg.lookup("numberOfObjects");

	while(true) {
		simulation.resetSimulation();
		simulation.addTrack(100, 200, 50, -100, 100);

		//std::vector<Point> points;
		//points.push_back({-100, -50});
		//points.push_back({100, -50});
		//points.push_back({100, 50});
		//points.push_back({-100, 50});
		//std::vector<Wheel> wheels;
		//wheels.push_back({50, {-100, 100}});
		//wheels.push_back({50, {100, 100}});
		//simulation.addPolygonVehicle(600, 0, 0, 0, points, wheels, sf::Color::Green);
		//simulation.addPolygonVehicle(500, 300, 0, 0, points, wheels, sf::Color::Blue);

		//Polygon* polygon1 = simulation.addRandomPolygon(500, -200, 0, 0, 8, 10, 50, sf::Color::Red);
		//Polygon* polygon2 = simulation.addRandomPolygon(1000, 0, 0, 0, 8, 10, 50, sf::Color::Green);
		//Polygon* polygon3 = simulation.addRandomPolygon(1500, 0, 0, 0, 8, 10, 50, sf::Color::Blue);
		////simulation.addSphericalConstraint(polygon1, polygon2);
		////simulation.addSphericalConstraint(polygon2, polygon3);
		////simulation.addSphericalConstraint(polygon3, polygon1);
		//simulation.addHingeConstraint(polygon1, polygon2);
		//simulation.addSpringConstraint(polygon1, polygon2);

		for(int i = 0; i < 100; i++) {
			simulation.addRandomPolygonVehicle(500, 0, 0, 0, 8, 10, 50, 0, 5, utils::randomColor());
		}

		simulation.runSimulation();
	}

	return 0;
}