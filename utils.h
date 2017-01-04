#pragma once

#include <string>
#include <functional>
#include <SFML/Graphics.hpp>
#include "globals.h"

typedef struct _point {
	double x, y;
} Point;

namespace utils {

	double randomBetween(double min, double max);
	double nonLinearRandomBetween(double min, double max, std::function<double(double)> f);
	double random();
	sf::Color randomColor();
	sf::Color randomColorBetween(int min, int max);
	sf::Color randomHSVColor(int S, int V);
	sf::Color HSVtoRGB(int H, int S, int V);
	std::string toString(double var, int precision);
	double mapRange(double val, double min1, double max1, double min2, double max2, bool clamp = false);
	double quaternionToEulerZ(double x, double y, double z, double w);
	Point polarToCartesian(double angle, double radius);

}