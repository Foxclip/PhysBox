#pragma once

#include <SDL_ttf.h>
#include <string>
#include <functional>

namespace utils {

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
	double nonLinearRandomBetween(double min, double max, std::function<double(double)> f);
	double random();
	Color randomColor();
	std::string toString(double var, int precision);
	Color HSVtoRGB(int H, int S, int V);
	double mapRange(double val, double min1, double max1, double min2, double max2, bool clamp = false);

}