#include "drawing.hpp"
#include "utils.hpp"

void draw_rect(int x, int y, int sx, int sy, std::uint8_t v) {
    for (int j=max(0, y); j<min(y+sy, GFX_LCD_HEIGHT); j++) {
        for (int i=max(0, x); i<min(x+sx, GFX_LCD_WIDTH); i++) {
            gfx_vbuffer[j][i] = v;
        }
    }
}