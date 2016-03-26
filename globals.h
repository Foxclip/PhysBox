#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
#include "lwindow.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

extern SDL_Renderer* gRenderer;
extern TTF_Font* gFont;
extern LWindow gWindow;