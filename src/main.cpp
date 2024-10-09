#include "raylib.h"
#include "raymath.h"
#include "player.h"
#include <string>
#include <iostream>

//constant values
const unsigned winWidth = 800;
const unsigned winHeight = 600;
const unsigned FPS = 60;
const Vector2 mapDim = {8, 8};
const int testmap1[] = 
{1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 0, 0, 0, 0, 1, 1,
 1, 0, 0, 0, 0, 0, 0, 1,
 1, 0, 1, 0, 0, 1, 0, 1,
 1, 0, 1, 0, 0, 1, 0, 1,
 1, 0, 0, 0, 0, 0, 0, 1,
 1, 1, 0, 0, 0, 0, 1, 1,
 1, 1, 1, 1, 1, 1, 1, 1};
const int mapScale = 80;

struct CastRay {
    Vector2 dir;
    double dist;
};

enum HitType {
    NS,
    EW
};

std::string testString;
Player2D player;
Vector2 cPlane;
CastRay rayBuffer[winWidth];

//function headers
void draw(Color);
void drawPlayer(Player2D);
void drawRays2D();
void drawView();
void update();
void movePlayer(Player2D*);
void draw2DMap(const int*, Vector2, int);
void drawDebug();
void castRays(Vector2, Vector2);

int main() {
    SetTargetFPS(60);
    InitWindow(winWidth, winHeight, "Remus Engine");
    player = Player2D(Vector2{200, 120}, Vector2{-1.0f, 0.0f}, 2.0f, 0.08f);
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
        //draw2DMap(testmap1, Vector2{8, 8}, mapScale);
        //drawRays2D();
        //drawPlayer(player);
        //drawDebug();
        drawView();
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

//top-down visualization of map
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

//draws debugging information to screen at runtime
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

// moves a Player2D object given keyboard input
void movePlayer(Player2D *pl) {
    Vector2 mdir = pl->GetMoveDir(), pos = pl->position, ldir = pl->GetLookDir();
    float rs = pl->GetRSpeed(), ls = pl->GetLSpeed();

    //tank controls
    if(IsKeyDown(KEY_A)) {
        pl->SetMoveDir(Vector2Rotate(mdir, rs));
        pl->SetLookDir(Vector2Rotate(ldir, rs));
        cPlane = Vector2Rotate(cPlane, rs);
    }
    else if(IsKeyDown(KEY_D)) {
        pl->SetMoveDir(Vector2Rotate(mdir, -rs));
        pl->SetLookDir(Vector2Rotate(ldir, -rs));
        cPlane = Vector2Rotate(cPlane, -rs);
    }

    if(IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
        pl->position = {pos.x + (mdir.x*ls), pos.y + (mdir.y*ls)};
    if(IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
        pl->position = {pos.x - (mdir.x*ls), pos.y - (mdir.y*ls)};
}

//fills rayBuffer given a direction and camera plane.
void castRays(Vector2 dir, Vector2 camPlane) {
    double camX; // position on camera plane normalized between -1 and 1
    double dDistX, dDistY; // distance from one side to next for each axis
    double sideDistX, sideDistY; // distance from current position to ray collision point
    Vector2 rayDir; // stores direction of current ray
    Vector2 pPos = player.GetMapPos(mapScale); // initial position on the map
    int mapX, mapY; // current map square as int
    int stepX, stepY; // which direction along the axes
    HitType side; // determines whether there was a hit, and whether it was NS or EW

    for(int x = 0; x < winWidth; x++) {
        camX = 2.0*(double(x)/double(winWidth)) - 1.0;
        rayDir = Vector2{dir.x + camPlane.x*float(camX), dir.y + camPlane.y*float(camX)};
        mapX = pPos.x; mapY = pPos.y;
        dDistX = (rayDir.x == 0) ? 1e30 : std::abs(1/rayDir.x);
        dDistY = (rayDir.y == 0) ? 1e30 : std::abs(1/rayDir.y);

        //Calculate distance from current pos to edge of current map square, as well as step direction
        if(rayDir.x < 0) {
            stepX = -1;
            sideDistX = (pPos.x - mapX) * dDistX;
        } else {
            stepX = 1;
            sideDistX = (mapX + 1.0 - pPos.x) * dDistX;
        }
        if(rayDir.y < 0) {
            stepY = -1;
            sideDistY = (pPos.y - mapY) * dDistY;
        } else {
            stepY = 1;
            sideDistY = (mapY + 1.0 - pPos.y) * dDistY;
        }
        //DDA
        int hit = 0; // stores whether a map object has been hit
        int currentIndex; // stores 1d index for 2d map coordinates
        while(hit == 0) {
            currentIndex = mapY*8 + mapX;
            if(testmap1[currentIndex] > 0) {
                hit = 1;
                // if hit, rolls back to outer edge of map square
                if(side == EW)
                    sideDistX -= dDistX;
                if(side == NS)
                    sideDistY -= dDistY;
            }
            if(hit == 0) { // only increases distance if no hit
                if(sideDistX < sideDistY) {
                    sideDistX += dDistX;
                    mapX += stepX;
                    side = EW;
                } else {
                    sideDistY += dDistY;
                    mapY += stepY;
                    side = NS;
                }
            }
        }

        rayBuffer[x] = CastRay{rayDir, __min(sideDistX, sideDistY)}; // stores ray in buffer with distance to map object
    }
}

//top-down visualization of ray-casting
void drawRays2D() {
    CastRay ray;
    for(int i = 0; i < winWidth; i++) {
        ray = rayBuffer[i];
        DrawLine(player.position.x, player.position.y, player.position.x + ray.dir.x*ray.dist*mapScale, player.position.y + ray.dir.y*ray.dist*mapScale, YELLOW);
    }
}

void drawView() {
    Color wallColor = WHITE;
    Color floorColor = DARKGRAY;
    Color ceilColor = LIGHTGRAY;
    int lineHeight;
    int drawStart; int drawEnd;

    DrawRectangle(0, 0, winWidth, winHeight/2, ceilColor);
    DrawRectangle(0, winHeight/2, winWidth, winHeight/2, floorColor);

    for(int i = 0; i < winWidth; i++) {
        lineHeight = (int)(winHeight/rayBuffer[i].dist);
        drawStart = -lineHeight/2 + winHeight/2;
        if(drawStart < 0) drawStart = 0;
        drawEnd = lineHeight/2+winHeight/2;
        if(drawEnd >= winHeight) drawEnd = winHeight - 1;

        DrawLine(i, drawStart, i, drawEnd, wallColor);
    }
}