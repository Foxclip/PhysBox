#pragma once

#include <SDL_ttf.h>
#include <string>

typedef struct Color {
	unsigned char red, green, blue;
} Color;

class Font {
public:
	Font();
	~Font();
	bool loadFont(std::string path, int size);
	TTF_Font* getSDLFont();
	int getSize();
	void free();

private:
	TTF_Font* mFont;
	int mSize;

};

double randomBetween(double min, double max);
double random();
Color randomColor();