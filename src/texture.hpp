#pragma once

#include <c++/cstdint>


struct texture_t {
    int width;
    int height;
    std::uint8_t* data;
    bool needsFreeing;
};

static int textureBytesAlloc = 0;

texture_t allocTexture(int w, int h);

texture_t wrapArrayTexture(std::uint8_t* d, int w, int h);

void blitTexture(int px, int py, const texture_t* t);

void scaleTexture(const texture_t* in, texture_t* out);

void destroyTexture(texture_t* data);