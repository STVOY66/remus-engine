#include "SDL2/SDL.h"
#include "sdl2utils.h"
#include <iostream>

float fVector2Length(Vector2f vector) {
    return SDL_sqrtf((vector.x * vector.x) + (vector.y * vector.y));
}

Vector2f fVector2Normalize(Vector2f vector) {
    float len = fVector2Length(vector);
    return Vector2f{vector.x/len, vector.y/len};
}

Vector2f fVector2Rotate(Vector2f vector, float amt) {
    float sinAmt = SDL_sinf(amt);
    float cosAmt = SDL_cosf(amt);
    Vector2f output = {(vector.x*cosAmt) - (vector.y*sinAmt), (vector.x*sinAmt) + (vector.y*cosAmt)};
    std::cout << "PlayerDir: { " << output.x << ", " << output.y << "} " << std::endl;
    return output;
}