#include "simulation.h"

void generateRandomBalls(Simulation* simulation);

int main(int argc, char* args[]) {

	Simulation simulation;
	simulation.generateSystem(mainWindow.getWidth() / 2, mainWindow.getHeight() / 2, 30, 5, 5, 50);
	simulation.gravityRadialEnabled = true;

	//TODO timers, condition function lists, object count

	simulation.runSimulation();

	return 0;
}

void generateRandomBalls(Simulation* simulation) {
	for(int i = 0; i < 100; i++) {
		simulation->addBall(
			utils::randomBetween(0, mainWindow.getWidth()),
			utils::randomBetween(0, mainWindow.getHeight()),
			10,
			utils::randomBetween(-1, 1),
			utils::randomBetween(-1, 1),
			utils::randomColor()
			);
	}
}