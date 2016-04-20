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
	for(int i = 0; i < 100; i++) {
		simulation.addBall(
			utils::randomBetween(0, mainWindow.getWidth()),
			utils::randomBetween(0, mainWindow.getHeight()),
			utils::nonLinearRandomBetween(1, 10, [](double x) { return x; }),
			utils::randomBetween(-1, 1),
			utils::randomBetween(-1, 1),
			utils::randomColor()
		);
	}
	//simulation.addBall(600, 300, 30, 1, 0, { 255, 0, 0 });
	//simulation.addBall(700, 300, 30, -1, 0, { 0, 0, 255 });

	//TODO timers, condition function lists, object count

	simulation.runSimulation();

	return 0;
}