#include "utils.h"
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <random>

std::random_device rd;
std::mt19937 mt(rd());
std::uniform_real_distribution<double> dist(0.0, 1.0);

double utils::random() {
	return dist(mt);
}

utils::Color utils::randomColor() {
	return { (unsigned char)(random()*256), (unsigned char)(random()*256), (unsigned char)(random()*256) };
}

double utils::distance(double x1, double x2, double y1, double y2) {
	return sqrt(pow(x1-x2, 2) + pow(y1-y2, 2));
}

double utils::randomBetween(double min, double max) {
	return random() * (max - min) + min;
}

utils::Font::Font() {
	mFont = NULL;
	mSize = 0;
}

utils::Font::~Font() {
	free();
}

bool utils::Font::loadFont(std::string path, int size) {
	TTF_Font* loadedFont = TTF_OpenFont(path.c_str(), size);
	if(!loadedFont) {
		printf("Font loading error: %s\n", TTF_GetError());
		return false;
	}
	mFont = loadedFont;
	mSize = size;
	return true;
}

TTF_Font* utils::Font::getSDLFont() {
	return mFont;
}

int utils::Font::getSize() {
	return mSize;
}

void utils::Font::free() {
	mFont = NULL;
	mSize = 0;
}
