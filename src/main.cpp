#include "raylib.h"
#include "raymath.h"
#include "player.h"
#include <string>
#include <iostream>

//constant values
const unsigned winWidth = 800;
const unsigned winHeight = 600;
const unsigned FPS = 60;

std::string testString;
Player2D player;

//function headers
void draw(Color);
void drawPlayer(Player2D);
void update();
void movePlayer(Player2D*);

int main() {
    SetTargetFPS(60);
    InitWindow(winWidth, winHeight, "Remus Engine");
    player = Player2D(Vector2{winWidth/2, winHeight/2}, Vector2{0.0f, 1.0f}, 2.0f, 0.08f);

    std::cout << "Beginning game-loop..." << std::endl;
    while(!WindowShouldClose()) {
        draw(BLACK);
        update();
    }

    CloseWindow();
}

//Handles all draw functions
void draw(Color bgColor) {
    BeginDrawing();
        ClearBackground(bgColor);
        drawPlayer(player);
    EndDrawing();
}

//Handles all update functions between frames
void update() {
    movePlayer(&player);
}

//Draws player on 2d plane.
void drawPlayer(Player2D pl) {
    //visualize movement direction (now locked to look direction) on 2d plane
    DrawLine(pl.position.x, pl.position.y, pl.position.x + pl.GetMoveDir().x*24, pl.position.y + pl.GetMoveDir().y*24, BLUE);
    //visualize player on 2d plane.
    DrawCircle(pl.position.x, pl.position.y, 8.0f, RED);
}

void movePlayer(Player2D *pl) {
    Vector2 mdir = pl->GetMoveDir(), ldir = pl->GetMoveDir(), pos = pl->position;
    float rs = pl->GetRSpeed(), ls = pl->GetLSpeed();
    if(IsKeyDown(KEY_LEFT))
        pl->SetMoveDir(Vector2Rotate(mdir, -rs));
    if(IsKeyDown(KEY_RIGHT))
        pl->SetMoveDir(Vector2Rotate(mdir, rs));
    if(IsKeyDown(KEY_UP))
        pl->position = {pos.x + (mdir.x*ls), pos.y + (mdir.y*ls)};
    if(IsKeyDown(KEY_DOWN))
        pl->position = {pos.x - (mdir.x*ls), pos.y - (mdir.y*ls)};
}