#include "raylib.h"
#include "player.h"

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