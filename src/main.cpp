#include <ti/screen.h>
#include <ti/getcsc.h>
#include <graphx.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <c++/cstdint>
#include <c++/cmath>

#include "xspr.h"

template <typename T>
T max(T o1, T o2) {
    if (o1 > o2) return o1;
    return o2;
}

template <typename T>
T min(T o1, T o2) {
    if (o1 < o2) return o1;
    return o2;
}


struct texture_t {
    int width;
    int height;
    std::uint8_t* data;
    bool needsFreeing;
};

static int textureBytesAlloc = 0;

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

void draw_rect(int x, int y, int sx, int sy, std::uint8_t v) {
    for (int j=max(0, y); j<min(y+sy, GFX_LCD_HEIGHT); j++) {
        for (int i=max(0, x); i<min(x+sx, GFX_LCD_WIDTH); i++) {
            gfx_vbuffer[j][i] = v;
        }
    }
}

void drawMark(int x, int y, const texture_t* t, int sF, int cellPad) {
    int textSz = 3*sF - cellPad*2;
    blitTexture(cellPad + (textSz + cellPad*2 + sF)*x, cellPad + (textSz + cellPad*2 + sF)*y, t);
}

int main(void) {
    os_ClrHome();

    gfx_Begin();
    gfx_SetDrawBuffer();

    bool running = true;

    int px = 0;
    int py = 0;


    texture_t xsprOG = wrapArrayTexture(d_xspr, XsprWIDTH, XsprHEIGHT);
    texture_t osprOG = wrapArrayTexture(d_ospr, OsprWIDTH, OsprHEIGHT);
    texture_t tgridOG = wrapArrayTexture(d_tgrid, tgridWIDTH, tgridHEIGHT);
    texture_t tselectOG = wrapArrayTexture(d_tselect, tselectWIDTH, tselectHEIGHT);

    int sF = 10;
    int cellPadding = 5;
    int cellTextSz = 3*sF - cellPadding*2;
    
    texture_t scaledX = allocTexture(cellTextSz, cellTextSz);
    texture_t scaledO = allocTexture(cellTextSz, cellTextSz);
    texture_t scaledGrid = allocTexture(tgridWIDTH*sF, tgridHEIGHT*sF);
    texture_t scaledSelect = allocTexture(cellTextSz, cellTextSz);

    scaleTexture(&xsprOG, &scaledX);
    scaleTexture(&osprOG, &scaledO);
    scaleTexture(&tgridOG, &scaledGrid);
    scaleTexture(&tselectOG, &scaledSelect);

    std::uint8_t grid[9] = {
        0, 0, 0,
        0, 0, 0,
        0, 0, 0
    };

    bool currentPerson = false;


    while(running) {
        // draw_rect(px, py, 25, 25, 150);
        blitTexture(0, 0, &scaledGrid);

        for (int y=0; y<3; y++) {
            for (int x=0; x<3; x++) {
                std::uint8_t a = grid[y*3 + x];
                if (a == 0) continue;
                else if (a == 1) drawMark(x, y, &scaledX, sF, cellPadding);
                else if (a == 2) drawMark(x, y, &scaledO, sF, cellPadding);
            }
        }

        texture_t* playerTexture = !currentPerson ? &scaledX : &scaledO;
        drawMark(px, py, &scaledSelect, sF, cellPadding);
        drawMark(3, 0, playerTexture, sF, cellPadding);

        std::uint8_t ksc = os_GetCSC();

        if (ksc == 15) running = false;
        else if (ksc == 1) py = min(py + 1, 2);
        else if (ksc == 2) px = max(px - 1, 0);
        else if (ksc == 3) px = min(px + 1, 2);
        else if (ksc == 4) py = max(py - 1, 0);
        else if (ksc == 9) {
            // If click then we place
            int index = py * 3 + px;
            grid[index] = !currentPerson ? 1 : 2;
            currentPerson = !currentPerson;
        }
        
        gfx_SwapDraw();
        memset(gfx_vbuffer, 0, GFX_LCD_WIDTH * GFX_LCD_HEIGHT);
    }

    gfx_End();

    printf("%d Bytes used for textures\n", textureBytesAlloc);

    destroyTexture(&scaledX);
    destroyTexture(&scaledO);
    destroyTexture(&scaledGrid);

    // Down 1
    // Left 2
    // Right 3
    // Up 4
    // Enter 9

    return 0;
}