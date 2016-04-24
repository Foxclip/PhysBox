#include "simulation.h"

int main(int argc, char* args[]) {

	Simulation simulation([](Simulation* sim) {
		return sim->objects.size() <= 3;
	});
	simulation.collisionsEnabled = true;
	simulation.gravityRadialEnabled = true;
	simulation.collisionType = COLLISION_TYPE_MERGE;
	//for(int i = 0; i < 3; i++) {
	while(true) {
		for(int i = 0; i < 100; i++) {
			simulation.addBall(
				utils::randomBetween(0, mainWindow.getWidth()),
				utils::randomBetween(0, mainWindow.getHeight()),
				utils::nonLinearRandomBetween(1, 10, [](double x) { return pow(x, 2); }),
				0, //utils::randomBetween(-1, 1),
				0, //utils::randomBetween(-1, 1),
				utils::randomColor()
				);
		}
			//simulation.generateSystem(mainWindow.getWidth() / 2, mainWindow.getHeight() / 2,
			//					30, //central body radius
			//					5,  //moon radius
			//					5, //moon count
			//					100  //gap
			//);
		simulation.runSimulation();
	}

	return 0;
}