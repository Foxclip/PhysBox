#include "simulation.h"

int main(int argc, char* args[]) {

	Simulation simulation;
	//simulation.generateSystem(mainWindow.getWidth() / 2, mainWindow.getHeight() / 2,
	//							10, //central body radius
	//							1,  //moon radius
	//							30, //moon count
	//							10  //gap
	//);
	simulation.collisionsEnabled = true;
	simulation.gravityRadialEnabled = true;
	simulation.collisionType = COLLISION_TYPE_MERGE;
	for(int i = 0; i < 3; i++) {
		for(int i = 0; i < 100; i++) {
			simulation.addBall(
				utils::randomBetween(0, mainWindow.getWidth()),
				utils::randomBetween(0, mainWindow.getHeight()),
				utils::nonLinearRandomBetween(1, 10, [](double x) { return x; }),
				0, //utils::randomBetween(-1, 1),
				0, //utils::randomBetween(-1, 1),
				utils::randomColor()
				);
		}
		if(simulation.runSimulation()) {
			break;
		}
	}

	return 0;
}