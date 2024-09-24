#pragma once
#include "raylib.h"

float quickHypot(float a, float b); // function to cheaply perform pythagorean addition on two floats.

// class for a player in 2-Dimensional space.
class Player2D {
    public:
        Player2D(); // default constructor
        Player2D(int posX, int posY); // construct with position as integers
        Player2D(Vector2 pos); // construct with position as vector
        Player2D(Vector2 pos, Vector2 uDir, float lSpeed, float rSpeed); // position, unit vector as direction, linear speed, rotational speed
        ~Player2D(); // destructor

        Vector2 position; // position of player in 2d space

        // gets/sets for look direction, set returns an int for error-handling
        Vector2 GetLookDir();
        int SetLookDir(Vector2 dir);
        // gets/sets for move direction
        Vector2 GetMoveDir();
        int SetMoveDir(Vector2 dir);
        // gets/sets for linear and rotational speed
        float GetLSpeed();
        int SetLSpeed(float speed);
        float GetRSpeed();
        int SetRSpeed(float speed);
    private:
        float lSpeed; // linear speed
        float rSpeed; // rotational speed
        Vector2 ldir; // direction player's facing as a 2d unit vector
        Vector2 mdir; // direction player's moving as a 2d unit vector (Idea to deal with collisions)

};
