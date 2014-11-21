#ifndef _DRAW_H_
#define _DRAW_H_

#include "3dstypes.h"

#define FCRAM     (0x20000000)
#define FCRAM_END (0x28000000)
#define SCREEN_TOP_W  (400)
#define SCREEN_BOT_W  (340)
#define SCREEN_TOP_H  (240)
#define SCREEN_BOT_H  (240)

#define FB_TOP_LEFT1  (0x20184E60)
#define FB_TOP_LEFT2  (0x201CB370)
#define FB_TOP_RIGHT1 (0x20282160)
#define FB_TOP_RIGHT2 (0x202C8670)
#define FB_BOT_1      (0x202118E0)
#define FB_BOT_2      (0x20249CF0)

#define FB_TOP_SIZE   (0x46500)
#define FB_BOT_SIZE   (0x3BC40)

#define RED    0xFF0000
#define GREEN  0x00FF00
#define BLUE   0x0000FF
#define CYAN   0x00FFFF
#define YELLOW 0xFFFF00
#define BLACK  0x000000
#define WHITE  0xFFFFFF

extern int console_y;

void ClearScreen();
void draw_plot(int x, int y, u32 color);
void draw_fillrect(int x, int y, int w, int h, u32 color);
//Return last X position
int font_draw_char(int x, int y, u32 color, char c);
int font_draw_string(int x, int y, u32 color, const char *string);
void font_draw_stringf(int x, int y, u32 color, const char *s, ...);

void DEBUG(const char* str, ...); //Need this to cut down payload size.

#define ERROR(args...) font_draw_stringf(10, (console_y+=10)-10, RED, ## args)
/*#define DEBUG(args...) \
    do {  \
        draw_fillrect(0, console_y, SCREEN_TOP_W, 10, BLACK);  \
        font_draw_stringf(10, (console_y+=10)-10, WHITE, ## args);  \
        if (console_y >= (SCREEN_TOP_H-10)) { \
            console_y = 50;  \
            draw_fillrect(0, 50, SCREEN_TOP_W, (SCREEN_TOP_H-10-50), BLACK);  \
        } \
    } while (0)
*/



#endif
