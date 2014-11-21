#include "draw.h"
#include "libc.h"

int console_y = 10;
extern const u8 msx_font[];

static char buf[128] __attribute((aligned(8)));

//Had to make this a function instead of a macro because the payload was getting too large
void DEBUG(const char* str, ...) 
{
	va_list va;
	
	draw_fillrect(0, console_y, SCREEN_TOP_W, 10, BLACK);
	
	char *dest = buf;
	va_start(va, str);
	tfp_format(&dest, putcp, (char*)str, va);
	putcp(&dest, 0);
	va_end(va);
	font_draw_string(10, console_y-10, WHITE, buf);
	
	console_y+=10;
	if (console_y >= (SCREEN_TOP_H-10)) {
		console_y = 50;
		draw_fillrect(0, 50, SCREEN_TOP_W, (SCREEN_TOP_H-10-50), BLACK);
	}
}

void ClearScreen()
{
    int i;
    for (i = 0; i < FB_TOP_SIZE; i+=4) {
        *(int *)(FB_TOP_LEFT1 + i) = 0;
        *(int *)(FB_TOP_LEFT2 + i) = 0;
        *(int *)(FB_TOP_RIGHT1 + i) = 0;
        *(int *)(FB_TOP_RIGHT2 + i) = 0;
    }
    for (i = 0; i < FB_BOT_SIZE; i+=4) {
        *(int *)(FB_BOT_1 + i) = 0;
        *(int *)(FB_BOT_2 + i) = 0;
    }
}

void draw_plot(int x, int y, u32 color)
{
    u8 *base = (u8*)((SCREEN_TOP_H-y-1)*3 +x*3*SCREEN_TOP_H);
    u8 *p1 = base + FB_TOP_LEFT1;
    u8 *p2 = base + FB_TOP_LEFT2;
    p1[0] = p2[0] = color & 0xFF;
    p1[1] = p2[1] = (color>>8) & 0xFF;
    p1[2] = p2[2] = (color>>16) & 0xFF;
}

void draw_fillrect(int x, int y, int w, int h, u32 color)
{
    int i, j;
    for (i = 0; i < w; ++i) {
        for (j = 0; j < h; ++j) {
            draw_plot(x+i, y+j, color);
        }
    }
}

int font_draw_char(int x, int y, u32 color, char c)
{
    u8 *font = (u8*)(msx_font + (c - (u32)' ') * 8);
    int i, j;
    for (i = 0; i < 8; ++i) {
        for (j = 0; j < 8; ++j) {
            if ((*font & (128 >> j))) draw_plot(x+j, y+i, color);
        }
        ++font;
    }
    return x+8;
}

int font_draw_string(int x, int y, u32 color, const char *string)
{
    if (string == NULL) return x;
    int startx = x;
    const char *s = string;
    while (*s) {
        if (*s == '\n') {
            x = startx;
            y+=8;
        } else if (*s == ' ') {
            x+=8;
        } else if(*s == '\t') {
            x+=8*4;
        } else {
            font_draw_char(x, y, color, *s);
            x+=8;
        }
        ++s;
    }
    return x;
}


void font_draw_stringf(int x, int y, u32 color, const char *s, ...)
{
    char *dest = buf;
    va_list va;
    va_start(va, s);
    tfp_format(&dest, putcp, (char*)s, va);
    putcp(&dest, 0);
    va_end(va);
    font_draw_string(x, y, color, buf);
}
