#pragma once
#include "SDL2/SDL.h"

// ******** SDL color constants ********
#define BLACK SDL_Color{0, 0, 0, 255}

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

Vector2f fVector2Rotate(Vector2f, float);