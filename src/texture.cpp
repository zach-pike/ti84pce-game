#include "texture.hpp"
#include <stdlib.h>
#include <graphx.h>
#include <c++/cmath>

static int textureBytesAlloc = 0;

template <typename T>
static T max(T o1, T o2) {
    if (o1 > o2) return o1;
    return o2;
}

template <typename T>
static T min(T o1, T o2) {
    if (o1 < o2) return o1;
    return o2;
}

texture_t allocTexture(int w, int h) {
    texture_t t;

    t.width = w;
    t.height = h;
    t.data = (std::uint8_t*) malloc(w*h);
    t.needsFreeing = true;

    textureBytesAlloc += w * h;

    return t;
}

texture_t wrapArrayTexture(std::uint8_t* d, int w, int h) {
    texture_t t;

    t.width = w;
    t.height = h;
    t.data = d;
    t.needsFreeing = false;

    return t;
}

void blitTexture(int px, int py, const texture_t* t) {
    int cy = py > 0 ? 0 : 0 - py;
    for(int y=max(0, py); y < min(py + t->height, GFX_LCD_HEIGHT); y++) {
        int cx = px > 0 ? 0 : 0 - px;
        for(int x=max(0, px); x < min(px + t->width, GFX_LCD_WIDTH); x++) {
            gfx_vbuffer[y][x] = t->data[cy * t->width + cx];
            cx++;
        }
        cy++;
    }
}

void scaleTexture(const texture_t* in, texture_t* out) {
    for (int y=0; y < out->height; y++) {
        for (int x=0; x < out->width; x++) {
            double normX = (double) x / (double) out->width;
            double normY = (double) y / (double) out->width;

            normX *= in->width;
            normY *= in->height;

            int fx = std::floor(normX);
            int fy = std::floor(normY);

            out->data[y * out->width + x] = in->data[fy * in->width + fx];
        }
    }
}

void destroyTexture(texture_t* data) {
    if (data->needsFreeing == false) return;
    free(data->data);

    textureBytesAlloc -= data->width * data->height;

    data->data = nullptr;
}

int getNumTextBytesAllocd() {
    return textureBytesAlloc;
}