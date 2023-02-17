#include <ti/screen.h>
#include <ti/getcsc.h>
#include <graphx.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <c++/cstdint>
#include <c++/cmath>

#include "texture.hpp"

#include "texture_data.h"

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

std::uint8_t check(std::uint8_t o1, std::uint8_t o2, std::uint8_t o3) {
    if (o1 == 1 && o2 == 1 && o3 == 1) {
        return 1;
    } else if (o1 == 2 && o2 == 2 && o3 == 2) {
        return 2;
    }
    return 0;

}

std::uint8_t checkWin(std::uint8_t b[9]) {
    int w = 0;

    if (w = check(b[0], b[1], b[2])) return w;
    if (w = check(b[3], b[4], b[5])) return w;
    if (w = check(b[6], b[7], b[8])) return w;

    if (w = check(b[0], b[3], b[6])) return w;
    if (w = check(b[1], b[4], b[7])) return w;
    if (w = check(b[2], b[5], b[8])) return w;

    if (w = check(b[0], b[4], b[8])) return w;
    if (w = check(b[2], b[4], b[6])) return w;

    return w;
} 

int main(void) {
    os_ClrHome();

    gfx_Begin();
    gfx_SetDrawBuffer();

    bool running = true;

    int selectX = 0;
    int selectY = 0;

    // U.I. Scaling
    int sF = 10;
    int cellPadding = 5;

    // 3 * sF gets the width of one cell - cellpadding * 2  makes it so that
    // there is cell padding on all sides
    int cellTextSz = 3*sF - cellPadding*2;

    // Scaled texture    
    texture_t XMarker = allocTexture(cellTextSz, cellTextSz);
    texture_t OMarker = allocTexture(cellTextSz, cellTextSz);
    texture_t tttGrid = allocTexture(t_tgrid.width*sF, t_tgrid.height*sF);
    texture_t selectMarker = allocTexture(cellTextSz, cellTextSz);

    // Scale texture to required size
    scaleTexture(&t_xspr, &XMarker);
    scaleTexture(&t_ospr, &OMarker);
    scaleTexture(&t_tgrid, &tttGrid);
    scaleTexture(&t_select, &selectMarker);

    // Tetris grid
    std::uint8_t grid[9] = {
        0, 0, 0,
        0, 0, 0,
        0, 0, 0
    };

    // Current person playing
    bool currentPerson = false;

    long ticker = 0;
    while(running) {
        // draw_rect(px, py, 25, 25, 150);
        blitTexture(0, 0, &tttGrid);

        // Draw the tic tac toe grid
        for (int y=0; y<3; y++) {
            for (int x=0; x<3; x++) {
                std::uint8_t a = grid[y*3 + x];
                if (a == 0) continue;
                else if (a == 1) drawMark(x, y, &XMarker, sF, cellPadding);
                else if (a == 2) drawMark(x, y, &OMarker, sF, cellPadding);
            }
        }

        // Determine the texture for the current player
        texture_t* playerTexture = !currentPerson ? &XMarker : &OMarker;

        // Display current player next to grid
        drawMark(3, 0, playerTexture, sF, cellPadding);

        // Flash the select marker so you can see under it
        if ((ticker & 0b10) == 0b10)  
            drawMark(selectX, selectY, &selectMarker, sF, cellPadding);

        // Check for wins and display if there is
        std::uint8_t w = checkWin(grid);
        if (w == 1) {
            drawMark(4, 0, &XMarker, sF, cellPadding);
        } else if (w == 2) {
            drawMark(4, 0, &OMarker, sF, cellPadding);
        }

        // Get key input
        std::uint8_t ksc = os_GetCSC();

        if (ksc == 15) running = false;
        else if (ksc == 1) selectY = min(selectY + 1, 2);
        else if (ksc == 2) selectX = max(selectX - 1, 0);
        else if (ksc == 3) selectX = min(selectX + 1, 2);
        else if (ksc == 4) selectY = max(selectY - 1, 0);
        else if (ksc == 9) {
            // If click then we place
            int index = selectY * 3 + selectX;

            // If the grid spot is empty and no one has won
            if (grid[index] == 0 && w == 0) {
                grid[index] = !currentPerson ? 1 : 2;
                currentPerson = !currentPerson;
            } else if (w > 0) {
                // If someone has won and presses enter it clears the board
                memset(&grid[0], 0, 9);
                currentPerson = false;
            }
        }
        
        gfx_SwapDraw();

        // Clears the scren
        memset(gfx_vbuffer, 0, GFX_LCD_WIDTH * GFX_LCD_HEIGHT);

        ticker ++;
    }

    gfx_End();

    printf("%d Bytes used for textures\n", getNumTextBytesAllocd());

    // Clean up after the program ends
    destroyTexture(&XMarker);
    destroyTexture(&OMarker);
    destroyTexture(&tttGrid);
    destroyTexture(&selectMarker);

    while (!os_GetCSC());
    

    // Down 1
    // Left 2
    // Right 3
    // Up 4
    // Enter 9

    return 0;
}