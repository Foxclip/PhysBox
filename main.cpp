#include "simulation.h"

int main(int argc, char* args[]) {

	Simulation simulation([](Simulation* sim) {
		return sim->objects.size() <= 3;
	});
	simulation.collisionsEnabled = true;
	simulation.gravityRadialEnabled = true;
	simulation.collisionType = COLLISION_TYPE_MERGE;
	while(true) {
		simulation.resetSimulation();
		for(int i = 0; i < 100; i++) {
			simulation.addBall(
								utils::randomBetween(0, mainWindow.getWidth()),
								utils::randomBetween(0, mainWindow.getHeight()),
								utils::nonLinearRandomBetween(1, 10, [](double x) { return pow(x, 2); }),
								0,
								0,
								utils::randomColor()
							  );
		}
		simulation.runSimulation();
	}

	return 0;
}