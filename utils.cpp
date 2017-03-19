#include "utils.h"
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <random>
#include <sstream>
#include <ios>
#include <iomanip>

std::random_device rd;
std::mt19937 mt(rd());
std::uniform_real_distribution<double> dist(0.0, 1.0);

namespace utils {

	double random() {
		return dist(mt);
	}

	sf::Color randomColor() {
		return { (unsigned char)(random()*256),
				 (unsigned char)(random()*256),
				 (unsigned char)(random()*256)  };
	}

	sf::Color randomColorBetween(int min, int max) {
		return { (unsigned char)(randomBetween(min/256.0, max/256.0)*256),
				 (unsigned char)(randomBetween(min/256.0, max/256.0)*256),
				 (unsigned char)(randomBetween(min/256.0, max/256.0)*256)  };
	}

	sf::Color randomHSVColor(int S, int V) {
		return HSVtoRGB((int)randomBetween(0, 360), S, V);
	}

	sf::Color HSVtoRGB(int H, int S, int V) {
		sf::Color rgb = { 255, 255, 255 };
		V *= 2;
		int Hi = (int)std::floor(H / 60) % 6;
		int Vmin = ((100 - S) * V) / 100;
		int a = (V - Vmin) * (H % 60) / 60;
		int Vinc = Vmin + a;
		int Vdec = V - a;
		switch(Hi) {
			case 0: rgb.r = V;	  rgb.g = Vinc;	rgb.b = Vmin;	break;
			case 1: rgb.r = Vdec; rgb.g = V;	rgb.b = Vmin;	break;
			case 2: rgb.r = Vmin; rgb.g = V;	rgb.b = Vinc;	break;
			case 3: rgb.r = Vmin; rgb.g = Vdec;	rgb.b = V;		break;
			case 4: rgb.r = Vinc; rgb.g = Vmin;	rgb.b = V;		break;
			case 5: rgb.r = V;	  rgb.g = Vmin;	rgb.b = Vdec;	break;
		}
		return rgb;
	}

	std::string toString(double var, int precision) {
		std::ostringstream strs;
		strs << std::fixed << std::setprecision(precision) << var;
		return strs.str();
	}

	double mapRange(double val, double min1, double max1, double min2, double max2, bool clamp) {
		double range = max1 - min1;
		if(range == 0) return 0;
		double scaledRange = max2 - min2;
		double scale = scaledRange / range;
		double dist = val - min1;
		double scaledDist = dist * scale;
		double result = min2 + scaledDist;
		if(clamp) {
			if(result < min2) result = min2;
			if(result > max2) result = max2;
		}
		return result;
	}

double quaternionToEulerZ(double x, double y, double z, double w) {
	double ysqr = y * y;
	double t0 = -2.0f * (ysqr + z * z) + 1.0f;
	double t1 = +2.0f * (x * y - w * z);
	return std::atan2(t0, t1);
}

Point polarToCartesian(double angle, double radius) {
	angle *= PI / 180;
	return {
		radius * std::cos(angle),
		radius * std::sin(angle)
	};
}

std::vector<Point> generateRandomTriangleFan(int vertexCount, double minLength, double maxLength) {
	std::vector<Point> points;
	for(int i = 0; i < vertexCount; i++) {
		points.push_back(utils::polarToCartesian(i * 360 / vertexCount, utils::randomBetween(minLength, maxLength)));
	}
	return points;
}

	double randomBetween(double min, double max) {
		return random() * (max - min) + min;
	}

	double nonLinearRandomBetween(double min, double max, std::function<double(double)> f) {
		return f(random()) * (max - min) + min;
	}

}