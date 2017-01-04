#include "simulation.h"

std::vector<Point> generateRandomTriangleFan(int vertexCount, double minLength, double maxLength);

int main(int argc, char* args[]) {

	Simulation simulation([](Simulation* sim) {
		return false;
	});
	libconfig::Config cfg;
	cfg.readFile("startup.cfg");
	int numberOfObjects = cfg.lookup("numberOfObjects");

	while(true) {
		simulation.resetSimulation();
		std::vector<Point> points = generateRandomTriangleFan(8, 10, 50);
		simulation.addPolygon(
			500,
			0,
			0,
			0,
			points,
			utils::randomColor()
		);
		simulation.addTrack(10, 200, 50, -20, 20);
		simulation.runSimulation();
	}

	return 0;
}

std::vector<Point> generateRandomTriangleFan(int vertexCount, double minLength, double maxLength) {
	std::vector<Point> points;
	for(int i = 0; i < vertexCount; i++) {
		points.push_back(utils::polarToCartesian(i * 360 / vertexCount, utils::randomBetween(minLength, maxLength)));
	}
	return points;
}