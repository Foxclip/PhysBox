#include "lwindow.h"
#include "globals.h"
#include <sstream>

const int DEFAULT_WINDOW_WIDTH = 640;
const int DEFAULT_WIINDOW_HEIGHT = 480;

LWindow::LWindow() {
	mWindow = NULL;
	mMouseFocus = false;
	mKeyboardFocus = false;
	mFullScreen = false;
	mMinimised = false;
	mWidth = 0;
	mHeight = 0;
}

LWindow::~LWindow() {
	free();
}

bool LWindow::init() {
	mWindow = SDL_CreateWindow("Window!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		DEFAULT_WINDOW_WIDTH, DEFAULT_WIINDOW_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if(!mWindow)
		return false;
	mMouseFocus = true;
	mKeyboardFocus = true;
	mWidth = DEFAULT_WINDOW_WIDTH;
	mHeight = DEFAULT_WIINDOW_HEIGHT;
	return true;
}

SDL_Renderer* LWindow::createRenderer() {
	return SDL_CreateRenderer(mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

SDL_Window* LWindow::getSDLWindow() {
	return mWindow;
}

void LWindow::handleEvent(SDL_Event& e) {
	if(e.type == SDL_WINDOWEVENT) {
		bool updateCaption = false;
		switch(e.window.event) {
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			updateSize();
			SDL_RenderPresent(mainRenderer);
			break;
		case SDL_WINDOWEVENT_EXPOSED:
			SDL_RenderPresent(mainRenderer);
			break;
		case SDL_WINDOWEVENT_ENTER:
			mMouseFocus = true;
			updateCaption = true;
			break;
		case SDL_WINDOWEVENT_LEAVE:
			mMouseFocus = false;
			updateCaption = true;
			break;
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			mKeyboardFocus = true;
			updateCaption = true;
			break;
		case SDL_WINDOWEVENT_FOCUS_LOST:
			mKeyboardFocus = false;
			updateCaption = true;
			break;
		case SDL_WINDOWEVENT_MINIMIZED:
			mMinimised = true;
			break;
		case SDL_WINDOWEVENT_MAXIMIZED:
			mMinimised = false;
			updateSize();
			break;
		case SDL_WINDOWEVENT_RESTORED:
			mMinimised = false;
			updateSize();
			break;
		}
	}
}

void LWindow::maximize() {
	if(!isMinimised()) {
		SDL_MaximizeWindow(mWindow);
		mMinimised = false;
	}
}

void LWindow::setFullScreen(bool fullscreen) {
	if(fullscreen) {
		SDL_SetWindowFullscreen(mWindow, SDL_TRUE);
		mFullScreen = true;
	} else {
		SDL_SetWindowFullscreen(mWindow, SDL_FALSE);
		mFullScreen = false;
	}
}


void LWindow::free() {
	if(mWindow) {
		SDL_DestroyWindow(mWindow);
		mWindow = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

int LWindow::getWidth() {
	return mWidth;
}

int LWindow::getHeight() {
	return mHeight;
}

bool LWindow::hasMouseFocus() {
	return mMouseFocus;
}

bool LWindow::hasKeyboardFocus() {
	return mKeyboardFocus;
}

bool LWindow::isMinimised() {
	return mMinimised;
}

bool LWindow::isFullscreen() {
	return mFullScreen;
}

void LWindow::updateSize() {
	int w, h;
	SDL_GetWindowSize(mWindow, &w, &h);
	mWidth = w;
	mHeight = h;
}
