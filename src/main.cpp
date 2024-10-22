#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "gameutils.h"
#include "player.h"
#include "sdl2utils.h"
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>


//constant values
const unsigned winWidth = 1280;
const unsigned winHeight = 720;
const float resScale = 0.5f;
const unsigned screenWidth = roundf(winWidth*resScale);
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
const int img_flags = IMG_INIT_JPG | IMG_INIT_PNG;
const std::string dirWallTex = "./resources/wall_textures";
const std::string dirSprTex = "./resources/spr_textures";

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

struct RemusSprite : Sprite {
    int vPos;
    double dist;
    Vector2f ldir;
};

std::string testString;
Player2D player;
Vector2f cPlane;
CastRay rayBuffer[screenWidth];
std::vector<RemusSprite> worldSprites;

ImgCache *wallTex = NULL;
ImgCache *sprTex = NULL;

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
bool initSprites();
void loadTextures();
void close();
void clearFrameBuffer();

void draw();
void drawPlayer(Player2D);
void drawRays2D();
void draw2DMap(const int*, Vector2i, int);
void drawDebug();

void renderView();
void renderCeilFloor(Uint32*, int*);
void renderSprites(Uint32*, int*);
void renderDebug();

void movePlayer(Player2D*, const Uint8*);
void castRays(Vector2f, Vector2f);
void updateSprites();


int main(int argc, char **argv) {
    //InitWindow(screenWidth, screenHeight, "Remus Engine");
    std::cout << "Executing program..." << "\n";
    bool quit = false;
    SDL_Event e;
    Uint64 ticks1 = SDL_GetTicks64(), ticks2 = SDL_GetTicks64(), deltaTime;

    if(!init(RENDERER)) {
        std::cout << "Program failed to initialize" << "\n";
        return 0;
    }

    std::cout << "Running main loop..." << "\n";
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

    std::cout << "Initializing SDL..." << "\n";
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "ERROR: SDL failed to initialize." << "\n";
        success = false;
    } else {
        std::cout << "Creating window..." << "\n";
        mainWin = SDL_CreateWindow("Remus Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, winWidth, winHeight, SDL_WINDOW_SHOWN);
        if(mainWin == NULL) {
            std::cout << "ERROR: Window failed to be created." << "\n";
            success = false;
        }
        else {
            std::cout << "Initializing SDL extensions..." << "\n";
            if(!initLibs()) {
                std::cout << "ERROR: Failed to initialize SDL extensions.";
                success = false;
            } else {
                if(renderType == SURFACE) {
                    std::cout << "Getting window surface..." << "\n";
                    screenSurface = SDL_GetWindowSurface(mainWin);
                    if(screenSurface == NULL) {
                        std::cout << "ERROR: Failed to get surface." << "\n";
                        success = false;
                    }
                } else if(renderType == RENDERER) {
                    std::cout << "Creating renderer..." << "\n";
                    mainRender = SDL_CreateRenderer(mainWin, -1, SDL_RENDERER_ACCELERATED);
                    if(mainRender == NULL) {
                        std::cout << "ERROR: Failed to get renderer." << "\n";
                        success = false;
                    } else {
                        std::cout << "Creating frame buffer..." << "\n";
                        frameBuffer = SDL_CreateTexture(mainRender, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, screenWidth, screenHeight);
                        if(frameBuffer == NULL) {
                            std::cout << "ERROR: Failed to create frame buffer." << "\n";
                            success = false;
                        }
                        wallTex = new ImgCache(dirWallTex, img_flags);
                        if(!initSprites()) {
                            success = false;
                        }
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
        std::cout << "ERROR: SDL_image failed to initialize." << "\n";
        return false;
    }

    return true;
}

bool initSprites() {
    std::cout << "Initializing sprite list..." << "\n";
    sprTex = new ImgCache(dirSprTex, img_flags);
    for(auto st : sprTex->cache) {
        worldSprites.push_back(RemusSprite{
            1.5f,
            5.5f, 
            st.second->w, 
            st.second->h,
            st.first,
            winHeight/2,
            0.0,
            Vector2f{0, 0} });
        if(!worldSprites.empty()) {
            if(worldSprites.back().texName == st.first) std::cout << "Sprite: " << st.first << " successfully created!" << "\n";
            else { 
                std::cout << "Sprite: " << st.first << " failed to create, exiting program..." << "\n";
                return false;
            }
        } else {
            std::cout << "Sprite: " << st.first << " failed to push to worldSprites, exiting program..." << "\n";
            return false;
        }
    }
    return true;
}

void close() {
    std::cout << "Closing program..." << "\n";

    wallTex->flush();
    wallTex = NULL;

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
    SDL_SetRenderDrawColor(mainRender, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(mainRender);

    renderView();
    //renderDebug();
    //draw();

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
    updateSprites();
}

void draw() {
    draw2DMap(testmap1, mapDim, 64);
    drawPlayer(player);
}

//Draws player on 2d plane.
void drawPlayer(Player2D pl) {
    //visualize movement direction (now locked to look direction) on 2d plane
    SDL_SetRenderDrawColor(mainRender, 0, 0xFF, 0xFF, SDL_ALPHA_OPAQUE);
    SDL_RenderDrawLine(mainRender, pl.position.x * 64, pl.position.y * 64, (pl.position.x * 64) + pl.GetDir().x*20.0f, (pl.position.y * 64) + pl.GetDir().y*20.0f);
    //visualize player on 2d plane.
    SDL_Rect playRect = SDL_Rect{(pl.position.x * 64) - 4, (pl.position.y * 64) - 4, 8, 8};
    SDL_SetRenderDrawColor(mainRender, 0xFF, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(mainRender, &playRect);
}

bool eventHandler(SDL_Event e) {
    while(SDL_PollEvent(&e)) {
        if(e.type == SDL_QUIT) return true;
    }
    return false;
}

//top-down visualization of map
void draw2DMap(const int *map, Vector2i dim, int squareLen) {
    Vector2i currPos;
    Uint32 squareColor;
    SDL_Rect currRect;
    for(int i = 0; i < (dim.x * dim.y); i++) {
        switch(map[i]) {
            case 0:
                squareColor = 0x000000FF;
                break;
            case 1:
                squareColor = 0xFFFFFFFF;
                break;
        }
        SDL_SetRenderDrawColor(mainRender, (squareColor >> 24) & 0xFF, (squareColor >> 16) & 0xFF, (squareColor >> 8) & 0xFF, SDL_ALPHA_OPAQUE);
        currPos.x = (i % (int)dim.x) * squareLen;
        currPos.y = (i / (int)dim.y) * squareLen;
        currRect = SDL_Rect{currPos.x, currPos.y, squareLen, squareLen};
        SDL_RenderFillRect(mainRender, &currRect);
    }
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
    Vector2f move = pl->position;

    if(keyStates[SDL_SCANCODE_W]) {
        if(testmap1[int(pos.y)*mapDim.x + int(pos.x + dir.x*ls*offWall)] <= 0) pl->position.x += (dir.x*ls);
        if(testmap1[int(pos.y + dir.y*ls*offWall)*mapDim.x + int(pos.x)] <= 0) pl->position.y += (dir.y*ls);
    }
    else if(keyStates[SDL_SCANCODE_S]) {
        if(testmap1[int(pos.y)*mapDim.x + int(pos.x - dir.x*ls*offWall)] <= 0) pl->position.x -= (dir.x*ls);
        if(testmap1[int(pos.y - dir.y*ls*offWall)*mapDim.x + int(pos.x)] <= 0) pl->position.y -= (dir.y*ls);
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

bool compDist(const RemusSprite &a, const RemusSprite &b) {
    return (a.dist < b.dist);
}

void updateSprites() {
    for(auto &spr : worldSprites) {
        spr.dist = ((player.position.x - spr.x) * (player.position.x - spr.x)) + ((player.position.y - spr.y) * (player.position.y - spr.y));
    }
    std::sort(worldSprites.begin(), worldSprites.end(), compDist);
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

    if(SDL_LockTexture(frameBuffer, NULL, (void**)&bufferPixels, &bufferPitch) == 0) {

        // for(int x = 0; x < screenWidth; x++) for(int y = 0; y < screenHeight; y++) bufferPixels[y*(bufferPitch/sizeof(Uint32)) + x] = 0x3BF2FFFF;
        // for(int x = 0; x < screenWidth; x++) for(int y = screenHeight/2; y < screenHeight; y++) bufferPixels[y*(bufferPitch/sizeof(Uint32)) + x] = 0x7D7D7DFF;
        
        renderCeilFloor(bufferPixels, &bufferPitch);

        for(int x = 0; x < screenWidth; x++) {
            currentRay = rayBuffer[x];
            mapVal = testmap1[currentRay.mapI];
            
            lineHeight = (int)(screenHeight/currentRay.dist);
            drawStart = -(lineHeight/2) + screenHeight/2;
            if(drawStart < 0) drawStart = 0;
            drawEnd = lineHeight/2+screenHeight/2;
            if(drawEnd >= screenHeight) drawEnd = screenHeight;
            deltaHeight = (lineHeight > screenHeight) ? lineHeight - screenHeight : 0;

            // pixColor = (currentRay.side == EW) ? 0xFF0000FF : 0x7D0000FF;

            if(mapVal <= wallTex->cache.size() && mapVal > 0) {
                currTex = wallTex->cache.at("wall.png");
                texPix = (Uint32*)currTex->pixels;
                texDim = wallTex->getDim("wall.png");
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
                    if(currentRay.side == EW) darkenPixelRGBA8888(&pixColor, 80);
                    // darkenPixelRGBA8888(&pixColor, int(150/currentRay.dist)); // rude lighting
                    bufferPixels[y*(bufferPitch/sizeof(Uint32)) + x] = pixColor;
                }
            }
        }

        renderSprites(bufferPixels, &bufferPitch);
    }
    SDL_UnlockTexture(frameBuffer);
    //SDL_RenderCopy(mainRender, frameBuffer, NULL, NULL);
}

void renderCeilFloor(Uint32* buffPix, int *buffPitch) {
    SDL_Surface *ceilTex = wallTex->cache.at("ceil.png"), *floorTex = wallTex->cache.at("floor.png");
    Uint32 *ceilTexBuff = (Uint32*)ceilTex->pixels, *floorTexBuff = (Uint32*)floorTex->pixels;
    CastRay lRay = rayBuffer[0], rRay = rayBuffer[screenWidth - 1];
    Vector2f floorPos; Vector2i cellPos, texPosF, texPosC;
    Uint32 pixColor;
    float zPos = 0.5f * screenHeight, rowDist, floorStepX, floorStepY;
    int p;

    for(int y = 0; y < screenHeight; y++) {
        p = y - screenHeight/2;
        rowDist = zPos/p;

        floorStepX = rowDist * float(rRay.dir.x - lRay.dir.x)/float(screenWidth);
        floorStepY = rowDist * float(rRay.dir.y - lRay.dir.y)/float(screenWidth);
        floorPos.x = player.position.x + rowDist * lRay.dir.x;
        floorPos.y = player.position.y + rowDist * lRay.dir.y;

        for(int x = 0; x < screenWidth; x++) {
            cellPos = {int(floorPos.x), int(floorPos.y)};
            texPosC = {int(ceilTex->w * (floorPos.x - cellPos.x)) & (ceilTex->w - 1), 
                       int(ceilTex->h * (floorPos.y - cellPos.y)) & (ceilTex->h - 1)};
            texPosF = {int(floorTex->w * (floorPos.x - cellPos.x)) & (floorTex->w - 1), 
                       int(floorTex->h * (floorPos.y - cellPos.y)) & (floorTex->h - 1)};
            floorPos.x += floorStepX;
            floorPos.y += floorStepY;

            pixColor = floorTexBuff[texPosF.y*(floorTex->pitch/sizeof(Uint32)) + texPosF.x];
            // darkenPixelRGBA8888(&pixColor, int(150/rowDist)); // rude lighting
            buffPix[y*(*buffPitch/sizeof(Uint32)) + x] = pixColor;

            pixColor = ceilTexBuff[texPosC.y*(ceilTex->pitch/sizeof(Uint32)) + texPosC.x];
            // darkenPixelRGBA8888(&pixColor, int(150/rowDist)); // rude lighting
            buffPix[(screenHeight - y - 1)*(*buffPitch/sizeof(Uint32)) + x] = pixColor;
        }
    }
}

void renderSprites(Uint32 *buffPix, int *buffPitch) {
    SDL_Surface* currTex = NULL;
    Uint32 *texBuff = NULL;
    Vector2f relSprPos, camSprPos, pDir = player.GetDir();
    float invMat = 1.0f / ((cPlane.x * pDir.y) - (cPlane.y * pDir.x)), sprScrX, texRatio;
    int sprH, sprW, drawStartY, drawEndY, drawStartX, drawEndX, texX, texY, I_drawStartX, delt;
    Uint32 scolor;

    for(auto &spr : worldSprites) {
        try { currTex = sprTex->cache.at(spr.texName); }
        catch(const std::exception& e) { std::cerr << e.what() << '\n'; return;}

        texBuff = (Uint32*)currTex->pixels;
        
        relSprPos = Vector2f{spr.x - player.position.x, spr.y - player.position.y};
        camSprPos = Vector2f{invMat * ((pDir.y * relSprPos.x) - (pDir.x * relSprPos.y)),
                             invMat * (((-cPlane.y) * relSprPos.x) + (cPlane.x * relSprPos.y))};

        sprScrX = int((screenWidth/2.0f) * (1.0f + camSprPos.x/camSprPos.y));
        // std::cout << sprScrX << '\n';
        sprH = abs(int(screenHeight/camSprPos.y));
        sprW = sprH;
        texRatio = float(currTex->w)/float(sprW);

        drawStartY = -sprH/2 + screenHeight/2;
        if(drawStartY < 0) drawStartY = 0;
        drawEndY = sprH/2 + screenHeight/2;
        if(drawEndY >= screenHeight) drawEndY = screenHeight - 1;

        drawStartX = -sprW/2 + sprScrX;
        I_drawStartX = drawStartX;
        if(drawStartX < 0) drawStartX = 0;
        drawEndX = sprW/2 + sprScrX;
        if(drawEndX >= (signed)screenWidth) drawEndX = screenWidth - 1;

        for(int x = drawStartX; x < drawEndX; x++) {
            if(camSprPos.y > 0 && x > 0 && x < screenWidth && camSprPos.y < rayBuffer[x].dist) {
                texX = int(256*(x - I_drawStartX)*texRatio)/256;
                for(int y = drawStartY; y < drawEndY; y++) {
                    delt = (y)*256 - screenHeight*128 + sprH*128;
                    texY = ((delt * currTex->h)/sprH)/256;
                    scolor = texBuff[(currTex->pitch/sizeof(Uint32))*texY + texX];
                    // darkenPixelRGBA8888(&scolor, int(150/camSprPos.y)); // rude lighting
                    if(scolor & 0xFF) buffPix[(*buffPitch/sizeof(Uint32))*y + x] = scolor;
                }
            }
        }
    }
}