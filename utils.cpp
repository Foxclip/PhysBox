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

	Color randomColor() {
		return{ (unsigned char)(random()*256), (unsigned char)(random()*256), (unsigned char)(random()*256) };
	}

	std::string toString(double var, int precision) {
		std::ostringstream strs;
		strs << std::fixed << std::setprecision(precision) << var;
		return strs.str();
	}

	Color HSVtoRGB(int H, int S, int V) {
		Color rgb = { 255, 255, 255 };
		int Hi = (int)std::floor(H / 60) % 6;
		int Vmin = ((100 - S) * V) / 100;
		int a = (V - Vmin) * (H % 60) / 60;
		int Vinc = Vmin + a;
		int Vdec = V - a;
		switch(Hi) {
			case 0: rgb.red = V;	rgb.green = Vinc;	rgb.blue = Vmin;	break;
			case 1: rgb.red = Vdec; rgb.green = V;		rgb.blue = Vmin;	break;
			case 2: rgb.red = Vmin; rgb.green = V;		rgb.blue = Vinc;	break;
			case 3: rgb.red = Vmin; rgb.green = Vdec;	rgb.blue = V;		break;
			case 4: rgb.red = Vinc; rgb.green = Vmin;	rgb.blue = V;		break;
			case 5: rgb.red = V;	rgb.green = Vmin;	rgb.blue = Vdec;	break;
		}
		return rgb;
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

	double randomBetween(double min, double max) {
		return random() * (max - min) + min;
	}

	double nonLinearRandomBetween(double min, double max, std::function<double(double)> f) {
		return f(random()) * (max - min) + min;
	}

	Font::Font() {
		mFont = NULL;
		mSize = 0;
	}

	Font::~Font() {
		free();
	}

	bool Font::loadFont(std::string path, int size) {
		TTF_Font* loadedFont = TTF_OpenFont(path.c_str(), size);
		if(!loadedFont) {
			printf("Font loading error: %s\n", TTF_GetError());
			return false;
		}
		mFont = loadedFont;
		mSize = size;
		return true;
	}

	TTF_Font* Font::getSDLFont() {
		return mFont;
	}

	int Font::getSize() {
		return mSize;
	}

	void Font::free() {
		mFont = NULL;
		mSize = 0;
	}

}