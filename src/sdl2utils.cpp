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

ImgCache::ImgCache() {
    this->flags = IMG_INIT_JPG;
    flag2str();
}

ImgCache::ImgCache(int flags) {
    this->flags = (flags > 63) ? IMG_INIT_JPG : flags;
    flag2str();
}

ImgCache::ImgCache(std::string path, int flags) {
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

ImgCache::~ImgCache() {
    flush();
}

void ImgCache::loadImage(fs::path workPath) {
    bool supported = false;
    std::string pathExt = workPath.extension().string();
    if(cache.find(workPath.filename().string()) == cache.end()){
        for(std::string ext : filetypes) {
            if(pathExt == ext) { supported = true; break; }
        }

        if(supported) {
            cache.insert({workPath.filename().string(), IMG_Load(workPath.string().c_str())});
            std::cout << workPath.string() << " loaded successfully." << std::endl;
        } else {
            std::cout << "ERROR: Filetype \"" << pathExt << "\" not supported." << std::endl;
        }
    }
}

void ImgCache::loadDir(fs::path workPath) {
    std::cout << "Loading directory \"" << workPath.string() << "\" into image cache..." << std::endl;
    for( fs::directory_entry file : fs::directory_iterator(workPath) ) {
        loadImage(file.path());
    }
}

void ImgCache::flag2str() {
    if(flags & IMG_INIT_JPG) { filetypes.push_back(".jpg"); filetypes.push_back(".jpeg"); }
    if(flags & IMG_INIT_PNG) { filetypes.push_back(".png"); }
    if(flags & IMG_INIT_TIF) { filetypes.push_back(".tif"); filetypes.push_back(".tiff");}
    if(flags & IMG_INIT_WEBP) { filetypes.push_back(".webp"); }
    if(flags & IMG_INIT_JXL) { filetypes.push_back(".jxl"); }
    if(flags & IMG_INIT_AVIF) { filetypes.push_back(".avif"); filetypes.push_back(".avifs"); }
}

void ImgCache::flush() {
    std::map<std::string, SDL_Surface*>::iterator i = cache.begin();
    for(;i != cache.end(); i++)
        SDL_FreeSurface(i->second);
    cache.clear();
}