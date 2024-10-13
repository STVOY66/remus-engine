#include "sdl2utils.h"
#include "player.h"
#include <cstdlib>

Player2D::Player2D() {
    position = {0.0f, 0.0f};
    dir = {0.0f, 1.0f};
    lSpeed = 8.0f;
    rSpeed = 2.0f;
}

Player2D::Player2D(int posX, int posY) {
    position = {(float)posX, (float)posY};
    dir = {0.0f, 1.0f};
    lSpeed = 8.0f;
    rSpeed = 2.0f;
}

Player2D::Player2D(Vector2f pos) {
    position = pos;
    dir = {0.0f, 1.0f};
    lSpeed = 8.0f;
    rSpeed = 2.0f;
}

Player2D::Player2D(Vector2f pos, Vector2f unitDir, float linearSpeed, float rotationalSpeed) {
    position = pos;
    dir = unitDir;
    lSpeed = linearSpeed;
    rSpeed = rotationalSpeed;
}

Vector2f Player2D::GetDir() {
    return dir;
}
void Player2D::SetDir(Vector2f dir) { //normalizes input vector to keep look direction a unit vector.
    Vector2f output = dir;
    if(fVector2Length(dir) != 1.0f) //making sure it's not already a unit vector.
        output = fVector2Normalize(dir);
    this->dir = output;
}

float Player2D::GetLSpeed() {
    return lSpeed;
}
void Player2D::SetLSpeed(float speed) {
    lSpeed = speed;
}

float Player2D::GetRSpeed() {
    return rSpeed;
}
void Player2D::SetRSpeed(float speed) {
    rSpeed = speed;
}