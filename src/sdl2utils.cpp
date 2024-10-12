#include "SDL.h"
#include "sdl2utils.h"

float fVector2Length(Vector2f vector) {
    return sqrtf(powf(vector.x, 2) + powf(vector.y, 2));
}

Vector2f fVector2Normalize(Vector2f vector) {
    float len = fVector2Length(vector);
    return Vector2f{vector.x/len, vector.y/len};
}