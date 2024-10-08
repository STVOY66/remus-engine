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
Vector2 cPlane;
Vector2 rayBuffer[winWidth];

//function headers
void draw(Color);
void drawPlayer(Player2D);
void drawRays();
void update();
void movePlayer(Player2D*);
void draw2DMap(const int*, Vector2, int);
void drawDebug();
void castRays(Vector2, Vector2); // returns endpoint of ray cast out from an initial position to a unit vector, inf if no collision.
Vector2 collisionCheck(Player2D); // returns a modified move dir if look dir ray collides with object/wall

int main() {
    SetTargetFPS(60);
    InitWindow(winWidth, winHeight, "Remus Engine");
    player = Player2D(Vector2{winWidth/2, winHeight/2}, Vector2{-1.0f, 0.0f}, 2.0f, 0.08f);
    cPlane = Vector2{0.0f, 0.66f};

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
        drawRays();
        drawPlayer(player);
        drawDebug();
    EndDrawing();
}

//Handles all update functions between frames
void update() {
    movePlayer(&player);
    castRays(player.GetLookDir(), cPlane);
}

//Draws player on 2d plane.
void drawPlayer(Player2D pl) {
    //visualize movement direction (now locked to look direction) on 2d plane
    DrawLine(pl.position.x, pl.position.y, pl.position.x + pl.GetMoveDir().x*mapScale, pl.position.y + pl.GetMoveDir().y*mapScale, BLUE);
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
    DrawText(TextFormat("%.3f, %.3f", player.GetMapPos(mapScale).x, player.GetMapPos(mapScale).y), 640, 24, 12, WHITE);
    //draws camera plane
    Vector2 dirPos = Vector2{player.position.x + (player.GetMoveDir().x*mapScale), player.position.y + (player.GetMoveDir().y*mapScale)};
    DrawLine(dirPos.x - cPlane.x*mapScale, dirPos.y - cPlane.y*mapScale, dirPos.x + cPlane.x*mapScale, dirPos.y + cPlane.y*mapScale, GREEN);
    //draws player look dir
    //DrawLine(player.position.x, player.position.y, player.position.x + player.GetLookDir().x*mapScale, player.position.y + player.GetLookDir().y*mapScale, YELLOW);
}

void movePlayer(Player2D *pl) {
    Vector2 mdir = pl->GetMoveDir(), pos = pl->position, ldir = pl->GetLookDir();
    float rs = pl->GetRSpeed(), ls = pl->GetLSpeed();

    //tank controls
    if(IsKeyDown(KEY_A)) {
        pl->SetMoveDir(Vector2Rotate(mdir, -rs));
        pl->SetLookDir(Vector2Rotate(ldir, -rs));
        cPlane = Vector2Rotate(cPlane, -rs);
    }
    else if(IsKeyDown(KEY_D)) {
        pl->SetMoveDir(Vector2Rotate(mdir, rs));
        pl->SetLookDir(Vector2Rotate(ldir, rs));
        cPlane = Vector2Rotate(cPlane, rs);
    }

    if(IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
        pl->position = {pos.x + (mdir.x*ls), pos.y + (mdir.y*ls)};
    if(IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
        pl->position = {pos.x - (mdir.x*ls), pos.y - (mdir.y*ls)};
}

void castRays(Vector2 dir, Vector2 camPlane) { //fills rayBuffer given a direction and camera plane.
    double camX;
    for(int x = 0; x < winWidth; x++) {
        camX = 2.0*(double(x)/double(winWidth)) - 1.0;
        rayBuffer[x] = Vector2{dir.x + camPlane.x*float(camX), dir.y + camPlane.y*float(camX)};
    }
}

void drawRays() {
    Vector2 ray;
    for(int i = 0; i < winWidth; i++) {
        ray = rayBuffer[i];
        DrawLine(player.position.x, player.position.y, player.position.x + ray.x*200, player.position.y + ray.y*200, YELLOW);
    }
}