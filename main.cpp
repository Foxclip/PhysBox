#include "simulation.h"

int main(int argc, char* args[]) {

	Simulation simulation([](Simulation* sim) {
		return false;
	});
	simulation.collisionsEnabled = true;
	libconfig::Config cfg;
	cfg.readFile("startup.cfg");
	int numberOfObjects = cfg.lookup("numberOfObjects");
	while(true) {
		simulation.resetSimulation();
		for(int i = 0; i < numberOfObjects; i++) {
			simulation.addBall(
								utils::randomBetween(0, mainWindow.getWidth()),
								utils::randomBetween(0, mainWindow.getHeight()),
								2.5,
								utils::randomBetween(-1, 1),
								utils::randomBetween(-1, 1),
								utils::randomColor()
							  );
		}
		simulation.runSimulation();
	}

	return 0;
}