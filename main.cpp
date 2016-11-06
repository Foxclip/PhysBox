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
		for(int i = 0; i < numberOfObjects; i++) {
			simulation.addBall(
								utils::randomBetween(0, mainWindow.getWidth()),
								utils::randomBetween(0, mainWindow.getHeight()),
								5,
								utils::randomBetween(0, 0),
								utils::randomBetween(0, 0),
								utils::randomColor()
							  );
		}
		//simulation.addBall(500, 500, 20, 10, 0, {255, 0, 0});
		simulation.addPlane(Plane::POS_LEFT);
		simulation.addPlane(Plane::POS_RIGHT);
		simulation.addPlane(Plane::POS_TOP);
		simulation.addPlane(Plane::POS_BOTTOM);
		simulation.runSimulation();
	}

	return 0;
}