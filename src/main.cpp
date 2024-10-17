#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "player.h"
#include "sdl2utils.h"
#include <string>
#include <iostream>
#include <vector>
#include <cmath>

//constant values
const unsigned winWidth = 1280;
const unsigned winHeight = 720;
const unsigned screenWidth = 640;
const unsigned screenHeight = int((float)screenWidth * (float(winHeight)/float(winWidth)));
const unsigned FPS = 60;
const Vector2i mapDim = {8, 8};
const int testmap1[] = 
{1, 1, 1, 1, 1, 1, 1, 1,
 1, 1, 0, 0, 0, 0, 1, 1,
 1, 0, 0, 0, 0, 0, 0, 1,
 1, 0, 1, 0, 0, 1, 0, 1,
 1, 0, 1, 0, 0, 1, 0, 1,
 1, 0, 0, 0, 0, 0, 0, 1,
 1, 1, 0, 0, 0, 0, 1, 1,
 1, 1, 1, 1, 1, 1, 1, 1};
const int img_flags = IMG_INIT_JPG;
const std::string resourceDir = "./resources";

enum HitType {
    NS,
    EW
};

enum RenderType {
    SURFACE,
    RENDERER
};

struct CastRay {
    Vector2f dir;
    double dist;
    HitType side;
    int mapI;
    double wallX;
};

std::string testString;
Player2D player;
Vector2f cPlane;
CastRay rayBuffer[screenWidth];
ImgCache *wallTexs = NULL;

SDL_Window* mainWin = NULL;
SDL_Surface* screenSurface = NULL;
SDL_Renderer* mainRender = NULL;
SDL_Texture* frameBuffer = NULL;

//function headers
void render();
void update();
bool eventHandler(SDL_Event);
bool init(RenderType);
bool initLibs();
void loadTextures();
void close();
void clearFrameBuffer();

void drawPlayer(Player2D);
void drawRays2D();
void draw2DMap(const int*, Vector2f, int);
void drawDebug();

void renderView();
void renderDebug();

void movePlayer(Player2D*, const Uint8*);
void castRays(Vector2f, Vector2f);


int main(int argc, char **argv) {
    //InitWindow(screenWidth, screenHeight, "Remus Engine");
    std::cout << "Executing program..." << std::endl;
    bool quit = false;
    SDL_Event e;
    Uint64 ticks1 = SDL_GetTicks64(), ticks2 = SDL_GetTicks64(), deltaTime;

    if(!init(RENDERER)) {
        std::cout << "Program failed to initialize" << std::endl;
        return 0;
    }

    std::cout << "Running main loop..." << std::endl;
    while(!quit) {
        ticks1 = SDL_GetTicks64();
        deltaTime = ticks1 - ticks2;
        if(deltaTime > 1000.0f/float(FPS)) {
            quit = eventHandler(e);
            update();
            render();
            ticks2 = ticks1;
        }
        
    }

    close();
    return 0;
}

bool init(RenderType renderType) {
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
            std::cout << "Initializing SDL extensions..." << std::endl;
            if(!initLibs()) {
                std::cout << "ERROR: Failed to initialize SDL extensions.";
                success = false;
            } else {
                if(renderType == SURFACE) {
                    std::cout << "Getting window surface..." << std::endl;
                    screenSurface = SDL_GetWindowSurface(mainWin);
                    if(screenSurface == NULL) {
                        std::cout << "ERROR: Failed to get surface." << std::endl;
                        success = false;
                    }
                } else if(renderType == RENDERER) {
                    std::cout << "Creating renderer..." << std::endl;
                    mainRender = SDL_CreateRenderer(mainWin, -1, SDL_RENDERER_ACCELERATED);
                    if(mainRender == NULL) {
                        std::cout << "ERROR: Failed to get renderer." << std::endl;
                        success = false;
                    } else {
                        std::cout << "Creating frame buffer..." << std::endl;
                        frameBuffer = SDL_CreateTexture(mainRender, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, screenWidth, screenHeight);
                        if(frameBuffer == NULL) {
                            std::cout << "ERROR: Failed to create frame buffer." << std::endl;
                            success = false;
                        }
                        wallTexs = new ImgCache(resourceDir, img_flags);
                    }
                }
            }
        }
    }

    player = Player2D(Vector2f{4.05821, 2.37642}, Vector2f{-1.0f, 0.0f}, 0.05f, 0.05f);
    cPlane = Vector2f{0.0f, 0.495f*(float(screenWidth)/float(screenHeight))};

    return success;
}

bool initLibs() {
    if(!(IMG_Init(img_flags) & img_flags)) {
        std::cout << "ERROR: SDL_image failed to initialize." << std::endl;
        return false;
    }

    return true;
}

void close() {
    std::cout << "Closing program..." << std::endl;

    wallTexs->flush();
    wallTexs = NULL;

    SDL_DestroyWindow(mainWin);
    mainWin = NULL;

    SDL_DestroyTexture(frameBuffer);
    frameBuffer = NULL;

    SDL_DestroyRenderer(mainRender);
    mainRender = NULL;

    SDL_Quit();
}

// handles functions that use a SDL_Renderer
void render() {
    SDL_SetRenderDrawColor(mainRender, BLACK.r, BLACK.g, BLACK.b, BLACK.a);
    SDL_RenderClear(mainRender);

    renderView();
    //renderDebug();

    SDL_RenderCopy(mainRender, frameBuffer, NULL, NULL);
    SDL_RenderPresent(mainRender);
}

void clearFrameBuffer() {
    Uint32 *pixels; int pitch;
    SDL_LockTexture(frameBuffer, NULL, (void**) &pixels, &pitch);
    for(int y = 0; y < screenHeight; y++) {
        for(int x = 0; x < screenWidth; x++) {
            pixels[y*pitch + x] = 0;
        }
    }
    SDL_UnlockTexture(frameBuffer);
}

//Handles all update functions between frames
void update() {
    const Uint8* currKeyStates = SDL_GetKeyboardState(NULL);
    movePlayer(&player, currKeyStates);
    castRays(player.GetDir(), cPlane);
}

//Draws player on 2d plane.
void drawPlayer(Player2D pl) {
    //visualize movement direction (now locked to look direction) on 2d plane
    //DrawLine(pl.position.x, pl.position.y, pl.position.x + pl.GetMoveDir().x*mapScale, pl.position.y + pl.GetMoveDir().y*mapScale, BLUE);
    //visualize player on 2d plane.
    //DrawCircle(pl.position.x, pl.position.y, 8.0f, RED);
}

bool eventHandler(SDL_Event e) {
    while(SDL_PollEvent(&e)) {
        if(e.type == SDL_QUIT) return true;
    }
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

void renderDebug() {
    Uint32 *buffPix;
    int buffPitch;
    Uint32 color = 0xff0000ff;
    int x, y;
    if(SDL_LockTexture(frameBuffer, NULL, (void**) &buffPix, &buffPitch) == 0) {
        for(x = 0; x < screenWidth; x++) {
            for(y = 0; y < screenHeight; y++) {
                buffPix[y*(buffPitch/sizeof(Uint32)) + x] = color;
            }
        }
        for(x -= 64; x < screenWidth; x++) {
            for(y -= 64; y < screenHeight; y++) buffPix[y*(buffPitch/sizeof(Uint32)) + x] = 0x0000ffff;
        }
    }
    SDL_UnlockTexture(frameBuffer);
}

// moves a Player2D object given keyboard input
void movePlayer(Player2D *pl, const Uint8* keyStates) {
    Vector2f dir = pl->GetDir(), pos = pl->position, newpos;
    float rs = pl->GetRSpeed(), ls = pl->GetLSpeed(), newX, newY;

    // tank controls
    if(keyStates[SDL_SCANCODE_A]) {
        pl->SetDir(fVector2Rotate(dir, rs));
        cPlane = fVector2Rotate(cPlane, rs);
    }
    else if(keyStates[SDL_SCANCODE_D]) {
        pl->SetDir(fVector2Rotate(dir, -rs));
        cPlane = fVector2Rotate(cPlane, -rs);
    }

    dir = pl->GetDir();
    float offWall = 5.0f;

    if(keyStates[SDL_SCANCODE_W]) {
        newX = (testmap1[int(pos.y)*mapDim.x + int(pos.x + dir.x*ls*offWall)] > 0) ? pos.x : pos.x + (dir.x*ls);
        newY = (testmap1[int(pos.y + dir.y*ls*offWall)*mapDim.x + int(pos.x)] > 0) ? pos.y : pos.y + (dir.y*ls);
        pl->position = {newX, newY};
    }
    else if(keyStates[SDL_SCANCODE_S]) {
        newX = (testmap1[int(pos.y)*mapDim.x + int(pos.x - dir.x*ls*offWall)] > 0) ? pos.x : pos.x - (dir.x*ls);
        newY = (testmap1[int(pos.y - dir.y*ls*offWall)*mapDim.x + int(pos.x)] > 0) ? pos.y : pos.y - (dir.y*ls);
        pl->position = {newX, newY};
    }
}

//fills rayBuffer given a direction and camera plane.
void castRays(Vector2f dir, Vector2f camPlane) {
    double camX; // position on camera plane normalized between -1 and 1
    double dDistX, dDistY; // distance from one side to next for each axis
    double sideDistX, sideDistY, wallDist; // distance from current position to ray collision point
    Vector2f rayDir; // stores direction of current ray
    Vector2f pPos = player.position; // initial position on the map
    int mapX, mapY; // current map square as int
    int stepX, stepY; // which direction along the axes
    HitType side; // determines whether there was a hit, and whether it was NS or EW

    for(int x = 0; x < screenWidth; x++) {
        camX = 2.0*(double(x)/double(screenWidth)) - 1.0;
        rayDir = Vector2f{dir.x + camPlane.x*float(camX), dir.y + camPlane.y*float(camX)};
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

        side = (sideDistX < sideDistY) ? EW : NS;
        wallDist = __min(sideDistX, sideDistY);

        double wallX;
        if(side == EW) wallX = pPos.y + wallDist*rayDir.y;
        else wallX = pPos.x + wallDist*rayDir.x;
        wallX -= std::floor((wallX));

        rayBuffer[x] = CastRay{rayDir, __min(sideDistX, sideDistY), side, currentIndex, wallX}; // stores ray in buffer with distance to map object
    }
}

//top-down visualization of ray-casting
void drawRays2D() {
    // CastRay ray;
    // for(int i = 0; i < screenWidth; i++) {
    //     ray = rayBuffer[i];
    //     DrawLine(player.position.x, player.position.y, player.position.x + ray.dir.x*ray.dist*mapScale, player.position.y + ray.dir.y*ray.dist*mapScale, YELLOW);
    // }
}

void renderView() {
    SDL_Surface* currTex = NULL;
    CastRay currentRay;
    Vector2i texDim;
    int mapVal, texX, texY, pixIndex;
    unsigned int lineHeight, deltaHeight = 0, offset;
    int drawStart, drawEnd;
    Uint32 *bufferPixels, *texPix; int bufferPitch;
    Uint32 pixColor;
    double texPos, texStep;

    SDL_LockTexture(frameBuffer, NULL, (void**)&bufferPixels, &bufferPitch);

    for(int x = 0; x < screenWidth; x++) for(int y = 0; y < screenHeight; y++) bufferPixels[y*(bufferPitch/sizeof(Uint32)) + x] = 0x3BF2FFFF;

    for(int x = 0; x < screenWidth; x++) for(int y = screenHeight/2; y < screenHeight; y++) bufferPixels[y*(bufferPitch/sizeof(Uint32)) + x] = 0x7D7D7DFF;
    
    for(int x = 0; x < screenWidth; x++) {
        currentRay = rayBuffer[x];
        mapVal = testmap1[currentRay.mapI];
        
        lineHeight = (int)(screenHeight/currentRay.dist);
        drawStart = -(lineHeight/2) + screenHeight/2;
        if(drawStart < 0) drawStart = 0;
        drawEnd = lineHeight/2+screenHeight/2;
        if(drawEnd >= screenHeight) drawEnd = screenHeight - 1;
        deltaHeight = (lineHeight > screenHeight) ? lineHeight - screenHeight : 0;

        pixColor = (currentRay.side == EW) ? 0xFF0000FF : 0x7D0000FF;

        if(mapVal <= wallTexs->cache.size() && mapVal > 0) {
            currTex = wallTexs->cache.at("0_testimg.jpg");
            texPix = (Uint32*)currTex->pixels;
            texDim = wallTexs->getDim(mapVal - 1);
            texStep = 1.0 * float(texDim.y)/float(lineHeight);
            texPos = (drawStart-screenHeight/2 + lineHeight/2) * texStep;
            texX = int(currentRay.wallX * double(texDim.x));
            if((currentRay.side == EW && currentRay.dir.x > 0) ||
               (currentRay.side == NS && currentRay.dir.y < 0)) texX = texDim.x - texX - 1;

            for(int y = drawStart; y < drawEnd; y++) {
                texY = int(texPos) & (texDim.y - 1);
                texPos += texStep;
                pixIndex = (texY*(currTex->pitch/sizeof(Uint32)) + texX);
                pixColor = texPix[pixIndex];
                if(currentRay.side == EW) pixColor = darkenPixelRGBA8888(pixColor, 0.8f);
                bufferPixels[y*(bufferPitch/sizeof(Uint32)) + x] = pixColor;
            }
        }
    }

    SDL_UnlockTexture(frameBuffer);
    //SDL_RenderCopy(mainRender, frameBuffer, NULL, NULL);
}