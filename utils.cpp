#include "utils.h"
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <random>

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

	double distance(double x1, double x2, double y1, double y2) {
		return sqrt(pow(x1-x2, 2) + pow(y1-y2, 2));
	}

	double randomBetween(double min, double max) {
		return random() * (max - min) + min;
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