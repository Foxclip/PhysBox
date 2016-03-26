#pragma once

typedef struct Color {
	unsigned char red, green, blue;
} Color;

double randomBetween(double min, double max);
double random();
Color randomColor();