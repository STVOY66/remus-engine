#include "raylib.h"
#include "raymath.h"
#include "player.h"
#include <string>
#include <iostream>

//constant values
const unsigned winWidth = 800;
const unsigned winHeight = 600;
const unsigned FPS = 60;
const int testmap1[] = 
{1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 0, 0, 0, 0, 1, 1,
 1, 0, 0, 0, 0, 0, 0, 1,
 1, 0, 1, 1, 1, 1, 0, 1,
 1, 0, 1, 1, 1, 1, 0, 1,
 1, 0, 0, 0, 0, 0, 0, 1,
 1, 1, 0, 0, 0, 0, 1, 1,
 1, 1, 1, 1, 1, 1, 1, 1};
const int mapScale = 80;

std::string testString;
Player2D player;

//function headers
void draw(Color);
void drawPlayer(Player2D);
void update();
void movePlayer(Player2D*);
void draw2DMap(const int*, Vector2, int);
void drawDebug();
Vector2 collisionCheck(Player2D); // returns a modified move dir if look dir ray collides with object/wall

int main() {
    SetTargetFPS(60);
    InitWindow(winWidth, winHeight, "Remus Engine");
    player = Player2D(Vector2{winWidth/2, winHeight/2}, Vector2{0.0f, 1.0f}, 2.0f, 0.08f);

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
        draw2DMap(testmap1, Vector2{8, 8}, mapScale);
        drawPlayer(player);
        drawDebug();
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

void draw2DMap(const int *map, Vector2 dim, int squareLen) {
    int currPos[2];
    Color squareColor;
    for(int i = 0; i < (dim.x * dim.y); i++) {
        squareColor = (map[i] > 0) ? WHITE : BLACK; 
        currPos[0] = i % (int)dim.x;
        currPos[1] = i / (int)dim.y;
        DrawRectangle(currPos[0]*squareLen, currPos[1]*squareLen, squareLen, squareLen, squareColor);
    }
}

void drawDebug() {
    DrawText("Player Position: ", 640, 0, 12, WHITE);
    DrawText(TextFormat("%d, %d", (int)player.position.x, (int)player.position.y), 640, 12, 12, WHITE);
    DrawText(TextFormat("%d, %d", (int)player.GetMapPos(mapScale).x, (int)player.GetMapPos(mapScale).y), 640, 24, 12, WHITE);
}

void movePlayer(Player2D *pl) {
    Vector2 ldir = pl->GetMoveDir(), pos = pl->position;
    float rs = pl->GetRSpeed(), ls = pl->GetLSpeed();

    //tank controls
    if(IsKeyDown(KEY_A))
        pl->SetMoveDir(Vector2Rotate(ldir, -rs));
    if(IsKeyDown(KEY_D))
        pl->SetMoveDir(Vector2Rotate(ldir, rs));

    //pl->SetMoveDir(collisionCheck(*pl));
    Vector2 mdir = pl->GetMoveDir();

    if(IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
        pl->position = {pos.x + (mdir.x*ls), pos.y + (mdir.y*ls)};
    if(IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
        pl->position = {pos.x - (mdir.x*ls), pos.y - (mdir.y*ls)};
}

Vector2 collisionCheck(Player2D pl) { //returns modified move dir if look dir collides with a surface, returned vector is a component of the look vector along the collided surface.

}
