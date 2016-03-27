#include "utils.h"
#include <cstdlib>
#include <cstdio>

double random() {
	return (double)rand()/(double)RAND_MAX;
}

Color randomColor() {
	return { (unsigned char)(random()*256), (unsigned char)(random()*256), (unsigned char)(random()*256) };
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