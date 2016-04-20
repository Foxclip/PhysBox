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
	simulation.addBall(600, 300, 30, 1, 0, { 255, 0, 0 });
	simulation.addBall(700, 300, 30, -1, 0, { 0, 0, 255 });

	//TODO timers, condition function lists, object count

	simulation.runSimulation();

	return 0;
}