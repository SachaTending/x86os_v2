#include <multiboot.h>
#include <libc.hpp>
#include <common.hpp>
#include <krnl.hpp>


static int *screen = (int *)mbi->framebuffer_addr;

void putpixel(int x, int y, uint32_t color) {
    size_t where = y + x * (mbi->framebuffer_pitch / sizeof(uint32_t));
    #define SET(off) screen[where + off] = color;
    for (int i=0;i<1;i++) SET(i);
}
static inline void outb(uint16_t port, uint8_t val)
{
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) :"memory");
    /* There's an outb %al, $imm8  encoding, for compile-time constant port numbers that fit in 8b.  (N constraint).
     * Wider immediate constants would be truncated at assemble-time (e.g. "i" constraint).
     * The  outb  %al, %dx  encoding is the only option for all other cases.
     * %1 expands to %dx because  port  is a uint16_t.  %w1 could be used if we had the port number a wider C type */
}

void dbgputchar(char c) {
    outb(0xe9, c);
}
void putchar(
    /* note that this is int, not char as it's a unicode character */
    unsigned short int c,
    /* cursor position on screen, in characters not in pixels */
    int cx, int cy,
    /* foreground and background colors, say 0xFFFFFF and 0x000000 */
    uint32_t fg, uint32_t bg);

int x, y;
int fx, fy;
int scanline;

void psf_putchar(char c) {
    if (c == '\n') {
        y+=fy;
        x = 0;
        return;
    }
    putchar(c, x, y, 0xffffff, 0x0);
    x+=fx;
}

void printdbg(const char *txt) {
    while (*txt) {
        dbgputchar(*txt);
        psf_putchar(*txt);
        txt++;
    }
}

multiboot_info_t *mbi;

#define s 500
void psf_init();
extern "C" void init_main(multiboot_info_t *mbi2) {
    mbi = mbi2;
    screen = (int *)mbi->framebuffer_addr;
    Kernel::pmm_setup();
    scanline = mbi->framebuffer_pitch;
    psf_init();
    printdbg("psf done\n");
}