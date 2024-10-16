#pragma once
#include "SDL2/SDL.h"
#include <filesystem>
#include <vector>
#include <string>
#include <map>

namespace fs = std::filesystem;

// ******** SDL color constants ********
#define BLACK SDL_Color{0, 0, 0, 255}
#define GRAY SDL_Color{125, 125, 125, 255};
#define WHITE SDL_Color{255, 255, 255, 255};
#define RED SDL_Color{255, 0, 0, 255};
#define DARK_RED SDL_Color{125, 0, 0, 255};
#define GREEN SDL_Color{0, 255, 0, 255};
#define DARK_GREEN SDL_Color{0, 125, 0, 255};
#define BLUE SDL_Color{0, 0, 255, 255};
#define DARK_BLUE SDL_Color{0, 0, 125, 255};

// ******** Vector2 utilities ********

// 2D Vector using floats
struct Vector2f {
    float x;
    float y;
};

struct Vector2i {
    int x;
    int y;
};

// Returns length of 2D Vector
float fVector2Length(Vector2f);

// Normalizes 2D Vector
Vector2f fVector2Normalize(Vector2f);

// Rotates vector by radians
Vector2f fVector2Rotate(Vector2f, float);

// Retrieve pixel data from position
Uint32 getPixelData(Uint32*, int, int, int);

Uint32 darkenPixelRGBA8888(Uint32, float);

// Class for texture storage
class TexCache {
    public:
        std::map<std::string, SDL_Texture*> cache;

        TexCache(SDL_Renderer*);
        TexCache(SDL_Renderer*, int);
        TexCache(SDL_Renderer*, std::string, int);
        ~TexCache();

        void loadDir(fs::path);
        void loadImage(fs::path);
        int getWidth(std::string);
        int getWidth(int);
        int getHeight(std::string);
        int getHeight(int);
        Vector2i getDim(int);
        Vector2i getDim(std::string);
        std::pair<std::string, SDL_Texture*> atIndex(int);
        void flush();
    private:
        std::vector<std::string> filetypes;
        unsigned int flags;
        SDL_Renderer* programRender;

        void flag2str();
};

// Class for image storage
class ImgCache {
    public:
        std::map<std::string, SDL_Surface*> cache;

        ImgCache();
        ImgCache(int);
        ImgCache(std::string, int);
        ~ImgCache();

        void loadDir(fs::path);
        void loadImage(fs::path);
        int getWidth(std::string);
        int getWidth(int);
        int getHeight(std::string);
        int getHeight(int);
        Vector2i getDim(int);
        Vector2i getDim(std::string);
        std::pair<std::string, SDL_Surface*> atIndex(int);
        void flush();
    private:
        std::vector<std::string> filetypes;
        unsigned int flags;

        void flag2str();
};