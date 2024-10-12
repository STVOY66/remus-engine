#include "SDL2/SDL.h"
#include "player.h"
#include "sdl2utils.h"
#include <string>
#include <iostream>

//constant values
const unsigned winWidth = 800;
const unsigned winHeight = 600;
const unsigned FPS = 60;
const Vector2f mapDim = {8, 8};
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

enum HitType {
    NS,
    EW
};

struct CastRay {
    Vector2f dir;
    double dist;
    HitType side;
};

std::string testString;
Player2D player;
Vector2f cPlane;
CastRay rayBuffer[winWidth];
SDL_Window* mainWin = NULL;
SDL_Surface* screenSurface = NULL;

//function headers
void draw();
void update();
bool eventHandler(SDL_Event);
bool init();
void close();

void drawPlayer(Player2D);
void drawRays2D();
void drawView();
void draw2DMap(const int*, Vector2f, int);
void drawDebug();

void movePlayer(Player2D*, SDL_Event);
void castRays(Vector2f, Vector2f);


int main(int argc, char **argv) {
    //InitWindow(winWidth, winHeight, "Remus Engine");
    std::cout << "Executing program..." << std::endl;
    bool quit = false;
    SDL_Event e;

    player = Player2D(Vector2f{200, 120}, Vector2f{-1.0f, 0.0f}, 2.0f, 0.08f);
    cPlane = Vector2f{0.0f, 0.66f};

    if(!init()) {
        std::cout << "Program failed to initialize" << std::endl;
        return 0;
    }

    std::cout << "Running main loop..." << std::endl;
    while(!quit) {
        draw();
        update();
        quit = eventHandler(e);
    }

    close();
    return 1;
}

bool init() {
    bool success = true;
    std::cout << "Initializing SDL..." << std::endl;
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "ERROR: SDL failed to initialize." << std::endl;
        success = false;
    } else {
        std::cout << "Creating window..." << std::endl;
        mainWin = SDL_CreateWindow("Remus Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, winWidth, winHeight, SDL_WINDOW_SHOWN);
        if(mainWin == NULL) {
            std::cout << "ERROR: Window failed to be created." << std::endl;
            success = false;
        }
        else {
            std::cout << "Getting window surface..." << std::endl;
            screenSurface = SDL_GetWindowSurface(mainWin);
            if(screenSurface == NULL) {
                std::cout << "ERROR: Failed to get surface." << std::endl;
            }
        }
    }
    return success;
}

void close() {
    std::cout << "Closing program..." << std::endl;

    SDL_DestroyWindow(mainWin);
    mainWin = NULL;

    SDL_Quit();
}

//Handles all draw functions
void draw() {
    SDL_UpdateWindowSurface(mainWin);
    // BeginDrawing();
    //     ClearBackground(bgColor);
        //draw2DMap(testmap1, Vector2{8, 8}, mapScale);
        //drawRays2D();
        //drawPlayer(player);
        //drawDebug();
    //     drawView();
    // EndDrawing();
}

//Handles all update functions between frames
void update() {
    // castRays(player.GetLookDir(), cPlane);
}

//Draws player on 2d plane.
void drawPlayer(Player2D pl) {
    //visualize movement direction (now locked to look direction) on 2d plane
    //DrawLine(pl.position.x, pl.position.y, pl.position.x + pl.GetMoveDir().x*mapScale, pl.position.y + pl.GetMoveDir().y*mapScale, BLUE);
    //visualize player on 2d plane.
    //DrawCircle(pl.position.x, pl.position.y, 8.0f, RED);
}

bool eventHandler(SDL_Event e) {
    if(e.type == SDL_QUIT) return true;
    //movePlayer(&player, e);
    return false;
}

//top-down visualization of map
void draw2DMap(const int *map, Vector2f dim, int squareLen) {
    // int currPos[2];
    // SDL_Color squareColor;
    // for(int i = 0; i < (dim.x * dim.y); i++) {
    //     squareColor = (map[i] > 0) ? WHITE : BLACK; 
    //     currPos[0] = i % (int)dim.x;
    //     currPos[1] = i / (int)dim.y;
    //     DrawRectangle(currPos[0]*squareLen, currPos[1]*squareLen, squareLen, squareLen, squareColor);
    // }
}

//draws debugging information to screen at runtime
void drawDebug() {
    // DrawText("Player Position: ", 640, 0, 12, WHITE);
    // DrawText(TextFormat("%d, %d", (int)player.position.x, (int)player.position.y), 640, 12, 12, WHITE);
    // DrawText(TextFormat("%.3f, %.3f", player.GetMapPos(mapScale).x, player.GetMapPos(mapScale).y), 640, 24, 12, WHITE);
    //draws camera plane
    // Vector2f dirPos = Vector2{player.position.x + (player.GetMoveDir().x*mapScale), player.position.y + (player.GetMoveDir().y*mapScale)};
    // DrawLine(dirPos.x - cPlane.x*mapScale, dirPos.y - cPlane.y*mapScale, dirPos.x + cPlane.x*mapScale, dirPos.y + cPlane.y*mapScale, GREEN);
    //draws player look dir
    //DrawLine(player.position.x, player.position.y, player.position.x + player.GetLookDir().x*mapScale, player.position.y + player.GetLookDir().y*mapScale, YELLOW);
}

// moves a Player2D object given keyboard input
void movePlayer(Player2D *pl, SDL_Event e) {
    Vector2f mdir = pl->GetMoveDir(), pos = pl->position, ldir = pl->GetLookDir();
    float rs = pl->GetRSpeed(), ls = pl->GetLSpeed();
    SDL_Keycode key;

    // tank controls
    if(e.type == SDL_KEYDOWN){
        key = e.key.keysym.sym;

        if(key == SDLK_a) {
            pl->SetMoveDir(fVector2Rotate(mdir, rs));
            pl->SetLookDir(fVector2Rotate(ldir, rs));
            cPlane = fVector2Rotate(cPlane, rs);
        }
        else if(key == SDLK_d) {
            pl->SetMoveDir(fVector2Rotate(mdir, -rs));
            pl->SetLookDir(fVector2Rotate(ldir, -rs));
            cPlane = fVector2Rotate(cPlane, -rs);
        }

        if(key == SDLK_w)
            pl->position = {pos.x + (mdir.x*ls), pos.y + (mdir.y*ls)};
        if(key == SDLK_s)
            pl->position = {pos.x - (mdir.x*ls), pos.y - (mdir.y*ls)};
    }
}

//fills rayBuffer given a direction and camera plane.
void castRays(Vector2f dir, Vector2f camPlane) {
    // double camX; // position on camera plane normalized between -1 and 1
    // double dDistX, dDistY; // distance from one side to next for each axis
    // double sideDistX, sideDistY; // distance from current position to ray collision point
    // Vector2f rayDir; // stores direction of current ray
    // Vector2f pPos = player.GetMapPos(mapScale); // initial position on the map
    // int mapX, mapY; // current map square as int
    // int stepX, stepY; // which direction along the axes
    // HitType side; // determines whether there was a hit, and whether it was NS or EW

    // for(int x = 0; x < winWidth; x++) {
    //     camX = 2.0*(double(x)/double(winWidth)) - 1.0;
    //     rayDir = Vector2{dir.x + camPlane.x*float(camX), dir.y + camPlane.y*float(camX)};
    //     mapX = pPos.x; mapY = pPos.y;
    //     dDistX = (rayDir.x == 0) ? 1e30 : std::abs(1/rayDir.x);
    //     dDistY = (rayDir.y == 0) ? 1e30 : std::abs(1/rayDir.y);

    //     //Calculate distance from current pos to edge of current map square, as well as step direction
    //     if(rayDir.x < 0) {
    //         stepX = -1;
    //         sideDistX = (pPos.x - mapX) * dDistX;
    //     } else {
    //         stepX = 1;
    //         sideDistX = (mapX + 1.0 - pPos.x) * dDistX;
    //     }
    //     if(rayDir.y < 0) {
    //         stepY = -1;
    //         sideDistY = (pPos.y - mapY) * dDistY;
    //     } else {
    //         stepY = 1;
    //         sideDistY = (mapY + 1.0 - pPos.y) * dDistY;
    //     }
    //     //DDA
    //     int hit = 0; // stores whether a map object has been hit
    //     int currentIndex; // stores 1d index for 2d map coordinates
    //     while(hit == 0) {
    //         currentIndex = mapY*8 + mapX;
    //         if(testmap1[currentIndex] > 0) {
    //             hit = 1;
    //             // if hit, rolls back to outer edge of map square
    //             if(side == EW)
    //                 sideDistX -= dDistX;
    //             if(side == NS)
    //                 sideDistY -= dDistY;
    //         }
    //         if(hit == 0) { // only increases distance if no hit
    //             if(sideDistX < sideDistY) {
    //                 sideDistX += dDistX;
    //                 mapX += stepX;
    //                 side = EW;
    //             } else {
    //                 sideDistY += dDistY;
    //                 mapY += stepY;
    //                 side = NS;
    //             }
    //         }
    //     }

    //     side = (sideDistX < sideDistY) ? EW : NS;

    //     rayBuffer[x] = CastRay{rayDir, __min(sideDistX, sideDistY), side}; // stores ray in buffer with distance to map object
    // }
}

//top-down visualization of ray-casting
void drawRays2D() {
    // CastRay ray;
    // for(int i = 0; i < winWidth; i++) {
    //     ray = rayBuffer[i];
    //     DrawLine(player.position.x, player.position.y, player.position.x + ray.dir.x*ray.dist*mapScale, player.position.y + ray.dir.y*ray.dist*mapScale, YELLOW);
    // }
}

void drawView() {
    // SDL_Color wallColorI = RED; SDL_Color wallColor;
    // SDL_Color floorColor = DARKGRAY;
    // SDL_Color ceilColor = BLUE;
    // int lineHeight;
    // int drawStart; int drawEnd;

    // DrawRectangle(0, 0, winWidth, winHeight/2, ceilColor);
    // DrawRectangle(0, winHeight/2, winWidth, winHeight/2, floorColor);

    // for(int i = 0; i < winWidth; i++) {
    //     lineHeight = (int)(winHeight/rayBuffer[i].dist);
    //     drawStart = -lineHeight/2 + winHeight/2;
    //     if(drawStart < 0) drawStart = 0;
    //     drawEnd = lineHeight/2+winHeight/2;
    //     if(drawEnd >= winHeight) drawEnd = winHeight - 1;

    //     //wallColor = ColorBrightness(wallColorI, -(((float)winHeight/(5.0f*(float)lineHeight)))); // rudimentary lighting based on lineHeight
    //     wallColor = (rayBuffer[i].side == EW) ? ColorBrightness(wallColorI, -0.2f) : wallColorI; // rudimentary lighting based on side

    //     DrawLine(i, drawStart, i, drawEnd, wallColor);
    // }
}