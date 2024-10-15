#include "SDL2/SDL.h"
#include "sdl2utils.h"
#include "SDL2/SDL_image.h"
#include <filesystem>
#include <iostream>
#include <string>
#include <map>

namespace fs = std::filesystem;

float fVector2Length(Vector2f vector) {
    return SDL_sqrtf((vector.x * vector.x) + (vector.y * vector.y));
}

Vector2f fVector2Normalize(Vector2f vector) {
    float len = fVector2Length(vector);
    return Vector2f{vector.x/len, vector.y/len};
}

Vector2f fVector2Rotate(Vector2f vector, float amt) {
    float sinAmt = SDL_sinf(amt);
    float cosAmt = SDL_cosf(amt);
    Vector2f output = {(vector.x*cosAmt) - (vector.y*sinAmt), (vector.x*sinAmt) + (vector.y*cosAmt)};
    return output;
}

TexCache::TexCache(SDL_Renderer* renderer) {
    this->programRender = renderer;
    this->flags = IMG_INIT_JPG;
    flag2str();
}

TexCache::TexCache(SDL_Renderer* renderer, int flags) {
    this->programRender = renderer;
    this->flags = (flags > 63) ? IMG_INIT_JPG : flags;
    flag2str();
}

TexCache::TexCache(SDL_Renderer* renderer, std::string path, int flags) {
    this->programRender = renderer;
    this->flags = (flags > 63) ? IMG_INIT_JPG : flags;
    flag2str();

    fs::path workPath = path;

    if(fs::exists(workPath)) {
        if(fs::is_directory(workPath)) loadDir(workPath);
        else {
            bool supported = false;
            std::string pathExt = workPath.extension().string();

            for(std::string ext : filetypes) {
                if(pathExt == ext) {
                    supported = true;
                    break;
                }
            }

            loadImage(workPath);
        }
    }
}

TexCache::~TexCache() {
    flush();
}

void TexCache::loadImage(fs::path workPath) {
    bool supported = false;
    std::string pathExt = workPath.extension().string();
    if(cache.find(workPath.filename().string()) == cache.end()){
        std::cout << "Loading " << workPath << " into cache..." << std::endl;
        for(std::string ext : filetypes) {
            if(pathExt == ext) { supported = true; break; }
        }

        if(supported) {
            cache.insert({workPath.filename().string(), IMG_LoadTexture(programRender, workPath.string().c_str())});
            std::cout << workPath << " loaded successfully." << std::endl;
        } else {
            std::cout << "ERROR: Filetype \"" << pathExt << "\" not supported." << std::endl;
        }
    }
}

void TexCache::loadDir(fs::path workPath) {
    std::cout << "Loading directory \"" << workPath.string() << "\" into image cache..." << std::endl;
    for( fs::directory_entry entry : fs::directory_iterator(workPath) ) {
        if(fs::is_directory(entry)) loadDir(entry.path());
        else loadImage(entry.path());
    }
}

void TexCache::flag2str() {
    if(flags & IMG_INIT_JPG) { filetypes.push_back(".jpg"); filetypes.push_back(".jpeg"); }
    if(flags & IMG_INIT_PNG) { filetypes.push_back(".png"); }
    if(flags & IMG_INIT_TIF) { filetypes.push_back(".tif"); filetypes.push_back(".tiff");}
    if(flags & IMG_INIT_WEBP) { filetypes.push_back(".webp"); }
    if(flags & IMG_INIT_JXL) { filetypes.push_back(".jxl"); }
    if(flags & IMG_INIT_AVIF) { filetypes.push_back(".avif"); filetypes.push_back(".avifs"); }
}

int TexCache::getWidth(int index) {
    return getWidth(atIndex(index).first);
}

int TexCache::getWidth(std::string key) {
    int output;
    SDL_QueryTexture(cache.at(key), NULL, NULL, &output, NULL);
    return output;
}

int TexCache::getHeight(int index) {
    return getHeight(atIndex(index).first);
}

int TexCache::getHeight(std::string key) {
    int output;
    SDL_QueryTexture(cache.at(key), NULL, NULL, NULL, &output);
    return output;
}

Vector2i TexCache::getDim(int index) {
    return getDim(atIndex(index).first);
}

Vector2i TexCache::getDim(std::string key) {
    Vector2i output;
    SDL_QueryTexture(cache.at(key), NULL, NULL, &output.x, &output.y);
    return output;
}

std::pair<std::string, SDL_Texture*> TexCache::atIndex(int index) {
    auto it = cache.begin();
    std::advance(it, index);
    return *it;
}

void TexCache::flush() {
    std::cout << "Flushing image cache..." << std::endl;
    std::map<std::string, SDL_Texture*>::iterator i = cache.begin();
    for(;i != cache.end(); i++)
        SDL_DestroyTexture(i->second);
    cache.clear();
}