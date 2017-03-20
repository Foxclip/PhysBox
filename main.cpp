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
		for(int i = 0; i < 10; i++) {
			simulation.addRandomPolygon(utils::randomBetween(0, 1000), utils::randomBetween(-200, 200), 0, 0, 8, 10, 50, utils::randomColor());
		}
		simulation.addTrack(100, 200, 50, -200, 200);
		simulation.runSimulation();
	}

	return 0;
}