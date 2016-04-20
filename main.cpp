#include "simulation.h"

int main(int argc, char* args[]) {

	Simulation simulation;
	simulation.generateSystem(mainWindow.getWidth() / 2, mainWindow.getHeight() / 2, 30, 5, 5, 50);
	simulation.gravityRadialEnabled = true;

	//TODO timers, condition function lists, object count

	simulation.runSimulation();

	return 0;
}