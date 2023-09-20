#include <stdint.h>
#include <libc.hpp>
#include <common.hpp>

int x, y, fx, fy; // psf

uint32_t fg = 0xffffff;
uint32_t bg = 0;

void putchar(
    /* note that this is int, not char as it's a unicode character */
    unsigned short int c,
    /* cursor position on screen, in characters not in pixels */
    int cx, int cy,
    /* foreground and background colors, say 0xFFFFFF and 0x000000 */
    uint32_t fg, uint32_t bg);
void dbgputchar(char c);
void putc(char c) {
    if (c == '\n') {
        y+=14;
        x = 0;
        return;
    }
    putchar(c, x, y, fg, bg);
    x+=8;
    if (x > 1024) {
        x = 0;
        y+=14;
    } if (y > 768) {
        memset((void *)mbi->framebuffer_addr, 0, mbi->framebuffer_pitch*mbi->framebuffer_height);
        y = 0;
        x = 0;
    }
    if (c == '\n') dbgputchar('\r');
    dbgputchar(c);
}