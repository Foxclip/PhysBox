#pragma once

#include <SDL_ttf.h>
#include <string>

class LTexture {
	public:
		LTexture();
		~LTexture();
		bool loadFromFile(std::string path);
		bool loadFromRenderedText(std::string textureText, SDL_Color textColor, bool antialiasing = true);
		bool createBlank(int width, int height, SDL_TextureAccess access = SDL_TEXTUREACCESS_STREAMING);
		void free();
		void setColor(Uint8 red, Uint8 green, Uint8 blue);
		void setBlendMode(SDL_BlendMode blendMode);
		void setAlpha(Uint8 alpha);
		void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);
		void setAsRenderTarget();
		int getWidth();
		int getHeight();
		bool lockTexture();
		bool unlockTexture();
		void* getPixels();
		void copyPixels(void* pixels);
		int getPitch();

	private:
		SDL_Texture* mTexture;
		int mWidth, mHeight;
		void* mPixels;
		int mPitch;

};