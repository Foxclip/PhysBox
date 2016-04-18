#include "simulation.h"

int main(int argc, char* args[]) {

	Simulation simulation;
	simulation.addBall(100, 100, 10, 5, 5, { 255, 0, 0 });
	simulation.addBall(500, 100, 100, 5, 5, { 0, 0, 255 });
	simulation.deleteAllObjects();

	for(int i = 0; i < 100; i++) {
		simulation.addBall(
			utils::randomBetween(0, mainWindow.getWidth()),
			utils::randomBetween(0, mainWindow.getHeight()),
			10,
			utils::randomBetween(-1, 1),
			utils::randomBetween(-1, 1),
			utils::randomColor()
			);
	}

	simulation.runSimulation();

	return 0;
}