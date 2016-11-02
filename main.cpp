#include "simulation.h"

int main(int argc, char* args[]) {

	Simulation simulation([](Simulation* sim) {
		return false; //sim->objects.size() <= 3;
	});
	simulation.collisionsEnabled = true;
	//simulation.gravityRadialEnabled = true;
	//simulation.gravityVerticalEnabled = true;
	while(true) {
		simulation.resetSimulation();
		for(int i = 0; i < 200; i++) {
			simulation.addBall(
								utils::randomBetween(0, mainWindow.getWidth()),
								utils::randomBetween(0, mainWindow.getHeight()),
								//utils::nonLinearRandomBetween(1, 10, [](double x) { return pow(x, 2); }),
								2.5,
								utils::randomBetween(-1, 1),
								utils::randomBetween(-1, 1),
								utils::randomColor()
								//utils::random() > 0.5
							  );
		}
		simulation.runSimulation();
	}

	return 0;
}