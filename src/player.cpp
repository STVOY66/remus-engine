#include "raylib.h"
#include "raymath.h"
#include "player.h"
#include <cstdlib>

//uses alphamax-betamin algorithm to cheaply calculate a close approximation of a pythagorean addition of two functions.
double quickHypot(double a, double b) {
    double alpha = 0.898204193266868;
    double beta = 0.485968200201465;
    double max = (a < b) ? b : a;
    double min = (a < b) ? a : b;
    double out = (alpha*max) + (beta*min);
    return (max < out) ? out : max;
} //unsure if want to use, using more expensive function in meantime.

Player2D::Player2D() {
    position = {0.0f, 0.0f};
    ldir = {0.0f, 1.0f};
    mdir = ldir;
    lSpeed = 8.0f;
    rSpeed = 2.0f;
}

Player2D::Player2D(int posX, int posY) {
    position = {(float)posX, (float)posY};
    ldir = {0.0f, 1.0f};
    mdir = ldir;
    lSpeed = 8.0f;
    rSpeed = 2.0f;
}

Player2D::Player2D(Vector2 pos) {
    position = pos;
    ldir = {0.0f, 1.0f};
    mdir = ldir;
    lSpeed = 8.0f;
    rSpeed = 2.0f;
}

Player2D::Player2D(Vector2 pos, Vector2 unitDir, float linearSpeed, float rotationalSpeed) {
    position = pos;
    ldir = unitDir;
    mdir = ldir;
    lSpeed = linearSpeed;
    rSpeed = rotationalSpeed;
}

Vector2 Player2D::GetLookDir() {
    return ldir;
}
void Player2D::SetLookDir(Vector2 dir) { //normalizes input vector to keep look direction a unit vector.
    Vector2 output = dir;
    if(Vector2Length(dir) != 1.0f) //making sure it's not already a unit vector.
        output = Vector2Normalize(dir);
    ldir = output;
}

Vector2 Player2D::GetMoveDir() {
    return mdir;
}
void Player2D::SetMoveDir(Vector2 dir) { //normalizes input vector to keep look direction a unit vector.
    Vector2 output = dir;
    if(Vector2Length(dir) != 1.0f) //making sure it's not already a unit vector.
        output = Vector2Normalize(dir);
    mdir = output;
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