#pragma once

static bool collisionsEnabled = false;
static bool gravityRadialEnabled = false;
static bool gravityVerticalEnabled = false;
static bool backgroundFrictionEnabled = false;
static bool springsEnabled = true;

static double gravityVerticalForce = 0.1;
static double gravityRadialForce = 0.1;
static double springForce = 10;
static double springDamping = 0.5;
static double springDistance = 50;
static double springInitialDistance = 0;
static double springMaxDistance = springDistance * 1.25;
static double springMaxConnections = 6;
static double backgroundFrictionForce = 1;
static double cubucPixelMass = 0.001;
static const double DAMPING = 1;
static const double INITIAL_BALLS_NUMBER = 500;

static bool variableTimestep = true;
static const double SIMULATION_SPEED_BASE = 4;
static double simulationSpeedExponent = 0;
static bool pause = true;