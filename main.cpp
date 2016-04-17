#include "simulation.h"

int main(int argc, char* args[]) {

	initSimulation();
	addBall(100, 100, 10, 5, 5, { 255, 0, 0 });
	addBall(500, 100, 100, 5, 5, { 0, 0, 255 });
	runSimulation();

	return 0;
}