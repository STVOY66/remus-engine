#include "raylib.h"
#include "player.h"
#include <string>
#include <iostream>

//constant values
const unsigned winWidth = 800;
const unsigned winHeight = 600;
const unsigned FPS = 60;

float hypotTest;
std::string testString;


//function headers
void draw(Color);
void update();

int main() {
    SetTargetFPS(60);
    InitWindow(winWidth, winHeight, "Remus Engine");

    std::cout << "Beginning game-loop..." << std::endl;
    while(!WindowShouldClose()) {
        draw(BLACK);
        update();
    }

    CloseWindow();
}

// Handles all draw functions
void draw(Color bgColor) {
    BeginDrawing();
        ClearBackground(bgColor);
    EndDrawing();
}

// Handles all update functions between frames
void update() {

}