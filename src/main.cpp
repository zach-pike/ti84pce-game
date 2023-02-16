#include <ti/screen.h>
#include <ti/getcsc.h>
#include <graphx.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <c++/cstdint>
#include <c++/cmath>
#include <c++/initializer_list>

// template <typename T>
// T max(std::initializer_list<T> data) {
//     T max = data.initializer_list
// }

void draw_rect(int x, int y, int sx, int sy, std::uint8_t v) {
    
    for (int i=x; i<x+sx; i++) {
        for (int j=y; j<y+sy; j++) {
            gfx_vbuffer[j][i] = v;
        }
    }
}

void draw_circle(int x, int y, int r, std::uint8_t c) {
    for (int i=0; i<r*2; i++) {
        for (int j=0; j<r*2; j++) {
            double mx = i - r;
            double my = j - r;

            double d = std::sqrt(mx*mx + my*my);

            if (d < (double)r) {
                gfx_vbuffer[y+j][x+i] = c;
            }

        }
    }
}

int main(void) {
    os_ClrHome();

    gfx_Begin();
    gfx_SetDrawBuffer();

    bool running = true;

    int px = 0;
    int py = 0;

    while(running) {
        draw_rect(px, py, 25, 25, 150);

        std::uint8_t ksc = os_GetCSC();

        if (ksc == 15) running = false;
        else if (ksc == 1) py += 25;
        else if (ksc == 2) px -= 25;
        else if (ksc == 3) px += 25;
        else if (ksc == 4) py -= 25;
        
        gfx_SwapDraw();
        memset(gfx_vbuffer, 0, GFX_LCD_WIDTH * GFX_LCD_HEIGHT);
    }

    gfx_End();

    // Down 1
    // Left 2
    // Right 3
    // Up 4

    return 0;
}