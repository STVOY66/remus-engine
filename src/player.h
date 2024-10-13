#pragma once
#include "sdl2utils.h"

// class for a player in 2-Dimensional space.
class Player2D {
    public:
        Player2D(); // default constructor
        Player2D(int posX, int posY); // construct with position as integers
        Player2D(Vector2f pos); // construct with position as vector
        Player2D(Vector2f pos, Vector2f uDir, float lSpeed, float rSpeed); // position, unit vector as direction, linear speed, rotational speed

        Vector2f position; // position of player in 2d space

        // gets/sets for move direction
        Vector2f GetDir();
        void SetDir(Vector2f dir);
        // gets/sets for linear and rotational speed
        float GetLSpeed();
        void SetLSpeed(float speed);
        float GetRSpeed();
        void SetRSpeed(float speed);
    private:
        float lSpeed; // linear speed
        float rSpeed; // rotational speed
        Vector2f dir; // direction player's facing as a 2d unit vector
};
