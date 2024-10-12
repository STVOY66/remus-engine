#pragma once
#include "SDL2/SDL.h"

// ******** SDL color constants ********
#define BLACK SDL_Color{0, 0, 0, 255}
#define GRAY SDL_Color{125, 125, 125, 255};
#define WHITE SDL_Color{255, 255, 255, 255};
#define RED SDL_Color{255, 0, 0, 255};
#define DARK_RED SDL_Color{125, 0, 0, 255};
#define GREEN SDL_Color{0, 255, 0, 255};
#define DARK_GREEN SDL_Color{0, 125, 0, 255};
#define BLUE SDL_Color{0, 0, 255, 255};
#define DARK_BLUE SDL_Color{0, 0, 125, 255};

// ******** Vector2 utilities ********

// 2D Vector using floats
struct Vector2f {
    float x;
    float y;
};

// Returns length of 2D Vector
float fVector2Length(Vector2f);

// Normalizes 2D Vector
Vector2f fVector2Normalize(Vector2f);

// Rotates vector by radians
Vector2f fVector2Rotate(Vector2f, float);