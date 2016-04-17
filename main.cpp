#include "simulation.h"

int main(int argc, char* args[]) {

	Simulation simulation;
	simulation.addBall(100, 100, 10, 5, 5, { 255, 0, 0 });
	simulation.addBall(500, 100, 100, 5, 5, { 0, 0, 255 });
	simulation.runSimulation();

	return 0;
}