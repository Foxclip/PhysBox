#pragma once

#include <SDL_ttf.h>
#include <string>

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
	double random();
	Color randomColor();
	double distance(double x1, double x2, double y1, double y2);

}