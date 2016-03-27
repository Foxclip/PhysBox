#pragma once

#include <SDL.h>

class LWindow {
public:
	LWindow();
	~LWindow();
	bool init();
	SDL_Window* getSDLWindow();
	SDL_Renderer* getRenderer();
	void handleEvent(SDL_Event& e);
	void maximize();
	void setFullScreen(bool fullscreen);
	void free();
	int getWidth();
	int getHeight();
	bool hasMouseFocus();
	bool hasKeyboardFocus();
	bool isMinimised();
	bool isFullscreen();

private:
	SDL_Window* mWindow;
	SDL_Renderer* mRenderer;
	int mWidth;
	int mHeight;
	bool mMouseFocus;
	bool mKeyboardFocus;
	bool mFullScreen;
	bool mMinimised;
	void updateSize();

};