#pragma once
#include "raylib.h"

// class for a player in 2-Dimensional space.
class Player2D {
    public:
        Player2D(); // default constructor
        Player2D(int posX, int posY); // construct with position as integers
        Player2D(Vector2 pos); // construct with position as vector
        Player2D(Vector2 pos, Vector2 uDir, float lSpeed, float rSpeed); // position, unit vector as direction, linear speed, rotational speed
        ~Player2D(); // destructor

        Vector2 GetPos();
        Vector2 GetDir();
        float GetLSpeed();
        float GetRSpeed();
    private:
        Vector2 position; // position of player in 2d space
        Vector2 direction; // direction player's facing as a 2d unit vector
        float lSpeed; // linear speed
        float rSpeed; // rotational speed
};
